/**
 * @file semantic_analyzer.cpp
 * @brief 语义分析器实现
 */
#include "semantic_analyzer.hpp"

#include <algorithm>
#include <ranges>
#include <variant>

#include "ast.hpp"
#include "symbol_table.hpp"
#include "utils.hpp"
#include "visitor.hpp"

/**
 * @brief SemanticAnalyzer 构造函数
 *
 * 初始化语义分析器的全局作用域和基本类型，并设置当前处理的函数/过程为 nullptr。
 *
 * - 初始化全局作用域栈，确保语义分析从全局作用域开始。
 * - 预定义基本类型，包括整数类型、实数类型、布尔类型和字符类型。
 * - 设置当前处理的函数/过程为 nullptr，用于后续 return 语句的检查。
 */
SemanticAnalyzer::SemanticAnalyzer() {
  // 初始化全局作用域
  scope_stack.push();

  // 预定义基本类型
  integer_type = SymbolType::createBasicType(BaseTypeKind::INTEGER);
  real_type = SymbolType::createBasicType(BaseTypeKind::REAL);
  boolean_type = SymbolType::createBasicType(BaseTypeKind::BOOLEAN);
  char_type = SymbolType::createBasicType(BaseTypeKind::CHAR);
}

/**
 * @brief 获取全局符号表
 * @return 指向全局符号表的指针，如果作用域栈为空则返回 nullptr
 */
auto SemanticAnalyzer::getGlobalSymbolTable() const -> SymbolTablePtr {
  return scope_stack.empty() ? nullptr : scope_stack.current();
}

/**
 * @brief 检查是否有语义错误
 * @return 如果存在语义错误则返回 true，否则返回 false
 */
auto SemanticAnalyzer::hasError() const -> bool { return error_count > 0; }

/**
 * @brief 访问 Program 节点
 *
 * 遍历程序的常量声明、变量声明、子程序声明和主程序体，进行语义分析。
 *
 * @param program Program AST 节点
 */
void SemanticAnalyzer::visit(const Program &program) const {
  for (const auto &const_decl : program.getConstDeclarations()) {
    const_decl->accept(*this);
  }
  for (const auto &var_decl : program.getVarDeclarations()) {
    var_decl->accept(*this);
  }
  for (const auto &subprogram : program.getSubprograms()) {
    subprogram->accept(*this);
  }
  program.getCompoundStatement()->accept(*this);
}

/**
 * @brief 访问 Subprogram 节点
 *
 * 处理子程序（函数或过程）的声明，包括创建新的作用域、处理参数、
 * 创建子程序符号表条目，并分析子程序内部的声明和语句。
 *
 * @param subprogram Subprogram AST 节点
 */
void SemanticAnalyzer::visit(const Subprogram &subprogram) const {
  // 创建子程序的符号表条目
  auto subprogram_id = subprogram.getId();
  const auto is_function = subprogram.isFunction();

  // 为子程序创建新的作用域
  scope_stack.push();

  // 处理参数
  std::vector<SymbolEntryPtr> parameters;
  for (const auto &param : subprogram.getParameters()) {
    param->accept(*this);

    // 收集参数信息
    for (const auto &param_id : param->getIdList()) {
      if (auto param_entry = scope_stack.current()->lookup(param_id)) {
        parameters.push_back(param_entry.value());
      }
    }
  }

  // 创建子程序类型
  std::shared_ptr<SymbolType> return_type = nullptr;
  if (is_function) {
    // 函数需要有返回类型
    switch (subprogram.getReturnType()) {
      case BasicType::INTEGER:
        return_type = integer_type;
        break;
      case BasicType::REAL:
        return_type = real_type;
        break;
      case BasicType::BOOLEAN:
        return_type = boolean_type;
        break;
      case BasicType::CHAR:
        return_type = char_type;
        break;
    }
  }

  // 保存当前作用域供后续引用
  const auto local_scope = scope_stack.current();

  // 创建子程序类型
  auto subprogram_type = SymbolType::createCallableType(is_function, return_type, parameters, local_scope);

  // 在父作用域中添加子程序的符号表条目
  scope_stack.pop();
  auto subprogram_entry =
      std::make_shared<SymbolEntry>(subprogram_id, subprogram_type, scope_stack.current()->getCurrentScope());
  scope_stack.current()->insert(subprogram_entry);

  // 重新进入子程序作用域，继续处理子程序内部
  scope_stack.push();

  // 处理子程序参数
  is_var_param[subprogram_id] = std::vector<bool>();
  params_name[subprogram_id] = std::vector<std::string>();
  for (const auto &param : subprogram.getParameters()) {
    param->accept(*this);
    if (param->isVar()) {
      // 如果是引用参数，添加到 is_var_param 映射中
      for (const auto &param_id : param->getIdList()) {
        is_var_param[subprogram_id].push_back(true);
        params_name[subprogram_id].push_back(param_id);
      }
    } else {
      // 如果是值参数，添加到 is_var_param 映射中
      for (const auto &param_id : param->getIdList()) {
        is_var_param[subprogram_id].push_back(false);
        params_name[subprogram_id].push_back(param_id);
      }
    }
  }

  // 处理子程序内的常量声明
  for (const auto &const_decl : subprogram.getConstDeclarations()) {
    const_decl->accept(*this);
  }

  // 处理子程序内的变量声明
  for (const auto &var_decl : subprogram.getVarDeclarations()) {
    var_decl->accept(*this);
  }

  // 处理子程序体
  subprogram.getCompoundStatement()->accept(*this);

  // 离开子程序作用域
  scope_stack.pop();
}

/**
 * @brief 访问 Parameter 节点
 *
 * 处理子程序的参数声明，为每个参数创建符号表条目，并设置其类型和属性（如是否为引用参数）。
 *
 * @param parameter Parameter AST 节点
 */
void SemanticAnalyzer::visit(const Parameter &parameter) const {
  auto basic_type = getSymbolTypeFromBasicType(parameter.getBasicType());
  if (!basic_type) return;

  // 为每个参数ID创建符号表条目
  for (const auto &param_id : parameter.getIdList()) {
    auto param_entry = std::make_shared<SymbolEntry>(param_id, basic_type, scope_stack.current()->getCurrentScope());

    // 设置参数属性
    param_entry->is_reference = parameter.isVar();

    // 将参数添加到当前作用域
    scope_stack.current()->insert(param_entry);
  }
}

/**
 * @brief 访问 ConstDecl 节点
 *
 * 处理常量声明，推断常量类型，创建常量符号表条目，并将其添加到当前作用域。
 *
 * @param const_decl ConstDecl AST 节点
 */
void SemanticAnalyzer::visit(const ConstDecl &const_decl) const {
  // 该函数检查一个**已经**保证是数字的字符串是否为整数字符串
  auto isIntegralString = [](const std::string &s) -> bool {
    return std::ranges::all_of(s.begin(), s.end(),
                               [](const char c) -> bool { return std::isdigit(c) || c == '+' || c == '-'; });
  };
  auto isFloatString = [](const std::string &s) -> bool {
    // 检查是否为浮点数
    return s.find('.') != std::string::npos;
  };
  auto isString = [](const std::string &s) -> bool { return s.length() >= 2 && s.front() == '"' && s.back() == '"'; };
  // 常量声明的处理
  std::shared_ptr<SymbolType> const_type;
  const auto &const_value = const_decl.getValue();

  if (isIntegralString(const_value)) {
    // 如果是整数
    const_type = integer_type;
  } else if (const_value.length() == 3 && const_value.front() == '\'' && const_value.back() == '\'') {
    // 如果是字符
    const_type = char_type;
  } else if (isFloatString(const_value)) {
    // 如果是浮点数
    const_type = real_type;
  } else if (isString(const_value)) {
    // 如果是字符串，用char_type代替
    const_type = char_type;
  } else {
    // 未知的常量类型，报错
    SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                          "Unknown constant type for '" + const_decl.getId() + "'", 0);
    error_count++;
    return;
  }

  // 创建常量符号表条目
  auto const_entry =
      std::make_shared<SymbolEntry>(const_decl.getId(), const_type, scope_stack.current()->getCurrentScope());

  // 标记为常量
  const_entry->is_constant = true;

  const_entry->const_value = const_value;

  // 添加到当前作用域
  scope_stack.current()->insert(const_entry);
}

/**
 * @brief 访问 VarDecl 节点
 *
 * 处理变量声明，解析变量类型，为每个变量创建符号表条目，并将其添加到当前作用域。
 *
 * @param var_decl VarDecl AST 节点
 */
void SemanticAnalyzer::visit(const VarDecl &var_decl) const {
  // 处理变量类型
  const auto &type_node = var_decl.getType();
  auto var_type = processType(*type_node);

  if (!var_type) {
    // 如果类型处理失败，则跳过该变量声明
    return;
  }

  // 为每个变量ID创建符号表条目
  for (const auto &var_id : var_decl.getIdList()) {
    auto var_entry = std::make_shared<SymbolEntry>(var_id, var_type, scope_stack.current()->getCurrentScope());

    // 添加到当前作用域
    scope_stack.current()->insert(var_entry);
  }
}

