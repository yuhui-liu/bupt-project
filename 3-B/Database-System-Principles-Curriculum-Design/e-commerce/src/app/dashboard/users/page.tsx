"use client";

import axiosInstance from "@/lib/axios";
import { approveUser, rejectUser } from "@/lib/fetchData";
import { Flex, Space, Typography, Table, Button, message } from "antd";
import Column from "antd/es/table/Column";
import { useEffect, useState } from "react";

interface UserInfo {
  key: number;
  username: string;
  email: string;
  applyDate: Date | null;
  passDate: Date | null;
  isAdmin: boolean;
}

interface WaitingInfo {
  key: number;
  username: string;
  email: string;
  applyDate: Date;
}

export default function UserPage() {
  const [userData, setUserData] = useState<UserInfo[]>([]);
  const [waitingData, setWaitingData] = useState<WaitingInfo[]>([]);
  const [loading, setLoading] = useState(true);
  const [waitingLoading, setWaitingLoading] = useState(true);
  const [messageApi, contextHolder] = message.useMessage();

  const fetchUserData = async () => {
    try {
      const response = await axiosInstance.get("/api/userinfo");
      setUserData(
        response.data.map((item: any) => ({
          key: item.key,
          username: item.username,
          email: item.email,
          applyDate: item.applyDate ? new Date(item.applyDate) : null,
          passDate: item.passDate ? new Date(item.passDate) : null,
          isAdmin: item.isAdmin,
        })),
      );
    } catch (error) {
      console.error("获取用户数据失败:", error);
    } finally {
      setLoading(false);
    }
  };

  const fetchWaitingData = async () => {
    try {
      const response = await axiosInstance.get("/api/waitlist");
      setWaitingData(
        response.data.map((item: any) => ({
          key: item.key,
          username: item.username,
          email: item.email,
          applyDate: new Date(item.applyDate),
        })),
      );
    } catch (error) {
      console.error("获取待通过用户数据失败:", error);
    } finally {
      setWaitingLoading(false);
    }
  };

  useEffect(() => {
    fetchUserData();
    fetchWaitingData();
  }, []);

  const handleDelete = async (username: string) => {
    try {
      await axiosInstance.post("/api/delete", { username });
      await fetchUserData();
      messageApi.success("用户删除成功");
    } catch (error) {
      console.error("删除用户失败:", error);
    }
  };

  const handlePass = async (username: string) => {
    try {
      console.log("通过用户:", username);
      const response = await approveUser(username);
      if (!(response.status >= 200 && response.status < 300)) {
        const errorData = await response.data;
        throw new Error(errorData.error || "通过用户失败");
      }
      await fetchUserData();
      await fetchWaitingData();
      messageApi.success("用户通过成功");
    } catch (error) {
      console.error("删除用户失败:", error);
      messageApi.error(error as string);
    }
  };
  const handleReject = async (username: string) => {
    try {
      console.log("拒绝用户:", username);
      const response = await rejectUser(username);
      if (!(response.status >= 200 && response.status < 300)) {
        const errorData = await response.data;
        throw new Error(errorData.error || "拒绝用户失败");
      }
      await fetchUserData();
      await fetchWaitingData();
      messageApi.success("用户拒绝成功");
    } catch (error) {
      console.error("拒绝用户失败:", error);
      messageApi.error(error as string);
    }
  };

  return (
    <>
      {contextHolder}
      <Flex justify="space-around" gap={"small"}>
        <Flex vertical>
          <Typography.Title level={3} className="pl-3">
            已注册用户
          </Typography.Title>
          <Table<UserInfo>
            loading={loading}
            dataSource={userData}
            scroll={{ y: 380 }}
          >
            <Column title="用户名" dataIndex={"username"} />
            <Column title="邮箱" dataIndex={"email"} />
            <Column
              title="申请日期"
              dataIndex={"applyDate"}
              render={(date: Date | null) =>
                date ? date.toLocaleDateString("zh") : "-"
              }
            />
            <Column
              title="通过日期"
              dataIndex={"passDate"}
              render={(date: Date | null) =>
                date ? date.toLocaleDateString("zh") : "-"
              }
            />
            <Column
              title=""
              render={(_, record: UserInfo) =>
                record.isAdmin ? (
                  "管理员"
                ) : (
                  <Space>
                    <Button
                      danger
                      onClick={() => handleDelete(record.username)}
                    >
                      删除
                    </Button>
                  </Space>
                )
              }
            />
          </Table>
        </Flex>
        <Flex vertical>
          <Typography.Title level={3} className="pl-3">
            待确认用户
          </Typography.Title>
          <Table<WaitingInfo>
            loading={waitingLoading}
            dataSource={waitingData}
            scroll={{ y: 380 }}
          >
            <Column title="用户名" dataIndex={"username"} />
            <Column title="邮箱" dataIndex={"email"} />
            <Column
              title="申请日期"
              dataIndex={"applyDate"}
              render={(date: Date | null) =>
                date ? date.toLocaleDateString("zh") : "-"
              }
            />
            <Column
              title="是否通过"
              render={(_, record: UserInfo) => (
                <Space>
                  <Button
                    type="primary"
                    style={{ backgroundColor: "#52c41a" }}
                    onClick={() => {
                      handlePass(record.username);
                    }}
                  >
                    同意
                  </Button>
                  <Button
                    danger
                    onClick={() => {
                      handleReject(record.username);
                    }}
                  >
                    拒绝
                  </Button>
                </Space>
              )}
            />
          </Table>
        </Flex>
      </Flex>
    </>
  );
}
