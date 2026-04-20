# MySQL 开发环境说明

## 安装 MySQL 开发头文件

在 Ubuntu/Debian 系统中，可以安装 MySQL 客户端开发包：

```bash
sudo apt install libmysqlclient-dev
```

如果使用 MariaDB，也可以安装兼容的开发包：

```bash
sudo apt install libmariadb-dev
```

## 引入头文件

代码中如果直接使用 MySQL C API，需要包含：

```cpp
#include <mysql/mysql.h>
```

如果使用本项目封装的连接池，建议在源文件中包含：

```cpp
#include "mysql/sql_connection_pool.hpp"
```

## 编译链接

编译时需要添加项目头文件目录，并链接 MySQL 客户端库：

```bash
g++ src/main.cpp -Iinclude -lmysqlclient
```

其中：

- `-Iinclude`：让编译器能够找到 `include` 目录下的项目头文件。
- `-lmysqlclient`：链接 MySQL 客户端库。

## MySQL 连接池对外接口

头文件：

```cpp
#include "mysql/sql_connection_pool.hpp"
```

### connection_pool

```cpp
static connection_pool* GetInstance();
```

获取连接池单例对象。

```cpp
void init(string url, string User, string PassWord, string DataBaseName,
          int Port, int MaxConn, int close_log);
```

初始化连接池，创建 `MaxConn` 个 MySQL 连接。

```cpp
MYSQL* GetConnection();
```

从连接池中获取一个数据库连接。

```cpp
bool ReleaseConnect(MYSQL* conn);
```

将数据库连接归还给连接池。

```cpp
int GetFreeConn();
```

获取当前空闲连接数量。

```cpp
void DestoryPool();
```

销毁连接池。

### connectionRALL

```cpp
connectionRALL(MYSQL** con, connection_pool* connPool);
```

从连接池中获取连接，并将连接保存到 `*con`。

```cpp
~connectionRALL();
```

对象销毁时自动释放连接。

### 简单使用示例

```cpp
connection_pool* connPool = connection_pool::GetInstance();

connPool->init("127.0.0.1", "root", "password", "test_db", 3306, 8, 0);

MYSQL* mysql = NULL;
connectionRALL mysqlConn(&mysql, connPool);

if (mysql != NULL) {
    mysql_query(mysql, "SELECT * FROM user");
}
```
