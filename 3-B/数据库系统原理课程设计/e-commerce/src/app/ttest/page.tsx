"use client";
import React from "react";

export default function Page() {
  const tt = async () => {
    // 创建一个新句柄
    const newHandle = await window.showSaveFilePicker();

    // 创建一个 FileSystemWritableFileStream 用于写入
    const writableStream = await newHandle.createWritable();

    // 写入我们的文件
    await writableStream.write("1");

    // 关闭文件并将内容写入到磁盘
    await writableStream.close();
  };
  return (
    <div style={{ padding: "20px" }}>
      <h1>饼图示例</h1>
      <button onClick={tt}>点击保存文件</button>
    </div>
  );
}
