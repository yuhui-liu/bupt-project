#!/usr/bin/env python3
# filepath: database-backend/scripts/create_admin.py

"""
管理员创建脚本

使用方法：
python scripts/create_admin.py --interactive  # 交互式创建
python scripts/create_admin.py --username admin --email admin@example.com --password yourpassword  # 命令行参数
python scripts/create_admin.py --list  # 列出现有管理员
"""

import sys
import os
import argparse
from datetime import datetime
from getpass import getpass

# 添加项目根目录到Python路径
project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, project_root)

# 切换到项目根目录，确保能找到 .flaskenv 文件
os.chdir(project_root)

# 加载环境变量
try:
    from dotenv import load_dotenv

    # 加载 .flaskenv 文件
    load_dotenv(".flaskenv")
    # 也可以加载 .env 文件（如果存在）
    load_dotenv(".env")
except ImportError:
    print(
        "警告：未安装 python-dotenv，如果遇到配置问题，请运行：pip install python-dotenv"
    )

try:
    from app.models import User, db
    from app import create_app
except ImportError as e:
    print(f"错误：无法导入必要模块 - {e}")
    print("请确保在项目根目录运行此脚本，并且已安装所有依赖")
    print(f"当前工作目录: {os.getcwd()}")
    print(f"项目根目录: {project_root}")
    sys.exit(1)


def list_admins():
    """列出现有管理员"""
    try:
        app = create_app()
        with app.app_context():
            admins = User.query.filter_by(is_admin=True).all()

            if not admins:
                print("📝 当前没有管理员用户")
                return

            print("👑 现有管理员列表:")
            print("-" * 60)
            for admin in admins:
                print(f"用户名: {admin.username}")
                print(f"邮箱: {admin.email}")
                print(f"创建时间: {admin.apply_date}")
                print(f"通过时间: {admin.pass_date}")
                print("-" * 60)

    except Exception as e:
        print(f"❌ 查询失败: {e}")
        print("请检查数据库连接配置和 .flaskenv 文件")


def create_admin_interactive():
    """交互式创建管理员"""
    print("=== 交互式创建管理员 ===")

    username = input("管理员用户名: ").strip()
    if not username:
        print("❌ 用户名不能为空")
        return False

    email = input("管理员邮箱: ").strip()
    if not email:
        print("❌ 邮箱不能为空")
        return False

    password = getpass("管理员密码: ")
    if not password:
        print("❌ 密码不能为空")
        return False

    confirm_password = getpass("确认密码: ")
    if password != confirm_password:
        print("❌ 密码不一致")
        return False

    return create_admin_user(username, email, password)


def create_admin_user(username, email, password):
    """创建管理员用户"""
    try:
        app = create_app()
        with app.app_context():
            # 检查用户是否已存在
            if User.query.filter_by(username=username).first():
                print(f"❌ 用户名 '{username}' 已存在")
                return False

            if User.query.filter_by(email=email).first():
                print(f"❌ 邮箱 '{email}' 已被使用")
                return False

            # 创建管理员
            current_time = datetime.utcnow()
            admin_user = User(
                username=username,
                email=email,
                is_admin=True,
                apply_date=current_time,
                pass_date=current_time,
            )
            admin_user.set_password(password)

            db.session.add(admin_user)
            db.session.commit()

            print(f"✅ 管理员 '{username}' 创建成功！")
            return True

    except Exception as e:
        print(f"❌ 创建失败: {e}")
        print("请检查数据库连接配置和 .flaskenv 文件")
        return False


def check_env_files():
    """检查环境文件是否存在"""
    flaskenv_exists = os.path.exists(".flaskenv")
    env_exists = os.path.exists(".env")

    print("🔍 环境文件检查:")
    print(f"当前工作目录: {os.getcwd()}")
    print(f".flaskenv: {'✅ 存在' if flaskenv_exists else '❌ 不存在'}")
    print(f".env: {'✅ 存在' if env_exists else '❌ 不存在'}")

    if flaskenv_exists:
        print("\n📄 .flaskenv 内容预览:")
        try:
            with open(".flaskenv", "r") as f:
                lines = f.readlines()[:10]  # 只显示前10行
                for line in lines:
                    # 隐藏敏感信息
                    if "PASSWORD" in line.upper() or "SECRET" in line.upper():
                        print(line.split("=")[0] + "=***")
                    else:
                        print(line.strip())
        except Exception as e:
            print(f"无法读取 .flaskenv: {e}")


def main():
    """主函数"""
    parser = argparse.ArgumentParser(description="管理员用户创建工具")
    parser.add_argument("--interactive", "-i", action="store_true", help="交互式创建")
    parser.add_argument("--username", "-u", help="用户名")
    parser.add_argument("--email", "-e", help="邮箱")
    parser.add_argument("--password", "-p", help="密码")
    parser.add_argument("--list", "-l", action="store_true", help="列出现有管理员")
    parser.add_argument("--check-env", action="store_true", help="检查环境文件")

    args = parser.parse_args()

    if args.check_env:
        check_env_files()
        return

    if args.list:
        list_admins()
        return

    if args.interactive:
        create_admin_interactive()
        return

    if args.username and args.email and args.password:
        create_admin_user(args.username, args.email, args.password)
        return

    if args.username or args.email or args.password:
        print("❌ 使用命令行参数时，必须同时提供 --username, --email, --password")
        return

    # 默认进入交互模式
    create_admin_interactive()


if __name__ == "__main__":
    main()
