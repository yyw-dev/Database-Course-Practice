#pragma once

#include<iostream>
#include<stdlib.h>
#include<pthread.h>
#include<sys/time.h>
#include"../lock/lock.hpp"

template<class T>
class block_queue
{
public:

    block_queue(int max_size=100)
    {
        if(max_size<=0)
        {
            exit(-1);
        }

        m_max_size=max_size;
        m_array=new T[max_size];
        m_size=0;
        m_front=-1;
        m_back=-1;
    }

    void clear()
    {
        m_mutex.lock();
        m_size=0;
        m_front=-1;
        m_back=-1;
        m_mutex.unlock();
    }

    ~block_queue()
    {
        m_mutex.lock();
        if(m_array!=NULL)
        {
            delete []m_array;
        }
        m_mutex.unlock();
    }
    
    bool full()
    {
        m_mutex.lock();
        if(m_size>=m_max_size)
        {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }

    bool empty()
    {
        m_mutex.lock();
        if(0==m_size)
        {
            m_mutex.unlock();
            return true;
        }

        m_mutex.unlock();
        return false;
    }

    bool front(T& value)
    {
        m_mutex.lock();
        m_mutex.unlock();
        return true;
    }

private:
    locker m_mutex;
    cond m_cond;

    T* m_array;
    int m_size;
    int m_max_size;
    int m_front;
    int m_back;
};