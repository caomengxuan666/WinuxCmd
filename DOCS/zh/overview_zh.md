# WinuxCmd: Windows兼容的Linux命令集

## 概述

WinuxCmd是一个轻量级的Windows可执行程序，为Windows系统提供了流行Linux命令的兼容性支持。该项目旨在解决AI模型在Windows环境下执行Linux命令时Windows不支持的问题。

## 核心定位与目标

### 项目核心

为Windows (PowerShell/CMD) 提供原生、轻量级、与AI无关的兼容Linux命令集，并预留跨平台扩展空间。

### 量化目标

- 支持20+个高频Linux命令及其核心参数（与Linux行为1:1兼容）
- 组合主程序 `winuxcmd.exe` 大小 ~400KB（静态）/ ~60KB（动态）
- 单个命令可执行文件（如 `ls.exe`）每个文件大小 ~400KB（硬链接）
- 纯Windows API实现，无第三方库依赖
- 使用C++23现代模块系统架构开发
- 支持多种构建模式：DEV、RELEASE、DEBUG_RELEASE

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

## 架构设计

### 基于管道的架构

项目采用**基于管道的架构**，将命令处理分离为不同阶段，提供更好的模块化和可测试性：

- **管道组件**：命令实现为一系列管道组件
- **CommandContext**：提供类型安全的命令选项和参数访问
- **声明式选项定义**：使用 constexpr 数组定义命令选项

```cpp
// 基于管道架构示例
namespace ls_pipeline {
  namespace cp = core::pipeline;
  
  // 验证参数
  auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>>;
  
  // 主管道
  template<size_t N>
  auto process_command(const CommandContext<N>& ctx) -> cp::Result<std::vector<std::string>>;
}
```

### 命令调度器

中央命令调度器管理命令注册和执行：

- **自动注册**：命令使用 `REGISTER_COMMAND` 宏自行注册
- **双模式执行**：支持 `winuxcmd <命令>` 和 `<命令>.exe` 两种格式
- **高效调度**：使用哈希表进行快速命令查找

```cpp
// 命令注册示例
REGISTER_COMMAND(ls,
                 /* name */ "ls",
                 /* synopsis */ "list directory contents",
                 /* description */ "List information about the FILEs",
                 /* examples */ "  ls -la",
                 /* see_also */ "cp, mv, rm",
                 /* author */ "WinuxCmd Team",
                 /* copyright */ "Copyright © 2026 WinuxCmd",
                 /* options */ LS_OPTIONS
) {
  // 命令实现
}

// 命令执行示例
int exit_code = CommandRegistry::dispatch("ls", args);
```

### CommandContext

`CommandContext` 类提供类型安全的命令选项和参数访问：

- **类型安全的选项访问**：使用 `ctx.get<bool>("--verbose", false)` 访问选项
- **位置参数访问**：通过 `ctx.positionals` 提供对位置参数的访问
- **自动选项解析**：自动处理选项解析和验证

### 管道组件

每个命令都结构化为管道组件：

1. **验证**：验证命令参数和选项
2. **处理**：包含主要命令逻辑
3. **输出**：处理命令输出

这种结构提高了代码组织、可测试性和可维护性。

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
│   │   ├── dispatcher.cppm    # 命令调度器
│   │   ├── cmd_meta.cppm      # 命令元数据
│   │   └── opt.cppm           # 选项解析
│   ├── commands/     # 单独命令实现
│   │   ├── ls.cppm   # ls命令模块
│   │   ├── cat.cppm  # cat命令模块
│   │   └── ...       # 其他命令
│   ├── utils/        # 工具模块
│   │   ├── console.cppm       # 控制台工具
│   │   └── utf8.cppm          # UTF-8处理
│   └── Main/         # 主程序入口
│       └── main.cpp
├── tests/            # 单元测试
├── cmake/            # CMake模块
├── DOCS/             # 文档文件
├── scripts/          # 辅助脚本
├── CMakeLists.txt    # 主CMake配置
├── CMakePresets.json # CMake预设配置
├── Project_Rules.md  # 开发指南
├── README.md         # 项目文档（英文）
└── TODO.md           # 项目待办列表（英文）
```

## 添加新命令

要添加新命令，请按照以下步骤操作：

1. **使用脚手架工具**生成命令模板
2. **更新生成文件中的命令选项**
3. **实现验证和处理的管道组件**
4. **在注册的命令函数中添加命令逻辑**
5. **构建项目**以包含新命令

为新命令 `echo` 示例：

```cpp
// src/commands/echo.cppm
export module cmd:echo;

import std;
import core;
import utils;
namespace fs = std::filesystem;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

// ======================================================
// 常量
// ======================================================
namespace echo_constants {
  // 在此添加常量
}

// ======================================================
// 选项 (constexpr)
// ======================================================

export auto constexpr ECHO_OPTIONS =
    std::array{OPTION("-n", "--no-newline", "不输出尾随换行符"),
               // 在此添加更多选项
              };

// ======================================================
// 管道组件
// ======================================================
namespace echo_pipeline {
   namespace cp=core::pipeline;
  
  // ----------------------------------------------
  // 1. 验证参数
  // ----------------------------------------------
  auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {
    std::vector<std::string> texts;
    for (auto arg : args) {
      texts.push_back(std::string(arg));
    }
    return texts;
  }

  // ----------------------------------------------
  // 2. 主管道
  // ----------------------------------------------
  template<size_t N>
  auto process_command(const CommandContext<N>& ctx)
      -> cp::Result<std::vector<std::string>>
  {
    return validate_arguments(ctx.positionals)
        // 在此添加更多管道步骤
        ;
  }

}

// ======================================================
// 命令注册
// ======================================================

REGISTER_COMMAND(echo,
                 /* name */
                 "echo",

                 /* synopsis */
                 "echo [选项]... [字符串]...",

                 /* description */
                 "显示一行文本。无参数时，输出空行。",

                 /* examples */
                 "  echo Hello World        显示 'Hello World'\n"
                 "  echo -n Hello           显示 'Hello' 无尾随换行符",

                 /* see_also */
                 "printf",

                 /* author */
                 "WinuxCmd Team",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 ECHO_OPTIONS
) {
  using namespace echo_pipeline;

  auto result = process_command(ctx);
  if (!result) {
    cp::report_error(result, L"echo");
    return 1;
  }

  auto texts = *result;

  // 命令逻辑
  for (size_t i = 0; i < texts.size(); ++i) {
    safePrint(utf8_to_wstring(texts[i]));
    if (i != texts.size() - 1) {
      safePrint(L" ");
    }
  }

  if (!ctx.get<bool>("--no-newline", false)) {
    safePrintLn(L"");
  }

  return 0;
}
```

## 构建说明

### 前提条件

- Windows 10/11（64位）
- Microsoft Visual Studio 2022+（支持C++23）
- CMake 3.30+（支持C++23模块）
- Ninja构建系统（推荐）

### 构建命令

```bash
# 创建构建目录
mkdir build
cd build

# 使用构建模式配置CMake
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_MODE=RELEASE

# 构建项目
cmake --build .

# 替代构建模式：
cmake .. -DBUILD_MODE=DEV          # 开发模式（快速增量）
cmake .. -DBUILD_MODE=DEBUG_RELEASE # 发布带调试功能
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
