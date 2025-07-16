"use client";

import { useState, useCallback } from "react";
import {
  Typography,
  Tabs,
  Select,
  Button,
  Card,
  Space,
  Row,
  Col,
  DatePicker,
  message,
  Table,
  Statistic,
  Collapse,
  InputNumber,
  Radio,
} from "antd";
import {
  PlayCircleOutlined,
  DatabaseOutlined,
  ClockCircleOutlined,
  EyeOutlined,
  ShoppingCartOutlined,
  CreditCardOutlined,
} from "@ant-design/icons";
import axiosInstance from "@/lib/axios";

const { TabPane } = Tabs;
const { Title, Paragraph, Text } = Typography;
const { Option } = Select;
const { Panel } = Collapse;

export default function Page() {
  const [messageApi, contextHolder] = message.useMessage();
  const [nations, setNations] = useState<string[]>([]);
  const [nationsFetched, setNationsFetched] = useState(false);
  const [regions, setRegions] = useState<string[]>([]);
  const [regionsFetched, setRegionsFetched] = useState(false);
  const [partTypes, setPartTypes] = useState<string[]>([]);
  const [partTypesFetched, setPartTypesFetched] = useState(false);
  const [batchShipmentLoading, setBatchShipmentLoading] = useState(false);
  const [marketShareLoading, setMarketShareLoading] = useState(false);
  const [batchShipmentPlanLoading, setBatchShipmentPlanLoading] =
    useState(false);
  const [marketSharePlanLoading, setMarketSharePlanLoading] = useState(false);
  const [batchShipmentResult, setBatchShipmentResult] = useState<{
    columns: string[];
    data: any[];
    exec_time: number;
  } | null>(null);
  const [marketShareResult, setMarketShareResult] = useState<{
    columns: string[];
    data: any[];
    exec_time: number;
  } | null>(null);
  const [batchShipmentPlan, setBatchShipmentPlan] = useState<string | null>(
    null
  );
  const [marketSharePlan, setMarketSharePlan] = useState<string | null>(null);
  const [tpccLoading, setTpccLoading] = useState(false);
  const [tpccRequestData, setTpccRequestData] = useState<{
    terminal: number;
    limitTxnsPerMin: number;
    runTxnsPerTerminal: number;
    runMins?: number;
    runMode: 'transaction' | 'time'; // 新增运行模式字段
  }>({
    terminal: 2,
    limitTxnsPerMin: 200,
    runTxnsPerTerminal: 30,
    runMins: undefined,
    runMode: 'transaction', // 默认按事务数量模式
  });
  const [newOrderLoading, setNewOrderLoading] = useState(false);
  const [paymentLoading, setPaymentLoading] = useState(false);
  const [newOrderResult, setNewOrderResult] = useState<{
    message: string;
    exec_time: number;
  } | null>(null);
  const [paymentResult, setPaymentResult] = useState<{
    message: string;
    exec_time: number;
  } | null>(null);
  const [newOrderRequestData, setNewOrderRequestData] = useState<{
    w_id: number;
    d_id: number;
    o_ol_cnt: number;
    c_id: number;
  }>({
    w_id: 1,
    d_id: 1,
    o_ol_cnt: 5,
    c_id: 1,
  });
  const [paymentRequestData, setPaymentRequestData] = useState<{
    w_id: number;
    d_id: number;
    c_id: number;
  }>({
    w_id: 1,
    d_id: 1,
    c_id: 1,
  });
  const [batchShipmentRequestData, setBatchShipmentRequestData] = useState<{
    startDate: string | undefined;
    endDate: string | undefined;
    supplierNation: string | undefined;
    customerNation: string | undefined;
  }>({
    startDate: undefined,
    endDate: undefined,
    supplierNation: undefined,
    customerNation: undefined,
  });
  const [marketShareRequestData, setMarketShareRequestData] = useState<{
    nation: string | undefined;
    region: string | undefined;
    partType: string | undefined;
    startDate: string | undefined;
    endDate: string | undefined;
  }>({
    nation: undefined,
    region: undefined,
    partType: undefined,
    startDate: undefined,
    endDate: undefined,
  });

  const fetchNations = useCallback(async () => {
    if (nationsFetched) return;

    try {
      const response = await axiosInstance.get("/api/nations");
      setNations(response.data);
      setNationsFetched(true);
    } catch (error) {
      console.error("获取国家数据失败:", error);
    }
  }, [nationsFetched]);

  const fetchRegions = useCallback(async () => {
    if (regionsFetched) return;

    try {
      const response = await axiosInstance.get("/api/regions");
      setRegions(response.data);
      setRegionsFetched(true);
    } catch (error) {
      console.error("获取地区数据失败:", error);
    }
  }, [regionsFetched]);

  const fetchPartTypes = useCallback(async () => {
    if (partTypesFetched) return;

    try {
      const response = await axiosInstance.get("/api/p_types");
      setPartTypes(response.data);
      setPartTypesFetched(true);
    } catch (error) {
      console.error("获取零件类型数据失败:", error);
    }
  }, [partTypesFetched]);

  // 解析查询计划文本的函数
  const parseQueryPlan = (planText: string) => {
    const lines = planText.split("\n");
    const planningTimeMatch = planText.match(/Planning Time: ([\d.]+) ms/);
    const executionTimeMatch = planText.match(/Execution Time: ([\d.]+) ms/);

    const planLines = lines.filter(
      (line) =>
        !line.includes("Planning Time:") &&
        !line.includes("Execution Time:") &&
        line.trim() !== ""
    );

    return {
      planLines,
      planningTime: planningTimeMatch ? parseFloat(planningTimeMatch[1]) : null,
      executionTime: executionTimeMatch
        ? parseFloat(executionTimeMatch[1])
        : null,
    };
  };

  // 格式化查询计划行的函数
  const formatPlanLine = (line: string) => {
    const indent = line.match(/^(\s*)/)?.[1]?.length || 0;
    const cleanLine = line.trim();

    // 识别不同类型的操作
    if (cleanLine.includes("Seq Scan") || cleanLine.includes("Index Scan")) {
      return { type: "scan", indent, text: cleanLine };
    } else if (
      cleanLine.includes("Hash Join") ||
      cleanLine.includes("Nested Loop")
    ) {
      return { type: "join", indent, text: cleanLine };
    } else if (
      cleanLine.includes("Sort") ||
      cleanLine.includes("GroupAggregate")
    ) {
      return { type: "aggregate", indent, text: cleanLine };
    } else if (cleanLine.includes("->")) {
      return { type: "operation", indent, text: cleanLine };
    } else {
      return { type: "detail", indent, text: cleanLine };
    }
  };

  const handleBatchShipment = async () => {
    // 这里可以添加批量出货查询的逻辑
    console.log("执行批量出货查询");
    if (
      !batchShipmentRequestData.startDate ||
      !batchShipmentRequestData.endDate ||
      !batchShipmentRequestData.supplierNation ||
      !batchShipmentRequestData.customerNation
    ) {
      console.error("请填写所有查询条件");
      messageApi.error("请填写所有查询条件");
      return;
    }

    setBatchShipmentLoading(true);
    try {
      const response = await axiosInstance.post("/api/tpc-h_analysis", {
        ...batchShipmentRequestData,
        predefined: "batch_shipment",
      });
      console.log("查询结果:", response.data);
      setBatchShipmentResult(response.data);
      messageApi.success("查询成功！");
    } catch (error) {
      console.error("查询失败:", error);
      messageApi.error("查询失败，请稍后重试");
    } finally {
      setBatchShipmentLoading(false);
    }
  };

  const handleMarketShare = async () => {
    console.log("执行全国市场份额查询");
    if (
      !marketShareRequestData.nation ||
      !marketShareRequestData.region ||
      !marketShareRequestData.partType ||
      !marketShareRequestData.startDate ||
      !marketShareRequestData.endDate
    ) {
      console.error("请填写所有查询条件");
      messageApi.error("请填写所有查询条件");
      return;
    }

    setMarketShareLoading(true);
    try {
      const response = await axiosInstance.post("/api/tpc-h_analysis", {
        ...marketShareRequestData,
        predefined: "market_share",
      });
      console.log("查询结果:", response.data);
      setMarketShareResult(response.data);
      messageApi.success("查询成功！");
    } catch (error) {
      console.error("查询失败:", error);
      messageApi.error("查询失败，请稍后重试");
    } finally {
      setMarketShareLoading(false);
    }
  };

  const handleBatchShipmentPlan = async () => {
    console.log("查看批量出货查询计划");
    if (
      !batchShipmentRequestData.startDate ||
      !batchShipmentRequestData.endDate ||
      !batchShipmentRequestData.supplierNation ||
      !batchShipmentRequestData.customerNation
    ) {
      console.error("请填写所有查询条件");
      messageApi.error("请填写所有查询条件");
      return;
    }

    setBatchShipmentPlanLoading(true);
    try {
      const response = await axiosInstance.post("/api/tpc-h_analysis", {
        ...batchShipmentRequestData,
        predefined: "batch_shipment",
        explain: true,
      });
      console.log("查询计划:", response.data);
      setBatchShipmentPlan(response.data.data || response.data);
      messageApi.success("查询计划获取成功！");
    } catch (error) {
      console.error("获取查询计划失败:", error);
      messageApi.error("获取查询计划失败，请稍后重试");
    } finally {
      setBatchShipmentPlanLoading(false);
    }
  };

  const handleMarketSharePlan = async () => {
    console.log("查看全国市场份额查询计划");
    if (
      !marketShareRequestData.nation ||
      !marketShareRequestData.region ||
      !marketShareRequestData.partType ||
      !marketShareRequestData.startDate ||
      !marketShareRequestData.endDate
    ) {
      console.error("请填写所有查询条件");
      messageApi.error("请填写所有查询条件");
      return;
    }

    setMarketSharePlanLoading(true);
    try {
      const response = await axiosInstance.post("/api/tpc-h_analysis", {
        ...marketShareRequestData,
        predefined: "market_share",
        explain: true,
      });
      console.log("查询计划:", response.data);
      setMarketSharePlan(response.data.data || response.data);
      messageApi.success("查询计划获取成功！");
    } catch (error) {
      console.error("获取查询计划失败:", error);
      messageApi.error("获取查询计划失败，请稍后重试");
    } finally {
      setMarketSharePlanLoading(false);
    }
  };

  const handleTpccConcurrent = async () => {
    console.log("执行TPC-C并发事务测试");

    // 验证参数
    if (!tpccRequestData.terminal || !tpccRequestData.limitTxnsPerMin) {
      messageApi.error("请填写终端数量和事务限制");
      return;
    }

    if (tpccRequestData.runMode === 'transaction' && (!tpccRequestData.runTxnsPerTerminal || tpccRequestData.runTxnsPerTerminal <= 0)) {
      messageApi.error("请设置每终端运行事务数");
      return;
    }

    if (tpccRequestData.runMode === 'time' && (!tpccRequestData.runMins || tpccRequestData.runMins <= 0)) {
      messageApi.error("请设置运行分钟数");
      return;
    }

    // 打开新窗口显示等待页面
    const newWindow = window.open(
      "",
      "_blank",
      "width=800,height=600,scrollbars=yes"
    );
    if (!newWindow) {
      messageApi.error("无法打开新窗口，请检查浏览器弹窗设置");
      return;
    }

    // 构建参数信息
    const paramInfo = `
      <h3>测试参数：</h3>
      <p>终端数量：${tpccRequestData.terminal}</p>
      <p>每分钟事务限制：${tpccRequestData.limitTxnsPerMin}</p>
      <p>运行模式：${tpccRequestData.runMode === 'transaction' ? '按事务数量' : '按运行时间'}</p>
      ${
        tpccRequestData.runMode === 'time'
          ? `<p>运行分钟数：${tpccRequestData.runMins}</p>`
          : `<p>每终端运行事务数：${tpccRequestData.runTxnsPerTerminal}</p>`
      }
    `;

    // 在新窗口中显示等待页面
    newWindow.document.write(`
      <html>
        <head>
          <title>TPC-C 并发事务测试</title>
          <style>
            body { 
              font-family: Arial, sans-serif; 
              display: flex; 
              justify-content: center; 
              align-items: center; 
              height: 100vh; 
              margin: 0; 
              background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
              color: white;
            }
            .container { 
              text-align: center; 
              padding: 40px;
              background: rgba(255,255,255,0.1);
              border-radius: 20px;
              backdrop-filter: blur(10px);
            }
            .spinner {
              border: 4px solid rgba(255,255,255,0.3);
              border-top: 4px solid white;
              border-radius: 50%;
              width: 50px;
              height: 50px;
              animation: spin 1s linear infinite;
              margin: 20px auto;
            }
            @keyframes spin {
              0% { transform: rotate(0deg); }
              100% { transform: rotate(360deg); }
            }
            .params {
              background: rgba(255,255,255,0.1);
              padding: 20px;
              border-radius: 10px;
              margin: 20px 0;
              text-align: left;
            }
          </style>
        </head>
        <body>
          <div class="container">
            <h1>🔄 TPC-C 并发事务测试进行中...</h1>
            <div class="spinner"></div>
            <p>测试可能需要较长时间，请耐心等待...</p>
            <div class="params">
              ${paramInfo}
            </div>
            <div id="status">正在初始化测试...</div>
          </div>
        </body>
      </html>
    `);

    setTpccLoading(true);

    try {
      const startTime = Date.now();

      // 更新状态
      if (newWindow.document.getElementById("status")) {
        newWindow.document.getElementById("status")!.innerHTML =
          "正在发送请求到服务器...";
      }

      // 构建请求参数
      const requestPayload: any = {
        terminal: tpccRequestData.terminal,
        limitTxnsPerMin: tpccRequestData.limitTxnsPerMin,
      };

      // 根据选择的模式添加对应参数
      if (tpccRequestData.runMode === 'time') {
        requestPayload.runMins = tpccRequestData.runMins;
      } else {
        requestPayload.runTxnsPerTerminal = tpccRequestData.runTxnsPerTerminal;
      }

      const response = await axiosInstance.post(
        "/api/tpc-c_concurrent",
        requestPayload
      );

      const endTime = Date.now();
      const duration = ((endTime - startTime) / 1000).toFixed(2);

      console.log("TPC-C测试结果:", response.data);

      // 检查响应状态并处理数据
      let displayContent = "";
      if (response.status === 200 && response.data) {
        // 如果响应成功，尝试获取stdout字段
        if (response.data.stdout) {
          displayContent = response.data.stdout;
        } else {
          // 如果没有stdout字段，显示完整的响应数据
          displayContent = JSON.stringify(response.data, null, 2);
        }
      } else {
        displayContent = "未获取到有效的测试结果";
      }

      // 在新窗口中显示结果
      newWindow.document.body.innerHTML = `
        <div style="
          font-family: Arial, sans-serif; 
          padding: 20px; 
          background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
          color: white;
          min-height: 100vh;
        ">
          <div style="
            background: rgba(255,255,255,0.1);
            padding: 30px;
            border-radius: 20px;
            backdrop-filter: blur(10px);
          ">
            <h1>✅ TPC-C 并发事务测试完成</h1>
            
            <div style="
              background: rgba(255,255,255,0.1);
              padding: 20px;
              border-radius: 10px;
              margin: 20px 0;
            ">
              <h3>测试参数：</h3>
              <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
                <p>终端数量：${tpccRequestData.terminal}</p>
                <p>每分钟事务限制：${tpccRequestData.limitTxnsPerMin}</p>
                <p>运行模式：${tpccRequestData.runMode === 'transaction' ? '按事务数量' : '按运行时间'}</p>
                ${
                  tpccRequestData.runMode === 'time'
                    ? `<p>运行分钟数：${tpccRequestData.runMins}</p>`
                    : `<p>每终端运行事务数：${tpccRequestData.runTxnsPerTerminal}</p>`
                }
                <p>测试耗时：${duration} 秒</p>
              </div>
            </div>

            <div style="
              background: rgba(255,255,255,0.1);
              padding: 20px;
              border-radius: 10px;
              margin: 20px 0;
            ">
              <h3>测试结果：</h3>
              <pre style="
                background: rgba(0,0,0,0.3);
                padding: 15px;
                border-radius: 8px;
                white-space: pre-wrap;
                word-wrap: break-word;
                max-height: 400px;
                overflow-y: auto;
                font-family: 'Courier New', monospace;
                line-height: 1.4;
              ">${displayContent}</pre>
            </div>

            <div style="text-align: center; margin-top: 30px;">
              <button onclick="window.close()" style="
                background: rgba(255,255,255,0.2);
                border: 1px solid rgba(255,255,255,0.3);
                color: white;
                padding: 10px 20px;
                border-radius: 5px;
                cursor: pointer;
                font-size: 16px;
              ">关闭窗口</button>
            </div>
          </div>
        </div>
      `;

      messageApi.success("TPC-C测试完成！结果已在新窗口中显示");
    } catch (error) {
      console.error("TPC-C测试失败:", error);

      // 在新窗口中显示错误
      newWindow.document.body.innerHTML = `
        <div style="
          font-family: Arial, sans-serif; 
          padding: 20px; 
          background: linear-gradient(135deg, #ff6b6b 0%, #ee5a24 100%);
          color: white;
          min-height: 100vh;
        ">
          <div style="
            background: rgba(255,255,255,0.1);
            padding: 30px;
            border-radius: 20px;
            backdrop-filter: blur(10px);
            text-align: center;
          ">
            <h1>❌ TPC-C 并发事务测试失败</h1>
            <p>测试过程中发生错误，请检查服务器状态或稍后重试</p>
            
            <div style="
              background: rgba(255,255,255,0.1);
              padding: 20px;
              border-radius: 10px;
              margin: 20px 0;
              text-align: left;
            ">
              <h3>错误信息：</h3>
              <pre style="
                background: rgba(0,0,0,0.3);
                padding: 15px;
                border-radius: 8px;
                white-space: pre-wrap;
                word-wrap: break-word;
              ">${error instanceof Error ? error.message : String(error)}</pre>
            </div>

            <button onclick="window.close()" style="
              background: rgba(255,255,255,0.2);
              border: 1px solid rgba(255,255,255,0.3);
              color: white;
              padding: 10px 20px;
              border-radius: 5px;
              cursor: pointer;
              font-size: 16px;
            ">关闭窗口</button>
          </div>
        </div>
      `;

      messageApi.error("TPC-C测试失败，请稍后重试");
    } finally {
      setTpccLoading(false);
    }
  };

  const handleNewOrder = async () => {
    console.log("执行新建订单事务");

    // 验证客户ID范围
    if (newOrderRequestData.c_id < 1 || newOrderRequestData.c_id > 3000) {
      messageApi.error("客户ID必须在1-3000范围内");
      return;
    }

    setNewOrderLoading(true);

    try {
      const response = await axiosInstance.post("/api/tpc-c_transaction", {
        name: "neword",
        w_id: newOrderRequestData.w_id,
        d_id: newOrderRequestData.d_id,
        o_ol_cnt: newOrderRequestData.o_ol_cnt,
        c_id: newOrderRequestData.c_id,
      });

      console.log("新建订单事务结果:", response.data);
      setNewOrderResult(response.data);
      messageApi.success("新建订单事务执行成功！");
    } catch (error) {
      console.error("新建订单事务失败:", error);
      messageApi.error("新建订单事务执行失败，请稍后重试");
    } finally {
      setNewOrderLoading(false);
    }
  };

  const handlePayment = async () => {
    console.log("执行支付事务");

    // 验证客户ID范围
    if (paymentRequestData.c_id < 1 || paymentRequestData.c_id > 3000) {
      messageApi.error("客户ID必须在1-3000范围内");
      return;
    }

    setPaymentLoading(true);

    try {
      const response = await axiosInstance.post("/api/tpc-c_transaction", {
        name: "payment",
        w_id: paymentRequestData.w_id,
        d_id: paymentRequestData.d_id,
        c_id: paymentRequestData.c_id,
      });

      console.log("支付事务结果:", response.data);
      setPaymentResult(response.data);
      messageApi.success("支付事务执行成功！");
    } catch (error) {
      console.error("支付事务失败:", error);
      messageApi.error("支付事务执行失败，请稍后重试");
    } finally {
      setPaymentLoading(false);
    }
  };

  return (
    <>
      {contextHolder}
      <Title level={2}>数据库性能分析</Title>
      <Tabs defaultActiveKey="1" type="card">
        <TabPane tab="TPC-H 统计分析" key="1">
          <div style={{ padding: "20px" }}>
            <Row gutter={[16, 24]}>
              <Col span={24}>
                <Card
                  title={
                    <Space>
                      <DatabaseOutlined />
                      <span>批量出货查询</span>
                    </Space>
                  }
                  variant="borderless"
                  style={{ marginBottom: "16px" }}
                >
                  <Paragraph style={{ color: "#666", marginBottom: "16px" }}>
                    给定时间和供应商所在的国家和客户所在的国家，查找出在该段时间内两个国家出货的总收入，返回供应国家名，客户国家名，年份，和出货的收入，结果按照供应国家名，客户国家名，年份升序。
                  </Paragraph>

                  <Row gutter={[16, 16]}>
                    <Col xs={24} sm={12} md={6}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        开始时间:
                      </label>
                      <DatePicker
                        placeholder="选择开始日期"
                        style={{ width: "100%" }}
                        size="middle"
                        format="YYYY-MM-DD"
                        onChange={(date, dateString) => {
                          setBatchShipmentRequestData((prev) => ({
                            ...prev,
                            startDate:
                              typeof dateString === "string" ? dateString : "",
                          }));
                        }}
                      />
                    </Col>
                    <Col xs={24} sm={12} md={6}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        结束时间:
                      </label>
                      <DatePicker
                        placeholder="选择结束日期"
                        style={{ width: "100%" }}
                        size="middle"
                        format="YYYY-MM-DD"
                        onChange={(date, dateString) => {
                          setBatchShipmentRequestData((prev) => ({
                            ...prev,
                            endDate:
                              typeof dateString === "string" ? dateString : "",
                          }));
                        }}
                      />
                    </Col>
                    <Col xs={24} sm={12} md={6}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        供应商国家:
                      </label>
                      <Select
                        placeholder="选择供应商国家"
                        style={{ width: "100%" }}
                        size="middle"
                        onDropdownVisibleChange={(open) => {
                          if (open) {
                            fetchNations();
                          }
                        }}
                        onChange={(value) => {
                          setBatchShipmentRequestData((prev) => ({
                            ...prev,
                            supplierNation: value,
                          }));
                        }}
                      >
                        {nations.map((nation) => (
                          <Option key={nation} value={nation}>
                            {nation}
                          </Option>
                        ))}
                      </Select>
                    </Col>
                    <Col xs={24} sm={12} md={6}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        客户国家:
                      </label>
                      <Select
                        placeholder="选择客户国家"
                        style={{ width: "100%" }}
                        size="middle"
                        onDropdownVisibleChange={(open) => {
                          if (open) {
                            fetchNations();
                          }
                        }}
                        onChange={(value) => {
                          setBatchShipmentRequestData((prev) => ({
                            ...prev,
                            customerNation: value,
                          }));
                        }}
                      >
                        {nations.map((nation) => (
                          <Option key={nation} value={nation}>
                            {nation}
                          </Option>
                        ))}
                      </Select>
                    </Col>
                  </Row>

                  <div style={{ marginTop: "20px", textAlign: "right" }}>
                    <Space>
                      <Button
                        icon={<EyeOutlined />}
                        size="middle"
                        loading={batchShipmentPlanLoading}
                        disabled={
                          batchShipmentLoading ||
                          marketShareLoading ||
                          batchShipmentPlanLoading ||
                          marketSharePlanLoading
                        }
                        onClick={handleBatchShipmentPlan}
                      >
                        查看查询计划
                      </Button>
                      <Button
                        type="primary"
                        icon={<PlayCircleOutlined />}
                        size="middle"
                        loading={batchShipmentLoading}
                        disabled={
                          batchShipmentLoading ||
                          marketShareLoading ||
                          batchShipmentPlanLoading ||
                          marketSharePlanLoading
                        }
                        style={{
                          background:
                            "linear-gradient(135deg, #667eea 0%, #764ba2 100%)",
                          border: "none",
                          borderRadius: "6px",
                        }}
                        onClick={handleBatchShipment}
                      >
                        执行查询
                      </Button>
                    </Space>
                  </div>
                </Card>

                {/* 批量出货查询结果展示 */}
                {batchShipmentResult && (
                  <Card
                    title={
                      <Space>
                        <DatabaseOutlined />
                        <span>批量出货查询结果</span>
                      </Space>
                    }
                    variant="borderless"
                    style={{ marginTop: "16px", marginBottom: "16px" }}
                  >
                    <Row gutter={[16, 16]} style={{ marginBottom: "16px" }}>
                      <Col span={8}>
                        <Statistic
                          title="执行时间"
                          value={batchShipmentResult.exec_time}
                          precision={4}
                          suffix="秒"
                          prefix={<ClockCircleOutlined />}
                          valueStyle={{ color: "#3f8600" }}
                        />
                      </Col>
                      <Col span={8}>
                        <Statistic
                          title="返回记录数"
                          value={batchShipmentResult.data.length}
                          suffix="条"
                        />
                      </Col>
                      <Col span={8}>
                        <Statistic
                          title="字段数"
                          value={batchShipmentResult.columns.length}
                          suffix="个"
                        />
                      </Col>
                    </Row>

                    <Table
                      dataSource={batchShipmentResult.data}
                      columns={batchShipmentResult.columns.map((col) => ({
                        title:
                          col === "supp_nation"
                            ? "供应商国家"
                            : col === "cust_nation"
                            ? "客户国家"
                            : col === "l_year"
                            ? "年份"
                            : col === "revenue"
                            ? "收入"
                            : col,
                        dataIndex: col,
                        key: col,
                        render: (text: any) => {
                          if (col === "revenue") {
                            return new Intl.NumberFormat("zh-CN", {
                              style: "currency",
                              currency: "USD",
                              minimumFractionDigits: 2,
                            }).format(parseFloat(text));
                          }
                          return text;
                        },
                      }))}
                      rowKey={(record, index) => index?.toString() || "0"}
                      pagination={{
                        pageSize: 10,
                        showSizeChanger: true,
                        showQuickJumper: true,
                        showTotal: (total, range) =>
                          `第 ${range[0]}-${range[1]} 条，共 ${total} 条记录`,
                      }}
                      scroll={{ x: 800 }}
                      size="middle"
                    />
                  </Card>
                )}

                {/* 批量出货查询计划展示 */}
                {batchShipmentPlan && (
                  <Card
                    title={
                      <Space>
                        <EyeOutlined />
                        <span>批量出货查询计划</span>
                      </Space>
                    }
                    variant="borderless"
                    style={{ marginTop: "16px", marginBottom: "16px" }}
                  >
                    {(() => {
                      const planData = parseQueryPlan(
                        typeof batchShipmentPlan === "string"
                          ? batchShipmentPlan
                          : JSON.stringify(batchShipmentPlan)
                      );
                      return (
                        <>
                          {/* 执行时间统计 */}
                          <Row
                            gutter={[16, 16]}
                            style={{ marginBottom: "16px" }}
                          >
                            {planData.planningTime && (
                              <Col span={12}>
                                <Statistic
                                  title="规划时间"
                                  value={planData.planningTime}
                                  precision={3}
                                  suffix="毫秒"
                                  valueStyle={{ color: "#1890ff" }}
                                />
                              </Col>
                            )}
                            {planData.executionTime && (
                              <Col span={12}>
                                <Statistic
                                  title="执行时间"
                                  value={planData.executionTime}
                                  precision={3}
                                  suffix="毫秒"
                                  valueStyle={{ color: "#52c41a" }}
                                />
                              </Col>
                            )}
                          </Row>

                          {/* 查询计划详情 */}
                          <Collapse defaultActiveKey={["1"]} size="small">
                            <Panel header="查询执行计划详情" key="1">
                              <div
                                style={{
                                  background: "#f6f8fa",
                                  padding: "16px",
                                  borderRadius: "6px",
                                  maxHeight: "400px",
                                  overflow: "auto",
                                }}
                              >
                                {planData.planLines.map((line, index) => {
                                  const formatted = formatPlanLine(line);
                                  const getLineColor = (type: string) => {
                                    switch (type) {
                                      case "scan":
                                        return "#1890ff";
                                      case "join":
                                        return "#52c41a";
                                      case "aggregate":
                                        return "#faad14";
                                      case "operation":
                                        return "#722ed1";
                                      default:
                                        return "#666";
                                    }
                                  };

                                  return (
                                    <div
                                      key={index}
                                      style={{
                                        marginLeft: `${formatted.indent * 2}px`,
                                        fontSize: "12px",
                                        lineHeight: "1.6",
                                        fontFamily:
                                          'Monaco, Consolas, "Courier New", monospace',
                                        color: getLineColor(formatted.type),
                                        marginBottom: "2px",
                                      }}
                                    >
                                      {formatted.text}
                                    </div>
                                  );
                                })}
                              </div>
                            </Panel>
                          </Collapse>
                        </>
                      );
                    })()}
                  </Card>
                )}
              </Col>

              <Col span={24}>
                <Card
                  title={
                    <Space>
                      <DatabaseOutlined />
                      <span>全国市场份额查询</span>
                    </Space>
                  }
                  variant="borderless"
                >
                  <Paragraph style={{ color: "#666", marginBottom: "16px" }}>
                    给定国家，地区，周期和零件
                    key，查找周期内的每年该国在该地区收入所占的份额，结果返回年份和周期比，按照年份升序。
                  </Paragraph>

                  <Row gutter={[16, 16]}>
                    <Col xs={24} sm={12} md={8}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        国家:
                      </label>
                      <Select
                        placeholder="选择国家"
                        style={{ width: "100%" }}
                        size="middle"
                        onDropdownVisibleChange={(open) => {
                          if (open) {
                            fetchNations();
                          }
                        }}
                        onChange={(value) => {
                          setMarketShareRequestData((prev) => ({
                            ...prev,
                            nation: value,
                          }));
                        }}
                      >
                        {nations.map((nation) => (
                          <Option key={nation} value={nation}>
                            {nation}
                          </Option>
                        ))}
                      </Select>
                    </Col>
                    <Col xs={24} sm={12} md={8}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        地区:
                      </label>
                      <Select
                        placeholder="选择地区"
                        style={{ width: "100%" }}
                        size="middle"
                        onDropdownVisibleChange={(open) => {
                          if (open) {
                            fetchRegions();
                          }
                        }}
                        onChange={(value) => {
                          setMarketShareRequestData((prev) => ({
                            ...prev,
                            region: value,
                          }));
                        }}
                      >
                        {regions.map((region) => (
                          <Option key={region} value={region}>
                            {region}
                          </Option>
                        ))}
                      </Select>
                    </Col>
                    <Col xs={24} sm={12} md={8}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        零件类型:
                      </label>
                      <Select
                        placeholder="选择零件类型"
                        style={{ width: "100%" }}
                        size="middle"
                        onDropdownVisibleChange={(open) => {
                          if (open) {
                            fetchPartTypes();
                          }
                        }}
                        onChange={(value) => {
                          setMarketShareRequestData((prev) => ({
                            ...prev,
                            partType: value,
                          }));
                        }}
                      >
                        {partTypes.map((partType) => (
                          <Option key={partType} value={partType}>
                            {partType}
                          </Option>
                        ))}
                      </Select>
                    </Col>
                    <Col xs={24} sm={12} md={12}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        起始时间:
                      </label>
                      <DatePicker
                        placeholder="选择起始日期"
                        style={{ width: "100%" }}
                        size="middle"
                        format="YYYY-MM-DD"
                        onChange={(date, dateString) => {
                          setMarketShareRequestData((prev) => ({
                            ...prev,
                            startDate:
                              typeof dateString === "string" ? dateString : "",
                          }));
                        }}
                      />
                    </Col>
                    <Col xs={24} sm={12} md={12}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        终止时间:
                      </label>
                      <DatePicker
                        placeholder="选择终止日期"
                        style={{ width: "100%" }}
                        size="middle"
                        format="YYYY-MM-DD"
                        onChange={(date, dateString) => {
                          setMarketShareRequestData((prev) => ({
                            ...prev,
                            endDate:
                              typeof dateString === "string" ? dateString : "",
                          }));
                        }}
                      />
                    </Col>
                  </Row>

                  <div style={{ marginTop: "20px", textAlign: "right" }}>
                    <Space>
                      <Button
                        icon={<EyeOutlined />}
                        size="middle"
                        loading={marketSharePlanLoading}
                        disabled={
                          batchShipmentLoading ||
                          marketShareLoading ||
                          batchShipmentPlanLoading ||
                          marketSharePlanLoading
                        }
                        onClick={handleMarketSharePlan}
                      >
                        查看查询计划
                      </Button>
                      <Button
                        type="primary"
                        icon={<PlayCircleOutlined />}
                        size="middle"
                        loading={marketShareLoading}
                        disabled={
                          batchShipmentLoading ||
                          marketShareLoading ||
                          batchShipmentPlanLoading ||
                          marketSharePlanLoading
                        }
                        style={{
                          background:
                            "linear-gradient(135deg, #667eea 0%, #764ba2 100%)",
                          border: "none",
                          borderRadius: "6px",
                        }}
                        onClick={handleMarketShare}
                      >
                        执行查询
                      </Button>
                    </Space>
                  </div>
                </Card>

                {/* 全国市场份额查询结果展示 */}
                {marketShareResult && (
                  <Card
                    title={
                      <Space>
                        <DatabaseOutlined />
                        <span>全国市场份额查询结果</span>
                      </Space>
                    }
                    variant="borderless"
                    style={{ marginTop: "16px" }}
                  >
                    <Row gutter={[16, 16]} style={{ marginBottom: "16px" }}>
                      <Col span={8}>
                        <Statistic
                          title="执行时间"
                          value={marketShareResult.exec_time}
                          precision={4}
                          suffix="秒"
                          prefix={<ClockCircleOutlined />}
                          valueStyle={{ color: "#3f8600" }}
                        />
                      </Col>
                      <Col span={8}>
                        <Statistic
                          title="返回记录数"
                          value={marketShareResult.data.length}
                          suffix="条"
                        />
                      </Col>
                      <Col span={8}>
                        <Statistic
                          title="字段数"
                          value={marketShareResult.columns.length}
                          suffix="个"
                        />
                      </Col>
                    </Row>

                    <Table
                      dataSource={marketShareResult.data}
                      columns={marketShareResult.columns.map((col) => ({
                        title: col,
                        dataIndex: col,
                        key: col,
                        render: (text: any) => {
                          // 如果是数字类型且看起来像百分比，格式化为百分比
                          if (
                            typeof text === "string" &&
                            !isNaN(parseFloat(text))
                          ) {
                            const num = parseFloat(text);
                            if (num <= 1 && num >= 0) {
                              return (num * 100).toFixed(2) + "%";
                            }
                          }
                          return text;
                        },
                      }))}
                      rowKey={(record, index) => index?.toString() || "0"}
                      pagination={{
                        pageSize: 10,
                        showSizeChanger: true,
                        showQuickJumper: true,
                        showTotal: (total, range) =>
                          `第 ${range[0]}-${range[1]} 条，共 ${total} 条记录`,
                      }}
                      scroll={{ x: 800 }}
                      size="middle"
                    />
                  </Card>
                )}

                {/* 全国市场份额查询计划展示 */}
                {marketSharePlan && (
                  <Card
                    title={
                      <Space>
                        <EyeOutlined />
                        <span>全国市场份额查询计划</span>
                      </Space>
                    }
                    variant="borderless"
                    style={{ marginTop: "16px", marginBottom: "16px" }}
                  >
                    {(() => {
                      const planData = parseQueryPlan(
                        typeof marketSharePlan === "string"
                          ? marketSharePlan
                          : JSON.stringify(marketSharePlan)
                      );
                      return (
                        <>
                          {/* 执行时间统计 */}
                          <Row
                            gutter={[16, 16]}
                            style={{ marginBottom: "16px" }}
                          >
                            {planData.planningTime && (
                              <Col span={12}>
                                <Statistic
                                  title="规划时间"
                                  value={planData.planningTime}
                                  precision={3}
                                  suffix="毫秒"
                                  valueStyle={{ color: "#1890ff" }}
                                />
                              </Col>
                            )}
                            {planData.executionTime && (
                              <Col span={12}>
                                <Statistic
                                  title="执行时间"
                                  value={planData.executionTime}
                                  precision={3}
                                  suffix="毫秒"
                                  valueStyle={{ color: "#52c41a" }}
                                />
                              </Col>
                            )}
                          </Row>

                          {/* 查询计划详情 */}
                          <Collapse defaultActiveKey={["1"]} size="small">
                            <Panel header="查询执行计划详情" key="1">
                              <div
                                style={{
                                  background: "#f6f8fa",
                                  padding: "16px",
                                  borderRadius: "6px",
                                  maxHeight: "400px",
                                  overflow: "auto",
                                }}
                              >
                                {planData.planLines.map((line, index) => {
                                  const formatted = formatPlanLine(line);
                                  const getLineColor = (type: string) => {
                                    switch (type) {
                                      case "scan":
                                        return "#1890ff";
                                      case "join":
                                        return "#52c41a";
                                      case "aggregate":
                                        return "#faad14";
                                      case "operation":
                                        return "#722ed1";
                                      default:
                                        return "#666";
                                    }
                                  };

                                  return (
                                    <div
                                      key={index}
                                      style={{
                                        fontFamily:
                                          'Monaco, Menlo, "Ubuntu Mono", monospace',
                                        fontSize: "12px",
                                        lineHeight: "1.6",
                                        paddingLeft: `${
                                          formatted.indent * 2
                                        }px`,
                                        color: getLineColor(formatted.type),
                                        fontWeight:
                                          formatted.type === "operation"
                                            ? "bold"
                                            : "normal",
                                      }}
                                    >
                                      {formatted.text}
                                    </div>
                                  );
                                })}
                              </div>
                            </Panel>
                          </Collapse>
                        </>
                      );
                    })()}
                  </Card>
                )}
              </Col>
            </Row>
          </div>
        </TabPane>
        <TabPane tab="TPC-C 事务查询" key="2">
          <div style={{ padding: "20px" }}>
            <Row gutter={[16, 24]}>
              <Col span={24}>
                <Card
                  title={
                    <Space>
                      <ShoppingCartOutlined />
                      <span>新建订单事务</span>
                    </Space>
                  }
                  variant="borderless"
                  style={{ marginBottom: "16px" }}
                >
                  <Paragraph style={{ color: "#666", marginBottom: "16px" }}>
                    执行TPC-C标准的新建订单事务，模拟客户创建新订单的业务流程，包括库存检查、订单创建等操作。
                  </Paragraph>

                  <Row gutter={[16, 16]} style={{ marginBottom: "20px" }}>
                    <Col xs={24} sm={12} md={6}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        仓库ID (w_id):
                      </label>
                      <Select
                        placeholder="选择仓库ID"
                        style={{ width: "100%" }}
                        size="middle"
                        value={newOrderRequestData.w_id}
                        onChange={(value) => {
                          setNewOrderRequestData((prev) => ({
                            ...prev,
                            w_id: value,
                          }));
                        }}
                      >
                        {[1, 2, 3, 4].map((id) => (
                          <Option key={id} value={id}>
                            仓库 {id}
                          </Option>
                        ))}
                      </Select>
                    </Col>
                    <Col xs={24} sm={12} md={6}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        地区ID (d_id):
                      </label>
                      <Select
                        placeholder="选择地区ID"
                        style={{ width: "100%" }}
                        size="middle"
                        value={newOrderRequestData.d_id}
                        onChange={(value) => {
                          setNewOrderRequestData((prev) => ({
                            ...prev,
                            d_id: value,
                          }));
                        }}
                      >
                        {Array.from({ length: 10 }, (_, i) => i + 1).map(
                          (id) => (
                            <Option key={id} value={id}>
                              地区 {id}
                            </Option>
                          )
                        )}
                      </Select>
                    </Col>
                    <Col xs={24} sm={12} md={6}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        物品数量 (o_ol_cnt):
                      </label>
                      <Select
                        placeholder="选择物品数量"
                        style={{ width: "100%" }}
                        size="middle"
                        value={newOrderRequestData.o_ol_cnt}
                        onChange={(value) => {
                          setNewOrderRequestData((prev) => ({
                            ...prev,
                            o_ol_cnt: value,
                          }));
                        }}
                      >
                        {Array.from({ length: 11 }, (_, i) => i + 5).map(
                          (count) => (
                            <Option key={count} value={count}>
                              {count} 件物品
                            </Option>
                          )
                        )}
                      </Select>
                    </Col>
                    <Col xs={24} sm={12} md={6}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        客户ID (c_id):
                      </label>
                      <InputNumber
                        placeholder="输入客户ID (1-3000)"
                        style={{ width: "100%" }}
                        size="middle"
                        min={1}
                        max={3000}
                        value={newOrderRequestData.c_id}
                        onChange={(value) => {
                          if (value && value >= 1 && value <= 3000) {
                            setNewOrderRequestData((prev) => ({
                              ...prev,
                              c_id: value,
                            }));
                          }
                        }}
                        status={
                          newOrderRequestData.c_id < 1 ||
                          newOrderRequestData.c_id > 3000
                            ? "error"
                            : ""
                        }
                      />
                      {(newOrderRequestData.c_id < 1 ||
                        newOrderRequestData.c_id > 3000) && (
                        <div
                          style={{
                            color: "#ff4d4f",
                            fontSize: "12px",
                            marginTop: "4px",
                          }}
                        >
                          客户ID必须在1-3000范围内
                        </div>
                      )}
                    </Col>
                  </Row>

                  <div style={{ textAlign: "right" }}>
                    <Button
                      type="primary"
                      icon={<PlayCircleOutlined />}
                      size="middle"
                      loading={newOrderLoading}
                      disabled={newOrderLoading || paymentLoading}
                      style={{
                        background:
                          "linear-gradient(135deg, #52c41a 0%, #389e0d 100%)",
                        border: "none",
                        borderRadius: "6px",
                      }}
                      onClick={handleNewOrder}
                    >
                      执行新建订单事务
                    </Button>
                  </div>
                </Card>

                {/* 新建订单事务结果展示 */}
                {newOrderResult && (
                  <Card
                    title={
                      <Space>
                        <ShoppingCartOutlined />
                        <span>新建订单事务结果</span>
                      </Space>
                    }
                    variant="borderless"
                    style={{ marginTop: "16px", marginBottom: "16px" }}
                  >
                    <Row gutter={[16, 16]} style={{ marginBottom: "16px" }}>
                      <Col span={8}>
                        <Statistic
                          title="执行时间"
                          value={newOrderResult.exec_time}
                          precision={4}
                          suffix="秒"
                          prefix={<ClockCircleOutlined />}
                          valueStyle={{ color: "#3f8600" }}
                        />
                      </Col>
                      <Col span={8}>
                        <div
                          style={{
                            background: "#f6ffed",
                            border: "1px solid #b7eb8f",
                            borderRadius: "6px",
                            padding: "16px",
                            height: "100%",
                            display: "flex",
                            alignItems: "center",
                          }}
                        >
                          <div>
                            <div
                              style={{
                                fontSize: "14px",
                                color: "#666",
                                marginBottom: "4px",
                              }}
                            >
                              执行结果
                            </div>
                            <div
                              style={{
                                fontSize: "16px",
                                fontWeight: "bold",
                                color: "#52c41a",
                              }}
                            >
                              {newOrderResult.message}
                            </div>
                          </div>
                        </div>
                      </Col>
                      <Col span={8}>
                        <div
                          style={{
                            background: "#f0f9ff",
                            border: "1px solid #91d5ff",
                            borderRadius: "6px",
                            padding: "16px",
                            height: "100%",
                          }}
                        >
                          <div
                            style={{
                              fontSize: "14px",
                              color: "#666",
                              marginBottom: "8px",
                            }}
                          >
                            执行参数
                          </div>
                          <div style={{ fontSize: "12px", lineHeight: "1.6" }}>
                            <div>
                              仓库ID:{" "}
                              <Text strong>{newOrderRequestData.w_id}</Text>
                            </div>
                            <div>
                              地区ID:{" "}
                              <Text strong>{newOrderRequestData.d_id}</Text>
                            </div>
                            <div>
                              物品数量:{" "}
                              <Text strong>{newOrderRequestData.o_ol_cnt}</Text>
                            </div>
                            <div>
                              客户ID:{" "}
                              <Text strong>{newOrderRequestData.c_id}</Text>
                            </div>
                          </div>
                        </div>
                      </Col>
                    </Row>
                  </Card>
                )}
              </Col>

              <Col span={24}>
                <Card
                  title={
                    <Space>
                      <CreditCardOutlined />
                      <span>支付事务</span>
                    </Space>
                  }
                  variant="borderless"
                  style={{ marginBottom: "16px" }}
                >
                  <Paragraph style={{ color: "#666", marginBottom: "16px" }}>
                    执行TPC-C标准的支付事务，模拟客户支付订单的业务流程，包括账户余额更新、支付记录创建等操作。
                  </Paragraph>

                  <Row gutter={[16, 16]} style={{ marginBottom: "20px" }}>
                    <Col xs={24} sm={8} md={8}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        仓库ID (w_id):
                      </label>
                      <Select
                        placeholder="选择仓库ID"
                        style={{ width: "100%" }}
                        size="middle"
                        value={paymentRequestData.w_id}
                        onChange={(value) => {
                          setPaymentRequestData((prev) => ({
                            ...prev,
                            w_id: value,
                          }));
                        }}
                      >
                        {[1, 2, 3, 4].map((id) => (
                          <Option key={id} value={id}>
                            仓库 {id}
                          </Option>
                        ))}
                      </Select>
                    </Col>
                    <Col xs={24} sm={8} md={8}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        地区ID (d_id):
                      </label>
                      <Select
                        placeholder="选择地区ID"
                        style={{ width: "100%" }}
                        size="middle"
                        value={paymentRequestData.d_id}
                        onChange={(value) => {
                          setPaymentRequestData((prev) => ({
                            ...prev,
                            d_id: value,
                          }));
                        }}
                      >
                        {Array.from({ length: 10 }, (_, i) => i + 1).map(
                          (id) => (
                            <Option key={id} value={id}>
                              地区 {id}
                            </Option>
                          )
                        )}
                      </Select>
                    </Col>
                    <Col xs={24} sm={8} md={8}>
                      <label
                        style={{
                          display: "block",
                          marginBottom: "8px",
                          fontWeight: "500",
                        }}
                      >
                        客户ID (c_id):
                      </label>
                      <InputNumber
                        placeholder="输入客户ID (1-3000)"
                        style={{ width: "100%" }}
                        size="middle"
                        min={1}
                        max={3000}
                        value={paymentRequestData.c_id}
                        onChange={(value) => {
                          if (value && value >= 1 && value <= 3000) {
                            setPaymentRequestData((prev) => ({
                              ...prev,
                              c_id: value,
                            }));
                          }
                        }}
                        status={
                          paymentRequestData.c_id < 1 ||
                          paymentRequestData.c_id > 3000
                            ? "error"
                            : ""
                        }
                      />
                      {(paymentRequestData.c_id < 1 ||
                        paymentRequestData.c_id > 3000) && (
                        <div
                          style={{
                            color: "#ff4d4f",
                            fontSize: "12px",
                            marginTop: "4px",
                          }}
                        >
                          客户ID必须在1-3000范围内
                        </div>
                      )}
                    </Col>
                  </Row>

                  <div style={{ textAlign: "right" }}>
                    <Button
                      type="primary"
                      icon={<PlayCircleOutlined />}
                      size="middle"
                      loading={paymentLoading}
                      disabled={newOrderLoading || paymentLoading}
                      style={{
                        background:
                          "linear-gradient(135deg, #1890ff 0%, #096dd9 100%)",
                        border: "none",
                        borderRadius: "6px",
                      }}
                      onClick={handlePayment}
                    >
                      执行支付事务
                    </Button>
                  </div>
                </Card>

                {/* 支付事务结果展示 */}
                {paymentResult && (
                  <Card
                    title={
                      <Space>
                        <CreditCardOutlined />
                        <span>支付事务结果</span>
                      </Space>
                    }
                    variant="borderless"
                    style={{ marginTop: "16px", marginBottom: "16px" }}
                  >
                    <Row gutter={[16, 16]}>
                      <Col span={8}>
                        <Statistic
                          title="执行时间"
                          value={paymentResult.exec_time}
                          precision={4}
                          suffix="秒"
                          prefix={<ClockCircleOutlined />}
                          valueStyle={{ color: "#3f8600" }}
                        />
                      </Col>
                      <Col span={8}>
                        <div
                          style={{
                            background: "#f6ffed",
                            border: "1px solid #b7eb8f",
                            borderRadius: "6px",
                            padding: "16px",
                            height: "100%",
                            display: "flex",
                            alignItems: "center",
                          }}
                        >
                          <div>
                            <div
                              style={{
                                fontSize: "14px",
                                color: "#666",
                                marginBottom: "4px",
                              }}
                            >
                              执行结果
                            </div>
                            <div
                              style={{
                                fontSize: "16px",
                                fontWeight: "bold",
                                color: "#1890ff",
                              }}
                            >
                              {paymentResult.message}
                            </div>
                          </div>
                        </div>
                      </Col>
                      <Col span={8}>
                        <div
                          style={{
                            background: "#f0f9ff",
                            border: "1px solid #91d5ff",
                            borderRadius: "6px",
                            padding: "16px",
                            height: "100%",
                          }}
                        >
                          <div
                            style={{
                              fontSize: "14px",
                              color: "#666",
                              marginBottom: "8px",
                            }}
                          >
                            执行参数
                          </div>
                          <div style={{ fontSize: "12px", lineHeight: "1.6" }}>
                            <div>
                              仓库ID:{" "}
                              <Text strong>{paymentRequestData.w_id}</Text>
                            </div>
                            <div>
                              地区ID:{" "}
                              <Text strong>{paymentRequestData.d_id}</Text>
                            </div>
                            <div>
                              客户ID:{" "}
                              <Text strong>{paymentRequestData.c_id}</Text>
                            </div>
                          </div>
                        </div>
                      </Col>
                    </Row>
                  </Card>
                )}
              </Col>
            </Row>

            <div
              style={{
                marginTop: "30px",
                padding: "16px",
                backgroundColor: "#f8f9fa",
                borderRadius: "6px",
              }}
            >
              <Title level={5} style={{ margin: "0 0 12px 0" }}>
                TPC-C 事务说明：
              </Title>
              <Row gutter={[16, 16]}>
                <Col span={12}>
                  <div
                    style={{
                      padding: "12px",
                      background: "#fff",
                      borderRadius: "4px",
                      border: "1px solid #e8e8e8",
                    }}
                  >
                    <Text strong style={{ color: "#52c41a" }}>
                      新建订单事务 (New-Order)
                    </Text>
                    <ul
                      style={{
                        marginTop: "8px",
                        marginBottom: 0,
                        paddingLeft: "20px",
                        fontSize: "14px",
                      }}
                    >
                      <li>模拟客户创建新订单的完整流程</li>
                      <li>包括客户信息验证、库存检查</li>
                      <li>创建订单记录和订单明细</li>
                      <li>更新库存数量</li>
                    </ul>
                  </div>
                </Col>
                <Col span={12}>
                  <div
                    style={{
                      padding: "12px",
                      background: "#fff",
                      borderRadius: "4px",
                      border: "1px solid #e8e8e8",
                    }}
                  >
                    <Text strong style={{ color: "#1890ff" }}>
                      支付事务 (Payment)
                    </Text>
                    <ul
                      style={{
                        marginTop: "8px",
                        marginBottom: 0,
                        paddingLeft: "20px",
                        fontSize: "14px",
                      }}
                    >
                      <li>模拟客户支付订单的完整流程</li>
                      <li>包括客户账户余额检查</li>
                      <li>创建支付记录</li>
                      <li>更新客户和商户账户余额</li>
                    </ul>
                  </div>
                </Col>
              </Row>
            </div>
          </div>
        </TabPane>
        {/* <TabPane tab="TPC-H 并发测试" key="3">
          <div style={{ padding: "20px" }}></div>
        </TabPane> */}
        <TabPane tab="TPC-C 并发事务测试" key="4">
          <div style={{ padding: "20px" }}>
            <Card
              title={
                <Space>
                  <DatabaseOutlined />
                  <span>TPC-C 并发事务测试</span>
                </Space>
              }
              variant="borderless"
              style={{ marginBottom: "16px" }}
            >
              <Paragraph style={{ color: "#666", marginBottom: "16px" }}>
                TPC-C
                并发事务测试是一个专业的数据库性能基准测试工具，用于评估数据库系统在高并发事务负载下的性能表现。此测试将在新窗口中运行，请耐心等待测试完成。
              </Paragraph>

              <Row gutter={[16, 16]}>
                <Col xs={24} sm={12} md={6}>
                  <label
                    style={{
                      display: "block",
                      marginBottom: "8px",
                      fontWeight: "500",
                    }}
                  >
                    终端数量 (terminal):
                  </label>
                  <InputNumber
                    placeholder="输入终端数量"
                    style={{ width: "100%" }}
                    size="middle"
                    min={1}
                    max={100}
                    value={tpccRequestData.terminal}
                    onChange={(value) => {
                      setTpccRequestData((prev) => ({
                        ...prev,
                        terminal: value || 2,
                      }));
                    }}
                  />
                </Col>
                <Col xs={24} sm={12} md={6}>
                  <label
                    style={{
                      display: "block",
                      marginBottom: "8px",
                      fontWeight: "500",
                    }}
                  >
                    每分钟事务限制 (limitTxnsPerMin):
                  </label>
                  <InputNumber
                    placeholder="输入事务限制"
                    style={{ width: "100%" }}
                    size="middle"
                    min={1}
                    max={10000}
                    value={tpccRequestData.limitTxnsPerMin}
                    onChange={(value) => {
                      setTpccRequestData((prev) => ({
                        ...prev,
                        limitTxnsPerMin: value || 200,
                      }));
                    }}
                  />
                </Col>
                <Col xs={24} sm={12} md={6}>
                  <label
                    style={{
                      display: "block",
                      marginBottom: "8px",
                      fontWeight: "500",
                    }}
                  >
                    每终端运行事务数 (runTxnsPerTerminal):
                  </label>
                  <InputNumber
                    placeholder="输入事务数"
                    style={{ width: "100%" }}
                    size="middle"
                    min={1}
                    max={1000}
                    value={tpccRequestData.runTxnsPerTerminal}
                    onChange={(value) => {
                      setTpccRequestData((prev) => ({
                        ...prev,
                        runTxnsPerTerminal: value || 30,
                      }));
                    }}
                  />
                </Col>
                <Col xs={24} sm={12} md={6}>
                  <label
                    style={{
                      display: "block",
                      marginBottom: "8px",
                      fontWeight: "500",
                    }}
                  >
                    运行分钟数 (runMins):
                  </label>
                  <InputNumber
                    placeholder="输入运行分钟数"
                    style={{ width: "100%" }}
                    size="middle"
                    min={1}
                    max={1440} // 最多24小时
                    value={tpccRequestData.runMins}
                    onChange={(value) => {
                      setTpccRequestData((prev) => ({
                        ...prev,
                        runMins: value || 5,
                      }));
                    }}
                  />
                </Col>
              </Row>

              <Row gutter={[16, 16]} style={{ marginTop: "16px" }}>
                <Col xs={24} sm={12} md={6}>
                  <label
                    style={{
                      display: "block",
                      marginBottom: "8px",
                      fontWeight: "500",
                    }}
                  >
                    运行模式:
                  </label>
                  <Radio.Group
                    value={tpccRequestData.runMode}
                    onChange={(e) => {
                      setTpccRequestData((prev) => ({
                        ...prev,
                        runMode: e.target.value,
                        // 切换模式时重置对应的值
                        runTxnsPerTerminal: e.target.value === 'transaction' ? 30 : 0,
                        runMins: e.target.value === 'time' ? 5 : undefined,
                      }));
                    }}
                    style={{ width: "100%" }}
                  >
                    <Radio value="transaction">按事务数量</Radio>
                    <Radio value="time">按运行时间</Radio>
                  </Radio.Group>
                </Col>
              </Row>

              <div
                style={{
                  marginTop: "16px",
                  padding: "12px",
                  backgroundColor: "#f0f9ff",
                  border: "1px solid #91d5ff",
                  borderRadius: "6px",
                }}
              >
                <Text style={{ fontSize: "14px", color: "#1890ff" }}>
                  <strong>当前设置：</strong>
                  {tpccRequestData.runMode === 'transaction'
                    ? ` 每个终端执行 ${tpccRequestData.runTxnsPerTerminal} 个事务`
                    : ` 运行 ${tpccRequestData.runMins} 分钟`}
                  ，共 {tpccRequestData.terminal} 个终端，每分钟最多 {tpccRequestData.limitTxnsPerMin} 个事务
                </Text>
              </div>

              <div style={{ marginTop: "20px", textAlign: "right" }}>
                <Button
                  type="primary"
                  icon={<PlayCircleOutlined />}
                  size="middle"
                  loading={tpccLoading}
                  disabled={tpccLoading}
                  style={{
                    background:
                      "linear-gradient(135deg, #667eea 0%, #764ba2 100%)",
                    border: "none",
                    borderRadius: "6px",
                  }}
                  onClick={handleTpccConcurrent}
                >
                  开始TPC-C测试
                </Button>
              </div>

              <div
                style={{
                  marginTop: "20px",
                  padding: "16px",
                  backgroundColor: "#f8f9fa",
                  borderRadius: "6px",
                }}
              >
                <Title level={5} style={{ margin: "0 0 12px 0" }}>
                  测试说明：
                </Title>
                <ul style={{ marginBottom: 0, paddingLeft: "20px" }}>
                  <li>测试将模拟真实的电商事务处理场景</li>
                  <li>包含订单处理、库存管理、支付等多种事务类型</li>
                  <li>
                    <strong>运行模式说明：</strong>
                  </li>
                  <ul style={{ marginTop: "4px" }}>
                    <li>
                      <strong>按事务数量：</strong>
                      每个终端执行指定数量的事务后停止
                    </li>
                    <li>
                      <strong>按运行时间：</strong>
                      运行指定分钟数后停止，不限制事务数量
                    </li>
                  </ul>
                  <li>测试过程可能需要数分钟至数十分钟</li>
                  <li>测试结果将在新窗口中显示，请勿关闭浏览器</li>
                  <li>建议在服务器负载较低时进行测试以获得准确结果</li>
                </ul>
              </div>
            </Card>
          </div>
        </TabPane>
      </Tabs>
    </>
  );
}
