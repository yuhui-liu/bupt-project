from datetime import datetime
from flask import Blueprint, request, jsonify
from flask_jwt_extended import create_access_token, jwt_required
import random
from flask_mail import Message
from app.models import User, db, UserToBePassed
from app import mail
from flask_jwt_extended import get_jwt_identity

auth_blueprint = Blueprint("auth", __name__)

# 用于存储验证码，实际应用中建议使用Redis或数据库
captcha_storage = {}


@auth_blueprint.route("/api/register", methods=["POST"])
def register():
    """
    注册用户。

    新用户的信息将被写入UserToBePassed表中。
    ---
    parameters:
      - name: body
        in: body
        schema:
          type: object
          properties:
            email:
              type: string
              descpiption: 邮箱
            username:
              type: string
              description: 用户名
            password:
              type: string
              description: 密码
          required:
            - username
            - password
    responses:
      201:
        description: 成功写入UserToBePassed表。
      400:
        description: 用户已注册或已存在于待通过列表。
        headers: {}
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      500:
        description: 服务器内部故障
        headers: {}
        schema:
          type: object
          properties:
            error:
              type: string
              description: 故障的具体信息
          required:
            - error
    consumes:
      - application/json
    produces:
      - application/json
    """
    data = request.get_json()

    email = data["email"]
    username = data["username"]
    password = data["password"]
    captcha = data["captcha"]  # 新增：获取验证码

    # 检查邮箱是否为空
    if not email:
        return jsonify({"error": "邮箱不能为空"}), 400

    # 检查用户名和密码是否为空
    if not username or not password:
        return jsonify({"error": "用户名和密码不能为空"}), 400

    # 检查验证码
    if not captcha:
        return jsonify({"error": "验证码不能为空"}), 400
    if email not in captcha_storage or captcha_storage[email] != captcha:
        return jsonify({"error": "验证码错误"}), 400

    # 检查邮箱是否已存在
    if User.query.filter_by(email=email).first():
        return jsonify({"error": "邮箱已被注册"}), 400
    if UserToBePassed.query.filter_by(email=email).first():
        return jsonify({"error": "邮箱已被注册，等待审核"}), 400

    # 检查用户名是否已存在
    if User.query.filter_by(username=username).first():
        return jsonify({"error": "用户名与已注册用户重复"}), 400
    if UserToBePassed.query.filter_by(username=username).first():
        return jsonify({"error": "用户名与审核中用户重复"}), 400

    # 创建新用户
    try:
        new_user = UserToBePassed(
            email=email, username=username, apply_date=datetime.utcnow()
        )
        new_user.set_password(password)

        db.session.add(new_user)
        db.session.commit()

        # 注册成功后删除验证码
        captcha_storage.pop(email, None)

        return jsonify(), 201

    except Exception as e:
        print("error:", e)
        db.session.rollback()
        return jsonify({"error": "服务器故障，请稍后重试"}), 500


@auth_blueprint.route("/api/login", methods=["POST"])
def login():
    """
    用户登录。

    验证用户名和密码，登录成功后返回JWT token。
    ---
    parameters:
      - name: body
        in: body
        schema:
          type: object
          properties:
            username:
              type: string
              description: 用户名
            password:
              type: string
              description: 密码
          required:
            - username
            - password
    responses:
      200:
        description: 登录成功，返回JWT token。
        schema:
          type: object
          properties:
            token:
              type: string
              description: JWT访问令牌
          required:
            - token
      400:
        description: 参数错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      401:
        description: 密码错误。
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
    """
    data = request.get_json()

    username = data["username"]
    password = data["password"]

    # 检查用户名和密码是否为空
    if not username or not password:
        return jsonify({"error": "用户名和密码不能为空"}), 400

    if User.query.filter_by(username=username).first():
        user = User.query.filter_by(username=username).first()
        if user.check_password(password):
            token = create_access_token(identity=username)
            return jsonify(token=token), 200
        else:
            return jsonify({"error": "密码错误"}), 401
    else:
        return jsonify({"error": "用户不存在"}), 404


