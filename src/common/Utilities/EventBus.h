#pragma once

#include <mutex>
#include <vector>
#include <unordered_map>
#include <memory>

class EventBus
{
public:
    template<typename Event, typename Class>
    void RegisterHandler(Class* instance, void (Class::*handler)(const Event&))
    {
        auto storage = GetOrCreateEventStorage<Event>();
        storage->RegisterHandler([instance, handler](void const* ptr) {
            auto ev = *static_cast<const Event*>(ptr);
            (instance->*handler)(ev);
        });
    }

    template<typename Event, typename... Args>
    void QueueEvent(Args&&... args)
    {
        std::lock_guard lock(m_queueMutex);

        auto storage = GetOrCreateEventStorage<Event>();

        Event ev{ std::forward<Args>(args)... };
        auto index = storage->QueueEvent(&ev);

        m_eventQueue.push_back(QueuedEvent{ GetEventHash<Event>(), index });
    }

    void DispatchEvents()
    {
        std::lock_guard lock(m_queueMutex);
        while (!m_eventQueue.empty())
        {
            auto ev = m_eventQueue.front();
            m_eventQueue.erase(m_eventQueue.begin());

            auto storage = GetEventStorage(ev.m_hash);
            storage->DispatchEvent(ev.m_index);
        }

        for (auto&& it : m_eventStorages)
        {
            it.second->ClearEvents();
        }

        m_eventQueue.clear();
    }

private:
    template<typename Event>
    size_t GetEventHash() const
    {
        return typeid(Event).hash_code();
    }

    class IEventStorage
    {
    public:
        virtual ~IEventStorage() = default;

        using EventHandler = std::function<void(void const*)>;

        virtual void RegisterHandler(EventHandler&&)   = 0;
        virtual void DispatchEvent(size_t index) const = 0;
        virtual size_t QueueEvent(void* ev)            = 0;
        virtual void ClearEvents()                     = 0;
    };

    template<typename Event>
    class EventStorage : public IEventStorage
    {
    public:
        void RegisterHandler(EventHandler&& handler) override
        {
            m_handlers.push_back(std::move(handler));
        }

        void DispatchEvent(size_t index) const override
        {
            ASSERT(index < m_storage.size());
            for (auto&& handler : m_handlers)
                handler(&m_storage[index]);
        }

        size_t QueueEvent(void* ev) override
        {
            size_t index = m_storage.size();
            m_storage.push_back(std::move(*static_cast<Event*>(ev)));

            return index;
        }

        void ClearEvents() override
        {
            m_storage.clear();
        }

    private:
        std::vector<EventHandler> m_handlers;
        std::vector<Event> m_storage;
    };

    IEventStorage* GetEventStorage(size_t hash) const
    {
        auto it = m_eventStorages.find(hash);
        if (it != m_eventStorages.end())
            return it->second.get();

        return nullptr;
    }

    template<typename Event>
    IEventStorage* GetOrCreateEventStorage()
    {
        auto hash = GetEventHash<Event>();

        auto storage = GetEventStorage(hash);
        if (storage != nullptr)
            return storage;

        auto result = m_eventStorages.emplace(hash, std::make_unique<EventStorage<Event> >());
        return result.first->second.get();
    }

    struct QueuedEvent
    {
        size_t m_hash;
        size_t m_index;
    };

    std::mutex m_queueMutex;
    std::vector<QueuedEvent> m_eventQueue;
    std::unordered_map<size_t, std::unique_ptr<IEventStorage> > m_eventStorages;
};
