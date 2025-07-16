import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import Container from "@components/container";

describe("Container Component", () => {
  test("renders children correctly", () => {
    const childText = "Hello, World!";

    render(
      <Container>
        <p>{childText}</p>
      </Container>,
    );

    const childElement = screen.getByText(childText);
    expect(childElement).toBeInTheDocument();
  });

  test("has the correct structure and styles", () => {
    const { container } = render(
      <Container>
        <div>Test Child</div>
      </Container>,
    );

    expect(container.firstChild).toHaveClass(
      "mx-auto flex flex-col h-screen w-10/12 border-2 border-x-gray-500 border-y-transparent px-10 py-2 md:w-1/2",
    );
  });
});
