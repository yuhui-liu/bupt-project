import React from "react";

/**
 * 容器组件，用一个带样式的 div 包裹其子元素。
 *
 * @param {Object} props - 组件的属性。
 * @param {React.ReactNode} props.children - 要在容器内渲染的子元素。
 * @returns {JSX.Element} 渲染的容器及其子元素。
 */
export default function Container({
  children,
}: {
  children: React.ReactNode;
}): JSX.Element {
  return (
    <div className="mx-auto flex flex-col h-screen w-10/12 border-2 border-x-gray-500 border-y-transparent px-10 py-2 md:w-1/2">
      {children}
    </div>
  );
}
