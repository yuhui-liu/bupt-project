from datetime import datetime
from flask import Blueprint, request, jsonify, current_app
from flask_jwt_extended import jwt_required, get_jwt_identity
import os
import pandas as pd
import shutil
import re
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
    FileUpload,
    FileUploadStatus,
)

upload_blueprint = Blueprint("upload", __name__)


@upload_blueprint.route("/api/upload", methods=["POST"])
@jwt_required()
def upload():
    """
    文件上传接口。

    接收前端上传的文件和表名，将文件数据直接导入到数据库指定表中。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: file
        in: formData
        type: file
        required: true
        description: 要上传的CSV或TXT文件
      - name: table
        in: formData
        type: string
        required: true
        description: 目标表名
        enum: [nation, region, part, supplier, customer, lineitem, orders, partsupp]
    responses:
      200:
        description: 文件上传和数据导入成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 成功消息
          required:
            - message
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
        description: 服务器内部错误或数据导入失败。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    consumes:
      - multipart/form-data
    produces:
      - application/json
    security:
      - Bearer: []
    """
    if "file" not in request.files or "table" not in request.form:
        return jsonify({"error": "缺少文件或表名"}), 400

    file_ = request.files["file"]
    table = request.form["table"]

    if file_.filename == "":
        return jsonify({"error": "未选择文件"}), 400

    tbl_names = [
        "nation",
        "region",
        "part",
        "supplier",
        "customer",
        "lineitem",
        "orders",
        "partsupp",
    ]
    if table not in tbl_names:
        return jsonify({"error": "不支持的表名"}), 400

    if not file_.filename.endswith((".csv", ".txt")):
        return jsonify({"error": "仅支持CSV或TXT文件"}), 400

    # 导入数据到指定表
    try:
        if table == "nation":
            df = pd.read_csv(file_, encoding="utf-8", header=None)
            nations = []
            for _, row in df.iterrows():
                nations.append(
                    {
                        "n_nationkey": row.iloc[0],
                        "n_name": row.iloc[1],
                        "n_regionkey": row.iloc[2],
                        "n_comment": row.iloc[3],
                    }
                )
            db.session.bulk_insert_mappings(Nation, nations)

        elif table == "region":
            df = pd.read_csv(file_, encoding="utf-8", header=None)
            regions = []
            for _, row in df.iterrows():
                regions.append(
                    {
                        "r_regionkey": row.iloc[0],
                        "r_name": row.iloc[1],
                        "r_comment": row.iloc[2],
                    }
                )
            db.session.bulk_insert_mappings(Region, regions)

        elif table == "part":
            df = pd.read_csv(file_, encoding="utf-8", header=None)
            parts = []
            for _, row in df.iterrows():
                parts.append(
                    {
                        "p_partkey": row.iloc[0],
                        "p_name": row.iloc[1],
                        "p_mfgr": row.iloc[2],
                        "p_brand": row.iloc[3],
                        "p_type": row.iloc[4],
                        "p_size": row.iloc[5],
                        "p_container": row.iloc[6],
                        "p_retailprice": row.iloc[7],
                        "p_comment": row.iloc[8],
                    }
                )
            db.session.bulk_insert_mappings(Part, parts)

        elif table == "supplier":
            df = pd.read_csv(file_, encoding="utf-8", header=None)
            suppliers = []
            for _, row in df.iterrows():
                suppliers.append(
                    {
                        "s_suppkey": row.iloc[0],
                        "s_name": row.iloc[1],
                        "s_address": row.iloc[2],
                        "s_nationkey": row.iloc[3],
                        "s_phone": row.iloc[4],
                        "s_acctbal": row.iloc[5],
                        "s_comment": row.iloc[6],
                    }
                )
            db.session.bulk_insert_mappings(Supplier, suppliers)

        elif table == "customer":
            df = pd.read_csv(file_, encoding="utf-8", header=None)
            customers = []
            for _, row in df.iterrows():
                customers.append(
                    {
                        "c_custkey": row.iloc[0],
                        "c_name": row.iloc[1],
                        "c_address": row.iloc[2],
                        "c_nationkey": row.iloc[3],
                        "c_phone": row.iloc[4],
                        "c_acctbal": row.iloc[5],
                        "c_mktsegment": row.iloc[6],
                        "c_comment": row.iloc[7],
                    }
                )
            db.session.bulk_insert_mappings(Customer, customers)

        elif table == "lineitem":
            df = pd.read_csv(file_, encoding="utf-8", header=None)
            lineitems = []
            for _, row in df.iterrows():
                lineitems.append(
                    {
                        "l_orderkey": row.iloc[0],
                        "l_partkey": row.iloc[1],
                        "l_suppkey": row.iloc[2],
                        "l_linenumber": row.iloc[3],
                        "l_quantity": row.iloc[4],
                        "l_extendedprice": row.iloc[5],
                        "l_discount": row.iloc[6],
                        "l_tax": row.iloc[7],
                        "l_returnflag": row.iloc[8],
                        "l_linestatus": row.iloc[9],
                        "l_shipdate": row.iloc[10],
                        "l_commitdate": row.iloc[11],
                        "l_receiptdate": row.iloc[12],
                        "l_shipinstruct": row.iloc[13],
                        "l_shipmode": row.iloc[14],
                        "l_comment": row.iloc[15],
                    }
                )
            db.session.bulk_insert_mappings(LineItem, lineitems)

        elif table == "orders":
            df = pd.read_csv(file_, encoding="utf-8", header=None)
            orders_list = []
            for _, row in df.iterrows():
                orders_list.append(
                    {
                        "o_orderkey": row.iloc[0],
                        "o_custkey": row.iloc[1],
                        "o_orderstatus": row.iloc[2],
                        "o_totalprice": row.iloc[3],
                        "o_orderdate": row.iloc[4],
                        "o_orderpriority": row.iloc[5],
                        "o_clerk": row.iloc[6],
                        "o_shippriority": row.iloc[7],
                        "o_comment": row.iloc[8],
                    }
                )
            db.session.bulk_insert_mappings(Orders, orders_list)

        elif table == "partsupp":
            df = pd.read_csv(file_, encoding="utf-8", header=None)
            partsupps = []
            for _, row in df.iterrows():
                partsupps.append(
                    {
                        "ps_partkey": row.iloc[0],
                        "ps_suppkey": row.iloc[1],
                        "ps_availqty": row.iloc[2],
                        "ps_supplycost": row.iloc[3],
                        "ps_comment": row.iloc[4],
                    }
                )
            db.session.bulk_insert_mappings(PartSupp, partsupps)

        db.session.commit()
    except Exception as e:
        print(e)
        detail_line = None
        for line in str(e).splitlines():
            if re.search(r"DETAIL", line):
                detail_line = line
                break
        if detail_line:
            print(detail_line)
        db.session.rollback()
        return jsonify({"error": f"数据导入失败。{detail_line}"}), 500
    return jsonify({"message": "文件上传成功"}), 200


