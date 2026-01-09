"""
管理员功能API
"""
from flask import Blueprint, request, current_app, g, make_response
from app import db
from app.models import User, Need, ServiceResponse, MonthlyStatistics, ServiceType
from app.utils.responses import success_response, error_response
from app.utils.decorators import admin_required
from sqlalchemy import or_, func
from datetime import datetime
import csv
from io import StringIO

admin_bp = Blueprint('admin', __name__)


@admin_bp.route('/users', methods=['GET'])
@admin_required
def get_users():
    """获取用户列表"""
    # 分页参数
    page = request.args.get('page', 1, type=int)
    page_size = request.args.get('pageSize', 20, type=int)
    page_size = min(page_size, current_app.config['MAX_PAGE_SIZE'])

    # 筛选参数
    keyword = request.args.get('keyword', '').strip()
    role = request.args.get('role')

    # 构建查询
    query = User.query

    if keyword:
        query = query.filter(
            or_(
                User.username.like(f'%{keyword}%'),
                User.phone.like(f'%{keyword}%')
            )
        )

    if role:
        query = query.filter_by(role=role)

    # 按创建时间倒序
    query = query.order_by(User.created_at.desc())

    # 分页
    pagination = query.paginate(page=page, per_page=page_size, error_out=False)

    items = []
    for user in pagination.items:
        # 统计数据
        needs_count = Need.query.filter_by(publisher_id=user.id).count()
        responses_count = ServiceResponse.query.filter_by(responder_id=user.id).count()
        accepted_count = ServiceResponse.query.filter_by(
            responder_id=user.id,
            status='accepted'
        ).count()
        completed_count = Need.query.filter_by(
            publisher_id=user.id,
            status='completed'
        ).count()

        user_dict = user.to_dict()
        user_dict['stats'] = {
            'needsCount': needs_count,
            'responsesCount': responses_count,
            'acceptedCount': accepted_count,
            'completedCount': completed_count
        }

        items.append(user_dict)

    return success_response('获取成功', {
        'total': pagination.total,
        'page': page,
        'pageSize': page_size,
        'items': items
    })


@admin_bp.route('/needs', methods=['GET'])
@admin_required
def get_all_needs():
    """获取所有需求"""
    # 分页参数
    page = request.args.get('page', 1, type=int)
    page_size = request.args.get('pageSize', 20, type=int)
    page_size = min(page_size, current_app.config['MAX_PAGE_SIZE'])

    # 筛选参数
    status = request.args.get('status')
    service_type = request.args.get('serviceType')
    keyword = request.args.get('keyword', '').strip()

    # 构建查询
    query = Need.query

    if status:
        query = query.filter_by(status=status)

    if service_type:
        query = query.filter_by(service_type_id=service_type)

    if keyword:
        # 搜索标题或发布者用户名
        query = query.join(User, Need.publisher_id == User.id).filter(
            or_(
                Need.title.like(f'%{keyword}%'),
                User.username.like(f'%{keyword}%')
            )
        )

    # 按创建时间倒序
    query = query.order_by(Need.created_at.desc())

    # 分页
    pagination = query.paginate(page=page, per_page=page_size, error_out=False)

    items = []
    for need in pagination.items:
        item_dict = need.to_dict()
        item_dict['publisher'] = {
            'id': need.publisher.id,
            'username': need.publisher.username,
            'phone': need.publisher.phone
        }
        items.append(item_dict)

    return success_response('获取成功', {
        'total': pagination.total,
        'page': page,
        'pageSize': page_size,
        'items': items
    })


