/**
 * @file utils.hpp
 * @brief 工具函数的声明。
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include "ast.hpp"

class Utils {
 public:
  /// @brief 获取基本类型名称
  /// @param basic_type 基本类型
  /// @return 基本类型名称
  static auto getBasicTypeName(BasicType basic_type) -> std::string;
  static auto getFactorTypeName(FactorType factor_type) -> std::string;
  static auto getAddOpName(SimpleExpression::AddOp add_op) -> std::string;
  static auto getMulOpName(Term::MulOp mul_op) -> std::string;
  static auto getRelOpName(Expression::RelOp rel_op) -> std::string;
};

#endif  // UTILS_HPP