import axiosInstance from "./axios";

export async function register(
  email: string,
  username: string,
  password: string,
  captcha: string,
): Promise<Response> {
  const response = await fetch(
    `${process.env.NEXT_PUBLIC_API_URL}/api/register`,
    {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ email, username, password, captcha }),
    },
  );
  return response;
}

export async function login(
  username: string,
  password: string,
): Promise<Response> {
  const response = await fetch(`${process.env.NEXT_PUBLIC_API_URL}/api/login`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ username, password }),
  });
  return response;
}

export async function getMyInfo(): Promise<any> {
  const response = await axiosInstance.get("/api/myinfo");
  return response;
}

export async function approveUser(username: string): Promise<any> {
  const response = await axiosInstance.post("/api/pass", { username });
  return response;
}

export async function rejectUser(username: string): Promise<any> {
  const response = await axiosInstance.post("/api/reject", { username });
  return response;
}

export async function getCaptcha(email: string): Promise<Response> {
  const response = await fetch(
    `${process.env.NEXT_PUBLIC_API_URL}/api/getCaptcha`,
    {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ email }),
    },
  );
  return response;
}

export async function sendResetCode(email: string): Promise<Response> {
  const response = await fetch(
    `${process.env.NEXT_PUBLIC_API_URL}/api/send_reset_code`,
    {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ email }),
    },
  );
  return response;
}

export async function resetPassword(
  email: string,
  verification_code: string,
  new_password: string,
): Promise<Response> {
  const response = await fetch(
    `${process.env.NEXT_PUBLIC_API_URL}/api/reset_password`,
    {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ email, verification_code, new_password }),
    },
  );
  return response;
}

export async function changePassword(
  username: string,
  currentPassword: string,
  newPassword: string,
): Promise<any> {
  const response = await axiosInstance.post(
    "/api/reset_password_with_old_password",
    {
      username: username,
      old_password: currentPassword,
      new_password: newPassword,
    },
  );
  return response;
}

export async function deleteAccount(password: string): Promise<any> {
  const response = await axiosInstance.post("/api/delete_account", {
    password: password,
  });
  return response;
}

export async function getSystemInfo() {
  const response = await axiosInstance.get("/api/systeminfo");
  return response;
}

// 新增：获取所有客户姓名列表（返回字符串数组）
export async function getCustomers(): Promise<any> {
  const response = await axiosInstance.get("/api/customers");
  return response;
}

export async function getCustomerInfo(customerName: string): Promise<any> {
  const response = await axiosInstance.post("/api/query", {
    type: "customer_info",
    customerName: customerName,
  });
  return response;
}
