# ContactManagementSystem

留守儿童关爱系统，基于 C++ 和 CMake 实现的控制台程序。

本项目面向志愿者服务管理场景，支持志愿者注册、信息查询、服务计时、积分获取、积分使用、账号注销、统计查询和 AI 助手等功能。

## 功能

- 注册志愿者账号
- 按账号查询志愿者信息
- 查看所有志愿者信息
- 开始服务与结束服务
- 根据服务时长结算积分
- 手动获取积分
- 使用积分并折算为捐赠金额
- 注销志愿者账号
- 统计查询志愿者服务情况
- AI 助手，调用 DeepSeek V4 Flash

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

## 构建方式

### 使用 CLion 构建

使用 CLion 打开项目根目录，等待 CMake 自动加载完成后，直接运行 `ContactManagementSystem` 即可。

由于项目中的数据文件使用相对路径，建议将 CLion 运行配置中的 Working directory 设置为项目根目录。

在 CLion 中可以设置为：

```text
$ProjectFileDir$
```

### 使用命令行构建

也可以在项目根目录下使用命令行构建：

```bash
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

## AI 助手配置

AI 助手功能使用 DeepSeek API。API Key 不写入源码，需要通过环境变量配置。

### PowerShell 永久配置

```powershell
[Environment]::SetEnvironmentVariable("DEEPSEEK_API_KEY", "你的 API Key", "User")
```

配置后需要重新打开 CLion 或终端，使环境变量生效。

### PowerShell 临时配置

如果只想在当前 PowerShell 会话中临时配置：

```powershell
$env:DEEPSEEK_API_KEY="你的 API Key"
```

如果不使用 AI 助手功能，可以跳过 API Key 配置。

## 数据文件

程序运行数据保存在 `data/` 目录下，例如：

```text
data/account.ams
data/tracking.ams
data/money.ams
```

这些文件用于保存账号信息、服务记录和积分流水。

仓库中只保留 `data/` 目录本身，不提交实际运行数据文件。`data/` 目录通过 `.gitkeep` 文件保留，真实运行数据已通过 `.gitignore` 忽略。

对应的 `.gitignore` 规则为：

```gitignore
data/*
!data/.gitkeep
```

因此，程序运行过程中生成的 `.ams` 数据文件只会保存在本地，不会被提交到 GitHub。

## 安全说明

不要将真实 API Key 写入代码、README、截图或提交历史中。

如果 API Key 曾经被提交到 GitHub，应立即在 DeepSeek 开放平台 删除该API Key。

请勿提交以下内容：

```text
真实 API Key
真实用户数据
本地调试数据
deepseek_request.json
deepseek_response.json
```

## 项目结构

```text
.
├── main.cpp
├── menu.cpp
├── menu.h
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

## 当前版本

v1.0 已完成核心功能。

v1.1 在不改变 `.ams` 数据文件格式的前提下，继续优化控制台使用体验：主菜单新增系统概览，并增加 `11. 服务记录查询`、`12. 积分流水查询`、`13. 使用帮助/规则说明`。查询结果中的账号状态、服务状态和积分流水类型改为中文文本展示，注册、服务、积分、捐赠和注销流程也增加了更明确的成功摘要与失败原因提示。

已实现：

- 志愿者账号管理
- 服务计时
- 积分获取与使用
- 统计查询
- 数据文件保存
- AI 助手调用
