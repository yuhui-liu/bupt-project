// 导出路径配置文件
// 这个文件可以根据不同的部署环境进行自定义配置

export interface PathConfig {
  value: string;
  label: string;
  description: string;
  category: "default" | "system" | "user" | "custom";
}

// 默认路径配置 - 可以根据部署环境修改
export const DEFAULT_PATH_CONFIG: PathConfig[] = [
  {
    value: "",
    label: "浏览器默认下载",
    description: "使用浏览器下载功能，适用于个人使用",
    category: "default",
  },
  {
    value: "/tmp",
    label: "系统临时目录",
    description: "通用临时文件目录，所有系统都可用",
    category: "system",
  },
  {
    value: "/var/exports",
    label: "服务器导出目录",
    description: "专用的数据导出目录",
    category: "system",
  },
  {
    value: "/opt/data",
    label: "应用数据目录",
    description: "应用程序数据存储目录",
    category: "system",
  },
];

// 根据环境变量或配置动态生成路径选项
export const getEnvironmentPaths = (): PathConfig[] => {
  const paths: PathConfig[] = [...DEFAULT_PATH_CONFIG];

  // 从环境变量获取自定义路径
  const customExportPath = process.env.NEXT_PUBLIC_EXPORT_PATH;
  if (customExportPath) {
    paths.push({
      value: customExportPath,
      label: "配置的导出目录",
      description: "管理员配置的默认导出路径",
      category: "system",
    });
  }

  // 如果是开发环境，添加更多选项
  if (process.env.NODE_ENV === "development") {
    // 使用相对于当前工作目录的路径，而不是硬编码用户路径
    paths.push({
      value: "./exports",
      label: "项目导出目录",
      description: "项目根目录下的 exports 文件夹",
      category: "user",
    });

    // 添加一些通用的开发路径建议
    paths.push({
      value: "/tmp/exports",
      label: "临时导出目录",
      description: "临时文件夹中的导出目录",
      category: "user",
    });
  }

  return paths;
};

// 验证路径是否有效的函数
export const validateExportPath = (
  path: string,
): { valid: boolean; message?: string } => {
  if (!path) {
    return { valid: true }; // 空路径表示使用默认下载
  }

  // 检查是否为绝对路径
  if (!path.startsWith("/")) {
    return {
      valid: false,
      message: "路径必须是绝对路径（以 / 开头）",
    };
  }

  // 检查路径格式
  const pathRegex = /^\/([^\/\0<>:"|?*]+\/)*[^\/\0<>:"|?*]*$/;
  if (!pathRegex.test(path)) {
    return {
      valid: false,
      message: "路径包含非法字符，请使用有效的路径格式",
    };
  }

  // 检查路径长度
  if (path.length > 255) {
    return {
      valid: false,
      message: "路径长度不能超过255个字符",
    };
  }

  return { valid: true };
};

// 生成路径建议的函数
export const generatePathSuggestions = (history: string[]): string[] => {
  const suggestions = new Set<string>();

  // 从历史记录提取父目录
  history.forEach((path) => {
    const parts = path.split("/");
    for (let i = 2; i < parts.length; i++) {
      const parentPath = parts.slice(0, i).join("/");
      if (parentPath && parentPath !== "/") {
        suggestions.add(parentPath);
      }
    }
  });

  // 添加常见的目录模式
  const commonPatterns = [
    "/data",
    "/exports",
    "/files",
    "/documents",
    "/shared",
    "/storage",
  ];

  commonPatterns.forEach((pattern) => suggestions.add(pattern));

  return Array.from(suggestions).slice(0, 5);
};
