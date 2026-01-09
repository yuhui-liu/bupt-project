'use client';

import { useState, useEffect } from 'react';
import { useRouter } from 'next/navigation';
import Link from 'next/link';
import { Button } from '@/components/ui/button';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select';
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
} from '@/components/ui/dialog';
import { Textarea } from '@/components/ui/textarea';
import { Label } from '@/components/ui/label';
import NeedList from '@/components/needs/NeedList';
import ResponseList from '@/components/needs/ResponseList';
import { api, buildQueryString } from '@/lib/api';
import { Need, ServiceResponse, PaginatedResponse, NeedStatus, ServiceType } from '@/types';
import { ROUTES, needStatusConfig } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { Plus, Filter } from 'lucide-react';
import {
  Pagination,
  PaginationContent,
  PaginationItem,
  PaginationLink,
  PaginationNext,
  PaginationPrevious,
} from '@/components/ui/pagination';

export default function MyNeedsPage() {
  const router = useRouter();
  const { toast } = useToast();

  const [needs, setNeeds] = useState<Need[]>([]);
  const [serviceTypes, setServiceTypes] = useState<ServiceType[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [pagination, setPagination] = useState({ page: 1, pageSize: 10, total: 0 });
  const [filters, setFilters] = useState<{ status: NeedStatus | ''; serviceType: string }>({
    status: '',
    serviceType: '',
  });

  // Cancel/Complete dialogs
  const [cancelDialog, setCancelDialog] = useState<{ open: boolean; needId: string; reason: string }>({
    open: false,
    needId: '',
    reason: '',
  });
  const [completeDialog, setCompleteDialog] = useState<{ open: boolean; needId: string }>({
    open: false,
    needId: '',
  });

  // View responses dialog
  const [responsesDialog, setResponsesDialog] = useState<{
    open: boolean;
    needId: string;
    responses: ServiceResponse[];
    loading: boolean;
  }>({
    open: false,
    needId: '',
    responses: [],
    loading: false,
  });

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
        status: filters.status,
        serviceType: filters.serviceType,
      });

      const data = await api.get<PaginatedResponse<Need>>(`/needs/my-needs${queryString}`);
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

  const handleEdit = (id: string) => {
    router.push(ROUTES.EDIT_NEED(id));
  };

  const handleDelete = async (id: string) => {
    if (!confirm('确定要删除这个需求吗？删除后无法恢复。')) return;

    try {
      await api.delete(`/needs/${id}`);
      toast({
        title: '删除成功',
        description: '需求已成功删除',
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

  const handleCancelClick = (id: string) => {
    setCancelDialog({ open: true, needId: id, reason: '' });
  };

  const handleCancelSubmit = async () => {
    try {
      await api.post(`/needs/${cancelDialog.needId}/cancel`, {
        reason: cancelDialog.reason || undefined,
      });
      toast({
        title: '取消成功',
        description: '需求已取消',
      });
      setCancelDialog({ open: false, needId: '', reason: '' });
      fetchNeeds();
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '取消失败',
        description: error.message || '无法取消需求',
      });
    }
  };

  const handleCompleteClick = (id: string) => {
    setCompleteDialog({ open: true, needId: id });
  };

  const handleCompleteSubmit = async () => {
    try {
      await api.post(`/needs/${completeDialog.needId}/complete`);
      toast({
        title: '标记成功',
        description: '需求已标记为完成',
      });
      setCompleteDialog({ open: false, needId: '' });
      fetchNeeds();
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '操作失败',
        description: error.message || '无法标记为完成',
      });
    }
  };

  const handleViewResponses = async (needId: string) => {
    setResponsesDialog({ open: true, needId, responses: [], loading: true });

    try {
      const data = await api.get<PaginatedResponse<ServiceResponse>>(
        `/needs/${needId}/responses?page=1&pageSize=100`
      );
      setResponsesDialog((prev) => ({
        ...prev,
        responses: data.items,
        loading: false,
      }));
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载响应列表',
      });
      setResponsesDialog((prev) => ({ ...prev, loading: false }));
    }
  };

  const handleAcceptResponse = async (responseId: string) => {
    if (!confirm('确定接受此响应吗？接受后其他响应将自动被拒绝。')) return;

    try {
      await api.post(`/needs/${responsesDialog.needId}/responses/${responseId}/accept`);
      toast({
        title: '接受成功',
        description: '已接受此响应，其他响应已自动拒绝',
      });
      setResponsesDialog({ open: false, needId: '', responses: [], loading: false });
      fetchNeeds();
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '操作失败',
        description: error.message || '无法接受响应',
      });
    }
  };

  const handleRejectResponse = async (responseId: string) => {
    if (!confirm('确定拒绝此响应吗？')) return;

    try {
      await api.post(`/needs/${responsesDialog.needId}/responses/${responseId}/reject`);
      toast({
        title: '已拒绝',
        description: '已拒绝此响应',
      });
      // Refresh responses
      handleViewResponses(responsesDialog.needId);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '操作失败',
        description: error.message || '无法拒绝响应',
      });
    }
  };

  const totalPages = Math.ceil(pagination.total / pagination.pageSize);

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="flex items-center justify-between">
        <h1 className="text-3xl font-bold">我的需求</h1>
        <Link href={ROUTES.CREATE_NEED}>
          <Button>
            <Plus className="mr-2 h-4 w-4" />
            发布需求
          </Button>
        </Link>
      </div>

      {/* Filters */}
      <div className="flex gap-4 items-center flex-wrap">
        <div className="flex items-center gap-2">
          <Filter className="h-4 w-4" />
          <span className="text-sm font-medium">筛选：</span>
        </div>

        <Select
          value={filters.status || undefined}
          onValueChange={(value) =>
            setFilters({ ...filters, status: value === 'all' ? '' : (value as NeedStatus | '') })
          }
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
          onValueChange={(value) => setFilters({ ...filters, serviceType: value === 'all' ? '' : value })}
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

        {(filters.status || filters.serviceType) && (
          <Button
            variant="ghost"
            size="sm"
            onClick={() => setFilters({ status: '', serviceType: '' })}
          >
            清除筛选
          </Button>
        )}
      </div>

      {/* Needs List */}
      <NeedList
        needs={needs}
        isLoading={isLoading}
        showActions
        emptyMessage="您还没有发布任何需求"
        onEdit={handleEdit}
        onDelete={handleDelete}
        onCancel={handleCancelClick}
        onComplete={handleCompleteClick}
        onViewResponses={handleViewResponses}
      />

      {/* Pagination */}
      {totalPages > 1 && (
        <Pagination>
          <PaginationContent>
            <PaginationItem>
              <PaginationPrevious
                onClick={() => pagination.page > 1 && setPagination({ ...pagination, page: pagination.page - 1 })}
                className={pagination.page === 1 ? 'pointer-events-none opacity-50' : 'cursor-pointer'}
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
                onClick={() => pagination.page < totalPages && setPagination({ ...pagination, page: pagination.page + 1 })}
                className={pagination.page === totalPages ? 'pointer-events-none opacity-50' : 'cursor-pointer'}
              />
            </PaginationItem>
          </PaginationContent>
        </Pagination>
      )}

      {/* Cancel Dialog */}
      <Dialog open={cancelDialog.open} onOpenChange={(open) => setCancelDialog({ ...cancelDialog, open })}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>取消需求</DialogTitle>
            <DialogDescription>请说明取消原因（可选）</DialogDescription>
          </DialogHeader>
          <div className="space-y-4 py-4">
            <div className="space-y-2">
              <Label htmlFor="reason">取消原因</Label>
              <Textarea
                id="reason"
                value={cancelDialog.reason}
                onChange={(e) => setCancelDialog({ ...cancelDialog, reason: e.target.value })}
                placeholder="例如：已找到其他服务提供者"
                rows={3}
              />
            </div>
          </div>
          <DialogFooter>
            <Button variant="outline" onClick={() => setCancelDialog({ ...cancelDialog, open: false })}>
              取消
            </Button>
            <Button onClick={handleCancelSubmit}>确认取消需求</Button>
          </DialogFooter>
        </DialogContent>
      </Dialog>

      {/* Complete Dialog */}
      <Dialog open={completeDialog.open} onOpenChange={(open) => setCompleteDialog({ ...completeDialog, open })}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>标记完成</DialogTitle>
            <DialogDescription>
              确认标记此需求为已完成吗？
            </DialogDescription>
          </DialogHeader>
          <DialogFooter>
            <Button variant="outline" onClick={() => setCompleteDialog({ ...completeDialog, open: false })}>
              取消
            </Button>
            <Button onClick={handleCompleteSubmit}>确认完成</Button>
          </DialogFooter>
        </DialogContent>
      </Dialog>

      {/* View Responses Dialog */}
      <Dialog
        open={responsesDialog.open}
        onOpenChange={(open) => setResponsesDialog({ ...responsesDialog, open })}
      >
        <DialogContent className="max-w-4xl max-h-[80vh] overflow-y-auto">
          <DialogHeader>
            <DialogTitle>查看响应</DialogTitle>
            <DialogDescription>所有对此需求的服务响应</DialogDescription>
          </DialogHeader>
          <div className="py-4">
            <ResponseList
              responses={responsesDialog.responses}
              isLoading={responsesDialog.loading}
              showActions
              emptyMessage="暂无响应"
              onAccept={handleAcceptResponse}
              onReject={handleRejectResponse}
            />
          </div>
        </DialogContent>
      </Dialog>
    </div>
  );
}
