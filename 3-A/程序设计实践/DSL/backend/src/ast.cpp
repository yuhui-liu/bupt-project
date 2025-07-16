/**
 * @file   ast.cpp
 * @brief  AST各个结点的实现，包括adders & setters和acceptors
 */

#include "ast.hpp"

#include <vector>

#include "visitor.hpp"

// adders & setters
void Program::addConstant(ASTNodePtr constant) { constants.push_back(std::move(constant)); }

void Program::addProcedure(ASTNodePtr procedure) { procedures.push_back(std::move(procedure)); }

void Procedure::addStatement(ASTNodePtr statement) { statements.push_back(std::move(statement)); }

void Say::addContent(ASTNodePtr content) { this->content.push_back(std::move(content)); }

void Listen::addClause(ASTNodePtr clause) { this->clauses.push_back(std::move(clause)); }

void Clause::setType(const Type type) { this->type = type; }

void Clause::setAction(const Action action) { this->action = action; }

void Clause::setBranch(const std::string& branch) { this->branch = branch; }

void Clause::setPattern(ASTNodePtr pattern) { this->pattern = std::move(pattern); }

void Clause::setDb(const std::string& db) { this->db = db; }

void Clause::setTable(const std::string& table) { this->table = table; }

void Clause::setColumn(const std::string& column) { this->column = column; }

// acceptors
void Program::accept(const Visitor& visitor) const { visitor.visit(*this); }

void ConstantDefine::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Procedure::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Say::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Listen::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Clause::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Lookup::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Exit::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Jump::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Identifier::accept(const Visitor& visitor) const { visitor.visit(*this); }

void String::accept(const Visitor& visitor) const { visitor.visit(*this); }
