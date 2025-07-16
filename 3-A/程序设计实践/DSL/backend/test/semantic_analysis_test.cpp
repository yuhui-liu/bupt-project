#include "semantic_analysis.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <exception>
#include <map>
#include <memory>
#include <string>

#include "exception.hpp"

TEST(SemanticAnalysisTest, Constants) {
  const auto program = std::make_shared<Program>();
  program->addConstant(std::make_unique<ConstantDefine>("constantA", "valueA"));
  program->addConstant(std::make_unique<ConstantDefine>("constantB", "valueB"));
  program->addProcedure(std::make_unique<Procedure>("main"));
  program->addProcedure(std::make_unique<Procedure>("proc"));
  const SemanticAnalysis sa(program);
  const auto constants = sa.getConstants();
  const std::map<std::string, std::string> std{{"constantA", "valueA"}, {"constantB", "valueB"}};
  ASSERT_EQ(constants, std);
}

TEST(SemanticAnalysisTest, Procedures) {
  const auto program = std::make_shared<Program>();
  program->addConstant(std::make_unique<ConstantDefine>("constantA", "valueA"));
  program->addConstant(std::make_unique<ConstantDefine>("constantB", "valueB"));
  program->addProcedure(std::make_unique<Procedure>("main"));
  program->addProcedure(std::make_unique<Procedure>("proc"));
  const SemanticAnalysis sa(program);
  const auto procedures = sa.getProcedures();
  const std::map<std::string, std::size_t> std{{"main", 0}, {"proc", 1}};
  ASSERT_EQ(procedures, std);
}

TEST(SemanticAnalysisTest, ErrorHandling1) {
  const auto program = std::make_shared<Program>();
  program->addConstant(std::make_unique<ConstantDefine>("constantA", "valueA"));
  program->addConstant(std::make_unique<ConstantDefine>("constantB", "valueB"));
  program->addProcedure(std::make_unique<Procedure>("proc"));
  std::string msg;
  try {
    const SemanticAnalysis sa(program);
    auto procedures = sa.getProcedures();
  } catch (const SemanticAnalysisException &e) {
    msg = e.what();
  }
  ASSERT_EQ(msg, "No 'main' procedure");
}

TEST(SemanticAnalysisTest, ErrorHandling2) {
  const auto program = std::make_shared<Program>();
  program->addConstant(std::make_unique<ConstantDefine>("constantA", "valueA"));
  program->addConstant(std::make_unique<ConstantDefine>("constantB", "valueB"));
  program->addConstant(std::make_unique<ConstantDefine>("constantB", "valueC"));
  program->addProcedure(std::make_unique<Procedure>("main"));
  program->addProcedure(std::make_unique<Procedure>("proc"));
  std::string msg;
  try {
    const SemanticAnalysis sa(program);
    auto procedures = sa.getProcedures();
  } catch (const SemanticAnalysisException &e) {
    msg = e.what();
  }
  ASSERT_EQ(msg, "Redefine constant: constantB");
}
