/**
 * @file   printer.cpp
 * @brief  Printer 类的实现。
 */

#include "printer.hpp"

#include <iostream>
#include <iterator>

#include "utils.hpp"

// ANSI color codes for beautiful output
#define COLOR_NODE "\033[1;34m"  // Bold blue for node types
#define COLOR_ATTR "\033[0;32m"  // Green for attributes
#define COLOR_VAL "\033[0;33m"   // Yellow for values
#define COLOR_RESET "\033[0m"

void Printer::print() const { std::cout << result.str() << std::endl; }

void Printer::clear() const {
  result.str("");
  result.clear();
}

auto Printer::getResult() const -> std::string { return result.str(); }

auto Printer::constructIndentation() const -> std::string {
  std::string indentation;
  for (int i = 0; i < now_level; i++) {
    indentation += (i == now_level - 1) ? "│ " : "  ";
  }
  return indentation;
}

auto Printer::constructString(const std::string &str) const -> std::string {
  std::string indentation;
  for (int i = 0; i < now_level; i++) {
    indentation += (i == now_level - 1) ? "├─ " : "│  ";
  }
  if (use_color) {
    return indentation + COLOR_NODE + str + COLOR_RESET + "\n";
  } else {
    return indentation + str + "\n";
  }
}

auto Printer::constructValue(const std::string &name, const std::string &value) const -> std::string {
  std::string indentation;
  for (int i = 0; i < now_level; i++) {
    indentation += (i == now_level - 1) ? "├─ " : "│  ";
  }
  if (use_color) {
    return indentation + COLOR_ATTR + name + COLOR_RESET + ": " + COLOR_VAL + value + COLOR_RESET + "\n";
  } else {
    return indentation + name + ": " + value + "\n";
  }
}

void Printer::visit(const Program &program) const {
  now_level = 0;
  result << constructString("Program: " + program.getProgramId());
  now_level++;

  // Parameters
  auto params = program.getParameters();
  if (!params.empty()) {
    std::ostringstream paramStream;
    std::copy(params.begin(), params.end() - 1, std::ostream_iterator<std::string>(paramStream, ", "));
    paramStream << params.back();
    std::string paramStr = paramStream.str();
    result << constructValue("Parameters", paramStr);
  }

  // Const declarations
  if (!program.getConstDeclarations().empty()) {
    result << constructString("Const Declarations");
    now_level++;
    for (const auto &const_decl : program.getConstDeclarations()) {
      const_decl->accept(*this);
    }
    now_level--;
  }

  // Var declarations
  if (!program.getVarDeclarations().empty()) {
    result << constructString("Var Declarations");
    now_level++;
    for (const auto &var_decl : program.getVarDeclarations()) {
      var_decl->accept(*this);
    }
    now_level--;
  }

  // Subprograms
  if (!program.getSubprograms().empty()) {
    result << constructString("Subprograms");
    now_level++;
    for (const auto &subprogram : program.getSubprograms()) {
      subprogram->accept(*this);
    }
    now_level--;
  }

  result << constructString("Body");
  program.getCompoundStatement()->accept(*this);
  now_level--;
}

void Printer::visit(const Subprogram &subprogram) const {
  result << constructString("Subprogram: " + subprogram.getId());
  now_level++;

  // Parameters
  if (!subprogram.getParameters().empty()) {
    result << constructString("Parameters");
    now_level++;
    for (const auto &param : subprogram.getParameters()) {
      param->accept(*this);
    }
    now_level--;
  }

  // Return type
  std::string return_type =
      subprogram.isFunction() ? Utils::getBasicTypeName(subprogram.getReturnType()) : "None (procedure)";
  result << constructValue("Return Type", return_type);

  // Const declarations
  if (!subprogram.getConstDeclarations().empty()) {
    result << constructString("Const Declarations");
    now_level++;
    for (const auto &const_decl : subprogram.getConstDeclarations()) {
      const_decl->accept(*this);
    }
    now_level--;
  }

  // Var declarations
  if (!subprogram.getVarDeclarations().empty()) {
    result << constructString("Var Declarations");
    now_level++;
    for (const auto &var_decl : subprogram.getVarDeclarations()) {
      var_decl->accept(*this);
    }
    now_level--;
  }

  // Body
  result << constructString("Body");
  subprogram.getCompoundStatement()->accept(*this);
  now_level--;
}

void Printer::visit(const Parameter &parameter) const {
  std::string paramType = parameter.isVar() ? "var " : "";
  auto ids = parameter.getIdList();

  std::ostringstream oss;
  std::copy(ids.begin(), ids.end() - 1, std::ostream_iterator<std::string>(oss, ", "));
  oss << ids.back();
  const auto idList = oss.str();

  result << constructString(paramType + idList + ": " + Utils::getBasicTypeName(parameter.getBasicType()));
}

void Printer::visit(const ConstDecl &const_decl) const {
  result << constructString(const_decl.getId() + " = " + const_decl.getValue());
}

void Printer::visit(const VarDecl &var_decl) const {
  auto ids = var_decl.getIdList();

  std::ostringstream oss;
  std::copy(ids.begin(), ids.end() - 1, std::ostream_iterator<std::string>(oss, ", "));
  oss << ids.back();
  const auto idList = oss.str();

  result << constructString(idList);
  now_level++;
  var_decl.getType()->accept(*this);
  now_level--;
}

void Printer::visit(const Type &type) const {
  std::string typeStr = Utils::getBasicTypeName(type.getBasicType());

  if (type.getPeriods().empty()) {
    result << constructValue("Type", typeStr);
  } else {
    result << constructString(typeStr + " Array");
    now_level++;
    for (const auto &period : type.getPeriods()) {
      result << constructString("Range: " + period.first + ".." + period.second);
    }
    now_level--;
  }
}

