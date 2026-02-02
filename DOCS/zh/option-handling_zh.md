# 选项处理指南

## 概述

本指南介绍如何使用 `core.opt` 模块在 WinuxCmd 项目中实现一致的选项处理。该模块提供了一种从选项定义生成选项处理程序的统一方法，减少代码重复并确保一致性。

## 核心概念

### OptionHandler 结构

`OptionHandler` 结构表示单个选项处理程序，具有以下字段：

```cpp
struct OptionHandler {
  char short_opt;         // 短选项字符（例如，'l' 表示 -l）
  bool requires_arg = false; // 选项是否需要参数
  const char* long_opt = nullptr; // 长选项名称（例如，"--long-list"）
};
```

### 关键函数

#### generate_option_handlers

从 `OptionMeta` 对象数组生成 `std::array<OptionHandler>`。此函数有两个重载：

1. **基本情况**：未指定需要参数的选项

   ```cpp
   template <size_t N>
   constexpr auto generate_option_handlers(const std::array<cmd::meta::OptionMeta, N>& options);
   ```

2. **带自定义需要参数的选项**：

   ```cpp
   template <size_t N, typename... Args>
   constexpr auto generate_option_handlers(const std::array<cmd::meta::OptionMeta, N>& options, Args... args);
   ```

#### requires_argument

检查选项是否需要参数。此函数有两个重载：

1. **基本情况**：始终返回 false

   ```cpp
   constexpr bool requires_argument(std::string_view long_name);
   ```

2. **带自定义需要参数的选项**：

   ```cpp
   template <typename... Args>
   constexpr bool requires_argument(std::string_view long_name, Args... args);
   ```

## 使用示例（ls 命令）

### 1. 定义选项

首先，使用 `OPTION` 宏定义命令选项：

```cpp
constexpr auto LS_OPTIONS = std::array{
    OPTION("-a", "--all", "do not ignore entries starting with ."),
    OPTION("-T", "--tabsize", "assume tab stops at each COLS instead of 8"),
    OPTION("-w", "--width", "set output width to COLS. 0 means no limit"),
    // ... 其他选项
};
```

### 2. 生成选项处理程序

从选项数组生成 `OPTION_HANDLERS`。指定任何需要参数的选项：

```cpp
// 从 LS_OPTIONS 自动生成选项查找表
constexpr auto OPTION_HANDLERS = generate_option_handlers(LS_OPTIONS, "--tabsize", "--width");
```

### 3. 使用选项处理程序

在选项解析逻辑中使用生成的 `OPTION_HANDLERS`：

```cpp
// 示例：解析长选项
if (arg.starts_with("--")) {
  for (const auto& handler : OPTION_HANDLERS) {
    if (handler.long_opt && arg == handler.long_opt) {
      if (handler.requires_arg) {
        // 处理带参数的选项
      } else {
        // 处理布尔选项
      }
      break;
    }
  }
}

// 示例：解析短选项
for (size_t j = 1; j < arg.size(); ++j) {
  char opt_char = arg[j];
  for (const auto& handler : OPTION_HANDLERS) {
    if (handler.short_opt == opt_char) {
      if (handler.requires_arg) {
        // 处理带参数的选项
      } else {
        // 处理布尔选项
      }
      break;
    }
  }
}
```

## 迁移指南

要将现有命令迁移到使用 `core.opt` 模块：

1. **添加导入语句**：

   ```cpp
   import core.opt;
   ```

2. **移除重复代码**：
   - 移除任何本地 `OptionHandler` 结构定义
   - 移除任何本地选项处理程序生成逻辑

3. **生成选项处理程序**：
   - 用对 `generate_option_handlers` 的调用替换手动定义的 `OPTION_HANDLERS`
   - 指定任何需要参数的选项

4. **更新选项解析**：
   - 在解析逻辑中使用生成的 `OPTION_HANDLERS`
   - 确保一致处理带参数的选项

## 最佳实践

- **单一事实来源**：仅在 `OPTIONS` 数组中定义选项一次
- **显式参数要求**：调用 `generate_option_handlers` 时始终指定哪些选项需要参数
- **一致命名**：尽可能在所有命令中使用相同的选项名称
- **文档**：使用清晰的描述记录所有选项

## 故障排除

### 常见问题

1. **选项未找到**：确保选项在 `OPTIONS` 数组中定义
2. **参数处理**：确保需要参数的选项在 `generate_option_handlers` 调用中正确指定
3. **模块导入**：确保 `import core.opt;` 存在于您的模块中

### 调试提示

- 打印生成的 `OPTION_HANDLERS` 数组以验证它包含预期的处理程序
- 检查短选项是否正确从 short_name 字段提取
- 确保长选项正确映射到其处理程序

## 示例：迁移 ls.cppm

`ls.cppm` 文件作为使用 `core.opt` 模块的参考实现。关键更改包括：

1. **添加导入**：`import core.opt;`
2. **移除本地定义**：移除重复的 `OptionHandler` 结构和辅助函数
3. **生成处理程序**：使用 `generate_option_handlers(LS_OPTIONS, "--tabsize", "--width")`
4. **更新解析**：在解析逻辑中使用生成的 `OPTION_HANDLERS`

通过遵循此模式，所有命令都可以从一致、可维护的选项处理中受益。
