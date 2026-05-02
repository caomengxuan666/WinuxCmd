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
