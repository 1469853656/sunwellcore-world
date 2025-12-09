#include "BackgroundWorker.hpp"

namespace Threading
{
    void BackgroundWorker::Initialize( size_t threadsCount )
    {
        m_threads.reserve( threadsCount );

        m_isRunning = true;
        for ( auto i = 0u; i < threadsCount; ++i )
        {
            m_threads.emplace_back( [this]
            {
                while ( m_isRunning )
                {
                    auto task = m_queue.pop();
                    if (task)
                        (*task)();
                }
            } );
        }
    }

    void BackgroundWorker::Shutdown()
    {
        m_isRunning = false;
        m_queue.close();
        for ( auto & thread : m_threads )
        {
            if ( thread.joinable() )
                thread.join();
        }

        m_threads.clear();
    }

    BackgroundWorker& GetBackgroundWorker()
    {
        static BackgroundWorker s_worker;
        return s_worker;
    }
}
