#!/bin/bash
# 代理状态检查脚本

echo "=== 代理状态检查 ==="
echo ""

# 检查环境变量
echo "1. 环境变量检查:"
echo "   http_proxy: ${http_proxy:-未设置}"
echo "   https_proxy: ${https_proxy:-未设置}"
echo "   all_proxy: ${all_proxy:-未设置}"
echo "   HTTP_PROXY: ${HTTP_PROXY:-未设置}"
echo "   HTTPS_PROXY: ${HTTPS_PROXY:-未设置}"
echo "   ALL_PROXY: ${ALL_PROXY:-未设置}"
echo ""

# 检查端口7890
echo "2. 端口检查 (7890):"
if netstat -tln 2>/dev/null | grep -q ":7890 "; then
    echo "   ✅ 端口7890正在监听"
    PORT_STATUS="listening"
else
    if ss -tln 2>/dev/null | grep -q ":7890 "; then
        echo "   ✅ 端口7890正在监听 (使用ss命令)"
        PORT_STATUS="listening"
    else
        echo "   ❌ 端口7890未监听"
        PORT_STATUS="not_listening"
    fi
fi
echo ""

# 检查Clash进程
echo "3. Clash进程检查:"
if pgrep -x "clash" > /dev/null 2>&1; then
    echo "   ✅ Clash进程运行中"
    CLASH_STATUS="running"
elif pgrep -f "clash" > /dev/null 2>&1; then
    echo "   ✅ Clash相关进程运行中"
    CLASH_STATUS="related_running"
else
    echo "   ❌ Clash进程未运行"
    CLASH_STATUS="not_running"
fi
echo ""

# 测试连接
echo "4. 连接测试:"

# 测试HTTP代理
echo "   测试HTTP代理..."
if curl -x http://127.0.0.1:7890 --connect-timeout 5 -s http://httpbin.org/ip > /dev/null 2>&1; then
    echo "   ✅ HTTP代理连接成功"
    HTTP_TEST="success"
else
    echo "   ❌ HTTP代理连接失败"
    HTTP_TEST="failed"
fi

# 测试HTTPS代理
echo "   测试HTTPS代理..."
if curl -x http://127.0.0.1:7890 --connect-timeout 5 -s https://httpbin.org/ip > /dev/null 2>&1; then
    echo "   ✅ HTTPS代理连接成功"
    HTTPS_TEST="success"
else
    echo "   ❌ HTTPS代理连接失败"
    HTTPS_TEST="failed"
fi

echo ""

# 总结
echo "=== 检查总结 ==="
if [ "$PORT_STATUS" = "listening" ] && [ "$CLASH_STATUS" = "running" ] && [ "$HTTP_TEST" = "success" ]; then
    echo "✅ 代理状态: 正常"
    echo "   所有检查通过，代理可用"
    exit 0
else
    echo "⚠️  代理状态: 有问题"
    echo "   请检查以下项目:"
    [ "$PORT_STATUS" != "listening" ] && echo "   - 端口7890未监听"
    [ "$CLASH_STATUS" = "not_running" ] && echo "   - Clash进程未运行"
    [ "$HTTP_TEST" = "failed" ] && echo "   - HTTP代理连接失败"
    [ "$HTTPS_TEST" = "failed" ] && echo "   - HTTPS代理连接失败"
    echo ""
    echo "建议操作:"
    echo "1. 启动Clash: clash &"
    echo "2. 检查配置文件: ~/.config/clash/config.yaml"
    echo "3. 检查防火墙: sudo ufw status"
    exit 1
fi