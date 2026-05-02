#!/bin/bash
# Clash 启动脚本
# 启动Clash守护进程并配置系统代理

CLASH_BIN="/usr/local/bin/clash"
CLASH_CONFIG_DIR="$HOME/.config/clash"
CLASH_LOG="$CLASH_CONFIG_DIR/clash.log"
CLASH_PID_FILE="/tmp/clash.pid"
MIXED_PORT=7890
PROXY_URL="http://127.0.0.1:$MIXED_PORT"
SOCKS5_URL="socks5://127.0.0.1:$MIXED_PORT"

echo "=== Clash 启动 ==="
echo ""

# 检查是否已在运行
if [ -f "$CLASH_PID_FILE" ] && kill -0 $(cat "$CLASH_PID_FILE") 2>/dev/null; then
    echo "⚠️  Clash 已在运行 (PID: $(cat $CLASH_PID_FILE))"
    echo "   如需重启，请先运行 clash-stop"
    exit 0
fi

# 检查 Clash 二进制
if [ ! -f "$CLASH_BIN" ]; then
    echo "❌ Clash 未找到: $CLASH_BIN"
    exit 1
fi

# 检查配置目录
if [ ! -d "$CLASH_CONFIG_DIR" ]; then
    echo "❌ 配置目录不存在: $CLASH_CONFIG_DIR"
    exit 1
fi

# 检查配置文件
if [ ! -f "$CLASH_CONFIG_DIR/config.yaml" ]; then
    echo "❌ 配置文件不存在: $CLASH_CONFIG_DIR/config.yaml"
    exit 1
fi

# 检查 Country.mmdb
if [ ! -f "$CLASH_CONFIG_DIR/Country.mmdb" ]; then
    echo "⚠️  Country.mmdb 未找到，Clash 可能无法正常解析地理位置"
fi

# 启动 Clash（后台守护进程）
echo "启动 Clash..."
nohup "$CLASH_BIN" -d "$CLASH_CONFIG_DIR" > "$CLASH_LOG" 2>&1 &
CLASH_PID=$!
echo $CLASH_PID > "$CLASH_PID_FILE"

# 等待启动
echo "等待 Clash 启动..."
for i in $(seq 1 10); do
    sleep 1
    if netstat -tln 2>/dev/null | grep -q ":$MIXED_PORT " || ss -tln 2>/dev/null | grep -q ":$MIXED_PORT "; then
        echo "✅ Clash 已启动 (PID: $CLASH_PID, 端口: $MIXED_PORT)"
        break
    fi
    if [ $i -eq 10 ]; then
        echo "❌ Clash 启动超时，请检查日志: $CLASH_LOG"
        kill $CLASH_PID 2>/dev/null
        rm -f "$CLASH_PID_FILE"
        exit 1
    fi
done

echo ""

# === 配置系统代理 ===
echo "配置系统代理..."

# 设置环境变量
export http_proxy="$PROXY_URL"
export https_proxy="$PROXY_URL"
export all_proxy="$SOCKS5_URL"
export HTTP_PROXY="$PROXY_URL"
export HTTPS_PROXY="$PROXY_URL"
export ALL_PROXY="$SOCKS5_URL"

echo "✅ 环境变量已设置:"
echo "   http_proxy=$http_proxy"
echo "   https_proxy=$https_proxy"
echo "   all_proxy=$all_proxy"

# 配置 Git 代理
if command -v git &>/dev/null; then
    git config --global http.proxy "$PROXY_URL" 2>/dev/null
    git config --global https.proxy "$PROXY_URL" 2>/dev/null
    echo "✅ Git 代理已配置"
fi

# 配置 npm 代理
if command -v npm &>/dev/null; then
    npm config set proxy "$PROXY_URL" 2>/dev/null
    npm config set https-proxy "$PROXY_URL" 2>/dev/null
    echo "✅ npm 代理已配置"
fi

echo ""

# === 验证代理 ===
echo "验证代理..."
HTTP_TEST=$(curl -x "$PROXY_URL" --connect-timeout 10 -s -o /dev/null -w "%{http_code}" https://httpbin.org/ip 2>/dev/null)
if [ "$HTTP_TEST" = "200" ]; then
    echo "✅ 代理验证成功 (HTTP 200)"
else
    echo "⚠️  代理验证返回: HTTP $HTTP_TEST"
fi

echo ""
echo "=== Clash 启动完成 ==="
echo ""
echo "📋 代理信息:"
echo "   HTTP/HTTPS: $PROXY_URL"
echo "   SOCKS5: $SOCKS5_URL"
echo "   PID: $CLASH_PID"
echo "   日志: $CLASH_LOG"
echo ""
echo "💡 要停止 Clash，运行: source clash-stop"