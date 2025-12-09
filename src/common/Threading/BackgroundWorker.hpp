#ifndef BACKGROUND_WORKER_H
#define BACKGROUND_WORKER_H

#include "Threading/SyncQueue.hpp"
#include "Utilities/UniqueFunction.hpp"

#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include <future>

namespace Threading
{
    namespace Detail::ReturnType
    {
        struct Void
        {
        };
        struct Future
        {
        };
    }
    template<typename T>
    struct deduct_false
    {
        constexpr static bool value = false;
    };

    constexpr Detail::ReturnType::Void      DetachedTask;
    constexpr Detail::ReturnType::Future    AwaitableTask;

    template< typename T >
    class TaskResult
    {
    public:
        using FutureType = std::future< T >;

        TaskResult() = default;

        TaskResult( FutureType && future )
            : m_future( std::move( future ) )
        {
        }

        bool IsValid() const
        {
            return m_future.valid();
        }

        bool IsReady() const
        {
            return m_future.wait_for( std::chrono::seconds( 0 ) ) == std::future_status::ready;
        }

        void Wait() const
        {
            m_future.wait();
        }

        T GetResult()
        {
            assert( IsValid() );
            return m_future.get();
        }

    private:
        std::future< T > m_future;
    };

    using BackgroundTask = UniqueFunction< void() >;

    class BackgroundWorker
    {
    public:
        BackgroundWorker() = default;

        void                        Initialize( size_t threadsCount );
        void                        Shutdown();

        template< typename Callable, typename CompletionToken = Detail::ReturnType::Void >
        auto                        Run( Callable task, CompletionToken )
        {
            if constexpr ( std::is_same_v< CompletionToken, Detail::ReturnType::Void > )
            {
                m_queue.push_back( std::move( task ) );
            }
            else if constexpr ( std::is_same_v< CompletionToken, Detail::ReturnType::Future> )
            {
                using ResultType = decltype( std::declval<Callable>()( ) );
                std::packaged_task< ResultType() > package( std::move( task ) );

                auto result = package.get_future();
                m_queue.emplace_back( std::move( package ) );

                return TaskResult< ResultType >( std::move( result ) );
            }
            else
            {
                static_assert(deduct_false< CompletionToken >::value,"ERROR: unsupported task type!");
            //    static_assert( false, "ERROR: unsupported task type!" );
            }
        }

    private:
        std::atomic_bool            m_isRunning = { false };
        std::vector< std::thread >  m_threads;
        SyncQueue< BackgroundTask > m_queue;
    };

    BackgroundWorker& GetBackgroundWorker();

    struct BackgroundGroupUpdater
    {
    public:
        BackgroundGroupUpdater()
        {
            m_future = m_promise.get_future();

            AddRef();
        }

        ~BackgroundGroupUpdater()
        {
            Wait();
        }

        template<typename Func>
        void Run(Func&& func)
        {
            AddRef();
            GetBackgroundWorker().Run([this, func = std::move(func)]
            {
                func();
                RemRef();
            }, Threading::DetachedTask);
        }

        void Wait()
        {
            if (!m_waitCalled.exchange( true ))
                RemRef();

            if (m_future.valid())
                m_future.wait();
        }

    private:
        void AddRef()
        {
            m_counter.fetch_add(1);
        }

        void RemRef()
        {
            if (m_counter.fetch_sub(1) == 1)
                m_promise.set_value();
        }

        std::atomic_bool    m_waitCalled = false;
        std::atomic_uint    m_counter = 0;
        std::future<void>   m_future;
        std::promise<void>  m_promise;
    };
}

#define sBackgroundWorker (&Threading::GetBackgroundWorker())
#endif
