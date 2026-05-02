---
name: find-skills
description: 帮助查找、安装和升级技能，是优先级最高的发现工具。使用场景：(1) 当用户需要特定功能时查找相关技能，(2) 当需要安装新技能时，(3) 当需要更新现有技能时，(4) 当需要了解可用技能时，(5) 当需要管理技能依赖时。支持本地技能库、ClawHub技能市场和自定义技能源。
---

# Find Skills（技能发现专家）

一站式技能发现、安装和管理工具。

## 为什么需要这个技能？

OpenClaw的技能生态系统正在快速增长，但：
- 用户不知道有哪些技能可用
- 技能分散在不同地方（本地、ClawHub、GitHub）
- 安装和更新流程不统一
- 依赖管理复杂

这个技能解决所有这些问题。

## 核心功能

### 1. 技能发现
- **本地技能库** - 查看已安装技能
- **ClawHub市场** - 浏览社区技能
- **GitHub搜索** - 查找开源技能
- **按分类浏览** - 开发工具、生产力、娱乐等

### 2. 技能安装
- **一键安装** - 从各种源安装
- **依赖检查** - 自动检查并安装依赖
- **配置向导** - 引导完成必要配置
- **权限管理** - 处理所需权限

### 3. 技能管理
- **更新检查** - 检查技能更新
- **版本管理** - 管理不同版本
- **启用/禁用** - 控制技能状态
- **卸载清理** - 完全移除技能

### 4. 技能推荐
- **基于使用历史** - 推荐相关技能
- **热门技能** - 社区最受欢迎
- **新技能** - 最近发布的技能
- **必备技能** - 基础工具集

## 工作流程

### 发现技能

```bash
# 查看本地技能
find-skills list

# 搜索技能
find-skills search "github"

# 浏览分类
find-skills browse --category "development"

# 查看技能详情
find-skills info skill-name
```

### 安装技能

```bash
# 从ClawHub安装
find-skills install clawhub:skill-name

# 从GitHub安装
find-skills install github:user/repo

# 从本地文件安装
find-skills install ./path/to/skill.skill

# 安装指定版本
find-skills install skill-name@1.2.3
```

### 管理技能

```bash
# 更新所有技能
find-skills update --all

# 更新特定技能
find-skills update skill-name

# 启用/禁用技能
find-skills enable skill-name
find-skills disable skill-name

# 卸载技能
find-skills uninstall skill-name
```

## 技能源配置

### 本地技能库
```
~/.openclaw/skills/          # 用户技能
/usr/local/share/openclaw/skills/  # 系统技能
```

### ClawHub技能市场
- 网址: https://clawhub.ai
- API: https://api.clawhub.ai/v1/skills
- 分类: 开发、生产力、娱乐、工具等

### GitHub技能仓库
- 组织: openclaw-skills
- 标签: openclaw-skill
- 模板: skill-template

### 自定义源
支持添加自定义技能源：
```json
{
  "name": "custom-source",
  "url": "https://example.com/skills.json",
  "type": "http"
}
```

## 技能元数据

每个技能包含：
```yaml
name: skill-name
version: 1.0.0
description: 技能描述
author: 作者
license: MIT
dependencies:
  - tool-name>=1.0
  - python-package>=2.0
compatibility:
  openclaw: ">=1.0.0"
  platform: ["linux", "macos"]
category: ["development", "tools"]
tags: ["git", "github", "automation"]
```

## 安装流程

### 1. 预检查
- 检查OpenClaw版本兼容性
- 检查系统依赖
- 检查磁盘空间
- 检查权限

### 2. 下载技能
- 从指定源下载技能包
- 验证签名（如果可用）
- 解压到临时目录

### 3. 安装依赖
- 安装系统依赖（apt/yum/brew）
- 安装Python包（pip）
- 安装Node.js包（npm）
- 配置环境变量

### 4. 配置技能
- 运行配置向导
- 设置必要参数
- 测试技能功能
- 创建快捷方式

### 5. 完成安装
- 移动技能到正确位置
- 更新技能索引
- 发送安装完成通知
- 记录安装日志

## 更新机制

### 自动更新检查
```bash
# 每天检查一次更新
0 2 * * * find-skills check-updates
```

### 更新策略
- **安全更新** - 自动安装
- **功能更新** - 询问用户
- **破坏性更新** - 需要确认
- **测试版本** - 可选安装

### 版本回滚
```bash
# 回滚到上一个版本
find-skills rollback skill-name

# 安装特定版本
find-skills install skill-name@1.0.0
```

## 技能推荐系统

### 推荐算法
1. **协同过滤** - 类似用户使用的技能
2. **内容推荐** - 基于技能描述和标签
3. **时序推荐** - 基于使用时间模式
4. **情境推荐** - 基于当前任务

### 推荐来源
```json
{
  "based_on_history": ["git", "github"],
  "popular_now": ["code-review", "ai-assistant"],
  "trending": ["docker", "kubernetes"],
  "essential": ["file-manager", "text-editor"]
}
```

## 故障排除

### 常见问题

#### 安装失败
```bash
# 查看详细日志
find-skills install skill-name --verbose

# 跳过依赖检查
find-skills install skill-name --no-deps

# 强制安装
find-skills install skill-name --force
```

#### 技能冲突
```bash
# 检查冲突
find-skills conflicts

# 解决冲突
find-skills resolve skill1 skill2
```

#### 权限问题
```bash
# 修复权限
find-skills fix-permissions

# 以管理员运行
sudo find-skills install skill-name
```

### 日志文件
```
~/.openclaw/logs/find-skills.log
~/.openclaw/logs/install-*.log
~/.openclaw/logs/update-*.log
```

## 安全考虑

### 技能验证
- **签名验证** - 验证技能作者签名
- **代码审查** - 自动代码安全检查
- **权限限制** - 限制危险操作
- **沙箱运行** - 在隔离环境测试

### 用户保护
- **明确权限** - 显示技能所需权限
- **用户确认** - 危险操作需要确认
- **操作日志** - 记录所有技能操作
- **恢复机制** - 支持快速恢复

## 扩展功能

### 技能包
```bash
# 创建技能包
find-skills pack ./my-skill

# 分享技能包
find-skills share skill-name

# 导入技能包
find-skills import ./skill-pack.zip
```

### 技能开发
```bash
# 初始化新技能
find-skills init my-new-skill

# 测试技能
find-skills test skill-name

# 发布技能
find-skills publish skill-name
```

### 技能分析
```bash
# 使用统计
find-skills stats

# 性能分析
find-skills profile skill-name

# 依赖分析
find-skills deps skill-name --tree
```

## 开始使用

### 快速开始
```bash
# 1. 查看可用技能
find-skills list

# 2. 搜索需要的技能
find-skills search "git"

# 3. 安装技能
find-skills install gh-issues

# 4. 检查更新
find-skills update --all
```

### 配置示例
```bash
# 添加自定义源
find-skills source add my-company https://skills.company.com

# 设置自动更新
find-skills config set auto-update true

# 设置代理
find-skills config set proxy http://proxy:8080
```

## 最佳实践

1. **定期更新** - 每周检查一次更新
2. **备份配置** - 备份技能配置
3. **测试环境** - 在生产环境前测试
4. **社区参与** - 分享和评价技能
5. **安全第一** - 只从可信源安装

## 资源

- **官方文档**: https://docs.openclaw.ai/skills
- **ClawHub市场**: https://clawhub.ai
- **GitHub组织**: https://github.com/openclaw-skills
- **社区论坛**: https://discord.gg/clawd

**记住：好的工具让工作更高效，好的技能让AI更强大。**