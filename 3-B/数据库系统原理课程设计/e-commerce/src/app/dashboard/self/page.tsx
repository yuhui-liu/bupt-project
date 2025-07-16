"use client";

import {
  Typography,
  Card,
  Descriptions,
  Space,
  Button,
  Modal,
  Form,
  Input,
  message,
  Progress,
} from "antd";
import { UserOutlined, LockOutlined, DeleteOutlined } from "@ant-design/icons";
import { useEffect, useState } from "react";
import { getMyInfo, changePassword, deleteAccount } from "@/lib/fetchData";
import {
  checkPasswordStrength,
  passwordValidationRules,
  type PasswordStrength,
} from "@/lib/passwordUtils";

export default function Page() {
  const [messageApi, contextHolder] = message.useMessage();
  const [userInfo, setUserInfo] = useState({
    username: "",
    email: "",
    applyDate: "",
    passDate: "",
  });
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchUserInfo = async () => {
      try {
        setLoading(true);
        const data = (await getMyInfo()).data;
        console.log(data);
        setUserInfo({
          username: data.username || "",
          email: data.email || "",
          applyDate: data.apply_date || "",
          passDate: data.pass_date || "",
        });
      } catch (error) {
        console.error("获取用户信息失败:", error);
        messageApi.error("获取用户信息失败");
      } finally {
        setLoading(false);
      }
    };

    fetchUserInfo();
  }, []);

  const [isPasswordModalVisible, setIsPasswordModalVisible] = useState(false);
  const [isDeleteModalVisible, setIsDeleteModalVisible] = useState(false);
  const [form] = Form.useForm();
  const [passwordStrength, setPasswordStrength] = useState<PasswordStrength>({
    score: 0,
    level: "weak",
    color: "#ff4d4f",
    text: "请输入密码",
    percentage: 0,
  });
  const [passwordLoading, setPasswordLoading] = useState(false);
  const [deleteLoading, setDeleteLoading] = useState(false);
  const [deleteForm] = Form.useForm();

  const handlePasswordUpdate = async (values: any) => {
    try {
      setPasswordLoading(true);
      const response = await changePassword(
        userInfo.username,
        values.currentPassword,
        values.newPassword,
      );

      if (response.status === 200) {
        messageApi.success("密码修改成功！");
        setIsPasswordModalVisible(false);
        form.resetFields();
        setPasswordStrength({
          score: 0,
          level: "weak",
          color: "#ff4d4f",
          text: "请输入密码",
          percentage: 0,
        });
      } else {
        const errorData = response.data;
        messageApi.error(errorData.message || "密码修改失败");
      }
    } catch (error: any) {
      console.error("密码修改失败:", error);
      messageApi.error(error.response?.data?.message || "密码修改失败，请重试");
    } finally {
      setPasswordLoading(false);
    }
  };

  const handlePasswordChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const password = e.target.value;
    const strength = checkPasswordStrength(password);
    setPasswordStrength(strength);
  };

  const handleAccountDelete = async (values: any) => {
    try {
      setDeleteLoading(true);
      const response = await deleteAccount(values.password);

      if (response.status === 200) {
        messageApi.success("账号已注销");
        setIsDeleteModalVisible(false);
        deleteForm.resetFields();
        // 清除本地存储的token
        localStorage.removeItem("token");
        // 跳转到登录页面
        window.location.href = "/";
      } else {
        const errorData = response.data;
        messageApi.error(errorData.message || "账号注销失败");
      }
    } catch (error: any) {
      console.error("账号注销失败:", error);
      messageApi.error(error.response?.data?.message || "账号注销失败，请重试");
    } finally {
      setDeleteLoading(false);
    }
  };

  return (
    <>
      {contextHolder}
      <div style={{ padding: "24px", maxWidth: "800px", margin: "0 auto" }}>
        <Card loading={loading}>
          <Space direction="vertical" size="large" style={{ width: "100%" }}>
            <Typography.Title level={2}>
              <UserOutlined style={{ marginRight: "8px" }} />
              个人中心
            </Typography.Title>

            <Descriptions
              bordered
              column={1}
              styles={{ label: { width: "120px", fontWeight: "bold" } }}
            >
              <Descriptions.Item label="用户名">
                {userInfo.username || "加载中..."}
              </Descriptions.Item>
              <Descriptions.Item label="邮箱">
                {userInfo.email || "加载中..."}
              </Descriptions.Item>
              <Descriptions.Item label="注册时间">
                {userInfo.applyDate || "加载中..."}
              </Descriptions.Item>
              <Descriptions.Item label="通过时间">
                {userInfo.passDate || "加载中..."}
              </Descriptions.Item>
            </Descriptions>

            <div
              style={{
                display: "flex",
                gap: "8px",
                justifyContent: "flex-end",
              }}
            >
              <Button
                type="primary"
                icon={<LockOutlined />}
                onClick={() => setIsPasswordModalVisible(true)}
                disabled={loading}
              >
                修改密码
              </Button>
              <Button
                danger
                icon={<DeleteOutlined />}
                onClick={() => setIsDeleteModalVisible(true)}
                disabled={loading}
              >
                注销账号
              </Button>
            </div>
          </Space>
        </Card>

        {/* Password Change Modal */}
        <Modal
          title="修改密码"
          open={isPasswordModalVisible}
          onOk={() => form.submit()}
          onCancel={() => {
            setIsPasswordModalVisible(false);
            form.resetFields();
            setPasswordStrength({
              score: 0,
              level: "weak",
              color: "#ff4d4f",
              text: "请输入密码",
              percentage: 0,
            });
          }}
          confirmLoading={passwordLoading}
          okText="确认修改"
          cancelText="取消"
        >
          <Form form={form} onFinish={handlePasswordUpdate} layout="vertical">
            <Form.Item
              name="currentPassword"
              label="当前密码"
              rules={[{ required: true, message: "请输入当前密码" }]}
            >
              <Input.Password />
            </Form.Item>
            <Form.Item
              name="newPassword"
              label="新密码"
              rules={passwordValidationRules}
              style={{ marginBottom: "8px" }}
            >
              <Input.Password onChange={handlePasswordChange} />
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
                    color: "#666",
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
                trailColor="#f0f0f0"
              />
              <div
                style={{
                  marginTop: "4px",
                  fontSize: "11px",
                  color: "#999",
                }}
              >
                密码需至少8位，包含大小写字母和数字
              </div>
            </div>

            <Form.Item
              name="confirmPassword"
              label="确认新密码"
              dependencies={["newPassword"]}
              rules={[
                { required: true, message: "请确认新密码" },
                ({ getFieldValue }) => ({
                  validator(_, value) {
                    if (!value || getFieldValue("newPassword") === value) {
                      return Promise.resolve();
                    }
                    return Promise.reject(new Error("两次输入的密码不一致"));
                  },
                }),
              ]}
            >
              <Input.Password />
            </Form.Item>
          </Form>
        </Modal>

        {/* Account Deletion Modal */}
        <Modal
          title="注销账号"
          open={isDeleteModalVisible}
          onOk={() => deleteForm.submit()}
          onCancel={() => {
            setIsDeleteModalVisible(false);
            deleteForm.resetFields();
          }}
          okText="确认注销"
          cancelText="取消"
          confirmLoading={deleteLoading}
          okButtonProps={{ danger: true }}
        >
          <Form
            form={deleteForm}
            onFinish={handleAccountDelete}
            layout="vertical"
          >
            <Typography.Paragraph type="danger">
              注意：账号注销后将无法恢复，请谨慎操作！
            </Typography.Paragraph>
            <Form.Item
              name="password"
              label="请输入密码以确认注销"
              rules={[{ required: true, message: "请输入密码" }]}
            >
              <Input.Password />
            </Form.Item>
          </Form>
        </Modal>
      </div>
    </>
  );
}
