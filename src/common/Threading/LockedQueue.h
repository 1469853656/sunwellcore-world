
#ifndef LOCKEDQUEUE_H
#define LOCKEDQUEUE_H

#include <deque>
#include "Debugging/Errors.h"

namespace Threading {
template<class T >
class LockedQueue
{
    //! Lock access to the queue.
    std::mutex _lock;

    //! Storage backing the queue.
    std::deque<T> _queue;

public:
    //! Create a LockedQueue.
    LockedQueue()
    {
    }

    //! Destroy a LockedQueue.
    virtual ~LockedQueue()
    {
    }

    //! Adds an item to the queue.
    void add(const T& item)
    {
        std::lock_guard<std::mutex> lg(_lock);
        _queue.push_back(item);
    }

    //! Gets the next result in the queue, if any.
    bool next(T& result)
    {
        std::lock_guard<std::mutex> lg(_lock);

        if (_queue.empty())
            return false;

        result = _queue.front();
        _queue.pop_front();

        return true;
    }

    template<class Checker>
    bool next(T& result, Checker& check)
    {
        std::lock_guard<std::mutex> lg(_lock);

        if (_queue.empty())
            return false;

        result = _queue.front();
        if (!check.Process(result))
            return false;

        _queue.pop_front();
        return true;
    }

    //! Peeks at the top of the queue
    T& peek()
    {
        std::lock_guard<std::mutex> lg(_lock);

        T& result = _queue.front();

        return result;
    }

    ///! Checks if we're empty or not with locks held
    bool empty()
    {
        std::lock_guard<std::mutex> lg(_lock);
        return _queue.empty();
    }
};
}
#endif
