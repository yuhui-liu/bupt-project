/**
 * @file   printer.hpp
 * @brief  Printer 类的声明。
 */

#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <sstream>
#include <string>

#include "visitor.hpp"

/**
 * @class Printer
 * @brief Printer 类
 * @details Printer 类，继承自 Visitor 类，用于打印 AST 中的各个结点。
 */
class Printer final : public Visitor {
 public:
  /// @brief 构造函数
  Printer() = default;
  /// @brief 构造函数
  Printer(bool use_color) : use_color(use_color) {}
  /// @brief 打印 AST
  void print() const;
  /// @brief 获取打印结果
  auto getResult() const -> std::string;
  /// @brief 清空打印结果
  void clear() const;
  void visit(const Program &program) const override;
  void visit(const Subprogram &subprogram) const override;
  void visit(const Parameter &parameter) const override;
  void visit(const ConstDecl &const_decl) const override;
  void visit(const VarDecl &var_decl) const override;
  void visit(const Type &type) const override;
  void visit(const Variable &variable) const override;
  void visit(const NullStatement &null_statement) const override;
  void visit(const Assign &assign) const override;
  void visit(const ProcedureCall &procedure_call) const override;
  void visit(const CompoundStatement &compound_statement) const override;
  void visit(const If &if_statement) const override;
  void visit(const For &for_statement) const override;
  void visit(const While &while_statement) const override;
  void visit(const Read &read_statement) const override;
  void visit(const Write &write_statement) const override;
  void visit(const Break &break_) const override;
  void visit(const Factor &factor) const override;
  void visit(const Term &term) const override;
  void visit(const SimpleExpression &simple_expression) const override;
  void visit(const Expression &expression) const override;

 private:
  /// @brief 表示结果的字符串流
  mutable std::ostringstream result;
  /// @brief 当前缩进级别
  mutable int now_level = 0;
  /// @brief 构造空白缩进字符串
  /// @return 空白缩进字符串
  auto constructIndentation() const -> std::string;
  /// @brief 构造含有缩进的字符串
  /// @param str 字符串
  /// @return 含有缩进的字符串
  auto constructString(const std::string &str) const -> std::string;
  /// @brief 构造值字符串
  /// @param name 名称
  /// @param value 值
  /// @return 值字符串
  auto constructValue(const std::string &name, const std::string &value) const -> std::string;
  /// @brief 是否使用颜色输出
  bool use_color = true;
};
#endif  // PRINTER_HPP
