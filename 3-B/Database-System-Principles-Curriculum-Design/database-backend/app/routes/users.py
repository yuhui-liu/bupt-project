from datetime import datetime
from flask import Blueprint, request, jsonify
from flask_jwt_extended import jwt_required, get_jwt_identity
from app.models import User, db, UserToBePassed

users_blueprint = Blueprint("users", __name__)


@users_blueprint.route("/api/myinfo", methods=["GET"])
@jwt_required()
def get_my_info():
    """
    获取当前用户信息。

    返回当前登录用户的详细信息，包括用户名、邮箱、管理员状态等。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取用户信息。
        schema:
          type: object
          properties:
            username:
              type: string
              description: 用户名
            email:
              type: string
              description: 邮箱地址
            is_admin:
              type: boolean
              description: 是否为管理员
            apply_date:
              type: string
              format: date-time
              description: 申请日期
            pass_date:
              type: string
              format: date-time
              description: 审核通过日期
          required:
            - username
            - email
            - is_admin
            - apply_date
            - pass_date
      401:
        description: 未授权，需要有效的JWT token。
        schema:
          type: object
          properties:
            msg:
              type: string
              description: 认证错误信息
          required:
            - msg
      404:
        description: 用户不存在。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    produces:
      - application/json
    security:
      - Bearer: []
    """
    current_user = get_jwt_identity()
    user = User.query.filter_by(username=current_user).first()

    if user:
        return (
            jsonify(
                {
                    "username": user.username,
                    "email": user.email,  # 新增：返回邮箱
                    "is_admin": user.is_admin,
                    "apply_date": user.apply_date,
                    "pass_date": user.pass_date,
                }
            ),
            200,
        )

    return jsonify({"error": "用户不存在"}), 404


@users_blueprint.route("/api/userinfo", methods=["GET"])
@jwt_required()
def get_user_info():
    """
    获取所有用户信息。

    仅管理员可访问，返回所有已注册用户的信息列表，包括用户详细信息和管理员状态。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取所有用户信息。
        schema:
          type: array
          items:
            type: object
            properties:
              key:
                type: integer
                description: 用户ID
              username:
                type: string
                description: 用户名
              email:
                type: string
                description: 邮箱地址
              applyDate:
                type: string
                format: date-time
                description: 申请日期
              passDate:
                type: string
                format: date-time
                description: 审核通过日期
              isAdmin:
                type: boolean
                description: 是否为管理员
            required:
              - key
              - username
              - email
              - applyDate
              - passDate
              - isAdmin
      401:
        description: 未授权，需要有效的JWT token。
        schema:
          type: object
          properties:
            msg:
              type: string
              description: 认证错误信息
          required:
            - msg
      403:
        description: 权限不足，需要管理员权限。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    produces:
      - application/json
    security:
      - Bearer: []
    """
    current_user = get_jwt_identity()
    user = User.query.filter_by(username=current_user).first()

    if user and user.is_admin:
        users = User.query.all()
        user_list = [
            {
                "key": u.id,
                "username": u.username,
                "email": u.email,  # 新增：返回邮箱
                "applyDate": u.apply_date,
                "passDate": u.pass_date,
                "isAdmin": u.is_admin,
            }
            for u in users
        ]
        return jsonify(user_list), 200
    else:
        return jsonify({"error": "权限不足"}), 403


@users_blueprint.route("/api/waitlist", methods=["GET"])
@jwt_required()
def get_waitlist():
    """
    获取待审核用户列表。

    仅管理员可访问，返回所有待审核用户的信息列表，用于管理员进行用户审核操作。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取待审核用户列表。
        schema:
          type: array
          items:
            type: object
            properties:
              key:
                type: integer
                description: 用户ID
              email:
                type: string
                description: 邮箱地址
              username:
                type: string
                description: 用户名
              applyDate:
                type: string
                format: date-time
                description: 申请日期
            required:
              - key
              - email
              - username
              - applyDate
      401:
        description: 未授权，需要有效的JWT token。
        schema:
          type: object
          properties:
            msg:
              type: string
              description: 认证错误信息
          required:
            - msg
      403:
        description: 权限不足，需要管理员权限。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    produces:
      - application/json
    security:
      - Bearer: []
    """
    current_user = get_jwt_identity()
    user = User.query.filter_by(username=current_user).first()

    if user and user.is_admin:
        waitlist = UserToBePassed.query.all()
        waitlist_info = [
            {
                "key": u.id,
                "email": u.email,
                "username": u.username,
                "applyDate": u.apply_date,
            }
            for u in waitlist
        ]
        return jsonify(waitlist_info), 200
    else:
        return jsonify({"error": "权限不足"}), 403


