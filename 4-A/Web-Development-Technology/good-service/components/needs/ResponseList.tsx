'use client';

import { ServiceResponse } from '@/types';
import { Badge } from '@/components/ui/badge';
import { Button } from '@/components/ui/button';
import { Card, CardContent, CardFooter, CardHeader } from '@/components/ui/card';
import { Alert, AlertDescription } from '@/components/ui/alert';
import { Skeleton } from '@/components/ui/skeleton';
import { responseStatusConfig } from '@/lib/constants';
import { getFileUrl } from '@/lib/utils';
import { formatDistanceToNow } from 'date-fns';
import { zhCN } from 'date-fns/locale';
import { User, DollarSign, Clock, Phone, Star } from 'lucide-react';

interface ResponseListProps {
  responses: ServiceResponse[];
  isLoading?: boolean;
  showActions?: boolean;
  emptyMessage?: string;
  onAccept?: (responseId: string) => void;
  onReject?: (responseId: string) => void;
}

export default function ResponseList({
  responses,
  isLoading = false,
  showActions = false,
  emptyMessage = '暂无响应',
  onAccept,
  onReject,
}: ResponseListProps) {
  if (isLoading) {
    return (
      <div className="space-y-4">
        {[...Array(3)].map((_, i) => (
          <Skeleton key={i} className="h-48 w-full" />
        ))}
      </div>
    );
  }

  if (responses.length === 0) {
    return (
      <Alert>
        <AlertDescription className="text-center py-8">
          {emptyMessage}
        </AlertDescription>
      </Alert>
    );
  }

  return (
    <div className="space-y-4">
      {responses.map((response) => {
        const statusConfig = responseStatusConfig[response.status];

        return (
          <Card key={response.id} className="hover:shadow-md transition-shadow">
            <CardHeader>
              <div className="flex items-start justify-between">
                <div className="space-y-2">
                  {response.responder && (
                    <div className="flex items-center gap-2">
                      <User className="h-5 w-5 text-muted-foreground" />
                      <span className="font-semibold">{response.responder.username}</span>
                      {response.responder.stats && (
                        <div className="flex items-center gap-1 text-sm text-muted-foreground">
                          <Star className="h-4 w-4 fill-yellow-400 text-yellow-400" />
                          <span>
                            完成率 {response.responder.stats.completionRate}%
                          </span>
                        </div>
                      )}
                    </div>
                  )}
                  <Badge className={`${statusConfig.bgColor} ${statusConfig.color} border-0`}>
                    {statusConfig.label}
                  </Badge>
                </div>
              </div>
            </CardHeader>

            <CardContent className="space-y-3">
              <p className="text-sm whitespace-pre-wrap">{response.description}</p>

              <div className="grid grid-cols-1 md:grid-cols-2 gap-2 text-sm">
                {response.price && (
                  <div className="flex items-center gap-2 text-muted-foreground">
                    <DollarSign className="h-4 w-4" />
                    <span>{response.price}</span>
                  </div>
                )}

                {response.availableTime && (
                  <div className="flex items-center gap-2 text-muted-foreground">
                    <Clock className="h-4 w-4" />
                    <span>{response.availableTime}</span>
                  </div>
                )}

                {response.responder?.phone && (
                  <div className="flex items-center gap-2 text-muted-foreground">
                    <Phone className="h-4 w-4" />
                    <span>{response.responder.phone}</span>
                  </div>
                )}
              </div>

              {response.responder?.bio && (
                <div className="text-sm text-muted-foreground border-t pt-3">
                  <p className="font-medium mb-1">服务者简介：</p>
                  <p>{response.responder.bio}</p>
                </div>
              )}

              {response.responder?.stats && (
                <div className="text-sm text-muted-foreground border-t pt-3">
                  <div className="flex gap-4">
                    <span>总服务: {response.responder.stats.totalServices}</span>
                    <span>被接受: {response.responder.stats.acceptedServices}</span>
                  </div>
                </div>
              )}

              {response.images && response.images.length > 0 && (
                <div className="grid grid-cols-3 gap-2">
                  {response.images.map((img, idx) => (
                    <img
                      key={idx}
                      src={getFileUrl(img)}
                      alt={`响应图片 ${idx + 1}`}
                      className="w-full h-24 object-cover rounded"
                    />
                  ))}
                </div>
              )}

              <div className="text-xs text-muted-foreground">
                响应于{' '}
                {formatDistanceToNow(new Date(response.createdAt), {
                  addSuffix: true,
                  locale: zhCN,
                })}
              </div>
            </CardContent>

            {showActions && response.status === 'pending' && (
              <CardFooter className="flex gap-2">
                {onAccept && (
                  <Button onClick={() => onAccept(response.id)}>
                    接受此响应
                  </Button>
                )}
                {onReject && (
                  <Button
                    variant="outline"
                    onClick={() => onReject(response.id)}
                  >
                    拒绝
                  </Button>
                )}
              </CardFooter>
            )}
          </Card>
        );
      })}
    </div>
  );
}
