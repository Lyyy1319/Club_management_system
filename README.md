# 社团管理系统 (Club Management System)

一个基于 C 语言开发的社团管理系统，支持用户认证、社团管理、财务管理和活动管理等功能。

## 功能特性

### 用户管理
- 用户注册与登录（密码加密存储）
- 多角色支持：
  - **普通成员** (ROLE_MEMBER)
  - **社长** (ROLE_PRESIDENT)
  - **管理员** (ROLE_ADMIN)
  - **干事** (ROLE_OFFICER)

### 社团管理
- 创建/编辑/删除社团
- 社团审批机制
- 成员管理（添加/移除成员、角色变更）
- 社团模糊搜索
- 多维度排序（按ID、名称、余额）

### 财务管理
- 收支记录管理
- 账目验证
- 资金申请功能
- 财务报表生成
- 数据关联性验证

### 活动管理
- 活动发布与审批
- 活动时间冲突检测
- 活动报名功能
- 按社团查看活动列表

## 项目结构

```
Club_management_system/
├── main.c              # 主程序入口，菜单与流程控制
├── auth.c / auth.h     # 用户认证模块
├── club.c / club.h     # 社团管理模块
├── finance.c / finance.h   # 财务管理模块
├── activity.c / activity.h # 活动管理模块
├── utils.c / utils.h   # 工具函数
├── models.h            # 数据模型定义
├── Makefile            # 编译配置文件
├── users.txt           # 用户数据存储
├── clubs.txt           # 社团数据存储
├── members.txt         # 成员数据存储
├── transactions.txt    # 交易记录存储
├── activities.txt      # 活动数据存储
├── participants.txt    # 活动参与者存储
└── fund_requests.txt   # 资金申请记录存储
```

## 编译与运行

### 环境要求
- GCC 编译器（支持 C11 标准）
- Windows / Linux / macOS

### 使用 Makefile 编译（推荐）

```bash
# 编译项目
make

# 运行程序
./club_system      # Linux/macOS
club_system.exe    # Windows

# 清理编译文件
make clean
```

### 手动编译

```bash
gcc -Wall -Wextra -g -std=c11 -o club_system.exe main.c auth.c club.c finance.c utils.c activity.c
```

## 数据存储

系统使用文本文件存储数据，各文件格式如下：

| 文件 | 说明 |
|------|------|
| `users.txt` | 用户账号信息 |
| `clubs.txt` | 社团基本信息 |
| `members.txt` | 社团成员关系 |
| `transactions.txt` | 财务交易记录 |
| `activities.txt` | 活动信息 |
| `participants.txt` | 活动参与者 |
| `fund_requests.txt` | 资金申请记录 |

## 使用说明

1. **首次运行**：系统会自动创建必要的数据文件
2. **管理员账号**：首次使用请先注册管理员账号
3. **创建社团**：注册用户后可申请创建社团，需管理员审批
4. **财务管理**：社长可管理社团财务，记录收支
5. **活动管理**：社长发布活动，成员可报名参加

## 安全特性

- 密码使用异或加密存储
- 基于角色的访问控制
- 输入验证与缓冲区溢出保护

## 开发信息

- **语言**: C (C11 标准)
- **编译器**: GCC
- **平台**: 跨平台支持 (Windows/Linux/macOS)

## 许可证

本项目仅供学习交流使用。
