"""
需求相关API
"""
from flask import Blueprint, request, current_app, g
from app import db
from app.models import Need, ServiceType, Region, ServiceResponse, AcceptedService, MonthlyStatistics
from app.utils.responses import success_response, error_response, created_response
from app.utils.decorators import login_required
from app.utils.upload import save_file, delete_files
from datetime import datetime, date
from sqlalchemy import or_, and_

needs_bp = Blueprint('needs', __name__)


def update_monthly_stats(need, field, increment=1):
    """更新月统计数据"""
    try:
        # 获取需求的月份和地域信息
        month = datetime.now().strftime('%Y%m')

        # 如果有地域信息，使用地域的省市，否则使用默认值
        if need.region:
            province = need.region.province
            city = need.region.city
        else:
            province = '未知省份'
            city = '未知城市'

        # 查找或创建月统计记录
        stat = MonthlyStatistics.query.filter_by(
            month=month,
            province=province,
            city=city,
            service_type_id=need.service_type_id
        ).first()

        if not stat:
            stat = MonthlyStatistics(
                month=month,
                province=province,
                city=city,
                service_type_id=need.service_type_id
            )
            db.session.add(stat)

        # 更新对应字段
        if field == 'needs_published':
            stat.needs_published += increment
        elif field == 'needs_accepted':
            stat.needs_accepted += increment
        elif field == 'needs_completed':
            stat.needs_completed += increment

        db.session.commit()
    except Exception as e:
        current_app.logger.error(f'Update monthly stats error: {e}')
        # 不影响主流程


@needs_bp.route('', methods=['POST'])
@login_required
def create_need():
    """发布需求"""
    user = g.current_user

    # 获取表单数据
    title = request.form.get('title', '').strip()
    service_type_id = request.form.get('serviceType', '').strip()
    description = request.form.get('description', '').strip()
    expected_time = request.form.get('expectedTime')
    location = request.form.get('location')
    budget = request.form.get('budget')
    region_id = request.form.get('regionId')

    # 验证必填字段
    if not title or not service_type_id or not description:
        return error_response('标题、服务类型和描述不能为空', 'INVALID_REQUEST')

    # 验证标题长度
    if len(title) < 2 or len(title) > 50:
        return error_response('标题长度必须在2-50个字符之间', 'INVALID_TITLE')

    # 验证描述长度
    if len(description) < 10 or len(description) > 500:
        return error_response('描述长度必须在10-500个字符之间', 'INVALID_DESCRIPTION')

    # 验证服务类型
    service_type = ServiceType.query.get(service_type_id)
    if not service_type or not service_type.is_active:
        return error_response('服务类型不存在或已禁用', 'INVALID_SERVICE_TYPE')

    # 验证地域（如果提供）
    if region_id:
        region = Region.query.get(region_id)
        if not region or not region.is_active:
            return error_response('地域不存在或已禁用', 'INVALID_REQUEST')

    try:
        # 创建需求
        need = Need(
            publisher_id=user.id,
            service_type_id=service_type_id,
            region_id=region_id,
            title=title,
            description=description,
            location=location,
            budget=budget
        )

        # 处理期望时间
        if expected_time:
            try:
                need.expected_time = datetime.fromisoformat(expected_time.replace('Z', '+00:00'))
            except:
                pass

        # 先提交need以获取ID
        db.session.add(need)
        db.session.flush()  # 生成ID但不提交事务

        # 处理图片上传
        image_urls = []
        images = request.files.getlist('images')
        if images and len(images) > current_app.config['MAX_IMAGES_PER_UPLOAD']:
            return error_response(
                f'图片数量超过限制（最多{current_app.config["MAX_IMAGES_PER_UPLOAD"]}张）',
                'TOO_MANY_FILES'
            )

        for image in images:
            if image and image.filename:
                try:
                    url = save_file(image, f'needs/{need.id}/images', 'image')
                    image_urls.append(url)
                except ValueError as e:
                    return error_response(str(e), 'INVALID_FILE_TYPE')
                except Exception as e:
                    current_app.logger.error(f'Image upload error: {e}')
                    return error_response('图片上传失败', 'UPLOAD_FAILED', 500)

        need.set_images(image_urls)

        # 处理视频上传
        video_urls = []
        videos = request.files.getlist('videos')
        if videos and len(videos) > current_app.config['MAX_VIDEOS_PER_UPLOAD']:
            return error_response(
                f'视频数量超过限制（最多{current_app.config["MAX_VIDEOS_PER_UPLOAD"]}个）',
                'TOO_MANY_FILES'
            )

        for video in videos:
            if video and video.filename:
                try:
                    url = save_file(video, f'needs/{need.id}/videos', 'video')
                    video_urls.append(url)
                except ValueError as e:
                    # 删除已上传的图片
                    delete_files(image_urls)
                    return error_response(str(e), 'INVALID_FILE_TYPE')
                except Exception as e:
                    current_app.logger.error(f'Video upload error: {e}')
                    delete_files(image_urls)
                    return error_response('视频上传失败', 'UPLOAD_FAILED', 500)

        need.set_videos(video_urls)

        # 提交事务
        db.session.commit()

        # 更新月统计
        update_monthly_stats(need, 'needs_published')

        return created_response('需求发布成功', {
            'needId': need.id,
            'title': need.title,
            'imageUrls': image_urls,
            'videoUrls': video_urls
        })

    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Create need error: {e}')
        return error_response('需求发布失败', 'SERVER_ERROR', 500)


