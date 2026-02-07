# 脚手架和DSL文档

## 脚手架工具概述

脚手架工具旨在简化 WinuxCmd 项目中新命令模块的创建过程。它生成遵循项目约定和最佳实践的样板代码，使开发者能够专注于实现命令的核心功能。该工具现在生成使用新的基于管道的架构的代码。

### 主要特性

- **自动代码生成**：为新命令创建所有必要的文件和样板代码
- **一致的结构**：确保所有命令遵循相同的结构和约定
- **与 DSL 集成**：生成使用项目 DSL 进行选项处理和命令注册的代码
- **错误处理**：包含使用 `core::pipeline::Result` 的基本错误处理模式
- **文档模板**：为命令文档提供占位符
- **管道架构**：生成遵循新的基于管道的架构的代码

### 使用方法

要使用脚手架工具创建新的命令模块，请运行以下命令：

```bash
# 从项目根目录
python scripts/scaffold.py <command_name> [options]

# 示例：创建一个名为 "example" 的新命令
python scripts/scaffold.py example
```

### 生成的文件

当您运行脚手架工具时，它会生成以下文件：

1. **`src/cmd/<command_name>.cppm`**：主命令模块文件

### 生成的代码结构

生成的代码遵循以下结构，实现了新的基于管道的架构：

```cpp
// 主模块文件 (<command_name>.cppm)
export module cmd.<command_name>;

import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

namespace <command_name>_constants {
    // 命令特定常量
}

// 选项定义
export auto constexpr <COMMAND_NAME>_OPTIONS =
    std::array{
        // 选项定义在这里
    };

namespace <command_name>_pipeline {
    namespace cp = core::pipeline;

    // 验证参数
    auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {
        std::vector<std::string> validated_args;
        for (auto arg : args) {
            validated_args.push_back(std::string(arg));
        }
        return validated_args;
    }

    // 处理命令
    template<size_t N>
    auto process_command(const CommandContext<N>& ctx)
        -> cp::Result<std::vector<std::string>>
    {
        auto args_result = validate_arguments(ctx.positionals);
        if (!args_result) {
            return args_result.error();
        }
        return *args_result;
    }

}

REGISTER_COMMAND(<command_name>,
                 /* name */
                 "<command_name>",

                 /* synopsis */
                 "<command_name> [options] [arguments]",

                 /* description */
                 "<Command description>",

                 /* examples */
                 "<Command examples>",

                 /* see_also */
                 "<See also commands>",

                 /* author */
                 "WinuxCmd Team",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 <COMMAND_NAME>_OPTIONS
) {
    using namespace <command_name>_pipeline;

    auto result = process_command(ctx);
    if (!result) {
        cp::report_error(result, L"<command_name>");
        return 1;
    }

    auto args = *result;

    // 命令实现在这里

    return 0;
}
```

## DSL 设计概述

WinuxCmd 项目使用领域特定语言（DSL）来简化命令实现、选项处理和命令注册。DSL 通过宏和模板元编程实现，提供了一种清晰的声明式语法。DSL 已更新以支持新的基于管道的架构。

### 设计原则

- **声明式语法**：专注于命令的功能，而不是如何解析选项
- **类型安全**：使用 C++ 类型系统在编译时捕获错误
- **最小样板代码**：减少重复代码
- **一致性**：确保所有命令遵循相同的模式
- **可扩展性**：允许自定义选项类型和处理
- **管道架构**：支持新的基于管道的命令处理

### 核心组件

#### 1. 选项定义

`OPTION` 宏用于定义命令选项：

```cpp
OPTION("<short_option>", "<long_option>", "<description>")
```

示例：

```cpp
OPTION("-l", "--list", "使用长列表格式")
```

#### 2. 命令注册

`REGISTER_COMMAND` 宏用于向系统注册命令：

