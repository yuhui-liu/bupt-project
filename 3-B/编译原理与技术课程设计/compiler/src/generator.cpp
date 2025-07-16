/**
 * @file generator.cpp
 * @brief Generator 类的实现。
 */

#include "generator.hpp"

#include <algorithm>
#include <iostream>
#include <string>

#include "utils.hpp"

void Generator::print() const { std::cout << result << std::endl; }

void Generator::clear() { result = ""; }

auto Generator::getResult() const -> std::string { return result; }

void Generator::addIndent() const {
  for (int i = 1; i <= now_level; i++) result += "  ";
}

void Generator::visit(const Program &program) const {
  now_level = 0;
  // Program.str="#include<stdio.h>\n"+ConstDecl.str+VarDecl.str+subprogram.str+"int main(){\n"+Statement.str+"}\n"
  result += R"(#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
)";

  for (const auto &const_decl : program.getConstDeclarations()) {
    const_decl->accept(*this);
  }

  // Var declarations
  for (const auto &var_decl : program.getVarDeclarations()) {
    var_decl->accept(*this);
  }

  // Subprograms
  for (const auto &var_decl : program.getSubprograms()) {
    var_decl->accept(*this);
  }
  result += "int main()\n";

  // Compoundstatement
  main_compound_statement = program.getCompoundStatement().get();
  now_in_which_subprogram = "main";
  program.getCompoundStatement()->accept(*this);
}

void Generator::visit(const Subprogram &subprogram) const {
  // Subprogram.str=Retype.str(若isfunc为flase则此处为void)+id.str+"("+Parameter.str（多个则用逗号分隔开）+"){\n"+ConstDecl.str+VarDecl.str+Statement.str+"}\n";
  addIndent();
  if (subprogram.isFunction()) {
    result += Utils::getBasicTypeName(subprogram.getReturnType()) + " ";
  } else {
    result += "void ";
  }
  result += subprogram.getId() + "(";
  for (const auto &parameter : subprogram.getParameters()) {
    parameter->accept(*this);
    if (parameter != subprogram.getParameters().back()) {
      result += ", ";
    }
  }
  result += ") {\n";
  now_level++;
  for (const auto &const_decl : subprogram.getConstDeclarations()) {
    const_decl->accept(*this);
  }
  for (const auto &var_decl : subprogram.getVarDeclarations()) {
    var_decl->accept(*this);
  }
  if (subprogram.isFunction()) {
    addIndent();
    result += Utils::getBasicTypeName(subprogram.getReturnType()) + " ";
    result += subprogram.getId() + "_return;\n";
  }
  now_in_which_subprogram = subprogram.getId();
  subprogram.getCompoundStatement()->accept(*this);
  if (subprogram.isFunction()) {
    addIndent();
    result += "return " + subprogram.getId() + "_return;\n";
  }
  now_level--;
  addIndent();
  result += "}\n";
}

void Generator::visit(const Parameter &parameter) const {
  // Parameter.str=type.str+idlist.str(用逗号分隔)
  std::string paramType = Utils::getBasicTypeName(parameter.getBasicType());
  std::string ptr;
  if (parameter.isVar()) {
    ptr = "*";
  }
  for (const auto &id : parameter.getIdList()) {
    result += paramType + ptr + " " + id;
    if (id != parameter.getIdList().back()) {
      result += ", ";
    }
  }
}

void Generator::visit(const ConstDecl &const_decl) const {
  // ConstDecl.str="const"+basic_type+id+"="+value+";\n"
  addIndent();
  auto const_value = const_decl.getValue();
  if (const_value[0] == '\'')
    result += "const char ";
  else if (const_value.find('.') != std::string::npos)
    result += "const float ";
  else if (const_value[0] == 't' || const_decl.getValue()[0] == 'f')
    result += "const bool ";
  else if (const_value[0] == '"')
    result += "const char* ";
  else
    result += "const int ";
  result += const_decl.getId() + " = " + const_decl.getValue() + ";\n";
}

