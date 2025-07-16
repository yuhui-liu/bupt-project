import re
from flask import Blueprint, request, jsonify, send_file
from flask_jwt_extended import jwt_required
import pandas as pd
import io
from other_expriments.neword import neword
from other_expriments.payment import payment

import psycopg2
from app.models import (
    db,
    Nation,
    Region,
    Part,
    Supplier,
    Customer,
    LineItem,
    Orders,
    PartSupp,
)
from app.utils.sql_loader import sql_loader
from sqlalchemy import text
import time
import subprocess
import tempfile
import os
from pathlib import Path

data_blueprint = Blueprint("data", __name__)


@data_blueprint.route("/api/export", methods=["GET"])
@jwt_required()
def export():
    """
    导出指定表的数据为文件。

    支持导出数据库表数据为CSV、TXT或XLSX格式的文件，供用户下载。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: table
        in: query
        type: string
        required: true
        description: 要导出的表名
        enum: [nation, region, part, supplier, customer, lineitem, orders, partsupp]
      - name: format
        in: query
        type: string
        required: false
        default: csv
        description: 导出文件格式
        enum: [csv, txt, xlsx]
    responses:
      200:
        description: 成功导出文件。
        content:
          text/csv:
            description: CSV格式文件
          text/plain:
            description: TXT格式文件
          application/vnd.openxmlformats-officedocument.spreadsheetml.sheet:
            description: Excel格式文件
      400:
        description: 请求参数错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      401:
        description: 未授权，需要有效的JWT token。
      404:
        description: 表中无数据。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      500:
        description: 服务器内部错误或导出失败。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    produces:
      - text/csv
      - text/plain
      - application/vnd.openxmlformats-officedocument.spreadsheetml.sheet
    security:
      - Bearer: []
    """

    table = request.args.get("table")
    file_format = request.args.get("format", "csv").lower()

    # 支持的表和模型映射
    table_model_map = {
        "nation": Nation,
        "region": Region,
        "part": Part,
        "supplier": Supplier,
        "customer": Customer,
        "lineitem": LineItem,
        "orders": Orders,
        "partsupp": PartSupp,
    }

    if table not in table_model_map:
        return jsonify({"error": "不支持的表名"}), 400

    if file_format not in ("csv", "txt", "xlsx"):
        return jsonify({"error": "仅支持csv、txt或xlsx格式"}), 400

    try:
        # 查询所有数据
        model = table_model_map[table]
        query = model.query.all()
        if not query:
            return jsonify({"error": "表中无数据"}), 404

        # 转为DataFrame
        df = pd.DataFrame([row.__dict__ for row in query])
        # 去除SQLAlchemy的内部属性
        df = df.loc[:, ~df.columns.str.startswith("_")]

        filename = f"{table}.{file_format}"

        if file_format == "xlsx":
            # 导出为Excel格式
            buf = io.BytesIO()
            with pd.ExcelWriter(buf, engine="openpyxl") as writer:
                df.to_excel(writer, sheet_name=table, index=False)
            buf.seek(0)

            mimetype = (
                "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"
            )
            return send_file(
                buf,
                mimetype=mimetype,
                as_attachment=True,
                download_name=filename,
            )
        else:
            # 导出为CSV或TXT格式
            buf = io.StringIO()
            df.to_csv(buf, index=False, header=True if file_format == "csv" else False)
            buf.seek(0)

            mimetype = "text/csv" if file_format == "csv" else "text/plain"
            return send_file(
                io.BytesIO(buf.getvalue().encode("utf-8")),
                mimetype=mimetype,
                as_attachment=True,
                download_name=filename,
            )
    except Exception as e:
        from flask import current_app

        current_app.logger.error(f"导出失败: {str(e)}")
        return jsonify({"error": f"导出失败: {str(e)}"}), 500


