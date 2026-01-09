/**
 * @file utils.cpp
 * @brief 工具函数的实现。
 */

#include "utils.hpp"

auto Utils::getBasicTypeName(BasicType basic_type) -> std::string {
  switch (basic_type) {
    case BasicType::INTEGER:
      return "int";
    case BasicType::REAL:
      return "float";
    case BasicType::BOOLEAN:
      return "bool";
    case BasicType::CHAR:
      return "char";
  }
  return {};  // this line is unreachable
}

auto Utils::getFactorTypeName(FactorType factor_type) -> std::string {
  switch (factor_type) {
    case FactorType::NUMBER:
      return "Number";
    case FactorType::WITH_UMINUS:
      return "With Uminus";
    case FactorType::WITH_NOT:
      return "With Not";
    case FactorType::VARIABLE:
      return "Variable";
    case FactorType::EXPRESSION:
      return "Expression";
    case FactorType::FUNCTION:
      return "Function";
    case FactorType::BOOLEAN:
      return "Boolean";
    case FactorType::WITH_PLUS:
      return "With Plus";
    case FactorType::UNKNOWN:
      return "Unknown";
  }
  return {};  // this line is unreachable
}

auto Utils::getAddOpName(SimpleExpression::AddOp add_op) -> std::string {
  switch (add_op) {
    case SimpleExpression::AddOp::PLUS:
      return "+";
    case SimpleExpression::AddOp::MINUS:
      return "-";
    case SimpleExpression::AddOp::OR:
      return "||";
  }
  return {};  // this line is unreachable
}

auto Utils::getMulOpName(Term::MulOp mul_op) -> std::string {
  switch (mul_op) {
    case Term::MulOp::TIMES:
      return "*";
    case Term::MulOp::RDIV:
      return "/";
    case Term::MulOp::DIV:
      return "/";
    case Term::MulOp::MOD:
      return "%";
    case Term::MulOp::AND:
      return "&&";
  }

  return {};  // this line is unreachable
}

auto Utils::getRelOpName(Expression::RelOp rel_op) -> std::string {
  switch (rel_op) {
    case Expression::RelOp::GT:
      return ">";
    case Expression::RelOp::EQ:
      return "==";
    case Expression::RelOp::LE:
      return "<=";
    case Expression::RelOp::GE:
      return ">=";
    case Expression::RelOp::LT:
      return "<";
    case Expression::RelOp::NE:
      return "!=";
  }
  return {};  // this line is unreachable
}
