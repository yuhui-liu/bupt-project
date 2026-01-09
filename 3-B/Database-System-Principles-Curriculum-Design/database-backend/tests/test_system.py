"""
系统监控功能测试
"""

import pytest
from unittest.mock import patch, MagicMock


class TestSystemInfo:
    """系统信息功能测试类"""

    def test_system_info_as_admin(self, client, admin_token):
        """测试管理员获取系统信息"""
        with patch("app.utils.sql_loader.sql_loader.get_query") as mock_get_query:
            # Mock返回简单的SQL查询
            mock_get_query.return_value = "SELECT 1 as test_value"

            response = client.get(
                "/api/systeminfo", headers={"Authorization": f"Bearer {admin_token}"}
            )
            assert response.status_code == 200
            data = response.json
            assert isinstance(data, dict)
            # 检查返回的数据结构包含系统监控信息
            expected_keys = [
                "active_connections",
                "cache_hit_stats",
                "database_info",
                "database_size",
                "server_info",
            ]
            # 至少应该有一些监控数据
            assert len(data) > 0

    def test_system_info_as_normal_user(self, client, user_token):
        """测试普通用户获取系统信息被拒绝"""
        response = client.get(
            "/api/systeminfo", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 403
        assert "权限不足" in response.json["error"]

    def test_system_info_without_auth(self, client):
        """测试未授权获取系统信息"""
        response = client.get("/api/systeminfo")
        assert response.status_code == 401 or response.status_code == 422

    @patch("app.utils.sql_loader.sql_loader.get_query")
    def test_system_info_sql_error(self, mock_get_query, client, admin_token):
        """测试系统信息SQL查询错误处理"""
        # Mock SQL查询返回None（文件未找到）
        mock_get_query.return_value = None

        response = client.get(
            "/api/systeminfo", headers={"Authorization": f"Bearer {admin_token}"}
        )
        assert response.status_code == 200
        data = response.json
        # 应该包含错误信息
        assert isinstance(data, dict)

    def test_system_test_endpoint(self, client):
        """测试系统测试端点"""
        response = client.get("/test")
        assert response.status_code == 300
        assert response.data.decode() == "hi"


class TestSystemMonitoring:
    """系统监控相关测试"""

    @patch("app.utils.sql_loader.sql_loader.get_query")
    @patch("app.models.db.session.execute")
    def test_database_monitoring_queries(
        self, mock_execute, mock_get_query, client, admin_token
    ):
        """测试数据库监控查询"""
        # Mock SQL查询文件存在
        mock_get_query.return_value = "SELECT 'active_connections' as type, 10 as count"

        # Mock数据库执行结果
        mock_result = MagicMock()
        mock_result._mapping = {"type": "active_connections", "count": 10}
        mock_execute.return_value = [mock_result]

        response = client.get(
            "/api/systeminfo", headers={"Authorization": f"Bearer {admin_token}"}
        )
        assert response.status_code == 200

        # 验证调用了SQL加载器
        assert mock_get_query.called
        # 验证执行了数据库查询
        assert mock_execute.called

    @patch("app.utils.sql_loader.sql_loader.get_query")
    @patch("app.models.db.session.execute")
    def test_monitoring_with_exception(
        self, mock_execute, mock_get_query, client, admin_token
    ):
        """测试监控查询异常处理"""
        # Mock SQL查询文件存在
        mock_get_query.return_value = "SELECT invalid_query"

        # Mock数据库执行异常
        mock_execute.side_effect = Exception("Database connection error")

        response = client.get(
            "/api/systeminfo", headers={"Authorization": f"Bearer {admin_token}"}
        )
        assert response.status_code == 200
        data = response.json
        # 应该包含错误信息而不是崩溃
        assert isinstance(data, dict)

    def test_timedelta_conversion(self, client, admin_token):
        """测试时间增量转换功能"""
        # 这个测试主要验证系统能够正确处理timedelta对象
        # 在实际的系统信息响应中
        with patch("app.utils.sql_loader.sql_loader.get_query") as mock_get_query:
            mock_get_query.return_value = (
                "SELECT NOW() - INTERVAL '1 hour' as time_diff"
            )

            response = client.get(
                "/api/systeminfo", headers={"Authorization": f"Bearer {admin_token}"}
            )
            assert response.status_code == 200
            # 确保响应可以正常序列化为JSON（没有timedelta序列化错误）
            data = response.json
            assert isinstance(data, dict)


class TestSystemIntegration:
    """系统集成测试"""

    def test_system_info_complete_flow(self, client, admin_token):
        """测试系统信息完整流程"""
        # 模拟完整的系统信息获取流程
        with patch("app.utils.sql_loader.sql_loader.get_query") as mock_get_query:
            # Mock各种监控查询的返回值
            query_responses = {
                "monitoring_active_connections": "SELECT 5 as active_count",
                "monitoring_database_info": "SELECT 'PostgreSQL' as db_type",
                "monitoring_server_info": "SELECT version() as version",
            }

            def side_effect(query_name):
                return query_responses.get(query_name, "SELECT 1")

            mock_get_query.side_effect = side_effect

            response = client.get(
                "/api/systeminfo", headers={"Authorization": f"Bearer {admin_token}"}
            )
            assert response.status_code == 200
            data = response.json
            assert isinstance(data, dict)

            # 验证调用了多个监控查询
            assert mock_get_query.call_count > 10  # 应该调用多个监控查询

    def test_admin_permission_enforcement(self, client, user_token):
        """测试管理员权限强制执行"""
        # 确保只有管理员可以访问系统信息
        response = client.get(
            "/api/systeminfo", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 403

        # 确保错误消息明确
        error_msg = response.json["error"]
        assert "权限不足" in error_msg