@admin_bp.route('/responses', methods=['GET'])
@admin_required
def get_all_responses():
    """获取所有服务响应"""
    # 分页参数
    page = request.args.get('page', 1, type=int)
    page_size = request.args.get('pageSize', 20, type=int)
    page_size = min(page_size, current_app.config['MAX_PAGE_SIZE'])

    # 筛选参数
    status = request.args.get('status')
    keyword = request.args.get('keyword', '').strip()

    # 构建查询
    query = ServiceResponse.query

    if status:
        query = query.filter_by(status=status)

    if keyword:
        # 搜索响应者用户名或需求标题
        query = query.join(User, ServiceResponse.responder_id == User.id).join(
            Need, ServiceResponse.need_id == Need.id
        ).filter(
            or_(
                User.username.like(f'%{keyword}%'),
                Need.title.like(f'%{keyword}%')
            )
        )

    # 按创建时间倒序
    query = query.order_by(ServiceResponse.created_at.desc())

    # 分页
    pagination = query.paginate(page=page, per_page=page_size, error_out=False)

    items = []
    for response in pagination.items:
        item_dict = response.to_dict()
        item_dict['responder'] = {
            'id': response.responder.id,
            'username': response.responder.username,
            'phone': response.responder.phone
        }
        item_dict['need'] = {
            'id': response.need.id,
            'title': response.need.title,
            'serviceType': response.need.service_type_id,
            'serviceTypeName': response.need.service_type.name if response.need.service_type else '未知',
            'publisher': {
                'id': response.need.publisher.id,
                'username': response.need.publisher.username
            }
        }
        items.append(item_dict)

    return success_response('获取成功', {
        'total': pagination.total,
        'page': page,
        'pageSize': page_size,
        'items': items
    })


@admin_bp.route('/statistics', methods=['GET'])
@admin_required
def get_statistics():
    """获取统计数据"""
    # 查询参数
    start_month = request.args.get('startMonth')  # YYYY-MM
    end_month = request.args.get('endMonth')  # YYYY-MM
    region = request.args.get('region')  # 省份或城市
    service_type_id = request.args.get('serviceType')

    if not start_month or not end_month:
        return error_response('起始月份和终止月份不能为空', 'INVALID_REQUEST')

    try:
        # 转换为YYYYMM格式
        start = start_month.replace('-', '')
        end = end_month.replace('-', '')

        if len(start) != 6 or len(end) != 6:
            return error_response('月份格式错误，应为YYYY-MM', 'INVALID_DATE_RANGE')

        if start > end:
            return error_response('起始月份不能晚于终止月份', 'INVALID_DATE_RANGE')

    except:
        return error_response('月份格式错误', 'INVALID_DATE_RANGE')

    # 构建查询
    query = MonthlyStatistics.query.filter(
        MonthlyStatistics.month >= start,
        MonthlyStatistics.month <= end
    )

    if region:
        query = query.filter(
            or_(
                MonthlyStatistics.province.like(f'%{region}%'),
                MonthlyStatistics.city.like(f'%{region}%')
            )
        )

    if service_type_id:
        query = query.filter_by(service_type_id=service_type_id)

    # 按月份和服务类型排序
    query = query.order_by(MonthlyStatistics.month, MonthlyStatistics.service_type_id)

    # 获取统计数据
    statistics = query.all()
    stats_data = [stat.to_dict() for stat in statistics]

    # 计算汇总数据
    total_needs = sum(s.needs_published for s in statistics)
    total_responses = sum(s.responses_received for s in statistics)
    total_accepted = sum(s.needs_accepted for s in statistics)
    total_completed = sum(s.needs_completed for s in statistics)
    overall_completion_rate = (
        round((total_completed / total_accepted) * 100, 2) if total_accepted > 0 else 0
    )

    # 计算活跃用户数（在此期间发布过需求或响应的用户）
    # 这里简化实现，实际应该查询具体的需求和响应记录
    active_users = User.query.filter_by(status='active').count()

    # 新用户数（在此期间注册的）
    try:
        start_date = datetime.strptime(start_month, '%Y-%m')
        end_date = datetime.strptime(end_month, '%Y-%m')
        end_date = end_date.replace(day=28)  # 简化处理，取月底
        new_users = User.query.filter(
            User.created_at >= start_date,
            User.created_at <= end_date
        ).count()
    except:
        new_users = 0

    summary = {
        'totalNeeds': total_needs,
        'totalResponses': total_responses,
        'totalAccepted': total_accepted,
        'totalCompleted': total_completed,
        'overallCompletionRate': overall_completion_rate,
        'activeUsers': active_users,
        'newUsers': new_users
    }

    # 生成图表数据

    # 需求趋势（按月）
    needs_trend = {}
    for stat in statistics:
        month = f"{stat.month[:4]}-{stat.month[4:]}"
        needs_trend[month] = needs_trend.get(month, 0) + stat.needs_published

    needs_trend_data = [
        {'month': month, 'count': count}
        for month, count in sorted(needs_trend.items())
    ]

    # 服务类型分布
    service_type_dist = {}
    for stat in statistics:
        type_name = stat.service_type.name if stat.service_type else '未知'
        service_type_dist[type_name] = service_type_dist.get(type_name, 0) + stat.needs_published

    total_for_percentage = sum(service_type_dist.values())
    service_type_dist_data = [
        {
            'serviceType': service_type,
            'count': count,
            'percentage': round((count / total_for_percentage) * 100, 2) if total_for_percentage > 0 else 0
        }
        for service_type, count in service_type_dist.items()
    ]

    # 完成率趋势（按月）
    completion_rate_trend = {}
    month_accepted = {}
    month_completed = {}

    for stat in statistics:
        month = f"{stat.month[:4]}-{stat.month[4:]}"
        month_accepted[month] = month_accepted.get(month, 0) + stat.needs_accepted
        month_completed[month] = month_completed.get(month, 0) + stat.needs_completed

    for month in month_accepted:
        if month_accepted[month] > 0:
            rate = round((month_completed[month] / month_accepted[month]) * 100, 2)
        else:
            rate = 0
        completion_rate_trend[month] = rate

    completion_rate_trend_data = [
        {'month': month, 'rate': rate}
        for month, rate in sorted(completion_rate_trend.items())
    ]

    charts = {
        'needsTrend': needs_trend_data,
        'serviceTypeDistribution': service_type_dist_data,
        'completionRateTrend': completion_rate_trend_data
    }

    return success_response('获取成功', {
        'statistics': stats_data,
        'summary': summary,
        'charts': charts
    })