@data_blueprint.route("/api/query", methods=["GET", "POST"])
@jwt_required()
def query():
    """
    执行数据库查询操作。

    支持执行预定义查询、客户信息查询等多种类型的数据库查询操作。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: body
        in: body
        schema:
          type: object
          properties:
            type:
              type: string
              description: 查询类型
              enum: [customer_info, predefined, custom]
            customerName:
              type: string
              description: 客户姓名（当type为customer_info时必需）
            query_name:
              type: string
              description: 预定义查询名称（当type为predefined时必需）
              enum: [order_priority, part_profit]
            additional:
              type: object
              description: 额外参数（根据查询类型而定）
              properties:
                start_date:
                  type: string
                  format: date
                  description: 开始日期（用于order_priority查询）
                end_date:
                  type: string
                  format: date
                  description: 结束日期（用于order_priority查询）
                p_name:
                  type: string
                  description: 零件名称（用于part_profit查询）
          required:
            - type
    responses:
      200:
        description: 查询执行成功。
        schema:
          oneOf:
            - type: object
              description: 客户信息查询结果
              properties:
                custkey:
                  type: integer
                  description: 客户键
                name:
                  type: string
                  description: 客户姓名
                address:
                  type: string
                  description: 客户地址
                nation:
                  type: string
                  description: 所属国家
                phone:
                  type: string
                  description: 电话号码
                acctbal:
                  type: string
                  description: 账户余额
                mktsegment:
                  type: string
                  description: 市场细分
                comment:
                  type: string
                  description: 备注信息
            - type: object
              description: 预定义查询结果
              properties:
                columns:
                  type: array
                  items:
                    type: string
                  description: 列名列表
                data:
                  type: array
                  items:
                    type: object
                  description: 查询结果数据
            - type: object
              description: 非查询SQL执行结果
              properties:
                success:
                  type: boolean
                  description: 执行是否成功
                message:
                  type: string
                  description: 执行消息
                affected_rows:
                  type: integer
                  description: 受影响的行数
      400:
        description: 请求参数错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      401:
        description: 未授权，需要有效的JWT token。
      404:
        description: 客户信息未找到或SQL查询文件未找到。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      500:
        description: 查询执行失败。
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
    security:
      - Bearer: []
    """
    if request.method == "GET":
        data = request.get_json()
        type_ = data["type"]
        if type_ == "predefined":
            pass
        elif type_ == "custom":
            pass
        return jsonify(), 200
    elif request.method == "POST":
        data = request.get_json()
        type_ = data["type"]
        if type_ == "customer_info":
            customer_name = data["customerName"]
            customer_info = (
                db.session.query(Customer).filter_by(c_name=customer_name).first()
            )
            if not customer_info:
                return jsonify({"error": "客户信息未找到"}), 404
            customer_nation = (
                db.session.query(Nation)
                .filter_by(n_nationkey=customer_info.c_nationkey)
                .first()
            )
            return (
                jsonify(
                    {
                        "custkey": customer_info.c_custkey,
                        "name": customer_info.c_name,
                        "address": customer_info.c_address,
                        "nation": customer_nation.n_name if customer_nation else "未知",
                        "phone": customer_info.c_phone,
                        "acctbal": str(customer_info.c_acctbal),
                        "mktsegment": customer_info.c_mktsegment,
                        "comment": customer_info.c_comment,
                    }
                ),
                200,
            )
        elif type_ == "predefined":
            query_name = data["query_name"]
            try:
                query = sql_loader.get_query("predefined_" + query_name)
                if not query:
                    return jsonify({"error": "SQL查询文件未找到"}), 404

                additional_data = data.get("additional", None)
                if query_name == "order_priority":
                    start_date = additional_data["start_date"]
                    end_date = additional_data["end_date"]
                    query = re.sub(
                        r"1970-01-01|2020-01-01",
                        lambda match: {
                            "1970-01-01": start_date,
                            "2020-01-01": end_date,
                        }[match.group(0)],
                        query,
                    )
                elif query_name == "part_profit":
                    p_name = additional_data["p_name"]
                    query = re.sub(r"green", p_name, query)
                result = db.session.execute(text(query))
                if result.returns_rows:
                    columns = list(result.keys())
                    rows = [dict(row._mapping) for row in result]
                    return (
                        jsonify(
                            {
                                "columns": columns,
                                "data": rows,
                            }
                        ),
                        200,
                    )
                else:
                    return (
                        jsonify(
                            {
                                "success": True,
                                "message": "查询执行成功",
                                "affected_rows": result.rowcount,
                            }
                        ),
                        200,
                    )
            except Exception as e:
                return jsonify({"error": f"查询执行失败: {str(e)}"}), 500


