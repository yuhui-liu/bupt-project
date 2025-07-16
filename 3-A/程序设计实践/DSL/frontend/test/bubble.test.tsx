import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import { ClientBubble, ServerBubble } from "@components/bubble";

describe("ClientBubble Component", () => {
  test("renders the message passed as a prop", () => {
    const message = "Hello, World!";

    // 使用 message 属性渲染组件
    render(<ClientBubble message={message} />);

    // 断言消息是否正确显示
    const messageElement = screen.getByText(message);
    expect(messageElement).toBeInTheDocument();
    expect(messageElement).toHaveClass("bubble my-bubble self-end mr-2");
  });

  test("has the correct DOM structure", () => {
    const message = "Test Message";

    // 渲染组件
    render(<ClientBubble message={message} />);

    // 检查外层 div
    const outerDiv = screen.getByText(message).closest("div");
    expect(outerDiv).toHaveClass("flex flex-col pr-2");

    // 检查右侧 div 是否存在
    const rightDiv = outerDiv?.querySelector(".right");
    expect(rightDiv).toBeInTheDocument();
  });
});

describe("ServerBubble Component", () => {
  test("renders the message passed as a prop", () => {
    const message = "Hello, World!";

    // 使用 message 属性渲染组件
    render(<ServerBubble message={message} />);

    // 断言消息是否正确显示
    const messageElement = screen.getByText(message);
    expect(messageElement).toBeInTheDocument();
    expect(messageElement).toHaveClass("bubble your-bubble self-start ml-2");
  });

  test("has the correct DOM structure", () => {
    const message = "Test Message";

    // 渲染组件
    render(<ServerBubble message={message} />);

    // 检查外层 div
    const outerDiv = screen.getByText(message).closest("div");
    expect(outerDiv).toHaveClass("flex flex-col");

    // 检查左侧 div 是否存在
    const leftDiv = outerDiv?.querySelector(".left");
    expect(leftDiv).toBeInTheDocument();
  });
});
