"""
应用启动入口
"""
import os
from dotenv import load_dotenv

# 加载环境变量
load_dotenv()

from app import create_app

app = create_app()

if __name__ == '__main__':
    host = os.getenv('HOST', '0.0.0.0')
    port = int(os.getenv('PORT', 8080))
    debug = os.getenv('FLASK_ENV') == 'development'

    print(f"Starting server on {host}:{port}")
    print(f"Debug mode: {debug}")

    app.run(host=host, port=port, debug=debug)
