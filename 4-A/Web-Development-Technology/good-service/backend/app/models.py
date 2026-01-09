"""
数据模型
"""
from datetime import datetime
from app import db
import uuid
import json


def generate_uuid():
    """生成UUID"""
    return str(uuid.uuid4())


class User(db.Model):
    """用户模型"""
    __tablename__ = 'user'

    id = db.Column(db.String(36), primary_key=True, default=generate_uuid)
    username = db.Column(db.String(50), unique=True, nullable=False, index=True)
    password_hash = db.Column(db.String(255), nullable=False)
    role = db.Column(db.String(20), nullable=False, default='user')  # 'user' or 'admin'
    phone = db.Column(db.String(11), unique=True, nullable=False, index=True)
    email = db.Column(db.String(100), unique=True, nullable=True)
    address = db.Column(db.String(200), nullable=True)
    bio = db.Column(db.String(500), nullable=True)
    status = db.Column(db.String(20), nullable=False, default='active')  # 'active' or 'disabled'
    created_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow, onupdate=datetime.utcnow)

    # 关系
    published_needs = db.relationship('Need', foreign_keys='Need.publisher_id', backref='publisher', lazy='dynamic')
    responses = db.relationship('ServiceResponse', foreign_keys='ServiceResponse.responder_id', backref='responder', lazy='dynamic')

    def to_dict(self, include_stats=False):
        """转换为字典"""
        data = {
            'id': self.id,
            'username': self.username,
            'role': self.role,
            'phone': self.phone,
            'email': self.email,
            'address': self.address,
            'bio': self.bio,
            'status': self.status,
            'createdAt': self.created_at.isoformat() if self.created_at else None,
            'updatedAt': self.updated_at.isoformat() if self.updated_at else None
        }

        if include_stats:
            needs_published = self.published_needs.count()
            needs_accepted = self.published_needs.filter_by(status='accepted').count() + \
                           self.published_needs.filter_by(status='completed').count()
            services_provided = self.responses.count()
            services_accepted = self.responses.filter_by(status='accepted').count()

            data['stats'] = {
                'needsPublished': needs_published,
                'needsAccepted': needs_accepted,
                'servicesProvided': services_provided,
                'servicesAccepted': services_accepted
            }

        return data


class Region(db.Model):
    """地域模型"""
    __tablename__ = 'region'

    id = db.Column(db.String(36), primary_key=True, default=generate_uuid)
    code = db.Column(db.String(20), unique=True, nullable=False)
    name = db.Column(db.String(100), nullable=False)
    city = db.Column(db.String(50), nullable=False)
    province = db.Column(db.String(50), nullable=False)
    is_active = db.Column(db.Boolean, nullable=False, default=True)
    created_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow, onupdate=datetime.utcnow)

    def to_dict(self):
        """转换为字典"""
        return {
            'id': self.id,
            'code': self.code,
            'name': self.name,
            'city': self.city,
            'province': self.province,
            'isActive': self.is_active
        }


class ServiceType(db.Model):
    """服务类型模型"""
    __tablename__ = 'service_type'

    id = db.Column(db.String(36), primary_key=True, default=generate_uuid)
    code = db.Column(db.String(20), unique=True, nullable=False)
    name = db.Column(db.String(100), nullable=False)
    description = db.Column(db.String(500), nullable=True)
    icon = db.Column(db.String(100), nullable=True)
    order = db.Column(db.Integer, nullable=False, default=0)
    is_active = db.Column(db.Boolean, nullable=False, default=True)
    created_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow, onupdate=datetime.utcnow)

    def to_dict(self):
        """转换为字典"""
        return {
            'id': self.id,
            'code': self.code,
            'name': self.name,
            'description': self.description,
            'icon': self.icon,
            'order': self.order
        }