@needs_bp.route('/my-needs', methods=['GET'])
@login_required
def get_my_needs():
    """获取我的需求列表"""
    user = g.current_user

    # 分页参数
    page = request.args.get('page', 1, type=int)
    page_size = request.args.get('pageSize', current_app.config['DEFAULT_PAGE_SIZE'], type=int)
    page_size = min(page_size, current_app.config['MAX_PAGE_SIZE'])

    # 筛选参数
    status = request.args.get('status')
    service_type = request.args.get('serviceType')

    # 构建查询
    query = Need.query.filter_by(publisher_id=user.id)

    if status:
        query = query.filter_by(status=status)

    if service_type:
        query = query.filter_by(service_type_id=service_type)

    # 按创建时间倒序
    query = query.order_by(Need.created_at.desc())

    # 分页
    pagination = query.paginate(page=page, per_page=page_size, error_out=False)

    items = []
    for need in pagination.items:
        item_dict = need.to_dict()

        # 添加响应统计
        item_dict['pendingResponseCount'] = need.responses.filter_by(status='pending').count()

        # 添加已接受的响应信息
        accepted_response = need.responses.filter_by(status='accepted').first()
        if accepted_response:
            item_dict['acceptedResponse'] = {
                'id': accepted_response.id,
                'responderName': accepted_response.responder.username,
                'responderPhone': accepted_response.responder.phone
            }

        items.append(item_dict)

    return success_response('获取成功', {
        'total': pagination.total,
        'page': page,
        'pageSize': page_size,
        'items': items
    })


@needs_bp.route('/<need_id>/detail', methods=['GET'])
@login_required
def get_need_detail(need_id):
    """获取需求详情"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 只有需求发布者可以查看详情（或者管理员）
    if need.publisher_id != user.id and user.role != 'admin':
        return error_response('无权访问', 'ACCESS_DENIED', 403)

    return success_response('获取成功', need.to_dict(include_details=True))


@needs_bp.route('/<need_id>/responses', methods=['GET'])
@login_required
def get_need_responses(need_id):
    """获取需求的所有响应"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 只有需求发布者可以查看响应
    if need.publisher_id != user.id:
        return error_response('仅需求发布者可以查看响应', 'NOT_NEED_OWNER', 403)

    # 分页参数
    page = request.args.get('page', 1, type=int)
    page_size = request.args.get('pageSize', current_app.config['DEFAULT_PAGE_SIZE'], type=int)
    page_size = min(page_size, current_app.config['MAX_PAGE_SIZE'])

    # 查询响应
    query = ServiceResponse.query.filter_by(need_id=need_id).order_by(ServiceResponse.created_at.desc())
    pagination = query.paginate(page=page, per_page=page_size, error_out=False)

    items = [resp.to_dict(include_details=True) for resp in pagination.items]

    return success_response('获取成功', {
        'total': pagination.total,
        'page': page,
        'pageSize': page_size,
        'items': items
    })


