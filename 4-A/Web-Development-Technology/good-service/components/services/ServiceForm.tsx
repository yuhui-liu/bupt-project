'use client';

import { useState } from 'react';
import { useForm } from 'react-hook-form';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Textarea } from '@/components/ui/textarea';
import { Card, CardContent } from '@/components/ui/card';
import { ResponseFormData } from '@/types';
import { VALIDATION_RULES, MAX_IMAGE_SIZE, MAX_VIDEO_SIZE, MAX_IMAGES, MAX_VIDEOS } from '@/lib/constants';
import { getFileUrl } from '@/lib/utils';
import { X, Upload } from 'lucide-react';
import { useToast } from '@/hooks/use-toast';

interface ServiceFormProps {
  initialData?: Partial<ResponseFormData> & { id?: string; existingImages?: string[]; existingVideos?: string[] };
  onSubmit: (data: FormData) => Promise<void>;
  onCancel?: () => void;
  isEditing?: boolean;
  needInfo?: {
    title: string;
    description: string;
    publisher?: string;
  };
}

export default function ServiceForm({
  initialData,
  onSubmit,
  onCancel,
  isEditing = false,
  needInfo,
}: ServiceFormProps) {
  const { toast } = useToast();
  const [isLoading, setIsLoading] = useState(false);
  const [imageFiles, setImageFiles] = useState<File[]>([]);
  const [videoFiles, setVideoFiles] = useState<File[]>([]);
  const [existingImages, setExistingImages] = useState<string[]>(initialData?.existingImages || []);
  const [existingVideos, setExistingVideos] = useState<string[]>(initialData?.existingVideos || []);
  const [deletedImages, setDeletedImages] = useState<string[]>([]);
  const [deletedVideos, setDeletedVideos] = useState<string[]>([]);

  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<ResponseFormData>({
    defaultValues: {
      description: initialData?.description || '',
      price: initialData?.price || '',
      availableTime: initialData?.availableTime || '',
    },
  });

  const handleImageChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const files = Array.from(e.target.files || []);
    const totalImages = imageFiles.length + existingImages.length - deletedImages.length;

    if (totalImages + files.length > MAX_IMAGES) {
      toast({
        variant: 'destructive',
        title: '文件数量超限',
        description: `最多只能上传 ${MAX_IMAGES} 张图片`,
      });
      return;
    }

    const validFiles = files.filter((file) => {
      if (file.size > MAX_IMAGE_SIZE) {
        toast({
          variant: 'destructive',
          title: '文件过大',
          description: `图片 ${file.name} 超过 5MB 限制`,
        });
        return false;
      }
      return true;
    });

    setImageFiles([...imageFiles, ...validFiles]);
  };

  const handleVideoChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const files = Array.from(e.target.files || []);
    const totalVideos = videoFiles.length + existingVideos.length - deletedVideos.length;

    if (totalVideos + files.length > MAX_VIDEOS) {
      toast({
        variant: 'destructive',
        title: '文件数量超限',
        description: `最多只能上传 ${MAX_VIDEOS} 个视频`,
      });
      return;
    }

    const validFiles = files.filter((file) => {
      if (file.size > MAX_VIDEO_SIZE) {
        toast({
          variant: 'destructive',
          title: '文件过大',
          description: `视频 ${file.name} 超过 50MB 限制`,
        });
        return false;
      }
      return true;
    });

    setVideoFiles([...videoFiles, ...validFiles]);
  };

  const removeImage = (index: number) => {
    setImageFiles(imageFiles.filter((_, i) => i !== index));
  };

  const removeVideo = (index: number) => {
    setVideoFiles(videoFiles.filter((_, i) => i !== index));
  };

  const removeExistingImage = (url: string) => {
    setExistingImages(existingImages.filter((img) => img !== url));
    setDeletedImages([...deletedImages, url]);
  };

  const removeExistingVideo = (url: string) => {
    setExistingVideos(existingVideos.filter((vid) => vid !== url));
    setDeletedVideos([...deletedVideos, url]);
  };

  const onFormSubmit = async (data: ResponseFormData) => {
    setIsLoading(true);
    try {
      const formData = new FormData();
      formData.append('description', data.description);
      if (data.price) formData.append('price', data.price);
      if (data.availableTime) formData.append('availableTime', data.availableTime);

      imageFiles.forEach((file) => {
        formData.append('images', file);
      });

      videoFiles.forEach((file) => {
        formData.append('videos', file);
      });

      if (isEditing) {
        deletedImages.forEach((url) => {
          formData.append('deleteImages', url);
        });
        deletedVideos.forEach((url) => {
          formData.append('deleteVideos', url);
        });
      }

      await onSubmit(formData);
    } catch (error) {
      console.error('Form submission error:', error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <form onSubmit={handleSubmit(onFormSubmit)} className="space-y-6">
      {/* 需求信息显示 */}
      {needInfo && (
        <Card className="bg-muted/50">
          <CardContent className="pt-6">
            <h3 className="font-semibold mb-2">需求信息</h3>
            <div className="space-y-2 text-sm">
              <div>
                <span className="text-muted-foreground">标题：</span>
                <span>{needInfo.title}</span>
              </div>
              {needInfo.publisher && (
                <div>
                  <span className="text-muted-foreground">发布者：</span>
                  <span>{needInfo.publisher}</span>
                </div>
              )}
              <div>
                <span className="text-muted-foreground">描述：</span>
                <p className="mt-1">{needInfo.description}</p>
              </div>
            </div>
          </CardContent>
        </Card>
      )}

      {/* 服务描述 */}
      <div className="space-y-2">
        <Label htmlFor="description">
          服务描述 <span className="text-destructive">*</span>
        </Label>
        <Textarea
          id="description"
          {...register('description', {
            required: '请输入服务描述',
            minLength: {
              value: VALIDATION_RULES.description.min,
              message: `描述至少 ${VALIDATION_RULES.description.min} 个字符`,
            },
            maxLength: {
              value: VALIDATION_RULES.description.max,
              message: `描述最多 ${VALIDATION_RULES.description.max} 个字符`,
            },
          })}
          placeholder="请描述您的服务内容、经验、优势等"
          rows={5}
        />
        {errors.description && (
          <p className="text-sm text-destructive">{errors.description.message}</p>
        )}
      </div>

      {/* 报价 */}
      <div className="space-y-2">
        <Label htmlFor="price">报价</Label>
        <Input
          id="price"
          {...register('price', {
            maxLength: {
              value: VALIDATION_RULES.budget.max,
              message: `报价最多 ${VALIDATION_RULES.budget.max} 个字符`,
            },
          })}
          placeholder="例如：150元/次"
        />
        {errors.price && (
          <p className="text-sm text-destructive">{errors.price.message}</p>
        )}
      </div>

      {/* 可服务时间 */}
      <div className="space-y-2">
        <Label htmlFor="availableTime">可服务时间</Label>
        <Input
          id="availableTime"
          {...register('availableTime', {
            maxLength: {
              value: VALIDATION_RULES.location.max,
              message: `时间描述最多 ${VALIDATION_RULES.location.max} 个字符`,
            },
          })}
          placeholder="例如：工作日晚上，周末全天"
        />
        {errors.availableTime && (
          <p className="text-sm text-destructive">{errors.availableTime.message}</p>
        )}
      </div>

      {/* 图片上传 */}
      <div className="space-y-2">
        <Label>案例图片（最多 {MAX_IMAGES} 张）</Label>
        <div className="grid grid-cols-2 md:grid-cols-3 gap-4">
          {existingImages.map((url, index) => (
            <Card key={`existing-img-${index}`} className="relative">
              <CardContent className="p-2">
                <img
                  src={getFileUrl(url)}
                  alt={`现有图片 ${index + 1}`}
                  className="w-full h-32 object-cover rounded"
                />
                <Button
                  type="button"
                  variant="destructive"
                  size="icon"
                  className="absolute top-1 right-1 h-6 w-6"
                  onClick={() => removeExistingImage(url)}
                >
                  <X className="h-4 w-4" />
                </Button>
              </CardContent>
            </Card>
          ))}

          {imageFiles.map((file, index) => (
            <Card key={`new-img-${index}`} className="relative">
              <CardContent className="p-2">
                <img
                  src={URL.createObjectURL(file)}
                  alt={`新图片 ${index + 1}`}
                  className="w-full h-32 object-cover rounded"
                />
                <Button
                  type="button"
                  variant="destructive"
                  size="icon"
                  className="absolute top-1 right-1 h-6 w-6"
                  onClick={() => removeImage(index)}
                >
                  <X className="h-4 w-4" />
                </Button>
              </CardContent>
            </Card>
          ))}

          {imageFiles.length + existingImages.length - deletedImages.length < MAX_IMAGES && (
            <Card className="border-dashed">
              <CardContent className="p-2 h-36 flex items-center justify-center">
                <Label htmlFor="images" className="cursor-pointer text-center">
                  <Upload className="h-8 w-8 mx-auto mb-2 text-muted-foreground" />
                  <span className="text-sm text-muted-foreground">上传图片</span>
                  <Input
                    id="images"
                    type="file"
                    accept="image/*"
                    multiple
                    className="hidden"
                    onChange={handleImageChange}
                  />
                </Label>
              </CardContent>
            </Card>
          )}
        </div>
      </div>

      {/* 视频上传 */}
      <div className="space-y-2">
        <Label>案例视频（最多 {MAX_VIDEOS} 个）</Label>
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          {existingVideos.map((url, index) => (
            <Card key={`existing-vid-${index}`} className="relative">
              <CardContent className="p-2">
                <video src={getFileUrl(url)} controls className="w-full h-48 rounded" />
                <Button
                  type="button"
                  variant="destructive"
                  size="icon"
                  className="absolute top-1 right-1 h-6 w-6"
                  onClick={() => removeExistingVideo(url)}
                >
                  <X className="h-4 w-4" />
                </Button>
              </CardContent>
            </Card>
          ))}

          {videoFiles.map((file, index) => (
            <Card key={`new-vid-${index}`} className="relative">
              <CardContent className="p-2">
                <video
                  src={URL.createObjectURL(file)}
                  controls
                  className="w-full h-48 rounded"
                />
                <Button
                  type="button"
                  variant="destructive"
                  size="icon"
                  className="absolute top-1 right-1 h-6 w-6"
                  onClick={() => removeVideo(index)}
                >
                  <X className="h-4 w-4" />
                </Button>
              </CardContent>
            </Card>
          ))}

          {videoFiles.length + existingVideos.length - deletedVideos.length < MAX_VIDEOS && (
            <Card className="border-dashed">
              <CardContent className="p-2 h-52 flex items-center justify-center">
                <Label htmlFor="videos" className="cursor-pointer text-center">
                  <Upload className="h-8 w-8 mx-auto mb-2 text-muted-foreground" />
                  <span className="text-sm text-muted-foreground">上传视频</span>
                  <Input
                    id="videos"
                    type="file"
                    accept="video/*"
                    multiple
                    className="hidden"
                    onChange={handleVideoChange}
                  />
                </Label>
              </CardContent>
            </Card>
          )}
        </div>
      </div>

      {/* 提交按钮 */}
      <div className="flex gap-4">
        <Button type="submit" disabled={isLoading}>
          {isLoading ? '提交中...' : isEditing ? '保存修改' : '提交响应'}
        </Button>
        {onCancel && (
          <Button type="button" variant="outline" onClick={onCancel}>
            取消
          </Button>
        )}
      </div>
    </form>
  );
}
