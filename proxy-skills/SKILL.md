# 代理技能 (Proxy Skills)

## 🎯 技能描述
为OpenClaw提供代理配置和管理功能，支持HTTP/HTTPS/SOCKS5代理，特别针对Clash代理（端口7890）进行优化。

## 📋 使用场景
1. 当需要访问被墙的外部资源时
2. 当GitHub克隆/推送失败时
3. 当需要稳定的网络连接时
4. 当执行需要代理的自动化任务时

## 🛠️ 工具配置

### 1. 代理状态检查
```bash
# 检查代理端口是否可用
check-proxy-port

# 测试代理连接
test-proxy-connection

# 查看当前代理设置
show-proxy-settings
```

### 2. 代理启用/禁用
```bash
# 启用代理（使用Clash 7890端口）
enable-proxy

# 禁用代理
disable-proxy

# 切换代理状态
toggle-proxy
```

### 3. 代理测试
```bash
# 测试HTTP代理
test-http-proxy

# 测试HTTPS代理  
test-https-proxy

# 测试SOCKS5代理
test-socks5-proxy

# 完整代理测试套件
full-proxy-test
```

## 🔧 技术实现

### 代理配置
```bash
# HTTP/HTTPS代理
export http_proxy=http://127.0.0.1:7890
export https_proxy=http://127.0.0.1:7890
export HTTP_PROXY=http://127.0.0.1:7890
export HTTPS_PROXY=http://127.0.0.1:7890

# SOCKS5代理
export all_proxy=socks5://127.0.0.1:7890
export ALL_PROXY=socks5://127.0.0.1:7890

# Git代理配置
git config --global http.proxy http://127.0.0.1:7890
git config --global https.proxy http://127.0.0.1:7890
```

### 环境检测
```bash
# 检测当前代理状态
if [ -n "$http_proxy" ]; then
    echo "代理已启用: $http_proxy"
else
    echo "代理未启用"
fi

# 检测Clash进程
if pgrep -x "clash" > /dev/null; then
    echo "Clash进程运行中"
else
    echo "Clash进程未运行"
fi
```

## 📁 文件结构
```
proxy-skills/
├── SKILL.md                    # 技能说明文档
├── scripts/                    # 脚本目录
│   ├── enable-proxy.sh        # 启用代理脚本
│   ├── disable-proxy.sh       # 禁用代理脚本
│   ├── check-proxy.sh         # 检查代理脚本
│   └── test-proxy.sh          # 测试代理脚本
├── config/                    # 配置文件
│   ├── proxy-env.sh          # 代理环境配置
│   └── git-proxy-config.sh   # Git代理配置
└── docs/                     # 文档
    ├── setup-guide.md        # 设置指南
    └── troubleshooting.md    # 故障排除
```

## 🚀 快速开始

### 安装技能
```bash
# 克隆技能仓库（如果需要）
git clone <proxy-skills-repo>

# 或直接使用本地技能
openclaw skill install ./proxy-skills
```

### 基本使用
```bash
# 1. 检查代理状态
check-proxy

# 2. 启用代理
enable-proxy

# 3. 测试代理
test-proxy-connection

# 4. 执行需要代理的任务
# （例如：克隆GitHub仓库）
```

## 🔍 代理检查脚本

### check-proxy.sh
```bash
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
if netstat -tln | grep -q ":7890 "; then
    echo "   ✅ 端口7890正在监听"
else
    echo "   ❌ 端口7890未监听"
fi
echo ""

# 检查Clash进程
echo "3. Clash进程检查:"
if pgrep -x "clash" > /dev/null; then
    echo "   ✅ Clash进程运行中"
else
    echo "   ❌ Clash进程未运行"
fi
echo ""

# 测试连接
echo "4. 连接测试:"
echo "   测试HTTP代理..."
if curl -x http://127.0.0.1:7890 --connect-timeout 5 -s http://httpbin.org/ip > /dev/null 2>&1; then
    echo "   ✅ HTTP代理连接成功"
else
    echo "   ❌ HTTP代理连接失败"
fi

echo "   测试HTTPS代理..."
if curl -x http://127.0.0.1:7890 --connect-timeout 5 -s https://httpbin.org/ip > /dev/null 2>&1; then
    echo "   ✅ HTTPS代理连接成功"
else
    echo "   ❌ HTTPS代理连接失败"
fi
```

## ⚡ 代理启用脚本

### enable-proxy.sh
```bash
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
echo ""

# 设置Git代理
echo "设置Git代理..."
git config --global http.proxy http://127.0.0.1:7890
git config --global https.proxy http://127.0.0.1:7890

echo "✅ Git代理已配置"
echo ""

# 验证设置
echo "验证代理设置..."
curl -x http://127.0.0.1:7890 --connect-timeout 5 -s http://httpbin.org/ip | grep -o '"origin":"[^"]*"' || echo "❌ 代理测试失败"

echo ""
echo "=== 代理启用完成 ==="
```

## 🔄 集成到OpenClaw

