'use client';

import { useState, useEffect } from 'react';
import { useForm } from 'react-hook-form';
import { Card, CardContent, CardHeader, CardTitle, CardDescription } from '@/components/ui/card';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Textarea } from '@/components/ui/textarea';
import { Badge } from '@/components/ui/badge';
import { Skeleton } from '@/components/ui/skeleton';
import { api } from '@/lib/api';
import { UserProfile, ProfileUpdateData, PasswordChangeData } from '@/types';
import { VALIDATION_RULES, SUCCESS_MESSAGES } from '@/lib/constants';
import { useToast } from '@/hooks/use-toast';
import { User, Phone, Mail, MapPin, FileText, Key, Award } from 'lucide-react';

export default function ProfilePage() {
  const { toast } = useToast();
  const [profile, setProfile] = useState<UserProfile | null>(null);
  const [isLoading, setIsLoading] = useState(true);
  const [isSaving, setIsSaving] = useState(false);
  const [isChangingPassword, setIsChangingPassword] = useState(false);

  const {
    register: registerProfile,
    handleSubmit: handleSubmitProfile,
    formState: { errors: profileErrors },
    reset: resetProfile,
  } = useForm<ProfileUpdateData>();

  const {
    register: registerPassword,
    handleSubmit: handleSubmitPassword,
    formState: { errors: passwordErrors },
    reset: resetPassword,
  } = useForm<PasswordChangeData>();

  useEffect(() => {
    fetchProfile();
  }, []);

  const fetchProfile = async () => {
    try {
      const data = await api.get<UserProfile>('/user/profile');
      setProfile(data);
      resetProfile({
        phone: data.phone,
        email: data.email || '',
        address: data.address || '',
        bio: data.bio || '',
      });
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '加载失败',
        description: error.message || '无法加载用户信息',
      });
    } finally {
      setIsLoading(false);
    }
  };

  const onUpdateProfile = async (data: ProfileUpdateData) => {
    setIsSaving(true);
    try {
      await api.put('/user/profile', data);
      toast({
        title: SUCCESS_MESSAGES.UPDATE_SUCCESS,
        description: '个人信息已更新',
      });
      fetchProfile();
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '更新失败',
        description: error.message || '无法更新个人信息',
      });
    } finally {
      setIsSaving(false);
    }
  };

  const onChangePassword = async (data: PasswordChangeData) => {
    setIsChangingPassword(true);
    try {
      await api.put('/user/password', data);
      toast({
        title: SUCCESS_MESSAGES.UPDATE_SUCCESS,
        description: '密码已成功修改',
      });
      resetPassword();
    } catch (error: any) {
      toast({
        variant: 'destructive',
        title: '修改失败',
        description: error.message || '无法修改密码',
      });
    } finally {
      setIsChangingPassword(false);
    }
  };

  if (isLoading) {
    return (
      <div className="max-w-4xl mx-auto space-y-6">
        <Skeleton className="h-10 w-48" />
        <Card>
          <CardHeader>
            <Skeleton className="h-6 w-32" />
          </CardHeader>
          <CardContent className="space-y-4">
            <Skeleton className="h-40 w-full" />
            <Skeleton className="h-40 w-full" />
          </CardContent>
        </Card>
      </div>
    );
  }

  if (!profile) {
    return null;
  }

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold">个人信息</h1>
        <p className="text-muted-foreground mt-2">管理您的账户信息和设置</p>
      </div>

      {/* User Info Card */}
      <Card>
        <CardHeader>
          <div className="flex items-center justify-between">
            <div>
              <CardTitle>基本信息</CardTitle>
              <CardDescription>您的账户基本信息</CardDescription>
            </div>
            <Badge variant={profile.role === 'admin' ? 'default' : 'secondary'}>
              {profile.role === 'admin' ? '管理员' : '普通用户'}
            </Badge>
          </div>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="flex items-center gap-3 p-3 bg-muted/50 rounded-lg">
              <User className="h-5 w-5 text-muted-foreground" />
              <div>
                <p className="text-sm text-muted-foreground">用户名</p>
                <p className="font-medium">{profile.username}</p>
              </div>
            </div>

            <div className="flex items-center gap-3 p-3 bg-muted/50 rounded-lg">
              <Award className="h-5 w-5 text-muted-foreground" />
              <div>
                <p className="text-sm text-muted-foreground">账户状态</p>
                <p className="font-medium">
                  {profile.status === 'active' ? '正常' : '已禁用'}
                </p>
              </div>
            </div>
          </div>

          {/* Statistics */}
          <div className="border-t my-4"></div>
          <div>
            <h3 className="font-semibold mb-3">活动统计</h3>
            <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
              <div className="text-center p-3 bg-blue-50 dark:bg-blue-950 rounded-lg">
                <p className="text-2xl font-bold text-blue-600 dark:text-blue-400">
                  {profile.stats.needsPublished}
                </p>
                <p className="text-sm text-muted-foreground">发布需求</p>
              </div>
              <div className="text-center p-3 bg-green-50 dark:bg-green-950 rounded-lg">
                <p className="text-2xl font-bold text-green-600 dark:text-green-400">
                  {profile.stats.needsAccepted}
                </p>
                <p className="text-sm text-muted-foreground">需求被接受</p>
              </div>
              <div className="text-center p-3 bg-purple-50 dark:bg-purple-950 rounded-lg">
                <p className="text-2xl font-bold text-purple-600 dark:text-purple-400">
                  {profile.stats.servicesProvided}
                </p>
                <p className="text-sm text-muted-foreground">提供服务</p>
              </div>
              <div className="text-center p-3 bg-orange-50 dark:bg-orange-950 rounded-lg">
                <p className="text-2xl font-bold text-orange-600 dark:text-orange-400">
                  {profile.stats.servicesAccepted}
                </p>
                <p className="text-sm text-muted-foreground">服务被接受</p>
              </div>
            </div>
          </div>
        </CardContent>
      </Card>

      {/* Edit Profile Card */}
      <Card>
        <CardHeader>
          <CardTitle>编辑信息</CardTitle>
          <CardDescription>更新您的个人信息</CardDescription>
        </CardHeader>
        <CardContent>
          <form onSubmit={handleSubmitProfile(onUpdateProfile)} className="space-y-4">
            {/* Phone */}
            <div className="space-y-2">
              <Label htmlFor="phone">
                <Phone className="inline h-4 w-4 mr-2" />
                手机号 <span className="text-destructive">*</span>
              </Label>
              <Input
                id="phone"
                type="tel"
                {...registerProfile('phone', {
                  required: '请输入手机号',
                  pattern: {
                    value: /^1[3-9]\d{9}$/,
                    message: '请输入有效的手机号',
                  },
                })}
                placeholder="请输入11位手机号"
              />
              {profileErrors.phone && (
                <p className="text-sm text-destructive">{profileErrors.phone.message}</p>
              )}
            </div>

            {/* Email */}
            <div className="space-y-2">
              <Label htmlFor="email">
                <Mail className="inline h-4 w-4 mr-2" />
                邮箱
              </Label>
              <Input
                id="email"
                type="email"
                {...registerProfile('email', {
                  pattern: {
                    value: /^[^\s@]+@[^\s@]+\.[^\s@]+$/,
                    message: '请输入有效的邮箱地址',
                  },
                })}
                placeholder="请输入邮箱地址"
              />
              {profileErrors.email && (
                <p className="text-sm text-destructive">{profileErrors.email.message}</p>
              )}
            </div>

            {/* Address */}
            <div className="space-y-2">
              <Label htmlFor="address">
                <MapPin className="inline h-4 w-4 mr-2" />
                地址
              </Label>
              <Input
                id="address"
                {...registerProfile('address', {
                  maxLength: {
                    value: VALIDATION_RULES.address.max,
                    message: `地址最多 ${VALIDATION_RULES.address.max} 个字符`,
                  },
                })}
                placeholder="请输入地址"
              />
              {profileErrors.address && (
                <p className="text-sm text-destructive">{profileErrors.address.message}</p>
              )}
            </div>

            {/* Bio */}
            <div className="space-y-2">
              <Label htmlFor="bio">
                <FileText className="inline h-4 w-4 mr-2" />
                个人简介
              </Label>
              <Textarea
                id="bio"
                {...registerProfile('bio', {
                  maxLength: {
                    value: VALIDATION_RULES.bio.max,
                    message: `简介最多 ${VALIDATION_RULES.bio.max} 个字符`,
                  },
                })}
                placeholder="介绍一下自己..."
                rows={4}
              />
              {profileErrors.bio && (
                <p className="text-sm text-destructive">{profileErrors.bio.message}</p>
              )}
            </div>

            <Button type="submit" disabled={isSaving}>
              {isSaving ? '保存中...' : '保存修改'}
            </Button>
          </form>
        </CardContent>
      </Card>

      {/* Change Password Card */}
      <Card>
        <CardHeader>
          <CardTitle>
            <Key className="inline h-5 w-5 mr-2" />
            修改密码
          </CardTitle>
          <CardDescription>更新您的登录密码</CardDescription>
        </CardHeader>
        <CardContent>
          <form onSubmit={handleSubmitPassword(onChangePassword)} className="space-y-4">
            {/* Old Password */}
            <div className="space-y-2">
              <Label htmlFor="oldPassword">
                当前密码 <span className="text-destructive">*</span>
              </Label>
              <Input
                id="oldPassword"
                type="password"
                {...registerPassword('oldPassword', {
                  required: '请输入当前密码',
                })}
                placeholder="请输入当前密码"
              />
              {passwordErrors.oldPassword && (
                <p className="text-sm text-destructive">{passwordErrors.oldPassword.message}</p>
              )}
            </div>

            {/* New Password */}
            <div className="space-y-2">
              <Label htmlFor="newPassword">
                新密码 <span className="text-destructive">*</span>
              </Label>
              <Input
                id="newPassword"
                type="password"
                {...registerPassword('newPassword', {
                  required: '请输入新密码',
                  minLength: {
                    value: VALIDATION_RULES.password.min,
                    message: `密码至少 ${VALIDATION_RULES.password.min} 个字符`,
                  },
                  maxLength: {
                    value: VALIDATION_RULES.password.max,
                    message: `密码最多 ${VALIDATION_RULES.password.max} 个字符`,
                  },
                })}
                placeholder="请输入新密码"
              />
              {passwordErrors.newPassword && (
                <p className="text-sm text-destructive">{passwordErrors.newPassword.message}</p>
              )}
            </div>

            <Button type="submit" disabled={isChangingPassword}>
              {isChangingPassword ? '修改中...' : '修改密码'}
            </Button>
          </form>
        </CardContent>
      </Card>

      {/* Account Info */}
      <Card>
        <CardHeader>
          <CardTitle>账户信息</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="text-sm text-muted-foreground space-y-1">
            <p>账户创建时间：{new Date(profile.createdAt).toLocaleString('zh-CN')}</p>
            {profile.updatedAt && (
              <p>最后更新时间：{new Date(profile.updatedAt).toLocaleString('zh-CN')}</p>
            )}
          </div>
        </CardContent>
      </Card>
    </div>
  );
}