/**
 * @brief 访问 Type 节点
 *
 * 此方法通常由 processType 方法内部调用，用于处理类型定义。
 *
 * @param type Type AST 节点
 */
void SemanticAnalyzer::visit(const Type &type) const {
  // 访问Type节点的方法（通常由processType方法调用）
}

/**
 * @brief 访问 Variable 节点
 *
 * 处理变量引用，检查变量是否已声明、是否在作用域内，并处理数组索引访问（检查维度和边界）。
 *
 * @param variable Variable AST 节点
 */
void SemanticAnalyzer::visit(const Variable &variable) const {
  // 变量引用的检查
  const auto &var_name = variable.getId();

  // 在符号表中查找变量
  auto entry_opt = scope_stack.current()->recursiveLookup(var_name);
  if (!entry_opt) {
    // 未定义的变量
    SemanticError::report(SemanticError::ErrorType::UNDEFINED_SYMBOL, "Use undeclared variables '" + var_name + "'",
                          0  // 此处需要行号信息
    );
    error_count++;
    return;
  }

  // 检查变量是否可访问
  if (!scope_stack.isAccessible(entry_opt.value())) {
    // 作用域违规
    SemanticError::report(SemanticError::ErrorType::SCOPE_VIOLATION, "variable '" + var_name + "' Out of its scope",
                          0  // 此处需要行号信息
    );
    error_count++;
    return;
  }

  const auto &entry = entry_opt.value();
  const auto var_type = entry->type;

  // 检查数组访问
  if (!variable.getExpressions().empty()) {
    // 首先检查变量是否是数组类型
    if (var_type->kind != TypeKind::ARRAY) {
      SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                            "Variable '" + var_name + "' is not an array type but used with indices",
                            0  // 此处需要行号信息
      );
      error_count++;
      return;
    }

    // 在确认是数组类型后进行转换
    auto array_type = std::get<ArrayType>(var_type->type_data);

    // 然后检查索引数量是否与数组维度匹配
    if (variable.getExpressions().size() != array_type.dimensions.size()) {
      SemanticError::report(
          SemanticError::ErrorType::OTHER_ERROR,
          "Array '" + var_name + "' has " + std::to_string(array_type.dimensions.size()) +
              " dimensions, but accessed with " + std::to_string(variable.getExpressions().size()) + " indices",
          0  // 此处需要行号信息
      );
      error_count++;
      return;
    }

    // 处理每个索引表达式
    for (size_t i = 0; i < variable.getExpressions().size(); i++) {
      const auto &expr = variable.getExpressions()[i];
      // 检查索引表达式
      expr->accept(*this);

      // 如果索引是常量表达式，可以在编译时检查是否越界
      if (auto literal = tryGetConstantValue(expr)) {
        int index_value = *literal;
        auto &dimension = array_type.dimensions[i];
        int lower_bound = dimension.first;
        int upper_bound = dimension.second;

        // 检查是否越界
        if (index_value < lower_bound || index_value > upper_bound) {
          SemanticError::report(
              SemanticError::ErrorType::ARRAY_INDEX_OUT_OF_BOUNDS,
              "Array index " + std::to_string(index_value) + " is out of range [" + std::to_string(lower_bound) + ".." +
                  std::to_string(upper_bound) + "] for array '" + var_name + "' at dimension " + std::to_string(i + 1),
              0  // 此处需要行号信息
          );
          error_count++;
        }
      }
    }
  }
}

/**
 * @brief 访问 NullStatement 节点
 *
 * 空语句不需要进行语义检查。
 *
 * @param null_statement NullStatement AST 节点
 */
void SemanticAnalyzer::visit(const NullStatement &null_statement) const {
  // 空语句不需要语义检查
}

/**
 * @brief 访问 Assign 节点
 *
 * 处理赋值语句，检查左侧变量是否可赋值（非常量），并检查左右两侧的类型兼容性。
 *
 * @param assign Assign AST 节点
 */
void SemanticAnalyzer::visit(const Assign &assign) const {
  // 处理左侧变量
  assign.getLeft()->accept(*this);

  // 处理右侧表达式
  assign.getRight()->accept(*this);

  // 获取左侧变量的符号表条目
  auto left_var_name = assign.getLeft()->getId();
  auto left_entry_opt = scope_stack.current()->recursiveLookup(left_var_name);

  if (!left_entry_opt) {
    return;  // 左侧变量未定义，错误已在visit(Variable)中报告
  }

  const auto &left_entry = left_entry_opt.value();

  // 检查左侧是否可赋值（不是常量）
  if (left_entry->is_constant) {
    SemanticError::report(SemanticError::ErrorType::CONSTANT_ASSIGNMENT,
                          "cannot to constants '" + left_var_name + "' assign values",
                          0  // 此处需要行号信息
    );
    error_count++;
    return;
  }

  // 获取左右两侧的类型
  auto left_type = left_entry->type;
  auto right_type = getExpressionType(assign.getRight().get());

  // 如果无法确定右侧表达式的类型，无法进行类型检查
  if (!right_type) {
    is_function_return.emplace(const_cast<Assign *>(&assign), false);
    return;
  }

  // 检查左侧变量是否为数组变量
  bool left_is_array = left_type->kind == TypeKind::ARRAY;
  bool left_has_index = !assign.getLeft()->getExpressions().empty();

  // 确定左侧实际类型（如果是有索引的数组，取元素类型）
  std::shared_ptr<SymbolType> actual_left_type;
  if (left_is_array && left_has_index) {
    actual_left_type = std::get<ArrayType>(left_type->type_data).element_type;
  } else {
    actual_left_type = left_type;
  }

  if (actual_left_type->kind == TypeKind::BASIC) {
    auto left_base_type = std::get<BaseType>(actual_left_type->type_data).base_kind;
    auto right_base_type = std::get<BaseType>(right_type->type_data).base_kind;
    if (right_type->kind == TypeKind::BASIC) {
      if (left_base_type == right_base_type) {
        is_function_return.emplace(const_cast<Assign *>(&assign), false);
      } else if (left_base_type == BaseTypeKind::REAL && right_base_type == BaseTypeKind::INTEGER) {
        // 整数可以赋值给实数
        is_function_return.emplace(const_cast<Assign *>(&assign), false);
      } else {
        // 类型不匹配
        SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                              "Cannot assign value of type '" + getTypeName(right_type) + "' to variable '" +
                                  left_var_name + "' of type '" + getTypeName(actual_left_type) + "'",
                              0  // 此处需要行号信息
        );
        error_count++;
      }
    } else {
      // 类型不匹配
      SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                            "Cannot assign value of type '" + getTypeName(right_type) + "' to variable '" +
                                left_var_name + "' of type '" + getTypeName(actual_left_type) + "'",
                            0  // 此处需要行号信息
      );
      error_count++;
    }
  } else if (actual_left_type->kind == TypeKind::FUNCTION) {
    auto return_type =
        std::get<BaseType>(std::get<CallableType>(actual_left_type->type_data).return_type->type_data).base_kind;
    if (return_type == std::get<BaseType>(right_type->type_data).base_kind ||
        (return_type == BaseTypeKind::REAL &&
         std::get<BaseType>(right_type->type_data).base_kind == BaseTypeKind::INTEGER)) {
      is_function_return.emplace(const_cast<Assign *>(&assign), true);
    } else {
      // 类型不匹配
      SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                            "Cannot assign value of type '" + getTypeName(right_type) + "' to variable '" +
                                left_var_name + "' of type '" + getTypeName(actual_left_type) + "'",
                            0  // 此处需要行号信息
      );
      error_count++;
    }
  } else {
    // 其他类型不支持赋值
    SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                          "Cannot assign value of type '" + getTypeName(right_type) + "' to variable '" +
                              left_var_name + "' of type '" + getTypeName(actual_left_type) + "'",
                          0  // 此处需要行号信息
    );
    error_count++;
  }
}

/**
 * @brief 访问 ProcedureCall 节点
 *
 * 处理过程调用，检查过程是否已声明、是否为可调用类型（过程或函数），
 * 并检查实际参数与形式参数的数量和类型兼容性。特别处理引用参数（VAR 参数）。
 *
 * @param procedure_call ProcedureCall AST 节点
 */
