/**
 * @file   parser.hpp
 * @brief  Parser 类的声明。
 */
#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstddef>
#include <string>
#include <vector>

#ifdef ENABLE_GTEST_FRIENDS
#include <gtest/gtest.h>
#endif

#include "ast.hpp"
#include "token.hpp"

/**
 * @class Parser
 * @brief 语法分析器类。解析token流，生成AST。
 */
class Parser {
 public:
  /// @param tokens 一个包含所有token的vector。
  explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}
  auto program() -> ProgramPtr;

 private:
  /// @brief 待解析的token流。
  std::vector<Token> tokens;
  /// @brief 当前解析的token的索引。
  std::size_t current = 0;

#ifdef ENABLE_GTEST_FRIENDS
  FRIEND_TEST(ParserTest, SubprogramTest_Procedure);
  FRIEND_TEST(ParserTest, SubprogramTest_Function);
  FRIEND_TEST(ParserTest, ParameterTest);
  FRIEND_TEST(ParserTest, ConstDeclarationTest);
  FRIEND_TEST(ParserTest, VarDeclarationTest);
  FRIEND_TEST(ParserTest, TypeTest);
  FRIEND_TEST(ParserTest, TypeTest_Array);
  FRIEND_TEST(ParserTest, VariableTest);
  FRIEND_TEST(ParserTest, VariableTest_Array);
  FRIEND_TEST(ParserTest, AssignTest);
  FRIEND_TEST(ParserTest, ProcedureCallTest);
  FRIEND_TEST(ParserTest, CompoundStatementTest);
  FRIEND_TEST(ParserTest, IfStatementTest);
  FRIEND_TEST(ParserTest, IfStatementTest_WithElse);
  FRIEND_TEST(ParserTest, ForStatementTest);
  FRIEND_TEST(ParserTest, WhileStatementTest);
  FRIEND_TEST(ParserTest, ReadStatementTest);
  FRIEND_TEST(ParserTest, WriteStatementTest);
  FRIEND_TEST(ParserTest, BreakStatementTest);
  FRIEND_TEST(ParserTest, FactorTest);
  FRIEND_TEST(ParserTest, FactorTest_Parentheses);
  FRIEND_TEST(ParserTest, FactorTest_Number);
  FRIEND_TEST(ParserTest, FactorTest_Boolean);
  FRIEND_TEST(ParserTest, FactorTest_Function);
  FRIEND_TEST(ParserTest, FactorTest_WithNot);
  FRIEND_TEST(ParserTest, FactorTest_WithUminus);
  FRIEND_TEST(ParserTest, TermTest);
  FRIEND_TEST(ParserTest, SimpleExpressionTest);
  FRIEND_TEST(ParserTest, ExpressionTest);
  FRIEND_TEST(ParserTest, MatchTest);
  FRIEND_TEST(ParserTest, CheckTest);
  FRIEND_TEST(ParserTest, IsEndTest);
  FRIEND_TEST(ParserTest, GetTokenTest);
  FRIEND_TEST(ParserTest, ForwardTest);
  FRIEND_TEST(ParserTest, BackwardTest);
  FRIEND_TEST(ParserTest, ConsumeTest);
  FRIEND_TEST(ParserTest, ConsumeTest_Fail);
  FRIEND_TEST(ParserTestSupplement, Factor);
  FRIEND_TEST(ParserTestSupplement, Term);
  FRIEND_TEST(ParserTestSupplement, SimpleExpression);
  FRIEND_TEST(ParserTestSupplement, Expression);
  FRIEND_TEST(ParserTestSupplement, ExpressionComplex);
#endif

  auto subprogram() -> SubprogramPtr;
  auto parameter() -> ParameterPtr;
  auto constDeclaration() -> ConstDeclPtr;
  auto varDeclaration() -> VarDeclPtr;
  auto type() -> TypePtr;
  auto variable() -> VariablePtr;
  auto statement() -> StatementPtr;
  auto assign() -> AssignPtr;
  auto procedureCall() -> ProcedureCallPtr;
  auto compoundStatement() -> CompoundStatementPtr;
  auto ifStatement() -> IfPtr;
  auto forStatement() -> ForPtr;
  auto whileStatement() -> WhilePtr;
  auto readStatement() -> ReadPtr;
  auto writeStatement() -> WritePtr;
  auto breakStatement() -> BreakPtr;
  auto factor() -> FactorPtr;
  auto term() -> TermPtr;
  auto simpleExpression() -> SimpleExpressionPtr;
  auto expression() -> ExpressionPtr;

  auto match(const TokenType&) -> bool;
  [[nodiscard]] auto check(const TokenType&) const -> bool;
  [[nodiscard]] auto isEnd() const -> bool;
  [[nodiscard]] auto getToken() const -> const Token&;
  void forward();
  void backward();
  auto consume(const TokenType&, const std::string&) -> Token;
};
#endif  // PARSER_HPP
