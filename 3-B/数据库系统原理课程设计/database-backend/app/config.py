import os


class Config:
    SECRET_KEY = os.environ.get("SECRET_KEY") or "you-will-never-guess"
    SQLALCHEMY_DATABASE_URI = (
        os.environ.get("DATABASE_URL")
        or "postgresql://username:password@1.1.1.1:1111/your_db"
    )
    SQLALCHEMY_TRACK_MODIFICATIONS = False

    # JWT配置
    JWT_SECRET_KEY = os.environ.get("JWT_SECRET_KEY") or "jwt-secret-string"
    JWT_ACCESS_TOKEN_EXPIRES = 3600  # token过期时间，单位为秒

    # 文件上传配置
    UPLOAD_FOLDER = os.environ.get("UPLOAD_FOLDER") or "uploads"
    CHUNK_FOLDER = os.environ.get("CHUNK_FOLDER") or "uploads/chunks"
    MAX_CONTENT_LENGTH = 1024 * 1024 * 1024  # 1GB
    ALLOWED_EXTENSIONS = {"csv", "txt"}

    # 批量导入配置
    BATCH_SIZE = 1000  # 每批次处理的记录数
