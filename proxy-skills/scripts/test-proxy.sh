#!/bin/bash
# 代理测试脚本 - 完整测试套件

echo "=== 代理完整测试套件 ==="
echo ""
echo "测试时间: $(date)"
echo ""

# 检查代理环境
echo "1️⃣ 代理环境检查"
echo "━━━━━━━━━━━━━━━━━━━━━━━━"
if [ -n "$http_proxy" ]; then
    echo "   ✅ 环境变量已设置: $http_proxy"
else
    echo "   ⚠️  环境变量未设置，使用默认测试: http://127.0.0.1:7890"
fi
echo ""

# 端口连通性
echo "2️⃣ 端口连通性测试"
echo "━━━━━━━━━━━━━━━━━━━━━━━━"
if netstat -tln 2>/dev/null | grep -q ":7890 " || ss -tln 2>/dev/null | grep -q ":7890 "; then
    echo "   ✅ 端口7890正在监听"
else
    echo "   ❌ 端口7890未监听"
    echo ""
    echo "   💡 请确保Clash代理正在运行"
    echo "   启动命令: ~/clash &"
    exit 1
fi
echo ""

# HTTP测试
echo "3️⃣ HTTP代理测试"
echo "━━━━━━━━━━━━━━━━━━━━━━━━"

echo "   测试1: httpbin.org (HTTP)"
START_TIME=$(date +%s%N)
HTTP1=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 -s -o /dev/null -w "%{http_code}" http://httpbin.org/ip 2>/dev/null)
END_TIME=$(date +%s%N)
TIME_MS=$(( (END_TIME - START_TIME) / 1000000 ))
if [ "$HTTP1" = "200" ]; then
    echo "   ✅ HTTP连接成功 (${TIME_MS}ms)"
else
    echo "   ❌ HTTP连接失败 ($HTTP1)"
    HTTP_FAILED=true
fi

echo "   测试2: google.com (HTTP)"
START_TIME=$(date +%s%N)
HTTP2=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 -s -o /dev/null -w "%{http_code}" http://www.google.com 2>/dev/null)
END_TIME=$(date +%s%N)
TIME_MS=$(( (END_TIME - START_TIME) / 1000000 ))
if [ "$HTTP2" = "200" ] || [ "$HTTP2" = "301" ] || [ "$HTTP2" = "302" ]; then
    echo "   ✅ Google HTTP连接成功 (${TIME_MS}ms)"
else
    echo "   ⚠️  Google HTTP连接结果: $HTTP2"
fi
echo ""

# HTTPS测试
echo "4️⃣ HTTPS代理测试"
echo "━━━━━━━━━━━━━━━━━━━━━━━━"

echo "   测试1: httpbin.org (HTTPS)"
START_TIME=$(date +%s%N)
HTTPS1=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 -s -o /dev/null -w "%{http_code}" https://httpbin.org/ip 2>/dev/null)
END_TIME=$(date +%s%N)
TIME_MS=$(( (END_TIME - START_TIME) / 1000000 ))
if [ "$HTTPS1" = "200" ]; then
    echo "   ✅ HTTPS连接成功 (${TIME_MS}ms)"
else
    echo "   ❌ HTTPS连接失败 ($HTTPS1)"
    HTTPS_FAILED=true
fi

echo "   测试2: github.com (HTTPS)"
START_TIME=$(date +%s%N)
HTTPS2=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 -s -o /dev/null -w "%{http_code}" https://github.com 2>/dev/null)
END_TIME=$(date +%s%N)
TIME_MS=$(( (END_TIME - START_TIME) / 1000000 ))
if [ "$HTTPS2" = "200" ] || [ "$HTTPS2" = "301" ] || [ "$HTTPS2" = "302" ]; then
    echo "   ✅ GitHub HTTPS连接成功 (${TIME_MS}ms)"
else
    echo "   ⚠️  GitHub HTTPS连接结果: $HTTPS2"
fi

