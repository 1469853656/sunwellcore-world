#include "ChatProcessor.hpp"
#include "Language.h"
#include "World.h"
#include "Duration.h"
#include "DatabaseEnv.h"
#include "Profiler.h"
#include "fmt/format.h"

ChatProcessor::~ChatProcessor()
{
    Shutdown();
}

void ChatProcessor::Initialize(size_t threadsCount)
{
    if (threadsCount == m_threads.size())
        return;

    Shutdown();

    m_messages.open();

    m_threads.resize(threadsCount);
    for (auto& context : m_threads)
    {
        context                  = std::make_unique<ThreadContext>();
        context->m_reloadFilters = true;

        context->m_thread = std::thread([context = context.get(), this] 
        {
            while (!m_messages.is_closed())
            {
                auto message = m_messages.pop();
                if (!message)
                    continue;

                if (context->m_reloadFilters)
                {
                    std::lock_guard lock(m_filtersMutex);
                    context->m_filters       = m_filters;
                    context->m_reloadFilters = false;
                }

                try
                {
                    using namespace boost::regex_constants;
                    for (ChatFilter& filter : context->m_filters)
                    {
                        if (!regex_search(message->m_message.begin(), message->m_message.end(), filter.m_regex, match_any | match_perl))
                            continue;

                        std::lock_guard lock(m_failedQueueMutex);
                        m_failedQueue.push_back(FailedCheck{ filter.m_filterId, filter.m_reportCount, std::move(*message) });
                    }
                }
                catch (std::exception& e)
                {
                    sLog->outError("%s", fmt::format("Error during ChatProcessorMgr::ProcessMessage (async) regex search: {}", e.what()).c_str());
                }
            }
        });
    }
}

void ChatProcessor::Shutdown()
{
    m_messages.close();

    for (auto& context : m_threads)
    {
        if (context && context->m_thread.joinable())
            context->m_thread.join();
    }

    m_threads.clear();
}

void ChatProcessor::Update(uint32_t diff)
{
    if (m_messages.is_closed())
        return;

    PROFILE_SCOPE("ChatProcessor");

    size_t cooldown  = sWorld->getIntConfig(CONFIG_CHAT_PROCESSOR_REPORT_COOLDOWN);
    auto currentTime = sWorld->GetGameTimeMS();

    std::lock_guard lock(m_failedQueueMutex);
    while (!m_failedQueue.empty())
    {
        FailedCheck check = std::move(m_failedQueue.back());
        m_failedQueue.pop_back();

        AccountFails& accountFails = m_failedChecks[check.m_message.m_accountId];

        auto& failedCount = accountFails.m_failedChecks[check.m_filterId];
        if (++failedCount < check.m_reportCount)
            continue;

        auto timeout = accountFails.m_reportTime + cooldown;
        if (timeout > currentTime)
            continue;

        accountFails.m_reportTime = sWorld->GetGameTimeMS();
        accountFails.m_failedChecks.erase(check.m_filterId);

        sWorld->SendGMText(LANG_POSSIBLE_GOLD_SELLER, check.m_message.m_playerName.c_str(), check.m_message.m_message.c_str());
        sLog->outChinaTown("Player: %s (Account: %u) has send a message: %s", check.m_message.m_playerName.c_str(), check.m_message.m_accountId, check.m_message.m_message.c_str());
    }
}

void ChatProcessor::LoadChatProcessorFilters()
{
    sLog->outString(">> Loading `chat_processor_filters` strings ...");

    uint32 oldMSTime = getMSTime();

    std::lock_guard lock(m_filtersMutex);
    m_filters.clear();

    auto* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_CHAT_PROCESSOR_FILTERS);

    auto result = WorldDatabase.Query(stmt);
    if (result)
    {
        do
        {
            Field* field = result->Fetch();

            ChatFilter filter;
            filter.m_filterId = field[0].GetUInt32();
            std::string regexString = field[1].GetString();
            try
            {
                filter.m_regex = boost::regex(regexString, boost::regex::icase);
            }
            catch (std::exception& e)
            {
                sLog->outError("%s", fmt::format("Failed to compile regex pattern '{}' (FilterID: {}, ErrorMessage: {}), skipping...", regexString, filter.m_filterId, e.what()).c_str());
                continue;
            }

            filter.m_filterType  = field[2].GetString();
            filter.m_reportCount = field[3].GetUInt32();

            m_filters.push_back(std::move(filter));

        } while (result->NextRow());
    }

    for (auto& context : m_threads)
        context->m_reloadFilters = true;

    sLog->outString(">> Loaded `chat_processor_filters` strings (count: " SIZEFMTD ") in %u ms", m_filters.size(), GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

void ChatProcessor::ProcessMessageAsync(const std::string& playerName, uint32_t accountId, const std::string& message)
{
    if (m_messages.is_closed())
        return;

    m_messages.push_back({ playerName, accountId, message });
}

bool ChatProcessor::ProcessMessage(std::string const& message)
{
    std::lock_guard lock(m_filtersMutex);

    try
    {
        using namespace boost::regex_constants;
        return std::ranges::none_of(m_filters, [message](ChatFilter const& chatFilter) { return regex_search(message.begin(), message.end(), chatFilter.m_regex, match_any | match_perl); });
    }
    catch (std::exception& e)
    {
        sLog->outError("%s", fmt::format("Error during ChatProcessorMgr::ProcessMessage regex search: {}", e.what()).c_str());
        return true;
    }
}

ChatProcessor& GetChatProcessor()
{
    static ChatProcessor s_chatProcessor;
    return s_chatProcessor;
}
