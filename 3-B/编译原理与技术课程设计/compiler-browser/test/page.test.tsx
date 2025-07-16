import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import Home from "@/app/page";
import React from "react";

beforeAll(() => {
  window.matchMedia = jest.fn().mockImplementation((query) => {
    return {
      matches: false,
      media: query,
      onchange: null,
      addEventListener: jest.fn(),
      removeEventListener: jest.fn(),
      addListener: jest.fn(),
      removeListener: jest.fn(),
      dispatchEvent: jest.fn(),
    };
  });
});

describe("Home component", () => {
  test("renders correctly", () => {
    render(<Home />);

    const title = screen.getByText("Welcome");
    const historyButton = screen.getByText("ℹ️显示历史记录");
    const compileButton = screen.getByText("⚙️编译");
    const copyButton = screen.getByText("📋复制输出");
    const inputEditor = screen.getByTestId("input-editor");
    const outputEditor = screen.getByTestId("output-editor");

    expect(title).toBeInTheDocument();
    expect(historyButton).toBeInTheDocument();
    expect(compileButton).toBeInTheDocument();
    expect(copyButton).toBeInTheDocument();
    expect(inputEditor).toBeInTheDocument();
    expect(outputEditor).toBeInTheDocument();
  });
});
