#ifndef THREAD_POOL_H
#define THREAD_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <future>

#include "safe_queue.h"

class ThreadPool
{
private:
    // 线程工作类
    class ThreadWorker
    {
    private:
        int id_;           // 工作id
        ThreadPool *pool_; //所属线程池

    public:
        ThreadWorker(ThreadPool *pool, const int id) : pool_(pool), id_(id) {}

        void operator()()
        {
            std::function<void()> func;
            bool is_dequeue_ing;
        }
    };

private:
    bool is_shutdown_;                         // 线程池是否关闭
    SafeQueue<std::function<void()>> queue_;   //执行函数安全队列，即任务队列
    std::vector<std::thread> threads_;         //工作线程队列
    std::mutex conditional_mutex_;             //线程休眠锁互斥变量
    std::condition_variable conditional_lock_; //线程环境锁，可以让线程处于休眠或者唤醒状态

public:
    ThreadPool(const int num_threads = 4) : threads_(std::vector<std::thread>(num_threads)), is_shutdown_(false) {}

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    //初始化线程池
    void init()
    {
        for (int i = 0; i < threads_.size(); ++i)
        {
            threads_.at(i) = std::thread(ThreadWorker(this, i)); //分配工作线程
        }
    }

    //等待直到线程完成它们的工作后关闭线程池
    void shutdown()
    {
        is_shutdown_ = true;
        conditional_lock_.notify_all(); //通知，唤醒所有工作线程

        for (int i = 0; i < threads_.size(); ++i)
        {
            if (threads_.at(i).joinable()) //判断线程是否在等待
            {
                threads_.at(i).join(); //将线程加入到等待队列
            }
        }
    }

    // 提交函数
    // F 函数
    // Args 函数实参
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        // Create a function with bounded parameter ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...); // 连接函数和参数定义，特殊函数类型，避免左右值错误

        // Encapsulate it into a shared pointer in order to be able to copy construct
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // Warp packaged task into void function
        std::function<void()>
            warpper_func = [task_ptr]()
        {
            (*task_ptr)();
        };

        // 队列通用安全封包函数，并压入安全队列
        queue_.enqueue(warpper_func);

        // 唤醒一个等待中的线程
        conditional_lock_.notify_one();

        // 返回先前注册的任务指针
        return task_ptr->get_future();
    }
};

#endif