from datetime import datetime
from werkzeug.security import generate_password_hash, check_password_hash
import enum

from flask_sqlalchemy import SQLAlchemy

db = SQLAlchemy()


class FileUploadStatus(enum.Enum):
    """文件上传状态枚举"""

    UPLOADING = "uploading"
    UPLOADED = "uploaded"
    PROCESSING = "processing"
    COMPLETED = "completed"
    FAILED = "failed"


class FileUpload(db.Model):
    """文件上传记录表"""

    __tablename__ = "file_uploads"

    id = db.Column(db.String(36), primary_key=True)  # UUID
    filename = db.Column(db.String(255), nullable=False)
    table_name = db.Column(db.String(50), nullable=False)
    total_chunks = db.Column(db.Integer, nullable=False)
    uploaded_chunks = db.Column(db.Integer, default=0)
    file_size = db.Column(db.BigInteger)
    status = db.Column(db.Enum(FileUploadStatus), default=FileUploadStatus.UPLOADING)
    upload_user = db.Column(db.String(80), nullable=False)
    created_at = db.Column(db.DateTime, default=datetime.utcnow)
    completed_at = db.Column(db.DateTime)
    error_message = db.Column(db.Text)

    def __repr__(self):
        return f"<FileUpload {self.filename}>"


class UserToBePassed(db.Model):
    """
    用户待审核表，主要用于存储待审核用户的信息。

    Attributes:
        id (int): 主键，自增的唯一标识符。
        email (str): 用户的电子邮件地址。
        username (str): 用户名。
        password_hash (str): 密码哈希值。
        apply_date (datetime): 申请日期。

    Methods:
        set_password(password): 设置密码
        check_password(password): 验证密码
        __repr__(): 返回用户的字符串表示
    """

    __tablename__ = "user_to_be_passed"
    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(120), unique=True, nullable=False)
    username = db.Column(db.String(80), unique=True, nullable=False)
    password_hash = db.Column(db.String(260), nullable=False)
    apply_date = db.Column(db.DateTime, nullable=False, default=datetime.utcnow())

    __table_args__ = (
        db.Index(
            "idx_user_to_be_passed_username", "username", unique=True
        ),  # 为用户名添加唯一索引
        db.Index(
            "idx_user_to_be_passed_email", "email", unique=True
        ),  # 为电子邮件添加唯一索引
    )

    def set_password(self, password):
        """设置密码"""
        self.password_hash = generate_password_hash(password)

    def check_password(self, password):
        """验证密码"""
        return check_password_hash(self.password_hash, password)

    def __repr__(self):
        return "<UserToBePassed %r>" % self.username


class User(db.Model):
    __tablename__ = "user"

    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(120), unique=True, nullable=False)
    username = db.Column(db.String(80), unique=True, nullable=False)
    password_hash = db.Column(db.String(260), nullable=False)
    is_admin = db.Column(db.Boolean, nullable=False, default=False)
    apply_date = db.Column(db.DateTime, nullable=True, default=datetime.utcnow())
    pass_date = db.Column(db.DateTime, nullable=True, default=datetime.utcnow())

    __table_args__ = (
        db.Index("idx_user_username", "username", unique=True),  # 为用户名添加唯一索引
        db.Index("idx_user_email", "email", unique=True),  # 为电子邮件添加唯一索引
    )

    def set_password(self, password):
        """设置密码"""
        self.password_hash = generate_password_hash(password)

    def check_password(self, password):
        """验证密码"""
        return check_password_hash(self.password_hash, password)

    def __repr__(self):
        return "<User %r>" % self.username


class Nation(db.Model):
    """国家表"""

    __tablename__ = "nation"

    n_nationkey = db.Column(db.Integer, primary_key=True, nullable=False)
    n_name = db.Column(db.String(25), nullable=False)
    n_regionkey = db.Column(db.Integer, nullable=False)
    n_comment = db.Column(db.String(152))

    __table_args__ = (db.Index("index_nation_name", "n_name"),)  # 为国家名称添加索引

    def __repr__(self):
        return f"<Nation {self.n_name}>"


class Region(db.Model):
    """地区表"""

    __tablename__ = "region"

    r_regionkey = db.Column(db.Integer, primary_key=True, nullable=False)
    r_name = db.Column(db.String(25), nullable=False)
    r_comment = db.Column(db.String(152))

    def __repr__(self):
        return f"<Region {self.r_name}>"


class Part(db.Model):
    """零件表"""

    __tablename__ = "part"

    p_partkey = db.Column(db.Integer, primary_key=True, nullable=False)
    p_name = db.Column(db.String(55), nullable=False)
    p_mfgr = db.Column(db.String(25), nullable=False)
    p_brand = db.Column(db.String(10), nullable=False)
    p_type = db.Column(db.String(25), nullable=False)
    p_size = db.Column(db.Integer, nullable=False)
    p_container = db.Column(db.String(10), nullable=False)
    p_retailprice = db.Column(db.Numeric(15, 2), nullable=False)
    p_comment = db.Column(db.String(23), nullable=False)

    def __repr__(self):
        return f"<Part {self.p_name}>"


