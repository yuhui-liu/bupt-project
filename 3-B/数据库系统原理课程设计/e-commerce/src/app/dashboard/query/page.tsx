"use client";

import {
  Button,
  Card,
  Input,
  Select,
  Typography,
  Tabs,
  Space,
  List,
  Modal,
  message,
  Table,
  Spin,
  DatePicker,
  Radio,
  Form,
} from "antd";
import { useState } from "react";
import {
  UserOutlined,
  SearchOutlined,
  HistoryOutlined,
  DownloadOutlined,
  FolderOutlined,
} from "@ant-design/icons";
import { Dayjs } from "dayjs";
import { Pie, Line } from "@ant-design/plots";

import axiosInstance from "@/lib/axios";

const { TextArea } = Input;
const { Title, Text } = Typography;

const predefinedQueries = [
  {
    title: "订单优先级查询",
    description:
      "用以查询指定日期范围内，实际送达日期晚于预计到达日期的各个优先级订单的订单数量，并根据订单优先级升序排序。",
    sql: `SELECT
    o_orderpriority,
    COUNT(*) AS order_count
FROM
    orders
WHERE
    o_orderdate >= DATE '1970-01-01'
    AND o_orderdate < DATE '2020-01-01'
    AND EXISTS (
        SELECT
            1
        FROM
            lineitem
        WHERE
            l_orderkey = o_orderkey
            AND l_commitdate < l_receiptdate
    )
GROUP BY
    o_orderpriority
ORDER BY
    o_orderpriority;`,
    query_name: "order_priority",
  },
  {
    title: "零件利润查询",
    description:
      "给出一个零件 key，查询该零件在不同国家，不同年份的利润，返回国家，年份和利润，按照国家升序，年份降序。",
    sql: `SELECT
    nation,
    o_year,
    SUM(amount) AS sum_profit
FROM (
    SELECT
        n_name AS nation,
        EXTRACT(YEAR FROM o_orderdate) AS o_year,
        l_extendedprice * (1 - l_discount) - ps_supplycost * l_quantity AS amount
    FROM
        part,
        supplier,
        lineitem,
        partsupp,
        orders,
        nation
    WHERE
        s_suppkey = l_suppkey
        AND ps_suppkey = l_suppkey
        AND ps_partkey = l_partkey
        AND p_partkey = l_partkey
        AND o_orderkey = l_orderkey
        AND s_nationkey = n_nationkey
        AND p_name LIKE '%green%'
) AS profit
GROUP BY
    nation,
    o_year
ORDER BY
    nation ASC,
    o_year DESC;`,
    query_name: "part_profit",
  },
];

// 添加客户接口定义
interface Customer {
  custkey: number;
  name: string;
  address: string;
  nation: string;
  phone: string;
  acctbal: string;
  mktsegment: string;
  comment: string;
}

// 添加查询结果接口定义
interface QueryResult {
  columns: string[];
  data: any[][];
}

