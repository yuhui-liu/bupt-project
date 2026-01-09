'use client';

import { useState, useEffect } from 'react';
import { useForm } from 'react-hook-form';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Textarea } from '@/components/ui/textarea';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select';
import { Alert, AlertDescription } from '@/components/ui/alert';
import { Card, CardContent } from '@/components/ui/card';
import { NeedFormData, ServiceType } from '@/types';
import { api } from '@/lib/api';
import { VALIDATION_RULES, MAX_IMAGE_SIZE, MAX_VIDEO_SIZE, MAX_IMAGES, MAX_VIDEOS } from '@/lib/constants';
import { getFileUrl } from '@/lib/utils';
import { X, Upload, Image as ImageIcon, Video } from 'lucide-react';
import { useToast } from '@/hooks/use-toast';

interface NeedFormProps {
  initialData?: Partial<NeedFormData> & { id?: string; existingImages?: string[]; existingVideos?: string[] };
  onSubmit: (data: FormData) => Promise<void>;
  onCancel?: () => void;
  isEditing?: boolean;
}

export default function NeedForm({
  initialData,
  onSubmit,
  onCancel,
  isEditing = false,
}: NeedFormProps) {
  const { toast } = useToast();
  const [serviceTypes, setServiceTypes] = useState<ServiceType[]>([]);
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
    setValue,
    watch,
    formState: { errors },
  } = useForm<NeedFormData>({
    defaultValues: {
      title: initialData?.title || '',
      serviceType: initialData?.serviceType || '',
      description: initialData?.description || '',
      expectedTime: initialData?.expectedTime || '',
      location: initialData?.location || '',
      budget: initialData?.budget || '',
    },
  });

  const selectedServiceType = watch('serviceType');

  useEffect(() => {
    fetchServiceTypes();
  }, []);

  const fetchServiceTypes = async () => {
    try {
      const data = await api.get<ServiceType[]>('/service-types');
      setServiceTypes(data);
    } catch (error) {
      console.error('Failed to fetch service types:', error);
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: '无法加载服务类型列表',
      });
    }
  };

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

  const onFormSubmit = async (data: NeedFormData) => {
    setIsLoading(true);
    try {
      const formData = new FormData();
      formData.append('title', data.title);
      formData.append('serviceType', data.serviceType);
      formData.append('description', data.description);
      if (data.expectedTime) formData.append('expectedTime', data.expectedTime);
      if (data.location) formData.append('location', data.location);
      if (data.budget) formData.append('budget', data.budget);

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
      {/* 需求标题 */}
      <div className="space-y-2">
        <Label htmlFor="title">
          需求标题 <span className="text-destructive">*</span>
        </Label>
        <Input
          id="title"
          {...register('title', {
            required: '请输入需求标题',
            minLength: {
              value: VALIDATION_RULES.title.min,
              message: `标题至少 ${VALIDATION_RULES.title.min} 个字符`,
            },
            maxLength: {
              value: VALIDATION_RULES.title.max,
              message: `标题最多 ${VALIDATION_RULES.title.max} 个字符`,
            },
          })}
          placeholder="请简要描述您的需求"
        />
        {errors.title && (
          <p className="text-sm text-destructive">{errors.title.message}</p>
        )}
      </div>

      {/* 服务类型 */}
      <div className="space-y-2">
        <Label htmlFor="serviceType">
          服务类型 <span className="text-destructive">*</span>
        </Label>
        <Select
          value={selectedServiceType}
          onValueChange={(value) => setValue('serviceType', value)}
        >
          <SelectTrigger>
            <SelectValue placeholder="请选择服务类型" />
          </SelectTrigger>
          <SelectContent>
            {serviceTypes.map((type) => (
              <SelectItem key={type.id} value={type.id}>
                {type.name}
              </SelectItem>
            ))}
          </SelectContent>
        </Select>
        {errors.serviceType && (
          <p className="text-sm text-destructive">{errors.serviceType.message}</p>
        )}
      </div>

      {/* 详细描述 */}
      <div className="space-y-2">
        <Label htmlFor="description">
          详细描述 <span className="text-destructive">*</span>
        </Label>
        <Textarea
          id="description"
          {...register('description', {
            required: '请输入详细描述',
            minLength: {
              value: VALIDATION_RULES.description.min,
              message: `描述至少 ${VALIDATION_RULES.description.min} 个字符`,
            },
            maxLength: {
              value: VALIDATION_RULES.description.max,
              message: `描述最多 ${VALIDATION_RULES.description.max} 个字符`,
            },
          })}
          placeholder="请详细描述您的需求、要求、注意事项等"
          rows={5}
        />
        {errors.description && (
          <p className="text-sm text-destructive">{errors.description.message}</p>
        )}
      </div>

      {/* 期望时间 */}
      <div className="space-y-2">
        <Label htmlFor="expectedTime">期望时间</Label>
        <Input
          id="expectedTime"
          type="datetime-local"
          {...register('expectedTime')}
        />
      </div>

      {/* 地点 */}
      <div className="space-y-2">
        <Label htmlFor="location">地点</Label>
        <Input
          id="location"
          {...register('location', {
            maxLength: {
              value: VALIDATION_RULES.location.max,
              message: `地点最多 ${VALIDATION_RULES.location.max} 个字符`,
            },
          })}
          placeholder="服务地点"
        />
        {errors.location && (
          <p className="text-sm text-destructive">{errors.location.message}</p>
        )}
      </div>

      {/* 预算范围 */}
      <div className="space-y-2">
        <Label htmlFor="budget">预算范围</Label>
        <Input
          id="budget"
          {...register('budget', {
            maxLength: {
              value: VALIDATION_RULES.budget.max,
              message: `预算最多 ${VALIDATION_RULES.budget.max} 个字符`,
            },
          })}
          placeholder="例如：100-200元"
        />
        {errors.budget && (
          <p className="text-sm text-destructive">{errors.budget.message}</p>
        )}
      </div>

      {/* 图片上传 */}
      <div className="space-y-2">
        <Label>图片（最多 {MAX_IMAGES} 张）</Label>
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
        <Label>视频（最多 {MAX_VIDEOS} 个）</Label>
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          {existingVideos.map((url, index) => (
            <Card key={`existing-vid-${index}`} className="relative">
              <CardContent className="p-2">
                <video
                  src={getFileUrl(url)}
                  controls
                  className="w-full h-48 rounded"
                />
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
          {isLoading ? '提交中...' : isEditing ? '保存修改' : '发布需求'}
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
