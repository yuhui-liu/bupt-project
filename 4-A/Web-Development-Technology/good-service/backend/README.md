# 好服务平台 - 后端API

基于 Flask + SQLAlchemy + SQLite 的后端服务。

## 技术栈

- Python 3.9+
- Flask 3.0+
- SQLAlchemy 2.0+
- SQLite
- JWT 认证
- uv 包管理

## 快速开始

### 1. 安装依赖

使用 uv 安装依赖：

```bash
# 安装 uv (如果还没安装)
curl -LsSf https://astral.sh/uv/install.sh | sh

# 创建虚拟环境并安装依赖
uv venv
source .venv/bin/activate  # Linux/Mac
# 或
.venv\Scripts\activate  # Windows

# 安装依赖
uv pip install -e .

# 安装开发依赖
uv pip install -e ".[dev]"

# 安装生产环境依赖
uv pip install -e ".[prod]"
```

### 2. 配置环境变量

```bash
cp .env.example .env
# 编辑 .env 文件，设置必要的配置
```

### 3. 初始化数据库

```bash
python init_db.py
```

这将创建数据库表并插入初始数据（管理员账户、服务类型、地域等）。

### 4. 运行开发服务器

```bash
python run.py
```

服务器将在 http://localhost:8080 启动。

### 5. 生产环境部署

```bash
gunicorn -w 4 -b 0.0.0.0:8080 run:app
```

## 项目结构

```
backend/
├── app/                    # 应用主目录
│   ├── __init__.py        # Flask应用初始化
│   ├── config.py          # 配置文件
│   ├── models.py          # 数据模型
│   ├── schemas.py         # 数据验证schema
│   ├── auth/              # 认证模块
│   ├── api/               # API路由
│   └── utils/             # 工具函数
├── uploads/               # 上传文件存储
├── tests/                 # 测试文件
├── init_db.py            # 数据库初始化脚本
├── run.py                # 应用启动入口
├── pyproject.toml        # 项目配置和依赖
├── .env.example          # 环境变量示例
└── README.md             # 项目说明
```
