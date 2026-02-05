# WinuxCmd: Windows兼容的Linux命令集

## 概述

WinuxCmd是一个轻量级的Windows可执行程序，为Windows系统提供了流行Linux命令的兼容性支持。该项目旨在解决AI模型在Windows环境下执行Linux命令时Windows不支持的问题。

## 核心定位与目标

### 项目核心

为Windows (PowerShell/CMD) 提供原生、轻量级、与AI无关的兼容Linux命令集，并预留跨平台扩展空间。

### 量化目标

- 支持数百个个高频Linux命令及其核心参数（与Linux行为1:1兼容）
- 组合主程序 `winuxcmd.exe` 大小 < 1MB
- 单个命令可执行文件（如 `ls.exe`）每个文件大小 ≤ 2KB
- 纯Windows API实现，无第三方库依赖
- 使用C++23现代模块系统架构开发

## 技术栈

- **编程语言**: C++23（支持C++23的MSVC 2022+）
- **构建工具**: CMake（主要构建系统）
- **编译器**: Microsoft Visual Studio 2022+（MSVC 19.34+）
- **核心库**: Windows API（无第三方依赖）
- **模块系统**: C++23模块（接口/实现文件分离）

## C++23模块实现

本项目展示了现代C++23模块架构，采用分离的接口和实现文件：

### 模块接口（`.cppm`文件）

- 使用 `export module` 定义模块导出
- 使用 `export` 关键字声明公共API
- 示例：`src/core/runtime.cppm`

### 模块实现（`.cpp`文件）

- 使用 `module`（无 `export`）实现模块功能
- 自动访问模块接口声明
- 示例：`src/core/runtime.cpp`

### CMake配置

- 使用CMake的CXX_MODULES支持
- 自动处理模块依赖
- 配置模块输出目录
- 支持MSVC的 `/std:c++latest` 标志

## 功能特性

### 命令兼容性

- 每个命令仅实现Linux核心参数（如 `ls` 支持 `-l/-a/-h/-r`）
- 命令输出格式与Linux原生输出1:1对齐
- 自动Linux路径到Windows路径转换 (/c/Users → C:\Users)
- Linux风格错误信息格式

### 双模式执行

- **独立命令模式**: 每个命令编译为单独的.exe文件（如 `ls.exe`）
- **组合模式**: 主程序 `winuxcmd.exe` 内置所有命令（如 `winuxcmd ls -l`）

### 性能优化

- 禁用RTTI和异常以优化大小
- 优先使用Windows原生API（FindFirstFileW, WriteConsoleA等）
- 优先使用栈内存而非堆分配
- 使用MSVC编译标志进行极致大小优化

## 目录结构

```
winuxcmd/
├── src/
│   ├── core/         # 核心模块实现
│   │   ├── runtime.cppm   # 模块接口
│   │   └── runtime.cpp    # 模块实现
│   └── main.cpp      # 主程序入口
├── DOCS/             # 文档文件
├── CMakeLists.txt    # CMake配置
├── Project_Rules.md  # 开发指南
├── README.md         # 项目文档（英文）
├── TODO.md           # 项目待办列表（英文）
└── DOCS/
    ├── README_zh.md  # 项目文档（中文）
    ├── TODO_zh.md    # 项目待办列表（中文）
    ├── scaffold_and_dsl.md  # 脚手架和DSL设计文档（中文）
```

## 构建说明

### 前提条件

- Windows 10/11（64位）
- Microsoft Visual Studio 2022+（支持C++23）
- CMake 3.26+（支持C++23模块）

### 构建命令

```bash
# 创建构建目录
mkdir build
cd build

# 配置CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# 构建项目
cmake --build . --config Release
```

## 使用示例

### 直接使用（无需激活）

```bash
# 使用winux前缀直接访问命令
winux ls -la
winux grep "pattern" file.txt
winux cp source.txt dest.txt
```

### 激活后使用

```bash
# 激活以直接使用命令
winux activate

# 现在可以直接使用命令
ls -la
grep "pattern" file.txt
cp source.txt dest.txt

# 使用完毕后取消激活
winux deactivate
```

### 组合模式

```bash
# 直接使用主可执行文件
winuxcmd ls -lh
winuxcmd grep "pattern" file.txt
```

## 版本控制

语义化版本控制（如 v1.0.0），格式为 `<major>.<minor>.<patch>`

## 许可证

MIT License

## 贡献

请阅读 Project_Rules.md 了解我们的行为准则和提交拉取请求的流程。