class Need(db.Model):
    """需求模型"""
    __tablename__ = 'need'

    id = db.Column(db.String(36), primary_key=True, default=generate_uuid)
    publisher_id = db.Column(db.String(36), db.ForeignKey('user.id'), nullable=False, index=True)
    region_id = db.Column(db.String(36), db.ForeignKey('region.id'), nullable=True)
    service_type_id = db.Column(db.String(36), db.ForeignKey('service_type.id'), nullable=False, index=True)
    title = db.Column(db.String(100), nullable=False)
    description = db.Column(db.Text, nullable=False)
    status = db.Column(db.String(20), nullable=False, default='open', index=True)
    # 'open', 'responded', 'accepted', 'completed', 'cancelled'
    expected_time = db.Column(db.DateTime, nullable=True)
    location = db.Column(db.String(200), nullable=True)
    budget = db.Column(db.String(100), nullable=True)
    images = db.Column(db.Text, nullable=True)  # JSON array
    videos = db.Column(db.Text, nullable=True)  # JSON array
    cancel_reason = db.Column(db.String(500), nullable=True)
    rating = db.Column(db.Integer, nullable=True)  # 1-5
    comment = db.Column(db.String(1000), nullable=True)
    created_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow, onupdate=datetime.utcnow)

    # 关系
    region = db.relationship('Region', backref='needs')
    service_type = db.relationship('ServiceType', backref='needs')
    responses = db.relationship('ServiceResponse', backref='need', lazy='dynamic', cascade='all, delete-orphan')

    def get_images(self):
        """获取图片列表"""
        if self.images:
            try:
                return json.loads(self.images)
            except:
                return []
        return []

    def set_images(self, image_list):
        """设置图片列表"""
        self.images = json.dumps(image_list) if image_list else None

    def get_videos(self):
        """获取视频列表"""
        if self.videos:
            try:
                return json.loads(self.videos)
            except:
                return []
        return []

    def set_videos(self, video_list):
        """设置视频列表"""
        self.videos = json.dumps(video_list) if video_list else None

    def to_dict(self, include_details=False):
        """转换为字典"""
        data = {
            'id': self.id,
            'publisherId': self.publisher_id,
            'serviceType': self.service_type_id,
            'serviceTypeName': self.service_type.name if self.service_type else '',
            'title': self.title,
            'description': self.description,
            'status': self.status,
            'expectedTime': self.expected_time.isoformat() if self.expected_time else None,
            'location': self.location,
            'budget': self.budget,
            'images': self.get_images(),
            'createdAt': self.created_at.isoformat() if self.created_at else None,
            'updatedAt': self.updated_at.isoformat() if self.updated_at else None,
            'responseCount': self.responses.count()
        }

        if include_details:
            data['videos'] = self.get_videos()
            data['cancelReason'] = self.cancel_reason
            data['rating'] = self.rating
            data['comment'] = self.comment
            if self.publisher:
                data['publisher'] = {
                    'id': self.publisher.id,
                    'username': self.publisher.username,
                    'phone': self.publisher.phone,
                    'bio': self.publisher.bio,
                    'address': self.publisher.address
                }
            if self.region:
                data['region'] = self.region.to_dict()

        return data


class ServiceResponse(db.Model):
    """服务响应模型"""
    __tablename__ = 'service_response'

    id = db.Column(db.String(36), primary_key=True, default=generate_uuid)
    need_id = db.Column(db.String(36), db.ForeignKey('need.id'), nullable=False, index=True)
    responder_id = db.Column(db.String(36), db.ForeignKey('user.id'), nullable=False, index=True)
    description = db.Column(db.Text, nullable=False)
    price = db.Column(db.String(100), nullable=True)
    available_time = db.Column(db.String(200), nullable=True)
    images = db.Column(db.Text, nullable=True)  # JSON array
    videos = db.Column(db.Text, nullable=True)  # JSON array
    status = db.Column(db.String(20), nullable=False, default='pending', index=True)
    # 'pending', 'accepted', 'rejected', 'cancelled'
    created_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow, onupdate=datetime.utcnow)

    def get_images(self):
        """获取图片列表"""
        if self.images:
            try:
                return json.loads(self.images)
            except:
                return []
        return []

    def set_images(self, image_list):
        """设置图片列表"""
        self.images = json.dumps(image_list) if image_list else None

    def get_videos(self):
        """获取视频列表"""
        if self.videos:
            try:
                return json.loads(self.videos)
            except:
                return []
        return []

    def set_videos(self, video_list):
        """设置视频列表"""
        self.videos = json.dumps(video_list) if video_list else None

    def to_dict(self, include_details=False):
        """转换为字典"""
        data = {
            'id': self.id,
            'needId': self.need_id,
            'responderId': self.responder_id,
            'description': self.description,
            'price': self.price,
            'availableTime': self.available_time,
            'status': self.status,
            'images': self.get_images(),
            'createdAt': self.created_at.isoformat() if self.created_at else None,
            'updatedAt': self.updated_at.isoformat() if self.updated_at else None
        }

        if include_details:
            data['videos'] = self.get_videos()
            if self.responder:
                # 计算响应者的统计数据
                total_services = ServiceResponse.query.filter_by(responder_id=self.responder_id).count()
                accepted_services = ServiceResponse.query.filter_by(
                    responder_id=self.responder_id,
                    status='accepted'
                ).count()
                completion_rate = (accepted_services / total_services * 100) if total_services > 0 else 0

                data['responder'] = {
                    'id': self.responder.id,
                    'username': self.responder.username,
                    'bio': self.responder.bio,
                    'phone': self.responder.phone,
                    'stats': {
                        'totalServices': total_services,
                        'acceptedServices': accepted_services,
                        'completionRate': round(completion_rate, 2)
                    }
                }

        return data


