/**
 * @file  semantic_analysis.hpp
 * @brief SemanticAnalysis 类的声明。
 *
 */

#ifndef SEMANTIC_ANALYSIS_HPP
#define SEMANTIC_ANALYSIS_HPP

#include <cstddef>
#include <map>
#include <memory>
#include <string>

#include "ast.hpp"

/**
 * @class SemanticAnalysis
 * @brief 语义分析类。分析程序的语义。
 *
 * 检查是否有重复定义的常量和过程；检查main过程是否存在。
 */
class SemanticAnalysis {
 public:
  explicit SemanticAnalysis(const std::shared_ptr<Program>&);
  /// @returns 构造出的常量表。
  [[nodiscard]] auto getConstants() const -> std::map<std::string, std::string>;
  /// @returns 构造出的过程表。
  [[nodiscard]] auto getProcedures() const -> std::map<std::string, std::size_t>;

 private:
  /// @brief 常量表。
  std::map<std::string, std::string> constants;
  /// @brief 过程表。
  std::map<std::string, std::size_t> procedures;
};

#endif  // SEMANTIC_ANALYSIS_HPP
