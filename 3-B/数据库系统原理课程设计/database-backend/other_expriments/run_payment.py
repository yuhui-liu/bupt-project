import psycopg2
from payment import payment


def main():
    # 数据库连接配置
    db_config = {
        "dbname": "tpccdb",
        "user": "tpcc",
        "password": "tpcc_password",
        "host": "49.235.139.218",
        "port": 5432,
    }

    try:
        # 创建数据库连接
        connection = psycopg2.connect(**db_config)
        print("Database connection established.")

        # 调用 payment 函数
        # 示例参数设置
        w_id = 1  # 仓库 ID
        d_id = 1  # 地区 ID
        c_id = 123  # 客户 ID
        c_last = "Smith"  # 客户姓氏（仅在 `byname=True` 时使用）
        h_amount = 100.0  # 支付金额
        byname = False  # 是否通过客户姓氏查询（False 表示通过客户 ID 查询）

        result = payment(
            connection,
            w_id,
            d_id,
            c_id=c_id,
            c_last=c_last,
            h_amount=h_amount,
            byname=byname,
        )

        if result == 0:
            print("Payment transaction completed successfully.")
        else:
            print("Payment transaction failed.")

    except Exception as e:
        print(f"Error occurred: {e}")
    finally:
        # 关闭数据库连接
        if connection:
            connection.close()
            print("Database connection closed.")


if __name__ == "__main__":
    main()
