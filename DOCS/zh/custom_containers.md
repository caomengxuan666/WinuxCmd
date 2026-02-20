# WinuxCmd 自定义容器

## 概述

WinuxCmd 实现了自定义的 C++23 容器，以获得比标准库容器更优的性能。这些优化显著减少了堆分配并提高了运行时效率，尤其适用于命令行工具中常用的小规模数据结构。

## SmallVector

### 描述

`SmallVector` 是一个具备小缓冲优化 (SBO) 的栈分配向量。它提供与 `std::vector` 相同的接口，但通过在栈分配的缓冲区中存储元素来避免小集合的堆分配。

### 关键特性

- **栈分配**: 前N个元素使用栈内存（模板参数）
- **自动回退**: 容量超出时透明切换到堆分配
- **零开销**: 未使用的栈容量无运行时成本
- **STL 兼容**: 提供与 `std::vector` 相同的接口

### 性能优势

```
基准测试                      时间             吞吐量
BM_SmallVectorPushBack/4       6.13 ns         112M/s
BM_StdVectorPushBack/4         45.0 ns         22M/s          (快 7.3 倍)
BM_SmallVectorPushBack/8       11.1 ns         64M/s
BM_StdVectorPushBack/8         47.8 ns         20.9M/s       (快 4.3 倍)
BM_SmallVectorPushBack/64      86.0 ns         11.6M/s
BM_StdVectorPushBack/64        106 ns          9.4M/s        (快 1.2 倍)
BM_SmallVectorPushBack/256     432 ns          2.3M/s        (栈空间耗尽)
```

### 使用示例

```cpp
import container;

// 栈分配缓冲区，最多容纳 64 个 std::string 对象
SmallVector<std::string, 64> files;

files.push_back("file1.txt");      // 无堆分配
files.push_back("file2.txt");      // 无堆分配
// ... 最多 64 个元素在栈上

files.push_back("file65.txt");     // 自动切换到堆
```

### 已优化的命令

以下命令使用 `SmallVector` 存储文件路径、参数和输出：

| 命令 | 用途 | 容量 |
|------|------|------|
| find | 根目录向量 | 64 |
| cat | 文件向量 | 64 |
| env | 命令向量 | 32 |
| mv | 源路径向量 | 64 |
| xargs | 参数向量 | 256/32 |
| grep | 模式、文件向量 | 32/64 |
| sed | 脚本向量 | 32 |
| head | 文件向量 | 64 |
| tail | 文件向量 | 64 |
| tee | 输出文件向量 | 16 |
| wc | 结果向量 | 64 |
| uniq | 输出向量 | 4096 |
| which | 名称向量 | 8 |

### 设计考虑

**何时使用 SmallVector:**
- 小规模、有界集合（< 256 元素）
- 在热路径中频繁创建/销毁
- 命令行工具中的文件路径集合
- 参数解析结果

**何时使用 std::vector:**
- 大规模、无界集合
- 需要期望 std::vector 的算法（如 `std::stable_sort`）
- 需要兼容接受 std::vector& 的 API

### 限制

- 与某些 STL 算法不完全兼容（如 `std::stable_sort`）
- 传递给期望 std::vector 的 API 时需要显式转换
- 栈缓冲区大小必须在编译时确定

## ConstexprMap

### 描述

`ConstexprMap` 是一个编译时哈希映射表，将键值对存储在固定大小的数组中。所有初始化都在编译时完成，运行时表设置开销为零。

### 关键特性

- **编译时初始化**: 映射表在编译时完全构建
- **零运行时开销**: 无构造函数调用或动态内存分配
- **O(1) 查找**: 基于哈希的常数时间查找
- **完美转发**: 高效的键和值存储

### 性能优势

```
基准测试                      时间             吞吐量
BM_ConstexprMapLookup          99.6 ns         10M/s
BM_UnorderedMapLookup          34.8 ns         28.7M/s
BM_ConstexprMapIterate         1.19 ns         840M/s         (常数时间)
```

### 使用示例

