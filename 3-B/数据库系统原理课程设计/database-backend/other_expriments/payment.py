from decimal import Decimal
import psycopg2
from datetime import datetime


def payment(connection, w_id, d_id, c_id=None, c_last=None, h_amount=0, byname=False):
    try:
        connection.autocommit = False
        cursor = connection.cursor()
        # 获取当前时间
        datetime_now = datetime.now()
        # 更新仓库累计金额
        cursor.execute(
            """
            UPDATE bmsql_warehouse
            SET w_ytd = w_ytd + %s
            WHERE w_id = %s
        """,
            (h_amount, w_id),
        )
        # 查询仓库地址信息
        cursor.execute(
            """
            SELECT w_street_1, w_street_2, w_city, w_state, w_zip, w_name
            FROM bmsql_warehouse
            WHERE w_id = %s
        """,
            (w_id,),
        )
        warehouse_data = cursor.fetchone()
        w_street_1, w_street_2, w_city, w_state, w_zip, w_name = warehouse_data
        # 更新地区累计金额
        cursor.execute(
            """
            UPDATE bmsql_district
            SET d_ytd = d_ytd + %s
            WHERE d_w_id = %s AND d_id = %s
        """,
            (h_amount, w_id, d_id),
        )
        # 查询地区地址信息
        cursor.execute(
            """
            SELECT d_street_1, d_street_2, d_city, d_state, d_zip, d_name
            FROM bmsql_district
            WHERE d_w_id = %s AND d_id = %s
        """,
            (w_id, d_id),
        )
        district_data = cursor.fetchone()
        d_street_1, d_street_2, d_city, d_state, d_zip, d_name = district_data
        # 查询客户信息
        if byname:
            # 根据客户姓氏查询
            cursor.execute(
                """
                SELECT COUNT(c_id)
                FROM bmsql_customer
                WHERE c_last = %s AND c_d_id = %s AND c_w_id = %s
            """,
                (c_last, d_id, w_id),
            )
            name_count = cursor.fetchone()[0]
            cursor.execute(
                """
                SELECT c_first, c_middle, c_id, c_street_1, c_street_2,
                       c_city, c_state, c_zip, c_phone, c_credit,
                       c_credit_lim, c_discount, c_balance, c_since
                FROM bmsql_customer
                WHERE c_w_id = %s AND c_d_id = %s AND c_last = %s
                ORDER BY c_first
            """,
                (w_id, d_id, c_last),
            )
            customers = cursor.fetchall()
            # 定位到中间客户
            midpoint = (name_count + 1) // 2
            customer_data = customers[midpoint - 1]
        else:
            # 根据客户 ID 查询
            cursor.execute(
                """
                SELECT c_first, c_middle, c_last, c_street_1, c_street_2,
                       c_city, c_state, c_zip, c_phone, c_credit,
                       c_credit_lim, c_discount, c_balance, c_since
                FROM bmsql_customer
                WHERE c_w_id = %s AND c_d_id = %s AND c_id = %s
            """,
                (w_id, d_id, c_id),
            )
            customer_data = cursor.fetchone()
        (
            c_first,
            c_middle,
            c_last,
            c_street_1,
            c_street_2,
            c_city,
            c_state,
            c_zip,
            c_phone,
            c_credit,
            c_credit_lim,
            c_discount,
            c_balance,
            c_since,
        ) = customer_data
        # 更新客户余额
        c_balance += Decimal(h_amount)
        if "BC" in c_credit:
            # 客户信用为坏信用
            cursor.execute(
                """
                SELECT c_data
                FROM bmsql_customer
                WHERE c_w_id = %s AND c_d_id = %s AND c_id = %s
            """,
                (w_id, d_id, c_id),
            )
            c_data = cursor.fetchone()[0]
            c_new_data = (
                f"| {c_id} {d_id} {w_id} {h_amount:.2f} {datetime_now} {c_data}"
            )
            cursor.execute(
                """
                UPDATE bmsql_customer
                SET c_balance = %s, c_data = %s
                WHERE c_w_id = %s AND c_d_id = %s AND c_id = %s
            """,
                (c_balance, c_new_data, w_id, d_id, c_id),
            )
        else:
            # 客户信用为好信用
            cursor.execute(
                """
                UPDATE bmsql_customer
                SET c_balance = %s
                WHERE c_w_id = %s AND c_d_id = %s AND c_id = %s
            """,
                (c_balance, w_id, d_id, c_id),
            )
        # 插入历史记录
        h_data = f"{w_name[:10]} {d_name[:10]}"
        cursor.execute(
            """
            INSERT INTO bmsql_history (h_c_d_id, h_c_w_id, h_c_id, h_d_id, h_w_id, h_date, h_amount, h_data)
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
        """,
            (d_id, w_id, c_id, d_id, w_id, datetime_now, h_amount, h_data),
        )
        # 提交事务
        connection.commit()
        print("Payment transaction completed successfully.")
        return 0
    except Exception as e:
        # 回滚事务
        connection.rollback()
        print(f"Transaction failed: {e}")
        return -1
