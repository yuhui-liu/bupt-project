import React from "react";
import "@styles/bubble.css";

/**
 * ClientBubble 组件渲染客户端的消息气泡。
 *
 * @param {Object} props - 组件的属性。
 * @param {string} props.message - 要显示在气泡中的消息。
 * @returns {JSX.Element} 渲染的客户端消息气泡。
 */
export function ClientBubble({ message }: { message: string }): JSX.Element {
  return (
    <div className="flex flex-col pr-2">
      <p className="bubble my-bubble self-end mr-2">{message}</p>
      <div className="right"></div>
    </div>
  );
}

/**
 * ServerBubble 组件渲染服务器的消息气泡。
 *
 * @param {Object} props - 组件的属性。
 * @param {string} props.message - 要显示在气泡中的消息。
 * @returns {JSX.Element} 渲染的服务器消息气泡。
 */
export function ServerBubble({ message }: { message: string }): JSX.Element {
  return (
    <div className="flex flex-col">
      <p className="bubble your-bubble self-start ml-2">{message}</p>
      <div className="left"></div>
    </div>
  );
}
