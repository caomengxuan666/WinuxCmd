#!/bin/bash
# Clash 重启脚本

echo "=== Clash 重启 ==="
echo ""

echo "1. 停止 Clash..."
source "$(dirname "$0")/clash-stop.sh"
echo ""

echo "2. 等待 3 秒..."
sleep 3
echo ""

echo "3. 启动 Clash..."
source "$(dirname "$0")/clash-start.sh"