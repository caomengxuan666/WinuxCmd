# Clash 代理技能

## 📋 技能描述
管理本地Clash代理（v1.18.0）的启动、停止和状态检查，自动配置系统代理环境变量。

## 🛠️ 工具

### 1. `clash-start` — 启动 Clash
启动Clash守护进程并配置系统代理。

### 2. `clash-stop` — 停止 Clash
停止Clash进程并清除代理设置。

### 3. `clash-status` — 查看状态
检查Clash进程、端口和代理连接状态。

### 4. `clash-restart` — 重启 Clash
重启Clash（先停止再启动）。

## ⚡ 快速使用

```bash
# 启动 Clash 并启用代理
source clash-start

# 查看状态
clash-status

# 停止 Clash
source clash-stop

# 重启
source clash-restart
```

## 🔧 技术实现

### Clash 配置
- 执行路径: `/usr/local/bin/clash`
- 版本: v1.18.0
- 工作目录: `~/.config/clash/`
- 配置文件: `~/.config/clash/config.yaml`
- 混合端口: 7890
- GeoIP: `~/.config/clash/Country.mmdb`

### Clash Verge 配置（GUI）
- 目录: `~/io.github.clash-verge-rev.clash-verge-rev/`
- 配置: `clash-verge.yaml`（端口7897，direct模式）
- 订阅配置文件在 `profiles/` 目录

### 代理设置
| 变量 | 值 |
|------|-----|
| http_proxy | http://127.0.0.1:7890 |
| https_proxy | http://127.0.0.1:7890 |
| all_proxy | socks5://127.0.0.1:7890 |
| Git http.proxy | http://127.0.0.1:7890 |
| Git https.proxy | http://127.0.0.1:7890 |

## 📁 文件结构
```
clash-skills/
├── SKILL.md
└── scripts/
    ├── clash-start.sh      # 启动 Clash
    ├── clash-stop.sh       # 停止 Clash
    ├── clash-status.sh     # 状态检查
    └── clash-restart.sh    # 重启 Clash
```

## 🐛 故障排除
- 检查日志: `tail -f ~/.config/clash/clash.log`
- 验证端口: `netstat -tlnp | grep 7890`
- 测试代理: `curl -x http://127.0.0.1:7890 https://httpbin.org/ip`
