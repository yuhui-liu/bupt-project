import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import MessageDisplay from "@components/message_display";

describe("MessageDisplay Component", () => {
  // 模拟 window.scrollIntoView
  beforeAll(() => {
    window.HTMLElement.prototype.scrollIntoView = jest.fn();
  });

  test("renders all messages", () => {
    const messages = [
      <p key="1">Hello, World!</p>,
      <p key="2">How are you?</p>,
      <p key="3">Goodbye!</p>,
    ];

    render(<MessageDisplay messages={messages} />);

    messages.forEach((message) => {
      expect(
        screen.getByText(message.props.children as string),
      ).toBeInTheDocument();
    });
  });

  test("auto-scrolls to the latest message", () => {
    const messages = [
      <p key="1">Message 1</p>,
      <p key="2">Message 2</p>,
      <p key="3">Message 3</p>,
    ];

    const { rerender } = render(
      <MessageDisplay messages={messages.slice(0, 2)} />,
    );

    // 重新渲染新消息
    rerender(<MessageDisplay messages={messages} />);

    // 检查是否调用了 scrollIntoView
    expect(window.HTMLElement.prototype.scrollIntoView).toHaveBeenCalled();
  });
});