@auth_blueprint.route("/api/getCaptcha", methods=["POST"])
def get_captcha():
    """
    获取邮箱验证码。

    向指定邮箱发送6位数字验证码，用于用户注册时验证邮箱有效性。
    ---
    parameters:
      - name: body
        in: body
        schema:
          type: object
          properties:
            email:
              type: string
              description: 邮箱地址
          required:
            - email
    responses:
      200:
        description: 验证码发送成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 成功信息
          required:
            - message
      400:
        description: 参数错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      500:
        description: 邮件发送失败。
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
    """
    data = request.get_json()
    email = data.get("email")
    if not email:
        return jsonify({"error": "邮箱不能为空"}), 400
    code = str(random.randint(100000, 999999))
    # 发送邮件
    msg = Message(
        subject="注册验证码",
        recipients=[email],
        body=f"您好，\n\n您的注册验证码是：{code}\n\n验证码有效期为10分钟，请及时使用。如非本人操作，请忽略此邮件。",
    )
    try:
        mail.send(msg)
        captcha_storage[email] = code  # 存储验证码
        return jsonify({"message": "验证码已发送"}), 200
    except Exception as e:
        print(e)
        return jsonify({"error": "邮件发送失败"}), 500


@auth_blueprint.route("/api/send_reset_code", methods=["POST"])
def send_reset_code():
    """
    发送密码重置验证码。

    向已注册用户的邮箱发送6位数字验证码，用于密码重置验证。
    ---
    parameters:
      - name: body
        in: body
        schema:
          type: object
          properties:
            email:
              type: string
              description: 注册用户的邮箱地址
          required:
            - email
    responses:
      200:
        description: 重置验证码发送成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 成功信息
          required:
            - message
      400:
        description: 参数错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      404:
        description: 邮箱未注册。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      500:
        description: 邮件发送失败。
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
    """
    data = request.get_json()
    email = data.get("email")

    if not email:
        return jsonify({"error": "邮箱不能为空"}), 400

    # 检查邮箱是否已注册
    user = User.query.filter_by(email=email).first()
    if not user:
        return jsonify({"error": "该邮箱未注册"}), 404

    # 生成6位数字验证码
    reset_code = str(random.randint(100000, 999999))

    # 发送邮件
    msg = Message(
        "密码重置验证码",
        recipients=[email],
        body=f"您好，{user.username}！\n\n您的密码重置验证码是：{reset_code}\n\n验证码有效期为10分钟，请及时使用。\n\n如果这不是您的操作，请忽略此邮件。",
    )

    try:
        mail.send(msg)
        # 存储重置验证码，使用特殊前缀区分重置验证码和注册验证码
        captcha_storage[f"reset_{email}"] = reset_code
        return jsonify({"message": "重置验证码已发送至您的邮箱"}), 200
    except Exception as e:
        print(f"密码重置邮件发送失败: {e}")
        return jsonify({"error": "邮件发送失败，请稍后重试"}), 500


