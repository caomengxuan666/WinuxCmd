#!/bin/bash
# Clash 停止脚本
# 停止Clash进程并清除系统代理

CLASH_PID_FILE="/tmp/clash.pid"
PROXY_URL="http://127.0.0.1:7890"

echo "=== Clash 停止 ==="
echo ""

# 停止 Clash 进程
if [ -f "$CLASH_PID_FILE" ]; then
    CLASH_PID=$(cat "$CLASH_PID_FILE")
    if kill -0 "$CLASH_PID" 2>/dev/null; then
        echo "停止 Clash (PID: $CLASH_PID)..."
        kill "$CLASH_PID" 2>/dev/null
        sleep 2
        if kill -0 "$CLASH_PID" 2>/dev/null; then
            echo "强制终止..."
            kill -9 "$CLASH_PID" 2>/dev/null
        fi
        echo "✅ Clash 已停止"
    else
        echo "ℹ️  Clash 进程不存在 (PID: $CLASH_PID)"
    fi
    rm -f "$CLASH_PID_FILE"
else
    echo "ℹ️  PID 文件不存在"
    # 尝试查找并停止 clash 进程
    CLASH_PID=$(pgrep -x "clash" 2>/dev/null)
    if [ -n "$CLASH_PID" ]; then
        echo "查找并停止 Clash (PID: $CLASH_PID)..."
        kill "$CLASH_PID" 2>/dev/null
        echo "✅ Clash 已停止"
    else
        echo "ℹ️  Clash 未在运行"
    fi
fi

echo ""

# === 清除系统代理 ===
echo "清除系统代理..."

# 清除环境变量
unset http_proxy
unset https_proxy
unset all_proxy
unset HTTP_PROXY
unset HTTPS_PROXY
unset ALL_PROXY

echo "✅ 环境变量已清除"
echo "   http_proxy: 已清除"
echo "   https_proxy: 已清除"
echo "   all_proxy: 已清除"

# 清除 Git 代理
if command -v git &>/dev/null; then
    git config --global --unset http.proxy 2>/dev/null
    git config --global --unset https.proxy 2>/dev/null
    echo "✅ Git 代理已清除"
fi

# 清除 npm 代理
if command -v npm &>/dev/null; then
    npm config delete proxy 2>/dev/null
    npm config delete https-proxy 2>/dev/null
    echo "✅ npm 代理已清除"
fi

echo ""
echo "=== Clash 停止完成 ==="
echo ""
echo "📋 当前状态:"
echo "   所有代理设置已清除"
echo "   网络流量将使用直接连接"
echo ""
echo "💡 要启动 Clash，运行: source clash-start"