void SemanticAnalyzer::visit(const ProcedureCall &procedure_call) const {
  // 查找过程名
  const auto &proc_name = procedure_call.getId();
  const auto proc_entry_opt = scope_stack.current()->recursiveLookup(proc_name);

  if (!proc_entry_opt) {
    SemanticError::report(SemanticError::ErrorType::UNDEFINED_SYMBOL,
                          "Invoke an undeclared procedure '" + proc_name + "'",
                          0  // 此处需要行号信息
    );
    error_count++;
    return;
  }

  const auto &proc_entry = proc_entry_opt.value();

  // 检查是否为过程或函数类型
  if (proc_entry->type->kind != TypeKind::PROCEDURE && proc_entry->type->kind != TypeKind::FUNCTION) {
    SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH, "'" + proc_name + "' Not a procedure or a function",
                          0  // 此处需要行号信息
    );
    error_count++;
    return;
  }

  // 转换为CallableType以访问参数和返回类型信息
  auto callable_type = std::get<CallableType>(proc_entry->type->type_data);

  // 检查参数数量
  const auto &formal_params = callable_type.parameters;
  const auto &actual_params = procedure_call.getParameters();

  if (formal_params.size() != actual_params.size()) {
    SemanticError::report(SemanticError::ErrorType::OTHER_ERROR,
                          "Procedure/function '" + proc_name + "' requires " + std::to_string(formal_params.size()) +
                              " parameters, but " + std::to_string(actual_params.size()) + " were given",
                          0);
    error_count++;
    return;
  }

  // 处理参数并进行类型检查
  std::vector<std::pair<SymbolEntryPtr, int>> args;
  for (size_t i = 0; i < actual_params.size(); i++) {
    const auto &expr = actual_params[i];
    expr->accept(*this);  // 分析表达式

    // 获取参数表达式的类型
    auto expr_type = getExpressionType(expr.get());

    if (!expr_type) {
      // 无法确定参数类型，跳过此参数的类型检查
      continue;
    }
    // 获取对应的形参信息
    const auto &formal_param = formal_params[i];
    auto formal_type = formal_param->type;

    // 检查参数类型兼容性
    if (!areTypesCompatible(formal_type, expr_type)) {
      SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                            "Parameter " + std::to_string(i + 1) + " of call to '" + proc_name +
                                "' has incompatible type: expected '" + getTypeName(formal_type) + "', got '" +
                                getTypeName(expr_type) + "'",
                            0);
      error_count++;
    }

    // 特殊处理引用参数(VAR参数)
    if (formal_param->is_reference) {
      // 检查实参是否为可修改的左值
      bool is_lvalue = false;

      auto first_factor_type = expr->getLeft()->getFirstTerm()->getFirstFactor()->getType();
      auto only_one_factor = expr->getLeft()->getFirstTerm()->getFactors().empty() &&
                             expr->getLeft()->getTerms().empty() && !expr->getRight().has_value();

      if (only_one_factor) {
        // 如果是单个因子且不是函数调用，则可以作为左值
        if (first_factor_type == FactorType::VARIABLE) {
          is_lvalue = true;
        } else if (first_factor_type == FactorType::UNKNOWN &&
                   std::holds_alternative<std::string>(expr->getLeft()->getFirstTerm()->getFirstFactor()->getValue())) {
          // 如果是未知类型，进一步检查是否为函数调用
          auto id = std::get<std::string>(expr->getLeft()->getFirstTerm()->getFirstFactor()->getValue());
          if (auto func_entry_opt = scope_stack.current()->recursiveLookup(id)) {
            // 如果是函数调用，则不能作为左值
            if (func_entry_opt.value()->type->kind == TypeKind::FUNCTION) {
              is_lvalue = false;
            } else {
              is_lvalue = true;
            }
          } else {
            // 未定义的变量
            SemanticError::report(SemanticError::ErrorType::UNDEFINED_SYMBOL, "Use undeclared variables '" + id + "'",
                                  0);
            error_count++;
          }
        }
      }

      if (!is_lvalue) {
        SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                              "Parameter " + std::to_string(i + 1) + " of '" + proc_name +
                                  "' requires a variable reference (VAR parameter)",
                              0);
        error_count++;
      }
    }

    // 收集参数信息用于后续处理
    if (auto var_ptr = dynamic_cast<const Variable *>(expr.get())) {
      auto var_name = var_ptr->getId();
      if (auto var_entry_opt = scope_stack.current()->recursiveLookup(var_name)) {
        args.emplace_back(var_entry_opt.value(), 0);  // 0作为行号占位符
      }
    }
  }
}

/**
 * @brief 访问 CompoundStatement 节点
 *
 * 递归地访问复合语句中的每一条语句。
 *
 * @param compound_statement CompoundStatement AST 节点
 */
void SemanticAnalyzer::visit(const CompoundStatement &compound_statement) const {
  // 处理复合语句中的所有语句
  for (const auto &stmt : compound_statement.getStatements()) {
    stmt->accept(*this);
  }
}

/**
 * @brief 访问 If 节点
 *
 * 处理 if 语句，分析条件表达式、then 分支和可选的 else 分支。
 * 检查条件表达式是否为布尔类型。
 *
 * @param if_statement If AST 节点
 */
void SemanticAnalyzer::visit(const If &if_statement) const {
  // 处理条件表达式
  if_statement.getCondition()->accept(*this);

  // 处理then部分
  if_statement.getThenStatement()->accept(*this);

  // 处理else部分（如果存在）
  if (if_statement.getElseStatement()) {
    if_statement.getElseStatement()->accept(*this);
  }

  // 检查条件表达式是否为布尔类型
  auto condition_type = getExpressionType(if_statement.getCondition().get());
  if (!condition_type) {
    // 无法确定条件表达式的类型，跳过检查
    return;
  }

  // 检查是否为布尔类型
  bool is_boolean = condition_type->kind == TypeKind::BASIC &&
                    std::holds_alternative<BaseType>(condition_type->type_data) &&
                    std::get<BaseType>(condition_type->type_data).base_kind == BaseTypeKind::BOOLEAN;

  if (!is_boolean) {
    SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                          "If condition must be of boolean type, but got '" + getTypeName(condition_type) + "'",
                          0  // 此处需要行号信息
    );
    error_count++;
  }
}

/**
 * @brief 访问 For 节点
 *
 * 处理 for 循环语句，检查循环变量是否已声明且为整型，分析循环边界表达式和循环体。
 *
 * @param for_statement For AST 节点
 */
void SemanticAnalyzer::visit(const For &for_statement) const {
  // 查找循环变量
  const auto &loop_var_name = for_statement.getId();
  const auto loop_var_entry_opt = scope_stack.current()->recursiveLookup(loop_var_name);

  if (!loop_var_entry_opt) {
    SemanticError::report(SemanticError::ErrorType::UNDEFINED_SYMBOL,
                          "Use undeclared loop variables '" + loop_var_name + "'",
                          0  // 此处需要行号信息
    );
    error_count++;
    return;
  }

  // 处理边界表达式
  for_statement.getLowerBound()->accept(*this);
  for_statement.getUpperBound()->accept(*this);

  // 处理循环体
  for_statement.getStatement()->accept(*this);

  // 检查循环变量是否为整型
  auto loop_var_type = loop_var_entry_opt.value()->type;
  bool is_integer_var = loop_var_type->kind == TypeKind::BASIC &&
                        std::holds_alternative<BaseType>(loop_var_type->type_data) &&
                        std::get<BaseType>(loop_var_type->type_data).base_kind == BaseTypeKind::INTEGER;

  if (!is_integer_var) {
    SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                          "For loop variable '" + loop_var_name + "' must be of integer type, but got '" +
                              getTypeName(loop_var_type) + "'",
                          0  // 此处需要行号信息
    );
    error_count++;
  }

  // 检查边界表达式是否为整型
  auto lower_bound_type = getExpressionType(for_statement.getLowerBound().get());
  auto upper_bound_type = getExpressionType(for_statement.getUpperBound().get());

  // 检查下界表达式
  if (lower_bound_type) {
    bool is_integer_lower = lower_bound_type->kind == TypeKind::BASIC &&
                            std::holds_alternative<BaseType>(lower_bound_type->type_data) &&
                            std::get<BaseType>(lower_bound_type->type_data).base_kind == BaseTypeKind::INTEGER;

    if (!is_integer_lower) {
      SemanticError::report(
          SemanticError::ErrorType::TYPE_MISMATCH,
          "For loop lower bound must be of integer type, but got '" + getTypeName(lower_bound_type) + "'",
          0  // 此处需要行号信息
      );
      error_count++;
    }
  }

  // 检查上界表达式
  if (upper_bound_type) {
    bool is_integer_upper = upper_bound_type->kind == TypeKind::BASIC &&
                            std::holds_alternative<BaseType>(upper_bound_type->type_data) &&
                            std::get<BaseType>(upper_bound_type->type_data).base_kind == BaseTypeKind::INTEGER;

    if (!is_integer_upper) {
      SemanticError::report(
          SemanticError::ErrorType::TYPE_MISMATCH,
          "For loop upper bound must be of integer type, but got '" + getTypeName(upper_bound_type) + "'",
          0  // 此处需要行号信息
      );
      error_count++;
    }
  }
}

/**
 * @brief 访问并处理 While 语句的语义分析。
 *
 * 此方法对 While 语句的条件表达式和循环体进行语义分析，并检查条件表达式的类型是否为布尔类型。
 * 如果条件表达式的类型不是布尔类型，则报告语义错误。
 *
 * @param while_statement 待处理的 While 语句对象。
 *
 * @note 如果无法确定条件表达式的类型，将跳过类型检查。
 * @note 如果条件表达式的类型不是布尔类型，将增加错误计数。
 */
