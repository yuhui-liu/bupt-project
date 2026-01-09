"""
公共API（服务类型等）
"""
from flask import Blueprint
from app.models import ServiceType, Region
from app.utils.responses import success_response

common_bp = Blueprint('common', __name__)


@common_bp.route('/service-types', methods=['GET'])
def get_service_types():
    """获取服务类型列表"""
    service_types = ServiceType.query.filter_by(is_active=True).order_by(ServiceType.order).all()

    data = [st.to_dict() for st in service_types]

    return success_response('获取成功', data)


@common_bp.route('/regions', methods=['GET'])
def get_regions():
    """获取地域列表"""
    regions = Region.query.filter_by(is_active=True).order_by(Region.province, Region.city).all()

    data = [r.to_dict() for r in regions]

    return success_response('获取成功', data)
