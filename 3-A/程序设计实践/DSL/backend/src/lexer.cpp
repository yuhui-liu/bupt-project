/**
 * @file   lexer.cpp
 * @brief  Lexer 类的实现。
 */

#include "lexer.hpp"

#include <cctype>
#include <iostream>
#include <utility>
#include <vector>

#include "exception.hpp"
#include "token.hpp"

Lexer::Lexer(std::string src_) : src(std::move(src_)) {}

/**
 * @brief 扫描输入源代码并生成标记。
 *
 * 该函数从当前位置开始迭代输入源代码，并扫描下一个标记，直到到达输入的末尾。
 * 一旦所有标记都被扫描完毕，将添加一个EOF（文件结束）标记以表示输入的结束。
 */
void Lexer::scan() {
  while (!isEnd()) {
    start = current;
    scanNextToken();
  }
  addToken(TokenType::EOF_);
}

/**
 * @brief 扫描下一个标记（Token）。
 *
 * 该函数通过读取下一个字符来确定并生成相应的标记（Token）。
 * 根据读取到的字符，执行不同的处理逻辑：
 * - 空格、制表符和回车符：跳过，不做处理。
 * - 换行符：增加行号计数。
 * - 特定符号（如 '=', ',', '?', '{', '}', ':'）：添加相应的标记。
 * - 双引号：处理字符串。
 * - 井号：跳过注释内容，直到行尾。
 * - 数字：处理数字。
 * - 字母：处理关键字和标识符。
 * - 其他字符：抛出未知字符异常。
 *
 * @throws LexerException 当遇到未知字符时抛出异常。
 */
void Lexer::scanNextToken() {
  switch (auto c = getNextChar()) {
    case ' ':
    case '\t':
    case '\r':
      break;
    case '\n':
      line_num++;
      break;
    case '=':
      addToken(TokenType::EQUAL);
      break;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case '?':
      addToken(TokenType::QUESTION);
      break;
    case '{':
      addToken(TokenType::LBRACE);
      break;
    case '}':
      addToken(TokenType::RBRACE);
      break;
    case ':':
      addToken(TokenType::COLON);
      break;
    case '"':
      processString();
      break;
    case '#':
      while ((c = getNextChar()) != '\n' && !isEnd()) {
        ;
      }
      if (c == '\n') {
        line_num++;
      }
      break;
    default:
      if (std::isdigit(c) != 0) {
        processNumber();
      } else if (std::isalpha(c) != 0) {
        processKeywordsAndIdentifiers();
      } else {
        throw LexerException("Unknown char.", line_num);
      }
      break;
  }
}

/**
 * @brief 获取下一个字符
 *
 * 该函数从源代码字符串中获取当前位置的字符，并将当前位置向前移动一位。
 *
 * @return char 返回当前位置的字符
 */
auto Lexer::getNextChar() -> char { return src[current++]; }

void Lexer::backward() { current--; }

void Lexer::addToken(const TokenType& tok_t) { tokens.emplace_back(tok_t, line_num); }

void Lexer::addToken(const TokenType& tok_t, const std::string& s) { tokens.emplace_back(tok_t, s, line_num); }

void Lexer::processString() {
  char c;
  if (isEnd()) {
    throw LexerException("Unclosed string.", line_num);
  }
  while ((c = getNextChar()) != '"') {
    if (c == '\n' || isEnd()) {
      throw LexerException("Unclosed string.", line_num);
    }
  }
  const auto& raw = src.substr(start, current - start);
  addToken(TokenType::STRING, raw.substr(1, raw.length() - 2));
}

/**
 * @brief 处理数字字面量的词法分析函数。
 *
 * 该函数用于处理源代码中的数字字面量，包括整数和浮点数。它会读取字符直到数字结束，
 * 并将其作为一个数字类型的Token添加到Token流中。
 *
 * 具体步骤如下：
 * 1. 读取连续的数字字符，直到遇到非数字字符或到达源代码末尾。
 * 2. 如果遇到小数点，继续读取后续的数字字符，处理浮点数。
 * 3. 将读取到的数字字面量作为Token添加到Token流中。
 *
 * @note 该函数假设源代码中的数字字面量格式正确，不进行格式校验。
 */
void Lexer::processNumber() {
  while ((std::isdigit(getNextChar()) != 0) && !isEnd()) {
    ;
  }
  if (!isEnd()) {
    backward();
  }
  if (!isEnd()) {
    if (getNextChar() != '.' || isEnd()) {
      backward();
    } else if (std::isdigit(getNextChar()) == 0) {
      backward();
      backward();
    } else {
      while ((std::isdigit(getNextChar()) != 0) && !isEnd()) {
        ;
      }
      if (!isEnd()) {
        backward();
      }
    }
  }
  addToken(TokenType::NUMBER, src.substr(start, current - start));
}

/**
 * @brief 处理关键字和标识符的词法分析函数。
 *
 * 该函数用于处理源代码中的关键字和标识符。它会读取字符直到标识符结束，
 * 并将其作为一个关键字或标识符类型的Token添加到Token流中。
 *
 * 具体步骤如下：
 * 1. 读取连续的字母字符，直到遇到非字母字符或到达源代码末尾。
 * 2. 将读取到的字符串作为关键字或标识符类型的Token添加到Token流中。
 *
 * @note 该函数假设源代码中的关键字和标识符格式正确，不进行格式校验。
 */
void Lexer::processKeywordsAndIdentifiers() {
  char c;
  while ((std::isalpha(c = getNextChar()) != 0) && !isEnd()) {
    ;
  }
  if (std::isalpha(c) == 0) {
    backward();
  }
  const auto word = src.substr(start, current - start);
  if (const auto it = KEYWORDS.find(word); it != KEYWORDS.end()) {
    addToken(it->second);
  } else {
    addToken(TokenType::IDENTIFIER, word);
  }
}

/// @brief 判断是否到达源代码末尾
[[nodiscard]] auto Lexer::isEnd() const -> bool { return current >= src.length(); }

/**
 * @brief 打印 Token 流。
 *
 * 该函数用于打印 Token 流中的所有 Token。
 * 每个 Token 的格式为：行号 类型 值。
 */
void Lexer::printTokens() {
  static const auto tokenTypeString = [] {
    using enum TokenType;
    std::map<TokenType, std::string_view> result;
#define INSERT(e) result.emplace(e, #e)
    INSERT(EQUAL);
    INSERT(COMMA);
    INSERT(QUESTION);
    INSERT(LBRACE);
    INSERT(RBRACE);
    INSERT(COLON);
    INSERT(HAS);
    INSERT(PROCEDURE);
    INSERT(SAY);
    INSERT(LISTEN);
    INSERT(TIMELIMIT);
    INSERT(TIMEOUT);
    INSERT(DEFAULT);
    INSERT(LOOKUP);
    INSERT(IN);
    INSERT(WRITETO);
    INSERT(EXIT);
    INSERT(ANYTHING);
    INSERT(STRING);
    INSERT(NUMBER);
    INSERT(IDENTIFIER);
    INSERT(EOF_);
#undef INSERT
    return result;
  }();
  for (const auto& token : tokens) {
    std::cout << token.line << ' ' << tokenTypeString.at(token.type) << ' ' << token.value << std::endl;
  }
}

/// @brief 获取 Token 流
[[nodiscard]] auto Lexer::getTokens() const -> std::vector<Token> { return tokens; }
