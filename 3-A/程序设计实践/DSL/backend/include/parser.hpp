/**
 * @file   parser.hpp
 * @brief  Parser 类的声明。
 */
#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

/**
 * @class Parser
 * @brief 解析器类。解析token流，生成AST。
 */
class Parser {
 public:
  /// @param tokens 一个包含所有token的vector。
  explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}
  auto parse() -> std::shared_ptr<Program>;

 private:
  /// @brief 待解析的token流。
  std::vector<Token> tokens;
  /// @brief 当前解析的token的索引。
  std::size_t current = 0;

  auto match(const TokenType&) -> bool;
  [[nodiscard]] auto check(const TokenType&) const -> bool;
  [[nodiscard]] auto isEnd() const -> bool;
  [[nodiscard]] auto getToken() const -> const Token&;
  void forward();
  void backward();
  auto constant() -> ASTNodePtr;
  auto procedure() -> ASTNodePtr;
  auto statement() -> ASTNodePtr;
  auto say() -> ASTNodePtr;
  auto listen() -> ASTNodePtr;
  auto clause() -> ASTNodePtr;
  auto lookup() -> ASTNodePtr;
  auto idOrString() -> ASTNodePtr;
  auto id() -> ASTNodePtr;
  auto consume(const TokenType&, const std::string&) -> Token;
};
#endif  // PARSER_HPP
