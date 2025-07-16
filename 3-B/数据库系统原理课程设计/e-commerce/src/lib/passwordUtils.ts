// 密码强度检测工具函数
export interface PasswordStrength {
  score: number; // 0-3 分数
  level: "weak" | "medium" | "strong";
  color: string;
  text: string;
  percentage: number;
}

export const checkPasswordStrength = (password: string): PasswordStrength => {
  if (!password) {
    return {
      score: 0,
      level: "weak",
      color: "#ff4d4f",
      text: "请输入密码",
      percentage: 0,
    };
  }

  let score = 0;
  const checks = {
    length: password.length >= 8,
    hasLowerCase: /[a-z]/.test(password),
    hasUpperCase: /[A-Z]/.test(password),
    hasNumbers: /\d/.test(password),
    hasSpecialChar: /[!@#$%^&*(),.?":{}|<>]/.test(password),
  };

  // 基础要求检查
  if (checks.length) score += 1;
  if (checks.hasLowerCase && checks.hasUpperCase) score += 1;
  if (checks.hasNumbers) score += 1;
  if (checks.hasSpecialChar) score += 1;

  // 根据分数确定强度等级
  let level: "weak" | "medium" | "strong";
  let color: string;
  let text: string;
  let percentage: number;

  if (score <= 1) {
    level = "weak";
    color = "#ff4d4f";
    text = "弱";
    percentage = 25;
  } else if (score <= 2) {
    level = "medium";
    color = "#faad14";
    text = "中等";
    percentage = 60;
  } else {
    level = "strong";
    color = "#52c41a";
    text = "强";
    percentage = 100;
  }

  return {
    score,
    level,
    color,
    text,
    percentage,
  };
};

// 密码验证规则
export const passwordValidationRules = [
  {
    required: true,
    message: "请输入密码！",
  },
  {
    min: 8,
    message: "密码至少8位字符！",
  },
  {
    validator: (_: any, value: string) => {
      if (!value) return Promise.resolve();

      const hasLowerCase = /[a-z]/.test(value);
      const hasUpperCase = /[A-Z]/.test(value);
      const hasNumbers = /\d/.test(value);

      if (!hasLowerCase || !hasUpperCase || !hasNumbers) {
        return Promise.reject(new Error("密码必须包含大小写字母和数字！"));
      }

      return Promise.resolve();
    },
  },
];
