/**
 * @file   lexer.cpp
 * @brief  Lexer 类的实现。
 */

#include "lexer.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>
#include <vector>

#include "exception.hpp"
#include "token.hpp"

Lexer::Lexer(std::string src_) : raw_src(src_), src(stringToLower(src_)) {}

/**
 * @brief 扫描输入源代码并生成标记。
 *
 * 该函数从当前位置开始迭代输入源代码，并扫描下一个标记，直到到达输入的末尾。
 * 一旦所有标记都被扫描完毕，将添加一个EOF（文件结束）标记以表示输入的结束。
 */
void Lexer::scan() {
  std::vector<LexerException> errors;
  while (!isEnd()) {
    start = current;
    try {
      scanNextToken();
    } catch (const LexerException& e) {
      errors.emplace_back(e);
    }
  }
  addToken(TokenType::END_OF_FILE);
  if (!errors.empty()) {
    throw errors;
  }
}

/**
 * @brief 扫描下一个标记（Token）。
 *
 * 该函数通过读取下一个字符来确定并生成相应的标记（Token）。
 * 根据读取到的字符，执行不同的处理逻辑：
 * - 空格、制表符和回车符：跳过，不做处理。
 * - 换行符：增加行号计数。
 * - 特定符号（如 '=', ',', '(', ')', ':'）：添加相应的标记。
 * - 左花括号 ：跳过注释内容，直到右花括号。
 * - 数字：处理数字。
 * - 字母：处理关键字和标识符以及div等运算符。
 * - 单引号：处理字符。
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
    case '(':
      addToken(TokenType::LPAREN);
      break;
    case ')':
      addToken(TokenType::RPAREN);
      break;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case ';':
      addToken(TokenType::SEMICOLON);
      break;
    case '.':
      if (isEnd()) {
        addToken(TokenType::DOT);
      } else {
        if (getNextChar() == '.') {
          addToken(TokenType::DOTDOT);
        } else {
          backward();
          addToken(TokenType::DOT);
        }
      }
      break;
    case ':':
      if (!isEnd() && getNextChar() == '=') {
        addToken(TokenType::ASSIGN);
      } else {
        backward();
        addToken(TokenType::COLON);
      }
      break;
    case '\'':
      if (isEnd()) {
        throw LexerException("Unclosed char literal '.", line_num);
      }
      if ((c = getNextChar()) == '\n') {
        throw LexerException("Unclosed char literal '" + src.substr(start, current - start - 1) + ".", line_num++);
      }
      if (c == '\'') {
        throw LexerException("There should be a char between a pair of \"'\".", line_num);
      }
      if (isEnd()) {
        throw LexerException("Unclosed char literal " + src.substr(start, current - start) + ".", line_num);
      }
      if ((c = getNextChar()) != '\'') {
        if (c == '\n') {
          throw LexerException("Unclosed char literal " + src.substr(start, current - start - 1) + ".", line_num++);
        }
        backward();
        backward();
        processString();
      }
      if (!processingString) {
        addToken(TokenType::CHAR_LITERAL, raw_src.substr(start, current - start));
      }
      processingString = false;
      break;
    case '[':
      addToken(TokenType::LBRACKET);
      break;
    case ']':
      addToken(TokenType::RBRACKET);
      break;
    case '+':
      addToken(TokenType::PLUS);
      break;
    case '-':
      addToken(TokenType::MINUS);
      break;
    case '*':
      addToken(TokenType::TIMES);
      break;
    case '/':
      if (!isEnd() && getNextChar() == '/') {
        while (!isEnd() && (c = getNextChar()) != '\n') {
          ;
        }
        if (c == '\n') {
          line_num++;
        }
        break;
      } else {
        backward();
      }
      addToken(TokenType::RDIV);
      break;
    case '=':
      addToken(TokenType::EQUAL);
      break;
    case '<':
      if (!isEnd() && (c = getNextChar()) == '=') {
        addToken(TokenType::LE);
      } else if (c == '>') {
        addToken(TokenType::NE);
      } else {
        backward();
        addToken(TokenType::LT);
      }
      break;
    case '>':
      if (!isEnd() && getNextChar() == '=') {
        addToken(TokenType::GE);
      } else {
        backward();
        addToken(TokenType::GT);
      }
      break;
    case '{':
      while (!isEnd() && (c = getNextChar()) != '}') {
        if (c == '\n') {
          line_num++;
        }
      }
      break;
    default:
      if (std::isdigit(c) != 0) {
        processNumber();
      } else if (std::isalpha(c) != 0) {
        processKeywordsAndIdentifiersAndAlphaOps();
      } else {
        throw LexerException("Unknown char '" + std::string(1, c) + "'.", line_num);
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

/**
 * @brief 处理数字字面量的词法分析函数。
 *
 * 该函数用于处理源代码中的数字字面量，包括整数和浮点数。它会读取字符直到数字结束，
 * 并将其作为一个数字类型的Token添加到Token流中。
 * 同时，定义了一个 lambda 函数用于处理科学计数法的数字。
 *
 * 具体步骤如下：
 * 1. 读取连续的数字字符，直到遇到非数字字符或到达源代码末尾。
 * 2. 如果遇到小数点，继续读取后续的数字字符，处理浮点数。
 * 3. 将读取到的数字字面量作为Token添加到Token流中。
 * 4. 在整数部分或小数部分后，检查是否有科学计数法的符号（'e' 或 'E'），并进一步处理。
 *
 * @note 该函数假设源代码中的数字字面量格式正确，不进行格式校验。
 */