@upload_blueprint.route("/api/upload-chunk", methods=["POST"])
@jwt_required()
def upload_chunk():
    """
    分块上传文件。

    接收文件的一个分块并保存到临时目录，支持大文件的分块上传。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: fileId
        in: formData
        type: string
        required: true
        description: 文件唯一标识符
      - name: chunkIndex
        in: formData
        type: integer
        required: true
        description: 当前分块的索引（从0开始）
      - name: totalChunks
        in: formData
        type: integer
        required: true
        description: 总分块数量
      - name: fileName
        in: formData
        type: string
        required: true
        description: 原始文件名
      - name: table
        in: formData
        type: string
        required: true
        description: 目标表名
        enum: [nation, region, part, supplier, customer, lineitem, orders, partsupp]
      - name: chunk
        in: formData
        type: file
        required: true
        description: 文件分块数据
    responses:
      200:
        description: 分块上传成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 成功消息
            uploaded_chunks:
              type: integer
              description: 已上传的分块数量
            total_chunks:
              type: integer
              description: 总分块数量
          required:
            - message
            - uploaded_chunks
            - total_chunks
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
        description: 服务器内部错误。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
    consumes:
      - multipart/form-data
    produces:
      - application/json
    security:
      - Bearer: []
    """
    try:
        # 获取表单数据
        file_id = request.form.get("fileId")
        chunk_index = int(request.form.get("chunkIndex", 0))
        total_chunks = int(request.form.get("totalChunks", 1))
        filename = request.form.get("fileName")
        table_name = request.form.get("table")

        # 获取文件分块
        if "chunk" not in request.files:
            return jsonify({"error": "没有找到文件分块"}), 400

        chunk_file = request.files["chunk"]
        if chunk_file.filename == "":
            return jsonify({"error": "文件分块为空"}), 400

        # 验证表名
        allowed_tables = [
            "nation",
            "region",
            "part",
            "supplier",
            "customer",
            "lineitem",
            "orders",
            "partsupp",
        ]
        if table_name not in allowed_tables:
            return jsonify({"error": f"不支持的表名: {table_name}"}), 400

        # 验证文件扩展名
        if not filename.lower().endswith((".csv", ".txt")):
            return jsonify({"error": "仅支持CSV和TXT文件"}), 400

        # 确保目录存在
        chunk_dir = os.path.join(current_app.config["CHUNK_FOLDER"], file_id)
        os.makedirs(chunk_dir, exist_ok=True)

        # 保存分块文件
        chunk_filename = f"chunk_{chunk_index}"
        chunk_path = os.path.join(chunk_dir, chunk_filename)
        chunk_file.save(chunk_path)

        # 获取当前用户
        current_user = get_jwt_identity()

        # 更新或创建文件上传记录
        file_upload = FileUpload.query.filter_by(id=file_id).first()
        if not file_upload:
            file_upload = FileUpload(
                id=file_id,
                filename=filename,
                table_name=table_name,
                total_chunks=total_chunks,
                uploaded_chunks=1,
                upload_user=current_user,
            )
            db.session.add(file_upload)
        else:
            file_upload.uploaded_chunks += 1

        db.session.commit()

        return (
            jsonify(
                {
                    "message": f"分块 {chunk_index + 1}/{total_chunks} 上传成功",
                    "uploaded_chunks": file_upload.uploaded_chunks,
                    "total_chunks": file_upload.total_chunks,
                }
            ),
            200,
        )

    except Exception as e:
        current_app.logger.error(f"分块上传失败: {str(e)}")
        return jsonify({"error": f"分块上传失败: {str(e)}"}), 500


