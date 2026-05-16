# ContactManagementSystem

留守儿童关爱志愿服务积分系统，基于 C++17 和 CMake 实现的 Windows 控制台程序。

本项目面向志愿服务管理场景，支持志愿者账号注册、登录、服务计时、积分获取、积分使用、物资捐赠、账号注销、统计汇总、历史记录查询和 DeepSeek AI 助手等功能。

## 功能概览

- 管理员入口：注册志愿者账号、查询志愿者信息、查看系统统计、查询服务记录、查询积分流水、注销账号、查看帮助说明。
- 用户入口：注册账号、登录账号、开始服务、结束服务、获取积分、使用积分、物资捐赠、注销本人账号、查看本人服务记录、查看本人积分流水、使用 AI 助手。
- 服务计时：记录服务开始和结束时间，并根据服务时长结算积分。
- 积分规则：支持基础积分、新用户倍率、周末倍率、物资捐赠换算积分和积分余额校验。
- 历史查询：支持查看全部服务记录、全部积分流水，也支持按账号筛选。
- 数据持久化：使用 `.ams` 二进制文件保存账号、服务记录和积分流水。
- AI 助手：通过 DeepSeek API 提供可选问答辅助，API Key 从环境变量读取。

## 技术栈

- C++17
- CMake
- CLion
- MinGW
- Windows 控制台
- DeepSeek API

## 运行环境

建议使用以下环境运行：

- Windows
- CLion
- CMake
- MinGW 编译器
- 可选：`curl`，用于调用 DeepSeek API

## 构建与运行

### 使用 CLion

使用 CLion 打开项目根目录，等待 CMake 自动加载完成后，运行 `ContactManagementSystem` 即可。

项目中的数据文件使用相对路径。为了让程序正确读写 `data/` 目录，建议将 CLion 运行配置中的 Working directory 设置为项目根目录：

```text
$ProjectFileDir$
```

### 使用命令行

也可以在项目根目录下使用 CMake 构建：

```bash
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

构建完成后运行生成的 `ContactManagementSystem` 可执行文件。运行时工作目录应保持为项目根目录，或确保程序能访问项目根目录下的 `data/` 文件夹。

## 菜单入口

程序启动后先进入主入口：

```text
1. 管理员登录
2. 用户入口
0. 退出程序
```

管理员登录成功后可使用 0 到 7 的管理员菜单：

```text
1. 注册志愿者账号
2. 查询志愿者账号
3. 统计查询
4. 服务记录查询
5. 积分流水查询
6. 注销账号
7. 使用帮助/规则说明
0. 退出管理员菜单
```

用户入口支持注册和登录。用户登录后可使用 0 到 10 的用户菜单：

```text
1. 开始服务
2. 结束服务
3. 获取积分
4. 使用积分
5. 物资捐赠
6. 注销当前账号
7. 查看本人服务记录
8. 查看本人积分流水
9. AI 助手
10. 使用帮助/规则说明
0. 退出用户菜单
```

## AI 助手配置

AI 助手功能使用 DeepSeek API。API Key 不写入源码，需要通过环境变量 `DEEPSEEK_API_KEY` 配置。

PowerShell 永久配置：

```powershell
[Environment]::SetEnvironmentVariable("DEEPSEEK_API_KEY", "你的 API Key", "User")
```

配置后需要重新打开 CLion 或终端，使环境变量生效。

PowerShell 临时配置：

```powershell
$env:DEEPSEEK_API_KEY="你的 API Key"
```

如果不使用 AI 助手功能，可以跳过 API Key 配置。AI 请求过程中会临时生成 `deepseek_request.json` 和 `deepseek_response.json`，项目已通过 `.gitignore` 忽略这些文件。

## 数据文件

程序运行数据保存在 `data/` 目录下：

```text
data/account.ams
data/tracking.ams
data/money.ams
data/points.ams
```

文件用途：

| 文件 | 说明 |
| --- | --- |
| `data/account.ams` | 保存志愿者账号、密码、状态和积分余额等信息 |
| `data/tracking.ams` | 保存服务开始、结束、时长和结算状态等记录 |
| `data/money.ams` | 保存积分获取、使用和物资捐赠等积分流水 |
| `data/points.ams` | 保留的积分相关数据文件路径 |

仓库中只保留 `data/` 目录本身，不提交真实运行数据。`data/` 目录通过 `.gitkeep` 保留，实际运行生成的 `.ams` 文件已通过 `.gitignore` 忽略：

```gitignore
data/*
!data/.gitkeep
```

## 安全说明

不要将真实 API Key、真实用户数据、本地调试数据或运行生成的 `.ams` 文件提交到版本库。

如果 API Key 曾经出现在代码、文档、截图或提交历史中，应立即在 DeepSeek 开放平台删除或重置该 Key。

请勿提交以下内容：

```text
真实 API Key
真实用户数据
本地调试数据
deepseek_request.json
deepseek_response.json
data/*.ams
```

## 项目结构

```text
.
├── main.cpp
├── menu.cpp
├── menu.h
├── service.cpp
├── service.h
├── account_service.cpp
├── account_service.h
├── account_file.cpp
├── account_file.h
├── tracking_service.cpp
├── tracking_service.h
├── tracking_file.cpp
├── tracking_file.h
├── points_service.cpp
├── points_service.h
├── points_file.cpp
├── points_file.h
├── large_model.cpp
├── large_model.h
├── model.h
├── global.cpp
├── global.h
├── tool.cpp
├── tool.h
├── CMakeLists.txt
├── .gitignore
└── data/
    └── .gitkeep
```

## 当前版本说明

当前版本已完成志愿者账号管理、服务计时、积分管理、物资捐赠、统计查询、历史记录查询、帮助页和 DeepSeek AI 助手等功能。

本项目仍保持 C++ 控制台程序形态，不引入数据库或图形界面，重点展示结构体、链表、动态数组、文件读写、函数拆分、多文件工程和 CMake 构建等基础程序设计能力。