void SemanticAnalyzer::visit(const While &while_statement) const {
  // 处理条件表达式
  while_statement.getCondition()->accept(*this);

  // 处理循环体
  while_statement.getStatement()->accept(*this);

  // 检查条件表达式是否为布尔类型
  auto condition_type = getExpressionType(while_statement.getCondition().get());
  if (!condition_type) {
    // 无法确定条件表达式的类型，跳过检查
    return;
  }

  // 检查是否为布尔类型
  bool is_boolean = condition_type->kind == TypeKind::BASIC &&
                    std::holds_alternative<BaseType>(condition_type->type_data) &&
                    std::get<BaseType>(condition_type->type_data).base_kind == BaseTypeKind::BOOLEAN;

  if (!is_boolean) {
    SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                          "While condition must be of boolean type, but got '" + getTypeName(condition_type) + "'",
                          0  // 此处需要行号信息
    );
    error_count++;
  }
}

/**
 * @brief 访问并处理 Read 语句的语义分析。
 *
 * 此函数会遍历 Read 语句中的所有变量，检查其合法性并报告相关的语义错误。
 *
 * @param read_statement 要处理的 Read 语句。
 *
 * 处理逻辑包括：
 * - 检查变量是否已定义。
 * - 检查变量是否为常量（常量不能作为 Read 的目标）。
 * - 检查数组类型变量是否提供了索引（不能读取整个数组）。
 *
 * 如果发现语义错误，会通过 SemanticError::report 报告错误，并增加错误计数。
 */
void SemanticAnalyzer::visit(const Read &read_statement) const {
  std::string fmt;
  // 处理所有要读取的变量
  for (const auto &var : read_statement.getVariables()) {
    var->accept(*this);

    // 检查变量是否为有效的左值
    auto var_name = var->getId();
    auto var_entry_opt = scope_stack.current()->recursiveLookup(var_name);

    if (!var_entry_opt) {
      // 变量未定义，错误已在visit(Variable)中报告
      continue;
    }

    const auto &var_entry = var_entry_opt.value();

    // 检查是否为常量（常量不能作为read目标）
    if (var_entry->is_constant) {
      SemanticError::report(SemanticError::ErrorType::CONSTANT_ASSIGNMENT,
                            "Cannot read into constant '" + var_name + "'",
                            0  // 此处需要行号信息
      );
      error_count++;
      continue;
    }

    // 检查数组访问
    bool is_array = var_entry->type->kind == TypeKind::ARRAY;
    bool has_index = !var->getExpressions().empty();

    // 如果是数组类型但没有提供索引，不能读取整个数组
    if (is_array && !has_index) {
      SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                            "Cannot read into an entire array '" + var_name + "', must specify array element",
                            0  // 此处需要行号信息
      );
      error_count++;
    }

    auto var_type = var_entry->type;
    if (var_type->kind == TypeKind::BASIC) {
      switch (std::get<BaseType>(var_type->type_data).base_kind) {
        case BaseTypeKind::INTEGER:
          fmt += "%d";
          break;
        case BaseTypeKind::REAL:
          fmt += "%f";
          break;
        case BaseTypeKind::CHAR:
          fmt += "%c";
          break;
        case BaseTypeKind::BOOLEAN:
          fmt += "%d";
          break;
      }
    } else if (var_type->kind == TypeKind::ARRAY &&
               std::holds_alternative<BaseType>(std::get<ArrayType>(var_type->type_data).element_type->type_data)) {
      switch (std::get<BaseType>(std::get<ArrayType>(var_type->type_data).element_type->type_data).base_kind) {
        case BaseTypeKind::INTEGER:
          fmt += "%d";
          break;
        case BaseTypeKind::REAL:
          fmt += "%f";
          break;
        case BaseTypeKind::CHAR:
          fmt += "%c";
          break;
        case BaseTypeKind::BOOLEAN:
          fmt += "%d";
          break;
      }
    } else if (var_type->kind == TypeKind::FUNCTION) {
      switch (std::get<BaseType>(std::get<CallableType>(var_type->type_data).return_type->type_data).base_kind) {
        case BaseTypeKind::INTEGER:
          fmt += "%d";
          break;
        case BaseTypeKind::REAL:
          fmt += "%f";
          break;
        case BaseTypeKind::CHAR:
          fmt += "%c";
          break;
        case BaseTypeKind::BOOLEAN:
          fmt += "%d";
          break;
      }
    } else {
      // 报错
      SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                            "Cannot read into variable '" + var_name + "' of type '" + getTypeName(var_type) + "'",
                            0  // 此处需要行号信息
      );
    }
    if (var != read_statement.getVariables().back()) {
      fmt += " ";
    }
  }
  read_fmt_specifier.emplace(const_cast<Read *>(&read_statement), fmt);
}

/**
 * @brief 访问 Write 语句节点并处理其包含的表达式。
 *
 * 此方法会遍历 Write 语句中的所有表达式，并对每个表达式调用 accept 方法，
 * 从而触发语义分析的相关逻辑。
 *
 * @param write_statement 要处理的 Write 语句节点。
 */
void SemanticAnalyzer::visit(const Write &write_statement) const {
  // 处理所有要输出的表达式
  std::string fmt;
  for (const auto &expr : write_statement.getExpressions()) {
    // 针对string特判断
    auto factor_ptr = expr->getLeft()->getFirstTerm()->getFirstFactor().get();
    if (factor_ptr->getType() == FactorType::UNKNOWN) {
      auto ent_opt = scope_stack.current()->recursiveLookup(std::get<std::string>(factor_ptr->getValue()));
      if (ent_opt.has_value()) {
        auto ent = ent_opt.value();
        if (ent->is_constant && std::get<std::string>(ent->const_value).length() >= 2) {
          auto const_value = std::get<std::string>(ent->const_value);
          if (const_value.front() == '"' && const_value.back() == '"') {
            fmt += "%s";
            continue;
          }
        }
      }
    }
    expr->accept(*this);
    auto expr_type = getExpressionType(expr.get());
    if (!expr_type) {
      // 无法确定表达式的类型，跳过检查
      continue;
    }
    if (expr_type == integer_type) {
      fmt += "%d";
    } else if (expr_type == real_type) {
      fmt += "%f";
    } else if (expr_type == char_type) {
      fmt += "%c";
    } else if (expr_type == boolean_type) {
      fmt += "%d";
    } else {
      // 报错
      SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                            "Cannot write variable of type '" + getTypeName(expr_type) + "'",
                            0  // 此处需要行号信息
      );
    }
  }
  write_fmt_specifier.emplace(const_cast<Write *>(&write_statement), fmt);
}

/**
 * @brief 访问 Break 语句节点
 *
 * @param break_ 要处理的 Break 语句节点。
 */
void SemanticAnalyzer::visit(const Break &break_) const {
  // do nothing
}

/**
 * @brief 访问并分析因子节点，根据因子的类型执行相应的语义检查。
 *
 * @param factor 要访问的因子节点。
 *
 * 此函数根据因子的类型执行不同的语义分析操作：
 * - 如果是数字字面量（NUMBER），无需特殊处理。
 * - 如果是变量（VARIABLE），递归访问变量节点。
 * - 如果是表达式（EXPRESSION），调用 `getExpressionType` 检查表达式类型。
 * - 如果是函数调用（FUNCTION），递归访问函数调用节点。
 * - 如果是带负号的因子（WITH_UMINUS），检查操作数是否为数值类型（整数或实数）。
 * - 如果是带NOT操作符的因子（WITH_NOT），检查操作数是否为布尔类型。
 * - 如果是未知类型（UNKNOWN），可能需要进一步分析。
 *
 * @note 对于不符合语义规则的因子，会报告语义错误并增加错误计数。
 */