```cpp
REGISTER_COMMAND(
    <command_name>,        // 命令标识符
    "<command_name>",      // 命令名称
    "<synopsis>",          // 命令概要
    "<description>",       // 命令描述
    "<examples>",          // 命令示例
    "<see_also>",          // 参见命令
    "<author>",            // 作者
    "<copyright>",         // 版权
    <OPTIONS_ARRAY>         // 选项数组
) {
    // 命令实现在这里
}
```

#### 3. CommandContext

`CommandContext` 类用于以类型安全的方式访问解析后的选项：

```cpp
// 获取布尔选项（默认值：false）
bool option_value = ctx.get<bool>("--option-name", false);

// 获取字符串选项（默认值：""）
std::string option_value = ctx.get<std::string>("--option-name", "");

// 获取整数选项（默认值：0）
int option_value = ctx.get<int>("--option-name", 0);
```

#### 4. 管道组件

DSL 支持创建用于命令处理的管道组件：

```cpp
namespace <command_name>_pipeline {
    namespace cp = core::pipeline;

    // 验证参数
    auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {
        // 验证逻辑在这里
    }

    // 处理命令
    template<size_t N>
    auto process_command(const CommandContext<N>& ctx) -> cp::Result<ReturnType> {
        // 处理逻辑在这里
    }

}
```

### 使用 DSL 与管道架构

要将 DSL 与新的基于管道的架构一起使用，请按照以下步骤操作：

1. **导入必要的模块**：导入 `core` 和 `utils` 模块
2. **定义常量**：为命令特定的常量创建命名空间
3. **定义选项**：使用 `OPTION` 宏定义命令选项
4. **创建管道组件**：实现验证和处理函数
5. **实现命令逻辑**：使用管道组件编写主命令逻辑
6. **注册命令**：使用 `REGISTER_COMMAND` 宏注册命令

### 示例：完整的命令实现（管道架构）

```cpp
// echo.cppm
export module cmd.echo;

import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

namespace echo_constants {
    // 命令特定常量
}

// 选项定义
export auto constexpr ECHO_OPTIONS =
    std::array{
        OPTION("-n", "--no-newline", "不输出尾随换行符"),
        OPTION("-e", "--escape", "启用反斜杠转义的解释"),
    };

namespace echo_pipeline {
    namespace cp = core::pipeline;

    // 验证参数
    auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {
        std::vector<std::string> validated_args;
        for (auto arg : args) {
            validated_args.push_back(std::string(arg));
        }
        return validated_args;
    }

    // 从参数构建文本
    auto build_text(const std::vector<std::string>& args) -> std::string {
        std::string text;
        for (size_t i = 0; i < args.size(); ++i) {
            text += args[i];
            if (i < args.size() - 1) {
                text += " ";
            }
        }
        return text;
    }

    // 处理命令
    template<size_t N>
    auto process_command(const CommandContext<N>& ctx)
        -> cp::Result<std::string>
    {
        auto args_result = validate_arguments(ctx.positionals);
        if (!args_result) {
            return args_result.error();
        }
        auto args = *args_result;
        return build_text(args);
    }

}

REGISTER_COMMAND(echo,
                 /* name */
                 "echo",

                 /* synopsis */
                 "echo [选项]... [字符串]...",

                 /* description */
                 "显示一行文本。",

                 /* examples */
                 "  echo Hello World      显示 'Hello World'\n"
                 "  echo -n Hello         显示 'Hello' 不带尾随换行符\n"
                 "  echo -e Hello\\nWorld  显示 'Hello' 和 'World' 在单独的行上",

                 /* see_also */
                 "cat, printf",

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

    auto text = *result;

    // 使用 CommandContext 获取选项
    bool no_newline = ctx.get<bool>("--no-newline", false);
    bool escape = ctx.get<bool>("--escape", false);

    // 如果启用，处理转义序列
    if (escape) {
        // 实现转义序列处理
    }

    // 输出结果
    std::cout << text;
    if (!no_newline) {
        std::cout << std::endl;
    }

    return 0;
}
```

