"""
认证相关工具函数
"""
import bcrypt
import jwt
from datetime import datetime, timedelta
from flask import current_app


def hash_password(password):
    """密码哈希"""
    salt = bcrypt.gensalt()
    return bcrypt.hashpw(password.encode('utf-8'), salt).decode('utf-8')


def verify_password(password, password_hash):
    """验证密码"""
    return bcrypt.checkpw(password.encode('utf-8'), password_hash.encode('utf-8'))


def generate_token(user_id, token_type='access'):
    """生成JWT token"""
    if token_type == 'access':
        expires = current_app.config['JWT_ACCESS_TOKEN_EXPIRES']
    else:  # refresh
        expires = current_app.config['JWT_REFRESH_TOKEN_EXPIRES']

    payload = {
        'user_id': user_id,
        'type': token_type,
        'exp': datetime.utcnow() + expires,
        'iat': datetime.utcnow()
    }

    token = jwt.encode(
        payload,
        current_app.config['JWT_SECRET_KEY'],
        algorithm=current_app.config['JWT_ALGORITHM']
    )

    return token


def verify_token(token, token_type='access'):
    """验证JWT token"""
    try:
        payload = jwt.decode(
            token,
            current_app.config['JWT_SECRET_KEY'],
            algorithms=[current_app.config['JWT_ALGORITHM']]
        )

        if payload.get('type') != token_type:
            return None

        return payload.get('user_id')
    except jwt.ExpiredSignatureError:
        return None
    except jwt.InvalidTokenError:
        return None


def validate_password_strength(password):
    """验证密码强度"""
    if len(password) < 6:
        return False, '密码长度至少6个字符'
    if len(password) > 20:
        return False, '密码长度最多20个字符'
    return True, ''


def validate_username(username):
    """验证用户名"""
    if len(username) < 3:
        return False, '用户名长度至少3个字符'
    if len(username) > 20:
        return False, '用户名长度最多20个字符'
    if not username.isalnum():
        return False, '用户名只能包含字母和数字'
    return True, ''


def validate_phone(phone):
    """验证手机号"""
    if not phone:
        return False, '手机号不能为空'
    if len(phone) != 11:
        return False, '手机号必须是11位数字'
    if not phone.isdigit():
        return False, '手机号只能包含数字'
    return True, ''
