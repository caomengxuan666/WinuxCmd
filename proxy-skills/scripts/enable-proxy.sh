#!/bin/bash
# 启用代理脚本

echo "=== 启用代理设置 ==="
echo ""

# 设置环境变量
export http_proxy=http://127.0.0.1:7890
export https_proxy=http://127.0.0.1:7890
export all_proxy=socks5://127.0.0.1:7890
export HTTP_PROXY=http://127.0.0.1:7890
export HTTPS_PROXY=http://127.0.0.1:7890
export ALL_PROXY=socks5://127.0.0.1:7890

echo "✅ 环境变量已设置:"
echo "   http_proxy=$http_proxy"
echo "   https_proxy=$https_proxy"
echo "   all_proxy=$all_proxy"
echo "   HTTP_PROXY=$HTTP_PROXY"
echo "   HTTPS_PROXY=$HTTPS_PROXY"
echo "   ALL_PROXY=$ALL_PROXY"
echo ""

# 设置Git代理
echo "设置Git代理..."
git config --global http.proxy http://127.0.0.1:7890 2>/dev/null
git config --global https.proxy http://127.0.0.1:7890 2>/dev/null

if [ $? -eq 0 ]; then
    echo "✅ Git代理已配置"
else
    echo "⚠️  Git代理配置失败（可能没有git命令）"
fi
echo ""

# 设置npm代理（如果存在）
if command -v npm &> /dev/null; then
    echo "设置npm代理..."
    npm config set proxy http://127.0.0.1:7890 2>/dev/null
    npm config set https-proxy http://127.0.0.1:7890 2>/dev/null
    echo "✅ npm代理已配置"
    echo ""
fi

# 验证设置
echo "验证代理设置..."
echo "   测试HTTP连接..."
HTTP_RESULT=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 -s http://httpbin.org/ip 2>/dev/null || echo "FAILED")

if [ "$HTTP_RESULT" != "FAILED" ] && echo "$HTTP_RESULT" | grep -q "origin"; then
    ORIGIN=$(echo "$HTTP_RESULT" | grep -o '"origin":"[^"]*"' | cut -d'"' -f4)
    echo "   ✅ HTTP代理连接成功"
    echo "   您的IP: $ORIGIN"
else
    echo "   ❌ HTTP代理连接失败"
fi

echo "   测试HTTPS连接..."
HTTPS_RESULT=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 -s https://httpbin.org/ip 2>/dev/null || echo "FAILED")

if [ "$HTTPS_RESULT" != "FAILED" ] && echo "$HTTPS_RESULT" | grep -q "origin"; then
    echo "   ✅ HTTPS代理连接成功"
else
    echo "   ❌ HTTPS代理连接失败"
fi

echo ""
echo "=== 代理启用完成 ==="
echo ""
echo "📋 当前代理设置:"
echo "   HTTP代理: http://127.0.0.1:7890"
echo "   HTTPS代理: http://127.0.0.1:7890"
echo "   SOCKS5代理: socks5://127.0.0.1:7890"
echo ""
echo "💡 提示:"
echo "   这些设置只在当前shell会话中有效"
echo "   要永久生效，请将设置添加到 ~/.bashrc 或 ~/.zshrc"
echo ""
echo "🔄 要禁用代理，运行: disable-proxy.sh"