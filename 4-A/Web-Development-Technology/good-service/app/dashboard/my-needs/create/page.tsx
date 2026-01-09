'use client';

import { useRouter } from 'next/navigation';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import NeedForm from '@/components/needs/NeedForm';
import { api } from '@/lib/api';
import { ROUTES, SUCCESS_MESSAGES } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { ArrowLeft } from 'lucide-react';
import Link from 'next/link';
import { Button } from '@/components/ui/button';

export default function CreateNeedPage() {
  const router = useRouter();
  const { toast } = useToast();

  const handleSubmit = async (formData: FormData) => {
    try {
      await api.post('/needs', formData);
      toast({
        title: SUCCESS_MESSAGES.CREATE_SUCCESS,
        description: '需求已成功发布',
      });
      router.push(ROUTES.MY_NEEDS);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '发布失败',
        description: error.message || '无法发布需求，请稍后重试',
      });
      throw error;
    }
  };

  const handleCancel = () => {
    router.back();
  };

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      {/* Header */}
      <div className="flex items-center gap-4">
        <Link href={ROUTES.MY_NEEDS}>
          <Button variant="ghost" size="icon">
            <ArrowLeft className="h-5 w-5" />
          </Button>
        </Link>
        <h1 className="text-3xl font-bold">发布需求</h1>
      </div>

      {/* Form Card */}
      <Card>
        <CardHeader>
          <CardTitle>需求信息</CardTitle>
        </CardHeader>
        <CardContent>
          <NeedForm onSubmit={handleSubmit} onCancel={handleCancel} />
        </CardContent>
      </Card>
    </div>
  );
}
