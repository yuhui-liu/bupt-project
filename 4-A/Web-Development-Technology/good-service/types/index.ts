// User Types
export type UserRole = 'user' | 'admin';
export type UserStatus = 'active' | 'disabled';

export interface User {
  id: string;
  username: string;
  phone: string;
  email?: string;
  address?: string;
  bio?: string;
  role: UserRole;
  status: UserStatus;
  createdAt: string;
  updatedAt?: string;
}

export interface UserProfile extends User {
  stats: {
    needsPublished: number;
    needsAccepted: number;
    servicesProvided: number;
    servicesAccepted: number;
  };
}

// Need Types
export type NeedStatus = 'open' | 'responded' | 'accepted' | 'completed' | 'cancelled';

export interface Need {
  id: string;
  publisherId: string;
  publisher?: {
    id: string;
    username: string;
    phone: string;
    bio?: string;
    address?: string;
  };
  title: string;
  serviceType: string;
  serviceTypeName?: string;
  description: string;
  status: NeedStatus;
  expectedTime?: string;
  location?: string;
  budget?: string;
  images?: string[];
  videos?: string[];
  cancelReason?: string;
  rating?: number;
  comment?: string;
  responseCount?: number;
  pendingResponseCount?: number;
  hasResponded?: boolean;
  myResponseStatus?: ResponseStatus;
  acceptedResponse?: {
    id: string;
    responderName: string;
    responderPhone: string;
  };
  myResponse?: {
    id: string;
    description: string;
    status: string;
    createdAt: string;
  };
  createdAt: string;
  updatedAt?: string;
}

// Service Response Types
export type ResponseStatus = 'pending' | 'accepted' | 'rejected';

export interface ServiceResponse {
  id: string;
  needId: string;
  need?: {
    id: string;
    title: string;
    serviceType: string;
    serviceTypeName?: string;
    description?: string;
    publisher: {
      id: string;
      username: string;
      phone: string;
      address?: string;
    };
    status: string;
  };
  responderId: string;
  responder?: {
    id: string;
    username: string;
    bio?: string;
    phone: string;
    stats?: {
      totalServices: number;
      acceptedServices: number;
      completionRate: number;
    };
  };
  description: string;
  price?: string;
  availableTime?: string;
  images?: string[];
  videos?: string[];
  status: ResponseStatus;
  createdAt: string;
  updatedAt?: string;
}

// Service Type
export interface ServiceType {
  id: string;
  name: string;
  description?: string;
  icon?: string;
  order: number;
  isActive?: boolean;
  createdAt?: string;
  updatedAt?: string;
}

// API Response Types
export interface ApiResponse<T = any> {
  success: boolean;
  message?: string;
  data?: T;
  error?: {
    code: string;
    details?: any;
    field?: string;
  };
}

export interface PaginatedResponse<T> {
  total: number;
  page: number;
  pageSize: number;
  items: T[];
}

// Auth Types
export interface LoginRequest {
  username: string;
  password: string;
  captcha: string;
  captchaId: string;
}

export interface LoginResponse {
  token: string;
  refreshToken: string;
  user: User;
}

export interface RegisterRequest {
  username: string;
  password: string;
  phone: string;
  email?: string;
  address?: string;
  bio?: string;
}

export interface CaptchaResponse {
  captchaId: string;
  captchaImage: string;
  expiresIn: number;
}

// Form Types
export interface NeedFormData {
  title: string;
  serviceType: string;
  description: string;
  expectedTime?: string;
  location?: string;
  budget?: string;
  images?: FileList | File[];
  videos?: FileList | File[];
  deleteImages?: string[];
  deleteVideos?: string[];
}

export interface ResponseFormData {
  description: string;
  price?: string;
  availableTime?: string;
  images?: FileList | File[];
  videos?: FileList | File[];
  deleteImages?: string[];
  deleteVideos?: string[];
}

export interface ProfileUpdateData {
  phone?: string;
  email?: string;
  address?: string;
  bio?: string;
}

export interface PasswordChangeData {
  oldPassword: string;
  newPassword: string;
}

// Statistics Types
export interface StatisticsData {
  month: string;
  serviceType: string;
  serviceTypeName: string;
  needsPublished: number;
  responsesReceived: number;
  needsAccepted: number;
  needsCompleted: number;
  completionRate: number;
  avgResponseTime: number;
  avgResponseCount: number;
}

export interface StatisticsSummary {
  totalNeeds: number;
  totalResponses: number;
  totalAccepted: number;
  totalCompleted: number;
  overallCompletionRate: number;
  activeUsers: number;
  newUsers: number;
}

export interface StatisticsCharts {
  needsTrend: Array<{
    month: string;
    count: number;
  }>;
  serviceTypeDistribution: Array<{
    serviceType: string;
    count: number;
    percentage: number;
  }>;
  completionRateTrend: Array<{
    month: string;
    rate: number;
  }>;
}

export interface StatisticsResponse {
  statistics: StatisticsData[];
  summary: StatisticsSummary;
  charts: StatisticsCharts;
}

// Query Params
export interface PaginationParams {
  page?: number;
  pageSize?: number;
}

export interface NeedsQueryParams extends PaginationParams {
  status?: NeedStatus | '';
  serviceType?: string;
  keyword?: string;
  sortBy?: 'createdAt' | 'responseCount';
}

export interface ResponsesQueryParams extends PaginationParams {
  status?: ResponseStatus | '';
}

export interface UsersQueryParams extends PaginationParams {
  keyword?: string;
  role?: UserRole | '';
}

export interface StatisticsQueryParams {
  startMonth: string;
  endMonth: string;
  region?: string;
  serviceType?: string;
}
