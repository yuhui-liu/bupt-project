'use client';

import { useState, useEffect } from 'react';
import { useParams, useRouter } from 'next/navigation';
import { Button } from '@/components/ui/button';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import { Skeleton } from '@/components/ui/skeleton';
import ServiceForm from '@/components/services/ServiceForm';
import { api } from '@/lib/api';
import { Need } from '@/types';
import { ROUTES, SUCCESS_MESSAGES } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { ArrowLeft } from 'lucide-react';
import Link from 'next/link';

export default function RespondNeedPage() {
  const params = useParams();
  const router = useRouter();
  const { toast } = useToast();
  const needId = params.id as string;

  const [need, setNeed] = useState<Need | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    fetchNeed();
  }, [needId]);

  const fetchNeed = async () => {
    try {
      const data = await api.get<Need>(`/needs/${needId}`);

      // Check if can respond
      if (data.hasResponded) {
        toast({
          variant: 'destructive',
          title: '无法响应',
          description: '您已经响应过此需求',
        });
        router.push(ROUTES.NEED_DETAIL(needId));
        return;
      }

      if (data.status !== 'open' && data.status !== 'responded') {
        toast({
          variant: 'destructive',
          title: '无法响应',
          description: '此需求当前状态不允许响应',
        });
        router.push(ROUTES.NEED_DETAIL(needId));
        return;
      }

      setNeed(data);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载需求信息',
      });
      router.push(ROUTES.BROWSE_NEEDS);
    } finally {
      setIsLoading(false);
    }
  };

  const handleSubmit = async (formData: FormData) => {
    try {
      await api.post(`/needs/${needId}/respond`, formData);
      toast({
        title: SUCCESS_MESSAGES.SUBMIT_SUCCESS,
        description: '服务响应已成功提交',
      });
      router.push(ROUTES.MY_SERVICES);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '提交失败',
        description: error.message || '无法提交服务响应，请稍后重试',
      });
      throw error;
    }
  };

  const handleCancel = () => {
    router.back();
  };

  if (isLoading) {
    return (
      <div className="max-w-4xl mx-auto space-y-6">
        <Skeleton className="h-10 w-48" />
        <Card>
          <CardHeader>
            <Skeleton className="h-6 w-32" />
          </CardHeader>
          <CardContent className="space-y-4">
            <Skeleton className="h-40 w-full" />
            <Skeleton className="h-40 w-full" />
          </CardContent>
        </Card>
      </div>
    );
  }

  if (!need) {
    return null;
  }

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      {/* Header */}
      <div className="flex items-center gap-4">
        <Link href={ROUTES.NEED_DETAIL(needId)}>
          <Button variant="ghost" size="icon">
            <ArrowLeft className="h-5 w-5" />
          </Button>
        </Link>
        <h1 className="text-3xl font-bold">响应服务</h1>
      </div>

      {/* Form Card */}
      <Card>
        <CardHeader>
          <CardTitle>提交服务响应</CardTitle>
        </CardHeader>
        <CardContent>
          <ServiceForm
            onSubmit={handleSubmit}
            onCancel={handleCancel}
            needInfo={{
              title: need.title,
              description: need.description,
              publisher: need.publisher?.username,
            }}
          />
        </CardContent>
      </Card>
    </div>
  );
}
