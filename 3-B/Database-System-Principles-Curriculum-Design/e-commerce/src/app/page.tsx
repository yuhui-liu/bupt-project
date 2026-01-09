"use client";

import React, { useState } from "react";
import type { FormProps } from "antd";
import { Button, Checkbox, Flex, Form, Input, message } from "antd";
import { useRouter } from "next/navigation";
import { login } from "@/lib/fetchData";
import { Typography } from "antd";

type FieldType = {
  username?: string;
  password?: string;
  remember?: string;
};

export default function Home() {
  const router = useRouter();
  const [loading, setLoading] = useState(false);
  const [messageApi, contextHolder] = message.useMessage();

  const onFinish: FormProps<FieldType>["onFinish"] = async (values) => {
    try {
      setLoading(true);
      // username 和 password 不会为空因为已经在表单验证中处理过了
      const response = await login(values.username!, values.password!);

      const data = await response.json();

      if (response.ok) {
        messageApi.success("登录成功！");
        // 如果需要保存登录状态
        if (values.remember) {
          localStorage.setItem("token", data.token);
        }
        router.push("/dashboard"); // 登录成功后跳转
      } else {
        messageApi.error(data.error || "登录失败，请重试！");
      }
    } catch (error) {
      messageApi.error("登录出错，请稍后重试！");
      console.error(error);
    } finally {
      setLoading(false);
    }
  };

  const onFinishFailed: FormProps<FieldType>["onFinishFailed"] = (
    errorInfo,
  ) => {
    console.log("Failed:", errorInfo);
  };
  return (
    <div
      style={{
        minHeight: "100vh",
        display: "flex",
        alignItems: "center",
        justifyContent: "center",
        padding: "20px",
        backgroundImage: `linear-gradient(135deg, #667eea 0%, #764ba2 100%)`,
        position: "relative",
      }}
    >
      {contextHolder}

      <div
        style={{
          width: "100%",
          maxWidth: "420px",
          position: "relative",
          zIndex: 1,
        }}
      >
        <Flex gap="large" align="center" vertical>
          {/* 标题区域 */}
          <div style={{ textAlign: "center", marginBottom: "20px" }}>
            <Typography.Title
              className="login-title"
              style={{
                color: "white",
                marginBottom: "8px",
                fontSize: "2.5rem",
                textShadow: "0 2px 4px rgba(0,0,0,0.1)",
              }}
            >
              Welcome
            </Typography.Title>
            <Typography.Text
              style={{
                color: "rgba(255, 255, 255, 0.8)",
                fontSize: "16px",
              }}
            >
              登录您的账户以继续
            </Typography.Text>
          </div>

          {/* 登录表单卡片 */}
          <div
            className="login-container"
            style={{
              backdropFilter: "blur(20px)",
              backgroundColor: "rgba(255, 255, 255, 0.25)",
              padding: "40px 32px",
              borderRadius: "20px",
              boxShadow: "0 8px 32px rgba(0, 0, 0, 0.1)",
              border: "1px solid rgba(255, 255, 255, 0.2)",
              width: "100%",
            }}
          >
            <Form
              name="login"
              layout="vertical"
              style={{ width: "100%" }}
              initialValues={{ remember: true }}
              onFinish={onFinish}
              onFinishFailed={onFinishFailed}
              autoComplete="off"
              size="large"
            >
              <Form.Item<FieldType>
                label={
                  <span
                    style={{
                      color: "rgba(255, 255, 255, 0.9)",
                      fontWeight: 500,
                    }}
                  >
                    用户名
                  </span>
                }
                name="username"
                rules={[{ required: true, message: "请输入用户名！" }]}
                style={{ marginBottom: "24px" }}
                className="login-input"
              >
                <Input
                  placeholder="请输入用户名"
                  style={{
                    backgroundColor: "rgba(255, 255, 255, 0.9)",
                    border: "1px solid rgba(255, 255, 255, 0.3)",
                    borderRadius: "12px",
                    transition: "all 0.3s ease",
                  }}
                />
              </Form.Item>

              <Form.Item<FieldType>
                label={
                  <span
                    style={{
                      color: "rgba(255, 255, 255, 0.9)",
                      fontWeight: 500,
                    }}
                  >
                    密码
                  </span>
                }
                name="password"
                rules={[{ required: true, message: "请输入密码！" }]}
                style={{ marginBottom: "24px" }}
                className="login-input"
              >
                <Input.Password
                  placeholder="请输入密码"
                  style={{
                    backgroundColor: "rgba(255, 255, 255, 0.9)",
                    border: "1px solid rgba(255, 255, 255, 0.3)",
                    borderRadius: "12px",
                    transition: "all 0.3s ease",
                  }}
                />
              </Form.Item>

              {/* 记住我和忘记密码 */}
              <Flex
                justify="space-between"
                align="center"
                style={{ marginBottom: "32px" }}
              >
                <Form.Item<FieldType>
                  name="remember"
                  valuePropName="checked"
                  noStyle
                >
                  <Checkbox style={{ color: "rgba(255, 255, 255, 0.9)" }}>
                    记住我
                  </Checkbox>
                </Form.Item>
                <a
                  href="/forget"
                  style={{
                    color: "rgba(255, 255, 255, 0.9)",
                    textDecoration: "none",
                    transition: "color 0.3s ease",
                  }}
                >
                  忘记密码？
                </a>
              </Flex>

              {/* 登录按钮 */}
              <Form.Item style={{ marginBottom: "24px" }}>
                <Button
                  block
                  type="primary"
                  htmlType="submit"
                  loading={loading}
                  className="login-button"
                  style={{
                    height: "48px",
                    borderRadius: "12px",
                    background:
                      "linear-gradient(135deg, #667eea 0%, #764ba2 100%)",
                    border: "none",
                    fontSize: "16px",
                    fontWeight: 500,
                    transition: "all 0.3s ease",
                  }}
                >
                  登录
                </Button>
              </Form.Item>

              {/* 注册链接 */}
              <div style={{ textAlign: "center" }}>
                <span style={{ color: "rgba(255, 255, 255, 0.8)" }}>
                  还没有账号？{" "}
                  <a
                    href="/register"
                    style={{
                      color: "rgba(255, 255, 255, 0.9)",
                      textDecoration: "none",
                      fontWeight: 500,
                      transition: "color 0.3s ease",
                    }}
                    onMouseEnter={(e) => {
                      e.currentTarget.style.color = "white";
                    }}
                    onMouseLeave={(e) => {
                      e.currentTarget.style.color = "rgba(255, 255, 255, 0.9)";
                    }}
                  >
                    立即注册
                  </a>
                </span>
              </div>
            </Form>
          </div>
        </Flex>
      </div>
    </div>
  );
}
