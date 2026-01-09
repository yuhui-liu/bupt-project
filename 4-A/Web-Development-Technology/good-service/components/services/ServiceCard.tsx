import Link from 'next/link';
import { ServiceResponse } from '@/types';
import { Badge } from '@/components/ui/badge';
import { Button } from '@/components/ui/button';
import { Card, CardContent, CardFooter, CardHeader } from '@/components/ui/card';
import { responseStatusConfig } from '@/lib/constants';
import { getFileUrl } from '@/lib/utils';
import { formatDistanceToNow } from 'date-fns';
import { zhCN } from 'date-fns/locale';
import { DollarSign, Clock, FileText } from 'lucide-react';

interface ServiceCardProps {
  response: ServiceResponse;
  showActions?: boolean;
  onEdit?: (id: string) => void;
  onDelete?: (id: string) => void;
}

export default function ServiceCard({
  response,
  showActions = false,
  onEdit,
  onDelete,
}: ServiceCardProps) {
  const statusConfig = responseStatusConfig[response.status];

  return (
    <Card className="hover:shadow-lg transition-shadow duration-200">
      <CardHeader>
        <div className="flex items-start justify-between">
          <div className="flex-1">
            {response.need && (
              <Link
                href={`/dashboard/browse-needs/${response.need.id}`}
                className="text-lg font-semibold hover:text-primary transition-colors"
              >
                {response.need.title}
              </Link>
            )}
            <div className="flex items-center gap-2 mt-2">
              {response.need?.serviceTypeName && (
                <Badge variant="outline">{response.need.serviceTypeName}</Badge>
              )}
              <Badge className={`${statusConfig.bgColor} ${statusConfig.color} border-0`}>
                {statusConfig.label}
              </Badge>
            </div>
          </div>
        </div>
      </CardHeader>

      <CardContent className="space-y-3">
        {response.need?.publisher && (
          <div className="text-sm">
            <span className="text-muted-foreground">需求方：</span>
            <span className="font-medium">{response.need.publisher.username}</span>
          </div>
        )}

        <div className="border-t pt-3">
          <div className="flex items-center gap-2 text-sm font-medium mb-2">
            <FileText className="h-4 w-4" />
            <span>我的响应：</span>
          </div>
          <p className="text-sm text-muted-foreground line-clamp-3">
            {response.description}
          </p>
        </div>

        <div className="grid grid-cols-2 gap-2 text-sm">
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
        </div>

        {response.images && response.images.length > 0 && (
          <div className="grid grid-cols-3 gap-2">
            {response.images.slice(0, 3).map((img, idx) => (
              <img
                key={idx}
                src={getFileUrl(img)}
                alt={`响应图片 ${idx + 1}`}
                className="w-full h-20 object-cover rounded"
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

      {showActions && (
        <CardFooter className="flex gap-2">
          {response.status === 'pending' && onEdit && (
            <Button size="sm" variant="outline" onClick={() => onEdit(response.id)}>
              编辑
            </Button>
          )}

          {(response.status === 'pending' || response.status === 'rejected') && onDelete && (
            <Button size="sm" variant="destructive" onClick={() => onDelete(response.id)}>
              删除
            </Button>
          )}

          {response.status === 'accepted' && response.need?.publisher && (
            <div className="text-sm text-muted-foreground">
              联系电话：{response.need.publisher.phone}
            </div>
          )}
        </CardFooter>
      )}
    </Card>
  );
}
