/**
 * @file  printer.cpp
 * @brief Printer 类的实现。
 */

#include "printer.hpp"

#include <iostream>

#include "ast.hpp"

void Printer::visit(const Program& program) const {
  std::cout << "Program:" << std::endl;
  std::cout << " Constants:" << std::endl;
  for (const auto& c : program.getConstants()) {
    std::cout << "  ";
    c->accept(*this);
  }
  std::cout << " Procedures:" << std::endl;
  for (const auto& p : program.getProcedures()) {
    std::cout << "  ";
    p->accept(*this);
  }
}

void Printer::visit(const ConstantDefine& c) const {
  std::cout << c.getId() << "=\"" << c.getValue() << '"' << std::endl;
}

void Printer::visit(const Procedure& procedure) const {
  std::cout << procedure.getId() << ":" << std::endl;
  for (const auto& s : procedure.getStatements()) {
    std::cout << "   ";
    s->accept(*this);
  }
}

void Printer::visit(const Say& say) const {
  std::cout << "Say";
  for (const auto& c : say.getContents()) {
    std::cout << ' ';
    c->accept(*this);
  }
  std::cout << std::endl;
}

void Printer::visit(const Listen& listen) const {
  std::cout << "Listen" << std::endl;
  for (const auto& clause : listen.getClauses()) {
    std::cout << "    ";
    clause->accept(*this);
  }
}

void Printer::visit(const Clause& c) const {
  if (c.getType() == Clause::Type::HAS) {
    std::cout << "Has ";
    c.getPattern()->accept(*this);
    std::cout << ' ';
  } else if (c.getType() == Clause::Type::ANYTHING) {
    std::cout << "Anything ";
  } else if (c.getType() == Clause::Type::TIMEOUT) {
    std::cout << "Timeout ";
  } else {
    std::cout << "Default ";
  }
  if (c.getAction() == Clause::Action::JUMP) {
    std::cout << "Jump to " << c.getBranch();
  } else {
    std::cout << "Write to DB " << c.getDb() << " Table " << c.getTable() << " Column" << c.getColumn();
  }
  std::cout << std::endl;
}

void Printer::visit(const Lookup& l) const {
  std::cout << "Lookup " << l.getValue() << " in " << l.getDb() << ":" << l.getTable() << std::endl;
}

void Printer::visit(const Exit& e) const { std::cout << "Exit" << std::endl; }

void Printer::visit(const Jump& j) const { std::cout << "Jump to " << j.getId() << std::endl; }

void Printer::visit(const Identifier& id) const { std::cout << id.getId(); }

void Printer::visit(const String& s) const { std::cout << '"' << s.getString() << '"'; }
