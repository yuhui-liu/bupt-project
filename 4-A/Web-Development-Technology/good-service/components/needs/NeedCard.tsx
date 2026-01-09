import Link from 'next/link';
import { Need } from '@/types';
import { Badge } from '@/components/ui/badge';
import { Button } from '@/components/ui/button';
import { Card, CardContent, CardFooter, CardHeader } from '@/components/ui/card';
import { needStatusConfig } from '@/lib/constants';
import { formatDistanceToNow } from 'date-fns';
import { zhCN } from 'date-fns/locale';
import { MapPin, Clock, DollarSign, User, MessageSquare } from 'lucide-react';

interface NeedCardProps {
  need: Need;
  showActions?: boolean;
  onEdit?: (id: string) => void;
  onDelete?: (id: string) => void;
  onCancel?: (id: string) => void;
  onComplete?: (id: string) => void;
  onViewResponses?: (id: string) => void;
}

export default function NeedCard({
  need,
  showActions = false,
  onEdit,
  onDelete,
  onCancel,
  onComplete,
  onViewResponses,
}: NeedCardProps) {
  const statusConfig = needStatusConfig[need.status];

  return (
    <Card className="hover:shadow-lg transition-shadow duration-200" data-need-id={need.id}>
      <CardHeader>
        <div className="flex items-start justify-between">
          <div className="flex-1">
            <Link
              href={`/dashboard/browse-needs/${need.id}`}
              className="text-lg font-semibold hover:text-primary transition-colors"
            >
              {need.title}
            </Link>
            <div className="flex items-center gap-2 mt-2">
              <Badge variant="outline">{need.serviceTypeName || need.serviceType}</Badge>
              <Badge className={`${statusConfig.bgColor} ${statusConfig.color} border-0`}>
                {statusConfig.label}
              </Badge>
              {need.hasResponded && (
                <Badge variant="secondary">已响应</Badge>
              )}
            </div>
          </div>
        </div>
      </CardHeader>

      <CardContent className="space-y-3">
        <p className="text-sm text-muted-foreground line-clamp-2">
          {need.description}
        </p>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-2 text-sm">
          {need.publisher && (
            <div className="flex items-center gap-2 text-muted-foreground">
              <User className="h-4 w-4" />
              <span>{need.publisher.username}</span>
            </div>
          )}

          {need.location && (
            <div className="flex items-center gap-2 text-muted-foreground">
              <MapPin className="h-4 w-4" />
              <span className="truncate">{need.location}</span>
            </div>
          )}

          {need.expectedTime && (
            <div className="flex items-center gap-2 text-muted-foreground">
              <Clock className="h-4 w-4" />
              <span>
                {new Date(need.expectedTime).toLocaleDateString('zh-CN')}
              </span>
            </div>
          )}

          {need.budget && (
            <div className="flex items-center gap-2 text-muted-foreground">
              <DollarSign className="h-4 w-4" />
              <span>{need.budget}</span>
            </div>
          )}
        </div>

        {need.responseCount !== undefined && (
          <div className="flex items-center gap-2 text-sm text-muted-foreground">
            <MessageSquare className="h-4 w-4" />
            <span>{need.responseCount} 个响应</span>
            {need.pendingResponseCount !== undefined && need.pendingResponseCount > 0 && (
              <Badge variant="destructive" className="ml-2">
                {need.pendingResponseCount} 个待处理
              </Badge>
            )}
          </div>
        )}

        <div className="text-xs text-muted-foreground">
          发布于{' '}
          {formatDistanceToNow(new Date(need.createdAt), {
            addSuffix: true,
            locale: zhCN,
          })}
        </div>
      </CardContent>

      {showActions && (
        <CardFooter className="flex gap-2 flex-wrap">
          {need.status === 'open' && onEdit && (
            <Button size="sm" variant="outline" onClick={() => onEdit(need.id)}>
              编辑
            </Button>
          )}

          {need.status === 'open' && onDelete && (
            <Button size="sm" variant="destructive" onClick={() => onDelete(need.id)}>
              删除
            </Button>
          )}

          {need.status !== 'completed' && need.status !== 'cancelled' && onCancel && (
            <Button size="sm" variant="outline" onClick={() => onCancel(need.id)}>
              取消
            </Button>
          )}

          {need.status === 'accepted' && onComplete && (
            <Button size="sm" onClick={() => onComplete(need.id)}>
              标记完成
            </Button>
          )}

          {(need.status === 'responded' || need.status === 'accepted') && onViewResponses && (
            <Button
              size="sm"
              variant="secondary"
              onClick={() => onViewResponses(need.id)}
            >
              查看响应 ({need.responseCount || 0})
            </Button>
          )}
        </CardFooter>
      )}
    </Card>
  );
}