void SemanticAnalyzer::visit(const Factor &factor) const {
  switch (factor.getType()) {
    case FactorType::NUMBER:
      // 数字字面量，不需要特殊处理
      break;
    case FactorType::VARIABLE: {
      // 处理变量因子
      const auto &var_ptr = std::get<ASTNodePtr>(factor.getValue());
      var_ptr->accept(*this);
      break;
    }
    case FactorType::EXPRESSION: {
      // 处理表达式因子
      const auto &expr = std::get<ASTNodePtr>(factor.getValue());
      auto expr_ptr = dynamic_cast<const Expression *>(expr.get());
      if (!expr_ptr) return;

      expr_ptr->accept(*this);  // 递归访问表达式
      break;
    }
    case FactorType::FUNCTION: {
      // 处理函数调用
      const auto &func_call_ptr = std::get<ASTNodePtr>(factor.getValue());
      func_call_ptr->accept(*this);
      break;
    }
    case FactorType::WITH_PLUS:
    case FactorType::WITH_UMINUS: {
      // 负号应用于数值类型
      const auto &operand = std::get<ASTNodePtr>(factor.getValue());
      auto operand_ptr = dynamic_cast<const Factor *>(operand.get());
      if (!operand_ptr) return;

      // 使用新的重载函数直接传递指针
      auto operand_type = getFactorType(operand_ptr);
      if (!std::holds_alternative<BaseType>(operand_type->type_data) ||
          (std::get<BaseType>(operand_type->type_data).base_kind != BaseTypeKind::INTEGER &&
           std::get<BaseType>(operand_type->type_data).base_kind != BaseTypeKind::REAL)) {
        SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH, "Unary minus operator requires numeric operand",
                              0);
        error_count++;
        return;
      }
      operand_ptr->accept(*this);  // 递归访问操作数

      return;
    }
    case FactorType::WITH_NOT: {
      // NOT应用于布尔类型
      const auto &operand = std::get<ASTNodePtr>(factor.getValue());
      const auto operand_ptr = dynamic_cast<const Factor *>(operand.get());
      if (!operand_ptr) return;

      const auto &operand_type = getFactorType(operand_ptr);
      // 直接使用操作数因子，而不是尝试获取"第一个因子"
      if (!std::holds_alternative<BaseType>(operand_type->type_data) ||
          (std::get<BaseType>(operand_type->type_data).base_kind != BaseTypeKind::BOOLEAN &&
           std::get<BaseType>(operand_type->type_data).base_kind != BaseTypeKind::INTEGER &&
           std::get<BaseType>(operand_type->type_data).base_kind != BaseTypeKind::REAL)) {
        SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH, "NOT operator requires boolean operand", 0);
        error_count++;
        return;
      }

      if (std::get<BaseType>(operand_type->type_data).base_kind == BaseTypeKind::INTEGER) {
        is_factor_with_not_number.emplace(const_cast<Factor *>(&factor), true);
      } else {
        is_factor_with_not_number.emplace(const_cast<Factor *>(&factor), false);
      }

      operand_ptr->accept(*this);  // 递归访问操作数

      return;
    }
    case FactorType::BOOLEAN:
      // 布尔字面量，不需要特殊处理
      break;
    case FactorType::UNKNOWN:
      // 未知类型，可能是变量或函数
      auto entry_opt = scope_stack.current()->recursiveLookup(std::get<std::string>(factor.getValue()));
      if (!entry_opt) {
        SemanticError::report(SemanticError::ErrorType::UNDEFINED_SYMBOL,
                              "Use undeclared variable or function '" + std::get<std::string>(factor.getValue()) + "'",
                              0);
        error_count++;
      } else {
        const auto &entry = entry_opt.value();
        if (entry->type->kind == TypeKind::FUNCTION || entry->type->kind == TypeKind::PROCEDURE) {
          is_factor_function_call.emplace(const_cast<Factor *>(&factor), true);
        } else {
          is_factor_function_call.emplace(const_cast<Factor *>(&factor), false);
        }
      }
      break;
  }
}

/**
 * @brief 访问并分析一个Term节点的语义。
 *
 * 此方法会依次处理Term中的第一个因子和其余因子，并根据操作符检查因子之间的类型兼容性。
 * 如果发现类型不匹配的情况，会报告语义错误。
 *
 * @param term 要分析的Term节点。
 *
 * 处理逻辑：
 * - 首先访问并获取第一个因子的类型。
 * - 遍历Term中的其余因子及其操作符，依次进行以下操作：
 *   - 访问当前因子并获取其类型。
 *   - 根据操作符检查第一个因子和当前因子的类型是否兼容：
 *     - 对于乘法 (*) 和实数除法 (/) 操作符，要求两侧操作数均为数值类型。
 *     - 对于整数除法 (DIV) 和取模 (MOD) 操作符，要求两侧操作数均为整数类型。
 *     - 对于逻辑与 (AND) 操作符，要求两侧操作数均为布尔类型。
 *   - 如果类型不兼容，报告语义错误。
 *   - 根据操作符更新第一个因子的类型，用于后续的链式操作。
 *
 * 错误处理：
 * - 如果无法确定某个因子的类型，会跳过该因子的类型检查。
 * - 如果发现类型不匹配，会调用SemanticError::report报告错误，并增加错误计数。
 */
void SemanticAnalyzer::visit(const Term &term) const {
  // 处理第一个因子
  term.getFirstFactor()->accept(*this);

  auto first_factor_type = getFactorType(term.getFirstFactor().get());
  if (!first_factor_type) return;  // 如果无法确定第一个因子的类型，退出

  // 处理其余因子及其操作符
  for (const auto &[op, factor] : term.getFactors()) {
    factor->accept(*this);

    // 获取当前因子的类型
    auto current_factor_type = getFactorType(factor.get());
    if (!current_factor_type) continue;  // 如果无法确定当前因子的类型，跳过类型检查

    // 检查操作符两侧的类型是否兼容
    switch (op) {
      case Term::MulOp::TIMES:  // 乘法
      case Term::MulOp::RDIV:   // 实数除法
        // 乘法和实数除法需要两边都是数值类型(整数或实数)
        if (!isNumericType(first_factor_type) || !isNumericType(current_factor_type)) {
          SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                                std::string("Operator '") + (op == Term::MulOp::TIMES ? "*" : "/") +
                                    "' requires numeric operands, but got '" + getTypeName(first_factor_type) +
                                    "' and '" + getTypeName(current_factor_type) + "'",
                                0);
          error_count++;
        }
        // 更新左操作数类型为结果类型，用于后续的链式操作
        first_factor_type = getNumericResultType(first_factor_type, current_factor_type);
        break;

      case Term::MulOp::DIV:  // 整数除法
      case Term::MulOp::MOD:  // 取模
        // 整数除法和取模需要两边都是整数类型
        if (!isIntegerType(first_factor_type) || !isIntegerType(current_factor_type)) {
          SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                                std::string("Operator '") + (op == Term::MulOp::DIV ? "DIV" : "MOD") +
                                    "' requires integer operands, but got '" + getTypeName(first_factor_type) +
                                    "' and '" + getTypeName(current_factor_type) + "'",
                                0);
          error_count++;
        }
        // DIV和MOD的结果总是整数
        first_factor_type = integer_type;
        break;

      case Term::MulOp::AND:  // 逻辑与
        // 逻辑与需要两边都是布尔类型
        if ((!isBooleanType(first_factor_type) || !isBooleanType(current_factor_type)) &&
            (!isIntegerType(first_factor_type) || !isIntegerType(current_factor_type))) {
          SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                                "Operator 'AND' requires boolean operands, but got '" + getTypeName(first_factor_type) +
                                    "' and '" + getTypeName(current_factor_type) + "'",
                                0);
          error_count++;
        }
        // AND的结果总是布尔值
        first_factor_type = boolean_type;
        break;
    }
  }
}

/**
 * @brief 访问简单表达式节点的函数。
 *
 * 此函数用于语义分析阶段，处理简单表达式节点。
 *
 * @param simple_expression 表示简单表达式的节点对象。
 */
void SemanticAnalyzer::visit(const SimpleExpression &simple_expression) const {
  // 处理第一个项
  simple_expression.getFirstTerm()->accept(*this);

  auto first_term_type = getTermType(simple_expression.getFirstTerm().get());
  if (!first_term_type) return;  // 如果无法确定第一个项的类型，退出

  // 处理其余项及其操作符
  for (const auto &[op, term] : simple_expression.getTerms()) {
    term->accept(*this);

    // 获取当前项的类型
    auto current_term_type = getTermType(term.get());
    if (!current_term_type) continue;  // 如果无法确定当前项的类型，跳过类型检查

    // 检查操作符两侧的类型是否兼容
    switch (op) {
      case SimpleExpression::AddOp::PLUS:   // 加法
      case SimpleExpression::AddOp::MINUS:  // 减法
        // 加法和减法需要两边都是数值类型(整数或实数)
        if (!isNumericType(first_term_type) || !isNumericType(current_term_type)) {
          SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                                std::string("Operator '") + (op == SimpleExpression::AddOp::PLUS ? "+" : "-") +
                                    "' requires numeric operands, but got '" + getTypeName(first_term_type) +
                                    "' and '" + getTypeName(current_term_type) + "'",
                                0);
          error_count++;
        }
        // 更新左操作数类型为结果类型，用于后续的链式操作
        first_term_type = getNumericResultType(first_term_type, current_term_type);
        break;

      case SimpleExpression::AddOp::OR:  // 逻辑或
        // 逻辑或需要两边都是布尔类型
        if ((!isBooleanType(first_term_type) || !isBooleanType(current_term_type)) &&
            (!isIntegerType(first_term_type) || !isIntegerType(current_term_type))) {
          SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                                "Operator 'OR' requires boolean operands, but got '" + getTypeName(first_term_type) +
                                    "' and '" + getTypeName(current_term_type) + "'",
                                0);
          error_count++;
        }
        // OR的结果总是布尔值
        first_term_type = boolean_type;
        break;
    }
  }
}

/**
 * @brief 访问表达式节点并进行语义分析。
 *
 * 此函数用于处理表达式节点的语义分析，包括检查表达式两侧的类型是否兼容。
 * 它支持关系操作符（如 >, <, >=, <=）和等值操作符（如 =, <>）的类型检查。
 *
 * @param expression 表达式节点，包含左侧和右侧的简单表达式以及操作符。
 *
 * @note 如果表达式的类型不兼容，将报告语义错误并增加错误计数。
 *
 * 处理逻辑：
 * - 首先访问左侧简单表达式。
 * - 如果右侧表达式存在，则访问右侧表达式并检查两侧类型的兼容性。
 * - 对于关系操作符（>, <, >=, <=），要求操作数是可比较的数值类型或完全相同的类型。
 * - 对于等值操作符（=, <>），允许更宽松的比较，包括数值类型、相同类型或布尔类型。
 * - 如果类型不兼容，将报告错误并记录详细信息。
 */
