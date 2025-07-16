import psycopg2
from datetime import datetime


def neword(connection, w_id, d_id, c_id, o_ol_cnt, supware, itemid, qty):
    try:
        # 开始事务
        connection.autocommit = False
        cursor = connection.cursor()

        # 获取当前时间戳
        datetime_now = datetime.now()

        # 查询客户和仓库信息
        cursor.execute(
            """
            SELECT c_discount, c_last, c_credit, w_tax
            FROM bmsql_customer c
            JOIN bmsql_warehouse w ON c.c_w_id = w.w_id
            WHERE w.w_id = %s
              AND c.c_w_id = w.w_id
              AND c.c_d_id = %s
              AND c.c_id = %s
        """,
            (w_id, d_id, c_id),
        )
        customer_data = cursor.fetchone()
        if not customer_data:
            raise Exception("Customer not found")
        c_discount, c_last, c_credit, w_tax = customer_data

        # 查询并更新 district 表
        cursor.execute(
            """
            SELECT d_next_o_id, d_tax
            FROM bmsql_district
            WHERE d_w_id = %s
              AND d_id = %s
        """,
            (w_id, d_id),
        )
        district_data = cursor.fetchone()
        if not district_data:
            raise Exception("District not found")
        d_next_o_id, d_tax = district_data

        cursor.execute(
            """
            UPDATE bmsql_district
            SET d_next_o_id = %s
            WHERE d_w_id = %s
              AND d_id = %s
        """,
            (d_next_o_id + 1, w_id, d_id),
        )

        # 插入订单记录到 orders 表
        cursor.execute(
            """
            INSERT INTO bmsql_oorder (o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_ol_cnt, o_all_local)
            VALUES (%s, %s, %s, %s, %s, %s, %s)
        """,
            (d_next_o_id, d_id, w_id, c_id, datetime_now, o_ol_cnt, 1),
        )

        # 插入新订单到 new_order 表
        cursor.execute(
            """
            INSERT INTO bmsql_new_order (no_o_id, no_d_id, no_w_id)
            VALUES (%s, %s, %s)
        """,
            (d_next_o_id, d_id, w_id),
        )

        # 初始化总金额
        total_amount = 0
        all_local = 1

        # 循环处理每个订单行
        for ol_number in range(o_ol_cnt):
            ol_supply_w_id = int(supware[ol_number])
            ol_i_id = int(itemid[ol_number])
            ol_quantity = int(qty[ol_number])

            if ol_supply_w_id != w_id:
                all_local = 0

            # 查询商品信息
            cursor.execute(
                """
                SELECT i_price, i_name, i_data
                FROM bmsql_item
                WHERE i_id = %s
            """,
                (ol_i_id,),
            )
            item_data = cursor.fetchone()
            if not item_data:
                raise Exception("Item not found")
            i_price, i_name, i_data = item_data

            # 查询库存信息
            cursor.execute(
                """
                SELECT s_quantity, s_data
                FROM bmsql_stock
                WHERE s_i_id = %s AND s_w_id = %s
            """,
                (ol_i_id, ol_supply_w_id),
            )
            stock_data = cursor.fetchone()
            if not stock_data:
                raise Exception("Stock not found")
            s_quantity, s_data = stock_data

            # 更新库存
            if s_quantity > ol_quantity:
                new_s_quantity = s_quantity - ol_quantity
            else:
                new_s_quantity = s_quantity - ol_quantity + 91

            cursor.execute(
                """
                UPDATE bmsql_stock
                SET s_quantity = %s
                WHERE s_i_id = %s AND s_w_id = %s
            """,
                (new_s_quantity, ol_i_id, ol_supply_w_id),
            )

            # 计算订单行金额
            ol_amount = ol_quantity * i_price * (1 + w_tax + d_tax) * (1 - c_discount)
            total_amount += ol_amount

            # 插入订单行记录到 order_line 表
            cursor.execute(
                """
                INSERT INTO bmsql_order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_quantity, ol_amount)
                VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
            """,
                (
                    d_next_o_id,
                    d_id,
                    w_id,
                    ol_number + 1,
                    ol_i_id,
                    ol_supply_w_id,
                    ol_quantity,
                    ol_amount,
                ),
            )

        # 提交事务
        connection.commit()
        print(f"Transaction completed successfully. Total amount: {total_amount}")
        return 0
    except Exception as e:
        # 回滚事务
        connection.rollback()
        print(f"Transaction failed: {e}")
        return -1
