/**
 * @file   parser_tester.cpp
 * @brief  ParserTester 类的实现。
 */

#include "parser_tester.hpp"

#include "utils.hpp"
auto ParserTester::getResult() const -> std::vector<std::string> { return result; }
void ParserTester::visit(const Program &program) const {
  result.emplace_back("Program");
  result.emplace_back(program.getProgramId());
  result.emplace_back("Parameters");
  for (const auto &param : program.getParameters()) {
    result.emplace_back(param);
  }
  result.emplace_back("Const Declarations");
  for (const auto &const_decl : program.getConstDeclarations()) {
    const_decl->accept(*this);
  }
  result.emplace_back("Var Declarations");
  for (const auto &var_decl : program.getVarDeclarations()) {
    var_decl->accept(*this);
  }
  result.emplace_back("Subprograms");
  for (const auto &subprogram : program.getSubprograms()) {
    subprogram->accept(*this);
  }
  result.emplace_back("Body");
  program.getCompoundStatement()->accept(*this);
}

void ParserTester::visit(const Subprogram &subprogram) const {
  result.emplace_back("Subprogram");
  result.emplace_back(subprogram.getId());
  result.emplace_back("Parameters");
  for (const auto &param : subprogram.getParameters()) {
    param->accept(*this);
  }
  result.emplace_back("Return Type");
  result.emplace_back(Utils::getBasicTypeName(subprogram.getReturnType()));
  if (subprogram.isFunction()) {
    result.emplace_back("Function");
  } else {
    result.emplace_back("Procedure");
  }
  result.emplace_back("Const Declarations");
  for (const auto &const_decl : subprogram.getConstDeclarations()) {
    const_decl->accept(*this);
  }
  result.emplace_back("Var Declarations");
  for (const auto &var_decl : subprogram.getVarDeclarations()) {
    var_decl->accept(*this);
  }
  result.emplace_back("Body");
  subprogram.getCompoundStatement()->accept(*this);
}

void ParserTester::visit(const Parameter &parameter) const {
  result.emplace_back("Parameter");
  result.emplace_back(parameter.isVar() ? "var" : "value");
  result.emplace_back(Utils::getBasicTypeName(parameter.getBasicType()));
  for (const auto &id : parameter.getIdList()) {
    result.emplace_back(id);
  }
}

void ParserTester::visit(const ConstDecl &const_decl) const {
  result.emplace_back("Const Declaration");
  result.emplace_back(const_decl.getId());
  result.emplace_back(const_decl.getValue());
}

void ParserTester::visit(const VarDecl &var_decl) const {
  result.emplace_back("Var Declaration");
  for (const auto &id : var_decl.getIdList()) {
    result.emplace_back(id);
  }
  var_decl.getType()->accept(*this);
}

void ParserTester::visit(const Type &type) const {
  result.emplace_back("Type");
  result.emplace_back(Utils::getBasicTypeName(type.getBasicType()));
  if (!type.getPeriods().empty()) {
    for (const auto &period : type.getPeriods()) {
      result.emplace_back(period.first + ".." + period.second);
    }
  }
}

void ParserTester::visit(const Variable &variable) const {
  result.emplace_back("Variable");
  result.emplace_back(variable.getId());
  for (const auto &expr : variable.getExpressions()) {
    expr->accept(*this);
  }
}

void ParserTester::visit(const NullStatement &null_statement) const { result.emplace_back("Null Statement"); }

void ParserTester::visit(const Assign &assign) const {
  result.emplace_back("Assignment");
  result.emplace_back("Left");
  assign.getLeft()->accept(*this);
  result.emplace_back("Right");
  assign.getRight()->accept(*this);
}

void ParserTester::visit(const ProcedureCall &procedure_call) const {
  result.emplace_back("Procedure Call");
  result.emplace_back(procedure_call.getId());
  for (const auto &param : procedure_call.getParameters()) {
    param->accept(*this);
  }
}

void ParserTester::visit(const CompoundStatement &compound_statement) const {
  result.emplace_back("Compound Statement");
  for (const auto &statement : compound_statement.getStatements()) {
    statement->accept(*this);
  }
}

void ParserTester::visit(const If &if_statement) const {
  result.emplace_back("If Statement");
  result.emplace_back("Condition");
  if_statement.getCondition()->accept(*this);
  result.emplace_back("Then");
  if_statement.getThenStatement()->accept(*this);
  if (if_statement.getElseStatement()) {
    result.emplace_back("Else");
    if_statement.getElseStatement()->accept(*this);
  }
}

void ParserTester::visit(const For &for_statement) const {
  result.emplace_back("For Statement");
  result.emplace_back(for_statement.getId());
  result.emplace_back("From");
  for_statement.getLowerBound()->accept(*this);
  result.emplace_back("To");
  for_statement.getUpperBound()->accept(*this);
  result.emplace_back("Body");
  for_statement.getStatement()->accept(*this);
}

void ParserTester::visit(const While &while_statement) const {
  result.emplace_back("While Statement");
  result.emplace_back("Condition");
  while_statement.getCondition()->accept(*this);
  result.emplace_back("Body");
  while_statement.getStatement()->accept(*this);
}

void ParserTester::visit(const Read &read_statement) const {
  result.emplace_back("Read Statement");
  for (const auto &variable : read_statement.getVariables()) {
    variable->accept(*this);
  }
}

void ParserTester::visit(const Write &write_statement) const {
  result.emplace_back("Write Statement");
  for (const auto &expression : write_statement.getExpressions()) {
    expression->accept(*this);
  }
}

void ParserTester::visit(const Break &break_) const { result.emplace_back("Break Statement"); }

void ParserTester::visit(const Factor &factor) const {
  result.emplace_back("Factor");
  result.emplace_back(Utils::getFactorTypeName(factor.getType()));
  if (std::holds_alternative<std::string>(factor.getValue())) {
    result.emplace_back(std::get<std::string>(factor.getValue()));
  } else {
    std::get<ASTNodePtr>(factor.getValue())->accept(*this);
  }
}

void ParserTester::visit(const Term &term) const {
  result.emplace_back("Term");
  term.getFirstFactor()->accept(*this);
  for (const auto &pair : term.getFactors()) {
    result.emplace_back(Utils::getMulOpName(pair.first));
    pair.second->accept(*this);
  }
}

void ParserTester::visit(const SimpleExpression &simple_expression) const {
  result.emplace_back("Simple Expression");
  simple_expression.getFirstTerm()->accept(*this);
  for (const auto &pair : simple_expression.getTerms()) {
    result.emplace_back(Utils::getAddOpName(pair.first));
    pair.second->accept(*this);
  }
}

void ParserTester::visit(const Expression &expression) const {
  result.emplace_back("Expression");
  expression.getLeft()->accept(*this);
  if (expression.getRight()) {
    result.emplace_back(Utils::getRelOpName(expression.getRight()->first));
    expression.getRight()->second->accept(*this);
  }
}