@data_blueprint.route("/api/customers", methods=["GET"])
@jwt_required()
def customers():
    """
    获取所有客户姓名列表。

    查询数据库中所有客户的姓名，返回客户姓名数组。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取客户姓名列表。
        schema:
          type: array
          items:
            type: string
            description: 客户姓名
      401:
        description: 未授权，需要有效的JWT token。
      500:
        description: 查询客户数据失败。
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
    try:
        customer_names = db.session.query(Customer.c_name).all()
        names_list = [name[0] for name in customer_names]
    except Exception as e:
        return jsonify({"error": f"查询客户数据失败: {str(e)}"}), 500
    return jsonify(names_list), 200


@data_blueprint.route("/api/nations", methods=["GET"])
@jwt_required()
def nations():
    """
    获取所有国家名称列表。

    查询数据库中所有国家的名称，返回国家名称数组。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取国家名称列表。
        schema:
          type: array
          items:
            type: string
            description: 国家名称
      401:
        description: 未授权，需要有效的JWT token。
      500:
        description: 查询国家数据失败。
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
    try:
        nation_names = db.session.query(Nation.n_name).all()
        names_list = [name[0] for name in nation_names]
    except Exception as e:
        return jsonify({"error": f"查询客户数据失败: {str(e)}"}), 500
    return jsonify(names_list), 200


@data_blueprint.route("/api/regions", methods=["GET"])
@jwt_required()
def regions():
    """
    获取所有地区名称列表。

    查询数据库中所有地区的名称，返回地区名称数组。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取地区名称列表。
        schema:
          type: array
          items:
            type: string
            description: 地区名称
      401:
        description: 未授权，需要有效的JWT token。
      500:
        description: 查询地区数据失败。
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
    try:
        region_names = db.session.query(Region.r_name).all()
        names_list = [name[0] for name in region_names]
    except Exception as e:
        return jsonify({"error": f"查询地区数据失败: {str(e)}"}), 500
    return jsonify(names_list), 200


@data_blueprint.route("/api/p_types", methods=["GET"])
@jwt_required()
def p_types():
    """
    获取所有零件类型列表。

    查询数据库中所有零件的类型，返回零件类型数组。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取零件类型列表。
        schema:
          type: array
          items:
            type: string
            description: 零件类型
      401:
        description: 未授权，需要有效的JWT token。
      500:
        description: 查询零件类型数据失败。
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
    try:
        part_types = db.session.query(Part.p_type).distinct().all()
        types_list = [ptype[0] for ptype in part_types]
    except Exception as e:
        return jsonify({"error": f"查询零件类型数据失败: {str(e)}"}), 500
    return jsonify(types_list), 200


