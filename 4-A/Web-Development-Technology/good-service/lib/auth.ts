import { api, setAuthTokens, clearAuthTokens } from './api';
import { STORAGE_KEYS } from './constants';
import type {
  LoginRequest,
  LoginResponse,
  RegisterRequest,
  CaptchaResponse,
  User,
} from '@/types';

// Get stored user data
export function getStoredUser(): User | null {
  if (typeof window === 'undefined') return null;

  const userData = localStorage.getItem(STORAGE_KEYS.USER_DATA);
  if (!userData) return null;

  try {
    return JSON.parse(userData);
  } catch {
    return null;
  }
}

// Store user data
export function storeUser(user: User): void {
  if (typeof window === 'undefined') return;
  localStorage.setItem(STORAGE_KEYS.USER_DATA, JSON.stringify(user));
}

// Clear user data
export function clearUser(): void {
  if (typeof window === 'undefined') return;
  localStorage.removeItem(STORAGE_KEYS.USER_DATA);
}

// Login
export async function login(credentials: LoginRequest): Promise<LoginResponse> {
  const response = await api.post<LoginResponse>('/auth/login', credentials, {
    skipAuth: true,
  } as any);

  // Store tokens and user data
  setAuthTokens(response.token, response.refreshToken);
  storeUser(response.user);

  return response;
}

// Register
export async function register(data: RegisterRequest): Promise<{ userId: string; username: string }> {
  const response = await api.post<{ userId: string; username: string }>(
    '/auth/register',
    data,
    { skipAuth: true } as any
  );

  return response;
}

// Logout
export async function logout(): Promise<void> {
  try {
    await api.post('/auth/logout');
  } catch (error) {
    console.error('Logout error:', error);
  } finally {
    clearAuthTokens();
    clearUser();
  }
}

// Get captcha
export async function getCaptcha(): Promise<CaptchaResponse> {
  return api.get<CaptchaResponse>('/auth/captcha', { skipAuth: true } as any);
}

// Refresh captcha
export async function refreshCaptcha(): Promise<CaptchaResponse> {
  return api.post<CaptchaResponse>('/auth/captcha/refresh', undefined, {
    skipAuth: true,
  } as any);
}

// Check if user is authenticated
export function isAuthenticated(): boolean {
  if (typeof window === 'undefined') return false;
  return !!localStorage.getItem(STORAGE_KEYS.AUTH_TOKEN);
}

// Check if user is admin
export function isAdmin(): boolean {
  const user = getStoredUser();
  return user?.role === 'admin';
}
