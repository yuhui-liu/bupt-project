"""
数据模型测试
"""

import pytest
from datetime import datetime, date
from decimal import Decimal
from app.models import (
    User,
    UserToBePassed,
    Nation,
    Region,
    Part,
    Supplier,
    Customer,
    Orders,
    LineItem,
    PartSupp,
    FileUpload,
    FileUploadStatus,
    db,
)


class TestUserModels:
    """用户模型测试"""

    def test_user_creation(self, app):
        """测试用户创建"""
        with app.app_context():
            user = User(username="testuser", email="test@example.com", is_admin=False)
            user.set_password("testpassword")

            db.session.add(user)
            db.session.commit()

            assert user.id is not None
            assert user.username == "testuser"
            assert user.email == "test@example.com"
            assert user.is_admin is False
            assert user.check_password("testpassword")
            assert not user.check_password("wrongpassword")

    def test_user_unique_constraints(self, app):
        """测试用户唯一约束"""
        with app.app_context():
            user1 = User(username="unique_test", email="unique@test.com")
            user1.set_password("password")
            db.session.add(user1)
            db.session.commit()

            # 尝试创建相同用户名的用户
            user2 = User(username="unique_test", email="different@test.com")
            user2.set_password("password")
            db.session.add(user2)

            with pytest.raises(Exception):  # 应该抛出唯一约束异常
                db.session.commit()

    def test_user_to_be_passed_model(self, app):
        """测试待审核用户模型"""
        with app.app_context():
            pending_user = UserToBePassed(
                username="pending", email="pending@example.com"
            )
            pending_user.set_password("testpassword")

            db.session.add(pending_user)
            db.session.commit()

            assert pending_user.id is not None
            assert pending_user.username == "pending"
            assert pending_user.check_password("testpassword")
            assert isinstance(pending_user.apply_date, datetime)

    def test_password_hashing(self, app):
        """测试密码哈希功能"""
        with app.app_context():
            user = User(username="hash_test", email="hash@test.com")
            original_password = "mypassword123"
            user.set_password(original_password)

            # 密码应该被哈希
            assert user.password_hash != original_password
            assert len(user.password_hash) > 50  # 哈希后的密码应该比较长

            # 验证密码功能应该正常工作
            assert user.check_password(original_password)
            assert not user.check_password("wrongpassword")


class TestBusinessModels:
    """业务模型测试"""

    def test_nation_model(self, app):
        """测试国家模型"""
        with app.app_context():
            nation = Nation(
                n_nationkey=99,
                n_name="TEST_NATION",
                n_regionkey=1,
                n_comment="Test nation comment",
            )
            db.session.add(nation)
            db.session.commit()

            assert nation.n_nationkey == 99
            assert nation.n_name == "TEST_NATION"
            assert str(nation) == "<Nation TEST_NATION>"

    def test_region_model(self, app):
        """测试地区模型"""
        with app.app_context():
            region = Region(
                r_regionkey=99, r_name="TEST_REGION", r_comment="Test region comment"
            )
            db.session.add(region)
            db.session.commit()

            assert region.r_regionkey == 99
            assert region.r_name == "TEST_REGION"
            assert str(region) == "<Region TEST_REGION>"

    def test_part_model(self, app):
        """测试零件模型"""
        with app.app_context():
            part = Part(
                p_partkey=99,
                p_name="Test Part Name",
                p_mfgr="Test Manufacturer",
                p_brand="TestBrand",
                p_type="TEST_TYPE",
                p_size=25,
                p_container="BOX",
                p_retailprice=Decimal("99.99"),
                p_comment="Test part comment",
            )
            db.session.add(part)
            db.session.commit()

            assert part.p_partkey == 99
            assert part.p_name == "Test Part Name"
            assert part.p_retailprice == Decimal("99.99")
            assert str(part) == "<Part Test Part Name>"

    def test_supplier_model(self, app):
        """测试供应商模型"""
        with app.app_context():
            supplier = Supplier(
                s_suppkey=99,
                s_name="Test Supplier",
                s_address="123 Test Street",
                s_nationkey=1,
                s_phone="555-0123",
                s_acctbal=Decimal("10000.50"),
                s_comment="Test supplier comment",
            )
            db.session.add(supplier)
            db.session.commit()

            assert supplier.s_suppkey == 99
            assert supplier.s_name == "Test Supplier"
            assert supplier.s_acctbal == Decimal("10000.50")
            assert str(supplier) == "<Supplier Test Supplier>"

    def test_customer_model(self, app):
        """测试客户模型"""
        with app.app_context():
            customer = Customer(
                c_custkey=99,
                c_name="Test Customer",
                c_address="456 Customer Ave",
                c_nationkey=1,
                c_phone="555-0456",
                c_acctbal=Decimal("5000.25"),
                c_mktsegment="BUILDING",
                c_comment="Test customer comment",
            )
            db.session.add(customer)
            db.session.commit()

            assert customer.c_custkey == 99
            assert customer.c_name == "Test Customer"
            assert customer.c_acctbal == Decimal("5000.25")
            assert str(customer) == "<Customer Test Customer>"

    def test_orders_model(self, app):
        """测试订单模型"""
        with app.app_context():
            order = Orders(
                o_orderkey=99,
                o_custkey=1,
                o_orderstatus="O",
                o_totalprice=Decimal("1234.56"),
                o_orderdate=date(2023, 1, 15),
                o_orderpriority="1-URGENT",
                o_clerk="Clerk#000000001",
                o_shippriority=0,
                o_comment="Test order comment",
            )
            db.session.add(order)
            db.session.commit()

            assert order.o_orderkey == 99
            assert order.o_totalprice == Decimal("1234.56")
            assert order.o_orderdate == date(2023, 1, 15)
            assert str(order) == "<Orders 99>"

    def test_lineitem_model(self, app):
        """测试订单明细模型"""
        with app.app_context():
            lineitem = LineItem(
                l_orderkey=99,
                l_partkey=1,
                l_suppkey=1,
                l_linenumber=1,
                l_quantity=Decimal("10.00"),
                l_extendedprice=Decimal("1000.00"),
                l_discount=Decimal("0.05"),
                l_tax=Decimal("0.08"),
                l_returnflag="N",
                l_linestatus="O",
                l_shipdate=date(2023, 2, 1),
                l_commitdate=date(2023, 1, 20),
                l_receiptdate=date(2023, 2, 5),
                l_shipinstruct="DELIVER IN PERSON",
                l_shipmode="TRUCK",
                l_comment="Test lineitem comment",
            )
            db.session.add(lineitem)
            db.session.commit()

            assert lineitem.l_orderkey == 99
            assert lineitem.l_quantity == Decimal("10.00")
            assert lineitem.l_shipdate == date(2023, 2, 1)
            assert str(lineitem) == "<LineItem 99-1>"

    def test_partsupp_model(self, app):
        """测试零件供应商模型"""
        with app.app_context():
            partsupp = PartSupp(
                ps_partkey=99,
                ps_suppkey=99,
                ps_availqty=100,
                ps_supplycost=Decimal("50.00"),
                ps_comment="Test partsupp comment",
            )
            db.session.add(partsupp)
            db.session.commit()

            assert partsupp.ps_partkey == 99
            assert partsupp.ps_suppkey == 99
            assert partsupp.ps_supplycost == Decimal("50.00")
            assert str(partsupp) == "<PartSupp 99-99>"


