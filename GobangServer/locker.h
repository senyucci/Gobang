/********************************************************************************* 
  *Copyright(C): Seny
  *FileName: locker
  *Author: Seny
  *Date: 2021/5/29
  *Description: Package the thread lock
**********************************************************************************/  
#ifndef _LOCKER_
#define _LOCKER_
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

class Locker
{
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

public:
    Locker()
    {
        if(pthread_mutex_init(&m_mutex,NULL)!=0)
        {
            printf("mutex init error");
        }
        if(pthread_cond_init(&m_cond,NULL)!=0)
        {
            pthread_mutex_destroy(&m_mutex);
            printf("cond init error");
        }
    }
    ~Locker()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    inline bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    inline bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

    inline bool wait()
    {
        int ans = 0;
        ans = pthread_cond_wait(&m_cond,&m_mutex);
        return ans == 0;
    }

    inline bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }

    inline bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }
};

#endif