@data_blueprint.route("/api/tpc-h_analysis", methods=["POST"])
@jwt_required()
def tpc_h_analysis():
    """
    执行TPC-H性能分析查询。

    执行标准的TPC-H基准测试查询，支持批量发货分析等预定义分析任务。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: body
        in: body
        schema:
          type: object
          properties:
            predefined:
              type: string
              description: 预定义分析类型
              enum: [batch_shipment, part_profit]
            startDate:
              type: string
              format: date
              description: 开始日期（用于batch_shipment分析）
            endDate:
              type: string
              format: date
              description: 结束日期（用于batch_shipment分析）
            supplierNation:
              type: string
              description: 供应商国家（用于batch_shipment分析）
            customerNation:
              type: string
              description: 客户国家（用于batch_shipment分析）
          required:
            - predefined
    responses:
      200:
        description: TPC-H分析执行成功。
        schema:
          oneOf:
            - type: object
              description: 分析查询结果
              properties:
                columns:
                  type: array
                  items:
                    type: string
                  description: 列名列表
                data:
                  type: array
                  items:
                    type: object
                  description: 分析结果数据
            - type: object
              description: 功能未实现提示
              properties:
                message:
                  type: string
                  description: 提示消息
      400:
        description: 请求参数错误或不支持的分析类型。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      401:
        description: 未授权，需要有效的JWT token。
      404:
        description: SQL查询文件未找到。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      500:
        description: 查询执行失败。
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
    security:
      - Bearer: []
    """
    data = request.get_json()
    print(data)
    if data.get("predefined") == "batch_shipment":
        start_date = data.get("startDate")
        end_date = data.get("endDate")
        supplier_nation = data.get("supplierNation")
        customer_nation = data.get("customerNation")
        explain = data.get("explain", False)
        if not start_date or not end_date:
            return jsonify({"error": "缺少开始或结束日期"}), 400
        if not supplier_nation or not customer_nation:
            return jsonify({"error": "缺少供应商或客户国家"}), 400

        query = sql_loader.get_query("tpc-h-analysis_batch_shipment")
        if not query:
            return jsonify({"error": "SQL查询文件未找到"}), 404

        query = re.sub(
            r"1970-01-01|2020-01-01|COUNTRYA|COUNTRYB",
            lambda match: {
                "1970-01-01": start_date,
                "2020-01-01": end_date,
                "COUNTRYA": supplier_nation,
                "COUNTRYB": customer_nation,
            }[match.group(0)],
            query,
        )

        try:
            if explain:
                query = "EXPLAIN ANALYZE" + query
                result = db.session.execute(text(query))
                return_result = [row[0] for row in result]
                return_result = "\n".join(return_result)
                print(return_result)
                return jsonify({"data": return_result}), 200
            start_time = time.time()
            result = db.session.execute(text(query))
            execution_time = time.time() - start_time
            columns = list(result.keys())
            rows = [dict(row._mapping) for row in result]
            return (
                jsonify(
                    {"columns": columns, "data": rows, "exec_time": execution_time}
                ),
                200,
            )
        except Exception as e:
            return jsonify({"error": f"查询执行失败: {str(e)}"}), 500
    elif data.get("predefined") == "market_share":
        nation = data.get("nation")
        region = data.get("region")
        part_type = data.get("partType")
        start_date = data.get("startDate")
        end_date = data.get("endDate")
        explain = data.get("explain", False)
        if not nation or not region or not part_type:
            return jsonify({"error": "缺少国家、地区或零件类型"}), 400
        if not start_date or not end_date:
            return jsonify({"error": "缺少开始或结束日期"}), 400
        query = sql_loader.get_query("tpc-h-analysis_national_market_share")
        if not query:
            return jsonify({"error": "SQL查询文件未找到"}), 404
        query = re.sub(
            r"1970-01-01|2020-01-01|NATION|REGION|PART_TYPE",
            lambda match: {
                "1970-01-01": start_date,
                "2020-01-01": end_date,
                "BRAZIL": nation,
                "AMERICA": region,
                "PART_TYPE": part_type,
            }[match.group(0)],
            query,
        )
        try:
            if explain:
                query = "EXPLAIN ANALYZE" + query
                result = db.session.execute(text(query))
                return_result = [row[0] for row in result]
                return_result = "\n".join(return_result)
                print(return_result)
                return jsonify({"data": return_result}), 200
            start_time = time.time()
            result = db.session.execute(text(query))
            execution_time = time.time() - start_time
            columns = list(result.keys())
            rows = [dict(row._mapping) for row in result]
            return (
                jsonify(
                    {"columns": columns, "data": rows, "exec_time": execution_time}
                ),
                200,
            )
        except Exception as e:
            return jsonify({"error": f"查询执行失败: {str(e)}"}), 500
    else:
        return jsonify({"error": "不支持的TPC-H分析类型"}), 400