@upload_blueprint.route("/api/merge-chunks", methods=["POST"])
@jwt_required()
def merge_chunk():
    """
    合并文件分块并导入数据。

    将所有分块合并成完整文件，然后批量导入到数据库指定表中。
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
            fileId:
              type: string
              description: 文件唯一标识符
            fileName:
              type: string
              description: 原始文件名
            table:
              type: string
              description: 目标表名
              enum: [nation, region, part, supplier, customer, lineitem, orders, partsupp]
            totalChunks:
              type: integer
              description: 总分块数量
          required:
            - fileId
            - fileName
            - table
            - totalChunks
    responses:
      200:
        description: 文件合并和数据导入成功。
        schema:
          type: object
          properties:
            message:
              type: string
              description: 成功消息
            imported_rows:
              type: integer
              description: 成功导入的记录数量
          required:
            - message
            - imported_rows
      400:
        description: 请求参数错误或文件分块不完整。
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
        description: 找不到文件上传记录。
        schema:
          type: object
          properties:
            error:
              type: string
              description: 具体错误信息
          required:
            - error
      500:
        description: 服务器内部错误或数据导入失败。
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
    try:
        data = request.get_json()
        file_id = data.get("fileId")
        filename = data.get("fileName")
        table_name = data.get("table")
        total_chunks = data.get("totalChunks")

        if not all([file_id, filename, table_name, total_chunks]):
            return jsonify({"error": "缺少必要参数"}), 400

        # 获取文件上传记录
        file_upload = FileUpload.query.filter_by(id=file_id).first()
        if not file_upload:
            return jsonify({"error": "找不到文件上传记录"}), 404

        # 检查所有分块是否已上传
        if file_upload.uploaded_chunks != file_upload.total_chunks:
            return (
                jsonify(
                    {
                        "error": f"文件分块不完整: {file_upload.uploaded_chunks}/{file_upload.total_chunks}"
                    }
                ),
                400,
            )

        # 更新状态为处理中
        file_upload.status = FileUploadStatus.PROCESSING
        db.session.commit()

        # 合并文件分块
        chunk_dir = os.path.join(current_app.config["CHUNK_FOLDER"], file_id)
        merged_file_path = os.path.join(current_app.config["UPLOAD_FOLDER"], filename)

        # 确保上传目录存在
        os.makedirs(current_app.config["UPLOAD_FOLDER"], exist_ok=True)

        with open(merged_file_path, "wb") as merged_file:
            for i in range(total_chunks):
                chunk_path = os.path.join(chunk_dir, f"chunk_{i}")
                if os.path.exists(chunk_path):
                    with open(chunk_path, "rb") as chunk_file:
                        merged_file.write(chunk_file.read())
                else:
                    raise Exception(f"分块文件 {i} 不存在")

        # 导入数据到数据库
        import_result = _import_data_to_table(merged_file_path, table_name)

        if import_result["success"]:
            # 更新状态为完成
            file_upload.status = FileUploadStatus.COMPLETED
            file_upload.completed_at = datetime.utcnow()
            file_upload.file_size = os.path.getsize(merged_file_path)
        else:
            # 更新状态为失败
            file_upload.status = FileUploadStatus.FAILED
            file_upload.error_message = import_result["error"]

        db.session.commit()

        # 清理临时文件
        _cleanup_temp_files(chunk_dir, merged_file_path)

        if import_result["success"]:
            return (
                jsonify(
                    {
                        "message": "文件上传和数据导入成功",
                        "imported_rows": import_result["imported_rows"],
                    }
                ),
                200,
            )
        else:
            return jsonify({"error": import_result["error"]}), 500

    except Exception as e:
        # 更新状态为失败
        if "file_upload" in locals():
            file_upload.status = FileUploadStatus.FAILED
            file_upload.error_message = str(e)
            db.session.commit()

        current_app.logger.error(f"文件合并失败: {str(e)}")
        return jsonify({"error": f"文件合并失败: {str(e)}"}), 500


@upload_blueprint.route("/api/upload_status/<file_id>", methods=["GET"])
@jwt_required()
def get_upload_status(file_id):
    """
    查询文件上传状态。

    返回指定文件ID的上传状态、进度和详细信息。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
      - name: file_id
        in: path
        type: string
        required: true
        description: 文件唯一标识符
    responses:
      200:
        description: 成功获取文件上传状态。
        schema:
          type: object
          properties:
            file_id:
              type: string
              description: 文件唯一标识符
            filename:
              type: string
              description: 文件名
            table_name:
              type: string
              description: 目标表名
            status:
              type: string
              description: 上传状态
              enum: [pending, uploading, processing, completed, failed]
            progress:
              type: number
              description: 上传进度百分比
            uploaded_chunks:
              type: integer
              description: 已上传的分块数量
            total_chunks:
              type: integer
              description: 总分块数量
            file_size:
              type: integer
              description: 文件大小（字节）
            created_at:
              type: string
              format: date-time
              description: 创建时间
            completed_at:
              type: string
              format: date-time
              description: 完成时间
            error_message:
              type: string
              description: 错误信息（如果有）
          required:
            - file_id
            - filename
            - table_name
            - status
            - progress
            - uploaded_chunks
            - total_chunks
      401:
        description: 未授权，需要有效的JWT token。
      404:
        description: 找不到文件上传记录。
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
    try:
        file_upload = FileUpload.query.filter_by(id=file_id).first()
        if not file_upload:
            return jsonify({"error": "找不到文件上传记录"}), 404

        # 计算上传进度
        progress = (
            (file_upload.uploaded_chunks / file_upload.total_chunks * 100)
            if file_upload.total_chunks > 0
            else 0
        )

        return (
            jsonify(
                {
                    "file_id": file_upload.id,
                    "filename": file_upload.filename,
                    "table_name": file_upload.table_name,
                    "status": file_upload.status.value,
                    "progress": round(progress, 2),
                    "uploaded_chunks": file_upload.uploaded_chunks,
                    "total_chunks": file_upload.total_chunks,
                    "file_size": file_upload.file_size,
                    "created_at": (
                        file_upload.created_at.isoformat()
                        if file_upload.created_at
                        else None
                    ),
                    "completed_at": (
                        file_upload.completed_at.isoformat()
                        if file_upload.completed_at
                        else None
                    ),
                    "error_message": file_upload.error_message,
                }
            ),
            200,
        )

    except Exception as e:
        current_app.logger.error(f"查询上传状态失败: {str(e)}")
        return jsonify({"error": f"查询上传状态失败: {str(e)}"}), 500


