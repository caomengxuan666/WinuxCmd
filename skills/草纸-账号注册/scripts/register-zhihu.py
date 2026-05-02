#!/usr/bin/env python3
"""
知乎注册助手
使用方法：
1. 先获取验证码（需要你告诉我手机号）
2. 你收到验证码后告诉我
3. 完成注册

需要安装: pip install requests
"""

import requests
import json
import sys
import re

ZHIHU_BASE = "https://www.zhihu.com"

class ZhihuSignup:
    def __init__(self):
        self.session = requests.Session()
        self.xsrf = None
        
    def init(self):
        """初始化会话，获取必要的cookie和xsrf"""
        r = self.session.get(f"{ZHIHU_BASE}/signup", timeout=10)
        
        # 从cookie中提取xsrf
        for cookie in self.session.cookies:
            if cookie.name == '_xsrf':
                self.xsrf = cookie.value
                break
        
        if not self.xsrf:
            # 尝试从页面提取
            match = re.search(r'name="xsrf"[^>]*value="([^"]*)"', r.text)
            if match:
                self.xsrf = match.group(1)
        
        print(f"✅ 会话已初始化 (xsrf: {self.xsrf[:8]}...)")
        return self.xsrf is not None
    
    def get_captcha(self):
        """获取图片验证码"""
        r = self.session.get(
            f"{ZHIHU_BASE}/api/v3/oauth/captcha?lang=en",
            timeout=10
        )
        data = r.json()
        if data.get("show_captcha", False):
            # 需要图片验证码
            r2 = self.session.put(
                f"{ZHIHU_BASE}/api/v3/oauth/captcha?lang=en",
                json={"input_text": "captcha"},
                timeout=10
            )
            if r2.status_code == 200 and "img_base64" in r2.json():
                print("⚠️ 需要图片验证码")
                print("   提示: 通常手机号注册时只需要短信验证码")
                return True
        return True  # 不需要图片验证码
    
    def send_sms(self, phone):
        """发送短信验证码"""
        headers = {
            "x-requested-with": "fetch",
            "Referer": f"{ZHIHU_BASE}/signup",
            "Content-Type": "application/x-www-form-urlencoded; charset=UTF-8",
        }
        data = {
            "phone": phone,
            "_xsrf": self.xsrf,
            "country_code": "86",
        }
        
        r = self.session.post(
            f"{ZHIHU_BASE}/api/v3/oauth/send_sms",
            headers=headers,
            data=data,
            timeout=10
        )
        
        if r.status_code == 200:
            print(f"✅ 验证码已发送到 {phone[:3]}****{phone[-4:]}")
            return True
        else:
            print(f"❌ 发送失败: {r.status_code} {r.text[:200]}")
            return False
    
    def signup(self, phone, code, password):
        """完成注册"""
        headers = {
            "x-requested-with": "fetch",
            "Referer": f"{ZHIHU_BASE}/signup",
            "Content-Type": "application/x-www-form-urlencoded; charset=UTF-8",
        }
        data = {
            "phone": phone,
            "code": code,
            "password": password,
            "_xsrf": self.xsrf,
            "country_code": "86",
            "fullname": "草纸",
        }
        
        r = self.session.post(
            f"{ZHIHU_BASE}/api/v3/oauth/signup",
            headers=headers,
            data=data,
            timeout=10
        )
        
        if r.status_code == 200:
            result = r.json()
            if "uid" in result.get("user", {}):
                print(f"✅ 注册成功！用户ID: {result['user']['uid']}")
                print(f"   用户名: 草纸")
                return True
            else:
                print(f"⚠️ 返回异常: {r.text[:200]}")
                return False
        else:
            print(f"❌ 注册失败: {r.status_code} {r.text[:200]}")
            return False

def main():
    signup = ZhihuSignup()
    
    if not signup.init():
        print("❌ 初始化失败")
        sys.exit(1)
    
    if len(sys.argv) < 2:
        print("用法:")
        print("  python3 register-zhihu.py send-sms <手机号>")
        print("  python3 register-zhihu.py signup <手机号> <验证码> <密码>")
        sys.exit(1)
    
    action = sys.argv[1]
    
    if action == "send-sms":
        if len(sys.argv) < 3:
            print("请提供手机号")
            sys.exit(1)
        phone = sys.argv[2]
        if not signup.get_captcha():
            sys.exit(1)
        signup.send_sms(phone)
    
    elif action == "signup":
        if len(sys.argv) < 5:
            print("请提供: 手机号 验证码 密码")
            sys.exit(1)
        phone = sys.argv[2]
        code = sys.argv[3]
        password = sys.argv[4]
        signup.signup(phone, code, password)
    
    else:
        print(f"未知操作: {action}")

if __name__ == "__main__":
    main()