@needs_bp.route('/<need_id>/responses/<response_id>/accept', methods=['POST'])
@login_required
def accept_response(need_id, response_id):
    """接受响应"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 只有需求发布者可以接受响应
    if need.publisher_id != user.id:
        return error_response('仅需求发布者可以接受响应', 'NOT_NEED_OWNER', 403)

    # 检查需求状态
    if need.status not in ['open', 'responded']:
        return error_response('需求状态不允许接受响应', 'INVALID_NEED_STATUS')

    # 查找响应
    response = ServiceResponse.query.get(response_id)
    if not response or response.need_id != need_id:
        return error_response('响应不存在', 'RESPONSE_NOT_FOUND', 404)

    # 检查响应状态
    if response.status != 'pending':
        return error_response('响应已被处理', 'RESPONSE_ALREADY_PROCESSED')

    try:
        # 更新响应状态
        response.status = 'accepted'

        # 更新需求状态
        need.status = 'accepted'

        # 拒绝其他待处理的响应
        other_responses = ServiceResponse.query.filter(
            ServiceResponse.need_id == need_id,
            ServiceResponse.id != response_id,
            ServiceResponse.status == 'pending'
        ).all()

        rejected_count = 0
        for other_resp in other_responses:
            other_resp.status = 'rejected'
            rejected_count += 1

        # 创建响应成功明细
        accepted_service = AcceptedService(
            need_id=need_id,
            need_publisher_id=need.publisher_id,
            response_id=response_id,
            responder_id=response.responder_id,
            accepted_date=date.today()
        )
        db.session.add(accepted_service)

        db.session.commit()

        # 更新月统计
        update_monthly_stats(need, 'needs_accepted')

        return success_response('接受响应成功', {
            'needId': need_id,
            'responseId': response_id,
            'needStatus': 'accepted',
            'otherResponsesRejected': rejected_count
        })

    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Accept response error: {e}')
        return error_response('接受响应失败', 'SERVER_ERROR', 500)


@needs_bp.route('/<need_id>/responses/<response_id>/reject', methods=['POST'])
@login_required
def reject_response(need_id, response_id):
    """拒绝响应"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 只有需求发布者可以拒绝响应
    if need.publisher_id != user.id:
        return error_response('仅需求发布者可以拒绝响应', 'NOT_NEED_OWNER', 403)

    # 查找响应
    response = ServiceResponse.query.get(response_id)
    if not response or response.need_id != need_id:
        return error_response('响应不存在', 'RESPONSE_NOT_FOUND', 404)

    # 检查响应状态
    if response.status != 'pending':
        return error_response('响应已被处理', 'RESPONSE_ALREADY_PROCESSED')

    try:
        response.status = 'rejected'
        db.session.commit()
        return success_response('拒绝响应成功')
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Reject response error: {e}')
        return error_response('拒绝响应失败', 'SERVER_ERROR', 500)


