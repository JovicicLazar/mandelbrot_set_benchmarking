#include "thread_pool.hpp"
#include <utility>

ThreadPool::ThreadPool(const unsigned int thread_number)
{
    for (unsigned int i = {0}; i < thread_number; ++i)
    {
        m_threads.emplace_back([this]
        {
            while (true)
            {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(this->m_q_mutex);
                    this->m_cv.wait(lock, [this]
                                    { return !this->m_tasks.empty() || this->m_stop; });

                    if (this->m_stop && this->m_tasks.empty())
                    {
                        return;
                    }

                    task = std::move(this->m_tasks.front());
                    this->m_tasks.pop();
                }

                task(); 
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(m_q_mutex);
        m_stop = true;
    }
    m_cv.notify_all();
    for (std::thread &worker : m_threads)
        worker.join();
}

