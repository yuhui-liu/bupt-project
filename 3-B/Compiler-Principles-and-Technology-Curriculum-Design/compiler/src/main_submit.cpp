// In this submission version, we don't need error handling
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "generator.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semantic_analyzer.hpp"

auto main(int argc, const char* argv[]) -> int {
  // 解析输入
  // ./execname -i src_file
  std::ifstream in_file(argv[2]);

  std::stringstream ss;
  ss << in_file.rdbuf();
  auto src = ss.str();

  // 重定向输出
  auto in_file_name = std::string(argv[2]);
  auto out_file_name = in_file_name.substr(0, in_file_name.find_last_of('.')) + ".c";
  freopen(out_file_name.c_str(), "w", stdout);

  // 词法分析
  Lexer lexer(src);
  lexer.scan();

  // 语法分析
  Parser parser(lexer.getTokens());
  auto program = parser.program();

  // 语义分析
  SemanticAnalyzer analyzer;
  program->accept(analyzer);
  auto symbol_table = analyzer.getGlobalSymbolTable();
  auto is_funtion_return = analyzer.getIsFunctionReturn();
  auto read_fmt_specifier = analyzer.getReadFmtSpecifier();
  auto write_fmt_specifier = analyzer.getWriteFmtSpecifier();
  auto is_factor_function_call = analyzer.getIsFactorFunctionCall();
  auto is_factor_with_not_number = analyzer.getIsFactorWithNotNumber();
  auto is_var_param = analyzer.getIsVarParam();
  auto params_name = analyzer.getParamsName();

  // 代码生成
  Generator generator(is_funtion_return, read_fmt_specifier, write_fmt_specifier, is_factor_function_call,
                      is_factor_with_not_number, is_var_param, params_name);
  program->accept(generator);
  std::cout << generator.getResult();
  return EXIT_SUCCESS;
}