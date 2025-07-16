/**
 * @file   ast.cpp
 * @brief  AST各个结点的实现，包括adders & setters和acceptors
 */

#include "ast.hpp"

#include <vector>

#include "visitor.hpp"

/*
本文件中的方法都是简单易懂的，不再一一注释。
*/

// acceptors
void Program::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Subprogram::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Parameter::accept(const Visitor& visitor) const { visitor.visit(*this); }

void ConstDecl::accept(const Visitor& visitor) const { visitor.visit(*this); }

void VarDecl::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Type::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Variable::accept(const Visitor& visitor) const { visitor.visit(*this); }

void NullStatement::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Assign::accept(const Visitor& visitor) const { visitor.visit(*this); }

void ProcedureCall::accept(const Visitor& visitor) const { visitor.visit(*this); }

void CompoundStatement::accept(const Visitor& visitor) const { visitor.visit(*this); }

void If::accept(const Visitor& visitor) const { visitor.visit(*this); }

void For::accept(const Visitor& visitor) const { visitor.visit(*this); }

void While::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Read::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Write::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Break::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Factor::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Term::accept(const Visitor& visitor) const { visitor.visit(*this); }

void SimpleExpression::accept(const Visitor& visitor) const { visitor.visit(*this); }

void Expression::accept(const Visitor& visitor) const { visitor.visit(*this); }

// getters
auto Program::getProgramId() const -> const std::string& { return program_id; }

auto Program::getParameters() const -> const std::vector<std::string>& { return parameters; }

auto Program::getConstDeclarations() const -> const std::vector<ConstDeclPtr>& { return const_declaration; }

auto Program::getVarDeclarations() const -> const std::vector<VarDeclPtr>& { return var_declaration; }

auto Program::getSubprograms() const -> const std::vector<SubprogramPtr>& { return subprograms; }

auto Program::getCompoundStatement() const -> const CompoundStatementPtr& { return compound_statement; }

auto Subprogram::getId() const -> const std::string& { return id; }

auto Subprogram::getParameters() const -> const std::vector<ParameterPtr>& { return parameters; }

auto Subprogram::getReturnType() const -> const BasicType& { return ret_type; }

auto Subprogram::isFunction() const -> bool { return is_func; }

auto Subprogram::getConstDeclarations() const -> const std::vector<ConstDeclPtr>& { return const_declaration; }

auto Subprogram::getVarDeclarations() const -> const std::vector<VarDeclPtr>& { return var_declaration; }

auto Subprogram::getCompoundStatement() const -> const CompoundStatementPtr& { return compound_statement; }

auto Parameter::isVar() const -> bool { return has_var; }

auto Parameter::getIdList() const -> const std::vector<std::string>& { return id_list; }

auto Parameter::getBasicType() const -> const BasicType& { return basic_type; }

auto ConstDecl::getId() const -> const std::string& { return id; }

auto ConstDecl::getValue() const -> const std::string& { return value; }

auto VarDecl::getIdList() const -> const std::vector<std::string>& { return id_list; }

auto VarDecl::getType() const -> const TypePtr& { return type; }

auto Type::getPeriods() const -> const std::vector<std::pair<std::string, std::string>>& { return periods; }

auto Type::getBasicType() const -> const BasicType& { return basic_type; }

auto Variable::getId() const -> const std::string& { return id; }

auto Variable::getExpressions() const -> const std::vector<ExpressionPtr>& { return expressions; }

auto Assign::getLeft() const -> const VariablePtr& { return left; }

auto Assign::getRight() const -> const ExpressionPtr& { return right; }

auto ProcedureCall::getId() const -> const std::string& { return id; }

auto ProcedureCall::getParameters() const -> const std::vector<ExpressionPtr>& { return parameters; }

auto CompoundStatement::getStatements() const -> const std::vector<StatementPtr>& { return statements; }

auto If::getCondition() const -> const ExpressionPtr& { return condition; }

auto If::getThenStatement() const -> const StatementPtr& { return then_statement; }

auto If::getElseStatement() const -> const StatementPtr& { return else_statement; }

auto For::getId() const -> const std::string& { return id; }

auto For::getLowerBound() const -> const ExpressionPtr& { return lb; }

auto For::getUpperBound() const -> const ExpressionPtr& { return ub; }

