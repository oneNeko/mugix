#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

template <typename T>
class SafeQueue
{
private:
    std::queue<T> queue_; //利用模板函数构造队列
    std::mutex mutex_;    //访问互斥信号量

public:
    SafeQueue(){};
    SafeQueue(SafeQueue &&other){};
    ~SafeQueue(){};

    bool empty()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    int size()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.size();
    }

    //队尾添加元素
    void enqueue(T &t)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.emplace(t);
    }

    //队首取出元素
    bool dequeue(T &t)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        if (queue_.empty())
        {
            return false;
        }

        t = std::move(queue_.front()); //取出队首元素，并进行右值引用

        queue_.pop();

        return true;
    }
};

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
            bool dequeued;

            while (!pool_->is_shutdown_)
            {
                {
                // 为线程环境加锁，互访问工作线程的休眠和唤醒
                std::unique_lock<std::mutex> lock(pool_->conditional_mutex_);

                // 如果任务队列为空，阻塞当前线程
                if (pool_->queue_.empty())
                {
                    pool_->conditional_lock_.wait(lock); //等待条件变量通知，开启线程
                }

                //取出任务队列中的元素
                dequeued = pool_->queue_.dequeue(func);
                }
                
                //如果成功取出，执行工作函数
                if (dequeued)
                {
                    func();
                }
            }
        }
    };

private:
    bool is_shutdown_;                         // 线程池是否关闭
    SafeQueue<std::function<void()>> queue_;   //执行函数安全队列，即任务队列
    std::vector<std::thread> threads_;         //工作线程队列
    std::mutex conditional_mutex_;             //线程休眠锁互斥变量
    std::condition_variable conditional_lock_; //线程环境锁，可以让线程处于休眠或者唤醒状态

public:
    ThreadPool(const int num_threads = 8) : threads_(std::vector<std::thread>(num_threads)), is_shutdown_(false) {}

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