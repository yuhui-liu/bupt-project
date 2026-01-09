'use client';

import { useState, useEffect } from 'react';
import { useRouter } from 'next/navigation';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select';
import { Skeleton } from '@/components/ui/skeleton';
import { api, buildQueryString } from '@/lib/api';
import { ServiceType } from '@/types';
import { useToast } from '@/hooks/use-toast';
import { useAuth } from '@/contexts/AuthContext';
import { Download, TrendingUp, Users, CheckCircle, Activity } from 'lucide-react';
import StatisticsChart from '@/components/statistics/StatisticsChart';
import StatisticsTable from '@/components/statistics/StatisticsTable';
import { format } from 'date-fns';

interface StatisticsData {
  statistics: Array<{
    month: string;
    serviceType: string;
    serviceTypeName: string;
    needsPublished: number;
    responsesReceived: number;
    needsAccepted: number;
    needsCompleted: number;
    completionRate: number;
    avgResponseTime: number;
    avgResponseCount: number;
  }>;
  summary: {
    totalNeeds: number;
    totalResponses: number;
    totalAccepted: number;
    totalCompleted: number;
    overallCompletionRate: number;
    activeUsers: number;
    newUsers: number;
  };
  charts: {
    needsTrend: Array<{ month: string; count: number }>;
    serviceTypeDistribution: Array<{ serviceType: string; count: number; percentage: number }>;
    completionRateTrend: Array<{ month: string; rate: number }>;
  };
}

