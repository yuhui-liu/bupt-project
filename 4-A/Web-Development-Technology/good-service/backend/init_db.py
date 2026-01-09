"""
数据库初始化脚本
"""
import os
from dotenv import load_dotenv

# 加载环境变量
load_dotenv()

from app import create_app, db
from app.models import User, Region, ServiceType, Need, ServiceResponse, AcceptedService, MonthlyStatistics
from app.auth.utils import hash_password
from datetime import datetime, timedelta, date
import random

def init_database():
    """初始化数据库"""
    app = create_app()

    with app.app_context():
        print("正在创建数据库表...")
        db.create_all()
        print("数据库表创建成功！")

        # 检查是否已有数据
        if User.query.first():
            print("数据库已包含数据，跳过初始化。")
            return

        print("\n正在插入初始数据...")

        # 1. 创建管理员账户
        print("创建管理员账户...")
        admin = User(
            username='admin',
            password_hash=hash_password('admin123'),
            role='admin',
            phone='13900139000',
            email='admin@goodservice.com',
            bio='系统管理员'
        )
        db.session.add(admin)

        # 2. 创建测试用户
        print("创建测试用户...")
        user1 = User(
            username='user1',
            password_hash=hash_password('123456'),
            role='user',
            phone='13800138001',
            email='user1@example.com',
            address='北京市朝阳区',
            bio='热心的社区服务提供者'
        )
        db.session.add(user1)

        user2 = User(
            username='user2',
            password_hash=hash_password('123456'),
            role='user',
            phone='13800138002',
            email='user2@example.com',
            address='上海市浦东新区',
            bio='需要帮助的居民'
        )
        db.session.add(user2)

        user3 = User(
            username='user3',
            password_hash=hash_password('123456'),
            role='user',
            phone='13800138003',
            email='user3@example.com',
            address='广州市天河区',
            bio='专业服务人员'
        )
        db.session.add(user3)

        db.session.commit()

        # 3. 创建服务类型
        print("创建服务类型...")
        service_types = [
            ServiceType(code='PLUMBING', name='管道维修', description='水管、下水道等管道维修服务', order=1),
            ServiceType(code='ELDERLY_CARE', name='助老服务', description='为老年人提供生活照料和陪伴', order=2),
            ServiceType(code='CLEANING', name='保洁服务', description='家庭或办公场所清洁服务', order=3),
            ServiceType(code='MEDICAL', name='就诊服务', description='陪同就医、取药等医疗相关服务', order=4),
            ServiceType(code='TEACHING', name='替教服务', description='临时代课、辅导等教学服务', order=5),
            ServiceType(code='TRANSPORT', name='定期接送服务', description='定时接送、代驾等交通服务', order=6),
            ServiceType(code='OTHER', name='其他', description='其他类型服务', order=7),
        ]

        for st in service_types:
            db.session.add(st)

        db.session.commit()

        # 4. 创建地域信息
        print("创建地域信息...")
        regions = [
            Region(code='BJ001', name='朝阳区', city='北京市', province='北京'),
            Region(code='BJ002', name='海淀区', city='北京市', province='北京'),
            Region(code='SH001', name='浦东新区', city='上海市', province='上海'),
            Region(code='SH002', name='徐汇区', city='上海市', province='上海'),
            Region(code='GD001', name='天河区', city='广州市', province='广东'),
            Region(code='GD002', name='福田区', city='深圳市', province='广东'),
        ]

        for region in regions:
            db.session.add(region)

        db.session.commit()

        # 获取已创建的数据用于关联
        plumbing_type = ServiceType.query.filter_by(code='PLUMBING').first()
        elderly_care_type = ServiceType.query.filter_by(code='ELDERLY_CARE').first()
        cleaning_type = ServiceType.query.filter_by(code='CLEANING').first()
        medical_type = ServiceType.query.filter_by(code='MEDICAL').first()

        beijing_region = Region.query.filter_by(code='BJ001').first()
        shanghai_region = Region.query.filter_by(code='SH001').first()
        guangzhou_region = Region.query.filter_by(code='GD001').first()

        # 5. 创建测试需求
        print("创建测试需求...")

        # 需求1: user2发布的管道维修需求（已完成）
        need1 = Need(
            publisher_id=user2.id,
            region_id=shanghai_region.id,
            service_type_id=plumbing_type.id,
            title='厨房水管漏水需要维修',
            description='厨房水槽下方的水管接口处漏水，需要专业人员检查维修。希望能尽快处理，影响正常使用。',
            status='completed',
            location='浦东新区XX小区3号楼',
            budget='100-200元',
            created_at=datetime.now() - timedelta(days=10),
            rating=5,
            comment='服务很专业，态度好，价格合理！'
        )
        db.session.add(need1)
        db.session.commit()

        # 需求1的响应（user1响应并被接受）
        response1 = ServiceResponse(
            need_id=need1.id,
            responder_id=user1.id,
            description='我有5年管道维修经验，可以快速定位问题并修复。提供1年质保服务。',
            price='150元',
            available_time='今天下午或明天上午都可以',
            status='accepted',
            created_at=datetime.now() - timedelta(days=9)
        )
        db.session.add(response1)
        db.session.commit()

        # 创建响应成功明细
        accepted1 = AcceptedService(
            need_id=need1.id,
            need_publisher_id=user2.id,
            response_id=response1.id,
            responder_id=user1.id,
            accepted_date=date.today() - timedelta(days=8),
            completed_date=date.today() - timedelta(days=7),
            rating=5,
            comment='服务很专业，态度好，价格合理！'
        )
        db.session.add(accepted1)

        # 需求2: user2发布的助老服务需求（已接受）
        need2 = Need(
            publisher_id=user2.id,
            region_id=shanghai_region.id,
            service_type_id=elderly_care_type.id,
            title='需要有人陪同老人定期检查',
            description='家中老人需要每周去医院做检查，子女工作忙，希望有热心人士能够陪同。',
            status='accepted',
            location='浦东新区',
            budget='50元/次',
            created_at=datetime.now() - timedelta(days=5)
        )
        db.session.add(need2)
        db.session.commit()

        # 需求2的响应
        response2 = ServiceResponse(
            need_id=need2.id,
            responder_id=user3.id,
            description='我可以帮忙陪同老人就医，有护理经验，细心耐心。',
            price='50元/次',
            available_time='每周三、周五有空',
            status='accepted',
            created_at=datetime.now() - timedelta(days=4)
        )
        db.session.add(response2)
        db.session.commit()

        accepted2 = AcceptedService(
            need_id=need2.id,
            need_publisher_id=user2.id,
            response_id=response2.id,
            responder_id=user3.id,
            accepted_date=date.today() - timedelta(days=3)
        )
        db.session.add(accepted2)

        # 需求3: user1发布的保洁服务需求（已响应）
        need3 = Need(
            publisher_id=user1.id,
            region_id=beijing_region.id,
            service_type_id=cleaning_type.id,
            title='新房开荒保洁',
            description='刚装修完的新房，需要做开荒保洁，面积约90平米。',
            status='responded',
            location='朝阳区XX小区',
            budget='500-800元',
            created_at=datetime.now() - timedelta(days=3)
        )
        db.session.add(need3)
        db.session.commit()

        # 需求3的响应
        response3_1 = ServiceResponse(
            need_id=need3.id,
            responder_id=user2.id,
            description='专业保洁团队，经验丰富，设备齐全。',
            price='600元',
            available_time='本周末可以',
            status='pending',
            created_at=datetime.now() - timedelta(days=2)
        )
        db.session.add(response3_1)

        response3_2 = ServiceResponse(
            need_id=need3.id,
            responder_id=user3.id,
            description='提供开荒保洁服务，包含全屋清洁、玻璃清洁等。',
            price='550元',
            available_time='随时可以上门',
            status='pending',
            created_at=datetime.now() - timedelta(days=1)
        )
        db.session.add(response3_2)

        # 需求4: user3发布的待响应需求
        need4 = Need(
            publisher_id=user3.id,
            region_id=guangzhou_region.id,
            service_type_id=medical_type.id,
            title='需要代买药品',
            description='腿脚不便，需要人帮忙去医院代买常用药品。',
            status='open',
            location='天河区',
            budget='10元跑腿费',
            created_at=datetime.now() - timedelta(days=1)
        )
        db.session.add(need4)

        # 需求5: 已取消的需求
        need5 = Need(
            publisher_id=user1.id,
            region_id=beijing_region.id,
            service_type_id=plumbing_type.id,
            title='卫生间马桶堵塞',
            description='卫生间马桶堵了，需要疏通。',
            status='cancelled',
            location='朝阳区',
            budget='100元',
            cancel_reason='已自行解决',
            created_at=datetime.now() - timedelta(days=6)
        )
        db.session.add(need5)

        db.session.commit()

        # 6. 创建月统计数据
        print("创建月统计数据...")

        current_month = datetime.now().strftime('%Y%m')
        last_month = (datetime.now() - timedelta(days=30)).strftime('%Y%m')

        # 上个月的统计数据
        stats_last_month = [
            MonthlyStatistics(
                month=last_month,
                province='北京',
                city='北京市',
                service_type_id=plumbing_type.id,
                needs_published=8,
                responses_received=15,
                needs_accepted=6,
                needs_completed=5
            ),
            MonthlyStatistics(
                month=last_month,
                province='上海',
                city='上海市',
                service_type_id=elderly_care_type.id,
                needs_published=12,
                responses_received=20,
                needs_accepted=10,
                needs_completed=8
            ),
            MonthlyStatistics(
                month=last_month,
                province='广东',
                city='广州市',
                service_type_id=cleaning_type.id,
                needs_published=15,
                responses_received=30,
                needs_accepted=12,
                needs_completed=10
            ),
        ]

        for stat in stats_last_month:
            db.session.add(stat)

        # 当月的统计数据
        stats_current_month = [
            MonthlyStatistics(
                month=current_month,
                province='北京',
                city='北京市',
                service_type_id=plumbing_type.id,
                needs_published=3,
                responses_received=5,
                needs_accepted=1,
                needs_completed=0
            ),
            MonthlyStatistics(
                month=current_month,
                province='上海',
                city='上海市',
                service_type_id=plumbing_type.id,
                needs_published=2,
                responses_received=3,
                needs_accepted=1,
                needs_completed=1
            ),
            MonthlyStatistics(
                month=current_month,
                province='上海',
                city='上海市',
                service_type_id=elderly_care_type.id,
                needs_published=1,
                responses_received=1,
                needs_accepted=1,
                needs_completed=0
            ),
            MonthlyStatistics(
                month=current_month,
                province='北京',
                city='北京市',
                service_type_id=cleaning_type.id,
                needs_published=1,
                responses_received=2,
                needs_accepted=0,
                needs_completed=0
            ),
            MonthlyStatistics(
                month=current_month,
                province='广东',
                city='广州市',
                service_type_id=medical_type.id,
                needs_published=1,
                responses_received=0,
                needs_accepted=0,
                needs_completed=0
            ),
        ]

        for stat in stats_current_month:
            db.session.add(stat)

        db.session.commit()

        print("\n数据库初始化完成！")
        print("\n创建的账户信息：")
        print("=" * 50)
        print("管理员账户：")
        print("  用户名: admin")
        print("  密码: admin")
        print("\n测试用户账户：")
        print("  用户名: user1, 密码: 123456")
        print("  用户名: user2, 密码: 123456")
        print("  用户名: user3, 密码: 123456")
        print("=" * 50)
        print("\n数据统计：")
        print(f"  用户数: {User.query.count()}")
        print(f"  服务类型: {ServiceType.query.count()}")
        print(f"  地域: {Region.query.count()}")
        print(f"  需求: {Need.query.count()}")
        print(f"  响应: {ServiceResponse.query.count()}")
        print(f"  成功明细: {AcceptedService.query.count()}")
        print(f"  月统计记录: {MonthlyStatistics.query.count()}")
        print("\n初始化完成！可以启动服务器了。")


if __name__ == '__main__':
    init_database()
