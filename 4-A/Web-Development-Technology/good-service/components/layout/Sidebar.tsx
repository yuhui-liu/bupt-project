'use client';

import Link from 'next/link';
import { usePathname } from 'next/navigation';
import { useAuth } from '@/contexts/AuthContext';
import { cn } from '@/lib/utils';
import { ROUTES } from '@/lib/constants';
import {
  Home,
  Search,
  FileText,
  Wrench,
  User,
  Users,
  ClipboardList,
  Settings,
  BarChart3,
} from 'lucide-react';

interface NavItem {
  href: string;
  label: string;
  icon: React.ReactNode;
  adminOnly?: boolean;
}

const navItems: NavItem[] = [
  {
    href: ROUTES.DASHBOARD,
    label: '首页',
    icon: <Home className="h-5 w-5" />,
  },
  {
    href: ROUTES.BROWSE_NEEDS,
    label: '浏览需求',
    icon: <Search className="h-5 w-5" />,
  },
  {
    href: ROUTES.MY_NEEDS,
    label: '我需要',
    icon: <FileText className="h-5 w-5" />,
  },
  {
    href: ROUTES.MY_SERVICES,
    label: '我服务',
    icon: <Wrench className="h-5 w-5" />,
  },
  {
    href: ROUTES.PROFILE,
    label: '个人信息',
    icon: <User className="h-5 w-5" />,
  },
  // Admin items
  {
    href: ROUTES.ADMIN_USERS,
    label: '用户管理',
    icon: <Users className="h-5 w-5" />,
    adminOnly: true,
  },
  {
    href: ROUTES.ADMIN_NEEDS,
    label: '需求管理',
    icon: <ClipboardList className="h-5 w-5" />,
    adminOnly: true,
  },
  {
    href: ROUTES.ADMIN_SERVICES,
    label: '服务管理',
    icon: <Settings className="h-5 w-5" />,
    adminOnly: true,
  },
  {
    href: ROUTES.ADMIN_STATISTICS,
    label: '统计分析',
    icon: <BarChart3 className="h-5 w-5" />,
    adminOnly: true,
  },
];

export default function Sidebar() {
  const pathname = usePathname();
  const { isAdmin } = useAuth();

  const filteredNavItems = navItems.filter(
    (item) => !item.adminOnly || isAdmin
  );

  return (
    <aside className="w-64 border-r bg-background h-[calc(100vh-4rem)] sticky top-16">
      <nav className="p-4 space-y-1">
        {filteredNavItems.map((item) => {
          // For dashboard home, only exact match
          // For other routes, match if pathname starts with the route
          const isActive = item.href === ROUTES.DASHBOARD
            ? pathname === item.href
            : pathname === item.href || pathname?.startsWith(item.href + '/');

          return (
            <Link
              key={item.href}
              href={item.href}
              className={cn(
                'flex items-center gap-3 px-4 py-3 rounded-lg transition-all hover:bg-accent',
                isActive
                  ? 'bg-primary text-primary-foreground hover:bg-primary/90'
                  : 'text-muted-foreground hover:text-foreground'
              )}
            >
              {item.icon}
              <span className="font-medium">{item.label}</span>
            </Link>
          );
        })}
      </nav>
    </aside>
  );
}
