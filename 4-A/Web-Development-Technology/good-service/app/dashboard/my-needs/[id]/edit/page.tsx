'use client';

import { useState, useEffect } from 'react';
import { useRouter, useParams } from 'next/navigation';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import { Skeleton } from '@/components/ui/skeleton';
import NeedForm from '@/components/needs/NeedForm';
import { api } from '@/lib/api';
import { Need } from '@/types';
import { ROUTES, SUCCESS_MESSAGES } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { ArrowLeft } from 'lucide-react';
import Link from 'next/link';
import { Button } from '@/components/ui/button';

export default function EditNeedPage() {
  const router = useRouter();
  const params = useParams();
  const { toast } = useToast();
  const needId = params.id as string;

  const [need, setNeed] = useState<Need | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    fetchNeed();
  }, [needId]);

  const fetchNeed = async () => {
    try {
      const data = await api.get<Need>(`/needs/${needId}/detail`);
      setNeed(data);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载需求信息',
      });
      router.push(ROUTES.MY_NEEDS);
    } finally {
      setIsLoading(false);
    }
  };

  const handleSubmit = async (formData: FormData) => {
    try {
      await api.put(`/needs/${needId}`, formData);
      toast({
        title: SUCCESS_MESSAGES.UPDATE_SUCCESS,
        description: '需求已成功更新',
      });
      router.push(ROUTES.MY_NEEDS);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '更新失败',
        description: error.message || '无法更新需求，请稍后重试',
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

  if (!need) {
    return null;
  }

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      {/* Header */}
      <div className="flex items-center gap-4">
        <Link href={ROUTES.MY_NEEDS}>
          <Button variant="ghost" size="icon">
            <ArrowLeft className="h-5 w-5" />
          </Button>
        </Link>
        <h1 className="text-3xl font-bold">编辑需求</h1>
      </div>

      {/* Form Card */}
      <Card>
        <CardHeader>
          <CardTitle>需求信息</CardTitle>
        </CardHeader>
        <CardContent>
          <NeedForm
            initialData={{
              title: need.title,
              serviceType: need.serviceType,
              description: need.description,
              expectedTime: need.expectedTime,
              location: need.location,
              budget: need.budget,
              existingImages: need.images,
              existingVideos: need.videos,
            }}
            onSubmit={handleSubmit}
            onCancel={handleCancel}
            isEditing
          />
        </CardContent>
      </Card>
    </div>
  );
}
