"""
测试配置文件
"""

import os
import tempfile
import pytest
from app import create_app, db
from app.models import User, UserToBePassed, Nation, Region, Part, Supplier, Customer


class TestConfig:
    """测试配置类"""

    SECRET_KEY = "test-secret-key"
    SQLALCHEMY_DATABASE_URI = "sqlite:///:memory:"
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    JWT_SECRET_KEY = "test-jwt-secret"
    JWT_ACCESS_TOKEN_EXPIRES = 3600
    TESTING = True
    WTF_CSRF_ENABLED = False

    # 邮件配置（测试用）
    MAIL_SERVER = "localhost"
    MAIL_PORT = 587
    MAIL_USE_SSL = False
    MAIL_USERNAME = "test@example.com"
    MAIL_PASSWORD = "test_password"
    MAIL_DEFAULT_SENDER = "test@example.com"

    # 文件上传测试配置
    UPLOAD_FOLDER = tempfile.mkdtemp()
    CHUNK_FOLDER = tempfile.mkdtemp()
    MAX_CONTENT_LENGTH = 1024 * 1024  # 1MB for testing
    ALLOWED_EXTENSIONS = {"csv", "txt"}
    BATCH_SIZE = 10


@pytest.fixture
def app():
    """创建测试应用"""
    app = create_app(TestConfig)

    with app.app_context():
        db.create_all()

        # 创建测试用户
        admin_user = User(username="admin_test", email="admin@test.com", is_admin=True)
        admin_user.set_password("password123")

        normal_user = User(username="user_test", email="user@test.com", is_admin=False)
        normal_user.set_password("password123")

        db.session.add(admin_user)
        db.session.add(normal_user)

        # 创建待审核用户
        pending_user = UserToBePassed(username="pending_test", email="pending@test.com")
        pending_user.set_password("password123")
        db.session.add(pending_user)

        # 创建测试数据
        nation = Nation(
            n_nationkey=1, n_name="CHINA", n_regionkey=1, n_comment="Test nation"
        )
        region = Region(r_regionkey=1, r_name="ASIA", r_comment="Test region")
        part = Part(
            p_partkey=1,
            p_name="Test Part",
            p_mfgr="Test Mfgr",
            p_brand="Test",
            p_type="Test Type",
            p_size=10,
            p_container="Box",
            p_retailprice=100.0,
            p_comment="Test comment",
        )
        supplier = Supplier(
            s_suppkey=1,
            s_name="Test Supplier",
            s_address="Test Address",
            s_nationkey=1,
            s_phone="123-456-7890",
            s_acctbal=1000.0,
            s_comment="Test supplier",
        )
        customer = Customer(
            c_custkey=1,
            c_name="Test Customer",
            c_address="Test Address",
            c_nationkey=1,
            c_phone="123-456-7890",
            c_acctbal=1000.0,
            c_mktsegment="BUILDING",
            c_comment="Test customer",
        )

        db.session.add_all([nation, region, part, supplier, customer])
        db.session.commit()

        yield app

        db.drop_all()


@pytest.fixture
def client(app):
    """创建测试客户端"""
    return app.test_client()


@pytest.fixture
def admin_token(client):
    """获取管理员JWT token"""
    response = client.post(
        "/api/login", json={"username": "admin_test", "password": "password123"}
    )
    return response.json["token"]


@pytest.fixture
def user_token(client):
    """获取普通用户JWT token"""
    response = client.post(
        "/api/login", json={"username": "user_test", "password": "password123"}
    )
    return response.json["token"]
