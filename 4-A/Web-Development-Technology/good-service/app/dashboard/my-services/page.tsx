'use client';

import { useState, useEffect } from 'react';
import { useRouter } from 'next/navigation';
import { Button } from '@/components/ui/button';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select';
import ServiceCard from '@/components/services/ServiceCard';
import { api, buildQueryString } from '@/lib/api';
import { ServiceResponse, PaginatedResponse, ResponseStatus } from '@/types';
import { ROUTES, responseStatusConfig } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { Filter } from 'lucide-react';
import { Alert, AlertDescription } from '@/components/ui/alert';
import { Skeleton } from '@/components/ui/skeleton';
import {
  Pagination,
  PaginationContent,
  PaginationItem,
  PaginationLink,
  PaginationNext,
  PaginationPrevious,
} from '@/components/ui/pagination';

export default function MyServicesPage() {
  const router = useRouter();
  const { toast } = useToast();

  const [responses, setResponses] = useState<ServiceResponse[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [pagination, setPagination] = useState({ page: 1, pageSize: 10, total: 0 });
  const [statusFilter, setStatusFilter] = useState<ResponseStatus | ''>('');

  useEffect(() => {
    fetchResponses();
  }, [pagination.page, statusFilter]);

  const fetchResponses = async () => {
    setIsLoading(true);
    try {
      const queryString = buildQueryString({
        page: pagination.page,
        pageSize: pagination.pageSize,
        status: statusFilter,
      });

      const data = await api.get<PaginatedResponse<ServiceResponse>>(
        `/services/my-responses${queryString}`
      );
      setResponses(data.items);
      setPagination((prev) => ({ ...prev, total: data.total }));
    } catch (error) {
      console.error('Failed to fetch responses:', error);
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: '无法加载服务响应列表',
      });
    } finally {
      setIsLoading(false);
    }
  };

  const handleEdit = (id: string) => {
    router.push(`${ROUTES.MY_SERVICES}/${id}/edit`);
  };

  const handleDelete = async (id: string) => {
    if (!confirm('确定要删除这个服务响应吗？删除后无法恢复。')) return;

    try {
      await api.delete(`/services/responses/${id}`);
      toast({
        title: '删除成功',
        description: '服务响应已成功删除',
      });
      fetchResponses();
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '删除失败',
        description: error.message || '无法删除服务响应',
      });
    }
  };

  const totalPages = Math.ceil(pagination.total / pagination.pageSize);

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="flex items-center justify-between">
        <h1 className="text-3xl font-bold">我的服务响应</h1>
      </div>

      {/* Filters */}
      <div className="flex gap-4 items-center flex-wrap">
        <div className="flex items-center gap-2">
          <Filter className="h-4 w-4" />
          <span className="text-sm font-medium">筛选：</span>
        </div>

        <Select
          value={statusFilter || undefined}
          onValueChange={(value) => setStatusFilter(value === 'all' ? '' : (value as ResponseStatus | ''))}
        >
          <SelectTrigger className="w-[150px]">
            <SelectValue placeholder="全部状态" />
          </SelectTrigger>
          <SelectContent>
            <SelectItem value="all">全部状态</SelectItem>
            {Object.entries(responseStatusConfig).map(([key, config]) => (
              <SelectItem key={key} value={key}>
                {config.label}
              </SelectItem>
            ))}
          </SelectContent>
        </Select>

        {statusFilter && (
          <Button variant="ghost" size="sm" onClick={() => setStatusFilter('')}>
            清除筛选
          </Button>
        )}
      </div>

      {/* Responses List */}
      {isLoading ? (
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
          {[...Array(6)].map((_, i) => (
            <Skeleton key={i} className="h-64 w-full" />
          ))}
        </div>
      ) : responses.length === 0 ? (
        <Alert>
          <AlertDescription className="text-center py-8">
            您还没有提交任何服务响应
          </AlertDescription>
        </Alert>
      ) : (
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
          {responses.map((response) => (
            <ServiceCard
              key={response.id}
              response={response}
              showActions
              onEdit={handleEdit}
              onDelete={handleDelete}
            />
          ))}
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

            {[...Array(totalPages)].map((_, i) => (
              <PaginationItem key={i}>
                <PaginationLink
                  onClick={() => setPagination({ ...pagination, page: i + 1 })}
                  isActive={pagination.page === i + 1}
                  className="cursor-pointer"
                >
                  {i + 1}
                </PaginationLink>
              </PaginationItem>
            ))}

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