echo "   测试3: google.com (HTTPS)"
START_TIME=$(date +%s%N)
HTTPS3=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 -s -o /dev/null -w "%{http_code}" https://www.google.com 2>/dev/null)
END_TIME=$(date +%s%N)
TIME_MS=$(( (END_TIME - START_TIME) / 1000000 ))
if [ "$HTTPS3" = "200" ] || [ "$HTTPS3" = "301" ] || [ "$HTTPS3" = "302" ]; then
    echo "   ✅ Google HTTPS连接成功 (${TIME_MS}ms)"
else
    echo "   ⚠️  Google HTTPS连接结果: $HTTPS3"
fi
echo ""

# DNS测试
echo "5️⃣ DNS解析测试（通过代理）"
echo "━━━━━━━━━━━━━━━━━━━━━━━━"
for host in "github.com" "google.com" "httpbin.org" "api.openai.com"; do
    START_TIME=$(date +%s%N)
    RESOLVED=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 -s -o /dev/null -w "%{time_namelookup}" https://$host 2>/dev/null)
    END_TIME=$(date +%s%N)
    TIME_MS=$(echo "$RESOLVED * 1000" | bc 2>/dev/null || echo "N/A")
    if [ "$TIME_MS" != "N/A" ] && (( $(echo "$TIME_MS > 0" | bc -l 2>/dev/null) )); then
        echo "   ✅ $host - DNS解析: ${TIME_MS}ms"
    else
        echo "   ⚠️  $host - DNS解析失败"
    fi
done
echo ""

# 速度测试
echo "6️⃣ 下载速度测试"
echo "━━━━━━━━━━━━━━━━━━━━━━━━"
echo "   测试下载小文件 (1MB)..."
START_TIME=$(date +%s%N)
SPEED=$(curl -x http://127.0.0.1:7890 --connect-timeout 10 --max-time 30 -s -o /dev/null -w "%{speed_download}" http://httpbin.org/bytes/1048576 2>/dev/null)
END_TIME=$(date +%s%N)
TIME_SEC=$(( (END_TIME - START_TIME) / 1000000000 ))
if [ "$TIME_SEC" -gt 0 ] 2>/dev/null; then
    SPEED_KB=$(echo "$SPEED / 1024" | bc 2>/dev/null || echo "N/A")
    echo "   下载速度: ${SPEED_KB}KB/s"
    echo "   耗时: ${TIME_SEC}s"
else
    echo "   下载速度测试失败"
fi
echo ""

# Git测试
echo "7️⃣ Git代理测试"
echo "━━━━━━━━━━━━━━━━━━━━━━━━"
GIT_HTTP_PROXY=$(git config --global http.proxy 2>/dev/null || echo "未设置")
GIT_HTTPS_PROXY=$(git config --global https.proxy 2>/dev/null || echo "未设置")
echo "   Git HTTP代理: $GIT_HTTP_PROXY"
echo "   Git HTTPS代理: $GIT_HTTPS_PROXY"

if [ "$GIT_HTTP_PROXY" = "http://127.0.0.1:7890" ]; then
    echo "   ✅ Git代理已配置"
else
    echo "   ⚠️  Git代理未配置为Clash"
fi
echo ""

# 总结
echo "📊 测试总结"
echo "━━━━━━━━━━━━━━━━━━━━━━━━"
PASS_COUNT=0
FAIL_COUNT=0

if [ -z "$HTTP_FAILED" ]; then PASS_COUNT=$((PASS_COUNT + 1)); else FAIL_COUNT=$((FAIL_COUNT + 1)); fi
if [ -z "$HTTPS_FAILED" ]; then PASS_COUNT=$((PASS_COUNT + 1)); else FAIL_COUNT=$((FAIL_COUNT + 1)); fi

echo "   通过: $PASS_COUNT 项"
echo "   失败: $FAIL_COUNT 项"
echo ""

if [ $FAIL_COUNT -eq 0 ]; then
    echo "🎉 所有测试通过！代理运行正常！"
    exit 0
else
    echo "⚠️  部分测试失败，请检查代理配置"
    exit 1
fi