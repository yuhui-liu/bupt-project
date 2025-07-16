import React, { JSX } from "react";

/**
 * 容器组件 - 为子组件提供统一的布局容器
 *
 * @param {Object} props - 组件属性
 * @param {React.ReactNode} props.children - 子组件内容
 * @returns {JSX.Element} 返回一个带有固定样式的div容器
 *
 * @example
 * ```tsx
 * <Container>
 *   <YourComponent />
 * </Container>
 * ```
 */
function Container({
  children,
}: {
  children: React.ReactNode;
}): JSX.Element {
  return (
    <div className="mx-auto flex flex-col h-screen w-10/12 px-10 py-2">
      {children}
    </div>
  );
}

export default Container;