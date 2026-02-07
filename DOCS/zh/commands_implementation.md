# 命令实现状态

本文档跟踪 WinuxCmd 项目中命令的实现状态，该项目现在使用基于管道的架构进行命令处理。

## 分类

### 文件管理

| 命令 | 状态 | 优先级 | 描述 | 参数选项 | 实现说明 |
|------|------|--------|------|----------|----------|
| `ls` | ✅ 已实现 | 高 | 列出目录内容 | `-a, --all`: 不忽略以 . 开头的条目<br>`-l, --list`: 使用长列表格式<br>`-h, --human-readable`: 与 -l 一起使用，以人类可读格式打印大小（例如，1K 234M 2G）<br>`-T, --tabsize`: 假设制表位在每个 COLS 处而不是 8<br>`-w, --width`: 将输出宽度设置为 COLS。0 表示无限制 | 使用管道架构实现 |
| `cat` | ✅ 已实现 | 高 | 连接文件并打印到标准输出 | `-n, --number`: 对所有输出行编号<br>`-b, --number-nonblank`: 对非空输出行编号，覆盖 `-n` | 简单文件读写，带有管道结构 |
| `cp` | ✅ 已实现 | 高 | 复制文件和目录 | `-i, --interactive`: 覆盖前提示<br>`-r, --recursive`: 递归复制目录<br>`-v, --verbose`: 解释正在执行的操作 | 文件系统操作，带有错误处理 |
| `mv` | ✅ 已实现 | 高 | 移动（重命名）文件 | `-i, --interactive`: 覆盖前提示<br>`-v, --verbose`: 解释正在执行的操作 | 文件系统操作，带有错误处理 |
| `rm` | ✅ 已实现 | 高 | 删除文件或目录 | `-f, --force`: 忽略不存在的文件和参数，从不提示<br>`-i, --interactive`: 每次删除前提示<br>`-r, -R, --recursive`: 递归删除目录及其内容 | 文件系统操作，带有错误处理 |
| `mkdir` | ✅ 已实现 | 高 | 创建目录 | `-p, --parents`: 不存在时无错误，必要时创建父目录<br>`-v, --verbose`: 为创建的每个目录打印一条消息 | 文件系统操作，带有错误处理 |
| `rmdir` | ✅ 已实现 | 中 | 删除空目录 | `-p, --parents`: 删除目录及其祖先<br>`-v, --verbose`: 为删除的每个目录打印一条消息 | 文件系统操作，带有错误处理 |
| `touch` | ✅ 已实现 | 中 | 更改文件时间戳或创建空文件 | `-a`: 仅更改访问时间<br>`-m`: 仅更改修改时间<br>`-c, --no-create`: 不创建任何文件 | 文件系统操作，带有错误处理 |
| `ln` | ❌ 未实现 | 低 | 在文件之间创建链接 | `-s, --symbolic`: 创建符号链接而不是硬链接 | 尚未实现 |

### 文本处理

| 命令 | 状态 | 优先级 | 描述 | 参数选项 | 实现说明 |
|------|------|--------|------|----------|----------|
| `echo` | ✅ 已实现 | 高 | 显示一行文本 | `-n`: 不输出尾随换行符<br>`-e`: 启用反斜杠转义的解释 | 使用管道架构实现，作为参考实现 |
| `grep` | ❌ 未实现 | 高 | 打印匹配模式的行 | `-i, --ignore-case`: 忽略大小写区别<br>`-v, --invert-match`: 反转匹配的含义，选择不匹配的行<br>`-n, --line-number`: 用 1 基行号为每行输出添加前缀 | 尚未实现 |
| `sort` | ❌ 未实现 | 中 | 对文本文件的行进行排序 | `-n, --numeric-sort`: 根据字符串数值进行比较<br>`-r, --reverse`: 反转比较结果 | 尚未实现 |
| `wc` | ❌ 未实现 | 中 | 为每个文件打印换行符、单词和字节计数 | `-l, --lines`: 打印换行符计数<br>`-w, --words`: 打印单词计数<br>`-c, --bytes`: 打印字节计数 | 尚未实现 |
| `head` | ❌ 未实现 | 低 | 输出文件的前几部分 | `-n, --lines`: 打印前 N 行而不是前 10 行 | 尚未实现 |
| `tail` | ❌ 未实现 | 低 | 输出文件的最后部分 | `-n, --lines`: 输出最后 N 行而不是最后 10 行 | 尚未实现 |

### 系统信息

| 命令 | 状态 | 优先级 | 描述 | 参数选项 | 实现说明 |
|------|------|--------|------|----------|----------|
| `pwd` | ✅ 已实现 | 高 | 打印工作目录 | 无选项 | 简单调用 GetCurrentDirectory，带有管道结构 |
| `date` | ✅ 已实现 | 高 | 打印或设置系统日期和时间 | `+FORMAT`: 指定输出格式 | 使用 Windows API 进行日期/时间操作 |
| `time` | ❌ 未实现 | 中 | 计时命令 | 无选项 | 尚未实现 |
| `env` | ❌ 未实现 | 中 | 打印环境变量 | 无选项 | 尚未实现 |
| `uname` | ❌ 未实现 | 低 | 打印系统信息 | `-s, --kernel-name`: 打印内核名称<br>`-r, --kernel-release`: 打印内核版本 | 尚未实现 |

