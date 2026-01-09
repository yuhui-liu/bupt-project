"use client";
import { Button, Form, Input, Typography, Flex, message, Progress } from "antd";
import { useState } from "react";
import { sendResetCode, resetPassword } from "@/lib/fetchData";
import {
  checkPasswordStrength,
  passwordValidationRules,
  type PasswordStrength,
} from "@/lib/passwordUtils";
import Link from "next/link";

export default function ForgetPassword() {
  const [step, setStep] = useState(1); // 1: 输入邮箱, 2: 输入验证码和新密码
  const [email, setEmail] = useState("");
  const [isLoading, setIsLoading] = useState(false);
  const [form] = Form.useForm();
  const [messageApi, contextHolder] = message.useMessage();
  const [passwordStrength, setPasswordStrength] = useState<PasswordStrength>({
    score: 0,
    level: "weak",
    color: "#ff4d4f",
    text: "请输入密码",
    percentage: 0,
  });

  // 发送验证码
  const sendVerificationCode = async (values: { email: string }) => {
    setIsLoading(true);
    try {
      const response = await sendResetCode(values.email);

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || "发送验证码失败");
      }

      setEmail(values.email);
      setStep(2);
      messageApi.success("验证码已发送到您的邮箱，请查收！");
    } catch (error: any) {
      messageApi.error(error.message || "发送验证码失败，请重试");
    } finally {
      setIsLoading(false);
    }
  };

  // 重置密码
  const resetPasswordHandler = async (values: {
    code: string;
    password: string;
    confirmPassword: string;
  }) => {
    if (values.password !== values.confirmPassword) {
      messageApi.error("两次输入的密码不一致！");
      return;
    }

    setIsLoading(true);
    try {
      const response = await resetPassword(email, values.code, values.password);

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || "密码重置失败");
      }

      messageApi.success("密码重置成功！请使用新密码登录");
      // 可以跳转到登录页面
      // router.push('/login');
    } catch (error: any) {
      messageApi.error(error.message || "密码重置失败，请重试");
    } finally {
      setIsLoading(false);
    }
  };

  const onFinish = (values: any) => {
    if (step === 1) {
      sendVerificationCode(values);
    } else {
      resetPasswordHandler(values);
    }
  };

  const onFinishFailed = (errorInfo: any) => {
    console.log("Failed:", errorInfo);
  };

  const goBack = () => {
    setStep(1);
    form.resetFields();
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
              {step === 1 ? "忘记密码" : "重置密码"}
            </Typography.Title>
            <Typography.Text
              style={{
                color: "rgba(255, 255, 255, 0.8)",
                fontSize: "16px",
              }}
            >
              {step === 1 ? "输入您的邮箱以接收验证码" : "设置您的新密码"}
            </Typography.Text>
          </div>

          {/* 表单卡片 */}
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
            {step === 1 ? (
              <Form
                form={form}
                name="sendCode"
                layout="vertical"
                onFinish={onFinish}
                onFinishFailed={onFinishFailed}
                style={{ width: "100%" }}
                size="large"
              >
                <Form.Item
                  label={
                    <span
                      style={{
                        color: "rgba(255, 255, 255, 0.9)",
                        fontWeight: 500,
                      }}
                    >
                      邮箱
                    </span>
                  }
                  name="email"
                  rules={[
                    { required: true, message: "请输入您的邮箱!" },
                    { type: "email", message: "请输入有效的邮箱地址!" },
                  ]}
                  style={{ marginBottom: "32px" }}
                  className="login-input"
                >
                  <Input
                    placeholder="请输入您的邮箱"
                    style={{
                      backgroundColor: "rgba(255, 255, 255, 0.9)",
                      border: "1px solid rgba(255, 255, 255, 0.3)",
                      borderRadius: "12px",
                      transition: "all 0.3s ease",
                    }}
                  />
                </Form.Item>

                <Form.Item style={{ marginBottom: "24px" }}>
                  <Button
                    type="primary"
                    htmlType="submit"
                    block
                    loading={isLoading}
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
                    发送验证码
                  </Button>
                </Form.Item>

                {/* 返回登录链接 */}
                <div style={{ textAlign: "center" }}>
                  <span style={{ color: "rgba(255, 255, 255, 0.8)" }}>
                    想起密码了？{" "}
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
                        e.currentTarget.style.color =
                          "rgba(255, 255, 255, 0.9)";
                      }}
                    >
                      返回登录
                    </Link>
                  </span>
                </div>
              </Form>
            ) : (
              <Form
                form={form}
                name="resetPassword"
                layout="vertical"
                onFinish={onFinish}
                onFinishFailed={onFinishFailed}
                style={{ width: "100%" }}
                size="large"
              >
                <Typography.Text
                  style={{
                    marginBottom: 24,
                    display: "block",
                    color: "rgba(255, 255, 255, 0.8)",
                    textAlign: "center",
                    fontSize: "14px",
                  }}
                >
                  验证码已发送至：{email}
                </Typography.Text>

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
                  name="code"
                  rules={[
                    { required: true, message: "请输入验证码!" },
                    { len: 6, message: "验证码为6位数字!" },
                  ]}
                  style={{ marginBottom: "24px" }}
                  className="login-input"
                >
                  <Input
                    placeholder="请输入6位验证码"
                    maxLength={6}
                    style={{
                      backgroundColor: "rgba(255, 255, 255, 0.9)",
                      border: "1px solid rgba(255, 255, 255, 0.3)",
                      borderRadius: "12px",
                      transition: "all 0.3s ease",
                    }}
                  />
                </Form.Item>

                <Form.Item
                  label={
                    <span
                      style={{
                        color: "rgba(255, 255, 255, 0.9)",
                        fontWeight: 500,
                      }}
                    >
                      新密码
                    </span>
                  }
                  name="password"
                  rules={passwordValidationRules}
                  style={{ marginBottom: "8px" }}
                  className="login-input"
                >
                  <Input.Password
                    placeholder="请输入新密码"
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

                <Form.Item
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
                    { required: true, message: "请确认密码!" },
                    ({ getFieldValue }) => ({
                      validator(_, value) {
                        if (!value || getFieldValue("password") === value) {
                          return Promise.resolve();
                        }
                        return Promise.reject(
                          new Error("两次输入的密码不一致!"),
                        );
                      },
                    }),
                  ]}
                  style={{ marginBottom: "32px" }}
                  className="login-input"
                >
                  <Input.Password
                    placeholder="请再次输入新密码"
                    style={{
                      backgroundColor: "rgba(255, 255, 255, 0.9)",
                      border: "1px solid rgba(255, 255, 255, 0.3)",
                      borderRadius: "12px",
                      transition: "all 0.3s ease",
                    }}
                  />
                </Form.Item>

                <Form.Item style={{ marginBottom: "24px" }}>
                  <Flex gap="small">
                    <Button
                      onClick={goBack}
                      style={{
                        flex: 1,
                        height: "48px",
                        borderRadius: "12px",
                        backgroundColor: "rgba(255, 255, 255, 0.9)",
                        border: "1px solid rgba(255, 255, 255, 0.3)",
                        fontSize: "16px",
                        fontWeight: 500,
                      }}
                    >
                      返回
                    </Button>
                    <Button
                      type="primary"
                      htmlType="submit"
                      loading={isLoading}
                      className="login-button"
                      style={{
                        flex: 1,
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
                      重置密码
                    </Button>
                  </Flex>
                </Form.Item>
              </Form>
            )}
          </div>
        </Flex>
      </div>
    </div>
  );
}
