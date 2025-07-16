/**
 * @file  lexer.hpp
 * @brief Lexer 类的声明。
 */
#ifndef LEXER_HPP
#define LEXER_HPP

#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "token.hpp"

/**
 * @class Lexer
 * @brief 词法分析器类。将字节流转换为 Token 序列。
 */
class Lexer {
 public:
  /// @brief 构造一个新的 Lexer 对象。
  explicit Lexer(std::string);

  void scan();

  void printTokens();

  [[nodiscard]] auto getTokens() const -> std::vector<Token>;

 private:
  /// @brief 源代码字符串。
  std::string src;
  /// @brief 原始源代码字符串
  std::string raw_src;
  /// @brief Token 序列。
  std::vector<Token> tokens;
  /// @brief 当前扫描起始位置。
  std::size_t start = 0;
  /// @brief 当前扫描位置。
  std::size_t current = 0;
  /// @brief 当前行号。
  std::size_t line_num = 1;
  /// @brief 关键字表。
  inline static const std::map<std::string_view, TokenType> KEYWORDS{{"program", TokenType::PROGRAM},
                                                                     {"const", TokenType::CONST},
                                                                     {"var", TokenType::VAR},
                                                                     {"procedure", TokenType::PROCEDURE},
                                                                     {"function", TokenType::FUNCTION},
                                                                     {"begin", TokenType::BEGIN},
                                                                     {"end", TokenType::END},
                                                                     {"array", TokenType::ARRAY},
                                                                     {"of", TokenType::OF},
                                                                     {"integer", TokenType::INTEGER},
                                                                     {"real", TokenType::REAL},
                                                                     {"boolean", TokenType::BOOLEAN},
                                                                     {"char", TokenType::CHAR},
                                                                     {"if", TokenType::IF},
                                                                     {"then", TokenType::THEN},
                                                                     {"else", TokenType::ELSE},
                                                                     {"for", TokenType::FOR},
                                                                     {"to", TokenType::TO},
                                                                     {"do", TokenType::DO},
                                                                     {"while", TokenType::WHILE},
                                                                     {"read", TokenType::READ},
                                                                     {"write", TokenType::WRITE},
                                                                     {"true", TokenType::TRUE},
                                                                     {"false", TokenType::FALSE},
                                                                     {"break", TokenType::BREAK}};

  static auto stringToLower(std::string) -> std::string;

  void scanNextToken();

  auto getNextChar() -> char;

  void backward();

  void addToken(const TokenType &);

  void addToken(const TokenType &, const std::string &);

  void processNumber();

  void processKeywordsAndIdentifiersAndAlphaOps();

  void processString();

  bool processingString = false;

  [[nodiscard]] auto isEnd() const -> bool;
};
#endif  // LEXER_HPP