export default function Page() {
  const [sqlModalVisible, setSqlModalVisible] = useState(false);
  const [selectedSql, setSelectedSql] = useState("");
  const [selectedTitle, setSelectedTitle] = useState("");
  const [loading, setLoading] = useState(false);

  // 新增状态
  const [customerNameInput, setCustomerNameInput] = useState("");
  const [selectedCustomerFromDropdown, setSelectedCustomerFromDropdown] =
    useState<string | undefined>();
  const [customerOptions, setCustomerOptions] = useState<
    Array<{ value: string; label: string }>
  >([]);
  const [customersLoading, setCustomersLoading] = useState(false);
  const [customerQueryLoading, setCustomerQueryLoading] = useState(false);
  const [queryResult, setQueryResult] = useState<QueryResult | null>(null);
  const [customerInfo, setCustomerInfo] = useState<Customer | null>(null);
  const [queryResultVisible, setQueryResultVisible] = useState(false);
  const [customerInfoVisible, setCustomerInfoVisible] = useState(false);

  // 新增：订单优先级查询的起止日期
  const [orderPriorityStartDate, setOrderPriorityStartDate] =
    useState<Dayjs | null>(null);
  const [orderPriorityEndDate, setOrderPriorityEndDate] =
    useState<Dayjs | null>(null);

  // 新增：part_profit 查询的 p_name 状态
  const [partProfitPName, setPartProfitPName] = useState<string>("green");

  // 新增：保存查询结果相关状态
  const [savingResult, setSavingResult] = useState(false);
  const [saveModalVisible, setSaveModalVisible] = useState(false);
  const [saveFileName, setSaveFileName] = useState("");
  const [saveFormat, setSaveFormat] = useState<"csv" | "json">("csv");
  const [exportMethod, setExportMethod] = useState("browser_download");
  const [currentSaveData, setCurrentSaveData] = useState<any>(null);
  const [saveDataType, setSaveDataType] = useState<"query" | "customer">(
    "query",
  );

  const [messageApi, contextHolder] = message.useMessage();

  // 检测浏览器对 File System Access API 的支持
  const isFileSystemAPISupported = (): boolean => {
    return "showSaveFilePicker" in window;
  };

  // 导出方式选项
  const getExportOptions = () => {
    const isAPISupported = isFileSystemAPISupported();

    return [
      {
        value: "browser_download",
        label: "浏览器默认下载",
        icon: <DownloadOutlined />,
        description: "文件将保存到浏览器的默认下载文件夹",
      },
      {
        value: "file_system_api",
        label: "选择保存位置",
        icon: (
          <FolderOutlined
            style={{ color: isAPISupported ? "#52c41a" : "#ff4d4f" }}
          />
        ),
        description: isAPISupported
          ? "让您选择具体的保存位置和文件名"
          : "您的浏览器不支持此功能，请使用 Chrome/Edge 最新版本",
        disabled: !isAPISupported,
      },
    ];
  };

  // 使用 File System Access API 保存文件
  const saveFileWithAPI = async (
    blob: Blob,
    fileName: string,
  ): Promise<boolean> => {
    try {
      if (!isFileSystemAPISupported()) {
        throw new Error("浏览器不支持 File System Access API");
      }

      const fileExtension = fileName.includes(".")
        ? fileName.split(".").pop()
        : saveFormat;
      const fileHandle = await (window as any).showSaveFilePicker({
        suggestedName: fileName,
        types: [
          {
            description: `${fileExtension?.toUpperCase()} 文件`,
            accept: {
              [`${fileExtension === "json" ? "application/json" : "text/csv"}`]:
                [`.${fileExtension}`],
            },
          },
        ],
      });

      const writable = await fileHandle.createWritable();
      await writable.write(blob);
      await writable.close();

      return true;
    } catch (error) {
      if ((error as any).name === "AbortError") {
        // 用户取消了文件选择
        return false;
      }
      throw error;
    }
  };

  // 使用浏览器默认下载
  const downloadFile = (blob: Blob, fileName: string) => {
    const link = document.createElement("a");
    link.href = window.URL.createObjectURL(blob);
    link.download = fileName;
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    window.URL.revokeObjectURL(link.href);
  };

  // 获取订单优先级查询的动态SQL
  function getOrderPrioritySQL(start: Dayjs | null, end: Dayjs | null) {
    const defaultStart = "1970-01-01";
    const defaultEnd = "2020-01-01";
    const startDate = start ? start.format("YYYY-MM-DD") : defaultStart;
    const endDate = end ? end.format("YYYY-MM-DD") : defaultEnd;
    return `SELECT\n    o_orderpriority,\n    COUNT(*) AS order_count\nFROM\n    orders\nWHERE\n    o_orderdate >= DATE '${startDate}'\n    AND o_orderdate < DATE '${endDate}'\n    AND EXISTS (\n        SELECT\n            1\n        FROM\n            lineitem\n        WHERE\n            l_orderkey = o_orderkey\n            AND l_commitdate < l_receiptdate\n    )\nGROUP BY\n    o_orderpriority\nORDER BY\n    o_orderpriority;`;
  }

  // 获取 part_profit 查询的动态 SQL
  function getPartProfitSQL(pName: string) {
    const safePName = pName && pName.trim() ? pName.trim() : "green";
    // 只替换 LIKE '%green%' 部分
    return predefinedQueries
      .find((q) => q.query_name === "part_profit")!
      .sql.replace(/LIKE '%.*?%'/, `LIKE '%${safePName}%'`);
  }

  const handleViewSql = (item: (typeof predefinedQueries)[0]) => {
    if (item.query_name === "order_priority") {
      setSelectedSql(
        getOrderPrioritySQL(orderPriorityStartDate, orderPriorityEndDate),
      );
    } else if (item.query_name === "part_profit") {
      setSelectedSql(getPartProfitSQL(partProfitPName));
    } else {
      setSelectedSql(item.sql);
    }
    setSelectedTitle(item.title);
    setSqlModalVisible(true);
  };

  const executeQuery = async (queryName: string) => {
    setLoading(true);
    try {
      const postData: any = {
        type: "predefined",
        query_name: queryName,
      };
      if (queryName === "order_priority") {
        postData.additional = {
          start_date: orderPriorityStartDate
            ? orderPriorityStartDate.format("YYYY-MM-DD")
            : undefined,
          end_date: orderPriorityEndDate
            ? orderPriorityEndDate.format("YYYY-MM-DD")
            : undefined,
        };
        postData.sql = getOrderPrioritySQL(
          orderPriorityStartDate,
          orderPriorityEndDate,
        );
      } else if (queryName === "part_profit") {
        postData.additional = {
          p_name:
            partProfitPName && partProfitPName.trim()
              ? partProfitPName.trim()
              : "green",
        };
        postData.sql = getPartProfitSQL(postData.additional.p_name);
      }
      const response = await axiosInstance.post("/api/query", postData);
      messageApi.success("查询执行成功");
      setQueryResult(response.data);
      setQueryResultVisible(true);
    } catch (error) {
      console.error("查询执行失败:", error);
      messageApi.error("查询执行失败，请稍后重试");
    } finally {
      setLoading(false);
    }
  };

  // 新增：获取所有客户名字的函数
  const fetchCustomers = async () => {
    if (customerOptions.length > 0) {
      // 如果已经加载过数据，就不重复加载
      return;
    }

    setCustomersLoading(true);
    try {
      const response = await axiosInstance.get("/api/customers");
      // 响应格式为 ['tom', 'alice', 'jerry']
      const customers = response.data.map((customerName: string) => ({
        value: customerName,
        label: customerName,
      }));
      setCustomerOptions(customers);
    } catch (error) {
      console.error("获取客户列表失败:", error);
      messageApi.error("获取客户列表失败，请稍后重试");
    } finally {
      setCustomersLoading(false);
    }
  };

  // 新增：根据客户姓名查询客户信息
  const queryCustomerInfo = async (
    customerName: string,
    queryType: "input" | "dropdown",
  ) => {
    console.log(queryType);
    if (!customerName.trim()) {
      messageApi.warning("请输入或选择客户姓名");
      return;
    }

    setCustomerQueryLoading(true);
    try {
      const response = await axiosInstance.post("/api/query", {
        type: "customer_info",
        customerName: customerName.trim(),
      });

      messageApi.success(`查询客户 "${customerName}" 信息成功`);
      setCustomerInfo(response.data);
      setCustomerInfoVisible(true);
    } catch (error) {
      console.error("客户信息查询失败:", error);
      messageApi.error("客户信息查询失败，请稍后重试");
    } finally {
      setCustomerQueryLoading(false);
    }
  };

  // 新增：渲染查询结果表格
  const renderQueryResult = () => {
    if (!queryResult) return null;

    // 判断是否为对象数组格式
    let dataSource;
    let columns;
    if (
      queryResult.data.length > 0 &&
      typeof queryResult.data[0] === "object" &&
      !Array.isArray(queryResult.data[0])
    ) {
      // 对象数组格式
      dataSource = queryResult.data.map((row: any, index: number) => ({
        key: index,
        ...row,
      }));
      columns = queryResult.columns.map((col) => ({
        title: col,
        dataIndex: col,
        key: col,
        render: (text: any) => text?.toString() || "",
      }));
    } else {
      // 兼容原二维数组格式
      columns = queryResult.columns.map((col, index) => ({
        title: col,
        dataIndex: index,
        key: index,
        render: (text: any) => text?.toString() || "",
      }));
      dataSource = queryResult.data.map((row, index) => ({
        key: index,
        ...row.reduce((acc, cell, cellIndex) => {
          acc[cellIndex] = cell;
          return acc;
        }, {} as any),
      }));
    }

    return (
      <Table
        columns={columns}
        dataSource={dataSource}
        pagination={{
          pageSize: 10,
          showSizeChanger: true,
          showQuickJumper: true,
          showTotal: (total) => `共 ${total} 条数据`,
        }}
        scroll={{ x: "max-content" }}
        size="small"
      />
    );
  };

  // 新增：渲染客户信息
  const renderCustomerInfo = () => {
    if (!customerInfo) return null;

    const customerData = [
      { label: "客户编号", value: customerInfo.custkey },
      { label: "客户姓名", value: customerInfo.name },
      { label: "地址", value: customerInfo.address },
      { label: "国家", value: customerInfo.nation },
      { label: "电话", value: customerInfo.phone },
      { label: "账户余额", value: `$${customerInfo.acctbal}` },
      { label: "市场细分", value: customerInfo.mktsegment },
      { label: "备注", value: customerInfo.comment },
    ];

    return (
      <div style={{ padding: "16px 0" }}>
        {customerData.map((item, index) => (
          <div
            key={index}
            style={{
              display: "flex",
              marginBottom: "12px",
              padding: "8px 0",
              borderBottom:
                index < customerData.length - 1 ? "1px solid #f0f0f0" : "none",
            }}
          >
            <div
              style={{
                width: "120px",
                fontWeight: "bold",
                color: "#666",
                flexShrink: 0,
              }}
            >
              {item.label}:
            </div>
            <div style={{ flex: 1, wordBreak: "break-word" }}>{item.value}</div>
          </div>
        ))}
      </div>
    );
  };

  // 新增：渲染订单优先级饼状图（适配对象数组格式）
  const renderOrderPriorityPie = () => {
    if (
      !queryResult ||
      !Array.isArray(queryResult.data) ||
      queryResult.data.length === 0
    ) {
      return (
        <div style={{ textAlign: "center", color: "#aaa", padding: "32px 0" }}>
          暂无数据
        </div>
      );
    }
    // 判断是否为订单优先级查询的结构
    const isOrderPriority =
      Array.isArray(queryResult.columns) &&
      queryResult.columns.length === 2 &&
      queryResult.columns[0] === "o_orderpriority" &&
      queryResult.columns[1] === "order_count";
    if (!isOrderPriority) return null;
    // 适配对象数组格式
    const data = queryResult.data.map((row: any) => ({
      o_orderpriority: row.o_orderpriority,
      order_count: Number(row.order_count),
    }));
    // 调试输出
    console.log("Pie data:", data);
    if (!data.length) {
      return (
        <div style={{ textAlign: "center", color: "#aaa", padding: "32px 0" }}>
          暂无数据
        </div>
      );
    }
    const config = {
      appendPadding: 10,
      data,
      angleField: "order_count",
      colorField: "o_orderpriority",
      radius: 0.9,
      label: {
        position: "spider",
        text: (d: { o_orderpriority: any; order_count: any }) =>
          `${d.o_orderpriority}\n ${d.order_count}`,
      },
      interactions: [{ type: "element-active" }],
      style: { width: "100%", minHeight: 400 },
    };
    return <Pie {...config} />;
  };

  // 新增：渲染零件利润折线图（适配对象数组格式）
  const renderPartProfitLine = () => {
    if (
      !queryResult ||
      !Array.isArray(queryResult.data) ||
      queryResult.data.length === 0
    ) {
      return null;
    }
    // 判断是否为 part_profit 查询的结构
    const isPartProfit =
      Array.isArray(queryResult.columns) &&
      queryResult.columns.length === 3 &&
      queryResult.columns[0] === "nation" &&
      queryResult.columns[1] === "o_year" &&
      queryResult.columns[2] === "sum_profit";
    if (!isPartProfit) return null;
    // 适配对象数组格式
    const data = queryResult.data.map((row: any) => ({
      nation: row.nation,
      o_year: row.o_year,
      sum_profit: Number(row.sum_profit),
    }));
    const config = {
      data,
      xField: "o_year",
      yField: "sum_profit",
      seriesField: "nation",
      smooth: true,
      interaction: {
        tooltip: {
          showCrosshairs: true,
          shared: true,
        },
      },
      point: { size: 4, shape: "circle" },
    };
    return <Line {...config} />;
  };

  // 新增：打开保存模态框
  const openSaveModal = (data: any, type: "query" | "customer") => {
    setCurrentSaveData(data);
    setSaveDataType(type);
    setSaveFileName(
      type === "query"
        ? `query_result_${new Date().getTime()}`
        : `customer_info_${new Date().getTime()}`,
    );
    setSaveModalVisible(true);
  };

  // 新增：保存数据功能（统一处理查询结果和客户信息）
  const saveData = async () => {
    if (!currentSaveData) {
      messageApi.warning("没有数据可以保存");
      return;
    }

    setSavingResult(true);
    try {
      let content: string;
      let filename: string;
      let mimeType: string;

      if (saveFormat === "csv") {
        // 生成CSV格式
        if (saveDataType === "query") {
          // 查询结果
          const headers = currentSaveData.columns.join(",");
          const rows = currentSaveData.data.map((row: any) => {
            if (typeof row === "object" && !Array.isArray(row)) {
              // 对象格式
              return currentSaveData.columns
                .map((col: string) => {
                  const value = row[col];
                  const stringValue = String(value || "");
                  // 处理包含逗号或引号的值
                  if (stringValue.includes(",") || stringValue.includes('"')) {
                    return `"${stringValue.replace(/"/g, '""')}"`;
                  }
                  return stringValue;
                })
                .join(",");
            } else if (Array.isArray(row)) {
              // 数组格式
              return row
                .map((cell) => {
                  const stringValue = String(cell || "");
                  if (stringValue.includes(",") || stringValue.includes('"')) {
                    return `"${stringValue.replace(/"/g, '""')}"`;
                  }
                  return stringValue;
                })
                .join(",");
            }
            return "";
          });
          content = [headers, ...rows].join("\n");
        } else {
          // 客户信息
          const headers = ["字段", "值"];
          const customerData = [
            ["客户编号", currentSaveData.custkey],
            ["客户姓名", currentSaveData.name],
            ["地址", currentSaveData.address],
            ["国家", currentSaveData.nation],
            ["电话", currentSaveData.phone],
            ["账户余额", `$${currentSaveData.acctbal}`],
            ["市场细分", currentSaveData.mktsegment],
            ["备注", currentSaveData.comment],
          ];
          const rows = customerData.map(([field, value]) => {
            const stringValue = String(value || "");
            const escapedValue =
              stringValue.includes(",") || stringValue.includes('"')
                ? `"${stringValue.replace(/"/g, '""')}"`
                : stringValue;
            return `"${field}",${escapedValue}`;
          });
          content = [headers.map((h) => `"${h}"`).join(","), ...rows].join(
            "\n",
          );
        }
        filename = saveFileName.includes(".csv")
          ? saveFileName
          : `${saveFileName}.csv`;
        mimeType = "text/csv;charset=utf-8;";
      } else {
        // 生成JSON格式
        let jsonData: any;
        if (saveDataType === "query") {
          jsonData = {
            timestamp: new Date().toISOString(),
            type: "query_result",
            columns: currentSaveData.columns,
            data: currentSaveData.data,
            total: currentSaveData.data.length,
          };
        } else {
          jsonData = {
            timestamp: new Date().toISOString(),
            type: "customer_info",
            data: currentSaveData,
          };
        }
        content = JSON.stringify(jsonData, null, 2);
        filename = saveFileName.includes(".json")
          ? saveFileName
          : `${saveFileName}.json`;
        mimeType = "application/json;charset=utf-8;";
      }

      // 创建 Blob
      const blob = new Blob([content], { type: mimeType });

      // 根据选择的导出方式处理文件
      if (exportMethod === "file_system_api") {
        const saved = await saveFileWithAPI(blob, filename);
        if (saved) {
          messageApi.success("文件已保存到您选择的位置");
          setSaveModalVisible(false);
        }
        // 如果用户取消了，不显示任何消息
      } else {
        // 默认使用浏览器下载
        downloadFile(blob, filename);
        messageApi.success("文件已开始下载，请检查您的下载文件夹");
        setSaveModalVisible(false);
      }
    } catch (error) {
      // 如果 File System API 失败，降级到默认下载
      if (exportMethod === "file_system_api") {
        try {
          let content: string;
          let filename: string;
          let mimeType: string;

          // 重新生成文件内容（这里可以复用上面的逻辑，但为了简化直接重复）
          if (saveFormat === "csv") {
            if (saveDataType === "query") {
              const headers = currentSaveData.columns.join(",");
              const rows = currentSaveData.data.map((row: any) => {
                if (typeof row === "object" && !Array.isArray(row)) {
                  return currentSaveData.columns
                    .map((col: string) => {
                      const value = row[col];
                      const stringValue = String(value || "");
                      if (
                        stringValue.includes(",") ||
                        stringValue.includes('"')
                      ) {
                        return `"${stringValue.replace(/"/g, '""')}"`;
                      }
                      return stringValue;
                    })
                    .join(",");
                } else if (Array.isArray(row)) {
                  return row
                    .map((cell) => {
                      const stringValue = String(cell || "");
                      if (
                        stringValue.includes(",") ||
                        stringValue.includes('"')
                      ) {
                        return `"${stringValue.replace(/"/g, '""')}"`;
                      }
                      return stringValue;
                    })
                    .join(",");
                }
                return "";
              });
              content = [headers, ...rows].join("\n");
            } else {
              const headers = ["字段", "值"];
              const customerData = [
                ["客户编号", currentSaveData.custkey],
                ["客户姓名", currentSaveData.name],
                ["地址", currentSaveData.address],
                ["国家", currentSaveData.nation],
                ["电话", currentSaveData.phone],
                ["账户余额", `$${currentSaveData.acctbal}`],
                ["市场细分", currentSaveData.mktsegment],
                ["备注", currentSaveData.comment],
              ];
              const rows = customerData.map(([field, value]) => {
                const stringValue = String(value || "");
                const escapedValue =
                  stringValue.includes(",") || stringValue.includes('"')
                    ? `"${stringValue.replace(/"/g, '""')}"`
                    : stringValue;
                return `"${field}",${escapedValue}`;
              });
              content = [headers.map((h) => `"${h}"`).join(","), ...rows].join(
                "\n",
              );
            }
            filename = saveFileName.includes(".csv")
              ? saveFileName
              : `${saveFileName}.csv`;
            mimeType = "text/csv;charset=utf-8;";
          } else {
            let jsonData: any;
            if (saveDataType === "query") {
              jsonData = {
                timestamp: new Date().toISOString(),
                type: "query_result",
                columns: currentSaveData.columns,
                data: currentSaveData.data,
                total: currentSaveData.data.length,
              };
            } else {
              jsonData = {
                timestamp: new Date().toISOString(),
                type: "customer_info",
                data: currentSaveData,
              };
            }
            content = JSON.stringify(jsonData, null, 2);
            filename = saveFileName.includes(".json")
              ? saveFileName
              : `${saveFileName}.json`;
            mimeType = "application/json;charset=utf-8;";
          }

          const blob = new Blob([content], { type: mimeType });
          downloadFile(blob, filename);
          messageApi.warning("保存文件失败，已自动使用默认下载方式");
          setSaveModalVisible(false);
        } catch (downloadError) {
          messageApi.error("保存失败，请稍后重试");
          console.error(downloadError);
        }
      } else {
        messageApi.error("保存失败，请稍后重试");
      }
      console.error("保存数据失败:", error);
    } finally {
      setSavingResult(false);
    }
  };

  return (
    <div style={{ padding: "24px" }}>
      {contextHolder}
      <Title level={2}>数据库查询界面</Title>

      <Tabs
        defaultActiveKey="1"
        items={[
          {
            key: "1",
            label: (
              <span>
                <UserOutlined />
                客户信息查询
              </span>
            ),
            children: (
              <Card>
                <Space
                  direction="vertical"
                  style={{ width: "100%" }}
                  size="large"
                >
                  {/* 方式1: 手动输入客户姓名 */}
                  <div>
                    <Text strong>方式1: 输入客户姓名查询</Text>
                    <Space
                      style={{ width: "100%", marginTop: 8 }}
                      direction="vertical"
                    >
                      <Input
                        placeholder="请输入客户姓名..."
                        style={{ width: "100%" }}
                        value={customerNameInput}
                        onChange={(e) => setCustomerNameInput(e.target.value)}
                        onPressEnter={() =>
                          queryCustomerInfo(customerNameInput, "input")
                        }
                      />
                      <Button
                        type="primary"
                        icon={<SearchOutlined />}
                        loading={customerQueryLoading}
                        onClick={() =>
                          queryCustomerInfo(customerNameInput, "input")
                        }
                      >
                        查询客户信息
                      </Button>
                    </Space>
                  </div>

                  {/* 方式2: 下拉框选择客户姓名 */}
                  <div>
                    <Text strong>方式2: 从列表中选择客户</Text>
                    <Space
                      style={{ width: "100%", marginTop: 8 }}
                      direction="vertical"
                    >
                      <Select
                        showSearch
                        style={{ width: "100%" }}
                        placeholder="点击展开选择客户..."
                        optionFilterProp="label"
                        value={selectedCustomerFromDropdown}
                        onChange={setSelectedCustomerFromDropdown}
                        onDropdownVisibleChange={(open) => {
                          if (open) {
                            fetchCustomers();
                          }
                        }}
                        loading={customersLoading}
                        options={customerOptions}
                        filterOption={(input, option) =>
                          (option?.label ?? "")
                            .toLowerCase()
                            .includes(input.toLowerCase())
                        }
                        notFoundContent={
                          customersLoading ? <Spin size="small" /> : "暂无数据"
                        }
                      />
                      <Button
                        type="primary"
                        icon={<SearchOutlined />}
                        loading={customerQueryLoading}
                        disabled={!selectedCustomerFromDropdown}
                        onClick={() =>
                          selectedCustomerFromDropdown &&
                          queryCustomerInfo(
                            selectedCustomerFromDropdown,
                            "dropdown",
                          )
                        }
                      >
                        查询客户信息
                      </Button>
                    </Space>
                  </div>
                </Space>
              </Card>
            ),
          },
          {
            key: "2",
            label: (
              <span>
                <HistoryOutlined />
                预定义查询
              </span>
            ),
            children: (
              <List
                itemLayout="vertical"
                dataSource={predefinedQueries}
                renderItem={(item) => (
                  <List.Item
                    actions={[
                      item.query_name === "order_priority" && (
                        <Space key="datepickers" style={{ marginRight: 16 }}>
                          <DatePicker
                            placeholder="起始日期"
                            value={orderPriorityStartDate}
                            onChange={setOrderPriorityStartDate}
                            allowClear
                          />
                          <DatePicker
                            placeholder="结束日期"
                            value={orderPriorityEndDate}
                            onChange={setOrderPriorityEndDate}
                            allowClear
                          />
                        </Space>
                      ),
                      item.query_name === "part_profit" && (
                        <Space key="pname" style={{ marginRight: 16 }}>
                          <Input
                            placeholder="请输入零件名称关键词，如 red"
                            value={partProfitPName}
                            onChange={(e) => {
                              setPartProfitPName(e.target.value);
                              // 实时更新SQL预览
                              const safePName =
                                e.target.value && e.target.value.trim()
                                  ? e.target.value.trim()
                                  : "green";
                              const sql = getPartProfitSQL(safePName);
                              // 只在当前为 part_profit 时更新SQL
                              if (selectedTitle === item.title) {
                                setSelectedSql(sql);
                              }
                            }}
                            style={{ width: 180 }}
                            allowClear
                          />
                        </Space>
                      ),
                      <Button
                        type="primary"
                        key="run"
                        loading={loading}
                        onClick={() => executeQuery(item.query_name)}
                      >
                        执行查询
                      </Button>,
                      <Button key="view" onClick={() => handleViewSql(item)}>
                        查看SQL
                      </Button>,
                    ].filter(Boolean)}
                  >
                    <List.Item.Meta
                      title={item.title}
                      description={item.description}
                    />
                  </List.Item>
                )}
              />
            ),
          },
        ]}
      />

      <Modal
        title={`SQL查询 - ${selectedTitle}`}
        open={sqlModalVisible}
        onCancel={() => setSqlModalVisible(false)}
        footer={[
          <Button
            key="copy"
            onClick={() => navigator.clipboard.writeText(selectedSql)}
          >
            复制SQL
          </Button>,
          <Button key="close" onClick={() => setSqlModalVisible(false)}>
            关闭
          </Button>,
        ]}
        width={800}
      >
        <div style={{ marginBottom: 16 }}>
          <Text type="secondary">SQL语句：</Text>
        </div>
        <TextArea
          value={selectedSql}
          rows={6}
          readOnly
          style={{
            fontFamily: "monospace",
            backgroundColor: "#f5f5f5",
          }}
        />
      </Modal>

      {/* 新增：查询结果展示模态框 */}
      <Modal
        title="查询结果"
        open={queryResultVisible}
        onCancel={() => setQueryResultVisible(false)}
        footer={[
          <Button
            key="save"
            icon={<DownloadOutlined />}
            onClick={() => openSaveModal(queryResult, "query")}
            disabled={!queryResult || queryResult.data.length === 0}
          >
            保存查询结果
          </Button>,
          <Button key="close" onClick={() => setQueryResultVisible(false)}>
            关闭
          </Button>,
        ]}
        width="90%"
        style={{ top: 20 }}
      >
        {/* 饼状图展示，仅订单优先级查询时显示 */}
        {renderOrderPriorityPie()}
        {/* 折线图展示，仅零件利润查询时显示 */}
        {renderPartProfitLine()}
        {/* 表格展示 */}
        {renderQueryResult()}
      </Modal>

      {/* 新增：客户信息展示模态框 */}
      <Modal
        title={`客户信息 - ${customerInfo?.name}`}
        open={customerInfoVisible}
        onCancel={() => setCustomerInfoVisible(false)}
        footer={[
          <Button
            key="save"
            icon={<DownloadOutlined />}
            onClick={() => openSaveModal(customerInfo, "customer")}
            disabled={!customerInfo}
          >
            保存客户信息
          </Button>,
          <Button key="close" onClick={() => setCustomerInfoVisible(false)}>
            关闭
          </Button>,
        ]}
        width={600}
      >
        {renderCustomerInfo()}
      </Modal>

      {/* 新增：保存数据模态框 */}
      <Modal
        title={`保存${saveDataType === "query" ? "查询结果" : "客户信息"}`}
        open={saveModalVisible}
        onCancel={() => setSaveModalVisible(false)}
        footer={[
          <Button key="cancel" onClick={() => setSaveModalVisible(false)}>
            取消
          </Button>,
          <Button
            key="save"
            type="primary"
            loading={savingResult}
            onClick={saveData}
          >
            保存
          </Button>,
        ]}
        width={600}
        zIndex={1010}
      >
        <Form layout="vertical">
          <Form.Item label="文件名">
            <Input
              value={saveFileName}
              onChange={(e) => setSaveFileName(e.target.value)}
              placeholder="请输入文件名"
              suffix={`.${saveFormat}`}
            />
          </Form.Item>

          <Form.Item label="文件格式">
            <Radio.Group
              value={saveFormat}
              onChange={(e) => setSaveFormat(e.target.value)}
            >
              <Radio value="csv">CSV 格式</Radio>
              <Radio value="json">JSON 格式</Radio>
            </Radio.Group>
          </Form.Item>

          <Form.Item label="保存方式">
            <Radio.Group
              value={exportMethod}
              onChange={(e) => setExportMethod(e.target.value)}
            >
              {getExportOptions().map((option) => (
                <Radio
                  key={option.value}
                  value={option.value}
                  disabled={option.disabled}
                  style={{ display: "block", marginBottom: 8 }}
                >
                  <Space>
                    {option.icon}
                    <span>{option.label}</span>
                  </Space>
                  <div
                    style={{
                      fontSize: "12px",
                      color: "#666",
                      marginLeft: 24,
                      marginTop: 4,
                    }}
                  >
                    {option.description}
                  </div>
                </Radio>
              ))}
            </Radio.Group>
          </Form.Item>
        </Form>
      </Modal>
    </div>
  );
}
