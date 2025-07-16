/**
 * @file   printer.hpp
 * @brief  Printer 类的声明。
 */

#ifndef PRINTER_HPP
#define PRINTER_HPP

#include "visitor.hpp"

/**
 * @class Printer
 * @brief 打印AST的访问者类
 */
class Printer final : public Visitor {
 public:
  void visit(const Program &) const override;
  void visit(const ConstantDefine &) const override;
  void visit(const Procedure &) const override;
  void visit(const Say &) const override;
  void visit(const Listen &) const override;
  void visit(const Clause &) const override;
  void visit(const Lookup &) const override;
  void visit(const Exit &) const override;
  void visit(const Jump &) const override;
  void visit(const Identifier &) const override;
  void visit(const String &) const override;
};

#endif  // PRINTER_HPP