void Generator::visit(const VarDecl &var_decl) const {
  // VarDecl.str=type.str+idlist.tr(用逗号分隔) 如果type为数组则特殊处理，在id后面加上[]分隔的数组每一维大小+';\n'
  addIndent();
  var_decl.getType()->accept(*this);
  if (flag == true) {
    for (const auto &id : var_decl.getIdList()) {
      result += id;
      if (id != var_decl.getIdList().back()) {
        result += ", ";
      }
    }
    result += ";\n";
    while (!tmp_periods.empty()) {
      tmp_periods.pop_back();
    }
    flag = false;
  } else {
    for (const auto &id : var_decl.getIdList()) {
      result += id;
      for (int i = 0; i < tmp_periods.size(); i += 2) {
        result += "[" + tmp_periods[i] + "]";
      }
      if (tmp_periods.size() >= 2) {
        std::vector<std::string> tmp_bias_list;
        for (int i = 1; i < tmp_periods.size(); i += 2) {
          tmp_bias_list.push_back((tmp_periods[i]));
        }
        arr_bias.push_back(std::make_pair(id, tmp_bias_list));
      }
      if (id != var_decl.getIdList().back()) {
        result += ", ";
      }
    }
    result += ";\n";
    while (!tmp_periods.empty()) {
      tmp_periods.pop_back();
    }
    flag = false;
  }
}

void Generator::visit(const Type &type) const {
  // Type.str=basic_type由于父节点根据periods的值进行判断，故此处无需考虑数组
  std::string typeStr = Utils::getBasicTypeName(type.getBasicType());

  if (type.getPeriods().empty()) {
    flag = true;
    result += typeStr + " ";
  } else {
    for (const auto &[lb, ub] : type.getPeriods()) {
      int tmp_num = std::stoi(ub) - std::stoi(lb) + 1;
      tmp_periods.push_back(std::to_string(tmp_num));
      tmp_periods.push_back(lb);
    }
    result += typeStr + " ";
  }
}

void Generator::visit(const Variable &variable) const {
  // Variable.str=id.str后面根据expr_list的值讨论，若不为空则是数组，则+[]的exprlist中每一个字串，代表数组的若干维大小，否则结束
  if (now_in_which_subprogram != "main" && variable.getExpressions().empty()) {
    const auto &params = params_name.at(now_in_which_subprogram);
    if (const auto &it = std::find(params.begin(), params.end(), variable.getId()); it != params.end()) {
      auto index = std::distance(params.begin(), it);
      if (is_var_param.at(now_in_which_subprogram)[index]) {
        result += "*";
      }
    }
  }
  result += variable.getId();
  if (!variable.getExpressions().empty()) {
    std::string target_first = variable.getId();
    auto it = std::find_if(
        arr_bias.begin(), arr_bias.end(),
        [target_first](const std::pair<std::string, std::vector<std::string>> &p) { return p.first == target_first; });

    // 检查是否找到
    if (it != arr_bias.end()) {
      int tmp_index = 0;
      for (const auto &expression : variable.getExpressions()) {
        result += "[";
        expression->accept(*this);
        if (std::stoi(it->second[tmp_index]) != 0) result += " - " + it->second[tmp_index];
        result += "]";
        tmp_index++;
      }
    } else {
      for (const auto &expression : variable.getExpressions()) {
        result += "[";
        expression->accept(*this);
        result += "]";
      }
    }
  }
}

void Generator::visit(const NullStatement &null_statement) const {
  // NullStatement.str=";"
  addIndent();
  result += ";\n";
}

void Generator::visit(const Assign &assign) const {
  // Assign.str=variable.str+"="+Expression.str
  if (assign.getLeft() == nullptr) {
    result += "\n";
    return;
  }
  addIndent();
  if (is_function_return.at(const_cast<Assign *>(&assign))) {
    assign.getLeft()->accept(*this);
    result += "_return = ";
    single_procedure++;
    assign.getRight()->accept(*this);
    single_procedure--;
  } else {
    assign.getLeft()->accept(*this);
    result += " = ";
    single_procedure++;
    assign.getRight()->accept(*this);
    single_procedure--;
  }
  result += ";\n";
}

void Generator::visit(const ProcedureCall &procedure_call) const {
  // ProcedureCall.str=id.str+"("+parameters.str+");\n"
  if (single_procedure <= 0) addIndent();
  result += procedure_call.getId() + "(";

  auto pos = 0;
  for (const auto &param : procedure_call.getParameters()) {
    if (is_var_param.at(procedure_call.getId())[pos]) {
      result += "&";
    }
    pos++;
    param->accept(*this);
    if (param != procedure_call.getParameters().back()) {
      result += ", ";
    }
  }

  result += ")";
  if (single_procedure <= 0) result += ";\n";
}

