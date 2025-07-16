#!/usr/bin/env python3
# filepath: /home/lyh/projects/database/database-backend/scripts/tbl_to_csv.py

"""
TBL文件转CSV格式脚本

TBL文件通常是制表符分隔的文本文件，此脚本将其转换为标准CSV格式。

使用方法：
python scripts/tbl_to_csv.py input.tbl output.csv
python scripts/tbl_to_csv.py input.tbl  # 自动生成output文件名
python scripts/tbl_to_csv.py --dir /path/to/tbl/files  # 批量转换目录下所有tbl文件
python scripts/tbl_to_csv.py --help  # 查看帮助
"""

import sys
import os
import argparse
import csv
import glob
from pathlib import Path


def convert_tbl_to_csv(input_file, output_file=None, delimiter="\t", encoding="utf-8"):
    """
    将TBL文件转换为CSV格式

    Args:
        input_file (str): 输入的TBL文件路径
        output_file (str): 输出的CSV文件路径，如果为None则自动生成
        delimiter (str): TBL文件的分隔符，默认为制表符
        encoding (str): 文件编码，默认为utf-8

    Returns:
        bool: 转换是否成功
    """
    try:
        # 如果没有指定输出文件，自动生成
        if output_file is None:
            input_path = Path(input_file)
            output_file = input_path.with_suffix(".csv")

        # 检查输入文件是否存在
        if not os.path.exists(input_file):
            print(f"❌ 输入文件不存在: {input_file}")
            return False

        # 读取TBL文件并写入CSV
        with open(input_file, "r", encoding=encoding, newline="") as tbl_file:
            with open(output_file, "w", encoding=encoding, newline="") as csv_file:
                # 创建CSV写入器
                csv_writer = csv.writer(csv_file)

                line_count = 0
                for line in tbl_file:
                    # 移除行尾换行符并按分隔符分割
                    cleaned_line = line.rstrip("\n\r")

                    # 移除行尾的多余竖线
                    if cleaned_line.endswith("|"):
                        cleaned_line = cleaned_line.rstrip("|")

                    fields = cleaned_line.split(delimiter)
                    csv_writer.writerow(fields)
                    line_count += 1

                print(f"✅ 转换成功: {input_file} -> {output_file}")
                print(f"📊 处理了 {line_count} 行数据")
                return True

    except UnicodeDecodeError as e:
        print(f"❌ 编码错误: {e}")
        print("💡 尝试使用不同的编码，如: --encoding gbk 或 --encoding latin-1")
        return False
    except Exception as e:
        print(f"❌ 转换失败: {e}")
        return False


def batch_convert_directory(directory, delimiter="\t", encoding="utf-8"):
    """
    批量转换目录下的所有TBL文件

    Args:
        directory (str): 包含TBL文件的目录路径
        delimiter (str): TBL文件的分隔符
        encoding (str): 文件编码

    Returns:
        tuple: (成功数量, 失败数量)
    """
    if not os.path.exists(directory):
        print(f"❌ 目录不存在: {directory}")
        return 0, 0

    # 查找所有TBL文件
    tbl_files = glob.glob(os.path.join(directory, "*.tbl"))
    tbl_files.extend(glob.glob(os.path.join(directory, "*.TBL")))

    if not tbl_files:
        print(f"📁 目录 {directory} 中没有找到TBL文件")
        return 0, 0

    print(f"🔍 找到 {len(tbl_files)} 个TBL文件")

    success_count = 0
    fail_count = 0

    for tbl_file in tbl_files:
        if convert_tbl_to_csv(tbl_file, delimiter=delimiter, encoding=encoding):
            success_count += 1
        else:
            fail_count += 1

    print(f"\n📈 批量转换完成:")
    print(f"✅ 成功: {success_count} 个文件")
    print(f"❌ 失败: {fail_count} 个文件")

    return success_count, fail_count


def preview_tbl_file(input_file, lines=5, delimiter="\t", encoding="utf-8"):
    """
    预览TBL文件的前几行

    Args:
        input_file (str): TBL文件路径
        lines (int): 预览的行数
        delimiter (str): 分隔符
        encoding (str): 文件编码
    """
    try:
        print(f"📋 预览文件: {input_file}")
        print(f"📝 编码: {encoding}")
        print("-" * 60)

        with open(input_file, "r", encoding=encoding) as f:
            for i, line in enumerate(f):
                if i >= lines:
                    break

                # 显示原始行和处理后的行
                original_line = line.rstrip("\n\r")
                cleaned_line = (
                    original_line.rstrip("|")
                    if original_line.endswith("|")
                    else original_line
                )
                fields = cleaned_line.split(delimiter)

                print(f"第{i+1}行 原始: {repr(original_line)}")
                print(f"第{i+1}行 处理后 ({len(fields)}字段): {fields}")
                print()

        print("-" * 60)

    except Exception as e:
        print(f"❌ 预览失败: {e}")


def main():
    """主函数"""
    parser = argparse.ArgumentParser(
        description="TBL文件转CSV格式工具",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
使用示例：
  python scripts/tbl_to_csv.py data.tbl                    # 转换单个文件
  python scripts/tbl_to_csv.py data.tbl output.csv         # 指定输出文件
  python scripts/tbl_to_csv.py --dir ./data                # 批量转换目录
  python scripts/tbl_to_csv.py --preview data.tbl          # 预览文件内容
        """,
    )

    parser.add_argument("input", nargs="?", help="输入的TBL文件路径")
    parser.add_argument("output", nargs="?", help="输出的CSV文件路径（可选）")
    parser.add_argument("--dir", "-d", help="批量转换目录下的所有TBL文件")
    parser.add_argument(
        "--delimiter", default="\t", help="TBL文件的分隔符（默认：制表符）"
    )
    parser.add_argument("--encoding", default="utf-8", help="文件编码（默认：utf-8）")
    parser.add_argument("--preview", "-p", action="store_true", help="预览TBL文件内容")
    parser.add_argument(
        "--lines", type=int, default=5, help="预览时显示的行数（默认：5）"
    )

    args = parser.parse_args()

    # 处理分隔符的特殊字符
    if args.delimiter == "\\t":
        args.delimiter = "\t"
    elif args.delimiter == "\\n":
        args.delimiter = "\n"
    elif args.delimiter == "\\r":
        args.delimiter = "\r"

    # 批量转换目录
    if args.dir:
        batch_convert_directory(args.dir, args.delimiter, args.encoding)
        return

    # 检查是否提供了输入文件
    if not args.input:
        parser.print_help()
        return

    # 预览模式
    if args.preview:
        preview_tbl_file(args.input, args.lines, args.delimiter, args.encoding)
        return

    # 单文件转换
    convert_tbl_to_csv(args.input, args.output, args.delimiter, args.encoding)


if __name__ == "__main__":
    main()