@data_blueprint.route("/api/tpc-c_concurrent", methods=["POST"])
@jwt_required()
def tpc_c_concurrent():
    """
    执行TPC-C并发基准测试。

    执行TPC-C基准测试的并发性能测试，支持自定义并发终端数、运行时间和事务限制。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: body
        in: body
        schema:
          type: object
          properties:
            terminal:
              type: integer
              description: 并发终端数量
              minimum: 1
            runTxnsPerTerminal:
              type: integer
              description: 每个终端运行的事务数量（与runMins二选一）
              minimum: 1
            runMins:
              type: integer
              description: 运行时间（分钟）（与runTxnsPerTerminal二选一）
              minimum: 1
            limitTxnsPerMin:
              type: integer
              description: 每分钟事务限制数量
              minimum: 1
          required:
            - terminal
            - limitTxnsPerMin
    responses:
      200:
        description: TPC-C基准测试执行成功。
        schema:
          type: object
          properties:
            success:
              type: boolean
              description: 执行是否成功
            message:
              type: string
              description: 执行消息
            stdout:
              type: string
              description: 标准输出内容
            stderr:
              type: string
              description: 标准错误输出内容（如果有）
      400:
        description: 请求参数错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      401:
        description: 未授权，需要有效的JWT token。
      500:
        description: 基准测试执行失败或配置文件处理错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
            returncode:
              type: integer
              description: 进程返回码（执行失败时）
            stdout:
              type: string
              description: 标准输出内容（执行失败时）
            stderr:
              type: string
              description: 标准错误输出内容（执行失败时）
          required:
            - error
    consumes:
      - application/json
    produces:
      - application/json
    security:
      - Bearer: []
    """
    data = request.get_json()
    print(data)
    terminal = data.get("terminal")
    runTxnsPerTerminal = data.get("runTxnsPerTerminal")
    runMins = data.get("runMins")
    limitTxnsPerMin = data.get("limitTxnsPerMin")

    if "terminal" not in data or "limitTxnsPerMin" not in data:
        return jsonify({"error": "缺少必要的参数"}), 400
    if "runTxnsPerTerminal" not in data and "runMins" not in data:
        return jsonify({"error": "必须提供runTxnsPerTerminal或runMins参数"}), 400

    # 使用绝对路径或相对于项目根目录的路径
    project_root = os.path.abspath(os.path.dirname(__file__) + "/../..")
    original_props_path = os.path.join(project_root, "benchmarksql-5.0/run/props.pg")
    benchmark_script_path = os.path.join(
        project_root, "benchmarksql-5.0/run/runBenchmark.sh"
    )
    benchmark_run_dir = os.path.join(project_root, "benchmarksql-5.0/run")

    # 检查文件是否存在
    if not os.path.exists(original_props_path):
        return jsonify({"error": f"原始配置文件不存在: {original_props_path}"}), 500
    if not os.path.exists(benchmark_script_path):
        return jsonify({"error": f"基准测试脚本不存在: {benchmark_script_path}"}), 500

    # 检查脚本是否可执行
    if not os.access(benchmark_script_path, os.X_OK):
        try:
            # 尝试添加执行权限
            os.chmod(benchmark_script_path, 0o755)
        except Exception as e:
            return jsonify({"error": f"脚本不可执行且无法设置权限: {str(e)}"}), 500

    temp_file_path = None
    try:
        # 读取原始配置文件
        with open(original_props_path, "r", encoding="utf-8") as f:
            original_content = f.read()

        # 准备要替换的参数
        replacements = {
            "terminals": str(terminal),
            "limitTxnsPerMin": str(limitTxnsPerMin),
        }

        # 根据提供的参数添加相应的替换规则
        if runTxnsPerTerminal:
            replacements["runTxnsPerTerminal"] = str(runTxnsPerTerminal)
        if runMins:
            replacements["runMins"] = str(runMins)

        # 修改配置内容
        modified_content = original_content
        for key, value in replacements.items():
            # 使用正则表达式替换配置项
            pattern = f"^{key}=.*$"
            replacement = f"{key}={value}"
            modified_content = re.sub(
                pattern, replacement, modified_content, flags=re.MULTILINE
            )
        print(modified_content)

        # 创建临时文件
        with tempfile.NamedTemporaryFile(
            mode="w",
            suffix=".temp",
            prefix="props.pg.",
            dir=benchmark_run_dir,
            delete=False,
            encoding="utf-8",
        ) as temp_file:
            temp_file.write(modified_content)
            temp_file_path = temp_file.name

        # 运行基准测试
        try:
            # 获取临时文件的basename用于脚本参数
            temp_filename = os.path.basename(temp_file_path)

            # 执行基准测试脚本 - 使用绝对路径
            result = subprocess.run(
                [benchmark_script_path, temp_filename],
                cwd=benchmark_run_dir,
                capture_output=True,
                text=True,
                timeout=3600,  # 设置1小时超时
                env=os.environ.copy(),  # 传递环境变量
            )
            result_split_by_newline = result.stdout.split("\n")
            print(result_split_by_newline[-6:])

            # 检查执行结果
            if result.returncode == 0:
                return (
                    jsonify(
                        {
                            "success": True,
                            "message": "TPC-C基准测试执行成功",
                            "stdout": result.stdout,
                            "stderr": result.stderr if result.stderr else None,
                        }
                    ),
                    200,
                )
            else:
                return (
                    jsonify(
                        {
                            "error": "基准测试执行失败",
                            "returncode": result.returncode,
                            "stdout": result.stdout,
                            "stderr": result.stderr,
                        }
                    ),
                    500,
                )

        except subprocess.TimeoutExpired:
            return jsonify({"error": "基准测试执行超时"}), 500
        except FileNotFoundError as e:
            return jsonify({"error": f"找不到执行文件: {str(e)}"}), 500
        except PermissionError as e:
            return jsonify({"error": f"权限错误: {str(e)}"}), 500
        except Exception as e:
            return jsonify({"error": f"执行基准测试时发生错误: {str(e)}"}), 500

    except Exception as e:
        return jsonify({"error": f"处理配置文件时发生错误: {str(e)}"}), 500
    finally:
        # 清理临时文件
        if temp_file_path and os.path.exists(temp_file_path):
            try:
                os.unlink(temp_file_path)
            except Exception as e:
                # 记录删除临时文件失败的日志，但不影响主要功能
                from flask import current_app

                current_app.logger.warning(
                    f"删除临时文件失败: {temp_file_path}, 错误: {str(e)}"
                )


