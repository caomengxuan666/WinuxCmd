---
name: skill-creator-enhanced
description: 增强版技能创造工坊，让 AI 自行创建和优化技能包，实现个性化扩展。使用场景：(1) 当需要创建新技能时，(2) 当需要优化现有技能时，(3) 当需要将工作流程打包为技能时，(4) 当需要为特定任务创建定制技能时，(5) 当需要批量创建相关技能时。包含模板系统、代码生成、测试框架和发布工具。
---

# Skill Creator Enhanced（技能创造工坊增强版）

一站式技能开发、测试和发布平台。

## 为什么需要增强版？

虽然 OpenClaw 已有 skill-creator，但增强版提供：
- **更多模板** - 各种类型的技能模板
- **代码生成** - 自动生成常用代码模式
- **测试框架** - 内置技能测试工具
- **发布工具** - 一键发布到多个平台
- **AI辅助** - 利用AI优化技能设计

## 核心功能

### 1. 技能模板系统
- **基础模板** - 最小技能结构
- **工具集成模板** - 带工具调用的技能
- **Web服务模板** - 带HTTP服务的技能
- **定时任务模板** - 带cron调度的技能
- **数据管道模板** - 数据处理技能
- **AI助手模板** - 对话式技能

### 2. 代码生成器
- **工具调用代码** - 自动生成工具调用逻辑
- **错误处理代码** - 标准错误处理模式
- **配置管理代码** - 配置文件读写
- **日志记录代码** - 结构化日志
- **测试代码** - 单元测试和集成测试

### 3. 测试框架
- **单元测试** - 测试单个功能
- **集成测试** - 测试技能整体
- **性能测试** - 测试响应时间
- **兼容性测试** - 测试不同环境
- **安全测试** - 安全检查

### 4. 发布工具
- **本地发布** - 打包为.skill文件
- **ClawHub发布** - 发布到技能市场
- **GitHub发布** - 发布到GitHub仓库
- **版本管理** - 语义化版本控制
- **更新日志** - 自动生成CHANGELOG

## 工作流程

### 1. 技能规划
```bash
# 交互式技能规划
skill-creator plan

# 基于现有工作流创建技能
skill-creator plan-from-workflow ./workflow.md

# 从对话历史提取技能
skill-creator plan-from-history ./conversation.log
```

### 2. 技能创建
```bash
# 使用模板创建技能
skill-creator create my-skill --template web-service

# 从现有代码创建技能
skill-creator create-from-code ./my-tool --name my-tool-skill

# 批量创建相关技能
skill-creator create-batch skills.yaml
```

### 3. 技能开发
```bash
# 添加工具调用
skill-creator add-tool my-skill --tool read --tool write

# 添加配置
skill-creator add-config my-skill --config api-key --config endpoint

# 添加测试
skill-creator add-test my-skill --type unit --type integration

# 生成文档
skill-creator generate-docs my-skill
```

### 4. 技能测试
```bash
# 运行所有测试
skill-creator test my-skill

# 运行特定测试
skill-creator test my-skill --test unit

# 性能测试
skill-creator benchmark my-skill

# 兼容性测试
skill-creator test-compatibility my-skill
```

### 5. 技能发布
```bash
# 打包技能
skill-creator package my-skill

# 发布到ClawHub
skill-creator publish my-skill --target clawhub

# 发布到GitHub
skill-creator publish my-skill --target github

# 更新版本
skill-creator version my-skill --bump minor
```

## 技能模板

### 基础技能模板
```
my-skill/
├── SKILL.md
└── scripts/
    └── main.sh
```

### 工具集成模板
```
my-tool-skill/
├── SKILL.md
├── scripts/
│   ├── main.py
│   └── utils.py
├── config/
│   └── settings.json
└── tests/
    └── test_main.py
```

### Web服务模板
```
web-service-skill/
├── SKILL.md
├── src/
│   ├── app.py
│   ├── routes.py
│   └── models.py
├── config/
│   └── config.yaml
├── tests/
│   ├── test_api.py
│   └── test_models.py
└── requirements.txt
```

### 定时任务模板
```
cron-skill/
├── SKILL.md
├── scripts/
│   ├── scheduler.py
│   └── tasks.py
├── config/
│   └── cron.yaml
└── logs/
    └── README.md
```

