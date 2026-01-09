"use client";

import React, { useState } from "react";
import type { FormProps } from "antd";
import { Button, Flex, Form, Input, message, Progress } from "antd";
import { register, getCaptcha } from "@/lib/fetchData";
import { Typography } from "antd";
import {
  checkPasswordStrength,
  passwordValidationRules,
  type PasswordStrength,
} from "@/lib/passwordUtils";
import Link from "next/link";

type FieldType = {
  username?: string;
  password?: string;
  confirmPassword?: string;
  email?: string;
  captcha?: string;
};

export default function Register() {
  // const router = useRouter();
  const [loading, setLoading] = useState(false);
  const [messageApi, contextHolder] = message.useMessage();
  const [form] = Form.useForm(); // 新增
  const [passwordStrength, setPasswordStrength] = useState<PasswordStrength>({
    score: 0,
    level: "weak",
    color: "#ff4d4f",
    text: "请输入密码",
    percentage: 0,
  });

  const onFinish: FormProps<FieldType>["onFinish"] = async (values) => {
    try {
      setLoading(true);
      // username 和 password 不会为空因为已经在表单验证中处理过了
      const response = await register(
        values.email!,
        values.username!,
        values.password!,
        values.captcha!,
      );

      const data = await response.json();

      if (response.ok) {
        messageApi.success("注册成功！");

        // router.push("/dashboard"); // 登录成功后跳转
      } else {
        messageApi.error(data.error || "注册失败，请重试！");
      }
    } catch (error) {
      messageApi.error("注册出错，请稍后重试！");
      console.error("注册出错:", error);
    } finally {
      setLoading(false);
    }
  };

  const onFinishFailed: FormProps<FieldType>["onFinishFailed"] = (
    errorInfo,
  ) => {
    console.log("Failed:", errorInfo);
  };

  const handleSendCode = async () => {
    const email = form.getFieldValue("email");
    if (!email) {
      messageApi.error("请先输入邮箱！");
      return;
    }
    try {
      const response = await getCaptcha(email);
      const data = await response.json();

      if (response.ok) {
        messageApi.success("验证码已发送到您的邮箱！");
      } else {
        messageApi.error(data.message || "发送验证码失败，请重试！");
      }
    } catch (error) {
      messageApi.error("发送验证码出错，请稍后重试！");
      console.error("发送验证码出错:", error);
    }
  };

  const handlePasswordChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const password = e.target.value;
    const strength = checkPasswordStrength(password);
    setPasswordStrength(strength);
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
          maxWidth: "480px",
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
              注册账号
            </Typography.Title>
            <Typography.Text
              style={{
                color: "rgba(255, 255, 255, 0.8)",
                fontSize: "16px",
              }}
            >
              创建您的新账户
            </Typography.Text>
          </div>

          {/* 注册表单卡片 */}
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
              form={form}
              name="register"
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
                    电子邮箱
                  </span>
                }
                name="email"
                rules={[
                  { required: true, message: "请输入电子邮箱！" },
                  { type: "email", message: "请输入有效的邮箱地址!" },
                ]}
                validateFirst
                validateTrigger="onBlur"
                style={{ marginBottom: "24px" }}
                className="login-input"
              >
                <Flex gap="small">
                  <Input
                    placeholder="请输入邮箱"
                    style={{
                      backgroundColor: "rgba(255, 255, 255, 0.9)",
                      border: "1px solid rgba(255, 255, 255, 0.3)",
                      borderRadius: "12px",
                      transition: "all 0.3s ease",
                      flex: 1,
                    }}
                  />
                  <Button
                    type="default"
                    onClick={handleSendCode}
                    style={{
                      backgroundColor: "rgba(255, 255, 255, 0.9)",
                      border: "1px solid rgba(255, 255, 255, 0.3)",
                      borderRadius: "12px",
                      minWidth: "100px",
                    }}
                  >
                    获取验证码
                  </Button>
                </Flex>
              </Form.Item>

              <Form.Item
                label={
                  <span
                    style={{
                      color: "rgba(255, 255, 255, 0.9)",
                      fontWeight: 500,
                    }}
                  >
                    验证码
                  </span>
                }
                name="captcha"
                rules={[{ required: true, message: "请输入验证码！" }]}
                style={{ marginBottom: "24px" }}
                className="login-input"
              >
                <Input
                  placeholder="请输入验证码"
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
                rules={passwordValidationRules}
                style={{ marginBottom: "8px" }}
                className="login-input"
              >
                <Input.Password
                  placeholder="请输入密码"
                  onChange={handlePasswordChange}
                  style={{
                    backgroundColor: "rgba(255, 255, 255, 0.9)",
                    border: "1px solid rgba(255, 255, 255, 0.3)",
                    borderRadius: "12px",
                    transition: "all 0.3s ease",
                  }}
                />
              </Form.Item>

              {/* 密码强度指示器 */}
              <div style={{ marginBottom: "24px" }}>
                <div
                  style={{
                    display: "flex",
                    justifyContent: "space-between",
                    alignItems: "center",
                    marginBottom: "8px",
                  }}
                >
                  <span
                    style={{
                      color: "rgba(255, 255, 255, 0.8)",
                      fontSize: "12px",
                    }}
                  >
                    密码强度
                  </span>
                  <span
                    style={{
                      color: passwordStrength.color,
                      fontSize: "12px",
                      fontWeight: 500,
                    }}
                  >
                    {passwordStrength.text}
                  </span>
                </div>
                <Progress
                  percent={passwordStrength.percentage}
                  strokeColor={passwordStrength.color}
                  showInfo={false}
                  size="small"
                  trailColor="rgba(255, 255, 255, 0.3)"
                />
                <div
                  style={{
                    marginTop: "4px",
                    fontSize: "11px",
                    color: "rgba(255, 255, 255, 0.7)",
                  }}
                >
                  密码需至少8位，包含大小写字母和数字
                </div>
              </div>

              <Form.Item<FieldType>
                label={
                  <span
                    style={{
                      color: "rgba(255, 255, 255, 0.9)",
                      fontWeight: 500,
                    }}
                  >
                    确认密码
                  </span>
                }
                name="confirmPassword"
                rules={[
                  { required: true, message: "请确认密码！" },
                  ({ getFieldValue }) => ({
                    validator(_, value) {
                      if (!value || getFieldValue("password") === value) {
                        return Promise.resolve();
                      }
                      return Promise.reject(new Error("两次输入的密码不一致!"));
                    },
                  }),
                ]}
                style={{ marginBottom: "32px" }}
                className="login-input"
              >
                <Input.Password
                  placeholder="请再次输入密码"
                  style={{
                    backgroundColor: "rgba(255, 255, 255, 0.9)",
                    border: "1px solid rgba(255, 255, 255, 0.3)",
                    borderRadius: "12px",
                    transition: "all 0.3s ease",
                  }}
                />
              </Form.Item>

              {/* 注册按钮 */}
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
                  注册
                </Button>
              </Form.Item>

              {/* 登录链接 */}
              <div style={{ textAlign: "center" }}>
                <span style={{ color: "rgba(255, 255, 255, 0.8)" }}>
                  已有账号？{" "}
                  <Link
                    href="/"
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
                    立即登录
                  </Link>
                </span>
              </div>
            </Form>
          </div>
        </Flex>
      </div>
    </div>
  );
}