void SemanticAnalyzer::visit(const Expression &expression) const {
  // 处理左侧简单表达式
  expression.getLeft()->accept(*this);

  // 处理右侧简单表达式（如果存在）
  if (expression.getRight()) {
    expression.getRight()->second->accept(*this);

    // 检查关系操作符两侧的类型是否兼容
    auto left_type = getSimpleExpressionType(expression.getLeft().get());
    auto right_type = getSimpleExpressionType(expression.getRight()->second.get());

    if (!left_type || !right_type) {
      return;  // 如果无法确定类型，跳过检查
    }

    // 获取操作符类型
    auto op = expression.getRight()->first;

    // 对于比较操作符(>, <, >=, <=)，通常要求操作数是可比较类型
    if (op == Expression::RelOp::GT || op == Expression::RelOp::LT || op == Expression::RelOp::GE ||
        op == Expression::RelOp::LE) {
      // 数值类型之间可以比较
      const bool are_numeric = isNumericType(left_type) && isNumericType(right_type);
      // 完全相同类型可以比较
      const bool are_same_type = areTypesCompatible(left_type, right_type) || areTypesCompatible(right_type, left_type);

      if (!are_numeric && !are_same_type) {
        std::string op_str = Utils::getRelOpName(op);
        SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                              "Operator '" + op_str + "' requires compatible operands, but got '" +
                                  getTypeName(left_type) + "' and '" + getTypeName(right_type) + "'",
                              0);
        error_count++;
      }
    }
    // 对于等于和不等于操作符(=, <>)，允许更宽松的比较
    else if (op == Expression::RelOp::EQ || op == Expression::RelOp::NE) {
      // 同类型或数值类型之间可以比较
      const bool are_numeric = isNumericType(left_type) && isNumericType(right_type);
      const bool are_same_type = areTypesCompatible(left_type, right_type) || areTypesCompatible(right_type, left_type);

      if (const bool are_both_boolean = isBooleanType(left_type) && isBooleanType(right_type);
          !are_numeric && !are_same_type && !are_both_boolean) {
        const std::string op_str = (op == Expression::RelOp::EQ) ? "=" : "<>";

        SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                              "Operator '" + op_str + "' requires compatible operands, but got '" +
                                  getTypeName(left_type) + "' and '" + getTypeName(right_type) + "'",
                              0);
        error_count++;
      }
    }
  }
}

/**
 * @brief 尝试从表达式中获取常量值。
 *
 * 该函数用于从表达式中提取常量值，仅处理最简单的情况：
 * 表达式是一个简单表达式，且该简单表达式只有一个项，
 * 该项只有一个因子，且因子是整数字面量。
 *
 * @param expr 表达式的智能指针。
 * @return 如果表达式是常量值，则返回其值；否则返回 std::nullopt。
 *
 * @note 这是一个简化实现，需要根据实际的AST结构进行调整。
 */
auto SemanticAnalyzer::tryGetConstantValue(const ExpressionPtr &expr) -> std::optional<int> {
  // 这是一个简化实现，需要根据实际的AST结构调整
  // 只处理最简单的情况：直接是整数字面量的情况
  // 如果表达式只有一个简单表达式，且该简单表达式只有一个项，
  // 该项只有一个因子，且因子是整数字面量
  if (!expr->getRight().has_value()) {                                                   // 没有关系操作符
    if (const auto &simple_expr = expr->getLeft(); simple_expr->getTerms().empty()) {    // 只有第一个项
      if (const auto &term = simple_expr->getFirstTerm(); term->getFactors().empty()) {  // 只有第一个因子
        if (const auto &factor = term->getFirstFactor(); factor->getType() == FactorType::NUMBER) {
          try {
            const auto value_str = std::get<std::string>(factor->getValue());
            return std::stoi(value_str);
          } catch (...) {
            return std::nullopt;
          }
        }
      }
    }
  }
  return std::nullopt;
}

/**
 * @brief 判断给定的类型是否为数值类型。
 *
 * 此函数检查类型是否为基本类型（BASIC），并且其类型数据为 BaseType。
 * 如果类型的基本类型是整数（INTEGER）或实数（REAL），则认为该类型是数值类型。
 *
 * @param type 要检查的类型，表示为一个指向 SymbolType 的共享指针。
 * @return 如果类型是数值类型，则返回 true；否则返回 false。
 */
auto SemanticAnalyzer::isNumericType(const std::shared_ptr<SymbolType> &type) -> bool {
  return type->kind == TypeKind::BASIC && std::holds_alternative<BaseType>(type->type_data) &&
         (std::get<BaseType>(type->type_data).base_kind == BaseTypeKind::INTEGER ||
          std::get<BaseType>(type->type_data).base_kind == BaseTypeKind::REAL);
}

/**
 * @brief 判断给定的类型是否为整数类型。
 *
 * 此函数检查传入的类型是否为基本类型（BASIC），并且其基础类型（BaseType）的种类为整数类型（INTEGER）。
 *
 * @param type 一个指向 SymbolType 的共享指针，表示需要检查的类型。
 * @return 如果类型是整数类型，返回 true；否则返回 false。
 */
auto SemanticAnalyzer::isIntegerType(const std::shared_ptr<SymbolType> &type) -> bool {
  return type->kind == TypeKind::BASIC && std::holds_alternative<BaseType>(type->type_data) &&
         std::get<BaseType>(type->type_data).base_kind == BaseTypeKind::INTEGER;
}

/**
 * @brief 判断给定的类型是否为布尔类型。
 *
 * 此函数检查传入的类型是否为基本类型，并且其基本类型种类为布尔类型。
 *
 * @param type 一个指向 SymbolType 的共享指针，表示需要检查的类型。
 * @return 如果类型是布尔类型，则返回 true；否则返回 false。
 */
auto SemanticAnalyzer::isBooleanType(const std::shared_ptr<SymbolType> &type) -> bool {
  return type->kind == TypeKind::BASIC && std::holds_alternative<BaseType>(type->type_data) &&
         std::get<BaseType>(type->type_data).base_kind == BaseTypeKind::BOOLEAN;
}

/**
 * @brief 根据基本类型获取符号类型的共享指针。
 *
 * 此函数根据传入的基本类型（BasicType），返回对应的符号类型（SymbolType）的共享指针。
 * 如果传入的基本类型不在已定义的范围内，则返回空指针（nullptr）。
 *
 * @param basic_type 基本类型的枚举值（BasicType）。
 * @return std::shared_ptr<SymbolType> 对应符号类型的共享指针，如果类型无效则返回nullptr。
 */
auto SemanticAnalyzer::getSymbolTypeFromBasicType(BasicType basic_type) const -> std::shared_ptr<SymbolType> {
  switch (basic_type) {
    case BasicType::INTEGER:
      return integer_type;
    case BasicType::REAL:
      return real_type;
    case BasicType::BOOLEAN:
      return boolean_type;
    case BasicType::CHAR:
      return char_type;
    default:
      return nullptr;
  }
}

/**
 * @brief 处理类型节点，生成对应的符号类型。
 *
 * @param type_node 表示类型的节点对象。
 * @return std::shared_ptr<SymbolType> 返回生成的符号类型指针，如果处理失败则返回nullptr。
 *
 * 此函数根据类型节点的信息生成符号类型。如果类型是基本类型，则直接返回对应的符号类型。
 * 如果类型是数组类型，则解析数组的维度信息并生成数组类型的符号。
 *
 * @note 数组维度的上下界目前假设为整数字面量，非法的数组边界会报告语义错误。
 *
 * 错误处理：
 * - 如果基本类型无法解析，返回nullptr。
 * - 如果数组维度的上下界非法（非整数字面量），报告语义错误并返回nullptr。
 */
auto SemanticAnalyzer::processType(const Type &type_node) const -> std::shared_ptr<SymbolType> {
  // 获取基本类型
  auto basic_type = getSymbolTypeFromBasicType(type_node.getBasicType());
  if (!basic_type) return nullptr;

  // 检查是否有数组维度
  const auto &periods = type_node.getPeriods();
  if (periods.empty()) {
    // 非数组类型，直接返回基本类型
    return basic_type;
  }

  // 处理数组类型
  std::vector<std::pair<int, int>> dims;
  for (const auto &[lower, upper] : periods) {
    // 解析上下界（这里简化处理，假设都是整数字面量）
    int lower_bound, upper_bound;
    try {
      lower_bound = std::stoi(lower);
      upper_bound = std::stoi(upper);
      // 上界和下界必须是递增的关系
      if (lower_bound > upper_bound) {
        SemanticError::report(SemanticError::ErrorType::OTHER_ERROR, "Invalid array bounds: " + lower + ".." + upper,
                              0  // 此处需要行号信息
        );
      }
    } catch (const std::exception &) {
      // 非法数组边界
      SemanticError::report(SemanticError::ErrorType::OTHER_ERROR, "Illegal array boundary: " + lower + ".." + upper,
                            0  // 此处需要行号信息
      );
      error_count++;
      return nullptr;
    }

    dims.emplace_back(lower_bound, upper_bound);
  }

  // 创建数组类型
  return SymbolType::createArrayType(dims, basic_type);
}

