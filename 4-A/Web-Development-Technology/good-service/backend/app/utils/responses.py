"""
统一响应格式
"""
from flask import jsonify

def success_response(message='操作成功', data=None):
    """成功响应"""
    response = {
        'success': True,
        'message': message
    }
    if data is not None:
        response['data'] = data
    return jsonify(response), 200

def error_response(message, code=None, status=400, details=None):
    """错误响应"""
    response = {
        'success': False,
        'message': message
    }
    if code:
        response['error'] = {
            'code': code
        }
        if details:
            response['error']['details'] = details
    return jsonify(response), status

def created_response(message='创建成功', data=None):
    """创建成功响应"""
    response = {
        'success': True,
        'message': message
    }
    if data is not None:
        response['data'] = data
    return jsonify(response), 201
