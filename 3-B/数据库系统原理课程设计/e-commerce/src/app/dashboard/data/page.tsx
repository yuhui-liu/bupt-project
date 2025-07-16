"use client";

import React, { useState } from "react";
import {
  InboxOutlined,
  UploadOutlined,
  DownloadOutlined,
  SettingOutlined,
  ImportOutlined,
  ExportOutlined,
  FolderOutlined,
} from "@ant-design/icons";
import type { UploadProps, UploadFile } from "antd";
import {
  message,
  Upload,
  Select,
  Button,
  Input,
  Modal,
  Space,
  Form,
  Tabs,
  Radio,
} from "antd";
import axiosInstance from "@/lib/axios";

const { Dragger } = Upload;

const tableOptions = [
  { value: "nation", label: "国家表" },
  { value: "region", label: "地区表" },
  { value: "part", label: "零件表" },
  { value: "supplier", label: "供应商表" },
  { value: "partsupp", label: "零件-供应商关系表" },
  { value: "customer", label: "客户表" },
  { value: "orders", label: "订单表" },
  { value: "lineitem", label: "条目表" },
];

const formatOptions = [
  { value: "csv", label: "CSV" },
  { value: "txt", label: "TXT" },
  { value: "xlsx", label: "Excel" },
];

// 检测浏览器对 File System Access API 的支持
const isFileSystemAPISupported = (): boolean => {
  return "showSaveFilePicker" in window;
};

// 导出方式选项
interface ExportOption {
  value: string;
  label: string;
  icon: React.ReactNode;
  description: string;
  disabled?: boolean;
}

