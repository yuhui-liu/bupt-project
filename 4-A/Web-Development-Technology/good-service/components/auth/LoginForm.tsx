'use client';

import { useState, useEffect } from 'react';
import Link from 'next/link';
import { useForm } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import * as z from 'zod';
import { useAuth } from '@/contexts/AuthContext';
import { getCaptcha, refreshCaptcha as refreshCaptchaApi } from '@/lib/auth';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Card, CardContent, CardDescription, CardFooter, CardHeader, CardTitle } from '@/components/ui/card';
import { Alert, AlertDescription } from '@/components/ui/alert';
import { useToast } from '@/components/ui/use-toast';
import { Loader2, RefreshCw } from 'lucide-react';
import { ROUTES } from '@/lib/constants';
import type { CaptchaResponse } from '@/types';

const loginSchema = z.object({
  username: z.string().min(3, '用户名至少3个字符').max(20, '用户名最多20个字符'),
  password: z.string().min(6, '密码至少6个字符').max(20, '密码最多20个字符'),
  captcha: z.string().min(4, '请输入验证码'),
});

type LoginFormData = z.infer<typeof loginSchema>;

export default function LoginForm() {
  const { login } = useAuth();
  const { toast } = useToast();
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState('');
  const [captchaData, setCaptchaData] = useState<CaptchaResponse | null>(null);
  const [loadingCaptcha, setLoadingCaptcha] = useState(false);

  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm<LoginFormData>({
    resolver: zodResolver(loginSchema),
  });

  const loadCaptcha = async () => {
    setLoadingCaptcha(true);
    try {
      const data = await getCaptcha();
      setCaptchaData(data);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '获取验证码失败',
        description: error.message || '请稍后重试',
      });
    } finally {
      setLoadingCaptcha(false);
    }
  };

  // Load captcha on component mount
  useEffect(() => {
    loadCaptcha();
  }, []);

  const refreshCaptcha = async () => {
    setLoadingCaptcha(true);
    try {
      const data = await refreshCaptchaApi();
      setCaptchaData(data);
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '刷新验证码失败',
        description: error.message || '请稍后重试',
      });
    } finally {
      setLoadingCaptcha(false);
    }
  };

  const onSubmit = async (data: LoginFormData) => {
    if (!captchaData) {
      setError('请先获取验证码');
      return;
    }

    setIsLoading(true);
    setError('');

    try {
      await login({
        username: data.username,
        password: data.password,
        captcha: data.captcha,
        captchaId: captchaData.captchaId,
      });

      toast({
        title: '登录成功',
        description: '欢迎回来！',
      });
    } catch (error: any) {
      setError(error.message || '登录失败，请检查用户名和密码');
      // Refresh captcha on error
      refreshCaptcha();
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <Card>
      <CardHeader>
        <CardTitle>登录</CardTitle>
        <CardDescription>请输入您的账号信息</CardDescription>
      </CardHeader>
      <form onSubmit={handleSubmit(onSubmit)}>
        <CardContent className="space-y-4">
          {error && (
            <Alert variant="destructive">
              <AlertDescription>{error}</AlertDescription>
            </Alert>
          )}

          <div className="space-y-2">
            <Label htmlFor="username">用户名</Label>
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
            <Label htmlFor="password">密码</Label>
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
            <Label htmlFor="captcha">验证码</Label>
            <div className="flex gap-2">
              <Input
                id="captcha"
                placeholder="请输入验证码"
                {...register('captcha')}
                disabled={isLoading}
                className="flex-1"
              />
              <Button
                type="button"
                variant="outline"
                onClick={refreshCaptcha}
                disabled={loadingCaptcha || isLoading}
                className="shrink-0"
              >
                {loadingCaptcha ? (
                  <Loader2 className="h-4 w-4 animate-spin" />
                ) : (
                  <RefreshCw className="h-4 w-4" />
                )}
              </Button>
            </div>
            {captchaData && (
              <div className="mt-2">
                <img
                  src={captchaData.captchaImage}
                  alt="验证码"
                  className="h-12 border rounded cursor-pointer hover:opacity-80"
                  onClick={refreshCaptcha}
                />
              </div>
            )}
            {errors.captcha && (
              <p className="text-sm text-red-500">{errors.captcha.message}</p>
            )}
          </div>
        </CardContent>

        <CardFooter className="flex flex-col space-y-4">
          <Button type="submit" className="w-full" disabled={isLoading}>
            {isLoading && <Loader2 className="mr-2 h-4 w-4 animate-spin" />}
            登录
          </Button>
          <div className="text-sm text-center text-muted-foreground">
            还没有账号？{' '}
            <Link href={ROUTES.REGISTER} className="text-primary hover:underline">
              立即注册
            </Link>
          </div>
        </CardFooter>
      </form>
    </Card>
  );
}
