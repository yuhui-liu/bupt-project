import React from "react";

/**
 * Title 组件渲染一个标题文本。
 *
 * @param {Object} props - 组件的属性。
 * @param {string} props.title - 要显示的标题文本。
 * @returns {JSX.Element} 渲染的标题组件。
 */
export default function Title({ title }: { title: string }): JSX.Element {
  return <p className="text-center mb-8 text-3xl">{title}</p>;
}
