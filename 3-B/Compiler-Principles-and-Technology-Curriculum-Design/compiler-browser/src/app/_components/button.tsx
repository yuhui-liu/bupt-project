import React, { JSX } from "react";

/**
 * Button - 一个通用的按钮组件
 *
 * @param {Object} props - 组件属性
 * @param {string} props.text - 按钮显示的文本内容
 * @param {function} props.onClick - 点击按钮时触发的回调函数
 * @param {boolean} [props.disabled=false] - 是否禁用按钮，默认为false
 * 
 * @returns {JSX.Element} 渲染的按钮组件
 */
export default function Button({
  text,
  onClick,
  disabled = false,
}: {
  text: string;
  onClick: (event: React.MouseEvent<HTMLButtonElement>) => void;
  disabled?: boolean;
}): JSX.Element {
  return (
    <button
      className={`mr-10 rounded-2xl border-4 px-2 text-2xl transition duration-300
      ${
        disabled
          ? "border-gray-300 bg-gray-300 cursor-not-allowed opacity-50"
          : "border-violet-300 bg-violet-300 hover:border-blue-500 hover:bg-blue-400 dark:border-violet-700 dark:bg-violet-700 dark:hover:border-blue-700 dark:hover:bg-blue-600"
      }`}
      onClick={onClick}
      disabled={disabled}
    >
      {text}
    </button>
  );
}
