'use client';

import { useState } from 'react';
import { useRouter } from 'next/navigation';
import Link from 'next/link';
import { useForm } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import * as z from 'zod';
import { register as registerUser } from '@/lib/auth';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Textarea } from '@/components/ui/textarea';
import { Label } from '@/components/ui/label';
import { Card, CardContent, CardDescription, CardFooter, CardHeader, CardTitle } from '@/components/ui/card';
import { Alert, AlertDescription } from '@/components/ui/alert';
import { useToast } from '@/components/ui/use-toast';
import { Loader2 } from 'lucide-react';
import { ROUTES, VALIDATION_RULES } from '@/lib/constants';

const registerSchema = z.object({
  username: z
    .string()
    .min(VALIDATION_RULES.username.min, `用户名至少${VALIDATION_RULES.username.min}个字符`)
    .max(VALIDATION_RULES.username.max, `用户名最多${VALIDATION_RULES.username.max}个字符`)
    .regex(/^[a-zA-Z0-9_]+$/, '用户名只能包含字母、数字和下划线'),
  password: z
    .string()
    .min(VALIDATION_RULES.password.min, `密码至少${VALIDATION_RULES.password.min}个字符`)
    .max(VALIDATION_RULES.password.max, `密码最多${VALIDATION_RULES.password.max}个字符`),
  confirmPassword: z.string(),
  phone: z
    .string()
    .regex(/^1[3-9]\d{9}$/, '请输入有效的11位手机号'),
  email: z.string().email('请输入有效的邮箱地址').optional().or(z.literal('')),
  address: z.string().max(VALIDATION_RULES.address.max, `地址最多${VALIDATION_RULES.address.max}个字符`).optional(),
  bio: z.string().max(VALIDATION_RULES.bio.max, `简介最多${VALIDATION_RULES.bio.max}个字符`).optional(),
}).refine((data) => data.password === data.confirmPassword, {
  message: '两次输入的密码不一致',
  path: ['confirmPassword'],
});

type RegisterFormData = z.infer<typeof registerSchema>;

export default function RegisterForm() {
  const router = useRouter();
  const { toast } = useToast();
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState('');

  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<RegisterFormData>({
    resolver: zodResolver(registerSchema),
  });

  const onSubmit = async (data: RegisterFormData) => {
    setIsLoading(true);
    setError('');

    try {
      await registerUser({
        username: data.username,
        password: data.password,
        phone: data.phone,
        email: data.email || undefined,
        address: data.address || undefined,
        bio: data.bio || undefined,
      });

      toast({
        title: '注册成功',
        description: '请使用您的账号登录',
      });

      // Redirect to login page
      router.push(ROUTES.HOME);
    } catch (error: any) {
      setError(error.message || '注册失败，请稍后重试');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <Card>
      <CardHeader>
        <CardTitle>注册新账号</CardTitle>
        <CardDescription>请填写以下信息完成注册</CardDescription>
      </CardHeader>
      <form onSubmit={handleSubmit(onSubmit)}>
        <CardContent className="space-y-4">
          {error && (
            <Alert variant="destructive">
              <AlertDescription>{error}</AlertDescription>
            </Alert>
          )}

          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-2">
              <Label htmlFor="username">
                用户名 <span className="text-red-500">*</span>
              </Label>
              <Input
                id="username"
                placeholder="请输入用户名"
                {...register('username')}
                disabled={isLoading}
              />
              {errors.username && (
                <p className="text-sm text-red-500">{errors.username.message}</p>
              )}
            </div>

            <div className="space-y-2">
              <Label htmlFor="phone">
                手机号 <span className="text-red-500">*</span>
              </Label>
              <Input
                id="phone"
                placeholder="请输入11位手机号"
                {...register('phone')}
                disabled={isLoading}
              />
              {errors.phone && (
                <p className="text-sm text-red-500">{errors.phone.message}</p>
              )}
            </div>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-2">
              <Label htmlFor="password">
                密码 <span className="text-red-500">*</span>
              </Label>
              <Input
                id="password"
                type="password"
                placeholder="请输入密码"
                {...register('password')}
                disabled={isLoading}
              />
              {errors.password && (
                <p className="text-sm text-red-500">{errors.password.message}</p>
              )}
            </div>

            <div className="space-y-2">
              <Label htmlFor="confirmPassword">
                确认密码 <span className="text-red-500">*</span>
              </Label>
              <Input
                id="confirmPassword"
                type="password"
                placeholder="请再次输入密码"
                {...register('confirmPassword')}
                disabled={isLoading}
              />
              {errors.confirmPassword && (
                <p className="text-sm text-red-500">{errors.confirmPassword.message}</p>
              )}
            </div>
          </div>

          <div className="space-y-2">
            <Label htmlFor="email">邮箱（可选）</Label>
            <Input
              id="email"
              type="email"
              placeholder="请输入邮箱地址"
              {...register('email')}
              disabled={isLoading}
            />
            {errors.email && (
              <p className="text-sm text-red-500">{errors.email.message}</p>
            )}
          </div>

          <div className="space-y-2">
            <Label htmlFor="address">地址（可选）</Label>
            <Input
              id="address"
              placeholder="请输入地址"
              {...register('address')}
              disabled={isLoading}
            />
            {errors.address && (
              <p className="text-sm text-red-500">{errors.address.message}</p>
            )}
          </div>

          <div className="space-y-2">
            <Label htmlFor="bio">个人简介（可选）</Label>
            <Textarea
              id="bio"
              placeholder="请输入个人简介"
              rows={3}
              {...register('bio')}
              disabled={isLoading}
            />
            {errors.bio && (
              <p className="text-sm text-red-500">{errors.bio.message}</p>
            )}
          </div>
        </CardContent>

        <CardFooter className="flex flex-col space-y-4">
          <Button type="submit" className="w-full" disabled={isLoading}>
            {isLoading && <Loader2 className="mr-2 h-4 w-4 animate-spin" />}
            注册
          </Button>
          <div className="text-sm text-center text-muted-foreground">
            已有账号？{' '}
            <Link href={ROUTES.HOME} className="text-primary hover:underline">
              返回登录
            </Link>
          </div>
        </CardFooter>
      </form>
    </Card>
  );
}
