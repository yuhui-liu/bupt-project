import React from 'react';
import { render, screen, fireEvent, act } from '@testing-library/react';
import '@testing-library/jest-dom';
import Home from '@/app/page';
import dotenv from 'dotenv';

dotenv.config();

// 模拟 WebSocket
let wsMock: any;
global.WebSocket = jest.fn(() => {
  wsMock = {
    send: jest.fn(),
    close: jest.fn(),
    onmessage: null as ((this: WebSocket, ev: MessageEvent) => any) | null,
    onerror: null as ((this: WebSocket, ev: Event) => any) | null,
    readyState: WebSocket.CONNECTING,
  };
  return wsMock;
}) as unknown as typeof WebSocket;

(global.WebSocket as any).CONNECTING = 0;
(global.WebSocket as any).OPEN = 1;
(global.WebSocket as any).CLOSING = 2;
(global.WebSocket as any).CLOSED = 3;

beforeAll(() => {
  // 模拟 scrollIntoView
  window.HTMLElement.prototype.scrollIntoView = jest.fn();
});

describe('Home Component', () => {
  beforeEach(() => {
    jest.clearAllMocks();
  });

  test('renders the main components', () => {
    render(<Home />);

    expect(screen.getByText('客服机器人')).toBeInTheDocument();
    expect(screen.getByPlaceholderText('请输入内容')).toBeInTheDocument();
  });

  test('sends a message through WebSocket and displays it', async () => {
    render(<Home />);

    const inputElement = screen.getByPlaceholderText('请输入内容');
    const buttonElement = screen.getByRole('button', { name: /发送/i });

    // 模拟输入消息
    fireEvent.change(inputElement, { target: { value: 'Hello, World!' } });

    // 模拟发送消息
    fireEvent.click(buttonElement);

    // 验证 send 方法是否被正确的消息调用
    expect(wsMock.send).toHaveBeenCalledWith('Hello, World!');

    // 验证客户端消息是否显示
    expect(screen.getByText('Hello, World!')).toBeInTheDocument();
  });

  test('receives a message from WebSocket and displays it', async () => {
    render(<Home />);

    // 模拟接收消息
    act(() => {
      wsMock.onmessage?.({ data: 'Message from server' } as MessageEvent);
    });

    // 检查来自服务器的消息是否显示
    expect(screen.getByText('Message from server')).toBeInTheDocument();
  });

  test('shows an alert on WebSocket error', async () => {
    jest.spyOn(window, 'alert').mockImplementation(() => {});

    render(<Home />);

    // 模拟 WebSocket 错误
    act(() => {
      wsMock.onerror?.(new Event('error'));
    });

    expect(window.alert).toHaveBeenCalledWith('error');

    // 清理 mock
    jest.restoreAllMocks();
  });
});
