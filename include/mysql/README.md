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