class TestFileUploadModel:
    """文件上传模型测试"""

    def test_file_upload_creation(self, app):
        """测试文件上传记录创建"""
        with app.app_context():
            file_upload = FileUpload(
                id="test-uuid-123",
                filename="test.csv",
                table_name="customer",
                total_chunks=5,
                uploaded_chunks=3,
                upload_user="testuser",
                status=FileUploadStatus.UPLOADING,
            )
            db.session.add(file_upload)
            db.session.commit()

            assert file_upload.id == "test-uuid-123"
            assert file_upload.filename == "test.csv"
            assert file_upload.total_chunks == 5
            assert file_upload.uploaded_chunks == 3
            assert file_upload.status == FileUploadStatus.UPLOADING
            assert str(file_upload) == "<FileUpload test.csv>"

    def test_file_upload_status_enum(self, app):
        """测试文件上传状态枚举"""
        with app.app_context():
            statuses = [
                FileUploadStatus.UPLOADING,
                FileUploadStatus.UPLOADED,
                FileUploadStatus.PROCESSING,
                FileUploadStatus.COMPLETED,
                FileUploadStatus.FAILED,
            ]

            for status in statuses:
                file_upload = FileUpload(
                    id=f"test-{status.value}",
                    filename=f"test_{status.value}.csv",
                    table_name="customer",
                    total_chunks=1,
                    upload_user="testuser",
                    status=status,
                )
                db.session.add(file_upload)

            db.session.commit()

            # 验证所有状态都能正确保存
            for status in statuses:
                found = FileUpload.query.filter_by(id=f"test-{status.value}").first()
                assert found is not None
                assert found.status == status

    def test_file_upload_timestamps(self, app):
        """测试文件上传时间戳"""
        with app.app_context():
            file_upload = FileUpload(
                id="timestamp-test",
                filename="timestamp.csv",
                table_name="customer",
                total_chunks=1,
                upload_user="testuser",
            )
            db.session.add(file_upload)
            db.session.commit()

            # created_at应该自动设置
            assert file_upload.created_at is not None
            assert isinstance(file_upload.created_at, datetime)

            # completed_at应该为空（文件还未完成）
            assert file_upload.completed_at is None

            # 更新完成时间
            file_upload.completed_at = datetime.utcnow()
            file_upload.status = FileUploadStatus.COMPLETED
            db.session.commit()

            assert file_upload.completed_at is not None
