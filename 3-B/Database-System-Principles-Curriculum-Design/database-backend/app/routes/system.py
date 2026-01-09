from datetime import timedelta
from flask import Blueprint, jsonify
from flask_jwt_extended import jwt_required, get_jwt_identity
from sqlalchemy import text
from app.models import User, db
from app.utils.sql_loader import sql_loader

system_blueprint = Blueprint("system", __name__)


@system_blueprint.route("/test", methods=["GET"])
def test():
    """
    测试接口。

    用于系统测试和调试的简单接口，返回测试响应。
    ---
    responses:
      300:
        description: 测试响应。
        schema:
          type: string
          description: 测试消息
    produces:
      - text/plain
    """
    # 用来随时测试，你也可以写test_a,test_b,test_c等等
    return "hi", 300


@system_blueprint.route("/api/systeminfo", methods=["GET"])
@jwt_required()
def system_info():
    """
    获取PostgreSQL数据库完整统计信息。

    管理员专用接口，返回数据库的各项监控统计数据，包括连接状态、性能指标、表统计等信息。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取数据库统计信息。
        schema:
          type: object
          properties:
            active_connections:
              type: array
              description: 活跃连接信息
            cache_hit_stats:
              type: array
              description: 缓存命中率统计
            checkpoint_stats:
              type: array
              description: 检查点统计信息
            connection_stats:
              type: array
              description: 连接统计信息
            database_activity:
              type: array
              description: 数据库活动状态
            database_info:
              type: array
              description: 数据库基本信息
            database_size:
              type: array
              description: 数据库大小信息
            database_stats:
              type: array
              description: 数据库统计数据
            deadlock_stats:
              type: array
              description: 死锁统计信息
            important_settings:
              type: array
              description: 重要配置参数
            index_usage:
              type: array
              description: 索引使用情况
            locks_info:
              type: array
              description: 锁信息统计
            long_running_queries:
              type: array
              description: 长时间运行的查询
            most_accessed_tables:
              type: array
              description: 最常访问的表
            replication_status:
              type: array
              description: 复制状态信息
            server_info:
              type: array
              description: 服务器信息
            tablespace_usage:
              type: array
              description: 表空间使用情况
            table_stats:
              type: array
              description: 表统计信息
            vacuum_stats:
              type: array
              description: 清理统计信息
            wal_checkpoint_info:
              type: array
              description: WAL检查点信息
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
      500:
        description: 服务器内部错误。
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
    if not user or not user.is_admin:
        return jsonify({"error": "权限不足"}), 403
    try:
        # 定义需要执行的查询
        query_names = [
            "monitoring_active_connections",
            "monitoring_cache_hit_stats",
            "monitoring_checkpoint_stats",
            "monitoring_connection_stats",
            "monitoring_database_activity",
            "monitoring_database_info",
            "monitoring_database_size",
            "monitoring_database_stats",
            "monitoring_deadlock_stats",
            "monitoring_important_settings",
            "monitoring_index_usage",
            "monitoring_locks_info",
            "monitoring_long_running_queries",
            "monitoring_most_accessed_tables",
            "monitoring_replication_status",
            "monitoring_server_info",
            "monitoring_tablespace_usage",
            "monitoring_table_stats",
            "monitoring_vacuum_stats",
            "monitoring_wal_checkpoint_info",
        ]

        results = {}
        for query_name in query_names:
            try:
                query = sql_loader.get_query(query_name)
                if query:
                    result = db.session.execute(text(query))
                    # 使用更简洁的键名（去掉monitoring_前缀）
                    key = query_name.replace("monitoring_", "")
                    results[key] = [dict(row._mapping) for row in result]
                else:
                    results[query_name] = {"error": f"SQL查询文件未找到: {query_name}"}
            except Exception as query_error:
                key = query_name.replace("monitoring_", "")
                results[key] = {"error": str(query_error)}

        def timedelta_to_str(obj):
            if isinstance(obj, timedelta):
                return int(obj.total_seconds())
            elif isinstance(obj, list):
                return [timedelta_to_str(x) for x in obj]
            elif isinstance(obj, dict):
                return {k: timedelta_to_str(v) for k, v in obj.items()}
            else:
                return obj

        results = timedelta_to_str(results)
        return jsonify(results), 200

    except Exception as e:
        return jsonify({"error": f"获取数据库统计信息失败: {str(e)}"}), 500
