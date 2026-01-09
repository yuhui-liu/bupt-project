"""
Flask应用初始化
"""
from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS
import os

# 初始化扩展
db = SQLAlchemy()

def create_app():
    """应用工厂函数"""
    app = Flask(__name__)

    # 加载配置
    from app.config import Config
    app.config.from_object(Config)

    # 确保上传目录存在
    os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)

    # 初始化扩展
    db.init_app(app)

    # 配置CORS
    cors_origins = os.getenv('CORS_ORIGINS', 'http://localhost:3000').split(',')
    CORS(app, resources={
        r"/api/*": {
            "origins": cors_origins,
            "methods": ["GET", "POST", "PUT", "DELETE", "OPTIONS"],
            "allow_headers": ["Content-Type", "Authorization"],
            "expose_headers": ["Content-Type", "Authorization"],
            "supports_credentials": True
        },
        r"/uploads/*": {
            "origins": cors_origins,
            "methods": ["GET"],
        }
    })

    # 静态文件服务（上传的文件）
    from flask import send_from_directory

    @app.route('/uploads/<path:filename>')
    def uploaded_file(filename):
        """提供上传文件的访问"""
        upload_folder = app.config['UPLOAD_FOLDER']
        return send_from_directory(upload_folder, filename)

    # 注册蓝图
    from app.auth.routes import auth_bp
    from app.api.user import user_bp
    from app.api.needs import needs_bp
    from app.api.services import services_bp
    from app.api.admin import admin_bp
    from app.api.common import common_bp

    app.register_blueprint(auth_bp, url_prefix='/api/auth')
    app.register_blueprint(user_bp, url_prefix='/api/user')
    app.register_blueprint(needs_bp, url_prefix='/api/needs')
    app.register_blueprint(services_bp, url_prefix='/api/services')
    app.register_blueprint(admin_bp, url_prefix='/api/admin')
    app.register_blueprint(common_bp, url_prefix='/api')

    # 错误处理
    from app.utils.responses import error_response

    @app.errorhandler(404)
    def not_found(error):
        return error_response('资源不存在', 'NOT_FOUND', 404)

    @app.errorhandler(500)
    def internal_error(error):
        db.session.rollback()
        app.logger.error(f'Server Error: {error}')
        return error_response('服务器内部错误', 'SERVER_ERROR', 500)

    @app.errorhandler(413)
    def request_entity_too_large(error):
        return error_response('文件大小超过限制', 'FILE_TOO_LARGE', 413)

    # 健康检查
    @app.route('/api/health')
    def health():
        return {'status': 'ok', 'message': '服务运行正常'}

    return app
