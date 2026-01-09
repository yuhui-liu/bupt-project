'use client';

import { useState, useEffect } from 'react';
import { useRouter, useParams } from 'next/navigation';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import { Skeleton } from '@/components/ui/skeleton';
import ServiceForm from '@/components/services/ServiceForm';
import { api } from '@/lib/api';
import { ServiceResponse } from '@/types';
import { ROUTES, SUCCESS_MESSAGES } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { ArrowLeft } from 'lucide-react';
import Link from 'next/link';
import { Button } from '@/components/ui/button';

export default function EditServiceResponsePage() {
  const router = useRouter();
  const params = useParams();
  const { toast } = useToast();
  const responseId = params.id as string;

  const [response, setResponse] = useState<ServiceResponse | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    fetchResponse();
  }, [responseId]);

  const fetchResponse = async () => {
    try {
      const data = await api.get<ServiceResponse>(`/services/responses/${responseId}`);

      // Check if can edit
      if (data.status !== 'pending') {
        toast({
          variant: 'destructive',
          title: '无法编辑',
          description: '只能编辑待处理状态的响应',
        });
        router.push(ROUTES.MY_SERVICES);
        return;
      }

      setResponse(data);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载响应信息',
      });
      router.push(ROUTES.MY_SERVICES);
    } finally {
      setIsLoading(false);
    }
  };

  const handleSubmit = async (formData: FormData) => {
    try {
      await api.put(`/services/responses/${responseId}`, formData);
      toast({
        title: SUCCESS_MESSAGES.UPDATE_SUCCESS,
        description: '服务响应已成功更新',
      });
      router.push(ROUTES.MY_SERVICES);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '更新失败',
        description: error.message || '无法更新响应，请稍后重试',
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
            <Skeleton className="h-40 w-full" />
          </CardContent>
        </Card>
      </div>
    );
  }

  if (!response) {
    return null;
  }

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      {/* Header */}
      <div className="flex items-center gap-4">
        <Link href={ROUTES.MY_SERVICES}>
          <Button variant="ghost" size="icon">
            <ArrowLeft className="h-5 w-5" />
          </Button>
        </Link>
        <h1 className="text-3xl font-bold">编辑服务响应</h1>
      </div>

      {/* Form Card */}
      <Card>
        <CardHeader>
          <CardTitle>响应信息</CardTitle>
        </CardHeader>
        <CardContent>
          <ServiceForm
            initialData={{
              description: response.description,
              price: response.price,
              availableTime: response.availableTime,
              existingImages: response.images,
              existingVideos: response.videos,
            }}
            onSubmit={handleSubmit}
            onCancel={handleCancel}
            isEditing
            needInfo={
              response.need
                ? {
                    title: response.need.title,
                    description: response.need.description || '',
                    publisher: response.need.publisher.username,
                  }
                : undefined
            }
          />
        </CardContent>
      </Card>
    </div>
  );
}
