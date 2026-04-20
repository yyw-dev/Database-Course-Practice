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

    // 构造一个固定容量的阻塞队列。
    // max_size 是底层循环数组的容量，必须大于 0。
    block_queue(int max_size=100)
    {
        if(max_size<=0)
        {
            exit(-1);
        }

        m_max_size=max_size;
        m_array=new T[max_size];

        // 当前队列元素个数。
        m_size=0;

        // 循环队列下标。
        // 这里的 m_front 表示“队首元素的前一个位置”，不是当前队首。
        // 因此 pop 时需要先 m_front=(m_front+1)%m_max_size，再取元素。
        m_front=-1;

        // m_back 表示当前队尾位置。push 时先向后移动，再写入元素。
        m_back=-1;
    }

    // 清空队列，只重置逻辑状态，不销毁底层数组。
    // 多线程环境下访问 m_size/m_front/m_back 都需要加锁。
    void clear()
    {
        m_mutex.lock();
        m_size=0;
        m_front=-1;
        m_back=-1;
        m_mutex.unlock();
    }

    // 析构时释放底层数组。
    // 注意：调用析构函数前，应保证没有其他线程正在使用这个队列。
    ~block_queue()
    {
        m_mutex.lock();
        if(m_array!=NULL)
        {
            delete []m_array;
        }
        m_mutex.unlock();
    }
    
    // 判断队列是否已满。
    // m_size 可能被多个线程读写，所以读取时也加锁。
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

    // 判断队列是否为空。
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

    // 读取队首元素，但不弹出。
    bool front(T& value)
    {
        m_mutex.lock();
        if(0==m_size)
        {
            m_mutex.unlock();
            return false;
        }

        // 注意：按照当前循环队列设计，m_front 是队首前一个位置。
        // 这里直接访问 m_array[m_front] 会在 m_front==-1 时越界。
        // 更合理的写法是：value=m_array[(m_front+1)%m_max_size];
        value=m_array[m_front];
        m_mutex.unlock();
        return true;
    }

    // 读取队尾元素，但不弹出。
    bool back(T& value)
    {
        m_mutex.lock();
        if(0==m_size)
        {
            m_mutex.unlock();
            return false;
        }
        value=m_array[m_back];
        m_mutex.unlock();
        return true;
    }

    // 返回当前元素个数。
    int size()
    {
        int temp=0;

        //线程安全的访问阻塞循环队列
        m_mutex.lock();
        temp=m_size;
        m_mutex.unlock();

        return temp;
    }

    // 返回队列最大容量。
    int max_size()
    {
        int tmp=0;

        m_mutex.lock();
        tmp=m_max_size;
        m_mutex.unlock();

        return tmp;
    }

    //往队列里面添加元素，需要将所有使用队列的线程唤醒
    //当有元素push进栈，生产者产生元素

    // 非阻塞入队。
    // 如果队列已满，直接返回 false；不会等待消费者 pop。
    bool push(const T& item)
    {
        m_mutex.lock();

        if(m_size>=m_max_size)
        {
            // 这里唤醒所有等待线程不是必须的。
            // 因为当前 push 满了会直接返回，并没有生产者在这里等待“队列非满”。
            m_cond.broadcast();
            m_mutex.unlock();
            return false;
        }

        // 队尾后移一格，利用取模实现循环数组。
        m_back=(m_back+1)%m_max_size;

        // 在新的队尾位置写入元素。
        m_array[m_back]=item;

        // 更新当前元素个数。
        m_size++;

        // 入队成功后，队列中至少有一个元素。
        // 唤醒等待在 pop 中的消费者线程，让它们重新检查 m_size。
        m_cond.broadcast();
        m_mutex.unlock();

        return true;

    }

    //pop,如果队列中没有元素，将会等待条件变量
    // 阻塞出队。
    // 如果队列为空，当前线程会释放锁并睡眠，直到被 push 唤醒。
    bool pop(T& item)
    {
        m_mutex.lock();

        // 条件变量等待必须用 while。
        // 被唤醒不代表一定有数据，可能是虚假唤醒，也可能数据已被其他消费者取走。
        while(m_size<=0)
        {
            // pthread_cond_wait 会原子地释放 m_mutex 并睡眠。
            // 被唤醒后，它会先重新获得 m_mutex，然后 wait 才返回。
            if(!m_cond.wait(m_mutex.get()))
            {
                //阻塞函数调用失败，直接解锁返回
                m_mutex.unlock();
                return false;
            }
        }

        //队首指针指向的是队首的上一位，避免空和满判断重合
        // 因此真正取元素前，要先把 m_front 移动到当前队首位置。
        m_front=(m_front+1)%m_max_size;
        item=m_array[m_front];
        m_size--;

        // 当前 push 是非阻塞的，所以 pop 后不需要唤醒等待“队列非满”的生产者。
        m_mutex.unlock();
        return true;

    }

    // 带超时时间的出队。
    // ms_timeout 表示最多等待多少毫秒。
    // 注意：当前函数签名是 T* item，但下面直接 item=m_array[m_front] 类型不匹配。
    // 如果要和上面的 pop 保持一致，建议改成 bool pop(T& item,int ms_timeout)。
    bool pop(T& item,int ms_timeout)
    {
        struct  timespec t={0,0};
        struct timeval now={0,0};

        // gettimeofday 获取当前时间。
        // pthread_cond_timedwait 需要的是“绝对超时时间”，不是相对等待时长。
        gettimeofday(&now,NULL);

        m_mutex.lock();
        if(m_size<=0)
        {
            t.tv_sec=now.tv_sec+ms_timeout/1000;

            // 注意：tv_nsec 单位是纳秒。
            // 1 毫秒 = 1000000 纳秒，这里乘 1000 会导致时间计算偏小。
            // 还应加上 now.tv_usec*1000，并处理超过 1 秒的进位。
            t.tv_nsec=(ms_timeout%1000)*1000;

            if(!m_cond.timewait(m_mutex.get(),t))
            {
                m_mutex.unlock();
                return false;
            }
        }

        // timewait 返回后，仍然需要再次判断队列是否为空。
        // 因为可能是虚假唤醒，或者被其他消费者先取走了数据。
        //唤醒取得锁还是没有满足条件，直接当超时处理，返回函数
        if(m_size<=0)
        {
            m_mutex.unlock();
            return false;
        }

        m_front=(m_front+1)%m_max_size;

        //  item 是 T&，这里才应该写 item=m_array[m_front]。
        item=m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }


private:
    // 互斥锁：保护下面所有队列状态。
    locker m_mutex;

    // 条件变量：用于队列为空时阻塞 pop，push 成功后唤醒等待线程。
    cond m_cond;

    // 循环数组，保存实际元素。
    T* m_array;

    // 当前元素个数。
    int m_size;

    // 队列最大容量。
    int m_max_size;

    // 队首元素的前一个位置。
    int m_front;

    // 队尾元素的位置。
    int m_back;
};