### 高级特性

#### 自定义选项类型

DSL 通过模板特化支持自定义选项类型：

```cpp
// 整数值的自定义选项类型
struct IntOption {
    int value;
};

// 为 IntOption 特化 OptionType
template<>
struct OptionType<IntOption> {
    static constexpr auto name = "int";
    static constexpr auto parse = [](const std::string_view& value) -> std::optional<IntOption> {
        try {
            return IntOption{std::stoi(std::string(value))};
        } catch (...) {
            return std::nullopt;
        }
    };
};

// 使用自定义选项类型
OPTION("-n", "--number", "指定一个数字", OptionType<IntOption>{})
```

#### 子命令

DSL 通过嵌套命令注册支持子命令：

```cpp
// 注册主命令
REGISTER_COMMAND(
    git,
    "git",
    "git [子命令] [选项]",
    "Git 版本控制系统",
    "git status\ngit commit",
    "",
    "WinuxCmd Team",
    "Copyright © 2026 WinuxCmd",
    GIT_OPTIONS
) {
    // 主 git 命令实现
    return 0;
};

// 注册子命令
REGISTER_COMMAND(
    git_status,
    "git status",
    "git status [选项]",
    "显示工作树状态",
    "git status",
    "git",
    "WinuxCmd Team",
    "Copyright © 2026 WinuxCmd",
    GIT_STATUS_OPTIONS
) {
    // git status 实现
    return 0;
};
```

## 最佳实践

### 对于脚手架工具

1. **使用描述性命令名称**：选择能清楚描述命令功能的命令名称
2. **遵循命名约定**：命令名称使用小写，常量使用大写
3. **保持选项简单**：从最基本的选项开始，稍后添加更多
4. **详细记录**：填写所有文档占位符
5. **尽早测试**：在添加复杂逻辑之前测试生成的命令结构
6. **遵循管道架构**：使用生成的管道组件结构

### 对于 DSL 使用

1. **使用 OPTION 宏**：始终使用 `OPTION` 宏进行选项定义
2. **遵循选项命名**：短选项使用 `-`，长选项使用 `--`
3. **提供清晰描述**：为选项编写清晰、简洁的描述
4. **正确注册命令**：填写 `REGISTER_COMMAND` 宏的所有字段
5. **优雅处理错误**：使用 `core::pipeline::Result` 进行错误处理
6. **使用 CommandContext**：使用 `CommandContext` 进行类型安全的选项访问
7. **实现管道组件**：将验证和处理逻辑分离到管道组件中

## 故障排除

### 常见问题

1. **选项解析错误**：检查选项定义是否正确，以及是否提供了必要的参数
2. **命令注册失败**：确保 `REGISTER_COMMAND` 宏的所有字段都已填写
3. **编译错误**：检查是否导入了所有必要的模块，以及代码是否遵循 C++23 语法
4. **管道组件错误**：确保管道组件返回 `core::pipeline::Result` 并正确处理错误

### 调试提示

1. **启用调试输出**：将 `WINUX_DEBUG` 环境变量设置为 `1` 以启用调试输出
2. **检查生成的代码**：检查生成的代码是否有语法错误或缺少组件
3. **使用简单命令测试**：从简单的命令实现开始，然后逐步增加复杂性
4. **参考现有命令**：查看现有命令的实现方式，尤其是作为参考的 `echo.cppm`

## 结论

脚手架工具和 DSL 是 WinuxCmd 项目的强大工具，简化了命令的创建和维护过程。通过新的基于管道的架构，命令现在更加模块化、易于测试和组织。通过遵循本文档中描述的模式和最佳实践，开发者可以创建一致、结构良好的命令，与项目架构无缝集成。

有关更多信息，请参考项目的 README.md 文件和现有的命令实现，尤其是作为新的基于管道的架构参考的 `echo.cppm`。