class AcceptedService(db.Model):
    """响应成功明细模型"""
    __tablename__ = 'accepted_service'

    id = db.Column(db.String(36), primary_key=True, default=generate_uuid)
    need_id = db.Column(db.String(36), db.ForeignKey('need.id'), nullable=False)
    need_publisher_id = db.Column(db.String(36), db.ForeignKey('user.id'), nullable=False)
    response_id = db.Column(db.String(36), db.ForeignKey('service_response.id'), nullable=False)
    responder_id = db.Column(db.String(36), db.ForeignKey('user.id'), nullable=False)
    accepted_date = db.Column(db.Date, nullable=False)
    completed_date = db.Column(db.Date, nullable=True)
    rating = db.Column(db.Integer, nullable=True)  # 1-5
    comment = db.Column(db.String(1000), nullable=True)
    created_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow, onupdate=datetime.utcnow)

    # 关系
    need_obj = db.relationship('Need', foreign_keys=[need_id])
    publisher = db.relationship('User', foreign_keys=[need_publisher_id])
    response = db.relationship('ServiceResponse', foreign_keys=[response_id])
    responder_obj = db.relationship('User', foreign_keys=[responder_id])

    def to_dict(self):
        """转换为字典"""
        return {
            'id': self.id,
            'needId': self.need_id,
            'needPublisherId': self.need_publisher_id,
            'responseId': self.response_id,
            'responderId': self.responder_id,
            'acceptedDate': self.accepted_date.isoformat() if self.accepted_date else None,
            'completedDate': self.completed_date.isoformat() if self.completed_date else None,
            'rating': self.rating,
            'comment': self.comment
        }


class MonthlyStatistics(db.Model):
    """月汇总统计模型"""
    __tablename__ = 'monthly_statistics'
    __table_args__ = (
        db.UniqueConstraint('month', 'province', 'city', 'service_type_id', name='uq_month_region_service'),
    )

    id = db.Column(db.String(36), primary_key=True, default=generate_uuid)
    month = db.Column(db.String(6), nullable=False, index=True)  # YYYYMM
    province = db.Column(db.String(50), nullable=False)
    city = db.Column(db.String(50), nullable=False)
    service_type_id = db.Column(db.String(36), db.ForeignKey('service_type.id'), nullable=False)
    needs_published = db.Column(db.Integer, nullable=False, default=0)
    responses_received = db.Column(db.Integer, nullable=False, default=0)
    needs_accepted = db.Column(db.Integer, nullable=False, default=0)
    needs_completed = db.Column(db.Integer, nullable=False, default=0)
    created_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    updated_at = db.Column(db.DateTime, nullable=False, default=datetime.utcnow, onupdate=datetime.utcnow)

    # 关系
    service_type = db.relationship('ServiceType', backref='statistics')

    def to_dict(self):
        """转换为字典"""
        completion_rate = 0
        if self.needs_accepted > 0:
            completion_rate = round((self.needs_completed / self.needs_accepted) * 100, 2)

        avg_response_count = 0
        if self.needs_published > 0:
            avg_response_count = round(self.responses_received / self.needs_published, 2)

        return {
            'month': self.month,
            'serviceType': self.service_type_id,
            'serviceTypeName': self.service_type.name if self.service_type else '',
            'province': self.province,
            'city': self.city,
            'needsPublished': self.needs_published,
            'responsesReceived': self.responses_received,
            'needsAccepted': self.needs_accepted,
            'needsCompleted': self.needs_completed,
            'completionRate': completion_rate,
            'avgResponseCount': avg_response_count
        }
