#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

/* 封装信号量的类 */
class Sem
{
public:
    // 创建并初始化信号量
    Sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw exception();
        }
    }

    // 销毁信号量
    ~Sem()
    {
        sem_destroy(&m_sem);
    }

    // 等待信号量
    bool Wait()
    {
        return sem_wait(&m_sem) == 0;
    }

    // 增加信号量
    bool Post()
    {
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

// 封装互斥锁的类
class Locker
{
public:
    // 创建并初始化互斥锁
    Locker()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
        }
    }

    // 销毁互斥锁
    ~Locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    // 获取互斥锁
    bool Lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    // 释放互斥锁
    bool Unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

private:
    pthread_mutex_t m_mutex;
};

// 封装条件变量的类
class Cond
{
public:
    // 创建并初始化条件变量
    Cond()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw exception();
        }
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            pthread_mutex_destroy(&m_mutex);
            throw exception();
        }
    }

    // 销毁条件变量
    ~Cond()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    // 等待条件变量
    bool Wait()
    {
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, &m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    // 唤醒等待条件变量的线程
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif