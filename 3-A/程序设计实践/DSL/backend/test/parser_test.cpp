#include "parser.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "ast.hpp"
#include "exception.hpp"
#include "printer.hpp"
#include "token.hpp"

TEST(ParserTest, ConstantDefines) {
  const std::vector<Token> tokens{{TokenType::IDENTIFIER, "constantA", 1},
                                  {TokenType::EQUAL, 1},
                                  {TokenType::STRING, "valueA", 1},
                                  {TokenType::IDENTIFIER, "constantB", 2},
                                  {TokenType::EQUAL, 2},
                                  {TokenType::STRING, "valueB", 2},
                                  {TokenType::EOF_, 2}};
  /*
    constantA = "valueA"
    constantB = "valueB"
  */
  Parser parser(tokens);
  const Printer printer;
  const auto program = parser.parse();
  testing::internal::CaptureStdout();
  program->accept(printer);
  const auto output = testing::internal::GetCapturedStdout();

  auto program_std = Program();
  program_std.addConstant(std::make_unique<ConstantDefine>("constantA", "valueA"));
  program_std.addConstant(std::make_unique<ConstantDefine>("constantB", "valueB"));
  testing::internal::CaptureStdout();
  program_std.accept(printer);
  const auto std = testing::internal::GetCapturedStdout();

  ASSERT_EQ(output, std);
}

TEST(ParserTest, Procedures) {
  const std::vector<Token> tokens{
      {TokenType::PROCEDURE, 1}, {TokenType::IDENTIFIER, "main", 1}, {TokenType::LBRACE, 1}, {TokenType::RBRACE, 2},
      {TokenType::PROCEDURE, 3}, {TokenType::IDENTIFIER, "proc", 3}, {TokenType::LBRACE, 3}, {TokenType::RBRACE, 4},
      {TokenType::EOF_, 4}};
  /*
    procedure main {
    }
    procedure proc {
    }
  */
  Parser parser(tokens);
  const Printer printer;
  const auto program = parser.parse();
  testing::internal::CaptureStdout();
  program->accept(printer);
  const auto output = testing::internal::GetCapturedStdout();

  auto program_std = Program();
  program_std.addProcedure(std::make_unique<Procedure>("main"));
  program_std.addProcedure(std::make_unique<Procedure>("proc"));
  testing::internal::CaptureStdout();
  program_std.accept(printer);
  const auto std = testing::internal::GetCapturedStdout();

  ASSERT_EQ(output, std);
}

TEST(ParserTest, Statements) {
  const std::vector<Token> tokens{{TokenType::PROCEDURE, 1},   {TokenType::IDENTIFIER, "main", 1},
                                  {TokenType::LBRACE, 1},

                                  {TokenType::SAY, 2},         {TokenType::STRING, "Hello ", 2},
                                  {TokenType::COMMA, 2},       {TokenType::IDENTIFIER, "world", 2},

                                  {TokenType::LOOKUP, 3},      {TokenType::IDENTIFIER, "data", 3},
                                  {TokenType::IN, 3},          {TokenType::IDENTIFIER, "db", 3},
                                  {TokenType::COLON, 3},       {TokenType::IDENTIFIER, "table", 3},
                                  {TokenType::LISTEN, 4},      {TokenType::TIMELIMIT, 4},
                                  {TokenType::NUMBER, "5", 4}, {TokenType::LBRACE, 4},
                                  {TokenType::RBRACE, 5},

                                  {TokenType::RBRACE, 6},      {TokenType::EOF_, 6}};
  /*
    procedure main {
      say "Hello ", world
      lookup data in db:table
      listen timelimit 5 {
      }
    }
  */
  Parser parser(tokens);
  const Printer printer;
  const auto program = parser.parse();
  testing::internal::CaptureStdout();
  program->accept(printer);
  const auto output = testing::internal::GetCapturedStdout();

  auto program_std = Program();
  auto procedure = std::make_unique<Procedure>("main");
  auto say = std::make_unique<Say>();
  say->addContent(std::make_unique<String>("Hello "));
  say->addContent(std::make_unique<Identifier>("world"));
  procedure->addStatement(std::move(say));
  procedure->addStatement(std::make_unique<Lookup>("data", "db", "table"));
  procedure->addStatement(std::make_unique<Listen>(5));
  program_std.addProcedure(std::move(procedure));
  testing::internal::CaptureStdout();
  program_std.accept(printer);
  const auto std = testing::internal::GetCapturedStdout();

  ASSERT_EQ(output, std);
}

TEST(ParserTest, ListenClauses) {
  const std::vector<Token> tokens{{TokenType::PROCEDURE, 1},
                                  {TokenType::IDENTIFIER, "main", 1},
                                  {TokenType::LBRACE, 1},

                                  {TokenType::LISTEN, 2},
                                  {TokenType::TIMELIMIT, 2},
                                  {TokenType::NUMBER, "5", 2},
                                  {TokenType::LBRACE, 2},

                                  {TokenType::HAS, 3},
                                  {TokenType::IDENTIFIER, "XYZ", 3},
                                  {TokenType::QUESTION, 3},
                                  {TokenType::IDENTIFIER, "procA", 3},

                                  {TokenType::ANYTHING, 4},
                                  {TokenType::QUESTION, 4},
                                  {TokenType::IDENTIFIER, "procB", 4},

                                  {TokenType::TIMEOUT, 5},
                                  {TokenType::QUESTION, 5},
                                  {TokenType::WRITETO, 5},
                                  {TokenType::IDENTIFIER, "db", 5},
                                  {TokenType::COLON, 5},
                                  {TokenType::IDENTIFIER, "tbl", 5},
                                  {TokenType::COLON, 5},
                                  {TokenType::IDENTIFIER, "col", 5},

                                  {TokenType::DEFAULT, 6},
                                  {TokenType::QUESTION, 6},
                                  {TokenType::WRITETO, 6},
                                  {TokenType::IDENTIFIER, "db", 6},
                                  {TokenType::COLON, 6},
                                  {TokenType::IDENTIFIER, "tbl", 6},
                                  {TokenType::COLON, 6},
                                  {TokenType::IDENTIFIER, "col", 6},

                                  {TokenType::RBRACE, 7},

                                  {TokenType::RBRACE, 8},
                                  {TokenType::EOF_, 8}};
  /*
    procedure main {
      listen timelimit 5 {
        has XYZ? procA
        anything? procB
        timeout? writeto db:tbl:col
        default? writeto db:tbl:col
      }
    }
  */
  Parser parser(tokens);
  const Printer printer;
  const auto program = parser.parse();
  testing::internal::CaptureStdout();
  program->accept(printer);
  const auto output = testing::internal::GetCapturedStdout();

  auto program_std = Program();
  auto procedure = std::make_unique<Procedure>("main");
  auto listen = std::make_unique<Listen>(5);
  auto has = std::make_unique<Clause>();
  has->setType(Clause::Type::HAS);
  has->setAction(Clause::Action::JUMP);
  has->setPattern(std::make_unique<Identifier>("XYZ"));
  has->setBranch("procA");
  listen->addClause(std::move(has));

  auto anything = std::make_unique<Clause>();
  anything->setType(Clause::Type::ANYTHING);
  anything->setAction(Clause::Action::JUMP);
  anything->setBranch("procB");
  listen->addClause(std::move(anything));

  auto timeout = std::make_unique<Clause>();
  timeout->setType(Clause::Type::TIMEOUT);
  timeout->setAction(Clause::Action::WRITETO);
  timeout->setDb("db");
  timeout->setTable("tbl");
  timeout->setColumn("col");
  listen->addClause(std::move(timeout));

  auto def = std::make_unique<Clause>();
  def->setType(Clause::Type::DEFAULT);
  def->setAction(Clause::Action::WRITETO);
  def->setDb("db");
  def->setTable("tbl");
  def->setColumn("col");
  listen->addClause(std::move(def));

  procedure->addStatement(std::move(listen));
  program_std.addProcedure(std::move(procedure));
  testing::internal::CaptureStdout();
  program_std.accept(printer);
  const auto std = testing::internal::GetCapturedStdout();

  ASSERT_EQ(output, std);
}

TEST(ParserTest, General) {
  const std::vector<Token> tokens{{TokenType::IDENTIFIER, "db", 1},
                                  {TokenType::EQUAL, 1},
                                  {TokenType::STRING, "./1.db", 1},
                                  {TokenType::PROCEDURE, 2},
                                  {TokenType::IDENTIFIER, "main", 2},
                                  {TokenType::LBRACE, 2},
                                  {TokenType::SAY, 3},
                                  {TokenType::STRING, "Hello", 3},
                                  {TokenType::LISTEN, 4},
                                  {TokenType::TIMELIMIT, 4},
                                  {TokenType::NUMBER, "20", 4},
                                  {TokenType::LBRACE, 4},
                                  {TokenType::DEFAULT, 5},
                                  {TokenType::QUESTION, 5},
                                  {TokenType::IDENTIFIER, "defaultProc", 5},
                                  {TokenType::RBRACE, 6},
                                  {TokenType::RBRACE, 7},

                                  {TokenType::PROCEDURE, 8},
                                  {TokenType::IDENTIFIER, "defaultProc", 8},
                                  {TokenType::LBRACE, 8},
                                  {TokenType::SAY, 9},
                                  {TokenType::STRING, "This is defaultProc", 9},
                                  {TokenType::RBRACE, 10},

                                  {TokenType::EOF_, 10}};
  /*
    db = "./1.db"
    procedure main {
      say "Hello"
      listen timelimit 20 {
        default? defaultProc
      }
    }
    procedure defaultProc {
      say "This is defaultProc"
    }
  */
  Parser parser(tokens);
  const Printer printer;
  const auto program = parser.parse();
  testing::internal::CaptureStdout();
  program->accept(printer);
  const auto output = testing::internal::GetCapturedStdout();

  auto program_std = Program();
  program_std.addConstant(std::make_unique<ConstantDefine>("db", "./1.db"));
  auto procedure = std::make_unique<Procedure>("main");

  auto say = std::make_unique<Say>();
  say->addContent(std::make_unique<String>("Hello"));
  procedure->addStatement(std::move(say));

  auto listen = std::make_unique<Listen>(20);
  auto def = std::make_unique<Clause>();
  def->setType(Clause::Type::DEFAULT);
  def->setAction(Clause::Action::JUMP);
  def->setBranch("defaultProc");
  listen->addClause(std::move(def));
  procedure->addStatement(std::move(listen));

  auto defaultProc = std::make_unique<Procedure>("defaultProc");
  auto sayDefault = std::make_unique<Say>();
  sayDefault->addContent(std::make_unique<String>("This is defaultProc"));
  defaultProc->addStatement(std::move(sayDefault));

  program_std.addProcedure(std::move(procedure));
  program_std.addProcedure(std::move(defaultProc));
  testing::internal::CaptureStdout();
  program_std.accept(printer);
  const auto std = testing::internal::GetCapturedStdout();

  ASSERT_EQ(output, std);
}

TEST(ParserTest, ErrorHandling1) {
  const std::vector<Token> tokens{{TokenType::IDENTIFIER, "constantA", 1}, {TokenType::EQUAL, 1},
                                  {TokenType::IDENTIFIER, "constantB", 2}, {TokenType::EQUAL, 2},
                                  {TokenType::STRING, "valueB", 2},        {TokenType::EOF_, 2}};
  /*
    constantA =
    constantB = "valueB"
  */
  Parser parser(tokens);
  std::string s;
  std::size_t line;
  try {
    auto program = parser.parse();
  } catch (const ParserException& ex) {
    s = ex.what();
    line = ex.getLine();
  }
  ASSERT_EQ(s, "Except a string.");
  ASSERT_EQ(line, 2);
}

TEST(ParserTest, ErrorHandling2) {
  const std::vector<Token> tokens{
      {TokenType::PROCEDURE, 1},
      {TokenType::IDENTIFIER, "main", 1},
      {TokenType::RBRACE, 1},
      {TokenType::EOF_, 1},
  };
  /*
     procedure main }
  */
  Parser parser(tokens);
  std::string s;
  std::size_t line;
  try {
    auto program = parser.parse();
  } catch (const ParserException& ex) {
    s = ex.what();
    line = ex.getLine();
  }
  ASSERT_EQ(s, "Except a '{'.");
  ASSERT_EQ(line, 1);
}
