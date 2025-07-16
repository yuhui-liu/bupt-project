#include "lexer.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "exception.hpp"
#include "token.hpp"

TEST(LexerTest, Keywords) {
  const std::string src{
      R"(program const var procedure function begin end array of integer real boolean char if then else for to do while read write true false)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::PROGRAM, 1);
  standard.emplace_back(TokenType::CONST, 1);
  standard.emplace_back(TokenType::VAR, 1);
  standard.emplace_back(TokenType::PROCEDURE, 1);
  standard.emplace_back(TokenType::FUNCTION, 1);
  standard.emplace_back(TokenType::BEGIN, 1);
  standard.emplace_back(TokenType::END, 1);
  standard.emplace_back(TokenType::ARRAY, 1);
  standard.emplace_back(TokenType::OF, 1);
  standard.emplace_back(TokenType::INTEGER, 1);
  standard.emplace_back(TokenType::REAL, 1);
  standard.emplace_back(TokenType::BOOLEAN, 1);
  standard.emplace_back(TokenType::CHAR, 1);
  standard.emplace_back(TokenType::IF, 1);
  standard.emplace_back(TokenType::THEN, 1);
  standard.emplace_back(TokenType::ELSE, 1);
  standard.emplace_back(TokenType::FOR, 1);
  standard.emplace_back(TokenType::TO, 1);
  standard.emplace_back(TokenType::DO, 1);
  standard.emplace_back(TokenType::WHILE, 1);
  standard.emplace_back(TokenType::READ, 1);
  standard.emplace_back(TokenType::WRITE, 1);
  standard.emplace_back(TokenType::TRUE, 1);
  standard.emplace_back(TokenType::FALSE, 1);
  standard.emplace_back(TokenType::END_OF_FILE, 1);
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
  standard.emplace_back(TokenType::END_OF_FILE, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, Delimeteres) {
  const std::string src{R"(,;.:[]..)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::COMMA, 1);
  standard.emplace_back(TokenType::SEMICOLON, 1);
  standard.emplace_back(TokenType::DOT, 1);
  standard.emplace_back(TokenType::COLON, 1);
  standard.emplace_back(TokenType::LBRACKET, 1);
  standard.emplace_back(TokenType::RBRACKET, 1);
  standard.emplace_back(TokenType::DOTDOT, 1);
  standard.emplace_back(TokenType::END_OF_FILE, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, Operators) {
  const std::string src{R"(+ - * / = := div mod and or not)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::PLUS, 1);
  standard.emplace_back(TokenType::MINUS, 1);
  standard.emplace_back(TokenType::TIMES, 1);
  standard.emplace_back(TokenType::RDIV, 1);
  standard.emplace_back(TokenType::EQUAL, 1);
  standard.emplace_back(TokenType::ASSIGN, 1);
  standard.emplace_back(TokenType::DIV, 1);
  standard.emplace_back(TokenType::MOD, 1);
  standard.emplace_back(TokenType::AND, 1);
  standard.emplace_back(TokenType::OR, 1);
  standard.emplace_back(TokenType::NOT, 1);
  standard.emplace_back(TokenType::END_OF_FILE, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, RelationalOperators) {
  const std::string src{R"(< <= > >= <>)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::LT, 1);
  standard.emplace_back(TokenType::LE, 1);
  standard.emplace_back(TokenType::GT, 1);
  standard.emplace_back(TokenType::GE, 1);
  standard.emplace_back(TokenType::NE, 1);
  standard.emplace_back(TokenType::END_OF_FILE, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, Identifier) {
  const std::string src{R"(abc1 dEF g2 a_)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::IDENTIFIER, "abc1", 1);
  standard.emplace_back(TokenType::IDENTIFIER, "def", 1);  // 将会自动转换为小写
  standard.emplace_back(TokenType::IDENTIFIER, "g2", 1);
  standard.emplace_back(TokenType::IDENTIFIER, "a_", 1);
  standard.emplace_back(TokenType::END_OF_FILE, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, CharLiteral) {
  const std::string src{R"('h' 'c')"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::CHAR_LITERAL, "\'h\'", 1);
  standard.emplace_back(TokenType::CHAR_LITERAL, "\'c\'", 1);
  standard.emplace_back(TokenType::END_OF_FILE, 1);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, Comment) {
  const std::string src{R"(program test;
{ this is a comment }
begin
{ multilines
comment }
end.)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::PROGRAM, 1);
  standard.emplace_back(TokenType::IDENTIFIER, "test", 1);
  standard.emplace_back(TokenType::SEMICOLON, 1);
  standard.emplace_back(TokenType::BEGIN, 3);
  standard.emplace_back(TokenType::END, 6);
  standard.emplace_back(TokenType::DOT, 6);
  standard.emplace_back(TokenType::END_OF_FILE, 6);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, General) {
  const std::string src{R"(program test;
const a = 1;
var b: integer;
c: real;
procedure main;
begin
  b := a + 1;
  c := 1.23 div 2;
end)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::PROGRAM, 1);
  standard.emplace_back(TokenType::IDENTIFIER, "test", 1);
  standard.emplace_back(TokenType::SEMICOLON, 1);
  standard.emplace_back(TokenType::CONST, 2);
  standard.emplace_back(TokenType::IDENTIFIER, "a", 2);
  standard.emplace_back(TokenType::EQUAL, 2);
  standard.emplace_back(TokenType::NUMBER, "1", 2);
  standard.emplace_back(TokenType::SEMICOLON, 2);
  standard.emplace_back(TokenType::VAR, 3);
  standard.emplace_back(TokenType::IDENTIFIER, "b", 3);
  standard.emplace_back(TokenType::COLON, 3);
  standard.emplace_back(TokenType::INTEGER, 3);
  standard.emplace_back(TokenType::SEMICOLON, 3);
  standard.emplace_back(TokenType::IDENTIFIER, "c", 4);
  standard.emplace_back(TokenType::COLON, 4);
  standard.emplace_back(TokenType::REAL, 4);
  standard.emplace_back(TokenType::SEMICOLON, 4);
  standard.emplace_back(TokenType::PROCEDURE, 5);
  standard.emplace_back(TokenType::IDENTIFIER, "main", 5);
  standard.emplace_back(TokenType::SEMICOLON, 5);
  standard.emplace_back(TokenType::BEGIN, 6);
  standard.emplace_back(TokenType::IDENTIFIER, "b", 7);
  standard.emplace_back(TokenType::ASSIGN, 7);
  standard.emplace_back(TokenType::IDENTIFIER, "a", 7);
  standard.emplace_back(TokenType::PLUS, 7);
  standard.emplace_back(TokenType::NUMBER, "1", 7);
  standard.emplace_back(TokenType::SEMICOLON, 7);
  standard.emplace_back(TokenType::IDENTIFIER, "c", 8);
  standard.emplace_back(TokenType::ASSIGN, 8);
  standard.emplace_back(TokenType::NUMBER, "1.23", 8);
  standard.emplace_back(TokenType::DIV, 8);
  standard.emplace_back(TokenType::NUMBER, "2", 8);
  standard.emplace_back(TokenType::SEMICOLON, 8);
  standard.emplace_back(TokenType::END, 9);
  standard.emplace_back(TokenType::END_OF_FILE, 9);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, UnknownChar) {
  const std::string src{R"(procedure ^)"};
  Lexer lexer(src);
  std::string s;
  std::size_t line;
  try {
    lexer.scan();
  } catch (const std::vector<LexerException>& ex) {
    ASSERT_EQ(ex.size(), 1);
    s = ex[0].what();
    line = ex[0].getLine();
  }
  ASSERT_EQ(s, "Unknown char '^'.");
  ASSERT_EQ(line, 1);
}

TEST(LexerTest, UnclosedCharLiteral) {
  const std::string src{R"('h)"};
  Lexer lexer(src);
  std::string s;
  std::size_t line;
  try {
    lexer.scan();
  } catch (const std::vector<LexerException>& ex) {
    ASSERT_EQ(ex.size(), 1);
    s = ex[0].what();
    line = ex[0].getLine();
  }
  ASSERT_EQ(s, "Unclosed char literal 'h.");
  ASSERT_EQ(line, 1);
}

TEST(LexerTest, VeryLongIdentifier) {
  const std::string src(300, 'a');
  Lexer lexer(src);
  std::string s;
  std::size_t line;
  try {
    lexer.scan();
  } catch (const std::vector<LexerException>& ex) {
    ASSERT_EQ(ex.size(), 1);
    s = ex[0].what();
    line = ex[0].getLine();
  }
  ASSERT_EQ(s, "Identifier 'aaaaaaaaaa...' is too long.");
  ASSERT_EQ(line, 1);
}

TEST(LexerTest, MultipleExceptions) {
  const std::string long_identifier(300, 'a');
  const std::string src{R"(@^
'h
)" + long_identifier};
  Lexer lexer(src);
  std::vector<std::string> s;
  std::vector<std::size_t> line;
  try {
    lexer.scan();
  } catch (const std::vector<LexerException>& ex) {
    for (const auto& e : ex) {
      s.emplace_back(e.what());
      line.emplace_back(e.getLine());
    }
  }
  ASSERT_EQ(s.size(), 4);
  ASSERT_EQ(s[0], "Unknown char '@'.");
  ASSERT_EQ(line[0], 1);
  ASSERT_EQ(s[1], "Unknown char '^'.");
  ASSERT_EQ(line[1], 1);
  ASSERT_EQ(s[2], "Unclosed char literal 'h.");
  ASSERT_EQ(line[2], 2);
  ASSERT_EQ(s[3], "Identifier 'aaaaaaaaaa...' is too long.");
  ASSERT_EQ(line[3], 3);
  auto result = lexer.getTokens();
}

TEST(LexerTest, NormalWithException) {
  const std::string src{R"(program test;
const a = 'a)"};
  Lexer lexer(src);
  std::vector<std::string> s;
  std::vector<std::size_t> line;
  try {
    lexer.scan();
  } catch (const std::vector<LexerException>& ex) {
    for (const auto& e : ex) {
      s.emplace_back(e.what());
      line.emplace_back(e.getLine());
    }
  }
  ASSERT_EQ(s.size(), 1);
  ASSERT_EQ(s[0], "Unclosed char literal 'a.");
  ASSERT_EQ(line[0], 2);

  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::PROGRAM, 1);
  standard.emplace_back(TokenType::IDENTIFIER, "test", 1);
  standard.emplace_back(TokenType::SEMICOLON, 1);
  standard.emplace_back(TokenType::CONST, 2);
  standard.emplace_back(TokenType::IDENTIFIER, "a", 2);
  standard.emplace_back(TokenType::EQUAL, 2);
  standard.emplace_back(TokenType::END_OF_FILE, 2);
  ASSERT_EQ(result, standard);
}

TEST(LexerTest, ScientificNotation) {
  const std::string src{R"(1e2 3e-4 1.23e+2 1.23E-2)"};
  Lexer lexer(src);
  lexer.scan();
  const auto result = lexer.getTokens();
  std::vector<Token> standard;
  standard.emplace_back(TokenType::NUMBER, "1e2", 1);
  standard.emplace_back(TokenType::NUMBER, "3e-4", 1);
  standard.emplace_back(TokenType::NUMBER, "1.23e+2", 1);
  standard.emplace_back(TokenType::NUMBER, "1.23e-2", 1);
  standard.emplace_back(TokenType::END_OF_FILE, 1);
  ASSERT_EQ(result, standard);
}