/**
 * @brief 获取表达式的类型。
 *
 * 此函数根据传入的表达式，判断并返回其对应的类型。
 *
 * @param expr 指向表达式的指针。
 * @return std::shared_ptr<SymbolType> 表达式的类型。
 *         如果是简单表达式，返回其左操作数的类型；
 *         如果是关系表达式，返回布尔类型。
 */
auto SemanticAnalyzer::getExpressionType(const Expression *expr) const -> std::shared_ptr<SymbolType> {
  // 如果是简单表达式
  if (!expr->getRight().has_value()) {
    return getSimpleExpressionType(expr->getLeft().get());
  }

  // 如果是关系表达式，返回布尔类型
  return boolean_type;
}

/**
 * @brief 获取简单表达式的类型。
 *
 * 此函数用于分析简单表达式的类型，并根据表达式的组成部分和操作符确定最终的类型。
 *
 * @param simple_expr 指向简单表达式的指针。
 * @return std::shared_ptr<SymbolType> 表示简单表达式的类型。
 *
 * @details
 * - 如果简单表达式只有一个项，则直接返回该项的类型。
 * - 如果简单表达式包含多个项，则根据操作符和项的类型进行处理：
 *   - 对于逻辑或操作符（OR），要求两侧操作数均为布尔类型，否则报告类型错误。
 *   - 对于加法（PLUS）和减法（MINUS）操作符，要求两侧操作数为数值类型，并通过类型提升规则确定结果类型。
 * - 如果遇到类型不匹配的情况，会报告语义错误并增加错误计数。
 *
 * @note OR操作的结果类型始终为布尔类型。
 *
 * @exception SemanticError 如果操作数类型不匹配，则报告语义错误。
 */
auto SemanticAnalyzer::getSimpleExpressionType(const SimpleExpression *simple_expr) const
    -> std::shared_ptr<SymbolType> {
  if (simple_expr->getTerms().empty()) {
    return getTermType(simple_expr->getFirstTerm().get());
  }
  auto type = getTermType(simple_expr->getFirstTerm().get());
  for (const auto &[op, term] : simple_expr->getTerms()) {
    auto term_type = getTermType(term.get());

    bool flag1, flag2;
    if (op == SimpleExpression::AddOp::OR) {
      flag1 = (std::holds_alternative<BaseType>(type->type_data) &&
               std::get<BaseType>(type->type_data).base_kind == BaseTypeKind::BOOLEAN &&
               std::holds_alternative<BaseType>(term_type->type_data) &&
               std::get<BaseType>(term_type->type_data).base_kind == BaseTypeKind::BOOLEAN);
      flag2 = (std::holds_alternative<BaseType>(type->type_data) &&
               std::get<BaseType>(type->type_data).base_kind == BaseTypeKind::INTEGER &&
               std::holds_alternative<BaseType>(term_type->type_data) &&
               std::get<BaseType>(term_type->type_data).base_kind == BaseTypeKind::INTEGER);

      if (flag1 == false && flag2 == false) {
        SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH,
                              "Logical OR operator requires boolean or integer operands", 0);
        error_count++;
      }
      if (flag1) {
        type = boolean_type;
      } else {
        type = integer_type;
      }
    } else {
      type = getNumericResultType(type, term_type);
    }
  }

  return type;
}

/**
 * @brief 获取给定术语（Term）的类型。
 *
 * 此函数根据术语的因子（Factor）计算并返回其对应的符号类型（SymbolType）。
 * 如果术语只有一个因子，则直接返回该因子的类型。
 * 如果术语包含多个因子，则会依次处理每个因子，并根据需要进行类型推导或检查。
 *
 * @param term 指向术语（Term）的指针，用于获取其类型。
 * @return std::shared_ptr<SymbolType> 返回表示术语类型的共享指针。
 */
auto SemanticAnalyzer::getTermType(const Term *term) const -> std::shared_ptr<SymbolType> {
  // 如果只有第一个因子
  if (term->getFactors().empty()) {
    return getFactorType(term->getFirstFactor().get());
  }
  // TODO
  // 处理有多个因子的情况
  auto type = getFactorType(term->getFirstFactor().get());
  for (const auto &factor : term->getFactors() | std::ranges::views::values) {
    auto factor_type = getFactorType(factor.get());
  }

  return type;
}

/**
 * @brief 获取因子的类型。
 *
 * 此函数根据因子的类型（FactorType）确定其对应的符号类型（SymbolType）。
 * 它支持处理数字字面量、变量、表达式、函数调用、带符号的因子（如负号或NOT操作）等多种情况。
 *
 * @param factor 指向因子对象的指针。如果因子为空，返回nullptr。
 * @return std::shared_ptr<SymbolType> 返回因子的符号类型。如果无法确定类型，返回nullptr。
 *
 * @note 支持以下因子类型：
 * - NUMBER: 根据数字字面量的格式（是否包含小数点）返回整数类型或浮点数类型。
 * - VARIABLE: 查找变量的符号表条目，返回变量类型。如果是数组且有索引，返回数组元素类型。
 * - EXPRESSION: 递归调用以获取表达式的类型。
 * - FUNCTION: 查找函数的符号表条目，返回函数的返回类型。
 * - WITH_UMINUS: 检查负号操作的操作数是否为数值类型，返回相同的数值类型。
 * - WITH_NOT: 检查NOT操作的操作数是否为布尔类型，返回布尔类型。
 * - UNKNOWN: 处理未知标识符，可能是变量或函数，返回其对应的类型。
 *
 * @warning 如果因子类型未知或无法解析，函数将返回nullptr。
 */
auto SemanticAnalyzer::getFactorType(const Factor *factor) const -> std::shared_ptr<SymbolType> {
  if (!factor) return nullptr;

  switch (factor->getType()) {
    case FactorType::NUMBER: {
      // 尝试确定数字字面量的类型
      auto value_str = std::get<std::string>(factor->getValue());
      // 检查是否包含小数点，判断是整数还是浮点数
      if (value_str.find('.') != std::string::npos) {
        return real_type;
      } else {
        return integer_type;
      }
    }

    case FactorType::VARIABLE: {
      // 处理变量因子
      const auto &var_ptr = std::get<ASTNodePtr>(factor->getValue());
      auto var = dynamic_cast<const Variable *>(var_ptr.get());
      if (!var) return nullptr;

      // 查找变量的符号表条目
      auto var_name = var->getId();
      auto var_entry_opt = scope_stack.current()->recursiveLookup(var_name);
      if (!var_entry_opt) return nullptr;  // 变量未定义

      auto var_type = var_entry_opt.value()->type;

      // 如果变量是数组且有索引，返回数组元素类型
      if (var_type->kind == TypeKind::ARRAY && !var->getExpressions().empty()) {
        return std::get<ArrayType>(var_type->type_data).element_type;
      }

      return var_type;
    }

    case FactorType::EXPRESSION: {
      // 处理表达式因子
      const auto &expr = std::get<ASTNodePtr>(factor->getValue());
      auto expr_ptr = dynamic_cast<const Expression *>(expr.get());
      if (!expr_ptr) return nullptr;

      return getExpressionType(expr_ptr);
    }

    case FactorType::FUNCTION: {
      // 处理函数调用
      const auto &func_call = std::get<ASTNodePtr>(factor->getValue());
      auto proc_call = dynamic_cast<const ProcedureCall *>(func_call.get());
      if (!proc_call) return nullptr;

      // 查找函数的符号表条目
      const auto func_name = proc_call->getId();
      const auto func_entry_opt = scope_stack.current()->recursiveLookup(func_name);
      if (!func_entry_opt) return nullptr;  // 函数未定义

      const auto &func_entry = func_entry_opt.value();

      // 确保是函数类型
      if (func_entry->type->kind != TypeKind::FUNCTION) return nullptr;

      // 获取函数返回类型
      const auto callable_type = std::get<CallableType>(func_entry->type->type_data);
      if (!callable_type.return_type) return nullptr;

      return callable_type.return_type;
    }

    case FactorType::WITH_PLUS:
    case FactorType::WITH_UMINUS: {
      // 负号应用于数值类型
      const auto &operand = std::get<ASTNodePtr>(factor->getValue());
      auto operand_ptr = dynamic_cast<const Factor *>(operand.get());
      if (!operand_ptr) return nullptr;

      auto operand_type = getFactorType(operand_ptr);

      // 检查是否为数值类型
      if (!operand_type || !std::holds_alternative<BaseType>(operand_type->type_data) ||
          (std::get<BaseType>(operand_type->type_data).base_kind != BaseTypeKind::INTEGER &&
           std::get<BaseType>(operand_type->type_data).base_kind != BaseTypeKind::REAL)) {
        return nullptr;
      }

      // 返回相同的数值类型
      return operand_type;
    }

    case FactorType::WITH_NOT: {
      // NOT应用于布尔类型
      const auto &operand = std::get<ASTNodePtr>(factor->getValue());
      const auto operand_ptr = dynamic_cast<const Factor *>(operand.get());
      if (!operand_ptr) return nullptr;

      // 检查是否为布尔类型
      const auto operand_type = getFactorType(operand_ptr);
      if (!operand_type) {
        return nullptr;
      }
      if ((std::holds_alternative<BaseType>(operand_type->type_data) &&
           std::get<BaseType>(operand_type->type_data).base_kind == BaseTypeKind::INTEGER) ||
          (std::holds_alternative<ArrayType>(operand_type->type_data) &&
           std::get<ArrayType>(operand_type->type_data).element_type->kind == TypeKind::BASIC &&
           std::holds_alternative<BaseType>(std::get<ArrayType>(operand_type->type_data).element_type->type_data) &&
           std::get<BaseType>(std::get<ArrayType>(operand_type->type_data).element_type->type_data).base_kind ==
               BaseTypeKind::INTEGER)) {
        return integer_type;
      }

      if (std::get<BaseType>(operand_type->type_data).base_kind != BaseTypeKind::BOOLEAN) {
        return nullptr;
      }

      // NOT操作的结果是布尔类型
      return boolean_type;
    }

    case FactorType::BOOLEAN: {
      // 布尔字面量
      return boolean_type;
    }

    case FactorType::UNKNOWN: {
      // 处理未知类型（可能是变量或函数）
      const auto id = std::get<std::string>(factor->getValue());
      const auto entry_opt = scope_stack.current()->recursiveLookup(id);
      if (!entry_opt) return nullptr;  // 标识符未定义

      const auto &entry = entry_opt.value();

      // 如果是函数，返回函数返回类型
      if (entry->type->kind == TypeKind::FUNCTION) {
        auto callable_type = std::get<CallableType>(entry->type->type_data);
        return callable_type.return_type;
      }

      // 否则作为变量处理
      return entry->type;
    }

    default:
      return nullptr;
  }
}

