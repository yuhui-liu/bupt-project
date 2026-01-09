'use client';

import { useState, useEffect } from 'react';
import { useRouter } from 'next/navigation';
import Link from 'next/link';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Badge } from '@/components/ui/badge';
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from '@/components/ui/table';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select';
import { Alert, AlertDescription } from '@/components/ui/alert';
import { Skeleton } from '@/components/ui/skeleton';
import { api, buildQueryString } from '@/lib/api';
import { Need, PaginatedResponse, NeedStatus, ServiceType } from '@/types';
import { ROUTES, needStatusConfig } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { useAuth } from '@/contexts/AuthContext';
import { Search, Filter, Trash2, Eye } from 'lucide-react';
import {
  Pagination,
  PaginationContent,
  PaginationItem,
  PaginationLink,
  PaginationNext,
  PaginationPrevious,
} from '@/components/ui/pagination';

export default function AdminNeedsPage() {
  const router = useRouter();
  const { toast } = useToast();
  const { isAdmin } = useAuth();

  const [needs, setNeeds] = useState<Need[]>([]);
  const [serviceTypes, setServiceTypes] = useState<ServiceType[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [pagination, setPagination] = useState({ page: 1, pageSize: 20, total: 0 });
  const [filters, setFilters] = useState<{
    keyword: string;
    status: NeedStatus | '';
    serviceType: string;
  }>({
    keyword: '',
    status: '',
    serviceType: '',
  });
  const [searchInput, setSearchInput] = useState('');

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
    fetchNeeds();
    fetchServiceTypes();
  }, [pagination.page, filters, isAdmin]);

  const fetchNeeds = async () => {
    setIsLoading(true);
    try {
      const queryString = buildQueryString({
        page: pagination.page,
        pageSize: pagination.pageSize,
        keyword: filters.keyword,
        status: filters.status,
        serviceType: filters.serviceType,
      });

      const data = await api.get<PaginatedResponse<Need>>(`/admin/needs${queryString}`);
      setNeeds(data.items);
      setPagination((prev) => ({ ...prev, total: data.total }));
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载需求列表',
      });
    } finally {
      setIsLoading(false);
    }
  };

  const fetchServiceTypes = async () => {
    try {
      const data = await api.get<ServiceType[]>('/service-types');
      setServiceTypes(data);
    } catch (error) {
      console.error('Failed to fetch service types:', error);
    }
  };

  const handleSearch = () => {
    setFilters({ ...filters, keyword: searchInput });
    setPagination({ ...pagination, page: 1 });
  };

  const handleDelete = async (needId: string) => {
    if (!confirm('确定要删除这个需求吗？此操作不可恢复。')) return;

    try {
      await api.delete(`/admin/needs/${needId}`);
      toast({
        title: '删除成功',
        description: '需求已被删除',
      });
      fetchNeeds();
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '删除失败',
        description: error.message || '无法删除需求',
      });
    }
  };

  const totalPages = Math.ceil(pagination.total / pagination.pageSize);

  if (!isAdmin) {
    return null;
  }

  return (
    <div className="space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold">需求管理</h1>
        <p className="text-muted-foreground mt-2">查看和管理所有需求</p>
      </div>

      {/* Search and Filters */}
      <div className="space-y-4">
        <div className="flex gap-2">
          <div className="flex-1 relative">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-4 w-4 text-muted-foreground" />
            <Input
              placeholder="搜索需求标题或发布者..."
              value={searchInput}
              onChange={(e) => setSearchInput(e.target.value)}
              onKeyPress={(e) => e.key === 'Enter' && handleSearch()}
              className="pl-10"
            />
          </div>
          <Button onClick={handleSearch}>搜索</Button>
        </div>

        <div className="flex gap-4 items-center flex-wrap">
          <div className="flex items-center gap-2">
            <Filter className="h-4 w-4" />
            <span className="text-sm font-medium">筛选：</span>
          </div>

          <Select
            value={filters.status || undefined}
            onValueChange={(value) => {
              setFilters({ ...filters, status: value === 'all' ? '' : (value as NeedStatus | '') });
              setPagination({ ...pagination, page: 1 });
            }}
          >
            <SelectTrigger className="w-[150px]">
              <SelectValue placeholder="全部状态" />
            </SelectTrigger>
            <SelectContent>
              <SelectItem value="all">全部状态</SelectItem>
              {Object.entries(needStatusConfig).map(([key, config]) => (
                <SelectItem key={key} value={key}>
                  {config.label}
                </SelectItem>
              ))}
            </SelectContent>
          </Select>

          <Select
            value={filters.serviceType || undefined}
            onValueChange={(value) => {
              setFilters({ ...filters, serviceType: value === 'all' ? '' : value });
              setPagination({ ...pagination, page: 1 });
            }}
          >
            <SelectTrigger className="w-[150px]">
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

          {(filters.keyword || filters.status || filters.serviceType) && (
            <Button
              variant="ghost"
              size="sm"
              onClick={() => {
                setFilters({ keyword: '', status: '', serviceType: '' });
                setSearchInput('');
                setPagination({ ...pagination, page: 1 });
              }}
            >
              清除筛选
            </Button>
          )}
        </div>
      </div>

      {/* Needs Table */}
      {isLoading ? (
        <div className="space-y-2">
          {[...Array(10)].map((_, i) => (
            <Skeleton key={i} className="h-16 w-full" />
          ))}
        </div>
      ) : needs.length === 0 ? (
        <Alert>
          <AlertDescription className="text-center py-8">暂无需求</AlertDescription>
        </Alert>
      ) : (
        <div className="border rounded-lg">
          <Table>
            <TableHeader>
              <TableRow>
                <TableHead>标题</TableHead>
                <TableHead>发布者</TableHead>
                <TableHead>服务类型</TableHead>
                <TableHead>状态</TableHead>
                <TableHead>响应数</TableHead>
                <TableHead>发布时间</TableHead>
                <TableHead className="text-right">操作</TableHead>
              </TableRow>
            </TableHeader>
            <TableBody>
              {needs.map((need) => {
                const statusConfig = needStatusConfig[need.status];
                return (
                  <TableRow key={need.id}>
                    <TableCell className="font-medium max-w-xs truncate">
                      {need.title}
                    </TableCell>
                    <TableCell>
                      {need.publisher?.username || '-'}
                    </TableCell>
                    <TableCell>
                      <Badge variant="outline">
                        {need.serviceTypeName || need.serviceType}
                      </Badge>
                    </TableCell>
                    <TableCell>
                      <Badge className={`${statusConfig.bgColor} ${statusConfig.color} border-0`}>
                        {statusConfig.label}
                      </Badge>
                    </TableCell>
                    <TableCell>{need.responseCount || 0}</TableCell>
                    <TableCell>
                      {new Date(need.createdAt).toLocaleDateString('zh-CN')}
                    </TableCell>
                    <TableCell className="text-right space-x-2">
                      <Link href={ROUTES.NEED_DETAIL(need.id)}>
                        <Button size="sm" variant="outline">
                          <Eye className="h-4 w-4" />
                        </Button>
                      </Link>
                      <Button
                        size="sm"
                        variant="destructive"
                        onClick={() => handleDelete(need.id)}
                      >
                        <Trash2 className="h-4 w-4" />
                      </Button>
                    </TableCell>
                  </TableRow>
                );
              })}
            </TableBody>
          </Table>
        </div>
      )}

      {/* Pagination */}
      {totalPages > 1 && (
        <Pagination>
          <PaginationContent>
            <PaginationItem>
              <PaginationPrevious
                onClick={() =>
                  pagination.page > 1 &&
                  setPagination({ ...pagination, page: pagination.page - 1 })
                }
                className={
                  pagination.page === 1 ? 'pointer-events-none opacity-50' : 'cursor-pointer'
                }
              />
            </PaginationItem>

            {[...Array(Math.min(totalPages, 7))].map((_, i) => {
              let pageNum: number;
              if (totalPages <= 7) {
                pageNum = i + 1;
              } else if (pagination.page <= 4) {
                pageNum = i + 1;
              } else if (pagination.page >= totalPages - 3) {
                pageNum = totalPages - 6 + i;
              } else {
                pageNum = pagination.page - 3 + i;
              }

              return (
                <PaginationItem key={i}>
                  <PaginationLink
                    onClick={() => setPagination({ ...pagination, page: pageNum })}
                    isActive={pagination.page === pageNum}
                    className="cursor-pointer"
                  >
                    {pageNum}
                  </PaginationLink>
                </PaginationItem>
              );
            })}

            <PaginationItem>
              <PaginationNext
                onClick={() =>
                  pagination.page < totalPages &&
                  setPagination({ ...pagination, page: pagination.page + 1 })
                }
                className={
                  pagination.page === totalPages
                    ? 'pointer-events-none opacity-50'
                    : 'cursor-pointer'
                }
              />
            </PaginationItem>
          </PaginationContent>
        </Pagination>
      )}
    </div>
  );
}
