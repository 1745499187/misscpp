/*
 * thread_pool.hpp
 *
 *  Created on: 2012-11-8
 *      Author: edward
 */

#ifndef THREAD_POOL_HPP_
#define THREAD_POOL_HPP_

#include <pthread.h>

namespace nemo {
namespace thread {

class thread
{
public:
    enum {
        S_IDLE,
        S_RUNNING,
        S_PAUSE,
        S_DEAD
    };

    thread(): m_t(0), m_status(S_IDLE)
    {
        pthread_mutex_init(&m_mtx, NULL);
        pthread_cond_init(&m_cond, NULL);
    }
    virtual ~thread()
    {
        pthread_mutex_destroy(&m_mtx);
        pthread_cond_destroy(&m_cond);
    }
    void start()
    {
        pthread_create(&m_t, NULL, thread::thread_func, this);
        m_status = S_RUNNING;
    }
    void stop()
    {
        pthread_exit(&m_t);
        m_status = S_IDLE;
    }
    void join()
    {
        pthread_join(m_t, NULL);
    }
    void pause()
    {
        pthread_mutex_lock(&m_mtx);
        while(m_status == S_RUNNING)
        {
            m_status = S_PAUSE;
            pthread_cond_wait(&m_cond, &m_mtx);
        }
        pthread_mutex_unlock(&m_mtx);

    }
    void resume()
    {
        pthread_cond_signal(&m_cond);
    }
    virtual void run() = 0;

private:
    static void *thread_func(void *arg)
    {
        thread *self = static_cast<thread *>(arg);
        while(self->m_status != S_DEAD)
        {
            if(self->m_status == S_RUNNING)
                self->run();
        }
        return NULL;
    }

protected:
    pthread_t m_t;
    pthread_mutex_t m_mtx;
    pthread_cond_t m_cond;
    int m_status;
};

class thread_pool
{

};

}
}


#endif /* THREAD_POOL_HPP_ */
