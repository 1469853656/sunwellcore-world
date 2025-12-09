#pragma once

#include "Threading/SyncQueue.hpp"
#include "Regex.h"

#include <vector>
#include <thread>
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>

struct ChatFilter
{
    uint32_t m_filterId;
    std::string m_filterType;
    size_t m_reportCount = 0;
    Trinity::Regex m_regex;
};

struct ChatMessage
{
    std::string m_playerName;
    uint32_t m_accountId;
    std::string m_message;
};

class ChatProcessor
{
public:
    ~ChatProcessor();

    void Initialize(size_t threadsCount);
    void Shutdown();

    void Update(uint32_t diff);

    void LoadChatProcessorFilters();

    void ProcessMessageAsync(const std::string& playerName, uint32_t accountId, const std::string& message);
    //! Synchronous call
    bool ProcessMessage(std::string const& message);

protected:
    Threading::SyncQueue<ChatMessage> m_messages;

    std::mutex m_filtersMutex;
    std::vector<ChatFilter> m_filters;

    struct FailedCheck
    {
        uint32_t m_filterId;
        size_t m_reportCount;
        ChatMessage m_message;
    };

    std::mutex m_failedQueueMutex;
    std::deque<FailedCheck> m_failedQueue;

    using FailedChatFilterMap = std::unordered_map<uint32_t, uint32_t>;

    struct AccountFails
    {
        uint32_t m_reportTime;
        FailedChatFilterMap m_failedChecks;
    };

    std::unordered_map<uint32_t, AccountFails> m_failedChecks;

    struct ThreadContext
    {
        std::thread m_thread;

        std::vector<ChatFilter> m_filters;
        std::atomic_bool m_reloadFilters;
    };

    std::vector<std::unique_ptr<ThreadContext>> m_threads;
};

ChatProcessor& GetChatProcessor();
