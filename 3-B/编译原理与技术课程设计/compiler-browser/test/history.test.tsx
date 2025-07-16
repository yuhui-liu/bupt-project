import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import { History, HistoryItem } from "@/app/_components/history";

describe("History component", () => {
  test("renders a table with history items", () => {
    const history: HistoryItem[] = [
      {
        mode: 2,
        input: "begin end.",
        output: "int main() {\n  return 0;\n}",
      },
      {
        mode: 2,
        input: "begin writeln('Hello, world!') end.",
        output: `#include <stdio.h>

int main() {
  printf("Hello, world!\\n");
  return 0;
}`,
      },
    ];

    render(<History history={history} />);

    expect(screen.getByText("begin end.")).toBeInTheDocument();
    expect(
      screen.getByText(/int main\(\) {\s+return 0;\s+}/),
    ).toBeInTheDocument();
    expect(
      screen.getByText("begin writeln('Hello, world!') end."),
    ).toBeInTheDocument();
    expect(
      screen.getByText(
        /#include <stdio\.h>\s+int main\(\) {\s+printf\("Hello, world!\\n"\);\s+return 0;\s+}/,
      ),
    ).toBeInTheDocument();
  });

  test("has correct DOM structure", () => {
    const history: HistoryItem[] = [
      {
        mode: 2,
        input: "begin end.",
        output: "int main() {\n  return 0;\n}",
      },
    ];

    const { container } = render(<History history={history} />);
    const table = container.querySelector("table");
    const thead = container.querySelector("thead");
    const tbody = container.querySelector("tbody");
    const trs = container.querySelectorAll("tr");
    const ths = container.querySelectorAll("th");
    const tds = container.querySelectorAll("td");
    const pres = container.querySelectorAll("pre");
    const codes = container.querySelectorAll("code");

    expect(table).toBeInTheDocument();
    expect(thead).toBeInTheDocument();
    expect(tbody).toBeInTheDocument();
    expect(trs.length).toBeGreaterThan(0);
    expect(ths.length).toBeGreaterThan(0);
    expect(tds.length).toBeGreaterThan(0);
    expect(pres.length).toBeGreaterThan(0);
    expect(codes.length).toBeGreaterThan(0);

    expect(table).toContainElement(thead);
    expect(table).toContainElement(tbody);

    expect(thead).toContainElement(trs[0]);
    expect(trs[0]).toContainElement(ths[0]);
    expect(trs[0]).toContainElement(ths[1]);
    expect(trs[0]).toContainElement(ths[2]);

    expect(tbody).toContainElement(trs[1]);
    expect(trs[1]).toContainElement(tds[0]);
    expect(trs[1]).toContainElement(tds[1]);
    expect(trs[1]).toContainElement(tds[2]);

    expect(tds[1]).toContainElement(pres[0]);
    expect(pres[0]).toContainElement(codes[0]);

    expect(tds[2]).toContainElement(pres[1]);
    expect(pres[1]).toContainElement(codes[1]);

    expect(ths[0]).toHaveTextContent("Mode");
    expect(ths[1]).toHaveTextContent("Input");
    expect(ths[2]).toHaveTextContent("Result");
    expect(tds[0]).toHaveTextContent("C code");
    expect(codes[0]).toHaveTextContent("begin end.");
    expect(codes[1]).toHaveTextContent("int main() { return 0; }");
  });
});
