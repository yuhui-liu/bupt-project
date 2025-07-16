#include "interpreter.hpp"

#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <streambuf>
#include <string>

#include "ast.hpp"
#include "exception.hpp"

// 自定义的 streambuf 类，用于重定向输入
class StringBuf : public std::streambuf {
 public:
  StringBuf(const std::string& str) { this->str(str); }

  void str(const std::string& str) {
    buffer = str;
    setg(buffer.data(), buffer.data(), buffer.data() + buffer.size());
  }

 private:
  std::string buffer;
};

TEST(InterpreterTest, SAY) {
  /*
    var="tom"
    procedure main {
      say "hello ", var
    }
  */
  auto program = std::make_shared<Program>();
  program->addConstant(std::make_unique<ConstantDefine>("var", "tom"));
  auto procedure = std::make_unique<Procedure>("main");
  auto say = std::make_unique<Say>();
  say->addContent(std::make_unique<String>("hello "));
  say->addContent(std::make_unique<Identifier>("var"));
  procedure->addStatement(std::move(say));
  program->addProcedure(std::move(procedure));

  Interpreter interpreter(program, "cli", {{"var", "tom"}}, {{"main", 0}});

  testing::internal::CaptureStdout();
  program->accept(interpreter);
  const auto output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "hello tom\n");
}

TEST(InterpreterTest, LISTEN) {
  /*
    procedure main {
      listen timelimit 10 {
        has "TeSt" ? A
      }
    }
    procedure A{
      say "call A"
    }
  */
  auto program = std::make_shared<Program>();
  auto procedure = std::make_unique<Procedure>("main");
  auto listen = std::make_unique<Listen>(10);
  auto clause = std::make_unique<Clause>();
  clause->setType(Clause::Type::HAS);
  clause->setAction(Clause::Action::JUMP);
  clause->setPattern(std::make_unique<String>("TeSt"));
  clause->setBranch("A");
  listen->addClause(std::move(clause));
  procedure->addStatement(std::move(listen));
  program->addProcedure(std::move(procedure));
  procedure = std::make_unique<Procedure>("A");
  auto say = std::make_unique<Say>();
  say->addContent(std::make_unique<String>("call A"));
  procedure->addStatement(std::move(say));
  program->addProcedure(std::move(procedure));

  Interpreter interpreter(program, "cli", {}, {{"main", 0}, {"A", 1}});
  std::string input = "TeSt\n";
  StringBuf simulatedInput(input);
  std::cin.rdbuf(&simulatedInput);

  testing::internal::CaptureStdout();
  program->accept(interpreter);
  const auto output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "call A\n");
}

TEST(InterpreterTest, EXIT) {
  /*
    procedure main {
      exit
    }
  */
  auto program = std::make_shared<Program>();
  auto procedure = std::make_unique<Procedure>("main");
  procedure->addStatement(std::make_unique<Exit>());
  program->addProcedure(std::move(procedure));

  Interpreter interpreter(program, "cli", {}, {{"main", 0}});
  EXPECT_THROW(program->accept(interpreter), ExitException);
}

TEST(InterpreterTest, JUMP) {
  /*
    procedure main {
      A
    }
    procedure A{}
      say "call A"
    }
  */
  auto program = std::make_shared<Program>();
  auto procedure = std::make_unique<Procedure>("main");
  procedure->addStatement(std::make_unique<Jump>("A"));
  program->addProcedure(std::move(procedure));
  procedure = std::make_unique<Procedure>("A");
  auto say = std::make_unique<Say>();
  say->addContent(std::make_unique<String>("call A"));
  procedure->addStatement(std::move(say));
  program->addProcedure(std::move(procedure));

  Interpreter interpreter(program, "cli", {}, {{"main", 0}, {"A", 1}});

  testing::internal::CaptureStdout();
  program->accept(interpreter);
  const auto output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "call A\n");
}
