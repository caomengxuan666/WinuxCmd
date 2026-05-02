#!/bin/bash
# 禁用代理脚本

echo "=== 禁用代理设置 ==="
echo ""

# 清除环境变量
unset http_proxy
unset https_proxy
unset all_proxy
unset HTTP_PROXY
unset HTTPS_PROXY
unset ALL_PROXY
unset proxy

echo "✅ 环境变量已清除:"
echo "   http_proxy: 已清除"
echo "   https_proxy: 已清除"
echo "   all_proxy: 已清除"
echo "   HTTP_PROXY: 已清除"
echo "   HTTPS_PROXY: 已清除"
echo "   ALL_PROXY: 已清除"
echo ""

# 清除Git代理
echo "清除Git代理..."
git config --global --unset http.proxy 2>/dev/null
git config --global --unset https.proxy 2>/dev/null

if [ $? -eq 0 ]; then
    echo "✅ Git代理已清除"
else
    echo "ℹ️  Git代理可能未设置"
fi
echo ""

# 清除npm代理（如果存在）
if command -v npm &> /dev/null; then
    echo "清除npm代理..."
    npm config delete proxy 2>/dev/null
    npm config delete https-proxy 2>/dev/null
    echo "✅ npm代理已清除"
    echo ""
fi

echo "=== 代理禁用完成 ==="
echo ""
echo "📋 当前状态:"
echo "   所有代理设置已清除"
echo "   网络流量将使用直接连接"
echo ""
echo "💡 提示:"
echo "   这些设置只在当前shell会话中有效"
echo ""
echo "🔄 要启用代理，运行: enable-proxy.sh"