void Printer::visit(const Variable &variable) const {
  std::string varStr = "Variable: " + variable.getId();
  if (!variable.getExpressions().empty()) {
    varStr += " [array]";
  }
  result << constructString(varStr);

  if (!variable.getExpressions().empty()) {
    now_level++;
    result << constructString("Indices");
    now_level++;
    for (const auto &expression : variable.getExpressions()) {
      expression->accept(*this);
    }
    now_level--;
    now_level--;
  }
}

void Printer::visit(const NullStatement &null_statement) const { result << constructString("NullStatement"); }

void Printer::visit(const Assign &assign) const {
  result << constructString("Assignment");
  now_level++;

  // Left side
  result << constructString("Left");
  now_level++;
  assign.getLeft()->accept(*this);
  now_level--;

  // Right side
  result << constructString("Right");
  now_level++;
  assign.getRight()->accept(*this);
  now_level--;

  now_level--;
}

void Printer::visit(const ProcedureCall &procedure_call) const {
  std::string procStr = "Call: " + procedure_call.getId();
  if (procedure_call.getParameters().empty()) {
    procStr += " (no params)";
  }
  result << constructString(procStr);

  if (!procedure_call.getParameters().empty()) {
    now_level++;
    result << constructString("Parameters");
    now_level++;
    for (const auto &param : procedure_call.getParameters()) {
      param->accept(*this);
    }
    now_level--;
    now_level--;
  }
}

void Printer::visit(const CompoundStatement &compound_statement) const {
  result << constructString("Compound {");
  now_level++;
  for (const auto &statement : compound_statement.getStatements()) {
    statement->accept(*this);
  }
  now_level--;
  result << constructString("}");
}

void Printer::visit(const If &if_statement) const {
  result << constructString("If");
  now_level++;

  result << constructString("Condition");
  now_level++;
  if_statement.getCondition()->accept(*this);
  now_level--;

  result << constructString("Then");
  now_level++;
  if_statement.getThenStatement()->accept(*this);
  now_level--;

  if (if_statement.getElseStatement()) {
    result << constructString("Else");
    now_level++;
    if_statement.getElseStatement()->accept(*this);
    now_level--;
  }

  now_level--;
}

void Printer::visit(const For &for_statement) const {
  result << constructString("For: " + for_statement.getId());
  now_level++;

  result << constructString("From");
  now_level++;
  for_statement.getLowerBound()->accept(*this);
  now_level--;

  result << constructString("To");
  now_level++;
  for_statement.getUpperBound()->accept(*this);
  now_level--;

  result << constructString("Do");
  now_level++;
  for_statement.getStatement()->accept(*this);
  now_level--;

  now_level--;
}

void Printer::visit(const While &while_statement) const {
  result << constructString("While");
  now_level++;

  result << constructString("Condition");
  now_level++;
  while_statement.getCondition()->accept(*this);
  now_level--;

  result << constructString("Do");
  now_level++;
  while_statement.getStatement()->accept(*this);
  now_level--;

  now_level--;
}

void Printer::visit(const Read &read_statement) const {
  result << constructString("Read");
  now_level++;

  for (const auto &variable : read_statement.getVariables()) {
    variable->accept(*this);
  }

  now_level--;
}

void Printer::visit(const Write &write_statement) const {
  result << constructString("Write");
  now_level++;

  for (const auto &expression : write_statement.getExpressions()) {
    expression->accept(*this);
  }

  now_level--;
}

void Printer::visit(const Break &break_) const { result << constructString("Break"); }

void Printer::visit(const Factor &factor) const {
  std::string factorStr = "Factor: " + Utils::getFactorTypeName(factor.getType());

  if (factor.getType() == FactorType::NUMBER) {
    factorStr += " (" + std::get<std::string>(factor.getValue()) + ")";
  }

  result << constructString(factorStr);

  now_level++;
  if (factor.getType() == FactorType::UNKNOWN || factor.getType() == FactorType::NUMBER) {
    result << constructValue("Value", std::get<std::string>(factor.getValue()));
  } else {
    result << constructString("Value");
    now_level++;
    std::get<ASTNodePtr>(factor.getValue())->accept(*this);
    now_level--;
  }
  now_level--;
}

void Printer::visit(const Term &term) const {
  result << constructString("Term");
  now_level++;

  term.getFirstFactor()->accept(*this);

  for (const auto &pair : term.getFactors()) {
    result << constructString("Op: " + Utils::getMulOpName(pair.first));
    now_level++;
    pair.second->accept(*this);
    now_level--;
  }

  now_level--;
}

void Printer::visit(const SimpleExpression &simple_expression) const {
  result << constructString("SimpleExpression");
  now_level++;

  simple_expression.getFirstTerm()->accept(*this);

  for (const auto &pair : simple_expression.getTerms()) {
    result << constructString("Op: " + Utils::getAddOpName(pair.first));
    now_level++;
    pair.second->accept(*this);
    now_level--;
  }

  now_level--;
}

void Printer::visit(const Expression &expression) const {
  std::string exprStr = "Expression";
  if (expression.getRight()) {
    exprStr += " [" + Utils::getRelOpName(expression.getRight()->first) + "]";
  }
  result << constructString(exprStr);

  now_level++;
  result << constructString("Left");
  now_level++;
  expression.getLeft()->accept(*this);
  now_level--;

  if (expression.getRight()) {
    result << constructString("Right");
    now_level++;
    expression.getRight()->second->accept(*this);
    now_level--;
  }
  now_level--;
}