'use client';

import { useAuth } from '@/contexts/AuthContext';
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card';
import { Button } from '@/components/ui/button';
import { useRouter } from 'next/navigation';
import { ROUTES } from '@/lib/constants';
import { FileText, Search, Wrench, TrendingUp } from 'lucide-react';

export default function DashboardPage() {
  const { user, isAdmin } = useAuth();
  const router = useRouter();

  const quickActions = [
    {
      title: '浏览需求',
      description: '查看社区中的服务需求',
      icon: <Search className="h-8 w-8 text-blue-500" />,
      action: () => router.push(ROUTES.BROWSE_NEEDS),
    },
    {
      title: '发布需求',
      description: '发布您的服务需求',
      icon: <FileText className="h-8 w-8 text-green-500" />,
      action: () => router.push(ROUTES.CREATE_NEED),
    },
    {
      title: '我的服务',
      description: '查看您提供的服务',
      icon: <Wrench className="h-8 w-8 text-purple-500" />,
      action: () => router.push(ROUTES.MY_SERVICES),
    },
  ];

  if (isAdmin) {
    quickActions.push({
      title: '统计分析',
      description: '查看平台数据统计',
      icon: <TrendingUp className="h-8 w-8 text-orange-500" />,
      action: () => router.push(ROUTES.ADMIN_STATISTICS),
    });
  }

  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-3xl font-bold text-gray-900">
          欢迎回来，{user?.username}！
        </h1>
        <p className="text-muted-foreground mt-2">
          开始使用好服务平台，发布需求或提供服务
        </p>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
        {quickActions.map((action, index) => (
          <Card
            key={index}
            className="cursor-pointer hover:shadow-lg transition-shadow"
            onClick={action.action}
          >
            <CardHeader>
              <div className="flex items-center justify-between">
                {action.icon}
              </div>
              <CardTitle className="mt-4">{action.title}</CardTitle>
              <CardDescription>{action.description}</CardDescription>
            </CardHeader>
            <CardContent>
              <Button variant="outline" className="w-full">
                查看详情
              </Button>
            </CardContent>
          </Card>
        ))}
      </div>

      <Card>
        <CardHeader>
          <CardTitle>快速入门</CardTitle>
          <CardDescription>了解如何使用好服务平台</CardDescription>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="space-y-2">
            <h3 className="font-semibold">1. 浏览需求</h3>
            <p className="text-sm text-muted-foreground">
              在"浏览需求"页面查看社区中的服务需求，找到您可以提供帮助的项目
            </p>
          </div>
          <div className="space-y-2">
            <h3 className="font-semibold">2. 响应服务</h3>
            <p className="text-sm text-muted-foreground">
              点击感兴趣的需求，提交您的服务响应，包括服务描述、报价和可服务时间
            </p>
          </div>
          <div className="space-y-2">
            <h3 className="font-semibold">3. 发布需求</h3>
            <p className="text-sm text-muted-foreground">
              如果您需要帮助，可以发布服务需求，等待社区成员响应
            </p>
          </div>
          <div className="space-y-2">
            <h3 className="font-semibold">4. 管理订单</h3>
            <p className="text-sm text-muted-foreground">
              在"我需要"和"我服务"页面管理您的需求和服务响应
            </p>
          </div>
        </CardContent>
      </Card>
    </div>
  );
}
