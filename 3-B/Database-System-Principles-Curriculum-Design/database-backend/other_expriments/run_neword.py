import psycopg2
from neword import neword

# PostgreSQL数据库连接参数
db_params = {
    "dbname": "tpccdb",
    "user": "tpcc",
    "password": "tpcc_password",
    "host": "49.235.139.218",
    "port": 5432,
}

# 创建数据库连接
connection = psycopg2.connect(**db_params)

# 调用 neword() 函数
w_id = 1
d_id = 1
c_id = 123
o_ol_cnt = 5
supware = [1, 1, 1, 1, 1]  # 仓库 ID 列表
itemid = [101, 102, 103, 104, 105]  # 商品 ID 列表
qty = [2, 3, 4, 1, 5]  # 购买数量列表

neword(connection, w_id, d_id, c_id, o_ol_cnt, supware, itemid, qty)

# 关闭连接
connection.close()
