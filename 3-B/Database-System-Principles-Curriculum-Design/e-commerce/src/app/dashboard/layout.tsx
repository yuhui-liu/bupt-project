"use client";

import React, { useState, useEffect } from "react";
import { useRouter, usePathname } from "next/navigation";
import {
  SettingOutlined,
  UserOutlined,
  DatabaseOutlined,
  SearchOutlined,
  BarChartOutlined,
  LogoutOutlined,
} from "@ant-design/icons";
import type { MenuProps } from "antd";
import {
  Layout,
  Menu,
  theme,
  Typography,
  Spin,
  Button,
} from "antd";
import Link from "next/link";
import axiosInstance from "@/lib/axios";

const { Header, Content, Footer, Sider } = Layout;
const { Text } = Typography;

type MenuItem = Required<MenuProps>["items"][number];

function getItem(
  label: React.ReactNode,
  key: React.Key,
  icon?: React.ReactNode,
  children?: MenuItem[],
): MenuItem {
  return {
    key,
    icon,
    children,
    label,
  } as MenuItem;
}

const items: MenuItem[] = [
  getItem("数据管理", "data", <DatabaseOutlined />),
  getItem("业务查询", "query", <SearchOutlined />),
  getItem("业务分析", "analysis", <BarChartOutlined />),
];

// 根据管理员状态生成菜单项
const getMenuItems = (isAdmin: boolean): MenuItem[] => {
  const baseItems = [...items];
  if (isAdmin) {
    baseItems.unshift(getItem("用户管理", "users", <UserOutlined />));
    baseItems.unshift(getItem("系统管理", "system", <SettingOutlined />));
  }
  return baseItems;
};

export default function Dashboard({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  const {
    token: { colorBgContainer, borderRadiusLG },
  } = theme.useToken();

  const router = useRouter();
  const pathname = usePathname();
  const [isAuthenticated, setIsAuthenticated] = useState<boolean | null>(null);
  const [isAdmin, setIsAdmin] = useState<boolean>(false);
  const [isCheckingAccess, setIsCheckingAccess] = useState<boolean>(true);

  const [selectedKey, setSelectedKey] = useState(() => {
    const key = pathname.split("/").pop() || "data";
    return key;
  });

  // 早期权限检查 - 在渲染任何内容之前
  useEffect(() => {
    const earlyAccessCheck = () => {
      const token = localStorage.getItem("token");
      if (!token) {
        router.replace("/");
        return;
      }

      // 如果在根 dashboard 路径，先不做处理，等待完整验证
      if (pathname === "/dashboard") {
        setIsCheckingAccess(false);
        return;
      }

      // 对于受限页面，立即检查权限
      if (pathname.includes("/system") || pathname.includes("/users")) {
        const storedIsAdmin = localStorage.getItem("isAdmin");
        if (storedIsAdmin === "false" || !storedIsAdmin) {
          router.replace("/dashboard/data");
          return;
        }
      }

      setIsCheckingAccess(false);
    };

    earlyAccessCheck();
  }, [pathname, router]);

  // 身份验证检查
  useEffect(() => {
    // 只有在早期检查完成后才进行完整的身份验证
    if (isCheckingAccess) return;

    const checkAuth = async () => {
      try {
        // 检查本地存储的 token
        const token = localStorage.getItem("token");
        if (!token) {
          router.replace("/");
          return;
        }

        const userInfoResponse = await axiosInstance.get("/api/myinfo");
        const adminStatus = userInfoResponse.data.is_admin;
        setIsAdmin(adminStatus);

        // 更新本地存储
        localStorage.setItem("isAdmin", adminStatus.toString());

        // 根据用户类型和当前路径决定跳转
        if (pathname === "/dashboard") {
          // 如果在根 dashboard 路径，根据管理员状态跳转
          if (adminStatus) {
            router.replace("/dashboard/system");
          } else {
            router.replace("/dashboard/data");
          }
          return;
        }

        // 权限检查：非管理员不能访问受限页面
        if (
          !adminStatus &&
          (pathname.includes("/system") || pathname.includes("/users"))
        ) {
          router.replace("/dashboard/data");
          return;
        }

        setIsAuthenticated(true);
      } catch (error) {
        console.error("认证失败:", error);
        localStorage.removeItem("token");
        localStorage.removeItem("isAdmin");
        router.replace("/");
      }
    };

    checkAuth();
  }, [router, pathname, isCheckingAccess]);

  useEffect(() => {
    const key = pathname.split("/").pop() || "data";
    setSelectedKey(key);
  }, [pathname]);

  // 登出功能
  const handleLogout = () => {
    localStorage.removeItem("token");
    localStorage.removeItem("isAdmin");
    router.replace("/");
  };

  // 显示加载状态直到认证完成
  if (isCheckingAccess || isAuthenticated === null) {
    return (
      <div
        style={{
          display: "flex",
          justifyContent: "center",
          alignItems: "center",
          height: "100vh",
        }}
      >
        <Spin size="large" />
      </div>
    );
  }

  // 如果未认证，不渲染内容（会被重定向）
  if (!isAuthenticated) {
    return null;
  }

  return (
    <Layout className="h-screen">
      <Sider
        breakpoint="lg"
        collapsedWidth="0"
        style={{
          overflow: "auto",
          height: "100vh",
          position: "fixed",
          left: 0,
          top: 0,
          bottom: 0,
        }}
      >
        {/* User Profile Section */}
        <div
          style={{
            padding: "16px",
            borderBottom: "1px solid rgba(255,255,255,0.1)",
            marginBottom: "8px",
          }}
        >
          <Link
            href="/dashboard/self"
            style={{
              display: "flex",
              alignItems: "center",
              padding: "8px 12px",
              borderRadius: "4px",
              transition: "all 0.3s",
              backgroundColor: "rgba(255,255,255,0.1)",
            }}
          >
            <UserOutlined style={{ fontSize: "16px", marginRight: "8px" }} />
            <Text strong style={{ color: "white", fontSize: "14px" }}>
              {isAdmin ? "管理员" : "用户"}
            </Text>
          </Link>

          {/* 添加登出按钮 */}
          <div style={{ marginTop: "16px" }}>
            <Button
              type="text"
              icon={<LogoutOutlined />}
              onClick={handleLogout}
              style={{ color: "rgba(255,255,255,0.65)" }}
            >
              登出
            </Button>
          </div>
        </div>

        {/* Navigation Menu */}
        <Menu
          theme="dark"
          mode="inline"
          selectedKeys={[selectedKey]}
          items={getMenuItems(isAdmin)}
          style={{
            borderRight: 0,
            padding: "8px",
          }}
          onClick={({ key }) => {
            setSelectedKey(key);
            router.push(`/dashboard/${key}`);
          }}
        />
      </Sider>

      <Layout style={{ marginLeft: 200 }}>
        <Header style={{ padding: 0, background: colorBgContainer }} />
        <Content style={{ margin: "24px 16px 0", overflow: "auto" }}>
          <div
            style={{
              padding: 24,
              minHeight: 360,
              background: colorBgContainer,
              borderRadius: borderRadiusLG,
            }}
          >
            {children}
          </div>
        </Content>
        <Footer className="text-center">
          ©{new Date().getFullYear()} Created by lyh
        </Footer>
      </Layout>
    </Layout>
  );
}
