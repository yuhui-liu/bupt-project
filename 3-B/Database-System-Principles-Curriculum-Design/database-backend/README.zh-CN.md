# 电子商务数据库管理系统

这是一个用于管理电子商务数据库的简单系统，允许用户对数据库执行CRUD操作。它提供了一个简单的网页界面，用户无需掌握SQL即可操作数据库。

这个系统使用Python和Flask Web框架作为后端，React作为前端。
此仓库仅包含后端部分。

系统使用PostgreSQL作为数据库，采用SQLAlchemy作为ORM（对象关系映射器）与数据库交互。

## 开发
### 前提条件
- Python 3.10或更高版本
- 配置有PostgreSQL的服务器，或本地安装的PostgreSQL

首先安装必要的依赖：
```shell
# 在此之前，您可能需要创建一个虚拟环境
# 例如，python -m venv venv
# 然后激活它
# 在Windows上
# venv\Scripts\activate
# 在Linux或Mac上
# source venv/bin/activate
pip install -r requirements.txt
```

然后在根目录下创建一个`.flaskenv`文件。可以参考`.flaskenv.example`文件。

### 数据库迁移
```shell
# 初始化数据库迁移（仅第一次运行时）
flask db init

# 为新模型创建迁移
flask db migrate -m "Add FileUpload model"

# 应用迁移
flask db upgrade
```

### 运行应用程序
运行：
```shell
python run.py
```