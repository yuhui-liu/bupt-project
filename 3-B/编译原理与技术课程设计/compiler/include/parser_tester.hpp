/**
 * @file   parser_tester.hpp
 * @brief  ParserTester 类的声明。
 */

#ifndef PARSER_TESTER_HPP
#define PARSER_TESTER_HPP

#include <string>

#include "ast.hpp"
#include "visitor.hpp"

/**
 * @class ParserTester
 * @brief ParserTester 类
 * @details ParserTester 类，继承自 Visitor 类，用于打印 AST 中的各个结点。
 */
class ParserTester final : public Visitor {
 public:
  auto getResult() const -> std::vector<std::string>;
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
  /// @brief 结果字符串
  mutable std::vector<std::string> result;
};

#endif  // PARSER_TESTER_HPP