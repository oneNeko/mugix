#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>

#include "../lock/locker.h"

template <typename T>
class ThreadPool
{
public:
    ThreadPool(int thread_number = 8, int max_requests = 10000);
    ~ThreadPool();

    bool Append(T *request);

private:
    static void *Worker(void *arg);
    void Run();

private:
    int m_thread_number;        //线程池中的线程数
    int m_max_requests;         //请求队列里允许的最大请求数
    pthread_t *m_threads;       //描述线程池的数组
    std::list<T *> m_workqueue; //请求队列
    Locker m_queuelocker;       //保护请求队列的互斥锁
    Sem m_queuestat;            //是否有任务要处理
    bool m_stop;                //是否结束线程
};

template <typename T>
ThreadPool<T>::ThreadPool(int thread_number, int max_requests) : m_thread_number(thread_number), m_max_requests(max_requests), m_stop(false), m_threads(NULL)
{
    if ((thread_number <= 0) || (max_requests <= 0))
    {
        throw exception();
    }

    m_threads = new pthread_t(m_thread_number);
    if (!m_threads)
    {
        throw exception();
    }

    // 创建thread_number个线程并将他们都设置为脱离线程
    for (int i = 0; i < thread_number; i++)
    {
        if (pthread_create(m_threads + i, NULL, Worker, this) != 0)
        {
            delete[] m_threads;
            throw exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw exception();
        }
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
    delete[] m_threads;
    m_stop = true;
}

template <typename T>
bool ThreadPool<T>::Append(T *request)
{
    // 操作工作队列时一定要加锁，因为他被所有线程共享
    m_queuelocker.Lock();
    if (m_workqueue.size() > m_max_requests)
    {
        m_queuelocker.Unlock();
        return false;
    }

    m_workqueue.push_back(request);
    m_queuelocker.Unlock();
    m_queuestat.Post();
    return true;
}

template <typename T>
void *ThreadPool<T>::Worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    pool->Run();
    return pool;
}

template <typename T>
void ThreadPool<T>::Run()
{
    while (!m_stop)
    {
        m_queuestat.Wait();
        m_queuelocker.Lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.Unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.Unlock();
        if (!request)
        {
            continue;
        }
        request->Process();
    }
}

#endif