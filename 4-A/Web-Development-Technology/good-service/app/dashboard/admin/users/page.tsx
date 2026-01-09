'use client';

import { useState, useEffect } from 'react';
import { useRouter } from 'next/navigation';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Badge } from '@/components/ui/badge';
import { Card } from '@/components/ui/card';
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
import { User, PaginatedResponse, UserRole } from '@/types';
import { useToast } from '@/hooks/use-toast';
import { useAuth } from '@/contexts/AuthContext';
import { Search, Filter, UserCheck, UserX } from 'lucide-react';
import {
  Pagination,
  PaginationContent,
  PaginationItem,
  PaginationLink,
  PaginationNext,
  PaginationPrevious,
} from '@/components/ui/pagination';

export default function AdminUsersPage() {
  const router = useRouter();
  const { toast } = useToast();
  const { user, isAdmin } = useAuth();

  const [users, setUsers] = useState<User[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [pagination, setPagination] = useState({ page: 1, pageSize: 20, total: 0 });
  const [filters, setFilters] = useState<{ keyword: string; role: UserRole | '' }>({
    keyword: '',
    role: '',
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
    fetchUsers();
  }, [pagination.page, filters, isAdmin]);

  const fetchUsers = async () => {
    setIsLoading(true);
    try {
      const queryString = buildQueryString({
        page: pagination.page,
        pageSize: pagination.pageSize,
        keyword: filters.keyword,
        role: filters.role,
      });

      const data = await api.get<PaginatedResponse<User>>(`/admin/users${queryString}`);
      setUsers(data.items);
      setPagination((prev) => ({ ...prev, total: data.total }));
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载用户列表',
      });
    } finally {
      setIsLoading(false);
    }
  };

  const handleSearch = () => {
    setFilters({ ...filters, keyword: searchInput });
    setPagination({ ...pagination, page: 1 });
  };

  const handleToggleStatus = async (userId: string, currentStatus: string) => {
    const newStatus = currentStatus === 'active' ? 'disabled' : 'active';
    const action = newStatus === 'active' ? '启用' : '禁用';

    if (!confirm(`确定${action}此用户吗？`)) return;

    try {
      await api.put(`/admin/users/${userId}/status`, { status: newStatus });
      toast({
        title: '操作成功',
        description: `用户已${action}`,
      });
      fetchUsers();
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '操作失败',
        description: error.message || `无法${action}用户`,
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
        <h1 className="text-3xl font-bold">用户管理</h1>
        <p className="text-muted-foreground mt-2">查看和管理所有用户</p>
      </div>

      {/* Search and Filters */}
      <div className="space-y-4">
        {/* Search Bar */}
        <div className="flex gap-2">
          <div className="flex-1 relative">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-4 w-4 text-muted-foreground" />
            <Input
              placeholder="搜索用户名或手机号..."
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
            value={filters.role || undefined}
            onValueChange={(value) => {
              setFilters({ ...filters, role: value === 'all' ? '' : (value as UserRole | '') });
              setPagination({ ...pagination, page: 1 });
            }}
          >
            <SelectTrigger className="w-[150px]">
              <SelectValue placeholder="全部角色" />
            </SelectTrigger>
            <SelectContent>
              <SelectItem value="all">全部角色</SelectItem>
              <SelectItem value="user">普通用户</SelectItem>
              <SelectItem value="admin">管理员</SelectItem>
            </SelectContent>
          </Select>

          {(filters.keyword || filters.role) && (
            <Button
              variant="ghost"
              size="sm"
              onClick={() => {
                setFilters({ keyword: '', role: '' });
                setSearchInput('');
                setPagination({ ...pagination, page: 1 });
              }}
            >
              清除筛选
            </Button>
          )}
        </div>
      </div>

      {/* Users Table */}
      {isLoading ? (
        <div className="space-y-2">
          {[...Array(10)].map((_, i) => (
            <Skeleton key={i} className="h-16 w-full" />
          ))}
        </div>
      ) : users.length === 0 ? (
        <Alert>
          <AlertDescription className="text-center py-8">暂无用户</AlertDescription>
        </Alert>
      ) : (
        <div className="border rounded-lg">
          <Table>
            <TableHeader>
              <TableRow>
                <TableHead>用户名</TableHead>
                <TableHead>手机号</TableHead>
                <TableHead>邮箱</TableHead>
                <TableHead>角色</TableHead>
                <TableHead>状态</TableHead>
                <TableHead>注册时间</TableHead>
                <TableHead className="text-right">操作</TableHead>
              </TableRow>
            </TableHeader>
            <TableBody>
              {users.map((user) => (
                <TableRow key={user.id}>
                  <TableCell className="font-medium">{user.username}</TableCell>
                  <TableCell>{user.phone}</TableCell>
                  <TableCell>{user.email || '-'}</TableCell>
                  <TableCell>
                    <Badge variant={user.role === 'admin' ? 'default' : 'secondary'}>
                      {user.role === 'admin' ? '管理员' : '普通用户'}
                    </Badge>
                  </TableCell>
                  <TableCell>
                    <Badge variant={user.status === 'active' ? 'default' : 'destructive'}>
                      {user.status === 'active' ? '正常' : '已禁用'}
                    </Badge>
                  </TableCell>
                  <TableCell>
                    {new Date(user.createdAt).toLocaleDateString('zh-CN')}
                  </TableCell>
                  <TableCell className="text-right">
                    {user.role !== 'admin' && (
                      <Button
                        size="sm"
                        variant={user.status === 'active' ? 'outline' : 'default'}
                        onClick={() => handleToggleStatus(user.id, user.status)}
                      >
                        {user.status === 'active' ? (
                          <>
                            <UserX className="h-4 w-4 mr-1" />
                            禁用
                          </>
                        ) : (
                          <>
                            <UserCheck className="h-4 w-4 mr-1" />
                            启用
                          </>
                        )}
                      </Button>
                    )}
                  </TableCell>
                </TableRow>
              ))}
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

      {/* Statistics */}
      <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
        <Card className="p-4">
          <div className="text-sm text-muted-foreground">总用户数</div>
          <div className="text-2xl font-bold">{pagination.total}</div>
        </Card>
        <Card className="p-4">
          <div className="text-sm text-muted-foreground">当前页</div>
          <div className="text-2xl font-bold">
            {pagination.page} / {totalPages || 1}
          </div>
        </Card>
        <Card className="p-4">
          <div className="text-sm text-muted-foreground">每页显示</div>
          <div className="text-2xl font-bold">{pagination.pageSize}</div>
        </Card>
        <Card className="p-4">
          <div className="text-sm text-muted-foreground">本页用户</div>
          <div className="text-2xl font-bold">{users.length}</div>
        </Card>
      </div>
    </div>
  );
}