### 网络

| 命令 | 状态 | 优先级 | 描述 | 参数选项 | 实现说明 |
|------|------|--------|------|----------|----------|
| `ping` | ❌ 未实现 | 高 | 向网络主机发送 ICMP ECHO_REQUEST | `-c COUNT`: 发送 COUNT 个 ECHO_REQUEST 数据包后停止<br>`-i INTERVAL`: 在发送每个数据包之间等待 INTERVAL 秒 | 尚未实现 |
| `curl` | ❌ 未实现 | 中 | 从服务器传输数据或向服务器传输数据 | `-s, --silent`: 静默模式<br>`-o, --output`: 将输出写入文件而不是 stdout | 尚未实现 |

### 其他

| 命令 | 状态 | 优先级 | 描述 | 参数选项 | 实现说明 |
|------|------|--------|------|----------|----------|
| `help` | ✅ 已实现 | 高 | 显示命令的帮助信息 | `COMMAND`: 显示特定命令的帮助 | 使用命令元数据生成帮助 |
| `exit` | ✅ 已实现 | 高 | 退出 shell | 无选项 | 简单退出调用，带有管道结构 |
| `clear` | ✅ 已实现 | 高 | 清除终端屏幕 | 无选项 | 使用系统调用清除屏幕 |
| `cd` | ✅ 已实现 | 高 | 更改当前目录 | `-L, --logical`: 强制遵循符号链接<br>`-P, --physical`: 使用物理目录结构而不遵循符号链接 | 使用 SetCurrentDirectory API，带有管道结构 |
| `type` | ❌ 未实现 | 中 | 描述命令类型 | `COMMAND`: 描述特定命令 | 尚未实现 |
| `alias` | ❌ 未实现 | 低 | 为命令创建别名 | `NAME=VALUE`: 定义别名<br>`-p`: 打印所有定义的别名 | 尚未实现 |

## 实现指南

### 一般结构

每个命令应遵循以下一般结构：

1. **模块声明**：以 `export module cmd.<command>;` 开头
2. **导入**：导入必要的模块（`import core;`、`import utils;` 等）
3. **常量命名空间**：定义命令特定的常量
4. **选项定义**：使用 `OPTION` 宏定义命令选项
5. **管道组件**：实现用于命令处理的管道组件
6. **命令实现**：实现主命令逻辑
7. **注册**：使用 `REGISTER_COMMAND` 宏注册命令

### 示例结构

```cpp
export module cmd.echo;

import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

namespace echo_constants {
    // 命令特定常量
}

// 定义选项
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

    // 处理转义序列
    auto to_uppercase(std::string text) -> std::string {
        std::ranges::transform(text, text.begin(), ::toupper);
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
                 "WinuxCmd 团队",

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

    // 获取选项
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

### 最佳实践

1. **管道架构**：使用管道组件进行模块化处理
2. **类型安全**：使用 `CommandContext` 进行类型安全的选项访问
3. **错误处理**：使用 `core::pipeline::Result` 进行一致的错误处理
4. **文档**：为每个命令提供清晰的文档
5. **测试**：使用各种输入和选项测试命令
6. **性能**：在适当的地方优化性能
7. **兼容性**：尽可能遵循 POSIX 标准

### 测试

每个命令应使用以下方式测试：

1. **基本功能**：测试无选项的命令
2. **所有选项**：单独测试每个选项
3. **组合选项**：一起测试多个选项
4. **边缘情况**：测试空输入、大输入等
5. **错误情况**：测试无效输入和选项

## 迁移指南

### 从旧实现

要将命令从旧实现迁移到新的基于管道的结构：

1. **更新模块声明**：使用 `export module cmd.<command>;`
2. **添加导入**：添加必要的导入（`import core;`、`import utils;`）
3. **定义选项**：使用 `OPTION` 宏定义选项
4. **创建管道命名空间**：实现管道组件
5. **更新命令实现**：使用 `CommandContext` 进行选项访问
6. **注册命令**：使用新的 `REGISTER_COMMAND` 宏

### 示例迁移

#### 之后（基于管道的架构）

```cpp
export module cmd.echo;

import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

namespace echo_constants {
    // 命令特定常量可以在这里定义
}

// 定义选项
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
                 "WinuxCmd 团队",

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

## 参考实现

`echo.cppm` 文件作为新的基于管道的架构的参考实现。它演示了：

1. **管道组件**：分离验证和处理逻辑
2. **选项处理**：使用 `CommandContext` 进行类型安全的选项访问
3. **错误处理**：使用 `core::pipeline::Result` 进行错误报告
4. **命令注册**：正确使用 `REGISTER_COMMAND` 宏

所有新命令应遵循此模式以保持一致性和可维护性。