void Generator::visit(const CompoundStatement &compound_statement) const {
  // CompoundStatement.str="{"+Statement.str+"}\n"
  addIndent();
  result += "{\n";
  now_level++;
  for (const auto &statement : compound_statement.getStatements()) {
    statement->accept(*this);
  }
  if (main_compound_statement == &compound_statement) {
    addIndent();
    result += "return 0;\n";
  }
  now_level--;
  addIndent();
  result += "}\n";
}

void Generator::visit(const If &if_statement) const {
  // If.str="if("+Expression.str+"){\n"+Statement.str+"}\n" 如果有else则+"else{\n"+Expression.str+"}\n"
  addIndent();
  single_procedure++;
  result += "if (";
  if_statement.getCondition()->accept(*this);
  result += ")\n";
  single_procedure--;

  bool is_compound = dynamic_cast<CompoundStatement *>(if_statement.getThenStatement().get()) != nullptr;
  if (!is_compound) {
    now_level++;
  }
  if_statement.getThenStatement()->accept(*this);
  if (!is_compound) {
    now_level--;
  }
  if (if_statement.getElseStatement() != nullptr) {
    addIndent();
    result += "else\n";
    is_compound = dynamic_cast<CompoundStatement *>(if_statement.getElseStatement().get()) != nullptr;
    if (!is_compound) {
      now_level++;
    }
    if_statement.getElseStatement()->accept(*this);
    if (!is_compound) {
      now_level--;
    }
  }
}

void Generator::visit(const For &for_statement) const {
  // For.str="for("+id.str+"="+lb+";"+id.str+"<="+rb+";"+id.str+"++){\n"+Statement.str+"}\n"
  addIndent();
  result += "for (" + for_statement.getId() + " = ";
  for_statement.getLowerBound()->accept(*this);
  result += "; " + for_statement.getId() + " <= ";
  for_statement.getUpperBound()->accept(*this);
  result += "; " + for_statement.getId() + "++)\n";
  bool is_compound = dynamic_cast<CompoundStatement *>(for_statement.getStatement().get()) != nullptr;
  if (!is_compound) {
    now_level++;
  }
  for_statement.getStatement()->accept(*this);
  if (!is_compound) {
    now_level--;
  }
}

void Generator::visit(const While &while_statement) const {
  // While.str="While("+Expression.str+"){\n"+Statement.str+"}\n"
  addIndent();
  result += "while (";
  while_statement.getCondition()->accept(*this);
  result += ")\n";
  bool is_compound = dynamic_cast<CompoundStatement *>(while_statement.getStatement().get()) != nullptr;
  if (!is_compound) {
    now_level++;
  }
  while_statement.getStatement()->accept(*this);
  if (!is_compound) {
    now_level--;
  }
}

void Generator::visit(const Read &read_statement) const {
  // Read.str="scanf("+%某种变量对应的输入格式，此处需要查符号表得到type+","+variables.str+");\n"
  addIndent();
  result += "scanf(\"";
  result += read_fmt_specifier.at(const_cast<Read *>(&read_statement));
  result += "\", ";
  for (const auto &variable : read_statement.getVariables()) {
    result += "&";
    variable->accept(*this);
    if (variable->getExpressions().empty() && variable->getId() == now_in_which_subprogram) {
      result += "_return";
    }
    if (variable != read_statement.getVariables().back()) {
      result += ", ";
    }
  }
  result += ");\n";
}

void Generator::visit(const Write &write_statement) const {
  // Write.str="Printf("+%variable或expression对应类型，此处需要查符号表+","+Expression.str+");\n" 如果多个则逗号分隔
  addIndent();
  result += "printf(\"";
  result += write_fmt_specifier.at(const_cast<Write *>(&write_statement));
  result += "\", ";
  single_procedure++;
  for (const auto &expr : write_statement.getExpressions()) {
    expr->accept(*this);
    if (expr != write_statement.getExpressions().back()) {
      result += ", ";
    }
  }
  result += ");\n";
  single_procedure--;
}