@auth_blueprint.route("/api/reset_password", methods=["POST"])
def reset_password():
    """
    重置用户密码。

    使用邮箱验证码验证身份后，重置用户密码为新密码。
    ---
    parameters:
      - name: body
        in: body
        schema:
          type: object
          properties:
            email:
              type: string
              description: 用户邮箱地址
            verification_code:
              type: string
              description: 邮箱验证码
            new_password:
              type: string
              description: 新密码（至少6位）
          required:
            - email
            - verification_code
            - new_password
    responses:
      200:
        description: 密码重置成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 成功信息
          required:
            - message
      400:
        description: 参数错误或验证码错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      404:
        description: 邮箱未注册。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      500:
        description: 密码重置失败。
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
    """
    data = request.get_json()
    email = data.get("email")
    verification_code = data.get("verification_code")
    new_password = data.get("new_password")

    # 参数验证
    if not email:
        return jsonify({"error": "邮箱不能为空"}), 400
    if not verification_code:
        return jsonify({"error": "验证码不能为空"}), 400
    if not new_password:
        return jsonify({"error": "新密码不能为空"}), 400

    # 验证新密码长度
    if len(new_password) < 6:
        return jsonify({"error": "密码长度不能少于6位"}), 400

    # 检查用户是否存在
    user = User.query.filter_by(email=email).first()
    if not user:
        return jsonify({"error": "该邮箱未注册"}), 404

    # 验证重置验证码
    reset_key = f"reset_{email}"
    if (
        reset_key not in captcha_storage
        or captcha_storage[reset_key] != verification_code
    ):
        return jsonify({"error": "验证码错误或已过期"}), 400

    try:
        # 重置密码
        user.set_password(new_password)
        db.session.commit()

        # 删除使用过的验证码
        captcha_storage.pop(reset_key, None)

        return jsonify({"message": "密码重置成功，请使用新密码登录"}), 200

    except Exception as e:
        db.session.rollback()
        print(f"密码重置失败: {e}")
        return jsonify({"error": "密码重置失败，请稍后重试"}), 500


@auth_blueprint.route("/api/reset_password_with_old_password", methods=["POST"])
def reset_password_with_old_password():
    """
    用户通过旧密码重置新密码。

    ---
    parameters:
      - name: body
        in: body
        schema:
          type: object
          properties:
            username:
              type: string
              description: 用户名
            old_password:
              type: string
              description: 旧密码
            new_password:
              type: string
              description: 新密码（至少6位）
          required:
            - username
            - old_password
            - new_password
    responses:
      200:
        description: 密码重置成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 成功信息
          required:
            - message
      400:
        description: 参数错误或密码错误。
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
      500:
        description: 密码重置失败。
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
    """
    data = request.get_json()
    username = data.get("username")
    old_password = data.get("old_password")
    new_password = data.get("new_password")

    # 参数校验
    if not username:
        return jsonify({"error": "用户名不能为空"}), 400
    if not old_password:
        return jsonify({"error": "旧密码不能为空"}), 400
    if not new_password:
        return jsonify({"error": "新密码不能为空"}), 400
    if len(new_password) < 6:
        return jsonify({"error": "新密码长度不能少于6位"}), 400

    user = User.query.filter_by(username=username).first()
    if not user:
        return jsonify({"error": "用户不存在"}), 404

    if not user.check_password(old_password):
        return jsonify({"error": "旧密码错误"}), 400

    try:
        user.set_password(new_password)
        db.session.commit()
        return jsonify({"message": "密码重置成功，请使用新密码登录"}), 200
    except Exception as e:
        db.session.rollback()
        print(f"通过旧密码重置失败: {e}")
        return jsonify({"error": "密码重置失败，请稍后重试"}), 500


@auth_blueprint.route("/api/delete_account", methods=["POST"])
@jwt_required()
def delete_account():
    """
    注销账户。

    需要提供当前登录token和密码，验证通过后删除用户账户。
    ---
    parameters:
      - name: body
        in: body
        schema:
          type: object
          properties:
            password:
              type: string
              description: 用户密码
          required:
            - password
    responses:
      200:
        description: 注销成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 注销成功信息
          required:
            - message
      400:
        description: 参数错误或密码错误。
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
      500:
        description: 注销失败。
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
    """
    data = request.get_json()
    password = data.get("password")
    if not password:
        return jsonify({"error": "密码不能为空"}), 400

    username = get_jwt_identity()
    user = User.query.filter_by(username=username).first()
    if not user:
        return jsonify({"error": "用户不存在"}), 404

    if not user.check_password(password):
        return jsonify({"error": "密码错误"}), 400

    try:
        db.session.delete(user)
        db.session.commit()
        return jsonify({"message": "账户已注销"}), 200
    except Exception as e:
        db.session.rollback()
        print(f"注销失败: {e}")
        return jsonify({"error": "注销失败，请稍后重试"}), 500
