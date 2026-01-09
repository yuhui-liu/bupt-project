#include "lexer.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "exception.hpp"
#include "token.hpp"

TEST(LexerTest, Keywords) {
  const std::string src{R"(procedure lookup has listen timelimit timeout default in writeto exit say anything)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::PROCEDURE, 1);
  standard.emplace_back(TokenType::LOOKUP, 1);
  standard.emplace_back(TokenType::HAS, 1);
  standard.emplace_back(TokenType::LISTEN, 1);
  standard.emplace_back(TokenType::TIMELIMIT, 1);
  standard.emplace_back(TokenType::TIMEOUT, 1);
  standard.emplace_back(TokenType::DEFAULT, 1);
  standard.emplace_back(TokenType::IN, 1);
  standard.emplace_back(TokenType::WRITETO, 1);
  standard.emplace_back(TokenType::EXIT, 1);
  standard.emplace_back(TokenType::SAY, 1);
  standard.emplace_back(TokenType::ANYTHING, 1);
  standard.emplace_back(TokenType::EOF_, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, String) {
  const std::string src{R"("hello" "good" "bingo" "very looooooooooong")"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::STRING, "hello", 1);
  standard.emplace_back(TokenType::STRING, "good", 1);
  standard.emplace_back(TokenType::STRING, "bingo", 1);
  standard.emplace_back(TokenType::STRING, "very looooooooooong", 1);
  standard.emplace_back(TokenType::EOF_, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, Number) {
  const std::string src{R"(123 1.23 369 0.1)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::NUMBER, "123", 1);
  standard.emplace_back(TokenType::NUMBER, "1.23", 1);
  standard.emplace_back(TokenType::NUMBER, "369", 1);
  standard.emplace_back(TokenType::NUMBER, "0.1", 1);
  standard.emplace_back(TokenType::EOF_, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, Operator) {
  const std::string src{R"(= , ? { } :)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::EQUAL, 1);
  standard.emplace_back(TokenType::COMMA, 1);
  standard.emplace_back(TokenType::QUESTION, 1);
  standard.emplace_back(TokenType::LBRACE, 1);
  standard.emplace_back(TokenType::RBRACE, 1);
  standard.emplace_back(TokenType::COLON, 1);
  standard.emplace_back(TokenType::EOF_, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, Identifier) {
  const std::string src{R"(abc dEF g)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::IDENTIFIER, "abc", 1);
  standard.emplace_back(TokenType::IDENTIFIER, "dEF", 1);
  standard.emplace_back(TokenType::IDENTIFIER, "g", 1);
  standard.emplace_back(TokenType::EOF_, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, General) {
  const std::string src{R"(procedure main { # 测试注释
  say "您好，欢迎致电中国移动"
  listen timelimit 10 {
    has "话费" ? checkBalance
    timeout ? silenceProc
    default ? defaultProc
  }
  goodbye
})"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::PROCEDURE, 1);
  standard.emplace_back(TokenType::IDENTIFIER, "main", 1);
  standard.emplace_back(TokenType::LBRACE, 1);

  standard.emplace_back(TokenType::SAY, 2);
  standard.emplace_back(TokenType::STRING, "您好，欢迎致电中国移动", 2);

  standard.emplace_back(TokenType::LISTEN, 3);
  standard.emplace_back(TokenType::TIMELIMIT, 3);
  standard.emplace_back(TokenType::NUMBER, "10", 3);
  standard.emplace_back(TokenType::LBRACE, 3);

  standard.emplace_back(TokenType::HAS, 4);
  standard.emplace_back(TokenType::STRING, "话费", 4);
  standard.emplace_back(TokenType::QUESTION, 4);
  standard.emplace_back(TokenType::IDENTIFIER, "checkBalance", 4);

  standard.emplace_back(TokenType::TIMEOUT, 5);
  standard.emplace_back(TokenType::QUESTION, 5);
  standard.emplace_back(TokenType::IDENTIFIER, "silenceProc", 5);

  standard.emplace_back(TokenType::DEFAULT, 6);
  standard.emplace_back(TokenType::QUESTION, 6);
  standard.emplace_back(TokenType::IDENTIFIER, "defaultProc", 6);

  standard.emplace_back(TokenType::RBRACE, 7);
  standard.emplace_back(TokenType::IDENTIFIER, "goodbye", 8);
  standard.emplace_back(TokenType::RBRACE, 9);
  standard.emplace_back(TokenType::EOF_, 9);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, ErrorHandling1) {
  const std::string src{R"("hello
procedure)"};
  Lexer lexer(src);
  std::string s;
  std::size_t line;
  try {
    lexer.scan();
  } catch (const LexerException& ex) {
    s = ex.what();
    line = ex.getLine();
  }
  ASSERT_EQ(s, "Unclosed string.");
  ASSERT_EQ(line, 1);
}

TEST(LexerTest, ErrorHandling2) {
  const std::string src{R"(procedure ^)"};
  Lexer lexer(src);
  std::string s;
  std::size_t line;
  try {
    lexer.scan();
  } catch (const LexerException& ex) {
    s = ex.what();
    line = ex.getLine();
  }
  ASSERT_EQ(s, "Unknown char.");
  ASSERT_EQ(line, 1);
}
