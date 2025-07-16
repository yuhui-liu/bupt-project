/**
 * @file semantic_analyzer.hpp
 * @brief 语义分析器类的声明
 */

#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include <map>
#include <optional>
#include <string>

#include "ast.hpp"
#include "symbol_table.hpp"
#include "visitor.hpp"

/**
 * @class   SemanticAnalyzer
 * @brief   语义分析器
 * @details
 * 实现了Visitor接口，用于遍历AST，构建符号表并进行语义检查
 */
class SemanticAnalyzer final : public Visitor {
 public:
  SemanticAnalyzer();

  // 获取分析结果的符号表
  auto getGlobalSymbolTable() const -> SymbolTablePtr;

  // 检查是否有语义错误
  auto hasError() const -> bool;

  // 实现Visitor接口的visit方法
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

  auto getIsFunctionReturn() const -> std::map<Assign *, bool>;
  auto getReadFmtSpecifier() const -> std::map<Read *, std::string>;
  auto getWriteFmtSpecifier() const -> std::map<Write *, std::string>;
  auto getIsFactorFunctionCall() const -> std::map<Factor *, bool>;
  auto getIsFactorWithNotNumber() const -> std::map<Factor *, bool>;
  auto getIsVarParam() const -> std::map<std::string, std::vector<bool>>;
  auto getParamsName() const -> std::map<std::string, std::vector<std::string>>;

 private:
  // Helper methods
  static auto isNumericType(const std::shared_ptr<SymbolType> &type) -> bool;
  static auto isIntegerType(const std::shared_ptr<SymbolType> &type) -> bool;
  static auto isBooleanType(const std::shared_ptr<SymbolType> &type) -> bool;
  auto getSymbolTypeFromBasicType(BasicType basic_type) const -> std::shared_ptr<SymbolType>;
  auto processType(const Type &type_node) const -> std::shared_ptr<SymbolType>;
  auto getExpressionType(const Expression *expr) const -> std::shared_ptr<SymbolType>;
  auto getSimpleExpressionType(const SimpleExpression *simple_expr) const -> std::shared_ptr<SymbolType>;
  auto getTermType(const Term *term) const -> std::shared_ptr<SymbolType>;
  auto getFactorType(const Factor *factor) const -> std::shared_ptr<SymbolType>;
  auto getNumericResultType(const std::shared_ptr<SymbolType> &left, const std::shared_ptr<SymbolType> &right) const
      -> std::shared_ptr<SymbolType>;
  static auto areTypesCompatible(const std::shared_ptr<SymbolType> &target_type,
                                 const std::shared_ptr<SymbolType> &source_type) -> bool;
  static auto getTypeName(const std::shared_ptr<SymbolType> &type) -> std::string;
  static auto tryGetConstantValue(const ExpressionPtr &expr) -> std::optional<int>;

  // Member variables
  std::shared_ptr<SymbolType> integer_type;
  std::shared_ptr<SymbolType> real_type;
  std::shared_ptr<SymbolType> boolean_type;
  std::shared_ptr<SymbolType> char_type;

  /// @brief 作用域栈
  mutable ScopeStack scope_stack;

  /// @brief 错误计数器
  mutable int error_count = 0;

  /// @brief 将 Assign 指针映射到一个布尔值，标识该赋值语句是否为函数返回
  mutable std::map<Assign *, bool> is_function_return;

  /// @brief 将 Read 指针映射到其在C语言中相应的格式说明符
  mutable std::map<Read *, std::string> read_fmt_specifier;

  /// @brief 将 Write 指针映射到其在C语言中相应的格式说明符
  mutable std::map<Write *, std::string> write_fmt_specifier;

  /// @brief 将 Unknown 的 Factor 指针映射到一个布尔值，标识该因子是否为函数调用（如果不是则为变量）
  mutable std::map<Factor *, bool> is_factor_function_call;

  /// @brief 将 With_Not 的 Factor 指针映射到一个布尔值，标识该因子后面的值是不是数值
  mutable std::map<Factor *, bool> is_factor_with_not_number;

  /// @brief 将函数名映射到一个布尔值数组，标识该函数的参数是否为引用参数（var参数）
  mutable std::map<std::string, std::vector<bool>> is_var_param;

  /// @brief 将函数名映射到一个字符串数组，标识该函数的参数名称
  mutable std::map<std::string, std::vector<std::string>> params_name;
};

#endif  // SEMANTIC_HPP