### 1. 技能配置
在OpenClaw配置中添加代理技能：
```yaml
skills:
  - name: proxy-skills
    path: /home/cmx/.openclaw/workspace/proxy-skills
    enabled: true
```

### 2. 自动代理管理
```bash
# 在任务开始时自动启用代理
pre-task:
  - enable-proxy
  
# 在任务结束时自动禁用代理  
post-task:
  - disable-proxy
```

### 3. 条件代理
```bash
# 只在需要时启用代理
if [[ "$TASK_NEEDS_PROXY" == "true" ]]; then
    enable-proxy
fi
```

## 🐛 故障排除

### 常见问题

#### 1. 端口7890未监听
```bash
# 检查Clash是否运行
ps aux | grep clash

# 启动Clash
# （根据您的Clash安装方式）
```

#### 2. 代理连接失败
```bash
# 测试本地连接
curl http://127.0.0.1:7890

# 检查防火墙
sudo ufw status

# 检查代理配置
cat ~/.config/clash/config.yaml | head -20
```

#### 3. Git仍然无法连接
```bash
# 检查Git代理设置
git config --global --get http.proxy
git config --global --get https.proxy

# 临时禁用SSL验证（仅测试用）
export GIT_SSL_NO_VERIFY=1
```

### 调试命令
```bash
# 详细调试
DEBUG_PROXY=1 check-proxy

# 查看所有网络连接
netstat -tulpn | grep 7890

# 查看系统日志
journalctl -u clash --since "5 minutes ago"
```

## 📈 性能优化

### 代理缓存
```bash
# 启用代理缓存
export http_proxy="http://127.0.0.1:7890"
export https_proxy="http://127.0.0.1:7890"

# 设置超时
export CURL_TIMEOUT=30
export WGET_TIMEOUT=30
```

### 连接池优化
```bash
# 保持连接
export CURL_MAX_CONNECTS=10
export CURL_MAX_TIME=300
```

## 🔒 安全考虑

### 1. 代理认证
```bash
# 如果需要认证
export http_proxy="http://username:password@127.0.0.1:7890"
```

### 2. 敏感信息保护
```bash
# 不记录代理密码
unset PROXY_PASSWORD
history -c
```

### 3. 本地代理限制
```bash
# 只允许本地连接
iptables -A INPUT -p tcp --dport 7890 -s 127.0.0.1 -j ACCEPT
iptables -A INPUT -p tcp --dport 7890 -j DROP
```

## 🌐 支持的协议

### HTTP/HTTPS代理
- 支持标准HTTP/HTTPS流量
- 支持WebSocket over HTTP
- 支持HTTP/2

### SOCKS5代理
- 支持TCP连接
- 支持UDP转发
- 支持身份验证

### 特殊协议支持
- SSH over HTTP (需要特殊配置)
- Git over SSH (需要特殊配置)
- Docker over HTTP (需要特殊配置)

## 🎯 使用示例

### 示例1: 克隆GitHub仓库
```bash
# 启用代理
enable-proxy

# 克隆仓库
git clone https://github.com/caomengxuan666/WinuxCmd.git

# 禁用代理
disable-proxy
```

### 示例2: 访问被墙的API
```bash
# 启用代理
enable-proxy

# 调用API
curl https://api.openai.com/v1/chat/completions

# 禁用代理
disable-proxy
```

### 示例3: 自动化任务
```bash
#!/bin/bash
# 自动化脚本示例

# 启用代理
source ./scripts/enable-proxy.sh

# 执行需要代理的任务
npm install
git push origin main
wget https://example.com/large-file.zip

# 禁用代理
source ./scripts/disable-proxy.sh
```

## 📚 相关资源

### 文档链接
- [Clash官方文档](https://github.com/Dreamacro/clash)
- [Clash Verge Rev文档](https://github.com/clash-verge-rev/clash-verge-rev)
- [OpenClaw网络配置](https://docs.openclaw.ai/configuration/network)

### 工具推荐
- **代理测试工具**: `curl`, `wget`, `httpie`
- **网络诊断**: `netstat`, `ss`, `lsof`
- **进程管理**: `pgrep`, `pkill`, `systemctl`

## 🔄 更新日志

### v1.0.0 (2026-04-23)
- 初始版本发布
- 支持Clash代理 (端口7890)
- 提供完整的代理管理功能
- 集成到OpenClaw技能系统

### v1.1.0 (计划中)
- 支持多代理配置
- 自动代理切换
- 代理健康检查
- 性能监控

## 🆘 获取帮助

### 问题反馈
1. 检查代理状态: `check-proxy`
2. 查看日志: `tail -f ~/.openclaw/logs/proxy.log`
3. 提交Issue: [GitHub Issues]

### 社区支持
- OpenClaw Discord: [链接]
- GitHub Discussions: [链接]
- 技术论坛: [链接]

---

**技能名称**: Proxy Skills  
**版本**: v1.0.0  
**作者**: OpenClaw AI Assistant  
**创建时间**: 2026-04-23  
**适用场景**: 需要代理访问的网络任务  
**依赖**: Clash代理 (端口7890)  
**许可证**: MIT