/**
 * @brief 获取两个操作数的数值结果类型。
 *
 * 此函数用于确定两个操作数在数值运算中的结果类型。
 * 如果任一操作数不是数值类型（整数或实数），将报告类型不匹配错误，并返回默认的整型类型。
 * 如果任一操作数是实数，则结果类型为实数。
 * 如果两个操作数均为整数，则结果类型为整数。
 *
 * @param left 左操作数的类型，使用 std::shared_ptr<SymbolType> 表示。
 * @param right 右操作数的类型，使用 std::shared_ptr<SymbolType> 表示。
 * @return std::shared_ptr<SymbolType> 返回运算结果的类型。
 *         如果发生错误，默认返回整型类型。
 */
auto SemanticAnalyzer::getNumericResultType(const std::shared_ptr<SymbolType> &left,
                                            const std::shared_ptr<SymbolType> &right) const
    -> std::shared_ptr<SymbolType> {
  // 如果任一操作数不是数值类型，报错
  if ((!std::holds_alternative<BaseType>(left->type_data) ||
       (std::get<BaseType>(left->type_data).base_kind != BaseTypeKind::INTEGER &&
        std::get<BaseType>(left->type_data).base_kind != BaseTypeKind::REAL)) ||
      (!std::holds_alternative<BaseType>(right->type_data) ||
       (std::get<BaseType>(right->type_data).base_kind != BaseTypeKind::INTEGER &&
        std::get<BaseType>(right->type_data).base_kind != BaseTypeKind::REAL))) {
    SemanticError::report(SemanticError::ErrorType::TYPE_MISMATCH, "Arithmetic operation requires numeric operands", 0);
    error_count++;
    return integer_type;  // 默认返回整型
  }

  // 如果任一操作数是实数，结果是实数
  if ((std::holds_alternative<BaseType>(left->type_data) &&
       std::get<BaseType>(left->type_data).base_kind == BaseTypeKind::REAL) ||
      (std::holds_alternative<BaseType>(right->type_data) &&
       std::get<BaseType>(right->type_data).base_kind == BaseTypeKind::REAL)) {
    return real_type;
  }

  // 两个整数操作的结果是整数
  return integer_type;
}

/**
 * @brief 检查两个类型是否兼容。
 *
 * 此函数用于判断目标类型和源类型是否可以兼容，例如赋值操作中目标类型是否可以接受源类型的值。
 *
 * @param target_type 目标类型的智能指针。
 * @param source_type 源类型的智能指针。
 * @return bool 如果类型兼容返回 true，否则返回 false。
 *
 * @details
 * - 如果两种类型的种类（kind）相同：
 *   - 对于基本类型（BASIC），检查两者的基本类型是否相同，或者允许整数类型赋值给实数类型。
 *   - 对于数组类型（ARRAY），递归检查数组的元素类型是否兼容。
 *   - 如果两者都是函数或过程类型，则认为不兼容。
 * - 如果两种类型的种类不同，则认为不兼容。
 */
auto SemanticAnalyzer::areTypesCompatible(const std::shared_ptr<SymbolType> &target_type,
                                          const std::shared_ptr<SymbolType> &source_type) -> bool {
  // 如果类型完全相同的类型枚举，进一步检查
  if (target_type->kind == source_type->kind) {
    // 如果是基本类型，检查基本类型是否匹配
    if (target_type->kind == TypeKind::BASIC && std::holds_alternative<BaseType>(target_type->type_data) &&
        std::holds_alternative<BaseType>(source_type->type_data)) {
      // 检查基本类型是否相同
      auto flag1 =
          std::get<BaseType>(target_type->type_data).base_kind == std::get<BaseType>(source_type->type_data).base_kind;
      // 整数可以赋值给实数的特殊规则
      auto flag2 = std::get<BaseType>(target_type->type_data).base_kind == BaseTypeKind::REAL &&
                   std::get<BaseType>(source_type->type_data).base_kind == BaseTypeKind::INTEGER;
      return flag1 || flag2;
    }
    // 如果是数组类型，检查数组元素类型是否匹配
    if (target_type->kind == TypeKind::ARRAY && source_type->kind == TypeKind::ARRAY) {
      return areTypesCompatible(std::get<ArrayType>(target_type->type_data).element_type,
                                std::get<ArrayType>(source_type->type_data).element_type);
    }
    // 左右两边都是函数/过程类型，认为不兼容
    return false;
  }
  // 其他情况不兼容
  return false;
}

/**
 * @brief 获取符号类型的名称。
 *
 * 此函数根据传入的符号类型对象，返回其对应的类型名称字符串。
 *
 * @param type 指向符号类型的智能指针。如果类型为空，返回 "unknown"。
 * @return std::string 返回类型名称的字符串表示：
 * - 如果是基本类型（BASIC），返回对应的基本类型名称：
 *   - "integer" 表示整数类型。
 *   - "real" 表示实数类型。
 *   - "boolean" 表示布尔类型。
 *   - "char" 表示字符类型。
 *   - "unknown basic type" 表示未知的基本类型。
 * - 如果是其他类型，返回：
 *   - "array" 表示数组类型。
 *   - "function" 表示函数类型。
 *   - "procedure" 表示过程类型。
 *   - "basic type" 表示基本类型（兜底情况）。
 * - 如果类型未知，返回 "unknown"。
 */
auto SemanticAnalyzer::getTypeName(const std::shared_ptr<SymbolType> &type) -> std::string {
  if (!type) return "unknown";

  // 首先检查是否是基本类型
  if (type->kind == TypeKind::BASIC && std::holds_alternative<BaseType>(type->type_data)) {
    // 处理基本类型
    const auto &base_type = std::get<BaseType>(type->type_data);
    switch (base_type.base_kind) {
      case BaseTypeKind::INTEGER:
        return "integer";
      case BaseTypeKind::REAL:
        return "real";
      case BaseTypeKind::BOOLEAN:
        return "boolean";
      case BaseTypeKind::CHAR:
        return "char";
      default:
        return "unknown basic type";
    }
  }

  // 处理其他类型
  switch (type->kind) {
    case TypeKind::ARRAY:
      return "array";
    case TypeKind::FUNCTION:
      return "function";
    case TypeKind::PROCEDURE:
      return "procedure";
    case TypeKind::BASIC:
      return "basic type";  // 这里是兜底，正常应该在前面的if中就处理了
    default:
      return "unknown";
  }
}

auto SemanticAnalyzer::getIsFunctionReturn() const -> std::map<Assign *, bool> { return is_function_return; }

auto SemanticAnalyzer::getReadFmtSpecifier() const -> std::map<Read *, std::string> { return read_fmt_specifier; }

auto SemanticAnalyzer::getWriteFmtSpecifier() const -> std::map<Write *, std::string> { return write_fmt_specifier; }

auto SemanticAnalyzer::getIsFactorFunctionCall() const -> std::map<Factor *, bool> { return is_factor_function_call; }

auto SemanticAnalyzer::getIsFactorWithNotNumber() const -> std::map<Factor *, bool> {
  return is_factor_with_not_number;
}

auto SemanticAnalyzer::getIsVarParam() const -> std::map<std::string, std::vector<bool>> { return is_var_param; }

auto SemanticAnalyzer::getParamsName() const -> std::map<std::string, std::vector<std::string>> { return params_name; }