void Generator::visit(const Break &break_) const {
  addIndent();
  result += "break;";
}

void Generator::visit(const Factor &factor) const {
  // Factor.str=value(此处需要根据factor的类型判断value的内容以什么形式读出，若factor为表达式则Expression.str否则为Variable.str)
  auto factor_type = factor.getType();
  if (factor_type == FactorType::FUNCTION || factor_type == FactorType::VARIABLE) {
    std::get<ASTNodePtr>(factor.getValue())->accept(*this);
  } else if (factor_type == FactorType::EXPRESSION) {
    result += " ( ";
    std::get<ASTNodePtr>(factor.getValue())->accept(*this);
    result += " ) ";
  } else if (factor_type == FactorType::WITH_PLUS) {
    result += "+";
    std::get<ASTNodePtr>(factor.getValue())->accept(*this);
  } else if (factor_type == FactorType::WITH_UMINUS) {
    result += "- ";
    std::get<ASTNodePtr>(factor.getValue())->accept(*this);
  } else if (factor_type == FactorType::WITH_NOT) {
    if (is_factor_with_not_number.at(const_cast<Factor *>(&factor))) {
      result += "~";
    } else {
      result += "!";
    }
    std::get<ASTNodePtr>(factor.getValue())->accept(*this);
  } else if (factor_type == FactorType::BOOLEAN) {
    result += std::get<std::string>(factor.getValue());
  } else if (factor_type == FactorType::UNKNOWN) {
    if (is_factor_function_call.contains(const_cast<Factor *>(&factor)) &&
        is_factor_function_call.at(const_cast<Factor *>(&factor))) {
      result += std::get<std::string>(factor.getValue()) + "()";
    } else {
      int reslen = result.size();

      if (reslen >= 3 && result[reslen - 2] == '&' && result[reslen - 3] == '&') {
        result.erase(result.end() - 1);
        result.erase(result.end() - 1);
        result += ' ';
      }
      if (reslen >= 3 && result[reslen - 2] == '|' && result[reslen - 3] == '|') {
        result.erase(result.end() - 1);
        result.erase(result.end() - 1);
        result += ' ';
      }
      auto var_name = std::get<std::string>(factor.getValue());
      if (now_in_which_subprogram != "main") {
        const auto &params = params_name.at(now_in_which_subprogram);
        if (const auto &it = std::find(params.begin(), params.end(), var_name); it != params.end()) {
          auto index = std::distance(params.begin(), it);
          if (is_var_param.at(now_in_which_subprogram)[index]) {
            result += "*";
          }
        }
      }
      result += var_name;
    }
  } else {
    // std::cout<<std::get<std::string>(factor.getValue())<<'\n';
    result += std::get<std::string>(factor.getValue());
  }
}

void Generator::visit(const Term &term) const {
  // Term.str=Factor.str如果为多个因子+"* / div mod &其中之一"+Factor.str
  single_procedure++;
  term.getFirstFactor()->accept(*this);
  if (term.getFactors().empty()) {
    single_procedure--;
    return;
  }
  for (const auto &next_factor : term.getFactors()) {
    result += " " + Utils::getMulOpName(next_factor.first) + " ";
    next_factor.second->accept(*this);
  }
  single_procedure--;
}

void Generator::visit(const SimpleExpression &simple_expression) const {
  // SimpleExpression.str=Term.str如果为多个项拼成则+"+ - |其中之一"+Term.str
  single_procedure++;
  simple_expression.getFirstTerm()->accept(*this);
  if (simple_expression.getTerms().empty()) {
    single_procedure--;
    return;
  }
  for (const auto &next_term : simple_expression.getTerms()) {
    result += " " + Utils::getAddOpName(next_term.first) + " ";
    next_term.second->accept(*this);
  }
  single_procedure--;
}

void Generator::visit(const Expression &expression) const {
  // Expressions.str=SimpleExpression.str如果为多个简单表达式拼成则+"> = < <= >= <>其中之一"+SimpleExpression.str
  single_procedure++;
  expression.getLeft()->accept(*this);
  if (expression.getRight().has_value()) {
    result += " " + Utils::getRelOpName(expression.getRight()->first) + " ";
    expression.getRight()->second->accept(*this);
  }
  single_procedure--;
}
