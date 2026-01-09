"""
文件上传处理
"""
import os
import uuid
from datetime import datetime
from flask import current_app
from werkzeug.utils import secure_filename


def allowed_file(filename, file_type='image'):
    """检查文件类型是否允许"""
    if '.' not in filename:
        return False

    ext = filename.rsplit('.', 1)[1].lower()

    if file_type == 'image':
        return ext in current_app.config['ALLOWED_IMAGE_EXTENSIONS']
    elif file_type == 'video':
        return ext in current_app.config['ALLOWED_VIDEO_EXTENSIONS']

    return False


def save_file(file, folder, file_type='image'):
    """保存上传的文件"""
    if not file:
        return None

    # 检查文件类型
    if not allowed_file(file.filename, file_type):
        raise ValueError(f'不支持的{file_type}格式')

    # 检查文件大小
    file.seek(0, os.SEEK_END)
    file_size = file.tell()
    file.seek(0)

    if file_type == 'image' and file_size > current_app.config['MAX_IMAGE_SIZE']:
        raise ValueError('图片大小超过限制（最大5MB）')
    elif file_type == 'video' and file_size > current_app.config['MAX_VIDEO_SIZE']:
        raise ValueError('视频大小超过限制（最大50MB）')

    # 生成安全的文件名
    original_filename = secure_filename(file.filename)
    ext = original_filename.rsplit('.', 1)[1].lower()
    timestamp = datetime.now().strftime('%Y%m%d%H%M%S')
    random_str = str(uuid.uuid4())[:8]
    filename = f'{timestamp}_{random_str}.{ext}'

    # 确保目录存在
    upload_folder = current_app.config['UPLOAD_FOLDER']
    full_path = os.path.join(upload_folder, folder)
    os.makedirs(full_path, exist_ok=True)

    # 保存文件
    filepath = os.path.join(full_path, filename)
    file.save(filepath)

    # 返回相对路径
    return os.path.join(folder, filename).replace('\\', '/')


def delete_file(filepath):
    """删除文件"""
    if not filepath:
        return

    try:
        upload_folder = current_app.config['UPLOAD_FOLDER']
        full_path = os.path.join(upload_folder, filepath)
        if os.path.exists(full_path):
            os.remove(full_path)
    except Exception as e:
        current_app.logger.error(f'Error deleting file {filepath}: {e}')


def delete_files(filepaths):
    """批量删除文件"""
    if not filepaths:
        return

    for filepath in filepaths:
        delete_file(filepath)
