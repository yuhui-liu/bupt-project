import os
import csv
from datetime import datetime

# 配置参数
SEPARATOR = "|"
INPUT_DIR = ""
OUTPUT_DIR = "cleaned_data"
LOG_DIR = "logs"

os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(LOG_DIR, exist_ok=True)


def clean_table(table_name, columns, checks):
    input_path = os.path.join(INPUT_DIR, f"{table_name}.txt")
    output_path = os.path.join(OUTPUT_DIR, f"{table_name}_clean.txt")
    log_path = os.path.join(LOG_DIR, f"{table_name}_clean.log")

    cleaned_count = 0
    error_count = 0

    with open(input_path, "r", encoding="utf-8") as infile, open(
        output_path, "w", encoding="utf-8", newline=""
    ) as outfile, open(log_path, "w", encoding="utf-8") as logfile:

        writer = csv.writer(outfile, delimiter=SEPARATOR)
        reader = csv.reader(infile, delimiter=SEPARATOR)

        logfile.write(f"Cleaning log for {table_name}\n")
        logfile.write(f"Total rows processed: 0, Cleaned rows: 0, Errors: 0\n\n")

        for row_num, row in enumerate(reader, 1):
            if len(row) != len(columns):
                logfile.write(
                    f"Row {row_num}: Incorrect column count (expected {len(columns)}, got {len(row)})\n"
                )
                error_count += 1
                continue

            valid = True
            error_details = []

            # 逐列检查
            for col_idx, (col_name, col_type) in enumerate(columns.items()):
                value = row[col_idx]

                # 空值检查
                if col_name in checks and "not_null" in checks[col_name]:
                    if value.strip() == "":
                        valid = False
                        error_details.append(f"{col_name} is NULL")
                        continue  # 跳过后续检查

                # 数据类型检查 - 分开处理每种类型
                if col_name in checks:
                    # 整数检查
                    if "int" in checks[col_name]:
                        try:
                            int(value)
                        except ValueError:
                            valid = False
                            error_details.append(f"{col_name} invalid integer")
                            continue

                    # 小数检查
                    if "decimal" in checks[col_name]:
                        try:
                            float(value)
                        except ValueError:
                            valid = False
                            error_details.append(f"{col_name} invalid decimal")
                            continue

                    # 日期检查
                    if "date" in checks[col_name]:
                        try:
                            datetime.strptime(value, "%Y-%m-%d")
                        except ValueError:
                            valid = False
                            error_details.append(f"{col_name} invalid date")
                            continue

                # 数据范围检查
                if col_name in checks and "range" in checks[col_name]:
                    min_val, max_val = checks[col_name]["range"]
                    try:
                        num_val = float(value)
                        if num_val < min_val or (
                            max_val is not None and num_val > max_val
                        ):
                            valid = False
                            error_details.append(f"{col_name} out of range")
                    except ValueError:
                        valid = False
                        error_details.append(f"{col_name} range check failed")

            if valid:
                writer.writerow(row)
                cleaned_count += 1
            else:
                error_count += 1
                logfile.write(f"Row {row_num}: Invalid - {', '.join(error_details)}\n")

        # 更新日志摘要
        logfile.seek(0)
        logfile.write(f"Cleaning log for {table_name}\n")
        logfile.write(
            f"Total rows processed: {row_num}, Cleaned rows: {cleaned_count}, Errors: {error_count}\n\n"
        )


