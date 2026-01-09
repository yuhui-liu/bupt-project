"use client";
import { useState, useEffect, useRef } from "react";
import Container from "@components/container";
import Title from "@components/title";
import MessageComposer from "@components/message_composer";
import MessageDisplay from "@components/message_display";
import { ServerBubble, ClientBubble } from "@components/bubble";
import dotenv from "dotenv";

dotenv.config();
const port = process.env.PORT || 8080;
export default function Home() {
  const serverUrl = `ws://localhost:${port}`;
  const [messages, setMessages] = useState<React.JSX.Element[]>([]);
  const wsRef = useRef<WebSocket>();

  useEffect(() => {
    const ws = new WebSocket(serverUrl);
    wsRef.current = ws;
    ws.onmessage = (msg) => {
      console.log("receive new message");
      setMessages((m) => [
        ...m,
        <ServerBubble key={Date.now()} message={msg.data} />,
      ]);
    };
    ws.onerror = (event) => {
      console.log("error", event);
      alert("error");
    };
    return () => {
      ws.close();
    };
  }, [serverUrl]);

  return (
    <Container>
      <Title title="客服机器人" />
      <MessageDisplay messages={messages} />
      <MessageComposer
        sendMessage={(msg: string) => {
          if (msg.trim() !== "") {
            if (wsRef.current) wsRef.current.send(msg);
            setMessages((m) => [
              ...m,
              <ClientBubble key={Date.now()} message={msg} />,
            ]);
          }
        }}
      />
    </Container>
  );
}
