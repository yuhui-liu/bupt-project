from flask import Blueprint


def register_blueprints(app):
    """注册所有蓝图到应用"""
    try:
        from .auth import auth_blueprint

        app.register_blueprint(auth_blueprint)
        print("注册 auth_blueprint 成功")
    except Exception as e:
        print(f"注册 auth_blueprint 失败: {e}")

    try:
        from .users import users_blueprint

        app.register_blueprint(users_blueprint)
        print("注册 users_blueprint 成功")
    except Exception as e:
        print(f"注册 users_blueprint 失败: {e}")

    try:
        from .upload import upload_blueprint

        app.register_blueprint(upload_blueprint)
        print("注册 upload_blueprint 成功")
    except Exception as e:
        print(f"注册 upload_blueprint 失败: {e}")

    try:
        from .data import data_blueprint

        app.register_blueprint(data_blueprint)
        print("注册 data_blueprint 成功")
    except Exception as e:
        print(f"注册 data_blueprint 失败: {e}")

    try:
        from .system import system_blueprint

        app.register_blueprint(system_blueprint)
        print("注册 system_blueprint 成功")
    except Exception as e:
        print(f"注册 system_blueprint 失败: {e}")
