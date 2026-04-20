#pragma once

#include<iostream>
#include<string>
#include<pthread.h>
#include"block_queue.hpp"
#include<stdarg.h>

using namespace std;

class Log
{
public:
    static Log* get_instance()
    {
        static Log instance;
        return &instance;
    }

    static void* flush_log_thread(void* args)
    {
        Log::get_instance()->async_write_log();
    }
    //文件路径，开关参数，日志缓冲区大小，最大行数，最长日志条队列
    bool init(const char* file_name,int close_log,int log_buf_size=8192,int split_lines=5000000,int max_queue_size=0);

    void write_log(int level,const char* format,...);

    void flush(void);

private:
    Log();
    virtual ~Log();
    void *async_write_log()
    {
        string single_log;
        //从阻塞对列中去除一个日志string，写入文件
        while(m_log_queue->pop(single_log))
        {
            m_mutex.lock();
            fputs(single_log.c_str(),m_fp);
            m_mutex.unlock();
        }
    }

private:
    char dir_name[128];//文件路径名
    char log_name[128];//log文件路经名
    int m_split_lines;//日志最大行数
    int m_log_buf_size;//日志缓冲区大小
    long long m_count;//日志行数
    int m_today;//记录当前时间
    FILE* m_fp;//日志文件指针
    char* m_buf;
    block_queue<string>* m_log_queue;//日志阻塞队列
    bool m_is_async;//是否同步标志位
    locker m_mutex;//互斥锁
    int m_close_log;//关闭日志
};
