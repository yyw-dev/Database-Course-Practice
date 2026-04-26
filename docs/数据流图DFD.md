# 高校班级事务管理系统 - 数据流图（DFD）

## 目录
- [Level 0：系统上下文图](#level-0系统上下文图)
- [Level 1：系统整体数据流](#level-1系统整体数据流)
- [Level 2：各模块详细流程](#level-2各模块详细流程)
- [核心数据存储清单](#核心数据存储清单)

---

## Level 0：系统上下文图

```mermaid
graph TD
    User["👤 用户<br/>(班主任、班长、班委、学生)"]
    System["🎓 班级事务管理系统"]
    DB["💾 数据库"]
    Email["📧 邮件系统"]
    
    User -->|登录、提交数据、查询| System
    System -->|返回数据、页面显示| User
    System -->|读写数据| DB
    System -->|发送通知| Email
    
    style System fill:#4A90E2,color:#fff
    style User fill:#50C878,color:#fff
    style DB fill:#FFB347,color:#fff
    style Email fill:#9B59B6,color:#fff
```

---

## Level 1：系统整体数据流

```mermaid
graph TB
    User["👤 用户"]
    
    Auth["🔐 身份认证"]
    UserMgmt["👥 用户管理"]
    ClassMgmt["🏫 班级信息管理"]
    Notice["📢 通知公告"]
    Learning["📚 学习管理"]
    Finance["💰 班费管理"]
    Activity["🎉 活动管理"]
    Leave["✈️ 请销假管理"]
    Discipline["📋 纪律管理"]
    Vote["🗳️ 评优投票"]
    Report["📊 统计报表"]
    Log["📝 操作日志"]
    
    UserDB[("👤 用户表")]
    ClassDB[("🏫 班级表")]
    NoticeDB[("📢 通知表")]
    LearningDB[("📚 学习表")]
    FinanceDB[("💰 班费表")]
    ActivityDB[("🎉 活动表")]
    LeaveDB[("✈️ 请假表")]
    DisciplineDB[("📋 纪律表")]
    VoteDB[("🗳️ 投票表")]
    LogDB[("📝 日志表")]
    
    User -->|登录| Auth
    Auth -->|验证| UserDB
    Auth -->|返回| User
    
    User -->|管理操作| UserMgmt
    UserMgmt -->|R/W| UserDB
    UserMgmt -->|记录| LogDB
    
    User -->|查看修改| ClassMgmt
    ClassMgmt -->|R/W| ClassDB
    ClassMgmt -->|记录| LogDB
    
    User -->|发布查询| Notice
    Notice -->|R/W| NoticeDB
    Notice -->|记录| LogDB
    
    User -->|维护查询| Learning
    Learning -->|R/W| LearningDB
    Learning -->|记录| LogDB
    
    User -->|记录审核| Finance
    Finance -->|R/W| FinanceDB
    Finance -->|记录| LogDB
    
    User -->|发布报名| Activity
    Activity -->|R/W| ActivityDB
    Activity -->|记录| LogDB
    
    User -->|申请审核| Leave
    Leave -->|R/W| LeaveDB
    Leave -->|记录| LogDB
    
    User -->|登记审核| Discipline
    Discipline -->|R/W| DisciplineDB
    Discipline -->|记录| LogDB
    
    User -->|投票统计| Vote
    Vote -->|R/W| VoteDB
    Vote -->|记录| LogDB
    
    User -->|查看报表| Report
    Report -->|查询| NoticeDB
    Report -->|查询| FinanceDB
    Report -->|查询| ActivityDB
    Report -->|查询| LeaveDB
    Report -->|查询| DisciplineDB
    Report -->|查询| LearningDB
    Report -->|查询| VoteDB
    
    style User fill:#50C878,color:#fff
    style Auth fill:#FF6B6B,color:#fff
    style Report fill:#4ECDC4,color:#fff
    style Log fill:#95A5A6,color:#fff
    
```

---

## Level 2：各模块详细流程

### 6.1 身份认证与用户登录

```mermaid
graph TD
    A["👤 用户输入<br/>学号/账号+密码"] -->|步骤1:验证输入| B{"输入合法?"}
    B -->|否| C["❌ 返回错误提示"]
    B -->|是| D["🔍 查询用户表"]
    D -->|步骤2:匹配| E{"账户<br/>存在?"}
    E -->|否| C
    E -->|是| F["🔐 验证密码摘要"]
    F -->|步骤3| G{"密码<br/>正确?"}
    G -->|否| C
    G -->|是| H["✅ 检查账号状态"]
    H -->|步骤4| I{"账号<br/>停用?"}
    I -->|是| C
    I -->|否| J["🔑 生成会话令牌"]
    J -->|步骤5| K["📝 记录登录日志"]
    K -->|步骤6| L["👤 返回用户信息<br/>班级、角色"]
    
    subgraph "数据存储"
        UserDB["👤 用户表<br/>学号、密码摘要、状态"]
    end
    
    D --> UserDB
    F --> UserDB
    H --> UserDB
    K -->|写入| LogDB["📝 操作日志表"]
    
    style A fill:#E8F4F8
    style C fill:#FFE8E8
    style L fill:#E8F8E8
    style UserDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.2 用户与权限管理

```mermaid
graph TD
    A["班长/班主任"] -->|查询班级成员| B["📋 班级成员列表"]
    B -->|查询| UserDB["👤 用户表"]
    B -->|查询| RoleDB["🔐 角色表"]
    B -->|查询| UserRoleDB["🔗 用户角色表"]
    
    A -->|新增成员| C["填写成员信息"]
    C -->|验证学号唯一| UserDB
    C -->|创建| D["✅ 成员账户创建"]
    D -->|默认角色| UserRoleDB
    D -->|记录| LogDB["📝 日志"]
    
    A -->|分配角色| E["选择成员+角色"]
    E -->|验证权限| RoleDB
    E -->|插入关联| UserRoleDB
    E -->|记录| LogDB
    
    A -->|停用成员| F["选择成员"]
    F -->|更新状态| UserDB
    F -->|记录| LogDB
    
    style A fill:#E8F4F8
    style D fill:#E8F8E8
    style UserDB fill:#FFB347,color:#fff
    style RoleDB fill:#FFB347,color:#fff
    style UserRoleDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.3 通知公告管理

```mermaid
graph TD
    subgraph "发布流程"
        A["班主任/班长"] -->|发布通知| B["📝 填写通知内容"]
        B -->|标题、内容、类型、重要程度| C{"验证<br/>必填字段?"}
        C -->|否| E["❌ 返回错误"]
        C -->|是| D["✅ 创建通知记录"]
        D -->|发布人、时间自动填充| NoticeDB["📢 通知表"]
        D -->|为所有成员创建订阅| ReadDB["📖 通知阅读表"]
        D -->|记录| LogDB["📝 日志表"]
    end
    
    subgraph "阅读流程"
        F["👥 班级成员"] -->|查看通知| G["📋 通知列表"]
        G -->|查询| NoticeDB
        F -->|点击查看详情| H{"已生成<br/>阅读记录?"}
        H -->|否| I["📝 创建阅读记录"]
        H -->|是| J["🔄 更新阅读时间"]
        I -->|记录阅读时间| ReadDB
        J -->|记录阅读时间| ReadDB
        H -->|返回详情| F
    end
    
    subgraph "统计流程"
        K["发布者"] -->|查看统计| L["📊 已读统计"]
        L -->|查询| ReadDB
        L -->|计算已读率| M["返回统计结果"]
    end
    
    subgraph "查询流程"
        N["用户"] -->|按条件搜索| O["按时间/类型/关键字"]
        O -->|多条件查询| NoticeDB
        O -->|返回结果| N
    end
    
    style NoticeDB fill:#FFB347,color:#fff
    style ReadDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.4 班费管理

```mermaid
graph TD
    subgraph "记录流程"
        A["生活/财务委员"] -->|新增班费| B["输入金额、事由、类型"]
        B -->|验证金额>0| C{"金额<br/>≥500?"}
        C -->|是| D["状态:待审核"]
        C -->|否| E["状态:已确认"]
        D -->|创建流水| FinanceDB["💰 班费流水表"]
        E -->|创建流水| FinanceDB
        FinanceDB -->|自动计算| F["更新班费余额"]
        F -->|写入| ClassDB["🏫 班级表"]
        FinanceDB -->|记录| LogDB["📝 日志"]
    end
    
    subgraph "审核流程"
        G["班主任"] -->|查看待审核| H["📋 待审核列表"]
        H -->|查询| FinanceDB
        G -->|审核操作| I{"同意<br/>or<br/>驳回?"}
        I -->|同意| J["状态:已通过"]
        I -->|驳回| K["状态:已驳回"]
        J -->|更新| FinanceDB
        K -->|更新| FinanceDB
        J -->|更新班费余额| ClassDB
        FinanceDB -->|记录| LogDB
    end
    
    subgraph "查看与统计"
        L["班级成员"] -->|查看班费| M["已确认的流水"]
        M -->|查询| FinanceDB
        M -->|显示余额| ClassDB
        N["班长/班主任"] -->|统计分析| O["按时间/类型统计"]
        O -->|聚合查询| FinanceDB
    end
    
    style FinanceDB fill:#FFB347,color:#fff
    style ClassDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.5 活动管理

```mermaid
graph TD
    subgraph "发布流程"
        A["班长/团支书"] -->|发布活动| B["输入活动信息"]
        B -->|验证时间逻辑| C{"报名截止<br/>活动开始<br/>活动结束?"}
        C -->|否| D["❌ 错误提示"]
        C -->|是| E["✅ 创建活动"]
        E -->|状态:报名中| ActivityDB["🎉 活动表"]
        E -->|向成员发送通知| NoticeDB["📢 通知表"]
        E -->|记录| LogDB["📝 日志"]
    end
    
    subgraph "报名流程"
        F["班级成员"] -->|查看活动| G["🎉 活动列表"]
        G -->|查询| ActivityDB
        F -->|点击报名| H{"检查条件<br/>截止时间<br/>人数限制<br/>未报名?"}
        H -->|不符合| I["❌ 无法报名"]
        H -->|符合| J["创建报名记录"]
        J -->|状态:已报名| SignUpDB["📝 活动报名表"]
        J -->|更新活动人数| ActivityDB
    end
    
    subgraph "结果记录流程"
        K["发布者"] -->|活动结束| L["输入总结信息"]
        L -->|参与人数、总结、附件| M["更新活动状态"]
        M -->|状态:已结束| ActivityDB
        M -->|记录| LogDB
    end
    
    subgraph "统计流程"
        N["发布者"] -->|查看统计| O["报名人数/参与率"]
        O -->|查询| SignUpDB
        O -->|返回成员名单| N
    end
    
    style ActivityDB fill:#FFB347,color:#fff
    style SignUpDB fill:#FFB347,color:#fff
    style NoticeDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.6 请销假管理

```mermaid
graph TD
    subgraph "申请流程"
        A["学生"] -->|提交请假| B["填写请假信息"]
        B -->|类型、时间、原因、证明| C{"验证<br/>结束时间<br/>开始时间?"}
        C -->|否| D["❌ 错误提示"]
        C -->|是| E["✅ 创建申请"]
        E -->|状态:待审核| LeaveDB["✈️ 请假申请表"]
        E -->|向班委发送通知| NoticeDB["📢 通知表"]
        E -->|记录| LogDB["📝 日志"]
    end
    
    subgraph "审核流程"
        F["班委/班主任"] -->|查看待审核| G["📋 待审核列表"]
        G -->|查询| LeaveDB
        F -->|审核操作| H{"通过<br/>or<br/>驳回?"}
        H -->|通过| I["状态:已通过"]
        H -->|驳回| J["状态:已驳回"]
        I -->|更新| LeaveDB
        J -->|更新| LeaveDB
        I -->|向学生发通知| NoticeDB
        J -->|向学生发通知| NoticeDB
        LeaveDB -->|记录| LogDB
    end
    
    subgraph "销假流程"
        K["学生"] -->|假期结束| L["查看已通过申请"]
        L -->|查询| LeaveDB
        K -->|提交销假| M{"验证<br/>销假时间<br/>请假结束时间?"}
        M -->|否| N["❌ 错误提示"]
        M -->|是| O["✅ 创建销假记录"]
        O -->|状态:已销假| LeaveDB
        O -->|记录| LogDB
    end
    
    subgraph "统计查询"
        P["班长/班主任"] -->|统计分析| Q["按学生/类型/时间"]
        Q -->|多维度查询| LeaveDB
        Q -->|返回统计数据| P
    end
    
    style LeaveDB fill:#FFB347,color:#fff
    style NoticeDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.7 纪律管理

```mermaid
graph TD
    subgraph "登记流程"
        A["纪律委员"] -->|新增纪律记录| B["选择学生+类型"]
        B -->|考勤/违纪/奖惩| C["输入发生时间、地点、说明"]
        C -->|验证时间有效| D{"重大<br/>处分?"}
        D -->|是| E["状态:待审核"]
        D -->|否| F["状态:已处理"]
        E -->|创建记录| DisciplineDB["📋 纪律记录表"]
        F -->|创建记录| DisciplineDB
        DisciplineDB -->|记录| LogDB["📝 日志"]
    end
    
    subgraph "审核流程"
        G["班主任"] -->|查看待审核| H["📋 待审核列表"]
        H -->|查询| DisciplineDB
        G -->|审核操作| I{"通过<br/>or<br/>驳回?"}
        I -->|通过| J["状态:已处理"]
        I -->|驳回| K["状态:已驳回"]
        J -->|更新| DisciplineDB
        K -->|更新| DisciplineDB
        DisciplineDB -->|记录| LogDB
    end
    
    subgraph "查看流程"
        L["普通成员"] -->|查看纪律| M["仅查看本人记录"]
        M -->|隐私保护| DisciplineDB
        N["班长/班主任"] -->|查看纪律| O["查看全班记录"]
        O -->|查询| DisciplineDB
    end
    
    subgraph "统计流程"
        P["班长/班主任"] -->|统计分析| Q["考勤/违纪/奖励统计"]
        Q -->|聚合查询| DisciplineDB
        Q -->|返回排名| P
    end
    
    style DisciplineDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.8 学习管理

```mermaid
graph TD
    subgraph "课程管理"
        A["学习委员"] -->|管理课程| B["新增/修改/删除"]
        B -->|输入课程信息| C["验证课程名唯一"]
        C -->|创建| CourseDB["📚 课程表"]
        C -->|记录| LogDB["📝 日志"]
    end
    
    subgraph "课表管理"
        D["学习委员"] -->|管理课表| E["选择课程+班级"]
        E -->|输入时间、地点| F["验证时间有效"]
        F -->|创建| ScheduleDB["📅 课表表"]
        F -->|记录| LogDB
    end
    
    subgraph "课表查询"
        G["班级成员"] -->|查看课表| H["按班级查询"]
        H -->|查询| ScheduleDB
        H -->|支持筛选和导出| G
    end
    
    subgraph "成绩录入"
        I["学习委员"] -->|录入成绩| J["选择课程+考试类型"]
        J -->|批量输入| K{"验证<br/>学号、成绩<br/>有效性?"}
        K -->|否| L["❌ 显示错误"]
        K -->|是| M["✅ 创建成绩记录"]
        M -->|或导入文件| N["解析+验证+插入"]
        M -->|写入| GradeDB["📖 成绩表"]
        N -->|写入| GradeDB
        GradeDB -->|记录| LogDB
    end
    
    subgraph "成绩查看与统计"
        O["学生"] -->|查看成绩| P["个人成绩列表"]
        P -->|查询| GradeDB
        Q["班长/班主任"] -->|统计分析| R["课程/学生维度统计"]
        R -->|平均分、及格率等| S["聚合查询"]
        S -->|查询| GradeDB
        S -->|返回报表| Q
    end
    
    style CourseDB fill:#FFB347,color:#fff
    style ScheduleDB fill:#FFB347,color:#fff
    style GradeDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.9 评优投票管理

```mermaid
graph TD
    subgraph "项目创建"
        A["班长/班主任"] -->|创建项目| B["填写项目信息"]
        B -->|名称、规则、时间段| C{"验证<br/>时间逻辑?"}
        C -->|否| D["❌ 错误提示"]
        C -->|是| E["✅ 创建项目"]
        E -->|状态:报名中| ProjectDB["🗳️ 评优项目表"]
        E -->|记录| LogDB["📝 日志"]
    end
    
    subgraph "候选人管理"
        F["学生"] -->|自行报名| G["输入报名理由"]
        G -->|创建候选人| H["状态:待审核"]
        H -->|写入| CandidateDB["👤 候选人表"]
        I["班长/班主任"] -->|管理员提名| J["选择学生"]
        J -->|创建候选人| H
        K["班主任"] -->|审核候选人| L["查看待审核"]
        L -->|通过/驳回| CandidateDB
        CandidateDB -->|记录| LogDB
    end
    
    subgraph "投票流程"
        M["班级成员"] -->|参与投票| N{"投票时间<br/>有效?"}
        N -->|否| O["❌ 不在投票时间"]
        N -->|是| P{"已投票?"}
        P -->|是| Q["❌ 已投票"]
        P -->|否| R["显示候选人列表"]
        R -->|选择候选人| S{"单选<br/>or<br/>多选?"}
        S -->|单选| T["确认选择"]
        S -->|多选| U["选择多个"]
        T -->|创建投票记录| VoteDB["📝 投票记录表"]
        U -->|创建投票记录| VoteDB
        VoteDB -->|记录| LogDB
    end
    
    subgraph "结果统计与公示"
        V["班长/班主任"] -->|投票结束| W["进入统计页面"]
        W -->|查询投票| VoteDB
        W -->|聚合统计| X["得票数、排名、参与率"]
        Y["班主任"] -->|审核结果| Z["最终确认"]
        Z -->|创建评优结果| ProjectDB
        Z -->|发送通知| NoticeDB["📢 通知表"]
        Z -->|记录| LogDB
        AA["班级成员"] -->|查看结果| AB["查看最终名单"]
        AB -->|查询| ProjectDB
    end
    
    style ProjectDB fill:#FFB347,color:#fff
    style CandidateDB fill:#FFB347,color:#fff
    style VoteDB fill:#FFB347,color:#fff
    style NoticeDB fill:#FFB347,color:#fff
    style LogDB fill:#FFB347,color:#fff
```

### 6.10 统计报表

```mermaid
graph TD
    A["班长/班主任"] -->|查看报表| B["选择报表类型"]
    
    B -->|班费统计| C["💰 班费报表"]
    C -->|查询| FinanceDB["班费流水表"]
    C -->|按时间/类型分析| C
    C -->|返回趋势图表| A
    
    B -->|通知统计| D["📢 通知报表"]
    D -->|查询| NoticeDB["通知表"]
    D -->|查询| ReadDB["通知阅读表"]
    D -->|计算已读率| D
    D -->|返回成员名单| A
    
    B -->|活动统计| E["🎉 活动报表"]
    E -->|查询| ActivityDB["活动表"]
    E -->|查询| SignUpDB["活动报名表"]
    E -->|统计参与率| E
    E -->|返回分析数据| A
    
    B -->|请假统计| F["✈️ 请假报表"]
    F -->|查询| LeaveDB["请假申请表"]
    F -->|按学生/类型统计| F
    F -->|返回排名| A
    
    B -->|纪律统计| G["📋 纪律报表"]
    G -->|查询| DisciplineDB["纪律记录表"]
    G -->|缺勤率、违纪统计| G
    G -->|返回排名| A
    
    B -->|成绩统计| H["📚 成绩报表"]
    H -->|查询| GradeDB["成绩表"]
    H -->|课程/学生维度统计| H
    H -->|返回分布分析| A
    
    style FinanceDB fill:#FFB347,color:#fff
    style NoticeDB fill:#FFB347,color:#fff
    style ReadDB fill:#FFB347,color:#fff
    style ActivityDB fill:#FFB347,color:#fff
    style SignUpDB fill:#FFB347,color:#fff
    style LeaveDB fill:#FFB347,color:#fff
    style DisciplineDB fill:#FFB347,color:#fff
    style GradeDB fill:#FFB347,color:#fff
```

### 6.11 操作日志

```mermaid
graph TD
    A["系统关键操作"] -->|自动触发| B["创建日志记录"]
    
    A -->|用户管理操作| C["新增/修改/停用"]
    A -->|班级信息修改| D["编辑基本信息"]
    A -->|通知发布/删除| E["发布/取消"]
    A -->|班费操作| F["记录/审核"]
    A -->|活动管理| G["发布/总结"]
    A -->|请假审批| H["申请/审核/销假"]
    A -->|纪律记录| I["登记/审核"]
    A -->|评优投票| J["创建/投票/公示"]
    A -->|成绩录入| K["新增/导入/修改"]
    
    C -->|记录| B
    D -->|记录| B
    E -->|记录| B
    F -->|记录| B
    G -->|记录| B
    H -->|记录| B
    I -->|记录| B
    J -->|记录| B
    K -->|记录| B
    
    B -->|操作人、时间、对象、类型、详情、结果| LogDB["📝 操作日志表"]
    
    LogDB -->|班主任查询| L["按操作人筛选"]
    LogDB -->|班主任查询| M["按操作类型筛选"]
    LogDB -->|班主任查询| N["按时间范围筛选"]
    LogDB -->|班主任查询| O["按结果(成功/失败)筛选"]
    
    L -->|聚合分析| P["操作统计报表"]
    M -->|聚合分析| P
    N -->|聚合分析| P
    O -->|聚合分析| P
    
    P -->|返回| L
    
    style LogDB fill:#FFB347,color:#fff
```

---

## 核心数据存储清单

| 序号 | 数据存储 | 标识符 | 主要数据 | 来源 | 使用者 |
|-----|--------|--------|--------|------|--------|
| 1 | 👤 用户表 | D1 | 学号、姓名、性别、手机、邮箱、密码、状态 | 班长/班主任 | 全系统 |
| 2 | 🔐 角色表 | D2 | 角色名称、权限说明 | 班长/班主任 | 权限控制模块 |
| 3 | 🔗 用户角色表 | D3 | 用户编号、角色编号、授权时间 | 班长/班主任 | 权限验证 |
| 4 | 🏫 班级表 | D4 | 班级编号、名称、学院、专业、班主任、余额 | 班长/班主任 | 全系统 |
| 5 | 📢 通知表 | D5 | 通知编号、标题、内容、发布人、时间、类型、重要程度 | 班主任/班长 | 通知模块 |
| 6 | 📖 通知阅读表 | D6 | 通知编号、用户编号、阅读时间 | 系统自动 | 统计模块 |
| 7 | 📚 课程表 | D7 | 课程编号、名称、教师、类型、学分 | 学习委员 | 学习模块 |
| 8 | 📅 课表表 | D8 | 课表编号、课程编号、班级编号、时间、地点 | 学习委员 | 课表查询 |
| 9 | 📖 成绩表 | D9 | 成绩编号、学生编号、课程编号、成绩、考试类型、录入人 | 学习委员 | 成绩统计 |
| 10 | 💰 班费流水表 | D10 | 流水编号、收支类型、金额、事由、经办人、时间、凭证、审核状态 | 财务委员 | 班费模块 |
| 11 | 🎉 活动表 | D11 | 活动编号、名称、地点、时间、报名截止、人数限制、状态 | 班长/团支书 | 活动模块 |
| 12 | 📝 活动报名表 | D12 | 活动编号、用户编号、报名时间、状态 | 班级成员 | 活动统计 |
| 13 | ✈️ 请假申请表 | D13 | 请假编号、申请人、类型、时间、原因、证明、审核状态 | 学生 | 请假模块 |
| 14 | 📝 销假记录表 | D14 | 销假编号、请假编号、销假时间、状态 | 学生 | 请假统计 |
| 15 | 📋 纪律记录表 | D15 | 记录编号、学生编号、类型、发生时间、地点、说明、审核状态 | 纪律委员 | 纪律模块 |
| 16 | 🗳️ 评优项目表 | D16 | 项目编号、名称、说明、时间段、投票规则、创建人、状态 | 班长/班主任 | 投票模块 |
| 17 | 👤 候选人表 | D17 | 项目编号、用户编号、推荐理由、审核状态 | 学生/班长 | 投票模块 |
| 18 | 📝 投票记录表 | D18 | 项目编号、投票人、候选人编号、投票时间 | 班级成员 | 统计分析 |
| 19 | 📝 操作日志表 | D19 | 日志编号、操作人、操作对象、操作类型、操作时间、详情、结果 | 系统自动 | 审计追溯 |
| 20 | 🏫 班级成员表 | D20 | 班级编号、用户编号、加入时间、成员状态 | 班长/班主任 | 成员查询 |

---

## 关键数据流说明

### 时间检查与验证规则

| 数据流 | 验证规则 | 触发时机 |
|--------|---------|---------|
| 请假申请 | 结束时间 > 开始时间 | 提交时 |
| 销假记录 | 销假时间 > 请假结束时间 | 销假时 |
| 活动报名 | 报名时间 ≤ 报名截止时间 < 活动开始时间 < 活动结束时间 | 报名和活动创建时 |
| 投票 | 当前时间在投票开始和结束时间内 | 投票时 |
| 班费审核 | 金额 ≥ 500元为大额支出 | 记录创建时 |

### 状态转移规则

| 业务对象 | 状态流转 | 说明 |
|---------|---------|------|
| 班费流水 | 待审核 → 已通过/已驳回 → 已确认 | 大额支出需班主任审核 |
| 请假申请 | 待审核 → 已通过/已驳回 → 已销假 | 审核通过后才可销假 |
| 纪律记录 | 待审核 → 已处理/已驳回 | 重大处分需班主任审核 |
| 候选人 | 待审核 → 已通过/已驳回 | 班主任审核候选人资格 |
| 活动 | 报名中 → 进行中 → 已结束 | 自动或手动状态转移 |
| 通知 | 正常 → 已过期/已删除 | 时间自动过期或手动删除 |

### 权限隔离与数据访问

| 模块 | 班主任 | 班长 | 班委 | 普通成员 |
|-----|-------|------|------|---------|
| 用户管理 | R/W | R/W | - | - |
| 班级信息 | R/W | R/W | R | R |
| 通知发布 | R/W | R/W | R/W（授权）| R |
| 班费审核 | R/W | R | - | R（已确认）|
| 请假审核 | R/W | R/W | R/W | 仅自己 |
| 评优最终审核 | R/W | - | - | 仅查看公示 |
| 纪律审核 | R/W | - | - | 仅自己 |
| 操作日志 | R/W | R | - | - |

---

## 系统数据流总结

- **用户入口**：登录（身份认证）→ 权限验证 → 功能菜单
- **业务流程**：用户操作 → 数据验证 → 数据存储 → 自动日志记录 → 返回结果
- **查询统计**：多维度查询 → 聚合计算 → 报表生成 → 导出
- **审计追溯**：所有关键操作自动记录日志 → 班主任查询分析 → 数据追踪

---

*此DFD文档与《功能需求细化.md》对应，可作为数据库设计和业务流程分析的基础。*
