/**
 * @file  token.hpp
 * @brief 该文件定义了标记（Token）类和标记类型枚举（Token_t）。
 */
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstddef>
#include <string>
#include <utility>

/**
 * @enum TokenType
 * @brief 该枚举表示标记的类型。
 */
enum class TokenType {
  EQUAL,       // =
  COMMA,       // ,
  QUESTION,    // ?
  LBRACE,      // {
  RBRACE,      // }
  COLON,       // :
  HAS,         // has
  PROCEDURE,   // procedure
  SAY,         // say
  LISTEN,      // listen
  TIMELIMIT,   // timelimit
  TIMEOUT,     // timeout
  DEFAULT,     // default
  LOOKUP,      // lookup
  IN,          // in
  WRITETO,     // writeto
  EXIT,        // exit
  ANYTHING,    // anything
  STRING,      // ""
  NUMBER,      // 123
  IDENTIFIER,  // any identifier
  EOF_         // end-of-file
};

/**
 * @class Token
 * @brief 该类表示一个标记（Token）。
 */
class Token {
 public:
  /// @param token_t 标记类型
  /// @param s       标记值
  /// @param lineNum 标记所在行号
  Token(const TokenType token_t, std::string s, const std::size_t lineNum)
      : type(token_t), value(std::move(s)), line(lineNum){};
  /// @param token_t 标记类型
  /// @param lineNum 标记所在行号
  Token(const TokenType token_t, const std::size_t lineNum) : type(token_t), line(lineNum){};
  /// @brief 标记类型
  TokenType type;
  /// @brief 标记值
  std::string value;
  /// @brief 标记所在行号
  std::size_t line;
  /// @brief 重载等于运算符
  auto operator==(const Token& other) const -> bool {
    return type == other.type && line == other.line && value == other.value;
  }
};
#endif  // TOKEN_HPP