tables = {
    "orders": {
        "columns": {
            0: ("O_ORDERKEY", "INTEGER"),
            1: ("O_CUSTKEY", "INTEGER"),
            2: ("O_ORDERSTATUS", "CHAR(1)"),
            3: ("O_TOTALPRICE", "DECIMAL(15,2)"),
            4: ("O_ORDERDATE", "DATE"),
            5: ("O_ORDERPRIORITY", "CHAR(15)"),
            6: ("O_CLERK", "CHAR(15)"),
            7: ("O_SHIPPRIORITY", "INTEGER"),
            8: ("O_COMMENT", "VARCHAR(79)"),
        },
        "checks": {
            "O_ORDERKEY": {"not_null": True, "int": True},
            "O_CUSTKEY": {"not_null": True, "int": True},
            "O_ORDERSTATUS": {"not_null": True, "char_1": True},
            "O_TOTALPRICE": {"not_null": True, "decimal": True, "range": (0, None)},
            "O_ORDERDATE": {"not_null": True, "date": True},
            "O_SHIPPRIORITY": {"not_null": True, "int": True},
        },
    },
    "region": {
        "columns": {
            0: ("R_REGIONKEY", "INTEGER"),
            1: ("R_NAME", "CHAR(25)"),
            2: ("R_COMMENT", "VARCHAR(152)"),
        },
        "checks": {
            "R_REGIONKEY": {"not_null": True, "int": True},
            "R_NAME": {"not_null": True},
        },
    },
    "nation": {
        "columns": {
            0: ("N_NATIONKEY", "INTEGER"),
            1: ("N_NAME", "CHAR(25)"),
            2: ("N_REGIONKEY", "INTEGER"),
            3: ("N_COMMENT", "VARCHAR(152)"),
        },
        "checks": {
            "N_NATIONKEY": {"not_null": True, "int": True},
            "N_NAME": {"not_null": True},
            "N_REGIONKEY": {"not_null": True, "int": True},
        },
    },
    "supplier": {
        "columns": {
            0: ("S_SUPPKEY", "INTEGER"),
            1: ("S_NAME", "CHAR(25)"),
            2: ("S_ADDRESS", "VARCHAR(40)"),
            3: ("S_NATIONKEY", "INTEGER"),
            4: ("S_PHONE", "CHAR(15)"),
            5: ("S_ACCTBAL", "DECIMAL(15,2)"),
            6: ("S_COMMENT", "VARCHAR(101)"),
        },
        "checks": {
            "S_SUPPKEY": {"not_null": True, "int": True},
            "S_NATIONKEY": {"not_null": True, "int": True},
            "S_ACCTBAL": {"not_null": True, "decimal": True},
        },
    },
    "part": {
        "columns": {
            0: ("P_PARTKEY", "INTEGER"),
            1: ("P_NAME", "VARCHAR(55)"),
            2: ("P_MFGR", "CHAR(25)"),
            3: ("P_BRAND", "CHAR(10)"),
            4: ("P_TYPE", "VARCHAR(25)"),
            5: ("P_SIZE", "INTEGER"),
            6: ("P_CONTAINER", "CHAR(10)"),
            7: ("P_RETAILPRICE", "DECIMAL(15,2)"),
            8: ("P_COMMENT", "VARCHAR(23)"),
        },
        "checks": {
            "P_PARTKEY": {"not_null": True, "int": True},
            "P_SIZE": {"not_null": True, "int": True},
            "P_RETAILPRICE": {"not_null": True, "decimal": True, "range": (0, None)},
        },
    },
    "partsupp": {
        "columns": {
            0: ("PS_PARTKEY", "INTEGER"),
            1: ("PS_SUPPKEY", "INTEGER"),
            2: ("PS_AVAILQTY", "INTEGER"),
            3: ("PS_SUPPLYCOST", "DECIMAL(15,2)"),
            4: ("PS_COMMENT", "VARCHAR(199)"),
        },
        "checks": {
            "PS_PARTKEY": {"not_null": True, "int": True},
            "PS_SUPPKEY": {"not_null": True, "int": True},
            "PS_AVAILQTY": {"not_null": True, "int": True, "range": (0, None)},
            "PS_SUPPLYCOST": {"not_null": True, "decimal": True},
        },
    },
    "customer": {
        "columns": {
            0: ("C_CUSTKEY", "INTEGER"),
            1: ("C_NAME", "VARCHAR(25)"),
            2: ("C_ADDRESS", "VARCHAR(40)"),
            3: ("C_NATIONKEY", "INTEGER"),
            4: ("C_PHONE", "CHAR(15)"),
            5: ("C_ACCTBAL", "DECIMAL(15,2)"),
            6: ("C_MKTSEGMENT", "CHAR(10)"),
            7: ("C_COMMENT", "VARCHAR(117)"),
        },
        "checks": {
            "C_CUSTKEY": {"not_null": True, "int": True},
            "C_NATIONKEY": {"not_null": True, "int": True},
            "C_ACCTBAL": {"not_null": True, "decimal": True},
        },
    },
    "lineitem": {
        "columns": {
            0: ("L_ORDERKEY", "INTEGER"),
            1: ("L_PARTKEY", "INTEGER"),
            2: ("L_SUPPKEY", "INTEGER"),
            3: ("L_LINENUMBER", "INTEGER"),
            4: ("L_QUANTITY", "DECIMAL(15,2)"),
            5: ("L_EXTENDEDPRICE", "DECIMAL(15,2)"),
            6: ("L_DISCOUNT", "DECIMAL(15,2)"),
            7: ("L_TAX", "DECIMAL(15,2)"),
            8: ("L_RETURNFLAG", "CHAR(1)"),
            9: ("L_LINESTATUS", "CHAR(1)"),
            10: ("L_SHIPDATE", "DATE"),
            11: ("L_COMMITDATE", "DATE"),
            12: ("L_RECEIPTDATE", "DATE"),
            13: ("L_SHIPINSTRUCT", "CHAR(25)"),
            14: ("L_SHIPMODE", "CHAR(10)"),
            15: ("L_COMMENT", "VARCHAR(44)"),
        },
        "checks": {
            "L_QUANTITY": {"not_null": True, "decimal": True, "range": (0, None)},
            "L_EXTENDEDPRICE": {"not_null": True, "decimal": True, "range": (0, None)},
            "L_DISCOUNT": {"not_null": True, "decimal": True, "range": (0, 1)},
            "L_TAX": {"not_null": True, "decimal": True, "range": (0, None)},
            "L_SHIPDATE": {"not_null": True, "date": True},
        },
    },
}

# 执行清洗
for table_name, config in tables.items():
    print(f"Cleaning {table_name}...")
    clean_table(table_name, config["columns"], config["checks"])
    print(f"  {table_name} cleaning completed")

print("All tables cleaned successfully!")
