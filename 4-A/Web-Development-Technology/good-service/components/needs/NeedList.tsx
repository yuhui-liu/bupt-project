import { Need } from '@/types';
import NeedCard from './NeedCard';
import { Alert, AlertDescription } from '@/components/ui/alert';
import { Skeleton } from '@/components/ui/skeleton';

interface NeedListProps {
  needs: Need[];
  isLoading?: boolean;
  showActions?: boolean;
  emptyMessage?: string;
  onEdit?: (id: string) => void;
  onDelete?: (id: string) => void;
  onCancel?: (id: string) => void;
  onComplete?: (id: string) => void;
  onViewResponses?: (id: string) => void;
}

export default function NeedList({
  needs,
  isLoading = false,
  showActions = false,
  emptyMessage = '暂无需求',
  onEdit,
  onDelete,
  onCancel,
  onComplete,
  onViewResponses,
}: NeedListProps) {
  if (isLoading) {
    return (
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
        {[...Array(6)].map((_, i) => (
          <div key={i} className="space-y-3">
            <Skeleton className="h-48 w-full" />
          </div>
        ))}
      </div>
    );
  }

  if (needs.length === 0) {
    return (
      <Alert>
        <AlertDescription className="text-center py-8">
          {emptyMessage}
        </AlertDescription>
      </Alert>
    );
  }

  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
      {needs.map((need) => (
        <NeedCard
          key={need.id}
          need={need}
          showActions={showActions}
          onEdit={onEdit}
          onDelete={onDelete}
          onCancel={onCancel}
          onComplete={onComplete}
          onViewResponses={onViewResponses}
        />
      ))}
    </div>
  );
}
