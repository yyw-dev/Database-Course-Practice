# 高校班级事务管理系统

## 项目概述
高校班级事务管理系统是一个基于C++的班级管理平台，提供用户管理、通知管理、班级信息管理、学习管理、班费管理、纪律管理和活动管理等全面功能。

## 技术栈
- 语言：C++17
- 数据库：MySQL
- Web框架：Crow (C++微框架)
- 构建工具：CMake
- 测试框架：Google Test

## 项目结构
```
高校班级事务管理系统/
├── src/                      # 源代码
│   ├── core/                # 核心业务逻辑
│   │   ├── user/           # 用户管理模块
│   │   ├── notification/   # 通知管理模块
│   │   ├── class/          # 班级信息管理模块
│   │   ├── study/          # 学习管理模块
│   │   ├── finance/        # 班费管理模块
│   │   ├── discipline/     # 纪律管理模块
│   │   └── activity/       # 活动管理模块
│   ├── database/            # 数据库操作
│   ├── utils/               # 工具函数
│   ├── config/              # 配置文件处理
│   └── web/                 # Web服务
├── include/                  # 头文件
├── tests/                    # 测试代码
├── docs/                     # 文档
│   ├── design/              # 设计文档
│   └── api/                 # API文档
├── third_party/              # 第三方库
├── build/                    # 构建输出
└── CMakeLists.txt           # CMake配置文件
```

## 功能模块

### 1. 用户管理模块
- 登录/登出
- 角色权限管理
- 个人信息维护

### 2. 通知管理模块
- 发布通知
- 查看通知
- 通知归档

### 3. 班级信息管理模块
- 班级基本信息维护
- 成员信息管理
- 班委职务分配

### 4. 学习管理模块
- 课表管理
- 成绩录入与查询
- 成绩统计与分析
- 学习资料共享

### 5. 班费管理模块
- 收入记账
- 支出记账
- 班费余额查询
- 收支明细公示

### 6. 纪律管理模块
- 请假申请
- 请假审批
- 销假处理
- 纪律处分记录

### 7. 活动管理模块
- 活动发布
- 活动报名
- 活动总结

## 安装和编译

### 环境要求
- C++17编译器 (g++ 7.0+ 或 clang 5.0+)
- CMake 3.10+
- MySQL 5.7+
- Crow C++ Web框架

### 编译步骤
```bash
# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake ..

# 编译项目
make -j4

# 运行测试
make test

# 安装
sudo make install
```

## 数据库配置
1. 创建MySQL数据库
2. 导入数据库schema
3. 配置数据库连接信息
4. 初始化基础数据

## 运行
```bash
# 启动服务
./class_management_system

# 服务默认监听8080端口
```

## 开发规范
- 遵循Google C++ Style Guide
- 使用智能指针管理内存
- RAII原则管理资源
- 异常安全编程

## API接口
系统提供RESTful API接口：
- 用户相关：`/api/v1/users/*`
- 通知相关：`/api/v1/notifications/*`
- 班级相关：`/api/v1/classes/*`
- 学习相关：`/api/v1/study/*`
- 班费相关：`/api/v1/finance/*`
- 纪律相关：`/api/v1/discipline/*`
- 活动相关：`/api/v1/activity/*`

## 测试
```bash
# 运行单元测试
./tests/unit_tests

# 运行集成测试
./tests/integration_tests
```

## 部署
- 支持Linux/Windows平台
- 可作为系统服务运行
- 支持Docker容器化部署