@admin_bp.route('/statistics/export', methods=['GET'])
@admin_required
def export_statistics():
    """导出统计数据"""
    # 查询参数（与get_statistics相同）
    start_month = request.args.get('startMonth')
    end_month = request.args.get('endMonth')
    region = request.args.get('region')
    service_type_id = request.args.get('serviceType')
    export_format = request.args.get('format', 'csv')

    if not start_month or not end_month:
        return error_response('起始月份和终止月份不能为空', 'INVALID_REQUEST')

    if export_format not in ['csv', 'excel']:
        return error_response('导出格式不支持，仅支持csv和excel', 'INVALID_FORMAT')

    try:
        start = start_month.replace('-', '')
        end = end_month.replace('-', '')
    except:
        return error_response('月份格式错误', 'INVALID_DATE_RANGE')

    # 构建查询
    query = MonthlyStatistics.query.filter(
        MonthlyStatistics.month >= start,
        MonthlyStatistics.month <= end
    )

    if region:
        query = query.filter(
            or_(
                MonthlyStatistics.province.like(f'%{region}%'),
                MonthlyStatistics.city.like(f'%{region}%')
            )
        )

    if service_type_id:
        query = query.filter_by(service_type_id=service_type_id)

    query = query.order_by(MonthlyStatistics.month)

    statistics = query.all()

    # 生成CSV
    if export_format == 'csv':
        si = StringIO()
        writer = csv.writer(si)

        # 写入表头
        writer.writerow([
            '月份', '省份', '城市', '服务类型',
            '发布需求数', '收到响应数', '接受需求数', '完成需求数', '完成率(%)'
        ])

        # 写入数据
        for stat in statistics:
            completion_rate = 0
            if stat.needs_accepted > 0:
                completion_rate = round((stat.needs_completed / stat.needs_accepted) * 100, 2)

            writer.writerow([
                f"{stat.month[:4]}-{stat.month[4:]}",
                stat.province,
                stat.city,
                stat.service_type.name if stat.service_type else '',
                stat.needs_published,
                stat.responses_received,
                stat.needs_accepted,
                stat.needs_completed,
                completion_rate
            ])

        output = make_response(si.getvalue())
        output.headers["Content-Disposition"] = f"attachment; filename=statistics_{start_month}_to_{end_month}.csv"
        output.headers["Content-type"] = "text/csv; charset=utf-8"
        return output

    else:
        # Excel格式（需要额外的库，这里暂不实现）
        return error_response('Excel导出功能暂未实现', 'INVALID_FORMAT')
