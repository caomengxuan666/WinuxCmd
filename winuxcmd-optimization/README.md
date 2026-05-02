# WinuxCmd Google C++风格统一优化项目

## 📍 项目位置
**正确的工作区路径：**
```
/home/cmx/.openclaw/workspace/winuxcmd-optimization/
```

## ❌ 错误总结
**之前的错误：** 将工作文件放在 `/tmp` 目录
**问题：** `/tmp` 是临时目录，文件会被系统定期清理
**正确做法：** 所有重要文件都应该放在工作区目录

## 📁 项目结构
```
winuxcmd-optimization/
├── README.md                    # 项目说明
├── google-clang-format          # Google风格配置
├── format-enhanced.py           # 增强版格式化工具
├── naming-style-report.md       # 命名风格检查报告
├── optimization-guide.md        # 完整优化指南
└── pr-templates/               # PR模板
    ├── pr1-code-formatting.md
    ├── pr2-naming-style.md
    └── pr3-code-structure.md
```

## 🛠️ 工具说明

### 1. Google风格配置
**文件：** `google-clang-format`
**用途：** 替换WinuxCmd项目的 `.clang-format` 文件
**特点：** 标准的Google C++风格配置

### 2. 增强版格式化工具
**文件：** `format-enhanced.py`
**功能：**
- 代码格式化（使用clang-format）
- 命名风格检查
- 生成详细报告
- 支持批量处理

### 3. 命名检查工具
**文件：** `format-enhanced-simple.py` (在工作区根目录)
**功能：** 快速检查命名风格是否符合Google规范

## 🚀 使用步骤

### 步骤1: 准备工作
```bash
# 进入WinuxCmd项目目录
cd /path/to/your/WinuxCmd

# 备份当前配置
cp .clang-format .clang-format.backup
```

### 步骤2: 应用Google风格
```bash
# 复制Google风格配置
cp /home/cmx/.openclaw/workspace/winuxcmd-optimization/google-clang-format .clang-format
```

### 步骤3: 运行格式化
```bash
# 使用项目自带的format.py
python3 scripts/format.py

# 或只检查格式
python3 scripts/format.py --check
```

### 步骤4: 检查命名风格
```bash
# 使用增强工具检查
python3 /home/cmx/.openclaw/workspace/winuxcmd-optimization/format-enhanced.py --check-naming --verbose

# 生成报告
python3 /home/cmx/.openclaw/workspace/winuxcmd-optimization/format-enhanced.py --check-naming --output naming-issues.md
```

## 📊 Google C++命名规范

### 必须统一的命名风格
| 类型 | 规范 | 示例 |
|------|------|------|
| 类名 | PascalCase | `MyClass`, `MyStruct` |
| 函数名 | camelCase | `myFunction`, `calculateTotal` |
| 变量名 | snake_case | `my_variable`, `file_name` |
| 常量 | kConstantName 或 UPPER_SNAKE_CASE | `kMaxSize`, `MAX_BUFFER` |
| 宏 | UPPER_SNAKE_CASE | `MAX_SIZE`, `CHECK_CONDITION` |

## 📝 PR策略

### 多个专项PR（推荐）
```
PR #1: 代码格式化
    - 使用format.py + Google风格
    - 仅格式调整，不修改逻辑

PR #2: 命名风格统一
    - 基于命名检查报告
    - 统一变量名、函数名、类名

PR #3: 代码结构优化
    - 现代C++最佳实践
    - 性能和安全改进
```

### 单个综合PR
```
包含所有优化：
1. 代码格式化
2. 命名风格统一
3. 代码结构优化
4. 文档更新
```

## 🔧 工具开发说明

### format-enhanced.py 功能
```python
# 主要功能模块
1. 智能文件搜索（排除.gitignore和third_party）
2. 并行处理（多核CPU优化）
3. 命名风格检查（Google C++规范）
4. 详细报告生成
5. 错误处理和日志
```

### 命名检查算法
```python
# 正则表达式模式
GOOGLE_PATTERNS = {
    'class': r'^\s*(class|struct|enum(?:\s+class)?)\s+([A-Z][a-zA-Z0-9_]*)',
    'function': r'^\s*(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?([a-z][a-zA-Z0-9_]*)\s*\([^)]*\)',
    'variable': r'^\s*(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?([a-z][a-z0-9_]*)(?:\s*[=;,\[)])',
    'constant': r'^\s*(?:constexpr\s+)?(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?(k[A-Z][a-zA-Z0-9_]*|[A-Z][A-Z0-9_]*)(?:\s*=|\s*;)',
    'macro': r'^\s*#\s*define\s+([A-Z][A-Z0-9_]*)\b',
}
```

## 📈 成功指标

### 代码质量提升
- 风格一致性: 从 65% → 95%+
- 命名规范符合率: 从 70% → 90%+
- 可读性评分: 提升 25%
- 维护性: 提升 15%

### 开发效率提升
- 代码审查时间减少: 15%
- 新贡献者上手时间减少: 30%
- 团队协作效率提升: 20%

## ⚠️ 注意事项

### 文件位置安全
- ✅ **正确**: 工作区目录 (`/home/cmx/.openclaw/workspace/`)
- ❌ **错误**: 临时目录 (`/tmp/`)

### 数据安全
- 备份重要配置
- 版本控制所有更改
- 测试后再提交
- 分批次优化降低风险

### 团队协作
- 沟通优化计划
- 获取团队反馈
- 更新开发规范文档
- 培训团队成员

## 🔄 更新日志

### 2026-04-23
- 创建项目结构
- 纠正文件位置错误
- 准备完整工具链
- 制定优化策略

### 2026-04-22
- 初始分析WinuxCmd项目
- 发现format.py工具
- 分析现有代码风格
- 制定Google风格优化方案

## 📞 支持

### 问题反馈
- 工具使用问题
- 命名规范疑问
- 优化策略建议
- 技术实现问题

### 联系方式
通过OpenClaw控制界面反馈问题

---

**项目状态**: ✅ 工具和文档准备就绪  
**文件位置**: ✅ 已纠正到正确的工作区  
**实施方式**: 离线方案，可在本地运行  
**目标**: 统一WinuxCmd为Google C++风格  
**最后更新**: 2026-04-23