@needs_bp.route('/<need_id>', methods=['PUT'])
@login_required
def update_need(need_id):
    """修改需求"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 只有需求发布者可以修改
    if need.publisher_id != user.id:
        return error_response('仅需求发布者可以修改', 'NOT_NEED_OWNER', 403)

    # 检查需求状态
    if need.status not in ['open', 'responded']:
        return error_response('需求状态不允许修改', 'INVALID_STATUS')

    # 获取表单数据
    title = request.form.get('title')
    service_type_id = request.form.get('serviceType')
    description = request.form.get('description')
    expected_time = request.form.get('expectedTime')
    location = request.form.get('location')
    budget = request.form.get('budget')
    region_id = request.form.get('regionId')

    try:
        # 更新字段
        if title:
            if len(title) < 2 or len(title) > 50:
                return error_response('标题长度必须在2-50个字符之间', 'INVALID_TITLE')
            need.title = title

        if service_type_id:
            service_type = ServiceType.query.get(service_type_id)
            if not service_type or not service_type.is_active:
                return error_response('服务类型不存在或已禁用', 'INVALID_SERVICE_TYPE')
            need.service_type_id = service_type_id

        if description:
            if len(description) < 10 or len(description) > 500:
                return error_response('描述长度必须在10-500个字符之间', 'INVALID_DESCRIPTION')
            need.description = description

        if expected_time:
            try:
                need.expected_time = datetime.fromisoformat(expected_time.replace('Z', '+00:00'))
            except:
                pass

        if location is not None:
            need.location = location

        if budget is not None:
            need.budget = budget

        if region_id is not None:
            if region_id:
                region = Region.query.get(region_id)
                if not region or not region.is_active:
                    return error_response('地域不存在或已禁用', 'INVALID_REQUEST')
            need.region_id = region_id if region_id else None

        # 处理要删除的图片
        delete_image_urls = request.form.getlist('deleteImages')
        current_images = need.get_images()
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
                        url = save_file(image, f'needs/{need_id}/images', 'image')
                        current_images.append(url)
                    except ValueError as e:
                        return error_response(str(e), 'INVALID_FILE_TYPE')

        need.set_images(current_images)

        # 处理要删除的视频
        delete_video_urls = request.form.getlist('deleteVideos')
        current_videos = need.get_videos()
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
                        url = save_file(video, f'needs/{need_id}/videos', 'video')
                        current_videos.append(url)
                    except ValueError as e:
                        return error_response(str(e), 'INVALID_FILE_TYPE')

        need.set_videos(current_videos)

        db.session.commit()

        return success_response('修改成功', {
            'imageUrls': current_images,
            'videoUrls': current_videos
        })

    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Update need error: {e}')
        return error_response('修改失败', 'SERVER_ERROR', 500)


@needs_bp.route('/<need_id>', methods=['DELETE'])
@login_required
def delete_need(need_id):
    """删除需求"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 只有需求发布者可以删除
    if need.publisher_id != user.id:
        return error_response('仅需求发布者可以删除', 'NOT_NEED_OWNER', 403)

    # 只能删除未响应的需求
    if need.status != 'open':
        return error_response('只能删除未响应的需求，已有响应的需求请使用取消功能', 'CANNOT_DELETE')

    try:
        # 删除关联的文件
        delete_files(need.get_images())
        delete_files(need.get_videos())

        db.session.delete(need)
        db.session.commit()

        return success_response('删除成功')
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Delete need error: {e}')
        return error_response('删除失败', 'SERVER_ERROR', 500)


