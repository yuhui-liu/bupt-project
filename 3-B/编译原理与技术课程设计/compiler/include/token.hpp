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
  // keywords
  PROGRAM,    // program
  CONST,      // const
  VAR,        // var
  PROCEDURE,  // procedure
  FUNCTION,   // function
  BEGIN,      // begin
  END,        // end
  ARRAY,      // array
  OF,         // of
  INTEGER,    // integer
  REAL,       // real
  BOOLEAN,    // boolean
  CHAR,       // char
  IF,         // if
  THEN,       // then
  ELSE,       // else
  FOR,        // for
  TO,         // to
  DO,         // do
  WHILE,      // while
  READ,       // read
  WRITE,      // write
  TRUE,       // true
  FALSE,      // false
  BREAK,      // break
  // delimiters and punctuations
  LPAREN,     // (
  RPAREN,     // )
  COMMA,      // ,
  SEMICOLON,  // ;
  DOT,        // .
  COLON,      // :
  LBRACKET,   // [
  RBRACKET,   // ]
  DOTDOT,     // ..
  // algebraic and logical operators
  PLUS,    // +
  MINUS,   // -
  TIMES,   // *
  RDIV,    // /
  ASSIGN,  // :=
  DIV,     // div
  MOD,     // mod
  AND,     // and
  OR,      // or
  NOT,     // not
  // relational operators
  EQUAL,  // =
  LT,     // <
  LE,     // <=
  GT,     // >
  GE,     // >=
  NE,     // <>
  // identifiers and literals
  IDENTIFIER,      // 标识符
  NUMBER,          // 数字
  CHAR_LITERAL,    // 字符字面量
  STRING_LITERAL,  // 字符串字面量
  // end of file
  END_OF_FILE,
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
      : type(token_t), value(std::move(s)), line(lineNum) {};
  /// @param token_t 标记类型
  /// @param lineNum 标记所在行号
  Token(const TokenType token_t, const std::size_t lineNum) : type(token_t), line(lineNum) {};
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
