#!/bin/bash
# 账号注册助手
# 帮草猛注册各平台账号，他只需要收验证码

SIGNUP_DIR="/home/cmx/.openclaw/workspace/skills/草纸-账号注册"
ACCOUNTS_FILE="$SIGNUP_DIR/accounts.json"

# 更新账号状态
update_status() {
    local platform=$1 status=$2 note=$3
    python3 -c "
import json
with open('$ACCOUNTS_FILE') as f:
    data = json.load(f)
if '$platform' in data['accounts']:
    data['accounts']['$platform']['status'] = '$status'
    data['accounts']['$platform']['note'] = '$note'
else:
    data['accounts']['$platform'] = {'status': '$status', 'note': '$note'}
with open('$ACCOUNTS_FILE', 'w') as f:
    json.dump(data, f, indent=2, ensure_ascii=False)
"
}

case "$1" in
    zhihu)
        echo "📝 准备注册知乎..."
        echo "需要草猛做的事："
        echo "1. 打开 https://www.zhihu.com/signup"
        echo "2. 输入手机号"
        echo "3. 收验证码完成注册"
        update_status zhihu "ready" "已准备就绪，等草猛收验证码"
        ;;
    sspai)
        echo "📝 准备注册少数派..."
        echo "需要草猛做的事："
        echo "1. 打开 https://sspai.com 点右上角注册"
        echo "2. 输入邮箱"
        echo "3. 去邮箱收确认链接"
        update_status sspai "ready" "已准备就绪，等草猛确认邮箱"
        ;;
    status)
        python3 -c "
import json
with open('$ACCOUNTS_FILE') as f:
    data = json.load(f)
print('=== 账号注册状态 ===')
for platform, info in data['accounts'].items():
    icon = {'pending': '⏳', 'done': '✅', 'ready': '🔧', 'failed': '❌'}.get(info['status'], '❓')
    print(f'{icon} {platform}: {info[\"status\"]} — {info[\"note\"]}')
"
        ;;
    *)
        echo "用法: $0 {zhihu|sspai|status}"
        ;;
esac