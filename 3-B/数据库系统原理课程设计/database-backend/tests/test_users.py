"""
用户管理功能测试
"""

import pytest
from app.models import User, UserToBePassed, db


class TestUsers:
    """用户管理功能测试类"""

    def test_get_my_info(self, client, user_token):
        """测试获取当前用户信息"""
        response = client.get(
            "/api/myinfo", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 200
        data = response.json
        assert data["username"] == "user_test"
        assert data["email"] == "user@test.com"
        assert data["is_admin"] is False

    def test_get_user_info_as_admin(self, client, admin_token):
        """测试管理员获取所有用户信息"""
        response = client.get(
            "/api/userinfo", headers={"Authorization": f"Bearer {admin_token}"}
        )
        assert response.status_code == 200
        data = response.json
        assert isinstance(data, list)
        assert len(data) >= 2  # 至少有admin和user

    def test_get_user_info_as_normal_user(self, client, user_token):
        """测试普通用户获取用户信息被拒绝"""
        response = client.get(
            "/api/userinfo", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 403
        assert "权限不足" in response.json["error"]

    def test_get_waitlist_as_admin(self, client, admin_token):
        """测试管理员获取待审核用户列表"""
        response = client.get(
            "/api/waitlist", headers={"Authorization": f"Bearer {admin_token}"}
        )
        assert response.status_code == 200
        data = response.json
        assert isinstance(data, list)
        # 应该包含在conftest.py中创建的pending_test用户
        usernames = [user["username"] for user in data]
        assert "pending_test" in usernames

    def test_get_waitlist_as_normal_user(self, client, user_token):
        """测试普通用户获取待审核列表被拒绝"""
        response = client.get(
            "/api/waitlist", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 403
        assert "权限不足" in response.json["error"]

    def test_pass_user_success(self, client, admin_token):
        """测试管理员通过用户审核"""
        response = client.post(
            "/api/pass",
            headers={"Authorization": f"Bearer {admin_token}"},
            json={"username": "pending_test"},
        )
        assert response.status_code == 201

        # 验证用户已从待审核列表转移到正式用户列表
        with client.application.app_context():
            user = User.query.filter_by(username="pending_test").first()
            assert user is not None

            pending_user = UserToBePassed.query.filter_by(
                username="pending_test"
            ).first()
            assert pending_user is None

    def test_pass_nonexistent_user(self, client, admin_token):
        """测试通过不存在的待审核用户"""
        response = client.post(
            "/api/pass",
            headers={"Authorization": f"Bearer {admin_token}"},
            json={"username": "nonexistent_user"},
        )
        assert response.status_code == 404
        assert "用户不在待审核列表中" in response.json["error"]

    def test_pass_user_as_normal_user(self, client, user_token):
        """测试普通用户尝试通过审核被拒绝"""
        response = client.post(
            "/api/pass",
            headers={"Authorization": f"Bearer {user_token}"},
            json={"username": "pending_test"},
        )
        assert response.status_code == 403
        assert "权限不足" in response.json["error"]

    def test_reject_user_success(self, client, admin_token, app):
        """测试管理员拒绝用户审核"""
        # 先创建一个新的待审核用户用于测试
        with app.app_context():
            reject_user = UserToBePassed(
                username="reject_test", email="reject@test.com"
            )
            reject_user.set_password("password123")
            db.session.add(reject_user)
            db.session.commit()

        response = client.post(
            "/api/reject",
            headers={"Authorization": f"Bearer {admin_token}"},
            json={"username": "reject_test"},
        )
        assert response.status_code == 204

        # 验证用户已从待审核列表中删除
        with app.app_context():
            pending_user = UserToBePassed.query.filter_by(
                username="reject_test"
            ).first()
            assert pending_user is None

    def test_reject_nonexistent_user(self, client, admin_token):
        """测试拒绝不存在的待审核用户"""
        response = client.post(
            "/api/reject",
            headers={"Authorization": f"Bearer {admin_token}"},
            json={"username": "nonexistent_user"},
        )
        assert response.status_code == 404
        assert "用户不在待审核列表中" in response.json["error"]

    def test_delete_user_success(self, client, admin_token, app):
        """测试管理员删除用户"""
        # 先创建一个新用户用于删除测试
        with app.app_context():
            delete_user = User(username="delete_test", email="delete@test.com")
            delete_user.set_password("password123")
            db.session.add(delete_user)
            db.session.commit()

        response = client.post(
            "/api/delete",
            headers={"Authorization": f"Bearer {admin_token}"},
            json={"username": "delete_test"},
        )
        assert response.status_code == 204

        # 验证用户已被删除
        with app.app_context():
            user = User.query.filter_by(username="delete_test").first()
            assert user is None

    def test_delete_nonexistent_user(self, client, admin_token):
        """测试删除不存在的用户"""
        response = client.post(
            "/api/delete",
            headers={"Authorization": f"Bearer {admin_token}"},
            json={"username": "nonexistent_user"},
        )
        assert response.status_code == 404
        assert "用户不存在" in response.json["error"]

    def test_delete_user_as_normal_user(self, client, user_token):
        """测试普通用户尝试删除用户被拒绝"""
        response = client.post(
            "/api/delete",
            headers={"Authorization": f"Bearer {user_token}"},
            json={"username": "user_test"},
        )
        assert response.status_code == 403
        assert "权限不足" in response.json["error"]

    def test_unauthorized_access(self, client):
        """测试未授权访问"""
        endpoints = ["/api/myinfo", "/api/userinfo", "/api/waitlist"]

        for endpoint in endpoints:
            response = client.get(endpoint)
            assert (
                response.status_code == 401 or response.status_code == 422
            )  # JWT error

    def test_invalid_token(self, client):
        """测试无效token"""
        response = client.get(
            "/api/myinfo", headers={"Authorization": "Bearer invalid_token"}
        )
        assert response.status_code == 422  # JWT decode error
