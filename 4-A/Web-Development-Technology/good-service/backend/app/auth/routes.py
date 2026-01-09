"""
认证相关路由
"""
from flask import Blueprint, request, current_app
from app import db
from app.models import User
from app.utils.responses import success_response, error_response, created_response
from app.utils.captcha import generate_captcha, verify_captcha
from app.auth.utils import (
    hash_password, verify_password, generate_token, verify_token,
    validate_password_strength, validate_username, validate_phone
)
from app.utils.decorators import login_required
from flask import g

auth_bp = Blueprint('auth', __name__)


@auth_bp.route('/register', methods=['POST'])
def register():
    """用户注册"""
    data = request.get_json()

    # 验证必填字段
    username = data.get('username', '').strip()
    password = data.get('password', '').strip()
    phone = data.get('phone', '').strip()

    if not username or not password or not phone:
        return error_response('用户名、密码和手机号不能为空', 'INVALID_REQUEST')

    # 验证用户名
    is_valid, msg = validate_username(username)
    if not is_valid:
        return error_response(msg, 'INVALID_REQUEST')

    # 验证密码强度
    is_valid, msg = validate_password_strength(password)
    if not is_valid:
        return error_response(msg, 'WEAK_PASSWORD')

    # 验证手机号
    is_valid, msg = validate_phone(phone)
    if not is_valid:
        return error_response(msg, 'INVALID_PHONE')

    # 检查用户名是否已存在
    if User.query.filter_by(username=username).first():
        return error_response('用户名已存在', 'USER_EXISTS')

    # 检查手机号是否已存在
    if User.query.filter_by(phone=phone).first():
        return error_response('手机号已被注册', 'PHONE_EXISTS')

    # 检查邮箱是否已存在（如果提供）
    email = data.get('email')
    if email and User.query.filter_by(email=email).first():
        return error_response('邮箱已被注册', 'INVALID_EMAIL')

    # 创建用户
    try:
        user = User(
            username=username,
            password_hash=hash_password(password),
            phone=phone,
            email=email,
            address=data.get('address'),
            bio=data.get('bio')
        )

        db.session.add(user)
        db.session.commit()

        return created_response('注册成功', {
            'userId': user.id,
            'username': user.username
        })
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Registration error: {e}')
        return error_response('注册失败', 'SERVER_ERROR', 500)


@auth_bp.route('/login', methods=['POST'])
def login():
    """用户登录"""
    data = request.get_json()

    username = data.get('username', '').strip()
    password = data.get('password', '').strip()
    captcha = data.get('captcha', '').strip()
    captcha_id = data.get('captchaId', '').strip()

    # 验证必填字段
    if not username or not password:
        return error_response('用户名和密码不能为空', 'INVALID_REQUEST')

    if not captcha or not captcha_id:
        return error_response('请输入验证码', 'INVALID_REQUEST')

    # 验证验证码
    if not verify_captcha(captcha_id, captcha):
        return error_response('验证码错误', 'INVALID_CAPTCHA')

    # 查找用户
    user = User.query.filter_by(username=username).first()
    if not user:
        return error_response('用户名或密码错误', 'INVALID_CREDENTIALS')

    # 验证密码
    if not verify_password(password, user.password_hash):
        return error_response('用户名或密码错误', 'INVALID_CREDENTIALS')

    # 检查账号状态
    if user.status != 'active':
        return error_response('账号已被禁用', 'ACCOUNT_LOCKED')

    # 生成token
    access_token = generate_token(user.id, 'access')
    refresh_token = generate_token(user.id, 'refresh')

    return success_response('登录成功', {
        'token': access_token,
        'refreshToken': refresh_token,
        'user': user.to_dict()
    })


@auth_bp.route('/captcha', methods=['GET'])
def get_captcha():
    """获取验证码"""
    captcha_data = generate_captcha()
    return success_response('获取验证码成功', captcha_data)


@auth_bp.route('/captcha/refresh', methods=['POST'])
def refresh_captcha():
    """刷新验证码"""
    captcha_data = generate_captcha()
    return success_response('刷新验证码成功', captcha_data)


@auth_bp.route('/refresh', methods=['POST'])
def refresh():
    """刷新访问令牌"""
    data = request.get_json()
    refresh_token = data.get('refreshToken')

    if not refresh_token:
        return error_response('未提供刷新令牌', 'INVALID_REQUEST')

    # 验证refresh token
    user_id = verify_token(refresh_token, 'refresh')
    if not user_id:
        return error_response('刷新令牌无效或已过期', 'INVALID_REFRESH_TOKEN')

    # 查找用户
    user = User.query.get(user_id)
    if not user:
        return error_response('用户不存在', 'INVALID_REFRESH_TOKEN')

    if user.status != 'active':
        return error_response('账号已被禁用', 'ACCOUNT_LOCKED')

    # 生成新的token
    new_access_token = generate_token(user.id, 'access')
    new_refresh_token = generate_token(user.id, 'refresh')

    return success_response('刷新成功', {
        'token': new_access_token,
        'refreshToken': new_refresh_token
    })


@auth_bp.route('/logout', methods=['POST'])
@login_required
def logout():
    """用户登出"""
    # 实际应该将token加入黑名单（使用Redis）
    # 这里简单返回成功
    return success_response('登出成功')