class Supplier(db.Model):
    """供应商表"""

    __tablename__ = "supplier"

    s_suppkey = db.Column(db.Integer, primary_key=True, nullable=False)
    s_name = db.Column(db.String(25), nullable=False)
    s_address = db.Column(db.String(40), nullable=False)
    s_nationkey = db.Column(db.Integer, nullable=False)
    s_phone = db.Column(db.String(15), nullable=False)
    s_acctbal = db.Column(db.Numeric(15, 2), nullable=False)
    s_comment = db.Column(db.String(101), nullable=False)

    def __repr__(self):
        return f"<Supplier {self.s_name}>"


class PartSupp(db.Model):
    """零件供应商表"""

    __tablename__ = "partsupp"

    ps_partkey = db.Column(db.Integer, primary_key=True, nullable=False)
    ps_suppkey = db.Column(db.Integer, primary_key=True, nullable=False)
    ps_availqty = db.Column(db.Integer, nullable=False)
    ps_supplycost = db.Column(db.Numeric(15, 2), nullable=False)
    ps_comment = db.Column(db.String(199), nullable=False)

    def __repr__(self):
        return f"<PartSupp {self.ps_partkey}-{self.ps_suppkey}>"


class Customer(db.Model):
    """客户表"""

    __tablename__ = "customer"

    c_custkey = db.Column(db.Integer, primary_key=True, nullable=False)
    c_name = db.Column(db.String(25), nullable=False)
    c_address = db.Column(db.String(40), nullable=False)
    c_nationkey = db.Column(db.Integer, nullable=False)
    c_phone = db.Column(db.String(15), nullable=False)
    c_acctbal = db.Column(db.Numeric(15, 2), nullable=False)
    c_mktsegment = db.Column(db.String(10), nullable=False)
    c_comment = db.Column(db.String(117), nullable=False)

    __table_args__ = (db.Index("index_customer_name", "c_name"),)  # 为客户名称添加索引

    def __repr__(self):
        return f"<Customer {self.c_name}>"


class Orders(db.Model):
    """订单表"""

    __tablename__ = "orders"

    o_orderkey = db.Column(db.Integer, primary_key=True, nullable=False)
    o_custkey = db.Column(db.Integer, nullable=False)
    o_orderstatus = db.Column(db.String(1), nullable=False)
    o_totalprice = db.Column(db.Numeric(15, 2), nullable=False)
    o_orderdate = db.Column(db.Date, nullable=False)
    o_orderpriority = db.Column(db.String(15), nullable=False)
    o_clerk = db.Column(db.String(15), nullable=False)
    o_shippriority = db.Column(db.Integer, nullable=False)
    o_comment = db.Column(db.String(79), nullable=False)

    __table_args__ = (
        db.Index("index_orders_orderdate", "o_orderdate"),  # 为订单日期添加索引
    )

    def __repr__(self):
        return f"<Orders {self.o_orderkey}>"


class LineItem(db.Model):
    """订单明细表"""

    __tablename__ = "lineitem"

    l_orderkey = db.Column(db.Integer, primary_key=True, nullable=False)
    l_partkey = db.Column(db.Integer, nullable=False)
    l_suppkey = db.Column(db.Integer, nullable=False)
    l_linenumber = db.Column(db.Integer, primary_key=True, nullable=False)
    l_quantity = db.Column(db.Numeric(15, 2), nullable=False)
    l_extendedprice = db.Column(db.Numeric(15, 2), nullable=False)
    l_discount = db.Column(db.Numeric(15, 2), nullable=False)
    l_tax = db.Column(db.Numeric(15, 2), nullable=False)
    l_returnflag = db.Column(db.String(1), nullable=False)
    l_linestatus = db.Column(db.String(1), nullable=False)
    l_shipdate = db.Column(db.Date, nullable=False)
    l_commitdate = db.Column(db.Date, nullable=False)
    l_receiptdate = db.Column(db.Date, nullable=False)
    l_shipinstruct = db.Column(db.String(25), nullable=False)
    l_shipmode = db.Column(db.String(10), nullable=False)
    l_comment = db.Column(db.String(44), nullable=False)

    __table_args__ = (
        db.Index("index_lineitem_shipdate", "l_shipdate"),  # 为发货日期添加索引
        db.Index("index_lineitem_commitdate", "l_commitdate"),  # 为提交日期添加索引
        db.Index("index_lineitem_suppkey", "l_suppkey"),  # 为供应商键添加索引
        db.Index("index_lineitem_orderkey", "l_orderkey"),  # 为订单键添加索引
    )

    def __repr__(self):
        return f"<LineItem {self.l_orderkey}-{self.l_linenumber}>"