@users_blueprint.route("/api/pass", methods=["POST"])
@jwt_required()
def pass_user():
    """
    通过用户审核。

    仅管理员可操作，将待审核用户移至正式用户表，完成用户注册流程。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: body
        in: body
        schema:
          type: object
          properties:
            username:
              type: string
              description: 待审核用户的用户名
          required:
            - username
    responses:
      201:
        description: 用户审核通过，成功移至正式用户表。
      401:
        description: 未授权，需要有效的JWT token。
        schema:
          type: object
          properties:
            msg:
              type: string
              description: 认证错误信息
          required:
            - msg
      403:
        description: 权限不足，需要管理员权限。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      404:
        description: 用户不在待审核列表中。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    consumes:
      - application/json
    produces:
      - application/json
    security:
      - Bearer: []
    """
    current_user = get_jwt_identity()
    user = User.query.filter_by(username=current_user).first()

    if user and user.is_admin:
        data = request.get_json()
        username = data["username"]

        # 检查用户是否在待审核列表中
        user_to_pass = UserToBePassed.query.filter_by(username=username).first()
        if not user_to_pass:
            return jsonify({"error": "用户不在待审核列表中"}), 404

        # 创建新用户并删除待审核用户
        new_user = User(
            username=user_to_pass.username,
            email=user_to_pass.email,
            apply_date=user_to_pass.apply_date,
        )
        # 直接设置已经哈希过的密码，不要再次哈希
        new_user.password_hash = user_to_pass.password_hash
        db.session.add(new_user)
        db.session.delete(user_to_pass)
        db.session.commit()
        return "", 201
    else:
        return jsonify({"error": "权限不足"}), 403


@users_blueprint.route("/api/reject", methods=["POST"])
@jwt_required()
def reject_user():
    """
    拒绝用户审核。

    仅管理员可操作，删除待审核用户，拒绝其注册申请。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: body
        in: body
        schema:
          type: object
          properties:
            username:
              type: string
              description: 待拒绝用户的用户名
          required:
            - username
    responses:
      204:
        description: 用户审核被拒绝，已删除待审核记录。
      401:
        description: 未授权，需要有效的JWT token。
        schema:
          type: object
          properties:
            msg:
              type: string
              description: 认证错误信息
          required:
            - msg
      403:
        description: 权限不足，需要管理员权限。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      404:
        description: 用户不在待审核列表中。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    consumes:
      - application/json
    produces:
      - application/json
    security:
      - Bearer: []
    """
    current_user = get_jwt_identity()
    user = User.query.filter_by(username=current_user).first()

    if user and user.is_admin:
        data = request.get_json()
        username = data["username"]

        # 检查用户是否在待审核列表中
        user_to_reject = UserToBePassed.query.filter_by(username=username).first()
        if not user_to_reject:
            return jsonify({"error": "用户不在待审核列表中"}), 404

        # 删除待审核用户
        db.session.delete(user_to_reject)
        db.session.commit()
        return "", 204
    else:
        return jsonify({"error": "权限不足"}), 403


@users_blueprint.route("/api/delete", methods=["POST"])
@jwt_required()
def delete_user():
    """
    删除用户。

    仅管理员可操作，永久删除指定用户名的正式用户账户。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: body
        in: body
        schema:
          type: object
          properties:
            username:
              type: string
              description: 要删除的用户名
          required:
            - username
    responses:
      204:
        description: 用户删除成功。
      401:
        description: 未授权，需要有效的JWT token。
        schema:
          type: object
          properties:
            msg:
              type: string
              description: 认证错误信息
          required:
            - msg
      403:
        description: 权限不足，需要管理员权限。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      404:
        description: 用户不存在。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    consumes:
      - application/json
    produces:
      - application/json
    security:
      - Bearer: []
    """
    current_user = get_jwt_identity()
    user = User.query.filter_by(username=current_user).first()

    if user and user.is_admin:
        data = request.get_json()
        username = data["username"]

        # 检查用户是否存在
        user_to_delete = User.query.filter_by(username=username).first()
        if not user_to_delete:
            return jsonify({"error": "用户不存在"}), 404

        # 删除用户
        db.session.delete(user_to_delete)
        db.session.commit()
        return "", 204
    else:
        return jsonify({"error": "权限不足"}), 403
