#!/bin/bash

# 测试代码优化系统

set -e

WORKSPACE_DIR="/home/cmx/.openclaw/workspace"
LOG_FILE="$WORKSPACE_DIR/logs/test-$(date +%Y%m%d-%H%M%S).log"

echo "测试代码优化系统..."
echo "日志文件: $LOG_FILE"
echo ""

# 运行主优化脚本
bash "$WORKSPACE_DIR/scripts/code-optimization-runner.sh" 2>&1 | tee "$LOG_FILE"

echo ""
echo "✅ 测试完成"
echo "📝 查看日志: $LOG_FILE"
