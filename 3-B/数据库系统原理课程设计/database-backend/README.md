# E-commerce database management system

[中文](https://github.com/yuhui-liu/database-backend/blob/main/README.zh-CN.md)

This is a simple database management system which manages an e-commerce database. It allows users to perform CRUD operations on the database.
The system is designed to be user-friendly and easy to navigate, with a simple web interface that allows users to interact with the database without needing to know SQL.

It is built using Python and the Flask web framework as the backend, and React as the frontend.
This repo is the backend.

The system uses PostgreSQL as the database, and SQLAlchemy as the ORM (Object Relational Mapper) to interact with the database.

## Development
### Prerequisites
- Python 3.10 or higher
- a server with PostgreSQL, or local PostgreSQL

First install the requirements:
```shell
# before this, you may want to create a virtual environment
# e.g. python -m venv venv
# then activate it
# on Windows
# venv\Scripts\activate
# on Linux or Mac
# source venv/bin/activate
pip install -r requirements.txt
```

Then create a `.flaskenv` file in the root directory. You can take `.flaskenv.example` as an example.

### Database Migration
```shell
# Initialize database migration (first time only)
flask db init

# Create migration for new models
flask db migrate -m "Add FileUpload model"

# Apply migrations
flask db upgrade
```

### Run the Application
```shell
python run.py
```
