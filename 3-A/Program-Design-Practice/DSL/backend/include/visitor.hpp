/**
 * @file   visitor.hpp
 * @brief  Visitor 类的声明。
 */

#ifndef VISITOR_HPP
#define VISITOR_HPP

class Program;
class ConstantDefine;
class Procedure;
class Say;
class Listen;
class Clause;
class Lookup;
class Exit;
class Jump;
class Identifier;
class String;

/**
 * @class Visitor
 * @brief Visitor 类
 * @details Visitor 类，使用访问者模式，用于访问 AST 中的各个结点。
 */
class Visitor {
 public:
  virtual ~Visitor() = default;
  virtual void visit(const Program &) const = 0;
  virtual void visit(const ConstantDefine &) const = 0;
  virtual void visit(const Procedure &) const = 0;
  virtual void visit(const Say &) const = 0;
  virtual void visit(const Listen &) const = 0;
  virtual void visit(const Clause &) const = 0;
  virtual void visit(const Lookup &) const = 0;
  virtual void visit(const Exit &) const = 0;
  virtual void visit(const Jump &) const = 0;
  virtual void visit(const Identifier &) const = 0;
  virtual void visit(const String &) const = 0;
};
#endif  // VISITOR_HPP
