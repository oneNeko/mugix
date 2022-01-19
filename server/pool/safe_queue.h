#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <mutex>
#include <queue>

template <typename T>
class SafeQueue
{
private:
    std::queue<T> queue_; //利用模板函数构造队列
    std::mutex mutex_;    //访问互斥信号量

public:
    SafeQueue(){};
    SafeQueue(SafeQueue &&other){};
    ~SafeQueue();

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

#endif