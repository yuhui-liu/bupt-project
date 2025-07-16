/**
 * @file   generator.hpp
 * @brief  Generator 类的声明。
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <map>
#include <string>

#include "ast.hpp"
#include "visitor.hpp"

/**
 * @class Generator
 * @brief Generator 类
 * @details Generator 类，继承自 Visitor 类，用于根据语义分析的增强AST生成C语言代码。
 */
class Generator : public Visitor {
 public:
  Generator() = default;
  /// @brief 构造函数
  Generator(const std::map<Assign *, bool> &is_function_return, const std::map<Read *, std::string> &read_fmt_specifier,
            const std::map<Write *, std::string> &write_fmt_specifier,
            std::map<Factor *, bool> &is_factor_function_call, std::map<Factor *, bool> &is_factor_with_not_number,
            std::map<std::string, std::vector<bool>> &is_var_param,
            std::map<std::string, std::vector<std::string>> &params_name)
      : is_function_return(is_function_return),
        read_fmt_specifier(read_fmt_specifier),
        write_fmt_specifier(write_fmt_specifier),
        is_factor_function_call(is_factor_function_call),
        is_factor_with_not_number(is_factor_with_not_number),
        is_var_param(is_var_param),
        params_name(params_name) {}
  /// @brief 打印 AST
  void print() const;
  /// @brief 获取打印结果
  auto getResult() const -> std::string;
  /// @brief 清空打印结果
  void clear();
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
  /// @brief 表示结果的字符串流
  mutable std::string result;
  /// @brief 当前缩进级别
  mutable int now_level = 0;
  mutable bool flag = false;
  mutable std::vector<std::pair<std::string,std::vector<std::string>>> arr_bias;
  mutable std::vector<std::string> tmp_periods;
  mutable int single_procedure = 0;
  /// @brief 标识一个赋值语句是否是函数返回值
  std::map<Assign *, bool> is_function_return;
  /// @brief 将read语句映射到对应的fmt specifier
  std::map<Read *, std::string> read_fmt_specifier;
  /// @brief 将write语句映射到对应的fmt specifier
  std::map<Write *, std::string> write_fmt_specifier;
  /// @brief 标识一个因子是否是函数调用
  std::map<Factor *, bool> is_factor_function_call;
  /// @brief 标识一个with_not类型的因子是否是数字
  std::map<Factor *, bool> is_factor_with_not_number;
  /// @brief 将子程序名映射到布尔值数组，表示参数是否为引用类型
  std::map<std::string, std::vector<bool>> is_var_param;
  /// @brief 将子程序名映射到参数名列表
  std::map<std::string, std::vector<std::string>> params_name;
  /// @brief 添加缩进
  void addIndent() const;
  /// @brief 指向当前正在访问的main复合语句
  mutable CompoundStatement *main_compound_statement = nullptr;
  /// @brief 指向当前正在访问的子程序
  mutable std::string now_in_which_subprogram;
};
#endif  // GENERATOR_HPP
