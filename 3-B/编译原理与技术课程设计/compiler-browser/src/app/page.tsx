"use client";

import React, { useRef, useState, useEffect, JSX } from "react";
import { Fira_Code } from "next/font/google";

import CodeMirror, { ReactCodeMirrorRef } from "@uiw/react-codemirror";
import { langs } from "@uiw/codemirror-extensions-langs";
import { gruvboxDark, gruvboxLight } from "@uiw/codemirror-theme-gruvbox-dark";
import { Toaster, toast } from "react-hot-toast";

import Container from "@/app/_components/container";
import Button from "@/app/_components/button";
import { History, HistoryItem } from "@/app/_components/history";

const firaCode = Fira_Code({ subsets: ["latin"] });

/**
 * Home 组件 - 编译器应用的主页面
 *
 * 一个基于 Web 的编译器界面，允许用户输入 Pascal 代码并将其编译为 C 代码。
 * 主要功能包括：
 * - 支持实时代码编辑和语法高亮
 * - Pascal 到 C 的代码编译
 * - 编译结果的复制功能
 * - 编译状态的可视化反馈
 * - 显示编译历史记录
 * - 缓存编译结果，节省重复编译时间
 * - 支持暗色和亮色主题
 *
 * @returns {JSX.Element} 渲染后的 Home 组件
 */
export default function Home(): JSX.Element {
  // 创建两个 CodeMirror 编辑器的引用
  const outputRef = useRef<ReactCodeMirrorRef>(null);
  const inputRef = useRef<ReactCodeMirrorRef>(null);
  // 标识是否正在编译
  const [isCompiling, setIsCompiling] = useState(false);
  // 存储编译结果
  const [result, setResult] = useState("");
  // 输入框的值
  const [inputValue, setInputValue] = useState("");
  // 存储编译历史记录
  const [history, setHistory] = useState<HistoryItem[]>([]);
  // 控制历史记录的显示和隐藏
  const [showHistory, setShowHistory] = useState(false);
  // 控制历史记录的动画效果
  const [historyClass, setHistoryClass] = useState("");
  // 标识亮色/暗色主题
  const [isDark, setIsDark] = useState(false);
  // 标识编译是否出错，控制输出框的语言模式
  const [errorOccured, setErrorOccured] = useState(false);
  // 标识请求结果的模式
  const [resultMode, setResultMode] = useState("ccode");
  // 标识当前输出框的语言模式
  const [outputMode, setOutputMode] = useState("ccode");
  const getModeNum = (mode: string) => {
    switch (mode) {
      case "tokens":
        return 0;
      case "ast":
        return 1;
      case "ccode":
        return 2;
      default:
        return 2;
    }
  };

  // 监听系统主题变化
  useEffect(() => {
    const mq = window.matchMedia("(prefers-color-scheme: dark)");

    if (mq.matches) {
      setIsDark(true);
    }

    mq.addEventListener("change", (evt) => setIsDark(evt.matches));
  }, []);

  // 复制编译结果到剪贴板
  const copyOutput = () => {
    if (outputRef.current) {
      navigator.clipboard
        .writeText(result)
        .then(() => {
          toast.success("🥳复制成功！", {
            duration: 2000,
            position: "top-center",
          });
        })
        .catch((err) => {
          toast.error("😟复制失败");
          console.error("Failed to copy: ", err);
        });
    }
  };

  // 处理表单提交事件：首先检查输入是否为空，然后检查是否存在于历史记录中
  // 如果存在，则直接显示历史记录中的结果，否则向后端发送编译请求
  // 收到响应后更新结果和历史记录
  // 错误时将输出框的语言模式设置为纯文本
  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();

    if (inputValue === "") return;

    setIsCompiling(true);
    toast.loading("⏳编译中...", { id: "compile" });

    const foundInHistory = history.find((item) => item.input === inputValue && item.mode === getModeNum(resultMode));
    if (foundInHistory) {
      setResult(foundInHistory.output);
      setErrorOccured(false);
      setOutputMode(resultMode);
      setIsCompiling(false);
      toast.success("🎊编译成功！", {
        id: "compile",
        duration: 2000,
      });
      return;
    }

    try {
      const mode = getModeNum(resultMode);
      const response = await fetch(
        `${process.env.NEXT_PUBLIC_API_URL}/compile`,
        {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
          },
          body: JSON.stringify({ code: inputValue, mode: mode }),
        },
      );

      const data = await response.json();

      if (!response.ok) throw new Error(data.message);

      setResult(data.result);
      setErrorOccured(false);
      setOutputMode(resultMode);

      const newHistory = [
        ...history,
        { mode: mode, input: inputValue, output: data.result },
      ];
      setHistory(newHistory);

      toast.success("🎊编译成功！", {
        id: "compile",
        duration: 2000,
      });
    } catch (error) {
      toast.error("😟编译失败，请重试", { id: "compile" });
      setResult((error as Error).message);
      setErrorOccured(true);
      console.error("Compilation error:", error);
    } finally {
      setIsCompiling(false);
    }
  };

  // 控制历史记录的显示和隐藏的动画效果
  const toggleHistory = () => {
    if (showHistory) {
      setHistoryClass("fade-out");
      setTimeout(() => {
        setShowHistory(false);
        setHistoryClass("");
      }, 200);
    } else {
      setShowHistory(true);
      setHistoryClass("fade-in");
    }
  };

  return (
    <>
      {/* 使用FiraCode */}
      <style jsx global>{`
        .cm-content {
          font-family: ${firaCode.style.fontFamily}, monospace;
        }
      `}</style>
      <Container>
        <Toaster />
        <div className="my-3 flex flex-row justify-between">
          <p className="pl-3 text-4xl">Welcome</p>
          <div>
            <select
              className="mr-10 rounded-2xl border-4 px-2 text-xl transition duration-300 border-violet-300 bg-violet-300 hover:border-blue-500 hover:bg-blue-400 dark:border-violet-700 dark:bg-violet-700 dark:hover:border-blue-700 dark:hover:bg-blue-600"
              onChange={(e) => setResultMode(e.target.value)}
              value={resultMode}
            >
              <option value="tokens">输出Token序列</option>
              <option value="ast">输出AST</option>
              <option value="ccode">输出C代码</option>
            </select>
            <Button
              onClick={toggleHistory}
              text={`ℹ️${showHistory ? "隐藏" : "显示"}历史记录`}
            />
            <Button
              text="⚙️编译"
              onClick={handleSubmit}
              disabled={isCompiling}
            />
            <Button text="📋复制输出" onClick={copyOutput} />
          </div>
        </div>
        <div className="flex flex-1 flex-row justify-between">
          <CodeMirror
            className="w-1/2"
            height="100%"
            extensions={[langs.pascal()]}
            theme={isDark ? gruvboxDark : gruvboxLight}
            placeholder="请输入源代码"
            onChange={(value) => setInputValue(value)}
            ref={inputRef}
            data-testid="input-editor"
          />

          <CodeMirror
            value={result}
            className="w-1/2"
            height="100%"
            extensions={(errorOccured || outputMode != "ccode") ? [] : [langs.c()]}
            theme={isDark ? gruvboxDark : gruvboxLight}
            placeholder="编译结果"
            editable={false}
            ref={outputRef}
            data-testid="output-editor"
          />
        </div>
        <div
          data-testid="history-panel"
          className={`${showHistory ? historyClass : "hidden"} absolute mt-20 z-10 text-lg w-2/3 h-5/6 right-1/2 translate-x-1/2 overflow-auto rounded-lg`}
        >
          {showHistory && <History history={history} />}
        </div>
      </Container>
    </>
  );
}