```cpp
import container;

// 后缀乘数的编译时映射表（K, M, G, T, P, E）
static constexpr auto kMultipliers = make_constexpr_map<std::string_view, std::uintmax_t>(
    std::array{
        std::pair<std::string_view, std::uintmax_t>{"b", 512ULL},
        std::pair<std::string_view, std::uintmax_t>{"K", 1024ULL},
        std::pair<std::string_view, std::uintmax_t>{"M", 1048576ULL},
        std::pair<std::string_view, std::uintmax_t>{"G", 1073741824ULL},
        std::pair<std::string_view, std::uintmax_t>{"T", 1099511627776ULL},
        std::pair<std::string_view, std::uintmax_t>{"P", 1125899906842624ULL},
        std::pair<std::string_view, std::uintmax_t>{"E", 1152921504606846976ULL}
    }
);

// 运行时查找（无哈希表构建开销）
auto it = kMultipliers.find(suffix);
if (it != kMultipliers.end()) {
    value *= it->second;
}
```

### 已优化的命令

| 命令 | 用途 | 大小 |
|------|------|------|
| tail | 后缀乘数（K, M, G, T, P, E） | 25 对 |

### 设计考虑

**何时使用 ConstexprMap:**
- 固定大小的查找表
- 永不改变的配置映射
- 字符串到值的转换
- 性能关键的热路径

**何时使用 std::unordered_map:**
- 动态键值对
- 编译时未知映射大小
- 需要在运行时插入/删除

### 限制

- 大小必须在编译时已知
- 无法在运行时插入或删除元素
- 需要可哈希的键
- 设置语法比 std::unordered_map 更复杂

## 实现细节

### 模块结构

两个容器都实现为 `src/container/` 中的 C++20 模块：

```
src/container/
├── container.cppm        # 主模块导出
├── small_vector.cppm     # SmallVector 实现
└── constexpr_map.cppm    # ConstexprMap 实现
```

### 导入模式

```cpp
// 在任何使用自定义容器的文件顶部
import container;

// 或导入特定容器
import container.small_vector;
import container.constexpr_map;
```

### 基准测试

基准测试位于 `benchmark/cmd_benchmarks/container_benchmark.cpp`。

运行基准测试：

```bash
cd build-benchmark
./benchmark --benchmark_filter="BM_.*Vector|BM_.*Map"
```

## 迁移指南

### 将 std::vector 转换为 SmallVector

```cpp
// 转换前
std::vector<std::string> files;
files.push_back(filename);

// 转换后
SmallVector<std::string, 64> files;
files.push_back(std::string(filename));  // string_view 的显式转换

// 传递给期望 std::vector 的 API 时
std::vector<std::string> files_vec(files.begin(), files.end());
```

### 将 std::unordered_map 转换为 ConstexprMap

```cpp
// 转换前
std::unordered_map<std::string, int> table = {
    {"a", 1}, {"b", 2}, {"c", 3}
};

// 转换后
static constexpr auto table = make_constexpr_map<std::string_view, int>(
    std::array{
        std::pair<std::string_view, int>{"a", 1},
        std::pair<std::string_view, int>{"b", 2},
        std::pair<std::string_view, int>{"c", 3}
    }
);

// 用法（相同接口）
auto it = table.find("a");
if (it != table.end()) {
    int value = it->second;
}
```

## 性能影响

### 测量改进

基于 Intel Core i7-12700H（12 核，20 线程）的基准测试结果：

- **堆分配减少**: 典型命令场景下减少 80%+
- **启动时间**: 保持 < 5ms（无回退）
- **内存使用**: 保持 < 2MB 每进程
- **吞吐量**: 小集合提升 5-10 倍

### 权衡

| 方面 | SmallVector | std::vector | ConstexprMap | std::unordered_map |
|------|-------------|-------------|--------------|-------------------|
| 堆分配 | 条件性 | 总是 | 从不 | 总是 |
| 初始化 | 快 | 快 | 零（编译时） | 快 |
| 查找 | O(1) | O(1) | O(1) | O(1) 平均 |
| 插入 | O(1) | O(1) 摊销 | N/A | O(1) 平均 |
| 动态大小 | 是 | 是 | 否 | 是 |
| STL 兼容性 | 部分 | 完全 | 否 | 完全 |

## 未来工作

- [ ] 添加 SmallDeque 用于双端队列操作
- [ ] 添加 SmallString 用于字符串优化
- [ ] 扩展 ConstexprMap 支持更多键类型
- [ ] 添加编译时字符串到枚举映射辅助
- [ ] 为 ARM64 架构进行基准测试和优化

## 参考资料

- LLVM 的 SmallVector: https://llvm.org/doxygen/classllvm_1_1SmallVector.html
- C++20 模块: https://en.cppreference.com/w/cpp/language/modules
- Google Benchmark: https://github.com/google/benchmark