void Lexer::processNumber() {
  auto processScientific = [&]() {
    if (isEnd()) {
      return;
    }
    auto c = getNextChar();
    if (c == 'e') {
      if (isEnd()) {
        backward();
      } else if ((c = getNextChar()) == '+' || c == '-') {
        if (isEnd()) {
          backward();
          backward();
        } else {
          if (std::isdigit(getNextChar()) == 0) {
            backward();
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
      } else if (std::isdigit(c) != 0) {
        while ((std::isdigit(getNextChar()) != 0) && !isEnd()) {
          ;
        }
        if (!isEnd()) {
          backward();
        }
      } else {
        backward();
        backward();
      }
    } else {
      backward();
    }
  };
  while ((std::isdigit(getNextChar()) != 0) && !isEnd()) {
    ;
  }
  if (!isEnd()) {
    backward();
  }
  if (!isEnd()) {
    if (getNextChar() != '.' || isEnd()) {
      backward();
      processScientific();
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
      processScientific();
    }
  }
  addToken(TokenType::NUMBER, src.substr(start, current - start));
}

/**
 * @brief 处理关键字和标识符以及字母运算符的词法分析函数。
 *
 * 该函数用于处理源代码中的关键字和标识符以及字母组成的运算符。
 * 它会读取字符直到标识符结束，
 * 并将其作为一个关键字或标识符类型的Token添加到Token流中。
 *
 * 具体步骤如下：
 * 1. 读取连续的字母、数字或下划线，直到遇到其他类型字符或到达源代码末尾。
 * 2. 将读取到的字符串作为关键字或标识符或运算符类型的Token添加到Token流中。
 *
 * @throw LexerException 当标识符长度超过256时抛出异常。
 */
void Lexer::processKeywordsAndIdentifiersAndAlphaOps() {
  char c;
  unsigned int count = 1;
  while ((std::isalnum(c = getNextChar()) != 0 || c == '_') && !isEnd()) {
    count++;
  }
  if (std::isalnum(c) == 0 && c != '_') {
    backward();
  }
  if (count > 256) {
    throw LexerException("Identifier '" + src.substr(start, 10) + "...' is too long.", line_num);
  }
  const auto word = src.substr(start, current - start);
  if (word == "div") {
    addToken(TokenType::DIV);
  } else if (word == "mod") {
    addToken(TokenType::MOD);
  } else if (word == "and") {
    addToken(TokenType::AND);
  } else if (word == "or") {
    addToken(TokenType::OR);
  } else if (word == "not") {
    addToken(TokenType::NOT);
  } else if (const auto it = KEYWORDS.find(word); it != KEYWORDS.end()) {
    addToken(it->second);
  } else {
    addToken(TokenType::IDENTIFIER, word);
  }
}

/**
 * @brief 处理字符串字面量的词法分析函数。
 *
 * 该函数用于处理源代码中的字符串字面量。它会读取字符直到字符串结束，
 * 并将其作为一个字符串类型的Token添加到Token流中。
 *
 * 字符串不能跨行，且必须以单引号（'）开头和结尾。
 * @throw LexerException 当字符串未闭合时抛出异常。
 */
void Lexer::processString() {
  processingString = true;
  char c;
  if (isEnd()) {
    throw LexerException("Unclosed string '.", line_num);
  }
  while ((c = getNextChar()) != '\'') {
    if (c == '\n' || isEnd()) {
      if (c == '\n') {
        throw LexerException("Unclosed string " + src.substr(start, current - start - 1) + '.', line_num++);
      } else {
        throw LexerException("Unclosed string " + src.substr(start, current - start) + '.', line_num);
      }
    }
  }
  const auto& raw = src.substr(start, current - start);
  addToken(TokenType::STRING_LITERAL, raw.substr(1, raw.length() - 2));
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
    INSERT(PROGRAM);
    INSERT(CONST);
    INSERT(VAR);
    INSERT(PROCEDURE);
    INSERT(FUNCTION);
    INSERT(BEGIN);
    INSERT(END);
    INSERT(ARRAY);
    INSERT(OF);
    INSERT(INTEGER);
    INSERT(REAL);
    INSERT(BOOLEAN);
    INSERT(CHAR);
    INSERT(IF);
    INSERT(THEN);
    INSERT(ELSE);
    INSERT(FOR);
    INSERT(TO);
    INSERT(DO);
    INSERT(WHILE);
    INSERT(READ);
    INSERT(WRITE);
    INSERT(TRUE);
    INSERT(FALSE);
    INSERT(BREAK);
    INSERT(LPAREN);
    INSERT(RPAREN);
    INSERT(COMMA);
    INSERT(SEMICOLON);
    INSERT(DOT);
    INSERT(COLON);
    INSERT(LBRACKET);
    INSERT(RBRACKET);
    INSERT(DOTDOT);
    INSERT(PLUS);
    INSERT(MINUS);
    INSERT(TIMES);
    INSERT(RDIV);
    INSERT(ASSIGN);
    INSERT(DIV);
    INSERT(MOD);
    INSERT(AND);
    INSERT(OR);
    INSERT(NOT);
    INSERT(EQUAL);
    INSERT(LT);
    INSERT(LE);
    INSERT(GT);
    INSERT(GE);
    INSERT(NE);
    INSERT(IDENTIFIER);
    INSERT(NUMBER);
    INSERT(CHAR_LITERAL);
    INSERT(STRING_LITERAL);
    INSERT(END_OF_FILE);
#undef INSERT
    return result;
  }();
  for (const auto& token : tokens) {
    std::cout << token.line << ' ' << tokenTypeString.at(token.type) << ' ' << token.value << std::endl;
  }
}

/// @brief 获取 Token 流
[[nodiscard]] auto Lexer::getTokens() const -> std::vector<Token> { return tokens; }

/// @brief 将字符串转换为小写
auto Lexer::stringToLower(std::string s) -> std::string {
  std::ranges::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return s;
}
