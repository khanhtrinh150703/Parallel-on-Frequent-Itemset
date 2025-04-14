#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
#include <stdexcept>
#include <type_traits> // For std::result_of

class ThreadPool
{
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template <class F>
    auto enqueue(F &&f) -> std::future<typename std::result_of<F()>::type>;
    void wait();
    void taskCompleted();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::mutex mutex;
    std::atomic<int> activeTasks{0};
    bool stop;
};

// Definition of enqueue function
template <class F>
auto ThreadPool::enqueue(F &&f) -> std::future<typename std::result_of<F()>::type>
{
    using return_type = typename std::result_of<F()>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        // Don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task]()
                      { (*task)(); });
    }
    condition.notify_one();
    return res;
}

#endif // THREADPOOL_HPP