@needs_bp.route('/<need_id>/cancel', methods=['POST'])
@login_required
def cancel_need(need_id):
    """取消需求"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 只有需求发布者可以取消
    if need.publisher_id != user.id:
        return error_response('仅需求发布者可以取消', 'NOT_NEED_OWNER', 403)

    # 已完成的需求不能取消
    if need.status == 'completed':
        return error_response('已完成的需求不能取消', 'ALREADY_COMPLETED')

    if need.status == 'cancelled':
        return error_response('需求已取消', 'INVALID_STATUS')

    data = request.get_json() or {}
    reason = data.get('reason', '')

    try:
        need.status = 'cancelled'
        need.cancel_reason = reason

        # 拒绝所有待处理的响应
        pending_responses = ServiceResponse.query.filter_by(
            need_id=need_id,
            status='pending'
        ).all()

        for response in pending_responses:
            response.status = 'rejected'

        db.session.commit()

        return success_response('需求已取消')
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Cancel need error: {e}')
        return error_response('取消失败', 'SERVER_ERROR', 500)


@needs_bp.route('/<need_id>/complete', methods=['POST'])
@login_required
def complete_need(need_id):
    """标记需求完成"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 只有需求发布者可以标记完成
    if need.publisher_id != user.id:
        return error_response('仅需求发布者可以标记完成', 'NOT_NEED_OWNER', 403)

    # 只能标记已接受的需求为完成
    if need.status != 'accepted':
        return error_response('只能标记已接受的需求为完成', 'INVALID_STATUS')

    data = request.get_json() or {}
    rating = data.get('rating')
    comment = data.get('comment')

    # 验证评分
    if rating and (rating < 1 or rating > 5):
        return error_response('评分必须在1-5之间', 'INVALID_REQUEST')

    try:
        need.status = 'completed'
        need.rating = rating
        need.comment = comment

        # 更新响应成功明细
        accepted_service = AcceptedService.query.filter_by(need_id=need_id).first()
        if accepted_service:
            accepted_service.completed_date = date.today()
            accepted_service.rating = rating
            accepted_service.comment = comment

        db.session.commit()

        # 更新月统计
        update_monthly_stats(need, 'needs_completed')

        return success_response('需求已标记为完成')
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Complete need error: {e}')
        return error_response('标记完成失败', 'SERVER_ERROR', 500)


@needs_bp.route('', methods=['GET'])
@login_required
def browse_needs():
    """浏览所有需求"""
    user = g.current_user

    # 分页参数
    page = request.args.get('page', 1, type=int)
    page_size = request.args.get('pageSize', current_app.config['DEFAULT_PAGE_SIZE'], type=int)
    page_size = min(page_size, current_app.config['MAX_PAGE_SIZE'])

    # 筛选参数
    service_type = request.args.get('serviceType')
    keyword = request.args.get('keyword', '').strip()
    status_filter = request.args.get('status', 'open,responded')  # 默认只显示open和responded
    sort_by = request.args.get('sortBy', 'createdAt')

    # 构建查询 - 不显示自己发布的需求
    query = Need.query.filter(Need.publisher_id != user.id)

    # 状态筛选
    if status_filter:
        statuses = status_filter.split(',')
        query = query.filter(Need.status.in_(statuses))

    # 服务类型筛选
    if service_type:
        query = query.filter_by(service_type_id=service_type)

    # 关键词搜索
    if keyword and len(keyword) >= 2:
        query = query.filter(
            or_(
                Need.title.like(f'%{keyword}%'),
                Need.description.like(f'%{keyword}%')
            )
        )

    # 排序
    if sort_by == 'responseCount':
        # 按响应数量排序（需要子查询）
        query = query.order_by(Need.created_at.desc())
    else:
        query = query.order_by(Need.created_at.desc())

    # 分页
    pagination = query.paginate(page=page, per_page=page_size, error_out=False)

    items = []
    for need in pagination.items:
        item_dict = need.to_dict()

        # 添加发布者信息
        item_dict['publisher'] = {
            'id': need.publisher.id,
            'username': need.publisher.username,
            'phone': need.publisher.phone
        }

        # 检查当前用户是否已响应
        my_response = ServiceResponse.query.filter_by(
            need_id=need.id,
            responder_id=user.id
        ).first()

        item_dict['hasResponded'] = my_response is not None
        if my_response:
            item_dict['myResponseStatus'] = my_response.status

        items.append(item_dict)

    return success_response('获取成功', {
        'total': pagination.total,
        'page': page,
        'pageSize': page_size,
        'items': items
    })