export default function AdminStatisticsPage() {
  const router = useRouter();
  const { toast } = useToast();
  const { isAdmin } = useAuth();

  const [serviceTypes, setServiceTypes] = useState<ServiceType[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [isExporting, setIsExporting] = useState(false);
  const [statisticsData, setStatisticsData] = useState<StatisticsData | null>(null);

  // Get current month and 6 months ago as default range
  const now = new Date();
  const sixMonthsAgo = new Date(now.getFullYear(), now.getMonth() - 6, 1);
  const defaultStartMonth = format(sixMonthsAgo, 'yyyy-MM');
  const defaultEndMonth = format(now, 'yyyy-MM');

  const [filters, setFilters] = useState({
    startMonth: defaultStartMonth,
    endMonth: defaultEndMonth,
    region: '',
    serviceType: '',
  });

  useEffect(() => {
    if (!isAdmin) {
      toast({
        variant: 'destructive',
        title: '权限不足',
        description: '仅管理员可访问此页面',
      });
      router.push('/dashboard');
      return;
    }
    fetchServiceTypes();
    fetchStatistics();
  }, [isAdmin]);

  const fetchServiceTypes = async () => {
    try {
      const data = await api.get<ServiceType[]>('/service-types');
      setServiceTypes(data);
    } catch (error) {
      console.error('Failed to fetch service types:', error);
    }
  };

  const fetchStatistics = async () => {
    setIsLoading(true);
    try {
      const queryString = buildQueryString({
        startMonth: filters.startMonth,
        endMonth: filters.endMonth,
        region: filters.region,
        serviceType: filters.serviceType,
      });

      const data = await api.get<StatisticsData>(`/admin/statistics${queryString}`);
      setStatisticsData(data);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载统计数据',
      });
    } finally {
      setIsLoading(false);
    }
  };

  const handleExport = async (format: 'csv' | 'excel') => {
    setIsExporting(true);
    try {
      const queryString = buildQueryString({
        startMonth: filters.startMonth,
        endMonth: filters.endMonth,
        region: filters.region,
        serviceType: filters.serviceType,
        format,
      });

      const response = await fetch(
        `${process.env.NEXT_PUBLIC_API_BASE_URL || 'http://localhost:8080/api'}/admin/statistics/export${queryString}`,
        {
          headers: {
            Authorization: `Bearer ${localStorage.getItem('auth_token')}`,
          },
        }
      );

      if (!response.ok) {
        throw new Error('导出失败');
      }

      const blob = await response.blob();
      const url = window.URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = `statistics_${filters.startMonth}_${filters.endMonth}.${format === 'csv' ? 'csv' : 'xlsx'}`;
      document.body.appendChild(a);
      a.click();
      window.URL.revokeObjectURL(url);
      document.body.removeChild(a);

      toast({
        title: '导出成功',
        description: '统计数据已导出',
      });
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '导出失败',
        description: error.message || '无法导出数据',
      });
    } finally {
      setIsExporting(false);
    }
  };

  const handleApplyFilters = () => {
    if (filters.startMonth > filters.endMonth) {
      toast({
        variant: 'destructive',
        title: '日期范围错误',
        description: '起始月份不能晚于终止月份',
      });
      return;
    }
    fetchStatistics();
  };

  if (!isAdmin) {
    return null;
  }

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl font-bold">统计分析</h1>
          <p className="text-muted-foreground mt-2">查看服务平台数据统计和趋势分析</p>
        </div>
        <div className="flex gap-2">
          <Button
            variant="outline"
            onClick={() => handleExport('csv')}
            disabled={isExporting || !statisticsData}
          >
            <Download className="h-4 w-4 mr-2" />
            导出CSV
          </Button>
          <Button
            variant="outline"
            onClick={() => handleExport('excel')}
            disabled={isExporting || !statisticsData}
          >
            <Download className="h-4 w-4 mr-2" />
            导出Excel
          </Button>
        </div>
      </div>

      {/* Filters */}
      <Card>
        <CardHeader>
          <CardTitle>筛选条件</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-5 gap-4">
            <div className="space-y-2">
              <Label htmlFor="startMonth">起始月份</Label>
              <Input
                id="startMonth"
                type="month"
                value={filters.startMonth}
                onChange={(e) => setFilters({ ...filters, startMonth: e.target.value })}
              />
            </div>
            <div className="space-y-2">
              <Label htmlFor="endMonth">终止月份</Label>
              <Input
                id="endMonth"
                type="month"
                value={filters.endMonth}
                onChange={(e) => setFilters({ ...filters, endMonth: e.target.value })}
              />
            </div>
            <div className="space-y-2">
              <Label htmlFor="region">地域</Label>
              <Select
                value={filters.region || undefined}
                onValueChange={(value) =>
                  setFilters({ ...filters, region: value === 'all' ? '' : value })
                }
              >
                <SelectTrigger id="region">
                  <SelectValue placeholder="全部地域" />
                </SelectTrigger>
                <SelectContent>
                  <SelectItem value="all">全部地域</SelectItem>
                  <SelectItem value="北京市">北京市</SelectItem>
                  <SelectItem value="上海市">上海市</SelectItem>
                  <SelectItem value="广东省">广东省</SelectItem>
                  <SelectItem value="浙江省">浙江省</SelectItem>
                  <SelectItem value="江苏省">江苏省</SelectItem>
                  <SelectItem value="四川省">四川省</SelectItem>
                  <SelectItem value="湖北省">湖北省</SelectItem>
                  <SelectItem value="湖南省">湖南省</SelectItem>
                  <SelectItem value="河南省">河南省</SelectItem>
                  <SelectItem value="山东省">山东省</SelectItem>
                  <SelectItem value="其他">其他</SelectItem>
                </SelectContent>
              </Select>
            </div>
            <div className="space-y-2">
              <Label htmlFor="serviceType">服务类型</Label>
              <Select
                value={filters.serviceType || undefined}
                onValueChange={(value) =>
                  setFilters({ ...filters, serviceType: value === 'all' ? '' : value })
                }
              >
                <SelectTrigger id="serviceType">
                  <SelectValue placeholder="全部类型" />
                </SelectTrigger>
                <SelectContent>
                  <SelectItem value="all">全部类型</SelectItem>
                  {serviceTypes.map((type) => (
                    <SelectItem key={type.id} value={type.id}>
                      {type.name}
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>
            </div>
            <div className="flex items-end">
              <Button onClick={handleApplyFilters} className="w-full">
                应用筛选
              </Button>
            </div>
          </div>
        </CardContent>
      </Card>

      {isLoading ? (
        <div className="space-y-4">
          {[...Array(4)].map((_, i) => (
            <Skeleton key={i} className="h-32 w-full" />
          ))}
        </div>
      ) : statisticsData ? (
        <>
          {/* Summary Cards */}
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
            <Card>
              <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
                <CardTitle className="text-sm font-medium">总需求数</CardTitle>
                <TrendingUp className="h-4 w-4 text-muted-foreground" />
              </CardHeader>
              <CardContent>
                <div className="text-2xl font-bold">{statisticsData.summary.totalNeeds}</div>
                <p className="text-xs text-muted-foreground mt-1">
                  已完成: {statisticsData.summary.totalCompleted}
                </p>
              </CardContent>
            </Card>

            <Card>
              <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
                <CardTitle className="text-sm font-medium">总响应数</CardTitle>
                <Activity className="h-4 w-4 text-muted-foreground" />
              </CardHeader>
              <CardContent>
                <div className="text-2xl font-bold">{statisticsData.summary.totalResponses}</div>
                <p className="text-xs text-muted-foreground mt-1">
                  已接受: {statisticsData.summary.totalAccepted}
                </p>
              </CardContent>
            </Card>

            <Card>
              <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
                <CardTitle className="text-sm font-medium">平均完成率</CardTitle>
                <CheckCircle className="h-4 w-4 text-muted-foreground" />
              </CardHeader>
              <CardContent>
                <div className="text-2xl font-bold">
                  {statisticsData.summary.overallCompletionRate != null
                    ? statisticsData.summary.overallCompletionRate.toFixed(1)
                    : '0.0'}%
                </div>
                <p className="text-xs text-muted-foreground mt-1">
                  {statisticsData.summary.totalCompleted}/{statisticsData.summary.totalNeeds}
                </p>
              </CardContent>
            </Card>

            <Card>
              <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
                <CardTitle className="text-sm font-medium">活跃用户</CardTitle>
                <Users className="h-4 w-4 text-muted-foreground" />
              </CardHeader>
              <CardContent>
                <div className="text-2xl font-bold">{statisticsData.summary.activeUsers}</div>
                <p className="text-xs text-muted-foreground mt-1">
                  新增: {statisticsData.summary.newUsers}
                </p>
              </CardContent>
            </Card>
          </div>

          {/* Charts */}
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
            <StatisticsChart
              title="需求发布趋势"
              description="每月需求发布数量变化"
              data={statisticsData.charts.needsTrend}
              type="line"
              dataKeys={['count']}
              xAxisKey="month"
            />

            <StatisticsChart
              title="完成率趋势"
              description="每月需求完成率变化"
              data={statisticsData.charts.completionRateTrend}
              type="line"
              dataKeys={['rate']}
              xAxisKey="month"
              colors={['#10B981']}
            />
          </div>

          <StatisticsChart
            title="服务类型分布"
            description="各类服务需求占比"
            data={statisticsData.charts.serviceTypeDistribution.map((item) => ({
              name: item.serviceType,
              value: item.count,
            }))}
            type="pie"
          />

          {/* Detailed Statistics Table */}
          <StatisticsTable
            title="详细统计数据"
            description="按月份和服务类型统计"
            columns={[
              { key: 'month', label: '月份', align: 'left' },
              { key: 'serviceTypeName', label: '服务类型', align: 'left' },
              { key: 'needsPublished', label: '发布需求', align: 'right' },
              { key: 'responsesReceived', label: '收到响应', align: 'right' },
              { key: 'needsAccepted', label: '已接受', align: 'right' },
              { key: 'needsCompleted', label: '已完成', align: 'right' },
              {
                key: 'completionRate',
                label: '完成率',
                align: 'right',
                format: (value: number) => value != null ? `${value.toFixed(1)}%` : '0.0%',
              },
              {
                key: 'avgResponseCount',
                label: '平均响应数',
                align: 'right',
                format: (value: number) => value != null ? value.toFixed(1) : '0.0',
              },
              {
                key: 'avgResponseTime',
                label: '平均响应时间(小时)',
                align: 'right',
                format: (value: number) => value != null ? value.toFixed(1) : '0.0',
              },
            ]}
            data={statisticsData.statistics}
          />
        </>
      ) : (
        <Card>
          <CardContent className="text-center py-8 text-muted-foreground">
            暂无统计数据
          </CardContent>
        </Card>
      )}
    </div>
  );
}
