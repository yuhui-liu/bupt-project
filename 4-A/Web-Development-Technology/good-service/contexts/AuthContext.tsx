'use client';

import React, { createContext, useContext, useState, useEffect, useCallback } from 'react';
import { useRouter } from 'next/navigation';
import type { User, LoginRequest } from '@/types';
import * as authLib from '@/lib/auth';
import { ROUTES } from '@/lib/constants';

interface AuthContextType {
  user: User | null;
  isAuthenticated: boolean;
  isAdmin: boolean;
  isLoading: boolean;
  login: (credentials: LoginRequest) => Promise<void>;
  logout: () => Promise<void>;
  updateUser: (data: Partial<User>) => void;
  refreshAuth: () => void;
}

const AuthContext = createContext<AuthContextType | undefined>(undefined);

export function AuthProvider({ children }: { children: React.ReactNode }) {
  const [user, setUser] = useState<User | null>(null);
  const [isLoading, setIsLoading] = useState(true);
  const router = useRouter();

  const isAuthenticated = !!user;
  const isAdmin = user?.role === 'admin';

  // Load user from localStorage on mount
  useEffect(() => {
    const storedUser = authLib.getStoredUser();
    const isAuth = authLib.isAuthenticated();

    if (isAuth && storedUser) {
      setUser(storedUser);
    }

    setIsLoading(false);
  }, []);

  // Login function
  const login = useCallback(
    async (credentials: LoginRequest) => {
      try {
        const response = await authLib.login(credentials);
        setUser(response.user);
        router.push(ROUTES.DASHBOARD);
      } catch (error) {
        throw error;
      }
    },
    [router]
  );

  // Logout function
  const logout = useCallback(async () => {
    try {
      await authLib.logout();
      setUser(null);
      router.push(ROUTES.HOME);
    } catch (error) {
      console.error('Logout error:', error);
      // Force logout even if API call fails
      setUser(null);
      router.push(ROUTES.HOME);
    }
  }, [router]);

  // Update user data
  const updateUser = useCallback((data: Partial<User>) => {
    setUser((prev) => {
      if (!prev) return prev;
      const updated = { ...prev, ...data };
      authLib.storeUser(updated);
      return updated;
    });
  }, []);

  // Refresh authentication state
  const refreshAuth = useCallback(() => {
    const storedUser = authLib.getStoredUser();
    const isAuth = authLib.isAuthenticated();

    if (isAuth && storedUser) {
      setUser(storedUser);
    } else {
      setUser(null);
    }
  }, []);

  const value: AuthContextType = {
    user,
    isAuthenticated,
    isAdmin,
    isLoading,
    login,
    logout,
    updateUser,
    refreshAuth,
  };

  return <AuthContext.Provider value={value}>{children}</AuthContext.Provider>;
}

// Hook to use auth context
export function useAuth() {
  const context = useContext(AuthContext);
  if (context === undefined) {
    throw new Error('useAuth must be used within an AuthProvider');
  }
  return context;
}
