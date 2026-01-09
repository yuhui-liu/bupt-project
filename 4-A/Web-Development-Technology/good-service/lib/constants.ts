import { NeedStatus, ResponseStatus } from '@/types';

// API Configuration
export const API_BASE_URL = process.env.NEXT_PUBLIC_API_BASE_URL || 'http://localhost:8080/api';
export const UPLOAD_BASE_URL = process.env.NEXT_PUBLIC_UPLOAD_BASE_URL || 'http://localhost:8080/uploads';

// File Upload Configuration
export const MAX_IMAGE_SIZE = Number(process.env.NEXT_PUBLIC_MAX_IMAGE_SIZE) || 5 * 1024 * 1024; // 5MB
export const MAX_VIDEO_SIZE = Number(process.env.NEXT_PUBLIC_MAX_VIDEO_SIZE) || 50 * 1024 * 1024; // 50MB
export const MAX_IMAGES = Number(process.env.NEXT_PUBLIC_MAX_IMAGES) || 5;
export const MAX_VIDEOS = Number(process.env.NEXT_PUBLIC_MAX_VIDEOS) || 2;

export const ALLOWED_IMAGE_TYPES = ['image/jpeg', 'image/png', 'image/gif', 'image/webp'];
export const ALLOWED_VIDEO_TYPES = ['video/mp4', 'video/mpeg', 'video/quicktime'];

// Token Configuration
export const TOKEN_EXPIRE_TIME = Number(process.env.NEXT_PUBLIC_TOKEN_EXPIRE_TIME) || 86400000; // 24 hours
export const REFRESH_TOKEN_EXPIRE_TIME = Number(process.env.NEXT_PUBLIC_REFRESH_TOKEN_EXPIRE_TIME) || 604800000; // 7 days

// Feature Flags
export const ENABLE_VIDEO_UPLOAD = process.env.NEXT_PUBLIC_ENABLE_VIDEO_UPLOAD === 'true';
export const ENABLE_NOTIFICATIONS = process.env.NEXT_PUBLIC_ENABLE_NOTIFICATIONS === 'true';

// Pagination
export const DEFAULT_PAGE_SIZE = 10;
export const PAGE_SIZE_OPTIONS = [10, 20, 50];

// Status Configuration
export const needStatusConfig: Record<NeedStatus, { label: string; color: string; bgColor: string }> = {
  open: { label: '未响应', color: 'text-gray-600', bgColor: 'bg-gray-100' },
  responded: { label: '已响应', color: 'text-blue-600', bgColor: 'bg-blue-100' },
  accepted: { label: '已接受', color: 'text-green-600', bgColor: 'bg-green-100' },
  completed: { label: '已完成', color: 'text-purple-600', bgColor: 'bg-purple-100' },
  cancelled: { label: '已取消', color: 'text-red-600', bgColor: 'bg-red-100' },
};

export const responseStatusConfig: Record<ResponseStatus, { label: string; color: string; bgColor: string }> = {
  pending: { label: '待处理', color: 'text-yellow-600', bgColor: 'bg-yellow-100' },
  accepted: { label: '已接受', color: 'text-green-600', bgColor: 'bg-green-100' },
  rejected: { label: '已拒绝', color: 'text-red-600', bgColor: 'bg-red-100' },
};

// Predefined Service Types (fallback if API not available)
export const PREDEFINED_SERVICE_TYPES = [
  { id: '1', name: '管道维修', order: 1 },
  { id: '2', name: '助老服务', order: 2 },
  { id: '3', name: '保洁服务', order: 3 },
  { id: '4', name: '就诊服务', order: 4 },
  { id: '5', name: '替教服务', order: 5 },
  { id: '6', name: '定期接送服务', order: 6 },
  { id: '7', name: '其他', order: 7 },
];

// Validation Rules
export const VALIDATION_RULES = {
  username: { min: 3, max: 20 },
  password: { min: 6, max: 20 },
  phone: { length: 11 },
  title: { min: 2, max: 50 },
  description: { min: 10, max: 500 },
  bio: { max: 200 },
  address: { max: 100 },
  budget: { max: 50 },
  location: { max: 100 },
  keyword: { min: 2, max: 50 },
};

// Error Messages
export const ERROR_MESSAGES = {
  NETWORK_ERROR: '网络连接失败，请检查网络设置',
  PERMISSION_ERROR: '您没有权限执行此操作',
  UNKNOWN_ERROR: '发生未知错误，请稍后重试',
  VALIDATION_ERROR: '请检查输入内容',
  FILE_TOO_LARGE: '文件大小超过限制',
  INVALID_FILE_TYPE: '不支持的文件类型',
  TOO_MANY_FILES: '文件数量超过限制',
};

// Success Messages
export const SUCCESS_MESSAGES = {
  LOGIN_SUCCESS: '登录成功',
  REGISTER_SUCCESS: '注册成功',
  LOGOUT_SUCCESS: '已退出登录',
  CREATE_SUCCESS: '创建成功',
  UPDATE_SUCCESS: '更新成功',
  DELETE_SUCCESS: '删除成功',
  SUBMIT_SUCCESS: '提交成功',
};

// Routes
export const ROUTES = {
  HOME: '/',
  REGISTER: '/register',
  DASHBOARD: '/dashboard',
  PROFILE: '/dashboard/profile',
  MY_NEEDS: '/dashboard/my-needs',
  CREATE_NEED: '/dashboard/my-needs/create',
  EDIT_NEED: (id: string) => `/dashboard/my-needs/${id}/edit`,
  MY_SERVICES: '/dashboard/my-services',
  BROWSE_NEEDS: '/dashboard/browse-needs',
  NEED_DETAIL: (id: string) => `/dashboard/browse-needs/${id}`,
  RESPOND_NEED: (id: string) => `/dashboard/browse-needs/${id}/respond`,
  ADMIN_USERS: '/dashboard/admin/users',
  ADMIN_NEEDS: '/dashboard/admin/needs',
  ADMIN_SERVICES: '/dashboard/admin/services',
  ADMIN_STATISTICS: '/dashboard/admin/statistics',
};

// Local Storage Keys
export const STORAGE_KEYS = {
  AUTH_TOKEN: 'auth_token',
  REFRESH_TOKEN: 'refresh_token',
  USER_DATA: 'user_data',
};
