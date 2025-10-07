#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <functional>
#include <queue>

class ThreadPool
{
public:
    ThreadPool(const unsigned int number_of_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <typename F, typename... Args>
    void enqueue(F &&f, Args &&...args);

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_q_mutex;
    std::condition_variable m_cv;
    bool m_stop = false;
};


template <typename F, typename... Args>
void ThreadPool::enqueue(F&& f, Args&&... args)
{
    {
        std::lock_guard<std::mutex> lock(m_q_mutex);
        if (m_stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        m_tasks.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    }
    m_cv.notify_one();
}

#endif