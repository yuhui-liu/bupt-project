"""
装饰器
"""
from functools import wraps
from flask import request, current_app, g
import jwt
from app.models import User
from app.utils.responses import error_response


def login_required(f):
    """登录验证装饰器"""
    @wraps(f)
    def decorated_function(*args, **kwargs):
        # 获取token
        auth_header = request.headers.get('Authorization')
        if not auth_header:
            return error_response('未提供认证令牌', 'UNAUTHORIZED', 401)

        try:
            # 解析Bearer token
            parts = auth_header.split()
            if len(parts) != 2 or parts[0].lower() != 'bearer':
                return error_response('认证令牌格式错误', 'UNAUTHORIZED', 401)

            token = parts[1]

            # 验证token
            payload = jwt.decode(
                token,
                current_app.config['JWT_SECRET_KEY'],
                algorithms=[current_app.config['JWT_ALGORITHM']]
            )

            user_id = payload.get('user_id')
            if not user_id:
                return error_response('认证令牌无效', 'UNAUTHORIZED', 401)

            # 查找用户
            user = User.query.get(user_id)
            if not user:
                return error_response('用户不存在', 'UNAUTHORIZED', 401)

            if user.status != 'active':
                return error_response('账号已被禁用', 'ACCOUNT_LOCKED', 403)

            # 将用户存储到g对象中
            g.current_user = user

        except jwt.ExpiredSignatureError:
            return error_response('认证令牌已过期', 'TOKEN_EXPIRED', 401)
        except jwt.InvalidTokenError:
            return error_response('认证令牌无效', 'UNAUTHORIZED', 401)
        except Exception as e:
            current_app.logger.error(f'Token verification error: {e}')
            return error_response('认证失败', 'UNAUTHORIZED', 401)

        return f(*args, **kwargs)

    return decorated_function


def admin_required(f):
    """管理员权限验证装饰器"""
    @wraps(f)
    @login_required
    def decorated_function(*args, **kwargs):
        if g.current_user.role != 'admin':
            return error_response('仅管理员可访问', 'ADMIN_ONLY', 403)
        return f(*args, **kwargs)

    return decorated_function
