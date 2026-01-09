# DNS Relay 中继服务器

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Language](https://img.shields.io/badge/language-C-blue.svg)]()
[![Standard](https://img.shields.io/badge/C-C11-blue.svg)]()
[![License](https://img.shields.io/badge/license-MIT-green.svg)]()

> 《计算机网络》课程设计项目 - DNS中继服务器实现

## 📖 项目简介

本项目是一个完整的DNS中继服务器实现，支持DNS请求转发、本地域名解析和不良网站屏蔽功能。该项目采用模块化设计，使用C语言开发，支持多线程并发处理，具有良好的性能和稳定性。

### 🚀 主要功能

- **DNS中继功能**: 将客户端DNS请求转发至上游DNS服务器
- **本地域名解析**: 支持从本地文件加载域名-IP映射表，提供快速本地解析
- **网站屏蔽功能**: 支持屏蔽指定域名（返回0.0.0.0或拒绝响应）
- **多线程并发**: 使用多线程处理多个DNS请求，提高服务器性能
- **ID映射机制**: 实现请求ID转换，避免ID冲突
- **调试功能**: 提供多级调试信息输出
- **超时处理**: 支持DNS查询超时机制

## 🛠️ 技术栈

- **编程语言**: C (C11标准)
- **构建工具**: CMake 3.28+
- **网络编程**: UDP Socket, POSIX线程
- **命令行解析**: GNU argp库
- **开发环境**: Linux/Unix兼容系统

## 📁 项目结构

```
.
├── CMakeLists.txt          # CMake构建配置
├── README.md              # 项目说明文档
├── compile_commands.json  # 编译数据库
├── include/               # 头文件目录
│   ├── args_handler.h     # 命令行参数处理
│   ├── consts.h          # 常量定义
│   ├── dns_parser.h      # DNS报文解析
│   ├── file_reader.h     # 文件读取
│   ├── logs.h            # 日志系统
│   ├── mapping.h         # ID映射
│   ├── network.h         # 网络处理
│   └── structs.h         # 数据结构定义
├── src/                  # 源代码目录
│   ├── args_handler.c    # 命令行参数处理实现
│   ├── dns_parser.c      # DNS报文解析实现
│   ├── dnsrelay.c        # 主程序入口
│   ├── file_reader.c     # 文件读取实现
│   ├── mapping.c         # ID映射实现
│   └── network.c         # 网络处理实现
└── report/               # 设计报告和文档
    ├── report.pdf        # 设计报告
    └── *.png            # 报告图片
```

## 🔧 编译和安装

### 系统要求

- Linux/Unix兼容系统
- GCC 编译器支持C11标准
- CMake 3.28或更高版本
- GNU argp库（通常包含在glibc中）

### 编译步骤

1. **克隆项目**
```bash
git clone [repository-url]
cd "Computer Network curriculum design"
```

2. **使用CMake编译**
```bash
mkdir build
cd build
cmake ..
make
```

3. **运行程序**
```bash
./dnsrelay [选项] [DNS服务器地址] [本地文件路径]
```

## 📚 使用说明

### 命令行参数

```bash
./dnsrelay [-d|-dd] [dns-server-ipaddr] [filename]
```

**参数说明:**
- `-d`: 启用简略调试信息输出
- `-dd`: 启用详细调试信息输出
- `dns-server-ipaddr`: 上游DNS服务器IP地址（默认: 10.3.9.4）
- `filename`: 本地DNS记录文件路径（默认: dnsrelay.txt）

### 使用示例

1. **基本使用**
```bash
./dnsrelay
```

2. **指定DNS服务器**
```bash
./dnsrelay 8.8.8.8
```

3. **使用调试模式**
```bash
./dnsrelay -dd 114.114.114.114 /etc/dnsrelay.txt
```

### 本地DNS记录文件格式

本地DNS记录文件每行包含一个IP地址和对应的域名，格式如下：

```
192.168.1.100 example.com
0.0.0.0 blocked-site.com
127.0.0.1 localhost
```

- `0.0.0.0`: 表示屏蔽该域名
- 其他有效IP地址: 提供本地解析

## 🏗️ 架构设计

### 模块划分

1. **参数处理模块** (`args_handler`)
   - 解析命令行参数
   - 初始化全局配置

2. **文件读取模块** (`file_reader`)
   - 加载本地DNS记录文件
   - 提供域名查询接口

3. **DNS解析模块** (`dns_parser`)
   - 解析DNS请求报文
   - 构造DNS响应报文

4. **网络处理模块** (`network`)
   - UDP服务器实现
   - 多线程请求处理

5. **ID映射模块** (`mapping`)
   - 请求ID转换管理
   - 客户端会话维护

6. **日志系统** (`logs`)
   - 多级调试信息输出
   - 时间戳记录

### 工作流程

```
客户端DNS请求 → UDP服务器接收 → 域名解析
    ↓
本地记录查找 → 找到 → 构造响应 → 返回客户端
    ↓
    未找到 → 转发上游DNS → 接收响应 → 返回客户端
```

## 🎯 技术特点

- **高性能**: 多线程并发处理，支持大量并发DNS请求
- **可靠性**: 实现超时重传机制，确保服务稳定
- **安全性**: 支持域名屏蔽，可用于网络安全管理
- **可扩展**: 模块化设计，易于功能扩展和维护
- **标准兼容**: 严格遵循RFC 1035 DNS协议标准

## 🧪 测试

可以使用以下工具测试DNS服务器：

```bash
# 使用dig测试
dig @127.0.0.1 example.com

# 使用nslookup测试
nslookup example.com 127.0.0.1
```

## 📄 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 👥 项目团队

- 刘昱辉
- 马杰瑞  
- 王鹏

---

**注**: 这是一个课程设计项目，仅供学习和研究使用。