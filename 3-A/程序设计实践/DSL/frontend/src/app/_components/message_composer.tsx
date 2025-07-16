import React, { useState } from "react";
import "@styles/msgcomp.css";

/**
 * MessageComposer 组件允许用户撰写和发送消息。
 *
 * @param {Object} props - 组件的属性。
 * @param {Function} props.sendMessage - 发送消息时调用的函数。
 * @returns {JSX.Element} 渲染的消息撰写组件。
 */
export default function MessageComposer({
  sendMessage,
}: {
  sendMessage: (msg: string) => void;
}): JSX.Element {
  const [newMsg, setNewMsg] = useState("");
  return (
    <div className="mt-2 flex">
      <input
        type="text"
        placeholder="请输入内容"
        className="h-full w-full rounded-lg border-2 bg-transparent p-3 outline-none input-border"
        value={newMsg}
        onChange={(e) => {
          setNewMsg(e.target.value);
        }}
        onKeyDown={(event) => {
          if (event.key === "Enter") {
            sendMessage(newMsg);
            setNewMsg("");
          }
        }}
      />
      <button
        onClick={() => {
          sendMessage(newMsg);
          setNewMsg("");
        }}
        className="button-border button-bg mx-2 rounded-full border-2"
      >
        发送
      </button>
    </div>
  );
}
