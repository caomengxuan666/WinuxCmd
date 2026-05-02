#!/bin/bash
# Clash 状态检查脚本

CLASH_BIN="/usr/local/bin/clash"
CLASH_PID_FILE="/tmp/clash.pid"
CLASH_LOG="$HOME/.config/clash/clash.log"
MIXED_PORT=7890
PROXY_URL="http://127.0.0.1:$MIXED_PORT"

echo "=== Clash 状态报告 ==="
echo ""

# 1. 版本
echo "1️⃣  Clash 版本"
echo "━━━━━━━━━━━━━━━━"
if [ -f "$CLASH_BIN" ]; then
    VERSION=$("$CLASH_BIN" -v 2>/dev/null | head -1)
    echo "   ✅ $VERSION"
else
    echo "   ❌ 二进制文件未找到: $CLASH_BIN"
fi
echo ""

# 2. 进程状态
echo "2️⃣  进程状态"
echo "━━━━━━━━━━━━━━━━"
if [ -f "$CLASH_PID_FILE" ] && kill -0 $(cat "$CLASH_PID_FILE") 2>/dev/null; then
    CLASH_PID=$(cat "$CLASH_PID_FILE")
    echo "   ✅ 运行中 (PID: $CLASH_PID)"
else
    # 尝试 pgrep
    CLASH_PID=$(pgrep -x "clash" 2>/dev/null)
    if [ -n "$CLASH_PID" ]; then
        echo "   ✅ 运行中 (PID: $CLASH_PID, 但 PID 文件不存在)"
    else
        echo "   ❌ 未运行"
    fi
fi
echo ""

# 3. 端口监听
echo "3️⃣  端口监听"
echo "━━━━━━━━━━━━━━━━"
if netstat -tln 2>/dev/null | grep -q ":$MIXED_PORT " || ss -tln 2>/dev/null | grep -q ":$MIXED_PORT "; then
    echo "   ✅ 端口 $MIXED_PORT 正在监听"
else
    echo "   ❌ 端口 $MIXED_PORT 未监听"
fi
echo ""

# 4. 代理环境变量
echo "4️⃣  代理环境变量"
echo "━━━━━━━━━━━━━━━━"
echo "   http_proxy: ${http_proxy:-❌ 未设置}"
echo "   https_proxy: ${https_proxy:-❌ 未设置}"
echo "   all_proxy: ${all_proxy:-❌ 未设置}"
echo "   HTTP_PROXY: ${HTTP_PROXY:-❌ 未设置}"
echo "   HTTPS_PROXY: ${HTTPS_PROXY:-❌ 未设置}"
echo "   ALL_PROXY: ${ALL_PROXY:-❌ 未设置}"
echo ""

# 5. Git 代理
echo "5️⃣  Git 代理"
echo "━━━━━━━━━━━━━━━━"
GIT_HTTP=$(git config --global http.proxy 2>/dev/null || echo "未设置")
GIT_HTTPS=$(git config --global https.proxy 2>/dev/null || echo "未设置")
echo "   http.proxy: $GIT_HTTP"
echo "   https.proxy: $GIT_HTTPS"
echo ""

# 6. 连接测试
echo "6️⃣  连接测试"
echo "━━━━━━━━━━━━━━━━"
if [ -n "${http_proxy:-}" ] || netstat -tln 2>/dev/null | grep -q ":$MIXED_PORT " || ss -tln 2>/dev/null | grep -q ":$MIXED_PORT "; then
    echo "   测试 httpbin.org..."
    HTTP_CODE=$(curl -x "$PROXY_URL" --connect-timeout 10 -s -o /dev/null -w "%{http_code}" http://httpbin.org/ip 2>/dev/null)
    if [ "$HTTP_CODE" = "200" ]; then
        echo "   ✅ HTTP 代理正常 (HTTP $HTTP_CODE)"
    else
        echo "   ⚠️  HTTP 代理返回: $HTTP_CODE"
    fi

    echo "   测试 github.com..."
    GH_CODE=$(curl -x "$PROXY_URL" --connect-timeout 10 -s -o /dev/null -w "%{http_code}" https://github.com 2>/dev/null)
    if [ "$GH_CODE" = "200" ] || [ "$GH_CODE" = "301" ] || [ "$GH_CODE" = "302" ]; then
        echo "   ✅ GitHub 可访问 (HTTP $GH_CODE)"
    else
        echo "   ⚠️  GitHub 返回: $GH_CODE"
    fi
else
    echo "   ⚠️  代理不可用，跳过连接测试"
fi
echo ""

# 7. 日志
echo "7️⃣  日志摘要"
echo "━━━━━━━━━━━━━━━━"
if [ -f "$CLASH_LOG" ]; then
    echo "   日志文件: $CLASH_LOG"
    echo "   最后5行:"
    tail -5 "$CLASH_LOG" | while read line; do
        echo "   $line"
    done
else
    echo "   日志文件不存在"
fi
echo ""

# 总结
echo "📊 总结"
echo "━━━━━━━━━━━━━━━━"
if (netstat -tln 2>/dev/null | grep -q ":$MIXED_PORT " || ss -tln 2>/dev/null | grep -q ":$MIXED_PORT ") && \
   ([ -n "${http_proxy:-}" ]); then
    echo "✅ Clash 运行中，代理已启用"
    echo "   端口: $MIXED_PORT"
    echo "   HTTP/HTTPS: $PROXY_URL"
    echo "   SOCKS5: socks5://127.0.0.1:$MIXED_PORT"
elif (netstat -tln 2>/dev/null | grep -q ":$MIXED_PORT " || ss -tln 2>/dev/null | grep -q ":$MIXED_PORT "); then
    echo "⚠️  Clash 运行中，但代理环境变量未设置"
    echo "   请运行: source clash-start"
else
    echo "❌ Clash 未运行"
    echo "   请运行: source clash-start"
fi