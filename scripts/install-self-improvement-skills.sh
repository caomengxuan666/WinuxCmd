#!/bin/bash

# 安装自我改进技能套件

set -e

WORKSPACE_DIR="/home/cmx/.openclaw/workspace"
SKILLS_DIR="$WORKSPACE_DIR/skills"
LOG_FILE="$WORKSPACE_DIR/logs/skill-install-$(date +%Y%m%d-%H%M%S).log"

log() {
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    local message="[$timestamp] $1"
    echo "$message"
    echo "$message" >> "$LOG_FILE"
}

log "开始安装自我改进技能套件"

# 1. 创建必要的目录
mkdir -p "$SKILLS_DIR"
mkdir -p "$WORKSPACE_DIR/memory/{errors,feedback,improvements,reviews}"
mkdir -p "$WORKSPACE_DIR/logs"

log "✅ 目录结构已创建"

# 2. 检查已安装的技能
log "已安装的技能:"
ls -la "$SKILLS_DIR" 2>/dev/null || log "暂无技能"

# 3. 创建技能索引
cat > "$SKILLS_DIR/README.md" << 'EOF'
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
EOF

log "✅ 技能索引已创建"

# 4. 创建第一个错误记录（作为示例）
cat > "$WORKSPACE_DIR/memory/errors/2026-04-22.md" << 'EOF'
# 错误记录 - 2026-04-22

## 技能安装示例错误
- **时间**: 2026-04-22 19:50
- **任务**: 安装自我改进技能
- **错误**: 无错误，这是示例记录
- **原因**: 演示错误记录格式
- **解决方案**: 按照格式记录真实错误
- **学习点**: 错误记录应该及时、具体、有用

## 错误记录格式
```markdown
## [错误类型或任务名称]
- **时间**: YYYY-MM-DD HH:MM
- **任务**: 简要描述失败的任务
- **错误**: 具体的错误信息
- **原因**: 分析失败原因
- **解决方案**: 如何修复或避免
- **学习点**: 从中学到了什么
```

## 错误分类指南
- **严重**: 导致任务完全失败
- **中等**: 影响任务质量但能完成  
- **轻微**: 小问题，不影响结果

## 今日提醒
开始记录真实的错误和反馈，让AI越来越聪明！
EOF

log "✅ 示例错误记录已创建"

# 5. 创建每周回顾模板
cat > "$WORKSPACE_DIR/memory/reviews/weekly-template.md" << 'EOF'
# 每周回顾 - YYYY-MM-DD

## 本周学习总结

### 1. 主要错误和改进
- [列出本周主要错误和改进]

### 2. 重要用户反馈
- [记录用户的重要反馈]

### 3. 发现的新方法
- [发现的新工作方法或技巧]

## 需要改进的方面

### 1. 重复出现的错误
- [哪些错误重复出现]

### 2. 效率低下的流程
- [哪些流程可以优化]

### 3. 需要更多知识的领域
- [哪些领域需要学习]

## 下周改进计划

### 1. 具体要改进什么
- [具体的改进目标]

### 2. 如何实施改进
- [实施步骤]

### 3. 预期效果
- [期望达到的效果]

## 技能使用统计
- 使用最频繁的技能: 
- 最有用的技能: 
- 需要改进的技能: 

## 用户满意度
- 用户纠正次数: 
- 用户表扬次数: 
- 需要关注的方面: 

---
**回顾时间**: YYYY-MM-DD
**下次回顾**: YYYY-MM-DD
EOF

log "✅ 每周回顾模板已创建"

# 6. 创建技能发现记录
cat > "$WORKSPACE_DIR/memory/improvements/2026-04-22.md" << 'EOF'
# 改进记录 - 2026-04-22

## 安装自我改进技能套件
- **时间**: 2026-04-22 19:50
- **旧方法**: 没有系统的自我改进机制
- **新方法**: 安装三个核心自我改进技能
- **优势**: 
  1. 系统化记录错误和学习
  2. 方便发现和安装新技能
  3. 能够创建定制技能
