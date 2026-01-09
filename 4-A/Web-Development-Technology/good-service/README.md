# 好服务 (Good Service)

社区服务供需匹配平台 - 用户可以发布服务需求（"我需要"）或响应他人需求（"我服务"），实现社区互助服务。

## 📋 项目概述

好服务是一个基于 Next.js 14 构建的现代化 Web 应用，旨在连接社区成员，促进互助服务。平台支持用户发布服务需求、响应他人需求、以及完整的订单管理功能。

## ✨ 主要功能

### 用户功能
- 🔐 **用户认证** - 注册、登录（带图形验证码）、登出
- 👤 **个人信息管理** - 查看和修改个人资料、修改密码
- 📝 **我需要** - 发布、编辑、删除服务需求，查看和管理响应
- 🛠️ **我服务** - 浏览需求、提交服务响应、管理服务记录
- 🔍 **浏览需求** - 搜索、筛选社区服务需求

### 管理员功能
- 👥 **用户管理** - 查看所有用户、用户统计
- 📋 **需求管理** - 管理所有服务需求
- 🔧 **服务管理** - 管理所有服务响应
- 📊 **统计分析** - 数据可视化、趋势分析、报表导出

## 🛠️ 技术栈

- **框架**: [Next.js 14](https://nextjs.org/) (App Router)
- **UI 库**: [React 18](https://react.dev/)
- **样式**: [Tailwind CSS](https://tailwindcss.com/)
- **组件库**: [Shadcn UI](https://ui.shadcn.com/)
- **表单管理**: [React Hook Form](https://react-hook-form.com/) + [Zod](https://zod.dev/)
- **数据获取**: [SWR](https://swr.vercel.app/)
- **图表**: [Recharts](https://recharts.org/)
- **图标**: [Lucide React](https://lucide.dev/)
- **语言**: [TypeScript](https://www.typescriptlang.org/)
- **包管理器**: [pnpm](https://pnpm.io/)

```

## 🚀 快速开始

### 环境要求

- Node.js 18+
- pnpm 8+

### 安装步骤

1. **克隆仓库**
```bash
git clone <repository-url>
cd good-service
```

2. **安装依赖**
```bash
pnpm install
```

3. **配置环境变量**

复制 `.env.example` 到 `.env.local` 并配置：

```bash
cp .env.example .env.local
```

编辑 `.env.local` 文件，设置后端 API 地址：

```env
NEXT_PUBLIC_API_BASE_URL=http://localhost:8080/api
NEXT_PUBLIC_UPLOAD_BASE_URL=http://localhost:8080/uploads
```

4. **启动开发服务器**
```bash
pnpm dev
```

应用将在 [http://localhost:3000](http://localhost:3000) 启动

### 构建生产版本

```bash
# 构建
pnpm build

# 启动生产服务器
pnpm start
```

