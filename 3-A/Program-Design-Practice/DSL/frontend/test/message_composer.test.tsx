import { render, screen, fireEvent } from "@testing-library/react";
import "@testing-library/jest-dom";
import MessageComposer from "@components/message_composer";

describe("MessageComposer Component", () => {
  test("allows user to type in the input field", () => {
    render(<MessageComposer sendMessage={() => {}} />);

    const inputElement = screen.getByPlaceholderText("请输入内容");
    fireEvent.change(inputElement, { target: { value: "Hello, World!" } });

    expect(inputElement).toHaveValue("Hello, World!");
  });

  test("calls sendMessage and clears input on button click", () => {
    const sendMessageMock = jest.fn();
    render(<MessageComposer sendMessage={sendMessageMock} />);

    const inputElement = screen.getByPlaceholderText("请输入内容");
    const buttonElement = screen.getByRole("button", { name: "发送" });

    fireEvent.change(inputElement, { target: { value: "Test Message" } });
    fireEvent.click(buttonElement);

    expect(sendMessageMock).toHaveBeenCalledWith("Test Message");
    expect(inputElement).toHaveValue("");
  });

  test("calls sendMessage and clears input on Enter key press", () => {
    const sendMessageMock = jest.fn();
    render(<MessageComposer sendMessage={sendMessageMock} />);

    const inputElement = screen.getByPlaceholderText("请输入内容");

    fireEvent.change(inputElement, {
      target: { value: "Another Test Message" },
    });
    fireEvent.keyDown(inputElement, { key: "Enter", code: "Enter" });

    expect(sendMessageMock).toHaveBeenCalledWith("Another Test Message");
    expect(inputElement).toHaveValue("");
  });
});
