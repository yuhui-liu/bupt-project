"""
验证码生成
"""
from captcha.image import ImageCaptcha
import random
import string
import base64
from io import BytesIO
import uuid

# 验证码存储（实际生产环境应使用Redis）
captcha_store = {}


def generate_captcha():
    """生成验证码"""
    # 生成验证码ID
    captcha_id = str(uuid.uuid4())

    # 生成验证码文本（4位数字+字母）
    captcha_text = ''.join(random.choices(string.ascii_uppercase + string.digits, k=4))

    # 创建图片验证码
    image = ImageCaptcha(width=120, height=50)
    data = image.generate(captcha_text)

    # 转换为base64
    buffered = BytesIO()
    buffered.write(data.read())
    img_str = base64.b64encode(buffered.getvalue()).decode()
    img_base64 = f'data:image/png;base64,{img_str}'

    # 存储验证码（5分钟有效期）
    captcha_store[captcha_id] = {
        'text': captcha_text.lower(),
        'expires': 300  # 5分钟
    }

    return {
        'captchaId': captcha_id,
        'captchaImage': img_base64,
        'expiresIn': 300
    }


def verify_captcha(captcha_id, captcha_text):
    """验证验证码"""
    if not captcha_id or not captcha_text:
        return False

    captcha_data = captcha_store.get(captcha_id)
    if not captcha_data:
        return False

    # 验证文本（不区分大小写）
    is_valid = captcha_data['text'] == captcha_text.lower()

    # 验证后删除
    if captcha_id in captcha_store:
        del captcha_store[captcha_id]

    return is_valid


def cleanup_expired_captchas():
    """清理过期验证码（可以定期调用）"""
    # 简化实现，实际应该检查过期时间
    # 这里只是示例，生产环境应使用Redis的TTL
    pass