## 代码生成示例

### 生成工具调用代码
```python
# 自动生成的工具调用代码
def call_tool(tool_name, **kwargs):
    """调用OpenClaw工具"""
    import json
    
    # 构建工具调用
    tool_call = {
        "name": tool_name,
        "parameters": kwargs
    }
    
    # 这里应该是实际的工具调用逻辑
    # 例如: openclaw.tools.call(tool_call)
    
    return {"success": True, "result": "Tool called"}
```

### 生成错误处理代码
```python
# 自动生成的错误处理
class SkillError(Exception):
    """技能错误基类"""
    pass

class ToolError(SkillError):
    """工具调用错误"""
    pass

class ConfigError(SkillError):
    """配置错误"""
    pass

def handle_error(error, context=None):
    """统一错误处理"""
    error_info = {
        "error": str(error),
        "type": type(error).__name__,
        "context": context,
        "timestamp": datetime.now().isoformat()
    }
    
    # 记录错误日志
    log_error(error_info)
    
    # 返回用户友好的错误信息
    return f"技能执行出错: {error}"
```

### 生成配置管理代码
```python
# 自动生成的配置管理
import json
import os
from pathlib import Path

class SkillConfig:
    """技能配置管理"""
    
    def __init__(self, skill_name):
        self.skill_name = skill_name
        self.config_dir = Path.home() / ".openclaw" / "skills" / skill_name
        self.config_file = self.config_dir / "config.json"
        
        # 确保配置目录存在
        self.config_dir.mkdir(parents=True, exist_ok=True)
    
    def load(self):
        """加载配置"""
        if self.config_file.exists():
            with open(self.config_file, 'r') as f:
                return json.load(f)
        return {}
    
    def save(self, config):
        """保存配置"""
        with open(self.config_file, 'w') as f:
            json.dump(config, f, indent=2)
    
    def get(self, key, default=None):
        """获取配置项"""
        config = self.load()
        return config.get(key, default)
    
    def set(self, key, value):
        """设置配置项"""
        config = self.load()
        config[key] = value
        self.save(config)
```

## 测试框架

### 测试结构
```
tests/
├── __init__.py
├── conftest.py          # 测试配置
├── test_skills.py       # 技能基础测试
├── test_tools.py        # 工具调用测试
├── test_config.py       # 配置测试
├── test_performance.py  # 性能测试
└── fixtures/           # 测试数据
    └── sample_data.json
```

### 测试示例
```python
# test_skills.py
import pytest
from my_skill import MySkill

class TestMySkill:
    """测试MySkill"""
    
    def setup_method(self):
        """测试设置"""
        self.skill = MySkill()
    
    def test_skill_initialization(self):
        """测试技能初始化"""
        assert self.skill.name == "my-skill"
        assert self.skill.version == "1.0.0"
    
    def test_tool_calls(self):
        """测试工具调用"""
        result = self.skill.call_tool("read", path="test.txt")
        assert result["success"] is True
    
    def test_error_handling(self):
        """测试错误处理"""
        with pytest.raises(ToolError):
            self.skill.call_tool("invalid_tool")
    
    @pytest.mark.performance
    def test_performance(self):
        """测试性能"""
        import time
        start = time.time()
        
        # 执行性能测试
        for _ in range(100):
            self.skill.simple_operation()
        
        duration = time.time() - start
        assert duration < 1.0  # 应该在1秒内完成
```

## 发布流程

### 1. 版本管理
```bash
# 查看当前版本
skill-creator version my-skill

# 升级版本
skill-creator version my-skill --bump major    # 主版本
skill-creator version my-skill --bump minor    # 次版本  
skill-creator version my-skill --bump patch    # 修订版本

# 设置特定版本
skill-creator version my-skill --set 2.1.0
```

### 2. 生成更新日志
```bash
# 自动生成CHANGELOG
skill-creator changelog my-skill

# 基于Git提交生成
skill-creator changelog my-skill --git

# 自定义格式
skill-creator changelog my-skill --format markdown
```

### 3. 打包技能
```bash
# 标准打包
skill-creator package my-skill

# 带依赖打包
skill-creator package my-skill --include-deps

# 最小化打包
skill-creator package my-skill --minify

# 签名打包
skill-creator package my-skill --sign
```

