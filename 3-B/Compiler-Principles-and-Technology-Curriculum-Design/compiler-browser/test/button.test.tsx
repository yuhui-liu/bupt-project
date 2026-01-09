import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import Button from "@/app/_components/button";

describe("Button component", () => {
  test("displays correct text", () => {
    const text = "ButtonText";

    render(<Button text={text} onClick={() => {}} />);

    const buttonElement = screen.getByText(text);
    expect(buttonElement).toBeInTheDocument();
  });

  test("calls onClick function when clicked", () => {
    const onClick = jest.fn();

    render(<Button text="ButtonText" onClick={onClick} />);

    const buttonElement = screen.getByText("ButtonText");
    buttonElement.click();

    expect(onClick).toHaveBeenCalled();
  });

  test("is disabled when disabled prop is true", () => {
    render(<Button text="ButtonText" onClick={() => {}} disabled />);

    const buttonElement = screen.getByText("ButtonText");
    expect(buttonElement).toBeDisabled();
  });

  test("is enabled when disabled prop is false", () => {
    render(<Button text="ButtonText" onClick={() => {}} disabled={false} />);

    const buttonElement = screen.getByText("ButtonText");
    expect(buttonElement).not.toBeDisabled();
  });

  test("has correct styles when disabled", () => {
    render(<Button text="ButtonText" onClick={() => {}} disabled />);

    const buttonElement = screen.getByText("ButtonText");
    expect(buttonElement).toHaveClass(
      "border-gray-300 bg-gray-300 cursor-not-allowed opacity-50",
    );
  });

  test("has correct styles when enabled", () => {
    render(<Button text="ButtonText" onClick={() => {}} disabled={false} />);

    const buttonElement = screen.getByText("ButtonText");
    expect(buttonElement).toHaveClass(
      "border-violet-300 bg-violet-300 hover:border-blue-500 hover:bg-blue-400 dark:border-violet-700 dark:bg-violet-700 dark:hover:border-blue-700 dark:hover:bg-blue-600",
    );
  });
});