auto For::getStatement() const -> const StatementPtr& { return statement; }

auto While::getCondition() const -> const ExpressionPtr& { return condition; }

auto While::getStatement() const -> const StatementPtr& { return statement; }

auto Read::getVariables() const -> const std::vector<VariablePtr>& { return variables; }

auto Write::getExpressions() const -> const std::vector<ExpressionPtr>& { return expressions; }

auto Factor::getType() const -> const FactorType& { return type; }

auto Factor::getValue() const -> const std::variant<std::string, ASTNodePtr>& { return value; }

auto Term::getFirstFactor() const -> const FactorPtr& { return firstFactor; }

auto Term::getFactors() const -> const std::vector<std::pair<MulOp, FactorPtr>>& { return factors; }

auto SimpleExpression::getFirstTerm() const -> const TermPtr& { return firstTerm; }

auto SimpleExpression::getTerms() const -> const std::vector<std::pair<AddOp, TermPtr>>& { return terms; }

auto Expression::getLeft() const -> const SimpleExpressionPtr& { return left; }

auto Expression::getRight() const -> const std::optional<std::pair<RelOp, SimpleExpressionPtr>>& { return right; }

// setters & adders
void Program::addParameter(const std::string& parameter) { parameters.push_back(parameter); }

void Program::addConstDeclaration(ConstDeclPtr const_decl) { const_declaration.push_back(std::move(const_decl)); }

void Program::addVarDeclaration(VarDeclPtr var_decl) { var_declaration.push_back(std::move(var_decl)); }

void Program::addSubprogram(SubprogramPtr subprogram) { subprograms.push_back(std::move(subprogram)); }

void Program::setCompoundStatement(CompoundStatementPtr cp) { this->compound_statement = std::move(cp); }

void Subprogram::addParameter(ParameterPtr parameter) { parameters.push_back(std::move(parameter)); }

void Subprogram::setReturnType(BasicType ret_type) { this->ret_type = ret_type; }

void Subprogram::addConstDeclaration(ConstDeclPtr const_decl) { const_declaration.push_back(std::move(const_decl)); }

void Subprogram::addVarDeclaration(VarDeclPtr var_decl) { var_declaration.push_back(std::move(var_decl)); }

void Subprogram::setCompoundStatement(CompoundStatementPtr cp) { this->compound_statement = std::move(cp); }

void Parameter::addId(const std::string& id) { id_list.push_back(id); }

void Parameter::setBasicType(BasicType basic_type) { this->basic_type = basic_type; }

void VarDecl::addId(const std::string& id) { id_list.push_back(id); }

void VarDecl::setType(TypePtr type) { this->type = std::move(type); }

void Type::addPeriod(const std::string& lower, const std::string& upper) { periods.emplace_back(lower, upper); }

void Type::setBasicType(BasicType basic_type) { this->basic_type = basic_type; }

void Variable::addExpression(ExpressionPtr expression) { expressions.push_back(std::move(expression)); }

void ProcedureCall::addParameter(ExpressionPtr parameter) { parameters.push_back(std::move(parameter)); }

void CompoundStatement::addStatement(StatementPtr statement) { statements.push_back(std::move(statement)); }

void For::setLowerBound(ExpressionPtr lb) { this->lb = std::move(lb); }

void For::setUpperBound(ExpressionPtr ub) { this->ub = std::move(ub); }

void For::setStatement(StatementPtr statement) { this->statement = std::move(statement); }

void Read::addVariable(VariablePtr variable) { variables.push_back(std::move(variable)); }

void Write::addExpression(ExpressionPtr expression) { expressions.push_back(std::move(expression)); }

void Term::setFirstFactor(FactorPtr firstFactor) { this->firstFactor = std::move(firstFactor); }

void Term::addFactor(MulOp op, FactorPtr factor) { factors.emplace_back(op, std::move(factor)); }

void SimpleExpression::setFirstTerm(TermPtr firstTerm) { this->firstTerm = std::move(firstTerm); }

void SimpleExpression::addTerm(AddOp op, TermPtr term) { terms.emplace_back(op, std::move(term)); }

void Expression::setLeft(SimpleExpressionPtr left) { this->left = std::move(left); }