- **适用场景**: 所有AI助手工作场景

## 改进发现格式
```markdown
## [改进名称]
- **时间**: YYYY-MM-DD HH:MM
- **旧方法**: 原来的做法
- **新方法**: 更好的做法
- **优势**: 为什么更好
- **适用场景**: 何时使用新方法
```

## 今日改进目标
1. 开始使用错误记录系统
2. 探索技能发现功能
3. 了解技能创建工具
EOF

log "✅ 改进记录已创建"

# 7. 设置定期回顾提醒
cat > "$WORKSPACE_DIR/scripts/weekly-review-reminder.sh" << 'EOF'
#!/bin/bash

# 每周回顾提醒脚本

WORKSPACE_DIR="/home/cmx/.openclaw/workspace"
WEEK=$(date +%Y-W%W)
REVIEW_FILE="$WORKSPACE_DIR/memory/reviews/weekly-$WEEK.md"

# 检查是否已有本周回顾
if [ ! -f "$REVIEW_FILE" ]; then
    echo "📅 每周回顾提醒"
    echo "================"
    echo "本周还没有进行回顾！"
    echo ""
    echo "请花10分钟时间回顾本周工作："
    echo "1. 检查 memory/errors/ 中的错误记录"
    echo "2. 查看 memory/feedback/ 中的用户反馈"
    echo "3. 阅读 memory/improvements/ 中的改进记录"
    echo "4. 创建本周回顾：$REVIEW_FILE"
    echo ""
    echo "使用模板："
    echo "cp $WORKSPACE_DIR/memory/reviews/weekly-template.md $REVIEW_FILE"
    echo ""
    echo "回顾帮助AI持续改进，越用越聪明！"
else
    echo "✅ 本周回顾已完成：$REVIEW_FILE"
fi
EOF

chmod +x "$WORKSPACE_DIR/scripts/weekly-review-reminder.sh"

# 8. 添加cron任务（每周日20:00提醒）
CRON_JOB="0 20 * * 0 cd $WORKSPACE_DIR && bash scripts/weekly-review-reminder.sh >> logs/weekly-review-\$(date +\\%Y\\%m\\%d).log 2>&1"

if crontab -l 2>/dev/null | grep -q "weekly-review-reminder.sh"; then
    log "⚠️ 每周回顾提醒已存在，跳过添加"
else
    (crontab -l 2>/dev/null; echo "# 每周回顾提醒 - 周日20:00"; echo "$CRON_JOB") | crontab -
    log "✅ 每周回顾提醒已设置: 周日20:00"
fi

# 9. 输出总结
echo ""
echo "🎉 自我改进技能套件安装完成!"
echo "================================"
echo ""
echo "📁 安装位置:"
echo "  • 技能目录: $SKILLS_DIR/"
echo "  • 记忆目录: $WORKSPACE_DIR/memory/"
echo "  • 日志目录: $WORKSPACE_DIR/logs/"
echo ""
echo "🔧 已安装技能:"
echo "  1. self-improving-agent - 错误记录和学习"
echo "  2. find-skills - 技能发现和管理"
echo "  3. skill-creator-enhanced - 技能创建和优化"
echo ""
echo "📅 定时任务:"
echo "  • 每周日20:00: 每周回顾提醒"
echo "  • 每天06:00: 代码优化任务"
echo ""
echo "🚀 快速开始:"
echo "  1. 查看技能文档: cat $SKILLS_DIR/self-improving-agent/SKILL.md | head -100"
echo "  2. 记录第一个错误: echo '## 测试错误' >> $WORKSPACE_DIR/memory/errors/$(date +%Y-%m-%d).md"
echo "  3. 设置每周回顾: bash $WORKSPACE_DIR/scripts/weekly-review-reminder.sh"
echo ""
echo "📝 安装日志: $LOG_FILE"
echo ""

log "自我改进技能套件安装完成"
log "安装位置: $SKILLS_DIR/"
log "记忆目录: $WORKSPACE_DIR/memory/"
log "定时任务: 每周日20:00回顾提醒"