@data_blueprint.route("/api/tpc-c_transaction", methods=["POST"])
@jwt_required()
def tpc_c_transcation():
    """
    执行TPC-C单个事务操作。

    执行TPC-C基准测试中的单个事务操作，支持新订单（neword）和支付（payment）两种事务类型。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: body
        in: body
        schema:
          type: object
          properties:
            name:
              type: string
              description: 事务类型
              enum: [neword, payment]
            w_id:
              type: integer
              description: 仓库ID
              default: 1
            d_id:
              type: integer
              description: 区域ID
              default: 1
            c_id:
              type: integer
              description: 客户ID
              default: 123
            o_ol_cnt:
              type: integer
              description: 订单行数（仅用于neword事务）
              default: 5
          required:
            - name
    responses:
      200:
        description: 事务执行成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 执行成功消息
            exec_time:
              type: number
              description: 执行时间（秒）
      400:
        description: 请求参数错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      401:
        description: 未授权，需要有效的JWT token。
      500:
        description: 事务执行失败。
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
    security:
      - Bearer: []
    """
    data = request.get_json()
    name = data.get("name")

    if not name:
        return jsonify({"error": "缺少必要的参数"}), 400

    if name not in ["neword", "payment"]:
        return jsonify({"error": "不支持的事务类型"}), 400

    db_params = {
        "dbname": "tpccdb",
        "user": "tpcc",
        "password": "tpcc_password",
        "host": "49.235.139.218",
        "port": 5432,
    }

    if name == "neword":
        try:
            connection = psycopg2.connect(**db_params)
            # 调用 neword() 函数
            w_id = data.get("w_id", 1)
            d_id = data.get("d_id", 1)
            c_id = data.get("c_id", 123)
            o_ol_cnt = data.get("o_ol_cnt", 5)  # 订单行数
            supware = [1, 2, 3, 4, 1]  # 仓库 ID 列表
            itemid = [101, 102, 103, 104, 105]  # 商品 ID 列表
            qty = [2, 3, 4, 1, 5]  # 购买数量列表
            start_time = time.time()
            result = neword(
                connection, w_id, d_id, c_id, o_ol_cnt, supware, itemid, qty
            )
            execution_time = time.time() - start_time
            if result == 0:
                return (
                    jsonify(
                        {"message": "新订单事务执行成功", "exec_time": execution_time}
                    ),
                    200,
                )
            else:
                return jsonify({"error": "新订单事务执行失败"}), 500
        except Exception as e:
            return jsonify({"error": f"新订单事务执行失败: {str(e)}"}), 500
        finally:
            # 关闭数据库连接
            if connection:
                connection.close()

    elif name == "payment":
        try:
            # 创建数据库连接
            connection = psycopg2.connect(**db_params)
            # 调用 payment 函数
            # 示例参数设置
            w_id = data.get("w_id", 1)  # 仓库 ID
            d_id = data.get("d_id", 1)  # 区域 ID
            c_id = data.get("c_id", 123)  # 客户 ID（仅在 `byname=False` 时使用）
            c_last = "Smith"  # 客户姓氏（仅在 `byname=True` 时使用）
            h_amount = 100.0  # 支付金额
            byname = False  # 是否通过客户姓氏查询（False 表示通过客户 ID 查询）
            start_time = time.time()
            result = payment(
                connection,
                w_id,
                d_id,
                c_id=c_id,
                c_last=c_last,
                h_amount=h_amount,
                byname=byname,
            )
            execution_time = time.time() - start_time
            if result == 0:
                return (
                    jsonify(
                        {"message": "支付事务执行成功", "exec_time": execution_time}
                    ),
                    200,
                )
            else:
                return jsonify({"error": "支付事务执行失败"}), 500
        except Exception as e:
            return jsonify({"error": f"支付事务执行失败: {str(e)}"}), 500
        finally:
            # 关闭数据库连接
            if connection:
                connection.close()
    return "ye", 200