@needs_bp.route('/<need_id>', methods=['GET'])
@login_required
def get_need(need_id):
    """查看需求详情（用于响应者）"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    data = need.to_dict(include_details=True)

    # 添加发布者信息
    data['publisher'] = {
        'id': need.publisher.id,
        'username': need.publisher.username,
        'bio': need.publisher.bio,
        'phone': need.publisher.phone,
        'address': need.publisher.address
    }

    # 检查当前用户是否已响应
    my_response = ServiceResponse.query.filter_by(
        need_id=need_id,
        responder_id=user.id
    ).first()

    data['hasResponded'] = my_response is not None
    if my_response:
        data['myResponse'] = {
            'id': my_response.id,
            'description': my_response.description,
            'status': my_response.status,
            'createdAt': my_response.created_at.isoformat() if my_response.created_at else None
        }

    return success_response('获取成功', data)


@needs_bp.route('/<need_id>/respond', methods=['POST'])
@login_required
def respond_to_need(need_id):
    """响应需求"""
    user = g.current_user

    need = Need.query.get(need_id)
    if not need:
        return error_response('需求不存在', 'NEED_NOT_FOUND', 404)

    # 不能响应自己的需求
    if need.publisher_id == user.id:
        return error_response('不能响应自己发布的需求', 'CANNOT_RESPOND_OWN_NEED')

    # 检查是否已响应
    existing_response = ServiceResponse.query.filter_by(
        need_id=need_id,
        responder_id=user.id
    ).first()

    if existing_response:
        return error_response('已经响应过此需求', 'ALREADY_RESPONDED')

    # 只能响应open或responded状态的需求
    if need.status not in ['open', 'responded']:
        return error_response('需求状态不允许响应', 'INVALID_NEED_STATUS')

    # 获取表单数据
    description = request.form.get('description', '').strip()
    price = request.form.get('price')
    available_time = request.form.get('availableTime')

    # 验证描述
    if not description or len(description) < 10 or len(description) > 500:
        return error_response('描述长度必须在10-500个字符之间', 'INVALID_DESCRIPTION')

    try:
        # 创建响应
        response = ServiceResponse(
            need_id=need_id,
            responder_id=user.id,
            description=description,
            price=price,
            available_time=available_time
        )

        # 先提交response以获取ID
        db.session.add(response)
        db.session.flush()  # 生成ID但不提交事务

        # 处理图片上传
        image_urls = []
        images = request.files.getlist('images')
        if images and len(images) > current_app.config['MAX_IMAGES_PER_UPLOAD']:
            return error_response(
                f'图片数量超过限制（最多{current_app.config["MAX_IMAGES_PER_UPLOAD"]}张）',
                'TOO_MANY_FILES'
            )

        for image in images:
            if image and image.filename:
                try:
                    url = save_file(image, f'responses/{response.id}/images', 'image')
                    image_urls.append(url)
                except ValueError as e:
                    return error_response(str(e), 'INVALID_FILE_TYPE')

        response.set_images(image_urls)

        # 处理视频上传
        video_urls = []
        videos = request.files.getlist('videos')
        if videos and len(videos) > current_app.config['MAX_VIDEOS_PER_UPLOAD']:
            delete_files(image_urls)
            return error_response(
                f'视频数量超过限制（最多{current_app.config["MAX_VIDEOS_PER_UPLOAD"]}个）',
                'TOO_MANY_FILES'
            )

        for video in videos:
            if video and video.filename:
                try:
                    url = save_file(video, f'responses/{response.id}/videos', 'video')
                    video_urls.append(url)
                except ValueError as e:
                    delete_files(image_urls)
                    return error_response(str(e), 'INVALID_FILE_TYPE')

        response.set_videos(video_urls)

        # 更新需求状态为responded（如果还是open）
        if need.status == 'open':
            need.status = 'responded'

        # 提交事务
        db.session.commit()

        # 更新月统计 - 响应数
        try:
            month = datetime.now().strftime('%Y%m')
            if need.region:
                province = need.region.province
                city = need.region.city
            else:
                province = '未知省份'
                city = '未知城市'

            stat = MonthlyStatistics.query.filter_by(
                month=month,
                province=province,
                city=city,
                service_type_id=need.service_type_id
            ).first()

            if stat:
                stat.responses_received += 1
                db.session.commit()
        except:
            pass

        return created_response('响应提交成功', {
            'responseId': response.id,
            'imageUrls': image_urls,
            'videoUrls': video_urls
        })

    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f'Respond to need error: {e}')
        return error_response('响应提交失败', 'SERVER_ERROR', 500)
