"""
服务响应相关API
"""
from flask import Blueprint, request, current_app, g
from app import db
from app.models import ServiceResponse, Need
from app.utils.responses import success_response, error_response
from app.utils.decorators import login_required
from app.utils.upload import save_file, delete_files

services_bp = Blueprint('services', __name__)


@services_bp.route('/my-responses', methods=['GET'])
@login_required
def get_my_responses():
    """获取我的服务响应列表"""
    user = g.current_user

    # 分页参数
    page = request.args.get('page', 1, type=int)
    page_size = request.args.get('pageSize', current_app.config['DEFAULT_PAGE_SIZE'], type=int)
    page_size = min(page_size, current_app.config['MAX_PAGE_SIZE'])

    # 筛选参数
    status = request.args.get('status')

    # 构建查询
    query = ServiceResponse.query.filter_by(responder_id=user.id)

    if status:
        query = query.filter_by(status=status)

    # 按创建时间倒序
    query = query.order_by(ServiceResponse.created_at.desc())

    # 分页
    pagination = query.paginate(page=page, per_page=page_size, error_out=False)

    items = []
    for response in pagination.items:
        item_dict = response.to_dict()

        # 添加需求信息
        need = response.need
        item_dict['need'] = {
            'id': need.id,
            'title': need.title,
            'serviceType': need.service_type_id,
            'serviceTypeName': need.service_type.name if need.service_type else '',
            'publisher': {
                'id': need.publisher.id,
                'username': need.publisher.username,
                'phone': need.publisher.phone
            },
            'status': need.status
        }

        items.append(item_dict)

    return success_response('获取成功', {
        'total': pagination.total,
        'page': page,
        'pageSize': page_size,
        'items': items
    })


@services_bp.route('/responses/<response_id>', methods=['GET'])
@login_required
def get_response_detail(response_id):
    """获取响应详情"""
    user = g.current_user

    response = ServiceResponse.query.get(response_id)
    if not response:
        return error_response('响应不存在', 'RESPONSE_NOT_FOUND', 404)

    # 只有响应者可以查看
    if response.responder_id != user.id:
        return error_response('仅响应提交者可以查看', 'NOT_RESPONSE_OWNER', 403)

    data = response.to_dict(include_details=True)

    # 添加需求信息
    need = response.need
    data['need'] = {
        'id': need.id,
        'title': need.title,
        'serviceType': need.service_type_id,
        'description': need.description,
        'publisher': {
            'id': need.publisher.id,
            'username': need.publisher.username,
            'phone': need.publisher.phone,
            'address': need.publisher.address
        },
        'status': need.status
    }

    return success_response('获取成功', data)


@services_bp.route('/responses/<response_id>', methods=['PUT'])
@login_required
def update_response(response_id):
    """修改服务响应"""
    user = g.current_user

    response = ServiceResponse.query.get(response_id)
    if not response:
        return error_response('响应不存在', 'RESPONSE_NOT_FOUND', 404)

    # 只有响应者可以修改
    if response.responder_id != user.id:
        return error_response('仅响应提交者可以修改', 'NOT_RESPONSE_OWNER', 403)

    # 只能修改待处理的响应
    if response.status != 'pending':
        return error_response('只能修改待处理的响应', 'INVALID_STATUS')

    # 获取表单数据
    description = request.form.get('description')
    price = request.form.get('price')
    available_time = request.form.get('availableTime')

    try:
        # 更新字段
        if description:
            if len(description) < 10 or len(description) > 500:
                return error_response('描述长度必须在10-500个字符之间', 'INVALID_DESCRIPTION')
            response.description = description

        if price is not None:
            response.price = price

        if available_time is not None:
            response.available_time = available_time

        # 处理要删除的图片
        delete_image_urls = request.form.getlist('deleteImages')
        current_images = response.get_images()
        if delete_image_urls:
            delete_files(delete_image_urls)
            current_images = [img for img in current_images if img not in delete_image_urls]

        # 处理新上传的图片
        new_images = request.files.getlist('images')
        if new_images:
            if len(current_images) + len(new_images) > current_app.config['MAX_IMAGES_PER_UPLOAD']:
                return error_response(
                    f'图片总数超过限制（最多{current_app.config["MAX_IMAGES_PER_UPLOAD"]}张）',
                    'TOO_MANY_FILES'
                )

            for image in new_images:
                if image and image.filename:
                    try:
                        url = save_file(image, f'responses/{response_id}/images', 'image')
                        current_images.append(url)
                    except ValueError as e:
                        return error_response(str(e), 'INVALID_FILE_TYPE')

        response.set_images(current_images)

        # 处理要删除的视频
        delete_video_urls = request.form.getlist('deleteVideos')
        current_videos = response.get_videos()
        if delete_video_urls:
            delete_files(delete_video_urls)
            current_videos = [vid for vid in current_videos if vid not in delete_video_urls]

        # 处理新上传的视频
        new_videos = request.files.getlist('videos')
        if new_videos:
            if len(current_videos) + len(new_videos) > current_app.config['MAX_VIDEOS_PER_UPLOAD']:
                return error_response(
                    f'视频总数超过限制（最多{current_app.config["MAX_VIDEOS_PER_UPLOAD"]}个）',
                    'TOO_MANY_FILES'
                )

            for video in new_videos:
                if video and video.filename:
                    try:
                        url = save_file(video, f'responses/{response_id}/videos', 'video')
                        current_videos.append(url)
                    except ValueError as e:
                        return error_response(str(e), 'INVALID_FILE_TYPE')

        response.set_videos(current_videos)

        db.session.commit()

        return success_response('修改成功', {
            'imageUrls': current_images,
            'videoUrls': current_videos
        })

    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Update response error: {e}')
        return error_response('修改失败', 'SERVER_ERROR', 500)


@services_bp.route('/responses/<response_id>', methods=['DELETE'])
@login_required
def delete_response(response_id):
    """删除服务响应"""
    user = g.current_user

    response = ServiceResponse.query.get(response_id)
    if not response:
        return error_response('响应不存在', 'RESPONSE_NOT_FOUND', 404)

    # 只有响应者可以删除
    if response.responder_id != user.id:
        return error_response('仅响应提交者可以删除', 'NOT_RESPONSE_OWNER', 403)

    # 只能删除待处理或已拒绝的响应
    if response.status not in ['pending', 'rejected']:
        return error_response('只能删除待处理或已拒绝的响应', 'CANNOT_DELETE')

    try:
        # 删除关联的文件
        delete_files(response.get_images())
        delete_files(response.get_videos())

        db.session.delete(response)
        db.session.commit()

        return success_response('删除成功')
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Delete response error: {e}')
        return error_response('删除失败', 'SERVER_ERROR', 500)
