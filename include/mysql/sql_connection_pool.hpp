#pragma once

#include <cstring>
#include <error.h>
#include <iostream>
#include <list>
#include<mysql/mysql.h>
#include<pthread.h>
#include<string>
#include"../lock/lock.hpp"
#include"../lock/lock.hpp"

using namespace std;

class connection_pool
{
public:
    MYSQL* GetConnection();
    bool ReleaseConnect(MYSQL* conn);
    int GetFreeConn();
    void DestoryPool();

    static connection_pool* GetInstance();

    void init(string url,string User,string PassWord,string DataBaseName,int Port,int MaxConn,int close_log);

private:
    connection_pool();
    ~connection_pool();

    int m_MaxConn;
    int m_CurConn;
    int m_FreeConn;
    locker lock;
    list<MYSQL*> connList;
    sem reserve;

public:
    string m_url;
    string m_Port;
    string m_User;
    string m_PassWord;
    string m_DataBaseName;
    int m_close_log;
};

class connectionRALL
{
public:
    connectionRALL(MYSQL**con, connection_pool* connPool);
    ~connectionRALL();
private:
    MYSQL* conRALL;
    connection_pool* poolRALL;
};

