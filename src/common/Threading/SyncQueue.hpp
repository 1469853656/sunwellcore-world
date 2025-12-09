#ifndef SyncQueue_hpp__
#define SyncQueue_hpp__

#include "Utilities/Optional.h"

#include <mutex>
#include <deque>
#include <condition_variable>
#include <atomic>

namespace Threading {
template<typename T>
class SyncQueue
{
public:
    SyncQueue()
    {
        m_closed = false;
    }

    ~SyncQueue()
    {
        close();
    }

    void close()
    {
        m_closed = true;
        m_condition.notify_all();
    }

    void open()
    {
        m_closed = false;
    }

    bool is_closed() const
    {
        return m_closed;
    }

    bool empty() 
    {
        std::lock_guard lock(m_mutex);
        return m_queue.empty();
    }

    Optional<T> pop()
    {
        std::unique_lock lock(m_mutex);
        while (m_queue.empty() && !m_closed)
        {
            m_condition.wait(lock);
        }

        if (m_closed)
            return {};

        auto item = std::move(m_queue.front());
        m_queue.pop_front();

        return item;
    }

    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        std::lock_guard lock(m_mutex);
        m_queue.emplace_back(std::forward<Args>(args)...);

        m_condition.notify_one();
    }

    void push_back(T&& item)
    {
        std::lock_guard lock(m_mutex);
        m_queue.push_back(std::move(item));

        m_condition.notify_one();
    }

    void push_back(T const& item)
    {
        std::lock_guard lock(m_mutex);
        m_queue.push_back(item);

        m_condition.notify_one();
    }

    void push_front(T&& item)
    {
        std::lock_guard lock(m_mutex);
        m_queue.push_front(std::move(item));

        m_condition.notify_one();
    }

    void push_front(T const& item)
    {
        std::lock_guard lock(m_mutex);
        m_queue.push_front(item);

        m_condition.notify_one();
    }

protected:
    std::deque<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic_bool m_closed;
};
}

#endif // SyncQueue_hpp__
