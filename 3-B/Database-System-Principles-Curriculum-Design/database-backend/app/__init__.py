import os

from flask import Flask
from flask_mail import Mail
from flask_migrate import Migrate
from flask_jwt_extended import JWTManager
from flask_cors import CORS
from flasgger import Swagger
from app.config import Config
from app.models import db


migrate = Migrate()
jwt = JWTManager()
mail = Mail()


def create_app(config_class=None):
    app = Flask(__name__)
    if config_class:
        app.config.from_object(config_class)
    else:
        app.config.from_object(Config)
    app.config["MAIL_SERVER"] = "smtp.163.com"
    app.config["MAIL_PORT"] = 587
    app.config["MAIL_USE_SSL"] = True
    app.config["MAIL_USERNAME"] = os.environ.get("MAIL_USERNAME")
    app.config["MAIL_PASSWORD"] = os.environ.get("MAIL_PASSWORD")
    app.config["MAIL_DEFAULT_SENDER"] = os.environ.get("MAIL_USERNAME")

    # 创建必要的目录
    os.makedirs(app.config["UPLOAD_FOLDER"], exist_ok=True)
    os.makedirs(app.config["CHUNK_FOLDER"], exist_ok=True)

    # 初始化所有扩展
    mail.init_app(app)
    db.init_app(app)
    migrate.init_app(app, db)
    jwt.init_app(app)
    CORS(app)
    Swagger(app)

    # 注册蓝图
    from app.routes import register_blueprints

    register_blueprints(app)
    return app
