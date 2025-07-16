/**
 * @file   visitor.hpp
 * @brief  Visitor 类的声明。
 */

#ifndef VISITOR_HPP
#define VISITOR_HPP

class Program;
class Subprogram;
class Parameter;
class ConstDecl;
class VarDecl;
class Type;
class Variable;
class NullStatement;
class Assign;
class ProcedureCall;
class CompoundStatement;
class If;
class For;
class While;
class Read;
class Write;
class Break;
class Factor;
class Term;
class SimpleExpression;
class Expression;

/**
 * @class Visitor
 * @brief Visitor 类
 * @details Visitor 类，使用访问者模式，用于访问 AST 中的各个结点。
 */
class Visitor {
 public:
  virtual ~Visitor() = default;
  virtual void visit(const Program &program) const = 0;
  virtual void visit(const Subprogram &subprogram) const = 0;
  virtual void visit(const Parameter &parameter) const = 0;
  virtual void visit(const ConstDecl &const_decl) const = 0;
  virtual void visit(const VarDecl &var_decl) const = 0;
  virtual void visit(const Type &type) const = 0;
  virtual void visit(const Variable &variable) const = 0;
  virtual void visit(const NullStatement &null_statement) const = 0;
  virtual void visit(const Assign &assign) const = 0;
  virtual void visit(const ProcedureCall &procedure_call) const = 0;
  virtual void visit(const CompoundStatement &compound_statement) const = 0;
  virtual void visit(const If &if_statement) const = 0;
  virtual void visit(const For &for_statement) const = 0;
  virtual void visit(const While &while_statement) const = 0;
  virtual void visit(const Read &read_statement) const = 0;
  virtual void visit(const Write &write_statement) const = 0;
  virtual void visit(const Break &break_) const = 0;
  virtual void visit(const Factor &factor) const = 0;
  virtual void visit(const Term &term) const = 0;
  virtual void visit(const SimpleExpression &simple_expression) const = 0;
  virtual void visit(const Expression &expression) const = 0;
};
#endif  // VISITOR_HPP
