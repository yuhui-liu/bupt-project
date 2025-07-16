#include "parser.hpp"

#include <gtest/gtest.h>

#include "exception.hpp"
#include "parser_tester.hpp"
#include "token.hpp"

TEST(ParserTest, ABareProgram) {
  /*
  program hello;
  begin
  end.
  */
  std::vector<Token> tokens{Token{TokenType::PROGRAM, 1},    Token{TokenType::IDENTIFIER, "hello", 1},
                            Token{TokenType::SEMICOLON, 1},  Token{TokenType::BEGIN, 2},
                            Token{TokenType::END, 3},        Token{TokenType::DOT, 3},
                            Token{TokenType::END_OF_FILE, 3}};
  Parser parser(tokens);
  auto program = parser.program();
  ParserTester tester;
  program->accept(tester);
  auto standard =
      std::vector<std::string>{"Program",     "hello", "Parameters",         "Const Declarations", "Var Declarations",
                               "Subprograms", "Body",  "Compound Statement", "Null Statement"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, SubprogramTest_Procedure) {
  /*
  procedure a();
  begin
  end
  */
  std::vector<Token> tokens{
      Token{TokenType::PROCEDURE, 1}, Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::LPAREN, 1},
      Token{TokenType::RPAREN, 1},    Token{TokenType::SEMICOLON, 1},       Token{TokenType::BEGIN, 2},
      Token{TokenType::END, 3},
  };
  Parser parser(tokens);
  auto subprogram = parser.subprogram();
  ParserTester tester;
  subprogram->accept(tester);
  auto standard = std::vector<std::string>{
      "Subprogram",       "a",    "Parameters",         "Return Type",    "", "Procedure", "Const Declarations",
      "Var Declarations", "Body", "Compound Statement", "Null Statement",
  };
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, SubprogramTest_Function) {
  /*
  function a: integer;
  begin
  end
  */
  std::vector<Token> tokens{
      Token{TokenType::FUNCTION, 1}, Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::COLON, 1},
      Token{TokenType::INTEGER, 1},  Token{TokenType::SEMICOLON, 1},       Token{TokenType::BEGIN, 2},
      Token{TokenType::END, 3},
  };
  Parser parser(tokens);
  auto subprogram = parser.subprogram();
  ParserTester tester;
  subprogram->accept(tester);
  auto standard = std::vector<std::string>{
      "Subprogram",       "a",    "Parameters",         "Return Type",    "int", "Function", "Const Declarations",
      "Var Declarations", "Body", "Compound Statement", "Null Statement",
  };
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, ParameterTest) {
  /*
  var a, b: integer
  */
  std::vector<Token> tokens{Token{TokenType::VAR, 1},   Token{TokenType::IDENTIFIER, "a", 1},
                            Token{TokenType::COMMA, 1}, Token{TokenType::IDENTIFIER, "b", 1},
                            Token{TokenType::COLON, 1}, Token{TokenType::INTEGER, 1}};
  Parser parser(tokens);
  auto parameter = parser.parameter();
  ParserTester tester;
  parameter->accept(tester);
  auto standard = std::vector<std::string>{"Parameter", "var", "int", "a", "b"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, ConstDeclarationTest) {
  /*
  a = 1;
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::EQUAL, 1},
                            Token{TokenType::NUMBER, "1", 1}, Token{TokenType::SEMICOLON, 1}};
  Parser parser(tokens);
  auto const_decl = parser.constDeclaration();
  ParserTester tester;
  const_decl->accept(tester);
  auto standard = std::vector<std::string>{"Const Declaration", "a", "1"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, VarDeclarationTest) {
  /*
  a: integer
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::COLON, 1},
                            Token{TokenType::INTEGER, 1}};
  Parser parser(tokens);
  auto var_decl = parser.varDeclaration();
  ParserTester tester;
  var_decl->accept(tester);
  auto standard = std::vector<std::string>{"Var Declaration", "a", "Type", "int"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, TypeTest) {
  /*
  integer
  */
  std::vector<Token> tokens{Token{TokenType::INTEGER, 1}};
  Parser parser(tokens);
  auto type = parser.type();
  ParserTester tester;
  type->accept(tester);
  auto standard = std::vector<std::string>{"Type", "int"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, TypeTest_Array) {
  /*
  array[1..10] of integer
  */
  std::vector<Token> tokens{Token{TokenType::ARRAY, 1},        Token{TokenType::LBRACKET, 1},
                            Token{TokenType::NUMBER, "1", 1},  Token{TokenType::DOTDOT, 1},
                            Token{TokenType::NUMBER, "10", 1}, Token{TokenType::RBRACKET, 1},
                            Token{TokenType::OF, 1},           Token{TokenType::INTEGER, 1}};
  Parser parser(tokens);
  auto type = parser.type();
  ParserTester tester;
  type->accept(tester);
  auto standard = std::vector<std::string>{"Type", "int", "1..10"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, VariableTest) {
  // a
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}};
  Parser parser(tokens);
  auto variable = parser.variable();
  ParserTester tester;
  variable->accept(tester);
  auto standard = std::vector<std::string>{"Variable", "a"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, VariableTest_Array) {
  // a[1, 2]
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::LBRACKET, 1},
                            Token{TokenType::NUMBER, "1", 1},     Token{TokenType::COMMA, 1},
                            Token{TokenType::NUMBER, "2", 1},     Token{TokenType::RBRACKET, 1}};
  Parser parser(tokens);
  auto variable = parser.variable();
  ParserTester tester;
  variable->accept(tester);
  auto standard = std::vector<std::string>{"Variable", "a", "Expression", "Simple Expression", "Term", "Factor",
                                           "Number",   "1", "Expression", "Simple Expression", "Term", "Factor",
                                           "Number",   "2"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, AssignTest) {
  // a := 1
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  auto assign = parser.assign();
  ParserTester tester;
  assign->accept(tester);
  auto standard = std::vector<std::string>{"Assignment",        "Left", "Variable", "a",      "Right", "Expression",
                                           "Simple Expression", "Term", "Factor",   "Number", "1"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, ProcedureCallTest) {
  // a(b)
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::LPAREN, 1},
                            Token{TokenType::IDENTIFIER, "b", 1}, Token{TokenType::RPAREN, 1}};
  Parser parser(tokens);
  auto procedure_call = parser.procedureCall();
  ParserTester tester;
  procedure_call->accept(tester);
  auto standard = std::vector<std::string>{"Procedure Call", "a",      "Expression", "Simple Expression",
                                           "Term",           "Factor", "Unknown",    "b"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, CompoundStatementTest) {
  /*
  begin
  end
  */
  std::vector<Token> tokens{Token{TokenType::BEGIN, 1}, Token{TokenType::END, 2}};
  Parser parser(tokens);
  auto compound_statement = parser.compoundStatement();
  ParserTester tester;
  compound_statement->accept(tester);
  auto standard = std::vector<std::string>{"Compound Statement", "Null Statement"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, IfStatementTest) {
  /*
  if a then begin
  end
  */
  std::vector<Token> tokens{Token{TokenType::IF, 1}, Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::THEN, 1},
                            Token{TokenType::BEGIN, 1}, Token{TokenType::END, 2}};
  Parser parser(tokens);
  auto if_statement = parser.ifStatement();
  ParserTester tester;
  if_statement->accept(tester);
  auto standard = std::vector<std::string>{
      "If Statement", "Condition", "Expression", "Simple Expression",  "Term",          "Factor",
      "Unknown",      "a",         "Then",       "Compound Statement", "Null Statement"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, IfStatementTest_WithElse) {
  /*
  if a then begin
  end else begin
  end
  */
  std::vector<Token> tokens{Token{TokenType::IF, 1},    Token{TokenType::IDENTIFIER, "a", 1},
                            Token{TokenType::THEN, 1},  Token{TokenType::BEGIN, 1},
                            Token{TokenType::END, 2},   Token{TokenType::ELSE, 2},
                            Token{TokenType::BEGIN, 2}, Token{TokenType::END, 3}};
  Parser parser(tokens);
  auto if_statement = parser.ifStatement();
  ParserTester tester;
  if_statement->accept(tester);
  auto standard = std::vector<std::string>{"If Statement",
                                           "Condition",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           "Then",
                                           "Compound Statement",
                                           "Null Statement",
                                           "Else",
                                           "Compound Statement",
                                           "Null Statement"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, ForStatementTest) {
  /*
  for a := 1 to 10 do begin
  end
  */
  std::vector<Token> tokens{Token{TokenType::FOR, 1},    Token{TokenType::IDENTIFIER, "a", 1},
                            Token{TokenType::ASSIGN, 1}, Token{TokenType::NUMBER, "1", 1},
                            Token{TokenType::TO, 1},     Token{TokenType::NUMBER, "10", 1},
                            Token{TokenType::DO, 1},     Token{TokenType::BEGIN, 1},
                            Token{TokenType::END, 2}};
  Parser parser(tokens);
  auto for_statement = parser.forStatement();
  ParserTester tester;
  for_statement->accept(tester);
  auto standard = std::vector<std::string>{
      "For Statement", "a",  "From", "Expression",         "Simple Expression", "Term", "Factor",
      "Number",        "1",  "To",   "Expression",         "Simple Expression", "Term", "Factor",
      "Number",        "10", "Body", "Compound Statement", "Null Statement"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, WhileStatementTest) {
  /*
  while a do begin
  end
  */
  std::vector<Token> tokens{Token{TokenType::WHILE, 1}, Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::DO, 1},
                            Token{TokenType::BEGIN, 1}, Token{TokenType::END, 2}};
  Parser parser(tokens);
  auto while_statement = parser.whileStatement();
  ParserTester tester;
  while_statement->accept(tester);
  auto standard = std::vector<std::string>{
      "While Statement", "Condition", "Expression", "Simple Expression",  "Term",          "Factor",
      "Unknown",         "a",         "Body",       "Compound Statement", "Null Statement"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, ReadStatementTest) {
  /*
  read(a, b)
  */
  std::vector<Token> tokens{
      Token{TokenType::READ, 1},  Token{TokenType::LPAREN, 1},          Token{TokenType::IDENTIFIER, "a", 1},
      Token{TokenType::COMMA, 1}, Token{TokenType::IDENTIFIER, "b", 1}, Token{TokenType::RPAREN, 1}};
  Parser parser(tokens);
  auto read_statement = parser.readStatement();
  ParserTester tester;
  read_statement->accept(tester);
  auto standard = std::vector<std::string>{"Read Statement", "Variable", "a", "Variable", "b"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, WriteStatementTest) {
  /*
  write(a, b)
  */
  std::vector<Token> tokens{Token{TokenType::WRITE, 1},           Token{TokenType::LPAREN, 1},
                            Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::COMMA, 1},
                            Token{TokenType::IDENTIFIER, "b", 1}, Token{TokenType::RPAREN, 1}};
  Parser parser(tokens);
  auto write_statement = parser.writeStatement();
  ParserTester tester;
  write_statement->accept(tester);
  auto standard = std::vector<std::string>{"Write Statement",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "b"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, BreakStatementTest) {
  /*
  break
  */
  std::vector<Token> tokens{Token{TokenType::BREAK, 1}};
  Parser parser(tokens);
  auto break_statement = parser.breakStatement();
  ParserTester tester;
  break_statement->accept(tester);
  auto standard = std::vector<std::string>{"Break Statement"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, FactorTest) {
  /*
  a
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}};
  Parser parser(tokens);
  auto factor = parser.factor();
  ParserTester tester;
  factor->accept(tester);
  auto standard = std::vector<std::string>{"Factor", "Unknown", "a"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, FactorTest_Parentheses) {
  /*
  (a)
  */
  std::vector<Token> tokens{Token{TokenType::LPAREN, 1}, Token{TokenType::IDENTIFIER, "a", 1},
                            Token{TokenType::RPAREN, 1}};
  Parser parser(tokens);
  auto factor = parser.factor();
  ParserTester tester;
  factor->accept(tester);
  auto standard = std::vector<std::string>{"Factor", "Expression", "Expression", "Simple Expression",
                                           "Term",   "Factor",     "Unknown",    "a"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, FactorTest_Number) {
  /*
  1
  */
  std::vector<Token> tokens{Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  auto factor = parser.factor();
  ParserTester tester;
  factor->accept(tester);
  auto standard = std::vector<std::string>{"Factor", "Number", "1"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, FactorTest_Boolean) {
  /*
  true
  */
  std::vector<Token> tokens{Token{TokenType::TRUE, 1}};
  Parser parser(tokens);
  auto factor = parser.factor();
  ParserTester tester;
  factor->accept(tester);
  auto standard = std::vector<std::string>{"Factor", "Boolean", "true"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, FactorTest_Function) {
  /*
  a(b)
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::LPAREN, 1},
                            Token{TokenType::IDENTIFIER, "b", 1}, Token{TokenType::RPAREN, 1}};
  Parser parser(tokens);
  auto factor = parser.factor();
  ParserTester tester;
  factor->accept(tester);
  auto standard = std::vector<std::string>{
      "Factor", "Function", "Procedure Call", "a", "Expression", "Simple Expression", "Term", "Factor", "Unknown", "b"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, FactorTest_WithNot) {
  /*
  not a
  */
  std::vector<Token> tokens{Token{TokenType::NOT, 1}, Token{TokenType::IDENTIFIER, "a", 1}};
  Parser parser(tokens);
  auto factor = parser.factor();
  ParserTester tester;
  factor->accept(tester);
  auto standard = std::vector<std::string>{"Factor", "With Not", "Factor", "Unknown", "a"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, FactorTest_WithUminus) {
  /*
  -a
  */
  std::vector<Token> tokens{Token{TokenType::MINUS, 1}, Token{TokenType::IDENTIFIER, "a", 1}};
  Parser parser(tokens);
  auto factor = parser.factor();
  ParserTester tester;
  factor->accept(tester);
  auto standard = std::vector<std::string>{"Factor", "With Uminus", "Factor", "Unknown", "a"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, TermTest) {
  /*
  a * b
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::TIMES, 1},
                            Token{TokenType::IDENTIFIER, "b", 1}};
  Parser parser(tokens);
  auto term = parser.term();
  ParserTester tester;
  term->accept(tester);
  auto standard = std::vector<std::string>{"Term", "Factor", "Unknown", "a", "*", "Factor", "Unknown", "b"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, SimpleExpressionTest) {
  /*
  a + b
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::PLUS, 1},
                            Token{TokenType::IDENTIFIER, "b", 1}};
  Parser parser(tokens);
  auto simple_expression = parser.simpleExpression();
  ParserTester tester;
  simple_expression->accept(tester);
  auto standard = std::vector<std::string>{"Simple Expression", "Term", "Factor", "Unknown", "a", "+", "Term", "Factor",
                                           "Unknown",           "b"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, ExpressionTest) {
  /*
  a + b > c
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::PLUS, 1},
                            Token{TokenType::IDENTIFIER, "b", 1}, Token{TokenType::GT, 1},
                            Token{TokenType::IDENTIFIER, "c", 1}};
  Parser parser(tokens);
  auto expression = parser.expression();
  ParserTester tester;
  expression->accept(tester);
  auto standard = std::vector<std::string>{
      "Expression", "Simple Expression", "Term", "Factor", "Unknown", "a", "+", "Term", "Factor", "Unknown", "b",
      ">",          "Simple Expression", "Term", "Factor", "Unknown", "c"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, MatchTest) {
  /*
  a := 1
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  ASSERT_TRUE(parser.match(TokenType::IDENTIFIER));
  ASSERT_FALSE(parser.match(TokenType::NUMBER));
}

TEST(ParserTest, CheckTest) {
  /*
  a := 1
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  ASSERT_TRUE(parser.check(TokenType::IDENTIFIER));
  ASSERT_FALSE(parser.check(TokenType::NUMBER));
}

TEST(ParserTest, IsEndTest) {
  /*
  a := 1
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}, Token{TokenType::END_OF_FILE, 1}};
  Parser parser(tokens);
  ASSERT_FALSE(parser.isEnd());
  parser.forward();
  parser.forward();
  ASSERT_FALSE(parser.isEnd());
  parser.forward();
  ASSERT_TRUE(parser.isEnd());
}

TEST(ParserTest, GetTokenTest) {
  /*
  a := 1
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  ASSERT_EQ(parser.getToken().type, TokenType::IDENTIFIER);
  parser.forward();
  ASSERT_EQ(parser.getToken().type, TokenType::ASSIGN);
}

TEST(ParserTest, ForwardTest) {
  /*
  a := 1
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  ASSERT_EQ(parser.getToken().type, TokenType::IDENTIFIER);
  parser.forward();
  ASSERT_EQ(parser.getToken().type, TokenType::ASSIGN);
}

TEST(ParserTest, BackwardTest) {
  /*
  a := 1
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  ASSERT_EQ(parser.getToken().type, TokenType::IDENTIFIER);
  parser.forward();
  ASSERT_EQ(parser.getToken().type, TokenType::ASSIGN);
  parser.backward();
  ASSERT_EQ(parser.getToken().type, TokenType::IDENTIFIER);
}

TEST(ParserTest, ConsumeTest) {
  /*
  a := 1
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  ASSERT_EQ(parser.getToken().type, TokenType::IDENTIFIER);
  parser.consume(TokenType::IDENTIFIER, "a");
  ASSERT_EQ(parser.getToken().type, TokenType::ASSIGN);
}

TEST(ParserTest, ConsumeTest_Fail) {
  /*
  a := 1
  */
  std::vector<Token> tokens{Token{TokenType::IDENTIFIER, "a", 1}, Token{TokenType::ASSIGN, 1},
                            Token{TokenType::NUMBER, "1", 1}};
  Parser parser(tokens);
  ASSERT_EQ(parser.getToken().type, TokenType::IDENTIFIER);
  try {
    parser.consume(TokenType::NUMBER, "Expected NUMBER");
    FAIL() << "Expected ParserException.";
  } catch (const ParserException& e) {
    ASSERT_STREQ(e.what(), "Expected NUMBER");
  }
  // ASSERT_THROW(parser.consume(TokenType::NUMBER, "a"), std::runtime_error);
}

TEST(ParserTest, ARealProgram) {
  /*
  program hello;
  const pi=3.14;
  var a, b: integer;
  function inc(b: integer): integer;
  begin
    inc := b + 1;
  end;
  begin
    read(a, b);
    inc(a); inc(b);
    if a > b then
        write(a)
    else
      begin
        for i := 1 to 10 do
          begin
            a := a + i;
            b := b - i;
          end
      end;
    while a > 0 do
      begin
        a := a - 1;
        write(a);
        if a = 0 then
          break
      end
  end.
  */

  /**
1 PROGRAM
1 IDENTIFIER hello
1 SEMICOLON
2 CONST
2 IDENTIFIER pi
2 EQUAL
2 NUMBER 3.14
2 SEMICOLON
3 VAR
3 IDENTIFIER a
3 COMMA
3 IDENTIFIER b
3 COLON
3 INTEGER
3 SEMICOLON
4 FUNCTION
4 IDENTIFIER inc
4 LPAREN
4 IDENTIFIER b
4 COLON
4 INTEGER
4 RPAREN
4 COLON
4 INTEGER
4 SEMICOLON
5 BEGIN
6 IDENTIFIER inc
6 ASSIGN
6 IDENTIFIER b
6 PLUS
6 NUMBER 1
6 SEMICOLON
7 END
7 SEMICOLON
8 BEGIN
9 READ
9 LPAREN
9 IDENTIFIER a
9 COMMA
9 IDENTIFIER b
9 RPAREN
9 SEMICOLON
10 IDENTIFIER inc
10 LPAREN
10 IDENTIFIER a
10 RPAREN
10 SEMICOLON
10 IDENTIFIER inc
10 LPAREN
10 IDENTIFIER b
10 RPAREN
10 SEMICOLON
11 IF
11 IDENTIFIER a
11 GT
11 IDENTIFIER b
11 THEN
12 WRITE
12 LPAREN
12 IDENTIFIER a
12 RPAREN
13 ELSE
14 BEGIN
15 FOR
15 IDENTIFIER i
15 ASSIGN
15 NUMBER 1
15 TO
15 NUMBER 10
15 DO
16 BEGIN
17 IDENTIFIER a
17 ASSIGN
17 IDENTIFIER a
17 PLUS
17 IDENTIFIER i
17 SEMICOLON
18 IDENTIFIER b
18 ASSIGN
18 IDENTIFIER b
18 MINUS
18 IDENTIFIER i
18 SEMICOLON
19 END
20 END
20 SEMICOLON
21 WHILE
21 IDENTIFIER a
21 GT
21 NUMBER 0
21 DO
22 BEGIN
23 IDENTIFIER a
23 ASSIGN
23 IDENTIFIER a
23 MINUS
23 NUMBER 1
23 SEMICOLON
24 WRITE
24 LPAREN
24 IDENTIFIER a
24 RPAREN
24 SEMICOLON
25 IF
25 IDENTIFIER a
25 EQUAL
25 NUMBER 0
25 THEN
26 BREAK
27 END
28 END
28 DOT
29 END_OF_FILE


   */
  std::vector<Token> tokens{Token{TokenType::PROGRAM, 1},
                            Token{TokenType::IDENTIFIER, "hello", 1},
                            Token{TokenType::SEMICOLON, 1},
                            Token{TokenType::CONST, 2},
                            Token{TokenType::IDENTIFIER, "pi", 2},
                            Token{TokenType::EQUAL, 2},
                            Token{TokenType::NUMBER, "3.14", 2},
                            Token{TokenType::SEMICOLON, 2},
                            Token{TokenType::VAR, 3},
                            Token{TokenType::IDENTIFIER, "a", 3},
                            Token{TokenType::COMMA, 3},
                            Token{TokenType::IDENTIFIER, "b", 3},
                            Token{TokenType::COLON, 3},
                            Token{TokenType::INTEGER, 3},
                            Token{TokenType::SEMICOLON, 3},
                            Token{TokenType::FUNCTION, 4},
                            Token{TokenType::IDENTIFIER, "inc", 4},
                            Token{TokenType::LPAREN, 4},
                            Token{TokenType::IDENTIFIER, "b", 4},
                            Token{TokenType::COLON, 4},
                            Token{TokenType::INTEGER, 4},
                            Token{TokenType::RPAREN, 4},
                            Token{TokenType::COLON, 4},
                            Token{TokenType::INTEGER, 4},
                            Token{TokenType::SEMICOLON, 4},
                            Token{TokenType::BEGIN, 5},
                            Token{TokenType::IDENTIFIER, "inc", 6},
                            Token{TokenType::ASSIGN, 6},
                            Token{TokenType::IDENTIFIER, "b", 6},
                            Token{TokenType::PLUS, 6},
                            Token{TokenType::NUMBER, "1", 6},
                            Token{TokenType::SEMICOLON, 6},
                            Token{TokenType::END, 7},
                            Token{TokenType::SEMICOLON, 7},
                            Token{TokenType::BEGIN, 8},
                            Token{TokenType::READ, 9},
                            Token{TokenType::LPAREN, 9},
                            Token{TokenType::IDENTIFIER, "a", 9},
                            Token{TokenType::COMMA, 9},
                            Token{TokenType::IDENTIFIER, "b", 9},
                            Token{TokenType::RPAREN, 9},
                            Token{TokenType::SEMICOLON, 9},
                            Token{TokenType::IDENTIFIER, "inc", 10},
                            Token{TokenType::LPAREN, 10},
                            Token{TokenType::IDENTIFIER, "a", 10},
                            Token{TokenType::RPAREN, 10},
                            Token{TokenType::SEMICOLON, 10},
                            Token{TokenType::IDENTIFIER, "inc", 10},
                            Token{TokenType::LPAREN, 10},
                            Token{TokenType::IDENTIFIER, "b", 10},
                            Token{TokenType::RPAREN, 10},
                            Token{TokenType::SEMICOLON, 10},
                            Token{TokenType::IF, 11},
                            Token{TokenType::IDENTIFIER, "a", 11},
                            Token{TokenType::GT, 11},
                            Token{TokenType::IDENTIFIER, "b", 11},
                            Token{TokenType::THEN, 11},
                            Token{TokenType::WRITE, 12},
                            Token{TokenType::LPAREN, 12},
                            Token{TokenType::IDENTIFIER, "a", 12},
                            Token{TokenType::RPAREN, 12},
                            Token{TokenType::ELSE, 13},
                            Token{TokenType::BEGIN, 14},
                            Token{TokenType::FOR, 15},
                            Token{TokenType::IDENTIFIER, "i", 15},
                            Token{TokenType::ASSIGN, 15},
                            Token{TokenType::NUMBER, "1", 15},
                            Token{TokenType::TO, 15},
                            Token{TokenType::NUMBER, "10", 15},
                            Token{TokenType::DO, 15},
                            Token{TokenType::BEGIN, 16},
                            Token{TokenType::IDENTIFIER, "a", 17},
                            Token{TokenType::ASSIGN, 17},
                            Token{TokenType::IDENTIFIER, "a", 17},
                            Token{TokenType::PLUS, 17},
                            Token{TokenType::IDENTIFIER, "i", 17},
                            Token{TokenType::SEMICOLON, 17},
                            Token{TokenType::IDENTIFIER, "b", 18},
                            Token{TokenType::ASSIGN, 18},
                            Token{TokenType::IDENTIFIER, "b", 18},
                            Token{TokenType::MINUS, 18},
                            Token{TokenType::IDENTIFIER, "i", 18},
                            Token{TokenType::SEMICOLON, 18},
                            Token{TokenType::END, 19},
                            Token{TokenType::END, 20},
                            Token{TokenType::SEMICOLON, 20},
                            Token{TokenType::WHILE, 21},
                            Token{TokenType::IDENTIFIER, "a", 21},
                            Token{TokenType::GT, 21},
                            Token{TokenType::NUMBER, "0", 21},
                            Token{TokenType::DO, 21},
                            Token{TokenType::BEGIN, 22},
                            Token{TokenType::IDENTIFIER, "a", 23},
                            Token{TokenType::ASSIGN, 23},
                            Token{TokenType::IDENTIFIER, "a", 23},
                            Token{TokenType::MINUS, 23},
                            Token{TokenType::NUMBER, "1", 23},
                            Token{TokenType::SEMICOLON, 23},
                            Token{TokenType::WRITE, 24},
                            Token{TokenType::LPAREN, 24},
                            Token{TokenType::IDENTIFIER, "a", 24},
                            Token{TokenType::RPAREN, 24},
                            Token{TokenType::SEMICOLON, 24},
                            Token{TokenType::IF, 25},
                            Token{TokenType::IDENTIFIER, "a", 25},
                            Token{TokenType::EQUAL, 25},
                            Token{TokenType::NUMBER, "0", 25},
                            Token{TokenType::THEN, 25},
                            Token{TokenType::BREAK, 26},
                            Token{TokenType::END, 27},
                            Token{TokenType::END, 28},
                            Token{TokenType::DOT, 28},
                            Token{TokenType::END_OF_FILE, 29}};
  Parser parser(tokens);
  auto program = parser.program();
  ParserTester tester;
  program->accept(tester);
  auto standard = std::vector<std::string>{"Program",
                                           "hello",
                                           "Parameters",
                                           "Const Declarations",
                                           "Const Declaration",
                                           "pi",
                                           "3.14",
                                           "Var Declarations",
                                           "Var Declaration",
                                           "a",
                                           "b",
                                           "Type",
                                           "int",
                                           "Subprograms",
                                           "Subprogram",
                                           "inc",
                                           "Parameters",
                                           "Parameter",
                                           "value",
                                           "int",
                                           "b",
                                           "Return Type",
                                           "int",
                                           "Function",
                                           "Const Declarations",
                                           "Var Declarations",
                                           "Body",
                                           "Compound Statement",
                                           "Assignment",
                                           "Left",
                                           "Variable",
                                           "inc",
                                           "Right",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "b",
                                           "+",
                                           "Term",
                                           "Factor",
                                           "Number",
                                           "1",
                                           "Null Statement",
                                           "Body",
                                           "Compound Statement",
                                           "Read Statement",
                                           "Variable",
                                           "a",
                                           "Variable",
                                           "b",
                                           "Procedure Call",
                                           "inc",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           "Procedure Call",
                                           "inc",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "b",
                                           "If Statement",
                                           "Condition",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           ">",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "b",
                                           "Then",
                                           "Write Statement",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           "Else",
                                           "Compound Statement",
                                           "For Statement",
                                           "i",
                                           "From",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Number",
                                           "1",
                                           "To",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Number",
                                           "10",
                                           "Body",
                                           "Compound Statement",
                                           "Assignment",
                                           "Left",
                                           "Variable",
                                           "a",
                                           "Right",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           "+",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "i",
                                           "Assignment",
                                           "Left",
                                           "Variable",
                                           "b",
                                           "Right",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "b",
                                           "-",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "i",
                                           "Null Statement",
                                           "While Statement",
                                           "Condition",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           ">",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Number",
                                           "0",
                                           "Body",
                                           "Compound Statement",
                                           "Assignment",
                                           "Left",
                                           "Variable",
                                           "a",
                                           "Right",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           "-",
                                           "Term",
                                           "Factor",
                                           "Number",
                                           "1",
                                           "Write Statement",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           "If Statement",
                                           "Condition",
                                           "Expression",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Unknown",
                                           "a",
                                           "==",
                                           "Simple Expression",
                                           "Term",
                                           "Factor",
                                           "Number",
                                           "0",
                                           "Then",
                                           "Break Statement"};
  ASSERT_EQ(tester.getResult(), standard);
}

TEST(ParserTest, ErrorHandling1) {
  /*
  program hello;
  begin end
  */
  std::vector<Token> tokens{Token{TokenType::PROGRAM, 1},   Token{TokenType::IDENTIFIER, "hello", 1},
                            Token{TokenType::SEMICOLON, 1}, Token{TokenType::BEGIN, 2},
                            Token{TokenType::END, 2},       Token{TokenType::END_OF_FILE, 2}};
  Parser parser(tokens);
  try {
    parser.program();
    FAIL() << "Expected ParserException.";
  } catch (const ParserException& e) {
    ASSERT_STREQ(e.what(), "Expected '.' at the end of the program.");
  }
}

TEST(ParserTest, ErrorHandling2) {
  /*
  program hello;
  var a: integer;
  begin
    read(a;
  end.
  */
  std::vector<Token> tokens{Token{TokenType::PROGRAM, 1},
                            Token{TokenType::IDENTIFIER, "hello", 1},
                            Token{TokenType::SEMICOLON, 1},
                            Token{TokenType::VAR, 2},
                            Token{TokenType::IDENTIFIER, "a", 2},
                            Token{TokenType::COLON, 2},
                            Token{TokenType::INTEGER, 2},
                            Token{TokenType::SEMICOLON, 2},
                            Token{TokenType::BEGIN, 3},
                            Token{TokenType::READ, 4},
                            Token{TokenType::LPAREN, 4},
                            Token{TokenType::IDENTIFIER, "a", 4},
                            Token{TokenType::SEMICOLON, 4},
                            Token{TokenType::END, 5},
                            Token(TokenType::DOT, 5),
                            Token{TokenType::END_OF_FILE, 4}};
  Parser parser(tokens);
  try {
    parser.program();
    FAIL() << "Expected ParserException.";
  } catch (const ParserException& e) {
    ASSERT_STREQ(e.what(), "Expected ')' to close variable list in read statement.");
  }
}

TEST(ParserTest, ErrorHandling3) {
  /*
  program hello;
  begin
    for i = 1 to 10 do ;
  end.
  */
  std::vector<Token> tokens{Token{TokenType::PROGRAM, 1},    Token{TokenType::IDENTIFIER, "hello", 1},
                            Token{TokenType::SEMICOLON, 1},  Token{TokenType::BEGIN, 2},
                            Token{TokenType::FOR, 3},        Token{TokenType::IDENTIFIER, "i", 3},
                            Token{TokenType::EQUAL, 3},      Token{TokenType::NUMBER, "1", 3},
                            Token{TokenType::TO, 3},         Token{TokenType::NUMBER, "10", 3},
                            Token{TokenType::DO, 3},         Token{TokenType::SEMICOLON, 3},
                            Token{TokenType::END, 4},        Token{TokenType::DOT, 4},
                            Token{TokenType::END_OF_FILE, 4}};
  Parser parser(tokens);
  try {
    parser.program();
    FAIL() << "Expected ParserException.";
  } catch (const ParserException& e) {
    ASSERT_STREQ(e.what(), "Expected ':=' for loop control variable initialization.");
  }
}