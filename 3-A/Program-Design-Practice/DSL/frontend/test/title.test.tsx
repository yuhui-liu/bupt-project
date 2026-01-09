import React from "react";
import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import Title from "@components/title";

describe("Title Component", () => {
  test("renders the title text", () => {
    const titleText = "Test Title";

    render(<Title title={titleText} />);

    // Assert that the title text is rendered
    const titleElement = screen.getByText(titleText);
    expect(titleElement).toBeInTheDocument();
  });

  test("applies the correct CSS classes", () => {
    const titleText = "Styled Title";

    render(<Title title={titleText} />);

    // Assert that the title element has the correct classes
    const titleElement = screen.getByText(titleText);
    expect(titleElement).toHaveClass("text-center mb-8 text-3xl");
  });
});
