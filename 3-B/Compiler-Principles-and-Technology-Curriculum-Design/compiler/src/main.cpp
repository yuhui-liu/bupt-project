#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "exception.hpp"
#include "generator.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "printer.hpp"
#include "semantic_analyzer.hpp"

// 输出模式：词法分析结果、语法分析结果、C代码
enum class OUTPUT_MODE { LEXER_RES, PARSER_RES, C_CODE };
/// @brief 解析输入
/// @return 输出模式和源代码
auto parseInput() -> std::tuple<OUTPUT_MODE, bool, std::string>;
/// @brief 输出词法分析结果
void solveLexerResult(const std::string&);
/// @brief 输出语法分析结果
void solveParserResult(const std::string&, bool);
/// @brief 输出C代码
void solveCCode(const std::string&);

auto main(int argc, const char* argv[]) -> int {
  switch (auto [output_mode, use_color, src] = parseInput(); output_mode) {
    case OUTPUT_MODE::LEXER_RES:
      solveLexerResult(src);
      break;
    case OUTPUT_MODE::PARSER_RES:
      solveParserResult(src, use_color);
      break;
    case OUTPUT_MODE::C_CODE:
      solveCCode(src);
      break;
  }
  return EXIT_SUCCESS;
}

auto parseInput() -> std::tuple<OUTPUT_MODE, bool, std::string> {
  auto numToMode = [](const int a) -> OUTPUT_MODE {
    if (a == 0) return OUTPUT_MODE::LEXER_RES;
    if (a == 1) return OUTPUT_MODE::PARSER_RES;
    return OUTPUT_MODE::C_CODE;
  };
  int modeNum;
  bool use_color = false;
  std::cin >> modeNum >> use_color;  // use_color 只在 solveParserResult 中使用
  std::getchar();                    // consume the \n after the mode number

  std::stringstream ss;
  std::string line;
  while (std::getline(std::cin, line)) {
    ss << line;
    if (!std::cin.eof()) {
      ss << "\n";
    }
  }

  return {numToMode(modeNum), use_color, ss.str()};
}

void solveLexerResult(const std::string& src) {
  Lexer lexer(src);
  try {
    lexer.scan();
  } catch (const std::vector<LexerException>& ex) {
    for (const auto& e : ex) {
      std::cerr << "Lexer error: "
                << "At line " << e.getLine() << ": " << e.what() << std::endl;
    }
    exit(EXIT_FAILURE);
  }
  lexer.printTokens();
}

void solveParserResult(const std::string& src, bool use_color) {
  Lexer lexer(src);
  try {
    lexer.scan();
  } catch (const std::vector<LexerException>& ex) {
    for (const auto& e : ex) {
      std::cerr << "Lexer error: "
                << "At line " << e.getLine() << ": " << e.what() << std::endl;
    }
    exit(EXIT_FAILURE);
  }

  Parser parser(lexer.getTokens());
  try {
    const auto program = parser.program();
    const Printer printer(use_color);
    program->accept(printer);
    std::cout << printer.getResult();
    printer.clear();
  } catch (const ParserException& e) {
    std::cerr << "Parser error: "
              << "At line " << e.getLine() << ": " << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}

void solveCCode(const std::string& src) {
  Lexer lexer(src);
  try {
    lexer.scan();
  } catch (const std::vector<LexerException>& ex) {
    for (const auto& e : ex) {
      std::cerr << "Lexer error: "
                << "At line " << e.getLine() << ": " << e.what() << std::endl;
    }
    exit(EXIT_FAILURE);
  }
  Parser parser(lexer.getTokens());
  ProgramPtr program = nullptr;

  try {
    program = parser.program();
  } catch (const ParserException& e) {
    std::cerr << "Parser error: "
              << "At line " << e.getLine() << ": " << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  const SemanticAnalyzer semantic_analyzer;
  program->accept(semantic_analyzer);
  if (semantic_analyzer.hasError()) {
    exit(EXIT_FAILURE);
  }
  auto st = semantic_analyzer.getGlobalSymbolTable();  // 后续代码可以使用该符号表
  auto is_function_return = semantic_analyzer.getIsFunctionReturn();
  auto read_fmt_specifier = semantic_analyzer.getReadFmtSpecifier();
  auto write_fmt_specifier = semantic_analyzer.getWriteFmtSpecifier();
  auto is_factor_function_call = semantic_analyzer.getIsFactorFunctionCall();
  auto is_factor_with_not_number = semantic_analyzer.getIsFactorWithNotNumber();
  auto is_var_param = semantic_analyzer.getIsVarParam();
  auto params_name = semantic_analyzer.getParamsName();

  Generator generator(is_function_return, read_fmt_specifier, write_fmt_specifier, is_factor_function_call,
                      is_factor_with_not_number, is_var_param, params_name);
  program->accept(generator);
  std::cout << generator.getResult();
  generator.clear();
}
