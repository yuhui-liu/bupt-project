import React, { useEffect, useRef } from "react";

/**
 * MessageDisplay 组件渲染消息列表并自动滚动到最新消息。
 *
 * @param {Object} props - 组件的属性。
 * @param {React.JSX.Element[]} props.messages - 要显示的消息元素数组。
 * @returns {JSX.Element} 渲染的消息显示组件。
 */
export default function MessageDisplay({
  messages,
}: {
  messages: React.JSX.Element[];
}): JSX.Element {
  const scrollRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (scrollRef.current)
      scrollRef.current.scrollIntoView({ behavior: "smooth" });
  }, [messages]);

  return (
    <div className="flex flex-col h-5/6 overflow-y-auto">
      {messages}
      <div ref={scrollRef}></div>
    </div>
  );
}