@upload_blueprint.route("/api/upload_history", methods=["GET"])
@jwt_required()
def get_upload_history():
    """
    获取当前用户的文件上传历史。

    返回当前用户的所有文件上传记录，按创建时间倒序排列。
    ---
    parameters:
      - name: Authorization
        in: header
        type: string
        required: true
        description: JWT token，格式为 "Bearer <token>"
    responses:
      200:
        description: 成功获取文件上传历史。
        schema:
          type: array
          items:
            type: object
            properties:
              file_id:
                type: string
                description: 文件唯一标识符
              filename:
                type: string
                description: 文件名
              table_name:
                type: string
                description: 目标表名
              status:
                type: string
                description: 上传状态
                enum: [pending, uploading, processing, completed, failed]
              progress:
                type: number
                description: 上传进度百分比
              file_size:
                type: integer
                description: 文件大小（字节）
              created_at:
                type: string
                format: date-time
                description: 创建时间
              completed_at:
                type: string
                format: date-time
                description: 完成时间
              error_message:
                type: string
                description: 错误信息（如果有）
            required:
              - file_id
              - filename
              - table_name
              - status
              - progress
      401:
        description: 未授权，需要有效的JWT token。
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
    try:
        current_user = get_jwt_identity()

        # 查询当前用户的上传记录
        uploads = (
            FileUpload.query.filter_by(upload_user=current_user)
            .order_by(FileUpload.created_at.desc())
            .all()
        )

        upload_list = []
        for upload in uploads:
            progress = (
                (upload.uploaded_chunks / upload.total_chunks * 100)
                if upload.total_chunks > 0
                else 0
            )
            upload_list.append(
                {
                    "file_id": upload.id,
                    "filename": upload.filename,
                    "table_name": upload.table_name,
                    "status": upload.status.value,
                    "progress": round(progress, 2),
                    "file_size": upload.file_size,
                    "created_at": (
                        upload.created_at.isoformat() if upload.created_at else None
                    ),
                    "completed_at": (
                        upload.completed_at.isoformat() if upload.completed_at else None
                    ),
                    "error_message": upload.error_message,
                }
            )

        return jsonify(upload_list), 200

    except Exception as e:
        current_app.logger.error(f"查询上传历史失败: {str(e)}")
        return jsonify({"error": f"查询上传历史失败: {str(e)}"}), 500


def _import_data_to_table(file_path, table_name):
    """
    将CSV文件数据批量导入到指定表。
    使用分块读取避免大文件导致的内存问题。
    """
    try:
        batch_size = current_app.config.get("BATCH_SIZE", 1000)
        imported_rows = 0

        # 创建数据清洗日志文件
        log_file_path = None
        if table_name == "orders":
            log_dir = current_app.config.get("LOG_FOLDER", "logs")
            os.makedirs(log_dir, exist_ok=True)
            log_file_path = os.path.join(
                log_dir,
                f"orders_cleaning_log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.txt",
            )

        # 使用chunksize参数分块读取文件，避免一次性加载到内存
        chunk_iter = pd.read_csv(
            file_path,
            encoding="utf-8",
            header=None,
            chunksize=batch_size,
            iterator=True,
        )

        # 处理每个数据块
        chunk_start_row = 0
        for chunk_df in chunk_iter:
            if chunk_df.empty:
                continue

            batch_data = []

            if table_name == "nation":
                for _, row in chunk_df.iterrows():
                    batch_data.append(
                        {
                            "n_nationkey": row.iloc[0],
                            "n_name": row.iloc[1],
                            "n_regionkey": row.iloc[2],
                            "n_comment": row.iloc[3] if len(row) > 3 else "",
                        }
                    )
                db.session.bulk_insert_mappings(Nation, batch_data)

            elif table_name == "region":
                for _, row in chunk_df.iterrows():
                    batch_data.append(
                        {
                            "r_regionkey": row.iloc[0],
                            "r_name": row.iloc[1],
                            "r_comment": row.iloc[2] if len(row) > 2 else "",
                        }
                    )
                db.session.bulk_insert_mappings(Region, batch_data)

            elif table_name == "part":
                for _, row in chunk_df.iterrows():
                    batch_data.append(
                        {
                            "p_partkey": row.iloc[0],
                            "p_name": row.iloc[1],
                            "p_mfgr": row.iloc[2],
                            "p_brand": row.iloc[3],
                            "p_type": row.iloc[4],
                            "p_size": row.iloc[5],
                            "p_container": row.iloc[6],
                            "p_retailprice": row.iloc[7],
                            "p_comment": row.iloc[8] if len(row) > 8 else "",
                        }
                    )
                db.session.bulk_insert_mappings(Part, batch_data)

            elif table_name == "supplier":
                for _, row in chunk_df.iterrows():
                    batch_data.append(
                        {
                            "s_suppkey": row.iloc[0],
                            "s_name": row.iloc[1],
                            "s_address": row.iloc[2],
                            "s_nationkey": row.iloc[3],
                            "s_phone": row.iloc[4],
                            "s_acctbal": row.iloc[5],
                            "s_comment": row.iloc[6] if len(row) > 6 else "",
                        }
                    )
                db.session.bulk_insert_mappings(Supplier, batch_data)

            elif table_name == "customer":
                for _, row in chunk_df.iterrows():
                    batch_data.append(
                        {
                            "c_custkey": row.iloc[0],
                            "c_name": row.iloc[1],
                            "c_address": row.iloc[2],
                            "c_nationkey": row.iloc[3],
                            "c_phone": row.iloc[4],
                            "c_acctbal": row.iloc[5],
                            "c_mktsegment": row.iloc[6],
                            "c_comment": row.iloc[7] if len(row) > 7 else "",
                        }
                    )
                db.session.bulk_insert_mappings(Customer, batch_data)

            elif table_name == "lineitem":
                for _, row in chunk_df.iterrows():
                    batch_data.append(
                        {
                            "l_orderkey": row.iloc[0],
                            "l_partkey": row.iloc[1],
                            "l_suppkey": row.iloc[2],
                            "l_linenumber": row.iloc[3],
                            "l_quantity": row.iloc[4],
                            "l_extendedprice": row.iloc[5],
                            "l_discount": row.iloc[6],
                            "l_tax": row.iloc[7],
                            "l_returnflag": row.iloc[8],
                            "l_linestatus": row.iloc[9],
                            "l_shipdate": row.iloc[10],
                            "l_commitdate": row.iloc[11],
                            "l_receiptdate": row.iloc[12],
                            "l_shipinstruct": row.iloc[13],
                            "l_shipmode": row.iloc[14],
                            "l_comment": row.iloc[15] if len(row) > 15 else "",
                        }
                    )
                db.session.bulk_insert_mappings(LineItem, batch_data)

            elif table_name == "orders":
                # Orders表的数据清洗逻辑
                batch_data, cleaned_count = _clean_orders_data(
                    chunk_df, chunk_start_row, log_file_path
                )
                if batch_data:
                    db.session.bulk_insert_mappings(Orders, batch_data)
                current_app.logger.info(
                    f"Orders数据清洗: 原始{len(chunk_df)}条，清洗后{len(batch_data)}条，剔除{cleaned_count}条"
                )

            elif table_name == "partsupp":
                for _, row in chunk_df.iterrows():
                    batch_data.append(
                        {
                            "ps_partkey": row.iloc[0],
                            "ps_suppkey": row.iloc[1],
                            "ps_availqty": row.iloc[2],
                            "ps_supplycost": row.iloc[3],
                            "ps_comment": row.iloc[4] if len(row) > 4 else "",
                        }
                    )
                db.session.bulk_insert_mappings(PartSupp, batch_data)

            # 提交当前批次
            db.session.commit()
            imported_rows += len(batch_data)
            chunk_start_row += len(chunk_df)

            current_app.logger.info(f"已导入 {imported_rows} 条记录到表 {table_name}")

        return {"success": True, "imported_rows": imported_rows}

    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f"数据导入失败: {str(e)}")
        return {"success": False, "error": f"数据导入失败: {str(e)}"}


def _clean_orders_data(chunk_df, chunk_start_row, log_file_path):
    """
    清洗Orders表数据

    Args:
        chunk_df: 数据块DataFrame
        chunk_start_row: 当前块在整个文件中的起始行号
        log_file_path: 日志文件路径

    Returns:
        tuple: (清洗后的数据列表, 被剔除的数据数量)
    """
    batch_data = []
    rejected_count = 0

    with open(log_file_path, "a", encoding="utf-8") as log_file:
        if chunk_start_row == 0:  # 第一次写入时添加表头
            log_file.write(
                f"Orders表数据清洗日志 - {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
            )
            log_file.write("=" * 80 + "\n")
            log_file.write("格式: [行号] [列号] 错误原因: 原始数据\n")
            log_file.write("-" * 80 + "\n")

        for idx, row in chunk_df.iterrows():
            file_row_num = chunk_start_row + idx + 1  # 文件中的实际行号（从1开始）
            is_valid = True
            error_messages = []

            try:
                # 检查列数是否足够
                if len(row) < 9:
                    error_messages.append(f"列数不足，期望9列，实际{len(row)}列")
                    is_valid = False

                # 验证 o_orderkey (Integer, Primary Key, 不能为空)
                try:
                    o_orderkey = int(row.iloc[0]) if pd.notna(row.iloc[0]) else None
                    if o_orderkey is None or o_orderkey <= 0:
                        error_messages.append(
                            f"[列1] o_orderkey必须为正整数: {row.iloc[0]}"
                        )
                        is_valid = False
                except (ValueError, TypeError):
                    error_messages.append(f"[列1] o_orderkey类型错误: {row.iloc[0]}")
                    is_valid = False

                # 验证 o_custkey (Integer, 不能为空)
                try:
                    o_custkey = int(row.iloc[1]) if pd.notna(row.iloc[1]) else None
                    if o_custkey is None or o_custkey <= 0:
                        error_messages.append(
                            f"[列2] o_custkey必须为正整数: {row.iloc[1]}"
                        )
                        is_valid = False
                except (ValueError, TypeError):
                    error_messages.append(f"[列2] o_custkey类型错误: {row.iloc[1]}")
                    is_valid = False

                # 验证 o_orderstatus (String(1), 不能为空)
                o_orderstatus = (
                    str(row.iloc[2]).strip() if pd.notna(row.iloc[2]) else ""
                )
                if not o_orderstatus or len(o_orderstatus) != 1:
                    error_messages.append(
                        f"[列3] o_orderstatus必须为单个字符: '{row.iloc[2]}'"
                    )
                    is_valid = False
                elif o_orderstatus not in ["O", "F", "P"]:  # 常见的订单状态
                    error_messages.append(
                        f"[列3] o_orderstatus值不在合法范围[O,F,P]: '{o_orderstatus}'"
                    )
                    is_valid = False

                # 验证 o_totalprice (Numeric(15,2), 不能为空)
                try:
                    o_totalprice = float(row.iloc[3]) if pd.notna(row.iloc[3]) else None
                    if o_totalprice is None or o_totalprice < 0:
                        error_messages.append(
                            f"[列4] o_totalprice必须为非负数: {row.iloc[3]}"
                        )
                        is_valid = False
                    elif o_totalprice > 999999999999999.99:  # 检查精度范围
                        error_messages.append(
                            f"[列4] o_totalprice超出范围: {row.iloc[3]}"
                        )
                        is_valid = False
                except (ValueError, TypeError):
                    error_messages.append(f"[列4] o_totalprice类型错误: {row.iloc[3]}")
                    is_valid = False

                # 验证 o_orderdate (Date, 不能为空)
                try:
                    o_orderdate_str = (
                        str(row.iloc[4]).strip() if pd.notna(row.iloc[4]) else ""
                    )
                    if not o_orderdate_str:
                        error_messages.append(
                            f"[列5] o_orderdate不能为空: {row.iloc[4]}"
                        )
                        is_valid = False
                    else:
                        # 尝试解析日期
                        o_orderdate = pd.to_datetime(o_orderdate_str).date()
                        # 检查日期合理性（比如不能是未来日期）
                        if o_orderdate > datetime.now().date():
                            error_messages.append(
                                f"[列5] o_orderdate不能是未来日期: {o_orderdate}"
                            )
                            is_valid = False
                except (ValueError, TypeError):
                    error_messages.append(
                        f"[列5] o_orderdate日期格式错误: {row.iloc[4]}"
                    )
                    is_valid = False

                # 验证 o_orderpriority (String(15), 不能为空)
                o_orderpriority = (
                    str(row.iloc[5]).strip() if pd.notna(row.iloc[5]) else ""
                )
                if not o_orderpriority or len(o_orderpriority) > 15:
                    error_messages.append(
                        f"[列6] o_orderpriority长度错误(1-15字符): '{row.iloc[5]}'"
                    )
                    is_valid = False

                # 验证 o_clerk (String(15), 不能为空)
                o_clerk = str(row.iloc[6]).strip() if pd.notna(row.iloc[6]) else ""
                if not o_clerk or len(o_clerk) > 15:
                    error_messages.append(
                        f"[列7] o_clerk长度错误(1-15字符): '{row.iloc[6]}'"
                    )
                    is_valid = False

                # 验证 o_shippriority (Integer, 不能为空)
                try:
                    o_shippriority = int(row.iloc[7]) if pd.notna(row.iloc[7]) else None
                    if o_shippriority is None:
                        error_messages.append(
                            f"[列8] o_shippriority不能为空: {row.iloc[7]}"
                        )
                        is_valid = False
                    elif (
                        o_shippriority < 0 or o_shippriority > 10
                    ):  # 假设优先级范围0-10
                        error_messages.append(
                            f"[列8] o_shippriority超出合理范围[0-10]: {o_shippriority}"
                        )
                        is_valid = False
                except (ValueError, TypeError):
                    error_messages.append(
                        f"[列8] o_shippriority类型错误: {row.iloc[7]}"
                    )
                    is_valid = False

                # 验证 o_comment (String(79), 可以为空)
                o_comment = (
                    str(row.iloc[8]).strip()
                    if pd.notna(row.iloc[8]) and len(row) > 8
                    else ""
                )
                if len(o_comment) > 79:
                    error_messages.append(
                        f"[列9] o_comment长度超限(最大79字符): 长度{len(o_comment)}"
                    )
                    is_valid = False

                # 如果数据有效，添加到批次数据中
                if is_valid:
                    batch_data.append(
                        {
                            "o_orderkey": o_orderkey,
                            "o_custkey": o_custkey,
                            "o_orderstatus": o_orderstatus,
                            "o_totalprice": o_totalprice,
                            "o_orderdate": o_orderdate,
                            "o_orderpriority": o_orderpriority,
                            "o_clerk": o_clerk,
                            "o_shippriority": o_shippriority,
                            "o_comment": o_comment,
                        }
                    )
                else:
                    # 记录被剔除的数据
                    rejected_count += 1
                    log_file.write(f"[行{file_row_num}] {'; '.join(error_messages)}\n")
                    log_file.write(f"    原始数据: {list(row)}\n")
                    log_file.write("-" * 40 + "\n")

            except Exception as e:
                # 处理其他异常
                rejected_count += 1
                log_file.write(f"[行{file_row_num}] 数据处理异常: {str(e)}\n")
                log_file.write(f"    原始数据: {list(row)}\n")
                log_file.write("-" * 40 + "\n")

    return batch_data, rejected_count


def _cleanup_temp_files(chunk_dir, merged_file_path):
    """
    清理临时文件和目录。

    删除分块上传过程中产生的临时文件和目录，包括分块文件目录和合并后的文件。

    Args:
        chunk_dir (str): 分块文件存储目录的完整路径
        merged_file_path (str): 合并后文件的完整路径

    Returns:
        None

    Note:
        - 该函数会尝试删除所有临时文件，即使某些删除操作失败也会继续执行
        - 删除失败的情况会记录到日志中，但不会抛出异常
        - 建议在文件处理完成后调用此函数以释放磁盘空间
    """
    try:
        # 删除分块文件目录
        if os.path.exists(chunk_dir):
            shutil.rmtree(chunk_dir)

        # 删除合并后的文件
        if os.path.exists(merged_file_path):
            os.remove(merged_file_path)

    except Exception as e:
        current_app.logger.warning(f"清理临时文件失败: {str(e)}")
