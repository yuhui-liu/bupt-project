#!/usr/bin/env python3
"""
文件上传管理工具
"""

import os
import sys
import shutil
from datetime import datetime, timedelta

# 添加项目根目录到Python路径
sys.path.insert(0, os.path.dirname(__file__))

from app import create_app
from app.models import db, FileUpload, FileUploadStatus


def cleanup_old_uploads(days=7):
    """清理超过指定天数的上传记录和文件"""
    app = create_app()

    with app.app_context():
        cutoff_date = datetime.utcnow() - timedelta(days=days)

        # 查找过期的上传记录
        old_uploads = FileUpload.query.filter(FileUpload.created_at < cutoff_date).all()

        print(f"找到 {len(old_uploads)} 个超过 {days} 天的上传记录")

        for upload in old_uploads:
            try:
                # 删除分块文件目录
                chunk_dir = os.path.join(app.config["CHUNK_FOLDER"], upload.id)
                if os.path.exists(chunk_dir):
                    shutil.rmtree(chunk_dir)
                    print(f"已删除分块目录: {chunk_dir}")

                # 删除合并后的文件
                merged_file = os.path.join(app.config["UPLOAD_FOLDER"], upload.filename)
                if os.path.exists(merged_file):
                    os.remove(merged_file)
                    print(f"已删除文件: {merged_file}")

                # 删除数据库记录
                db.session.delete(upload)
                print(f"已删除记录: {upload.filename}")

            except Exception as e:
                print(f"清理 {upload.filename} 时出错: {e}")

        db.session.commit()
        print("清理完成")


def cleanup_failed_uploads():
    """清理失败的上传记录"""
    app = create_app()

    with app.app_context():
        failed_uploads = FileUpload.query.filter(
            FileUpload.status == FileUploadStatus.FAILED
        ).all()

        print(f"找到 {len(failed_uploads)} 个失败的上传记录")

        for upload in failed_uploads:
            try:
                # 删除分块文件目录
                chunk_dir = os.path.join(app.config["CHUNK_FOLDER"], upload.id)
                if os.path.exists(chunk_dir):
                    shutil.rmtree(chunk_dir)
                    print(f"已删除失败上传的分块目录: {chunk_dir}")

                # 删除数据库记录
                db.session.delete(upload)
                print(f"已删除失败记录: {upload.filename}")

            except Exception as e:
                print(f"清理失败记录 {upload.filename} 时出错: {e}")

        db.session.commit()
        print("失败记录清理完成")


def list_uploads():
    """列出所有上传记录"""
    app = create_app()

    with app.app_context():
        uploads = FileUpload.query.order_by(FileUpload.created_at.desc()).all()

        print(
            f"\n{'文件名':<30} {'状态':<12} {'用户':<15} {'大小':<10} {'创建时间':<20}"
        )
        print("-" * 87)

        for upload in uploads:
            size_str = f"{upload.file_size // 1024}KB" if upload.file_size else "N/A"
            created_str = (
                upload.created_at.strftime("%Y-%m-%d %H:%M")
                if upload.created_at
                else "N/A"
            )

            print(
                f"{upload.filename:<30} {upload.status.value:<12} {upload.upload_user:<15} {size_str:<10} {created_str:<20}"
            )


def show_statistics():
    """显示上传统计"""
    app = create_app()

    with app.app_context():
        total = FileUpload.query.count()
        completed = FileUpload.query.filter(
            FileUpload.status == FileUploadStatus.COMPLETED
        ).count()
        failed = FileUpload.query.filter(
            FileUpload.status == FileUploadStatus.FAILED
        ).count()
        uploading = FileUpload.query.filter(
            FileUpload.status == FileUploadStatus.UPLOADING
        ).count()
        processing = FileUpload.query.filter(
            FileUpload.status == FileUploadStatus.PROCESSING
        ).count()

        print("\n上传统计:")
        print(f"总数: {total}")
        print(f"已完成: {completed}")
        print(f"失败: {failed}")
        print(f"上传中: {uploading}")
        print(f"处理中: {processing}")

        if total > 0:
            success_rate = (completed / total) * 100
            print(f"成功率: {success_rate:.1f}%")


def main():
    """主函数"""
    if len(sys.argv) < 2:
        print("用法: python file_manager.py <command> [args]")
        print("\n可用命令:")
        print("  list                - 列出所有上传记录")
        print("  stats               - 显示上传统计")
        print("  cleanup [days]      - 清理超过指定天数的记录 (默认7天)")
        print("  cleanup-failed      - 清理失败的上传记录")
        return

    command = sys.argv[1]

    if command == "list":
        list_uploads()
    elif command == "stats":
        show_statistics()
    elif command == "cleanup":
        days = int(sys.argv[2]) if len(sys.argv) > 2 else 7
        cleanup_old_uploads(days)
    elif command == "cleanup-failed":
        cleanup_failed_uploads()
    else:
        print(f"未知命令: {command}")


if __name__ == "__main__":
    main()
