'use client';

import { useState, useEffect } from 'react';
import { useRouter } from 'next/navigation';
import Link from 'next/link';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select';
import NeedList from '@/components/needs/NeedList';
import { api, buildQueryString } from '@/lib/api';
import { Need, PaginatedResponse, ServiceType } from '@/types';
import { ROUTES } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { Search, Filter } from 'lucide-react';
import {
  Pagination,
  PaginationContent,
  PaginationItem,
  PaginationLink,
  PaginationNext,
  PaginationPrevious,
} from '@/components/ui/pagination';

export default function BrowseNeedsPage() {
  const router = useRouter();
  const { toast } = useToast();

  const [needs, setNeeds] = useState<Need[]>([]);
  const [serviceTypes, setServiceTypes] = useState<ServiceType[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [pagination, setPagination] = useState({ page: 1, pageSize: 10, total: 0 });
  const [filters, setFilters] = useState<{
    keyword: string;
    serviceType: string;
    sortBy: 'createdAt' | 'responseCount';
  }>({
    keyword: '',
    serviceType: '',
    sortBy: 'createdAt',
  });
  const [searchInput, setSearchInput] = useState('');

  useEffect(() => {
    fetchNeeds();
    fetchServiceTypes();
  }, [pagination.page, filters]);

  const fetchNeeds = async () => {
    setIsLoading(true);
    try {
      const queryString = buildQueryString({
        page: pagination.page,
        pageSize: pagination.pageSize,
        keyword: filters.keyword,
        serviceType: filters.serviceType,
        sortBy: filters.sortBy,
        status: 'open,responded',
      });

      const data = await api.get<PaginatedResponse<Need>>(`/needs${queryString}`);
      setNeeds(data.items);
      setPagination((prev) => ({ ...prev, total: data.total }));
    } catch (error) {
      console.error('Failed to fetch needs:', error);
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: '无法加载需求列表',
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

  const handleNeedClick = (needId: string) => {
    router.push(ROUTES.NEED_DETAIL(needId));
  };

  const totalPages = Math.ceil(pagination.total / pagination.pageSize);

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="flex items-center justify-between">
        <h1 className="text-3xl font-bold">浏览需求</h1>
      </div>

      {/* Search and Filters */}
      <div className="space-y-4">
        {/* Search Bar */}
        <div className="flex gap-2">
          <div className="flex-1 relative">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-4 w-4 text-muted-foreground" />
            <Input
              placeholder="搜索需求标题或描述..."
              value={searchInput}
              onChange={(e) => setSearchInput(e.target.value)}
              onKeyPress={(e) => e.key === 'Enter' && handleSearch()}
              className="pl-10"
            />
          </div>
          <Button onClick={handleSearch}>搜索</Button>
        </div>

        {/* Filters */}
        <div className="flex gap-4 items-center flex-wrap">
          <div className="flex items-center gap-2">
            <Filter className="h-4 w-4" />
            <span className="text-sm font-medium">筛选：</span>
          </div>

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

          <Select
            value={filters.sortBy}
            onValueChange={(value: 'createdAt' | 'responseCount') => {
              setFilters({ ...filters, sortBy: value });
              setPagination({ ...pagination, page: 1 });
            }}
          >
            <SelectTrigger className="w-[150px]">
              <SelectValue />
            </SelectTrigger>
            <SelectContent>
              <SelectItem value="createdAt">最新发布</SelectItem>
              <SelectItem value="responseCount">响应最少</SelectItem>
            </SelectContent>
          </Select>

          {(filters.keyword || filters.serviceType) && (
            <Button
              variant="ghost"
              size="sm"
              onClick={() => {
                setFilters({ ...filters, keyword: '', serviceType: '' });
                setSearchInput('');
                setPagination({ ...pagination, page: 1 });
              }}
            >
              清除筛选
            </Button>
          )}
        </div>
      </div>

      {/* Needs List */}
      <div
        className="cursor-pointer"
        onClick={(e) => {
          const target = e.target as HTMLElement;
          const card = target.closest('[data-need-id]');
          if (card) {
            const needId = card.getAttribute('data-need-id');
            if (needId) handleNeedClick(needId);
          }
        }}
      >
        <NeedList
          needs={needs}
          isLoading={isLoading}
          emptyMessage="暂无需求"
        />
      </div>

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
