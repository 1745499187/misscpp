/*
 * object_pool.hpp
 *
 *  Created on: 2012-11-8
 *      Author: edward
 */

#ifndef OBJECT_POOL_HPP_
#define OBJECT_POOL_HPP_

#include <list>
#include <tr1/memory>
#include <pthread.h>
#include <stdio.h>

namespace nemo {

template<typename Object>
class object_pool
{
private:
    struct object_t
    {
        Object *obj;
        object_t *prev;
        object_t *next;
    };

public:
    typedef struct object_t *object_ptr;
    typedef object_t *object_list;

    // Constructor.
    object_pool() :
            live_list_(0), free_list_(0), live_list_sz_(0), free_list_sz_(0)
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    object_pool(size_t init_size) :
            live_list_(0), free_list_(0), live_list_sz_(0), free_list_sz_(0)
    {
        pthread_mutex_init(&m_mutex, NULL);
        for(size_t ind=0;ind<init_size;ind++)
        {
            object_ptr o = create();
            o->next = free_list_;
            if(free_list_ != NULL)
            {
                free_list_->prev = o;
            }
            free_list_ = o;
            free_list_sz_++;
        }
    }

    // Destructor destroys all objects.
    ~object_pool()
    {
        pthread_mutex_destroy(&m_mutex);
        destroy_list(live_list_);
        destroy_list(free_list_);
    }

    // Get the object at the start of the live list.
    object_ptr
    first()
    {
        return live_list_;
    }

    // Allocate a new object.
    object_ptr
    alloc()
    {
        pthread_mutex_lock(&m_mutex);
        object_ptr o = free_list_;
        if (o)
        {
            free_list_ = free_list_->next;
            free_list_sz_--;
        }
        else
        {
            o = create();
        }

        o->next = live_list_;
        o->prev = NULL;
        if (live_list_)
        {
            live_list_->prev = o;
        }
        live_list_ = o;
        live_list_sz_++;
        pthread_mutex_unlock(&m_mutex);
        return o;
    }

    // Free an object. Moves it to the free list. No destructors are run.
    void
    free(object_ptr o)
    {
        pthread_mutex_lock(&m_mutex);
        if (live_list_ == o)
        {
            live_list_ = o->next;
        }

        if (o->prev)
        {
            o->prev->next = o->next;
        }

        if (o->next)
        {
            o->next->prev = o->prev;
        }
        live_list_sz_--;

        o->next = free_list_;
        o->prev = NULL;
        free_list_ = o;
        free_list_sz_++;
        pthread_mutex_unlock(&m_mutex);
    }

    void
    print_stat(void)
    {
        printf("free=%zd, live=%zd\n", free_list_sz_, live_list_sz_);
    }

private:
    // Helper function to destroy all elements in a list.
    void
    destroy_list(object_list list)
    {
        while (list)
        {
            object_ptr o = list;
            list = o->next;
            delete o->obj;
            delete o;
        }
    }
    object_ptr
    create()
    {
        object_ptr o = new object_t;
        o->obj = new Object;
        o->prev = NULL;
        o->next = NULL;
        return o;
    }


    pthread_mutex_t m_mutex;

    // The list of live objects.
    object_list live_list_;

    // The free list.
    object_list free_list_;

    size_t live_list_sz_;
    size_t free_list_sz_;
};

// implemented by std::list, 效率是相当的低啊
template<typename T>
class object_pool1
{
public:
    typedef std::tr1::shared_ptr<T> object_ptr;
    typedef std::list<object_ptr> object_pool_t;

    object_pool1()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    object_pool1(size_t init_size)
    {
        pthread_mutex_init(&m_mutex, NULL);
        for(size_t ind=0;ind<init_size;ind++)
        {
            object_ptr o = create();
            m_freelist.push_back(o);
        }
    }
    ~object_pool1()
    {
        pthread_mutex_destroy(&m_mutex);
        m_freelist.clear();
        m_livelist.clear();
    }

    object_ptr
    alloc()
    {
        pthread_mutex_lock(&m_mutex);
        object_ptr ret;
        if (m_freelist.size() == 0)
        {
            m_freelist.push_back(create());
        }
        ret = m_freelist.front();
        m_freelist.pop_front();
        m_livelist.push_back(ret);
        pthread_mutex_unlock(&m_mutex);
        return ret;
    }

    void
    free(object_ptr o)
    {
        pthread_mutex_lock(&m_mutex);
        typename object_pool_t::iterator itr = m_livelist.begin();
        while (itr != m_livelist.end())
        {
            if (o.get() == (*itr).get())
            {
                m_freelist.push_back(o);
                m_livelist.erase(itr);
                break;
            }
            else
            {
                itr++;
            }
        }
        pthread_mutex_unlock(&m_mutex);
    }

    void
    print_stat(void)
    {
        printf("free=%zd, live=%zd\n", m_freelist.size(),
                m_livelist.size());
    }

private:
    object_ptr
    create()
    {
        return object_ptr(new T);
    }

    pthread_mutex_t m_mutex;
    object_pool_t m_freelist;
    object_pool_t m_livelist;
};

}

#endif /* OBJECT_POOL_HPP_ */