### 4. 发布技能
```bash
# 发布到ClawHub
skill-creator publish my-skill --target clawhub --token $CLAWHUB_TOKEN

# 发布到GitHub
skill-creator publish my-skill --target github --token $GITHUB_TOKEN

# 发布到多个平台
skill-creator publish my-skill --target all

# 发布预览版
skill-creator publish my-skill --pre-release
```

## AI辅助功能

### 技能优化建议
```bash
# 分析技能并提出优化建议
skill-creator analyze my-skill

# 自动优化技能
skill-creator optimize my-skill

# 代码审查
skill-creator review my-skill --ai
```

### 技能文档生成
```bash
# 生成完整文档
skill-creator docs my-skill --generate

# 更新现有文档
skill-creator docs my-skill --update

# 生成API文档
skill-creator docs my-skill --api
```

### 技能测试生成
```bash
# 基于代码生成测试
skill-creator tests my-skill --generate

# 生成边界测试
skill-creator tests my-skill --edge-cases

# 生成性能测试
skill-creator tests my-skill --performance
```

## 配置管理

### 技能配置
```yaml
# .skill-creator.yaml
skill:
  name: my-skill
  version: 1.0.0
  author: Your Name
  license: MIT
  
templates:
  default: basic
  custom_templates: ./templates/
  
publishing:
  clawhub:
    enabled: true
    token_env: CLAWHUB_TOKEN
  github:
    enabled: true
    token_env: GITHUB_TOKEN
    
testing:
  unit_tests: true
  integration_tests: true
  performance_tests: false
  
code_generation:
  language: python
  style: pep8
  include_comments: true
```

### 环境变量
```bash
# 发布令牌
export CLAWHUB_TOKEN="your-token"
export GITHUB_TOKEN="your-token"

# 开发配置
export SKILL_DEV_MODE="true"
export SKILL_LOG_LEVEL="debug"

# 测试配置
export SKILL_TEST_ENV="staging"
export SKILL_TEST_DATA="./test-data/"
```

## 最佳实践

### 技能设计
1. **单一职责** - 每个技能只做一件事
2. **明确接口** - 清晰的输入输出
3. **良好文档** - 完整的SKILL.md
4. **错误处理** - 健壮的错误处理
5. **配置分离** - 配置与代码分离

### 开发流程
1. **规划先行** - 先规划再编码
2. **测试驱动** - 先写测试再实现
3. **代码审查** - 使用AI辅助审查
4. **持续集成** - 自动化测试和发布
5. **版本控制** - 语义化版本管理

### 发布管理
1. **版本策略** - 清晰的版本策略
2. **更新日志** - 详细的更新日志
3. **向后兼容** - 尽量保持兼容性
4. **用户反馈** - 收集用户反馈
5. **持续改进** - 定期更新和维护

## 故障排除

### 常见问题
```bash
# 技能创建失败
skill-creator diagnose my-skill

# 测试失败
skill-creator test my-skill --debug

# 发布失败
skill-creator publish my-skill --verbose

# 依赖问题
skill-creator deps my-skill --check
```

### 调试工具
```bash
# 技能调试模式
skill-creator debug my-skill

# 性能分析
skill-creator profile my-skill

# 内存分析
skill-creator memory my-skill

# 代码覆盖率
skill-creator coverage my-skill
```

## 开始使用

### 快速开始
```bash
# 1. 创建新技能
skill-creator create my-first-skill --template basic

# 2. 开发技能
cd my-first-skill
# 编辑 SKILL.md 和代码

# 3. 测试技能
skill-creator test my-first-skill

# 4. 发布技能
skill-creator publish my-first-skill
```

### 进阶示例
```bash
# 创建Web服务技能
skill-creator create web-api --template web-service \
  --tools read write exec \
  --config api-key port \
  --tests unit integration

# 添加AI功能
skill-creator add-ai web-api --model deepseek-chat

# 打包并发布
skill-creator package web-api
skill-creator publish web-api --target clawhub
```

## 资源

- **技能模板库**: https://github.com/openclaw/skill-templates
- **技能示例**: https://github.com/openclaw/skill-examples
- **开发文档**: https://docs.openclaw.ai/skill-development
- **社区支持**: https://discord.gg/clawd

**记住：最好的技能不是最复杂的，而是最能解决问题的。**