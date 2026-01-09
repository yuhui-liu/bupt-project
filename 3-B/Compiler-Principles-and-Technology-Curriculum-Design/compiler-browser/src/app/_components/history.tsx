import React, { JSX } from "react";

/**
 * 历史记录项的接口定义
 *
 * @interface HistoryItem
 * 用于存储编译的输入和输出记录的数据结构
 *
 * @property {number} mode - 编译模式，0 表示 tokens，1 表示 ast，2 表示 C 代码
 * @property {string} input - 用户输入的原始代码
 * @property {string} output - 编译后生成的代码
 */
export interface HistoryItem {
  mode: number;
  input: string;
  output: string;
}

/**
 * History 组件 - 显示编译历史记录
 *
 * 以表格形式展示编译历史记录的组件，包括输入代码和编译结果
 *
 * @param {Object} props - 组件属性
 * @param {HistoryItem[]} props.history - 包含编译历史记录的数组
 *
 * @returns {JSX.Element} 渲染后的历史记录表格
 */
export function History({ history }: { history: HistoryItem[] }): JSX.Element {
  const getModeName = (mode: number): string => {
    switch (mode) {
      case 0:
        return "Tokens";
      case 1:
        return "AST";
      case 2:
        return "C code";
      default:
        return "C code";
    };
  };
  return (
    <table
      className="border-separate w-full rounded-lg font-mono"
      style={{ backgroundColor: "#45aa78ee" }}
    >
      <thead>
        <tr>
          <th>Mode</th>
          <th>Input</th>
          <th>Result</th>
        </tr>
      </thead>
      <tbody>
        {history.map((item, index) => (
          <tr key={index}>
            <td className="border-2 border-red-300 rounded-lg text-center">
              {getModeName(item.mode)}
            </td>
            <td className="border-2 border-red-300 rounded-lg">
              <pre className="pl-3">
                <code>{item.input}</code>
              </pre>
            </td>
            <td className="border-2 border-red-300 rounded-lg">
              <pre className="pl-3">
                <code>{item.output}</code>
              </pre>
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  );
}
