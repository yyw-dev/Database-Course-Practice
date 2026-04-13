# API文档

## 概述

高校班级事务管理系统提供RESTful API接口，支持用户管理、通知管理、班级管理、学习管理、班费管理、纪律管理和活动管理等功能。

## 基础信息

- **基础URL**: `http://localhost:8080/api/v1`
- **API版本**: v1
- **数据格式**: JSON
- **认证方式**: Token认证（后续版本支持）

## 通用响应格式

所有API响应都遵循以下格式：

```json
{
  "success": true,
  "message": "操作成功",
  "data": {},
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 响应字段说明

| 字段 | 类型 | 说明 |
|------|------|------|
| success | boolean | 操作是否成功 |
| message | string | 响应消息 |
| data | object | 响应数据（可选） |
| timestamp | string | 响应时间 |

## 错误处理

### HTTP状态码

| 状态码 | 说明 |
|--------|------|
| 200 | 请求成功 |
| 201 | 创建成功 |
| 400 | 请求参数错误 |
| 401 | 未授权访问 |
| 403 | 权限不足 |
| 404 | 资源不存在 |
| 500 | 服务器内部错误 |

### 错误响应示例

```json
{
  "success": false,
  "message": "用户名已存在",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

## 用户管理API

### 用户登录

**请求**
- **URL**: `/users/login`
- **方法**: `POST`
- **Content-Type**: `application/json`

**请求体**
```json
{
  "username": "admin",
  "password": "admin123"
}
```

**成功响应**
```json
{
  "success": true,
  "message": "登录成功",
  "data": {
    "user_id": 1,
    "username": "admin",
    "real_name": "系统管理员",
    "role": 6
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 用户注册

**请求**
- **URL**: `/users/register`
- **方法**: `POST`
- **Content-Type**: `application/json`

**请求体**
```json
{
  "username": "student001",
  "password": "password123",
  "real_name": "张三",
  "email": "zhangsan@example.com",
  "phone": "13800138000",
  "class_id": 1,
  "student_id": "2021001",
  "role": 0
}
```

**成功响应**
```json
{
  "success": true,
  "message": "用户注册成功",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 获取用户信息

**请求**
- **URL**: `/users/{user_id}`
- **方法**: `GET`
- **Headers**: `Authorization: Bearer {token}`

**成功响应**
```json
{
  "success": true,
  "message": "获取用户信息成功",
  "data": {
    "id": 1,
    "username": "admin",
    "real_name": "系统管理员",
    "email": "admin@example.com",
    "phone": "13800138000",
    "role": 6,
    "class_id": 1,
    "student_id": "ADMIN001",
    "is_active": true
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 更新用户信息

**请求**
- **URL**: `/users/{user_id}`
- **方法**: `PUT`
- **Headers**: `Authorization: Bearer {token}`
- **Content-Type**: `application/json`

**请求体**
```json
{
  "real_name": "李四",
  "email": "lisi@example.com",
  "phone": "13800138001"
}
```

**成功响应**
```json
{
  "success": true,
  "message": "用户信息更新成功",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 修改密码

**请求**
- **URL**: `/users/{user_id}/password`
- **方法**: `PUT`
- **Headers**: `Authorization: Bearer {token}`
- **Content-Type**: `application/json`

**请求体**
```json
{
  "old_password": "old_password",
  "new_password": "new_password"
}
```

**成功响应**
```json
{
  "success": true,
  "message": "密码修改成功",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 获取用户列表

**请求**
- **URL**: `/users?class_id=1&role=0`
- **方法**: `GET`
- **Headers**: `Authorization: Bearer {token}`

**查询参数**
- `class_id`: 班级ID（可选）
- `role`: 角色（可选）

**成功响应**
```json
{
  "success": true,
  "message": "获取用户列表成功",
  "data": {
    "users": [
      {
        "id": 1,
        "username": "student001",
        "real_name": "张三",
        "email": "zhangsan@example.com",
        "phone": "13800138000",
        "role": 0,
        "class_id": 1,
        "student_id": "2021001",
        "is_active": true
      }
    ]
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```

## 通知管理API

### 发布通知

**请求**
- **URL**: `/notifications`
- **方法**: `POST`
- **Headers**: `Authorization: Bearer {token}`
- **Content-Type**: `application/json`

**请求体**
```json
{
  "title": "重要通知",
  "content": "这是一条重要通知内容",
  "type": 0,
  "class_id": 1,
  "is_important": true
}
```

**字段说明**
- `type`: 通知类型 (0=普通, 1=紧急, 2=活动, 3=学术, 4=财务)

**成功响应**
```json
{
  "success": true,
  "message": "通知发布成功",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 获取通知列表

**请求**
- **URL**: `/notifications?class_id=1&page=1&page_size=20`
- **方法**: `GET`
- **Headers**: `Authorization: Bearer {token}`

**查询参数**
- `class_id`: 班级ID（必需）
- `page`: 页码（可选，默认1）
- `page_size`: 每页数量（可选，默认20）

**成功响应**
```json
{
  "success": true,
  "message": "获取通知列表成功",
  "data": {
    "notifications": [
      {
        "id": 1,
        "title": "重要通知",
        "content": "这是一条重要通知内容",
        "type": 0,
        "sender_id": 1,
        "class_id": 1,
        "is_important": true,
        "is_archived": false
      }
    ]
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 获取通知详情

**请求**
- **URL**: `/notifications/{notification_id}`
- **方法**: `GET`
- **Headers**: `Authorization: Bearer {token}`

**成功响应**
```json
{
  "success": true,
  "message": "获取通知详情成功",
  "data": {
    "id": 1,
    "title": "重要通知",
    "content": "这是一条重要通知内容",
    "type": 0,
    "sender_id": 1,
    "class_id": 1,
    "is_important": true,
    "is_archived": false
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 更新通知

**请求**
- **URL**: `/notifications/{notification_id}`
- **方法**: `PUT`
- **Headers**: `Authorization: Bearer {token}`
- **Content-Type**: `application/json`

**请求体**
```json
{
  "title": "更新后的通知标题",
  "content": "更新后的通知内容",
  "type": 1,
  "is_important": false
}
```

**成功响应**
```json
{
  "success": true,
  "message": "通知更新成功",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 删除通知

**请求**
- **URL**: `/notifications/{notification_id}`
- **方法**: `DELETE`
- **Headers**: `Authorization: Bearer {token}`

**成功响应**
```json
{
  "success": true,
  "message": "通知删除成功",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 标记通知为已读

**请求**
- **URL**: `/notifications/{notification_id}/read`
- **方法**: `POST`
- **Headers**: `Authorization: Bearer {token}`

**成功响应**
```json
{
  "success": true,
  "message": "通知已标记为已读",
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 获取未读通知

**请求**
- **URL**: `/notifications/unread?class_id=1`
- **方法**: `GET`
- **Headers**: `Authorization: Bearer {token}`

**查询参数**
- `class_id`: 班级ID（可选）

**成功响应**
```json
{
  "success": true,
  "message": "获取未读通知成功",
  "data": {
    "notifications": [
      {
        "id": 1,
        "title": "未读通知",
        "content": "这是一条未读通知",
        "type": 0,
        "sender_id": 1,
        "class_id": 1,
        "is_important": false
      }
    ]
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```

### 获取未读通知数量

**请求**
- **URL**: `/notifications/unread/count?class_id=1`
- **方法**: `GET`
- **Headers**: `Authorization: Bearer {token}`

**查询参数**
- `class_id`: 班级ID（可选）

**成功响应**
```json
{
  "success": true,
  "message": "获取未读通知数量成功",
  "data": {
    "count": 5
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```

## 数据模型

### 用户(User)

| 字段 | 类型 | 说明 |
|------|------|------|
| id | integer | 用户ID |
| username | string | 用户名 |
| real_name | string | 真实姓名 |
| email | string | 邮箱 |
| phone | string | 手机号 |
| role | integer | 角色 (0=学生, 1=班长, 2=学习委员, 3=生活委员, 4=体育委员, 5=教师, 6=管理员) |
| class_id | integer | 班级ID |
| student_id | string | 学号 |
| is_active | boolean | 是否激活 |

### 通知(Notification)

| 字段 | 类型 | 说明 |
|------|------|------|
| id | integer | 通知ID |
| title | string | 标题 |
| content | string | 内容 |
| type | integer | 类型 (0=普通, 1=紧急, 2=活动, 3=学术, 4=财务) |
| sender_id | integer | 发送者ID |
| class_id | integer | 班级ID |
| is_important | boolean | 是否重要 |
| is_archived | boolean | 是否归档 |

## 开发说明

### 认证机制

目前API使用简单的Token认证机制，后续版本将支持JWT认证。

### 分页参数

支持分页的API都接受以下查询参数：
- `page`: 页码（从1开始）
- `page_size`: 每页数量（最大100）

### 数据验证

所有输入数据都会进行验证，包括：
- 必填字段检查
- 数据类型验证
- 格式验证（邮箱、手机号等）
- 业务规则验证

### 性能优化

- 数据库查询使用索引优化
- 支持批量操作
- 响应数据压缩
- 连接池管理