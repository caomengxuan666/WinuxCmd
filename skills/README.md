# 自我改进技能套件

## 已安装技能

### 1. self-improving-agent（自我进化）
- **描述**: 记录错误、持续学习，让 AI 越用越聪明
- **用途**: 错误记录、用户反馈、改进发现、定期回顾
- **目录**: skills/self-improving-agent/

### 2. find-skills（技能发现专家）
- **描述**: 帮助查找、安装和升级技能
- **用途**: 技能发现、安装、管理、推荐
- **目录**: skills/find-skills/

### 3. skill-creator-enhanced（技能创造工坊增强版）
- **描述**: 让 AI 自行创建和优化技能包
- **用途**: 技能创建、优化、测试、发布
- **目录**: skills/skill-creator-enhanced/

## 使用说明

### 启用技能
技能会在相关任务时自动触发，根据SKILL.md中的description匹配。

### 手动调用
```bash
# 查看技能文档
cat skills/self-improving-agent/SKILL.md | head -50

# 使用技能功能
# 具体功能请参考各技能的SKILL.md
```

### 技能开发
要创建新技能，参考 skill-creator-enhanced 技能。

## 更新日志

- 2026-04-22: 初始安装
  - 安装三个核心自我改进技能
  - 创建记忆目录结构
  - 设置日志系统

## 下一步

1. 开始使用 self-improving-agent 记录错误和学习
2. 使用 find-skills 发现更多有用技能
3. 使用 skill-creator-enhanced 创建定制技能

---
**安装时间**: $(date +%Y-%m-%d\ %H:%M:%S)
**安装位置**: $WORKSPACE_DIR/skills/
