"""
数据查询和导出功能测试
"""

import pytest
import io
from unittest.mock import patch, MagicMock


class TestDataQueries:
    """数据查询功能测试类"""

    def test_get_customers(self, client, user_token):
        """测试获取客户列表"""
        response = client.get(
            "/api/customers", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 200
        data = response.json
        assert isinstance(data, list)
        assert "Test Customer" in data

    def test_get_nations(self, client, user_token):
        """测试获取国家列表"""
        response = client.get(
            "/api/nations", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 200
        data = response.json
        assert isinstance(data, list)
        assert "CHINA" in data

    def test_get_regions(self, client, user_token):
        """测试获取地区列表"""
        response = client.get(
            "/api/regions", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 200
        data = response.json
        assert isinstance(data, list)
        assert "ASIA" in data

    def test_get_p_types(self, client, user_token):
        """测试获取零件类型列表"""
        response = client.get(
            "/api/p_types", headers={"Authorization": f"Bearer {user_token}"}
        )
        assert response.status_code == 200
        data = response.json
        assert isinstance(data, list)
        assert "Test Type" in data

    def test_customer_info_query(self, client, user_token):
        """测试客户信息查询"""
        response = client.post(
            "/api/query",
            headers={"Authorization": f"Bearer {user_token}"},
            json={"type": "customer_info", "customerName": "Test Customer"},
        )
        assert response.status_code == 200
        data = response.json
        assert data["name"] == "Test Customer"
        assert data["custkey"] == 1
        assert data["nation"] == "CHINA"

    def test_customer_info_not_found(self, client, user_token):
        """测试查询不存在的客户信息"""
        response = client.post(
            "/api/query",
            headers={"Authorization": f"Bearer {user_token}"},
            json={"type": "customer_info", "customerName": "Nonexistent Customer"},
        )
        assert response.status_code == 404
        assert "客户信息未找到" in response.json["error"]

    @patch("app.utils.sql_loader.sql_loader.get_query")
    def test_predefined_query_success(self, mock_get_query, client, user_token):
        """测试预定义查询成功"""
        # Mock SQL查询
        mock_get_query.return_value = "SELECT 1 as test_col"

        response = client.post(
            "/api/query",
            headers={"Authorization": f"Bearer {user_token}"},
            json={"type": "predefined", "query_name": "test_query"},
        )
        assert response.status_code == 200
        data = response.json
        assert "columns" in data
        assert "data" in data

    @patch("app.utils.sql_loader.sql_loader.get_query")
    def test_predefined_query_not_found(self, mock_get_query, client, user_token):
        """测试预定义查询文件未找到"""
        mock_get_query.return_value = None

        response = client.post(
            "/api/query",
            headers={"Authorization": f"Bearer {user_token}"},
            json={"type": "predefined", "query_name": "nonexistent_query"},
        )
        assert response.status_code == 404
        assert "SQL查询文件未找到" in response.json["error"]

    def test_unauthorized_data_access(self, client):
        """测试未授权的数据访问"""
        endpoints = ["/api/customers", "/api/nations", "/api/regions", "/api/p_types"]

        for endpoint in endpoints:
            response = client.get(endpoint)
            assert response.status_code == 401 or response.status_code == 422


class TestDataExport:
    """数据导出功能测试类"""

    def test_export_csv_success(self, client, user_token):
        """测试CSV导出成功"""
        response = client.get(
            "/api/export?table=customer&format=csv",
            headers={"Authorization": f"Bearer {user_token}"},
        )
        assert response.status_code == 200
        assert response.content_type == "text/csv; charset=utf-8"

    def test_export_txt_success(self, client, user_token):
        """测试TXT导出成功"""
        response = client.get(
            "/api/export?table=nation&format=txt",
            headers={"Authorization": f"Bearer {user_token}"},
        )
        assert response.status_code == 200
        assert response.content_type == "text/plain; charset=utf-8"

    def test_export_xlsx_success(self, client, user_token):
        """测试Excel导出成功"""
        response = client.get(
            "/api/export?table=region&format=xlsx",
            headers={"Authorization": f"Bearer {user_token}"},
        )
        assert response.status_code == 200
        assert (
            "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"
            in response.content_type
        )

    def test_export_unsupported_table(self, client, user_token):
        """测试导出不支持的表"""
        response = client.get(
            "/api/export?table=invalid_table&format=csv",
            headers={"Authorization": f"Bearer {user_token}"},
        )
        assert response.status_code == 400
        assert "不支持的表名" in response.json["error"]

    def test_export_unsupported_format(self, client, user_token):
        """测试导出不支持的格式"""
        response = client.get(
            "/api/export?table=customer&format=pdf",
            headers={"Authorization": f"Bearer {user_token}"},
        )
        assert response.status_code == 400
        assert "仅支持csv、txt或xlsx格式" in response.json["error"]

    def test_export_without_auth(self, client):
        """测试未授权导出"""
        response = client.get("/api/export?table=customer&format=csv")
        assert response.status_code == 401 or response.status_code == 422


class TestTPCHAnalysis:
    """TPC-H分析功能测试类"""

    @patch("app.utils.sql_loader.sql_loader.get_query")
    def test_tpc_h_batch_shipment_analysis(self, mock_get_query, client, user_token):
        """测试TPC-H批量发货分析"""
        mock_get_query.return_value = "SELECT 'test' as result"

        response = client.post(
            "/api/tpc-h_analysis",
            headers={"Authorization": f"Bearer {user_token}"},
            json={
                "predefined": "batch_shipment",
                "startDate": "2020-01-01",
                "endDate": "2020-12-31",
                "supplierNation": "CHINA",
                "customerNation": "GERMANY",
            },
        )
        assert response.status_code == 200
        data = response.json
        assert "columns" in data
        assert "data" in data

    def test_tpc_h_analysis_missing_params(self, client, user_token):
        """测试TPC-H分析缺少参数"""
        response = client.post(
            "/api/tpc-h_analysis",
            headers={"Authorization": f"Bearer {user_token}"},
            json={
                "predefined": "batch_shipment",
                "startDate": "2020-01-01",
                # 缺少其他必要参数
            },
        )
        assert response.status_code == 400
        assert "缺少" in response.json["error"]

    @patch("app.utils.sql_loader.sql_loader.get_query")
    def test_tpc_h_market_share_analysis(self, mock_get_query, client, user_token):
        """测试TPC-H市场份额分析"""
        mock_get_query.return_value = "SELECT 'market_share' as result"

        response = client.post(
            "/api/tpc-h_analysis",
            headers={"Authorization": f"Bearer {user_token}"},
            json={
                "predefined": "market_share",
                "nation": "CHINA",
                "region": "ASIA",
                "partType": "STANDARD",
                "startDate": "2020-01-01",
                "endDate": "2020-12-31",
            },
        )
        assert response.status_code == 200

    def test_tpc_h_unsupported_analysis(self, client, user_token):
        """测试不支持的TPC-H分析类型"""
        response = client.post(
            "/api/tpc-h_analysis",
            headers={"Authorization": f"Bearer {user_token}"},
            json={"predefined": "unsupported_analysis"},
        )
        assert response.status_code == 400
        assert "不支持的TPC-H分析类型" in response.json["error"]
