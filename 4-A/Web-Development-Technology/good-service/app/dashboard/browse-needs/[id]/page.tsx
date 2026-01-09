'use client';

import { useState, useEffect } from 'react';
import { useParams, useRouter } from 'next/navigation';
import Link from 'next/link';
import { Button } from '@/components/ui/button';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { Skeleton } from '@/components/ui/skeleton';
import { api } from '@/lib/api';
import { Need } from '@/types';
import { ROUTES, needStatusConfig } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { getFileUrl } from '@/lib/utils';
import { ArrowLeft, MapPin, Clock, DollarSign, User, MessageSquare } from 'lucide-react';
import { formatDistanceToNow } from 'date-fns';
import { zhCN } from 'date-fns/locale';

export default function NeedDetailPage() {
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
      setNeed(data);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载需求详情',
      });
      router.push(ROUTES.BROWSE_NEEDS);
    } finally {
      setIsLoading(false);
    }
  };

  if (isLoading) {
    return (
      <div className="max-w-4xl mx-auto space-y-6">
        <Skeleton className="h-10 w-48" />
        <Card>
          <CardHeader>
            <Skeleton className="h-8 w-3/4" />
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

  const statusConfig = needStatusConfig[need.status];
  const canRespond =
    (need.status === 'open' || need.status === 'responded') && !need.hasResponded;

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      {/* Header */}
      <div className="flex items-center justify-between">
        <div className="flex items-center gap-4">
          <Button variant="ghost" size="icon" onClick={() => router.back()}>
            <ArrowLeft className="h-5 w-5" />
          </Button>
          <h1 className="text-3xl font-bold">需求详情</h1>
        </div>

        {canRespond && (
          <Link href={ROUTES.RESPOND_NEED(needId)}>
            <Button>响应此需求</Button>
          </Link>
        )}
      </div>

      {/* Need Info Card */}
      <Card>
        <CardHeader>
          <div className="space-y-4">
            <div>
              <CardTitle className="text-2xl mb-3">{need.title}</CardTitle>
              <div className="flex items-center gap-2 flex-wrap">
                <Badge variant="outline">{need.serviceTypeName || need.serviceType}</Badge>
                <Badge className={`${statusConfig.bgColor} ${statusConfig.color} border-0`}>
                  {statusConfig.label}
                </Badge>
                {need.hasResponded && <Badge variant="secondary">您已响应</Badge>}
              </div>
            </div>
          </div>
        </CardHeader>

        <CardContent className="space-y-6">
          {/* Publisher Info */}
          {need.publisher && (
            <div className="border-b pb-4">
              <h3 className="font-semibold mb-3">发布者信息</h3>
              <div className="grid grid-cols-1 md:grid-cols-2 gap-3 text-sm">
                <div className="flex items-center gap-2">
                  <User className="h-4 w-4 text-muted-foreground" />
                  <span>{need.publisher.username}</span>
                </div>
                {need.status === 'accepted' && (
                  <div className="flex items-center gap-2">
                    <span className="text-muted-foreground">联系电话：</span>
                    <span>{need.publisher.phone}</span>
                  </div>
                )}
                {need.publisher.address && (
                  <div className="flex items-center gap-2">
                    <MapPin className="h-4 w-4 text-muted-foreground" />
                    <span>{need.publisher.address}</span>
                  </div>
                )}
              </div>
              {need.publisher.bio && (
                <p className="mt-3 text-sm text-muted-foreground">{need.publisher.bio}</p>
              )}
            </div>
          )}

          {/* Description */}
          <div>
            <h3 className="font-semibold mb-3">需求描述</h3>
            <p className="text-sm whitespace-pre-wrap leading-relaxed">{need.description}</p>
          </div>

          {/* Details */}
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4 text-sm">
            {need.location && (
              <div className="flex items-center gap-2">
                <MapPin className="h-4 w-4 text-muted-foreground" />
                <span className="text-muted-foreground">地点：</span>
                <span>{need.location}</span>
              </div>
            )}

            {need.expectedTime && (
              <div className="flex items-center gap-2">
                <Clock className="h-4 w-4 text-muted-foreground" />
                <span className="text-muted-foreground">期望时间：</span>
                <span>{new Date(need.expectedTime).toLocaleString('zh-CN')}</span>
              </div>
            )}

            {need.budget && (
              <div className="flex items-center gap-2">
                <DollarSign className="h-4 w-4 text-muted-foreground" />
                <span className="text-muted-foreground">预算：</span>
                <span>{need.budget}</span>
              </div>
            )}

            {need.responseCount !== undefined && (
              <div className="flex items-center gap-2">
                <MessageSquare className="h-4 w-4 text-muted-foreground" />
                <span className="text-muted-foreground">响应数：</span>
                <span>{need.responseCount}</span>
              </div>
            )}
          </div>

          {/* Images */}
          {need.images && need.images.length > 0 && (
            <div>
              <h3 className="font-semibold mb-3">图片</h3>
              <div className="grid grid-cols-2 md:grid-cols-3 gap-4">
                {need.images.map((img, idx) => (
                  <img
                    key={idx}
                    src={getFileUrl(img)}
                    alt={`需求图片 ${idx + 1}`}
                    className="w-full h-48 object-cover rounded-lg"
                  />
                ))}
              </div>
            </div>
          )}

          {/* Videos */}
          {need.videos && need.videos.length > 0 && (
            <div>
              <h3 className="font-semibold mb-3">视频</h3>
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                {need.videos.map((video, idx) => (
                  <video key={idx} src={getFileUrl(video)} controls className="w-full rounded-lg" />
                ))}
              </div>
            </div>
          )}

          {/* My Response Status */}
          {need.myResponse && (
            <div className="border-t pt-4">
              <h3 className="font-semibold mb-3">我的响应</h3>
              <Card className="bg-muted/50">
                <CardContent className="pt-4">
                  <div className="space-y-2 text-sm">
                    <div>
                      <span className="text-muted-foreground">状态：</span>
                      <Badge className="ml-2">
                        {need.myResponse.status === 'pending' && '待处理'}
                        {need.myResponse.status === 'accepted' && '已接受'}
                        {need.myResponse.status === 'rejected' && '已拒绝'}
                      </Badge>
                    </div>
                    <div>
                      <span className="text-muted-foreground">响应内容：</span>
                      <p className="mt-1">{need.myResponse.description}</p>
                    </div>
                    <div className="text-xs text-muted-foreground">
                      响应于{' '}
                      {formatDistanceToNow(new Date(need.myResponse.createdAt), {
                        addSuffix: true,
                        locale: zhCN,
                      })}
                    </div>
                  </div>
                </CardContent>
              </Card>
            </div>
          )}

          {/* Timestamps */}
          <div className="border-t pt-4 text-xs text-muted-foreground">
            <div>
              发布于{' '}
              {formatDistanceToNow(new Date(need.createdAt), {
                addSuffix: true,
                locale: zhCN,
              })}
            </div>
            {need.updatedAt && need.updatedAt !== need.createdAt && (
              <div>
                更新于{' '}
                {formatDistanceToNow(new Date(need.updatedAt), {
                  addSuffix: true,
                  locale: zhCN,
                })}
              </div>
            )}
          </div>
        </CardContent>
      </Card>

      {/* Actions */}
      {canRespond && (
        <div className="flex justify-center">
          <Link href={ROUTES.RESPOND_NEED(needId)}>
            <Button size="lg">响应此需求</Button>
          </Link>
        </div>
      )}
    </div>
  );
}
