"""
用户相关API
"""
from flask import Blueprint, request, current_app, g
from app import db
from app.models import User
from app.utils.responses import success_response, error_response
from app.utils.decorators import login_required
from app.auth.utils import hash_password, verify_password, validate_password_strength, validate_phone

user_bp = Blueprint('user', __name__)


@user_bp.route('/profile', methods=['GET'])
@login_required
def get_profile():
    """获取用户信息"""
    user = g.current_user
    return success_response('获取成功', user.to_dict(include_stats=True))


@user_bp.route('/profile', methods=['PUT'])
@login_required
def update_profile():
    """更新用户信息"""
    user = g.current_user
    data = request.get_json()

    # 可更新的字段
    phone = data.get('phone')
    email = data.get('email')
    address = data.get('address')
    bio = data.get('bio')

    try:
        # 验证手机号
        if phone and phone != user.phone:
            is_valid, msg = validate_phone(phone)
            if not is_valid:
                return error_response(msg, 'INVALID_PHONE')

            # 检查手机号是否已被使用
            existing_user = User.query.filter_by(phone=phone).first()
            if existing_user and existing_user.id != user.id:
                return error_response('手机号已被使用', 'PHONE_EXISTS')

            user.phone = phone

        # 验证邮箱
        if email and email != user.email:
            # 简单的邮箱格式验证
            if '@' not in email or '.' not in email:
                return error_response('邮箱格式错误', 'INVALID_EMAIL')

            # 检查邮箱是否已被使用
            existing_user = User.query.filter_by(email=email).first()
            if existing_user and existing_user.id != user.id:
                return error_response('邮箱已被使用', 'INVALID_EMAIL')

            user.email = email

        # 更新地址
        if address is not None:
            if len(address) > 100:
                return error_response('地址最多100个字符', 'INVALID_REQUEST')
            user.address = address

        # 更新简介
        if bio is not None:
            if len(bio) > 200:
                return error_response('简介最多200个字符', 'INVALID_REQUEST')
            user.bio = bio

        db.session.commit()
        return success_response('更新成功', user.to_dict())

    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Update profile error: {e}')
        return error_response('更新失败', 'SERVER_ERROR', 500)


@user_bp.route('/password', methods=['PUT'])
@login_required
def update_password():
    """修改密码"""
    user = g.current_user
    data = request.get_json()

    old_password = data.get('oldPassword', '').strip()
    new_password = data.get('newPassword', '').strip()

    if not old_password or not new_password:
        return error_response('原密码和新密码不能为空', 'INVALID_REQUEST')

    # 验证原密码
    if not verify_password(old_password, user.password_hash):
        return error_response('原密码错误', 'WRONG_PASSWORD')

    # 验证新密码强度
    is_valid, msg = validate_password_strength(new_password)
    if not is_valid:
        return error_response(msg, 'WEAK_PASSWORD')

    # 检查新旧密码是否相同
    if old_password == new_password:
        return error_response('新密码不能与原密码相同', 'SAME_PASSWORD')

    try:
        user.password_hash = hash_password(new_password)
        db.session.commit()
        return success_response('密码修改成功')
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Update password error: {e}')
        return error_response('密码修改失败', 'SERVER_ERROR', 500)