const getExportOptions = (): ExportOption[] => {
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

const CHUNK_SIZE = 5 * 1024 * 1024; // 5MB per chunk

export default function Page() {
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [fileName, setFileName] = useState("export");
  const [selectedTable, setSelectedTable] = useState("");
  const [selectedFormat, setSelectedFormat] = useState("csv");
  const [uploading, setUploading] = useState(false);
  const [exporting, setExporting] = useState(false);
  const [fileList, setFileList] = useState<UploadFile[]>([]);
  const [selectedTableForImport, setSelectedTableForImport] = useState("");
  const [messageApi, contextHolder] = message.useMessage();
  const [uploadProgress, setUploadProgress] = useState(0);
  const [isChunkUploading, setIsChunkUploading] = useState(false);
  const [exportMethod, setExportMethod] = useState("browser_download");
  const [importProgress, setImportProgress] = useState(0);
  const [isImporting, setIsImporting] = useState(false);

  // 模拟导入进度条
  const simulateImportProgress = (): Promise<void> => {
    return new Promise((resolve) => {
      setIsImporting(true);
      setImportProgress(0);
      
      // 模拟进度更新
      const intervals = [
        { progress: 20, delay: 300 },  // 文件解析
        { progress: 40, delay: 500 },  // 数据验证
        { progress: 60, delay: 400 },  // 数据处理
        { progress: 80, delay: 300 },  // 数据插入
        { progress: 95, delay: 200 },  // 最终处理
      ];

      let currentIndex = 0;
      
      const updateProgress = () => {
        if (currentIndex < intervals.length) {
          const { progress, delay } = intervals[currentIndex];
          setTimeout(() => {
            setImportProgress(progress);
            currentIndex++;
            updateProgress();
          }, delay);
        } else {
          // 等待实际请求完成后再到100%
          resolve();
        }
      };
      
      updateProgress();
    });
  };

  // 文件格式验证函数
  const validateFileFormat = (
    fileName: string,
  ): { valid: boolean; message?: string } => {
    const allowedExtensions = [".txt", ".csv"];
    const fileExtension = fileName
      .toLowerCase()
      .slice(fileName.lastIndexOf("."));

    if (!fileExtension) {
      return {
        valid: false,
        message: "文件必须有扩展名",
      };
    }

    if (!allowedExtensions.includes(fileExtension)) {
      return {
        valid: false,
        message: `只支持 ${allowedExtensions.join(", ")} 格式的文件，当前文件格式为: ${fileExtension}`,
      };
    }

    return { valid: true };
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

      const fileHandle = await (window as any).showSaveFilePicker({
        suggestedName: fileName,
        types: [
          {
            description: `${selectedFormat.toUpperCase()} 文件`,
            accept: {
              [`text/${selectedFormat === "csv" ? "csv" : "plain"}`]: [
                `.${selectedFormat}`,
              ],
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

  const handleExport = async () => {
    try {
      setExporting(true);

      // 获取导出数据
      const response = await axiosInstance.get("/api/export", {
        params: {
          table: selectedTable,
          format: selectedFormat,
        },
        responseType: "blob",
      });

      const blob = response.data;
      const finalFileName = fileName.includes(selectedFormat)
        ? fileName
        : `${fileName}.${selectedFormat}`;

      // 根据选择的导出方式处理文件
      if (exportMethod === "file_system_api") {
        const saved = await saveFileWithAPI(blob, finalFileName);
        if (saved) {
          messageApi.success("文件已保存到您选择的位置");
        }
        // 如果用户取消了，不显示任何消息
      } else {
        // 默认使用浏览器下载
        downloadFile(blob, finalFileName);
        messageApi.success("文件已开始下载，请检查您的下载文件夹");
      }

      setIsModalOpen(false);
    } catch (error) {
      // 如果 File System API 失败，降级到默认下载
      if (exportMethod === "file_system_api") {
        try {
          const response = await axiosInstance.get("/api/export", {
            params: {
              table: selectedTable,
              format: selectedFormat,
            },
            responseType: "blob",
          });

          const blob = response.data;
          const finalFileName = fileName.includes(selectedFormat)
            ? fileName
            : `${fileName}.${selectedFormat}`;

          downloadFile(blob, finalFileName);
          messageApi.warning("保存文件失败，已自动使用默认下载方式");
        } catch (downloadError) {
          messageApi.error(
            "导出失败: " + ((downloadError as any).message || "未知错误"),
          );
        }
      } else {
        messageApi.error(
          "导出失败: " +
            ((error as any).response?.data?.error ||
              (error as any).message ||
              "未知错误"),
        );
      }
      console.error(error);
    } finally {
      setExporting(false);
    }
  };

  const uploadProps: UploadProps = {
    name: "file",
    multiple: false,
    fileList,
    accept: ".csv,.txt", // 限制文件选择对话框只显示 CSV 和 TXT 文件
    beforeUpload: (file) => {
      // 验证文件格式
      const formatValidation = validateFileFormat(file.name);
      if (!formatValidation.valid) {
        messageApi.error(formatValidation.message || "文件格式不支持");
        return false; // 阻止上传
      }

      // 创建正确的 UploadFile 对象
      const uploadFile: UploadFile = {
        uid: file.uid || `${Date.now()}`,
        name: file.name,
        status: "done",
        originFileObj: file,
      };
      setFileList([uploadFile]);
      return false;
    },
    onRemove: () => {
      setFileList([]);
    },
  };

  const uploadFileInChunks = async (file: File, tableName: string) => {
    // 在分片上传前验证文件格式
    const formatValidation = validateFileFormat(file.name);
    if (!formatValidation.valid) {
      messageApi.error(formatValidation.message || "文件格式不支持");
      return;
    }

    const totalChunks = Math.ceil(file.size / CHUNK_SIZE);
    const fileId = `${Date.now()}-${file.name}`;

    try {
      setIsChunkUploading(true);
      setUploadProgress(0);

      for (let chunkIndex = 0; chunkIndex < totalChunks; chunkIndex++) {
        const start = chunkIndex * CHUNK_SIZE;
        const end = Math.min(start + CHUNK_SIZE, file.size);
        const chunk = file.slice(start, end);

        const formData = new FormData();
        formData.append("chunk", chunk);
        formData.append("chunkIndex", chunkIndex.toString());
        formData.append("totalChunks", totalChunks.toString());
        formData.append("fileId", fileId);
        formData.append("fileName", file.name);
        formData.append("table", tableName);

        await axiosInstance.post("/api/upload-chunk", formData, {
          headers: { "Content-Type": "multipart/form-data" },
          timeout: 60000, // 1分钟超时
        });

        // 更新进度
        const progress = Math.round(((chunkIndex + 1) / totalChunks) * 100);
        setUploadProgress(progress);
      }

      // 通知服务器合并文件
      await axiosInstance.post("/api/merge-chunks", {
        fileId,
        fileName: file.name,
        table: tableName,
        totalChunks,
      });

      messageApi.success("文件上传成功");
      setFileList([]);
      setSelectedTableForImport("");
      setUploadProgress(0);
    } catch (error) {
      messageApi.error("文件上传失败");
      console.error(error);
    } finally {
      setIsChunkUploading(false);
    }
  };

  const handleUpload = async () => {
    if (!selectedTableForImport) {
      messageApi.error("请先选择目标表格");
      return;
    }

    if (fileList.length === 0) {
      messageApi.error("请先选择要上传的文件");
      return;
    }

    const file = fileList[0];
    if (!file?.originFileObj) {
      messageApi.error("文件对象无效");
      return;
    }

    // 在上传前再次验证文件格式
    const formatValidation = validateFileFormat(file.name);
    if (!formatValidation.valid) {
      messageApi.error(formatValidation.message || "文件格式不支持");
      return;
    }

    // 大文件使用分片上传
    if (file.originFileObj.size > 50 * 1024 * 1024) {
      // 50MB以上
      await uploadFileInChunks(file.originFileObj, selectedTableForImport);
    } else {
      // 小文件使用原有方式，加上模拟进度条
      setUploading(true);
      
      try {
        // 启动模拟进度条
        const progressPromise = simulateImportProgress();
        
        const formData = new FormData();
        formData.append("file", file.originFileObj, file.name);
        formData.append("table", selectedTableForImport);

        // 并行执行进度模拟和实际上传
        const [, response] = await Promise.all([
          progressPromise,
          axiosInstance.post("/api/upload", formData, {
            headers: { "Content-Type": "multipart/form-data" },
          })
        ]);

        // 收到响应后进度到100%
        setImportProgress(100);
        
        if (response.status >= 200 && response.status < 300) {
          messageApi.success("数据导入成功");
          setFileList([]);
          setSelectedTableForImport("");
        }
        
        // 稍微延迟后重置进度条
        setTimeout(() => {
          setImportProgress(0);
          setIsImporting(false);
        }, 1000);
        
      } catch (error) {
        messageApi.error((error as any).response?.data?.error || "导入失败");
        console.error(error);
        // 出错时也要重置进度条
        setImportProgress(0);
        setIsImporting(false);
      } finally {
        setUploading(false);
      }
    }
  };

  return (
    <div style={{ padding: "24px" }}>
      {contextHolder}
      <Tabs
        defaultActiveKey="import"
        items={[
          {
            key: "import",
            label: (
              <span>
                <ImportOutlined />
                数据导入
              </span>
            ),
            children: (
              <Space
                direction="vertical"
                size="large"
                style={{ width: "100%", padding: "20px 0" }}
              >
                <Form layout="vertical">
                  <Form.Item
                    label="选择目标表格"
                    required
                    tooltip="选择要导入数据的目标表"
                  >
                    <Select
                      style={{ width: "100%", maxWidth: 300 }}
                      options={tableOptions}
                      placeholder="请选择表格"
                      value={selectedTableForImport}
                      onChange={setSelectedTableForImport}
                    />
                  </Form.Item>

                  <Form.Item>
                    <Dragger {...uploadProps}>
                      <p className="ant-upload-drag-icon">
                        <InboxOutlined />
                      </p>
                      <p className="ant-upload-text">
                        点击或拖拽文件到此区域进行上传
                      </p>
                      <p className="ant-upload-hint">
                        仅支持 .csv 和 .txt 格式的数据文件，其他格式将被拒绝
                      </p>
                    </Dragger>
                  </Form.Item>

                  <Form.Item>
                    <Space direction="vertical" style={{ width: "100%" }}>
                      <Button
                        type="primary"
                        icon={<UploadOutlined />}
                        loading={uploading || isChunkUploading || isImporting}
                        size="large"
                        onClick={handleUpload}
                        disabled={
                          !selectedTableForImport || fileList.length === 0
                        }
                      >
                        {isChunkUploading
                          ? `分片上传中 ${uploadProgress}%`
                          : isImporting
                          ? `导入中 ${importProgress}%`
                          : "开始导入"}
                      </Button>

                      {/* 分片上传进度条 */}
                      {isChunkUploading && (
                        <div style={{ width: "100%" }}>
                          <div
                            style={{
                              height: "4px",
                              backgroundColor: "#f0f0f0",
                              borderRadius: "2px",
                              overflow: "hidden",
                            }}
                          >
                            <div
                              style={{
                                height: "100%",
                                backgroundColor: "#1890ff",
                                width: `${uploadProgress}%`,
                                transition: "width 0.3s ease",
                              }}
                            />
                          </div>
                          <div
                            style={{
                              textAlign: "center",
                              marginTop: "8px",
                              fontSize: "12px",
                              color: "#666",
                            }}
                          >
                            正在分片上传: {uploadProgress}%
                          </div>
                        </div>
                      )}

                      {/* 普通导入进度条 */}
                      {isImporting && (
                        <div style={{ width: "100%" }}>
                          <div
                            style={{
                              height: "6px",
                              backgroundColor: "#f0f0f0",
                              borderRadius: "3px",
                              overflow: "hidden",
                              marginTop: "12px",
                            }}
                          >
                            <div
                              style={{
                                height: "100%",
                                backgroundColor: "#52c41a",
                                width: `${importProgress}%`,
                                transition: "width 0.3s ease",
                                borderRadius: "3px",
                              }}
                            />
                          </div>
                          <div
                            style={{
                              textAlign: "center",
                              marginTop: "8px",
                              fontSize: "13px",
                              color: "#52c41a",
                              fontWeight: "500",
                            }}
                          >
                            {importProgress < 20 && "📄 解析文件格式..."}
                            {importProgress >= 20 && importProgress < 40 && "✅ 验证数据完整性..."}
                            {importProgress >= 40 && importProgress < 60 && "⚙️ 处理数据内容..."}
                            {importProgress >= 60 && importProgress < 80 && "💾 插入数据库..."}
                            {importProgress >= 80 && importProgress < 100 && "🔄 完成最终处理..."}
                            {importProgress === 100 && "✨ 导入完成！"}
                            {" "}({importProgress}%)
                          </div>
                        </div>
                      )}
                    </Space>
                  </Form.Item>
                </Form>
              </Space>
            ),
          },
          {
            key: "export",
            label: (
              <span>
                <ExportOutlined />
                数据导出
              </span>
            ),
            children: (
              <Space
                direction="vertical"
                size="large"
                style={{ width: "100%", padding: "20px 0" }}
              >
                <Form layout="vertical">
                  <Form.Item
                    label="选择导出表格"
                    required
                    tooltip="选择要导出的数据表"
                  >
                    <Select
                      style={{ width: "100%", maxWidth: 300 }}
                      options={tableOptions}
                      value={selectedTable}
                      onChange={setSelectedTable}
                      placeholder="请选择表格"
                    />
                  </Form.Item>

                  <Form.Item
                    label="选择导出格式"
                    required
                    tooltip="选择导出文件的格式"
                  >
                    <Select
                      style={{ width: "100%", maxWidth: 300 }}
                      options={formatOptions}
                      value={selectedFormat}
                      onChange={setSelectedFormat}
                    />
                  </Form.Item>

                  <Form.Item>
                    <Button
                      type="primary"
                      icon={<DownloadOutlined />}
                      onClick={() => setIsModalOpen(true)}
                      size="large"
                    >
                      导出数据
                    </Button>
                  </Form.Item>
                </Form>
              </Space>
            ),
          },
        ]}
      />

      {/* 简化的导出设置 Modal */}
      <Modal
        title={
          <>
            <SettingOutlined /> 导出设置
          </>
        }
        open={isModalOpen}
        onOk={handleExport}
        onCancel={() => setIsModalOpen(false)}
        confirmLoading={exporting}
        okText="确认导出"
        cancelText="取消"
        width={500}
      >
        <Form layout="vertical">
          <Form.Item label="文件名" required>
            <Input
              value={fileName}
              onChange={(e) => setFileName(e.target.value)}
              placeholder={`请输入文件名`}
              suffix={`.${selectedFormat}`}
            />
          </Form.Item>

          <Form.Item label="导出方式" required>
            <Radio.Group
              value={exportMethod}
              onChange={(e) => setExportMethod(e.target.value)}
            >
              <Space direction="vertical" style={{ width: "100%" }}>
                {getExportOptions().map((option: ExportOption) => (
                  <Radio
                    key={option.value}
                    value={option.value}
                    disabled={option.disabled}
                  >
                    <Space direction="vertical" size={2}>
                      <Space>
                        {option.icon}
                        <strong>{option.label}</strong>
                      </Space>
                      <div
                        style={{
                          color: option.disabled ? "#999" : "#666",
                          fontSize: "12px",
                          marginLeft: "20px",
                          maxWidth: "400px",
                        }}
                      >
                        {option.description}
                      </div>
                    </Space>
                  </Radio>
                ))}
              </Space>
            </Radio.Group>
          </Form.Item>

          <div
            style={{
              background: "#f6f8fa",
              border: "1px solid #e1e8ed",
              borderRadius: "6px",
              padding: "12px",
              fontSize: "12px",
              color: "#666",
            }}
          >
            <Space direction="vertical" size={4}>
              <div style={{ fontWeight: "500" }}>💡 导出方式说明：</div>
              <div>
                • <strong>浏览器默认下载</strong>
                ：文件保存到浏览器下载文件夹，兼容所有浏览器
              </div>
              <div>
                • <strong>选择保存位置</strong>：可自定义保存位置和文件名，需要
                Chrome/Edge 86+ 版本
              </div>
              <div style={{ color: "#fa8c16" }}>
                ⚠️ 如果您使用较旧的浏览器版本，请选择&#34;浏览器默认下载&#34;
              </div>
            </Space>
          </div>
        </Form>
      </Modal>
    </div>
  );
}