void Expression::setRight(RelOp op, SimpleExpressionPtr right) { this->right = std::make_pair(op, std::move(right)); }

auto Factor::checkSame(const Factor* target) const -> bool {
  if (type != target->getType()) return false;
  if (value.index() != target->getValue().index()) return false;
  if (value.index() == 0) {
    if (std::get<std::string>(value) != std::get<std::string>(target->getValue())) return false;
  } else {
    ASTNode* value_ptr = std::get<ASTNodePtr>(value).get();
    ASTNode* target_ptr = std::get<ASTNodePtr>(target->getValue()).get();

    ProcedureCall* value_call = dynamic_cast<ProcedureCall*>(value_ptr);
    ProcedureCall* target_call = dynamic_cast<ProcedureCall*>(target_ptr);
    auto both_call = value_call && target_call;

    Expression* value_exp = dynamic_cast<Expression*>(value_ptr);
    Expression* target_exp = dynamic_cast<Expression*>(target_ptr);
    auto both_exp = value_exp && target_exp;

    Variable* value_var = dynamic_cast<Variable*>(value_ptr);
    Variable* target_var = dynamic_cast<Variable*>(target_ptr);
    auto both_var = value_var && target_var;

    Factor* value_fac = dynamic_cast<Factor*>(value_ptr);
    Factor* target_fac = dynamic_cast<Factor*>(target_ptr);
    auto both_fac = value_fac && target_fac;

    if (both_call) {
      if (!value_call->checkSame(target_call)) return false;
    } else if (both_exp) {
      if (!value_exp->checkSame(target_exp)) return false;
    } else if (both_var) {
      if (!value_var->checkSame(target_var)) return false;
    } else if (both_fac) {
      if (!value_fac->checkSame(target_fac)) return false;
    } else {
      return false;
    }
  }

  return true;
}

auto Term::checkSame(const Term* target) const -> bool {
  if (!firstFactor->checkSame(target->getFirstFactor().get())) return false;
  const auto& target_factors = target->getFactors();
  if (factors.size() != target_factors.size()) return false;
  for (int i = 0; i < factors.size(); i++) {
    if (factors[i].first != target_factors[i].first) return false;
    if (!factors[i].second->checkSame(target_factors[i].second.get())) return false;
  }

  return true;
}

auto SimpleExpression::checkSame(const SimpleExpression* target) const -> bool {
  if (!firstTerm->checkSame(target->getFirstTerm().get())) return false;

  const auto& target_terms = target->getTerms();
  if (terms.size() != target_terms.size()) return false;
  for (int i = 0; i < terms.size(); i++) {
    if (terms[i].first != target_terms[i].first) return false;
    if (!terms[i].second->checkSame(target_terms[i].second.get())) return false;
  }

  return true;
}

auto Expression::checkSame(const Expression* target) const -> bool {
  if (!left->checkSame(target->getLeft().get())) return false;

  auto both = right.has_value() == target->getRight().has_value();
  if (!both) return false;
  if (!right.has_value()) return true;

  auto first_eq = right.value().first == target->getRight().value().first;
  auto second_eq = right.value().second->checkSame(target->getRight().value().second.get());

  return first_eq && second_eq;
}

bool ProcedureCall::checkSame(const ProcedureCall* target) const {
  if (id != target->getId()) return false;

  const auto& target_parameters = target->getParameters();
  if (parameters.size() != target_parameters.size()) return false;
  for (int i = 0; i < parameters.size(); i++)
    if (parameters[i]->checkSame(target_parameters[i].get()) == false) return false;

  return true;
}

bool Variable::checkSame(const Variable* target) const {
  if (id != target->getId()) return false;

  const auto& target_expressions = target->getExpressions();
  if (expressions.size() != target_expressions.size()) return false;
  for (int i = 0; i < expressions.size(); i++)
    if (!expressions[i]->checkSame(target_expressions[i].get())) return false;

  return true;
}

bool Parameter::checkSame(const Parameter* target) const {
  if (has_var != target->isVar()) return false;
  if (basic_type != target->getBasicType()) return false;

  auto target_IdList = target->getIdList();
  if (id_list.size() != target_IdList.size()) return false;
  for (int i = 0; i < id_list.size(); i++)
    if (id_list[i] != target_IdList[i]) return false;

  return true;
}