/**
 * @file    ast.hpp
 * @brief   抽象语法树（Abstract Syntax Tree）结点类的定义
 * @details 定义了抽象语法树的各个结点
 */

#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

/**
 * @class   ASTNode
 * @brief   抽象语法树结点
 * @details
 * 定义了抽象语法树的结点，所有结点都继承自此类。包含一个accept方法，用于接受visitor。
 */
class ASTNode {
 public:
  virtual ~ASTNode() = default;
  /// @brief 接受visitor
  virtual void accept(const class Visitor& visitor) const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

// 这里是前向声明，以便使用
class Program;
class Subprogram;
class Parameter;
class ConstDecl;
class VarDecl;
class Type;
class Variable;
class Statement;
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
using ProgramPtr = std::unique_ptr<Program>;
using SubprogramPtr = std::unique_ptr<Subprogram>;
using ParameterPtr = std::unique_ptr<Parameter>;
using ConstDeclPtr = std::unique_ptr<ConstDecl>;
using VarDeclPtr = std::unique_ptr<VarDecl>;
using TypePtr = std::unique_ptr<Type>;
using VariablePtr = std::unique_ptr<Variable>;
using StatementPtr = std::unique_ptr<Statement>;
using NullStatementPtr = std::unique_ptr<NullStatement>;
using AssignPtr = std::unique_ptr<Assign>;
using ProcedureCallPtr = std::unique_ptr<ProcedureCall>;
using CompoundStatementPtr = std::unique_ptr<CompoundStatement>;
using IfPtr = std::unique_ptr<If>;
using ForPtr = std::unique_ptr<For>;
using WhilePtr = std::unique_ptr<While>;
using ReadPtr = std::unique_ptr<Read>;
using WritePtr = std::unique_ptr<Write>;
using BreakPtr = std::unique_ptr<Break>;
using FactorPtr = std::unique_ptr<Factor>;
using TermPtr = std::unique_ptr<Term>;
using SimpleExpressionPtr = std::unique_ptr<SimpleExpression>;
using ExpressionPtr = std::unique_ptr<Expression>;

/// @brief 基本类型枚举
enum class BasicType { INTEGER, REAL, BOOLEAN, CHAR };

/**
 * @class Program
 * @brief 表示一个程序的ast结点
 *
 * 包含程序的标识符、参数列表、常量声明、变量声明、子程序列表和复合语句。
 */
class Program final : public ASTNode {
 public:
  /// @param program_id 程序标识符
  explicit Program(std::string program_id) : program_id(std::move(program_id)) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getProgramId() const -> const std::string&;
  [[nodiscard]] auto getParameters() const -> const std::vector<std::string>&;
  [[nodiscard]] auto getConstDeclarations() const -> const std::vector<ConstDeclPtr>&;
  [[nodiscard]] auto getVarDeclarations() const -> const std::vector<VarDeclPtr>&;
  [[nodiscard]] auto getSubprograms() const -> const std::vector<SubprogramPtr>&;
  [[nodiscard]] auto getCompoundStatement() const -> const CompoundStatementPtr&;

  void addParameter(const std::string& parameter);
  void addConstDeclaration(ConstDeclPtr const_decl);
  void addVarDeclaration(VarDeclPtr var_decl);
  void addSubprogram(SubprogramPtr subprogram);
  void setCompoundStatement(CompoundStatementPtr cp);

 private:
  std::string program_id;                       ///< 程序标识符
  std::vector<std::string> parameters;          ///< 程序参数，位于括号中
  std::vector<ConstDeclPtr> const_declaration;  ///< 常量声明
  std::vector<VarDeclPtr> var_declaration;      ///< 变量声明
  std::vector<SubprogramPtr> subprograms;       ///< 子程序
  CompoundStatementPtr compound_statement;      ///< 程序主体
};

/**
 * @brief 表示一个子程序（函数或过程）的抽象语法树节点。
 *
 * @details 该类包含子程序的标识符、参数列表、返回类型、常量声明、变量声明以及复合语句等信息。
 */
class Subprogram final : public ASTNode {
 public:
  /// @param id 子程序标识符
  /// @param is_func 是否为函数
  Subprogram(std::string id, bool is_func) : id(std::move(id)), is_func(is_func) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getId() const -> const std::string&;
  [[nodiscard]] auto getParameters() const -> const std::vector<ParameterPtr>&;
  [[nodiscard]] auto getReturnType() const -> const BasicType&;
  [[nodiscard]] auto isFunction() const -> bool;
  [[nodiscard]] auto getConstDeclarations() const -> const std::vector<ConstDeclPtr>&;
  [[nodiscard]] auto getVarDeclarations() const -> const std::vector<VarDeclPtr>&;
  [[nodiscard]] auto getCompoundStatement() const -> const CompoundStatementPtr&;
  void addParameter(ParameterPtr parameter);
  void setReturnType(BasicType ret_type);
  void addConstDeclaration(ConstDeclPtr const_decl);
  void addVarDeclaration(VarDeclPtr var_decl);
  void setCompoundStatement(CompoundStatementPtr cp);

 private:
  std::string id;                               ///< 子程序标识符
  std::vector<ParameterPtr> parameters;         ///< 子程序参数列表
  BasicType ret_type;                           ///< 返回类型
  bool is_func;                                 ///< 是否为函数
  std::vector<ConstDeclPtr> const_declaration;  ///< 常量声明
  std::vector<VarDeclPtr> var_declaration;      ///< 变量声明
  CompoundStatementPtr compound_statement;      ///< 子程序主体
};

/**
 * @class Parameter
 * @brief 表示一个参数的ast结点
 *
 * 包含参数的标识符、基本类型和是否含有`var`关键字。
 */
class Parameter final : public ASTNode {
 public:
  /// @param has_var 是否含有`var`关键字
  explicit Parameter(bool has_var) : has_var(has_var) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto isVar() const -> bool;
  [[nodiscard]] auto getIdList() const -> const std::vector<std::string>&;
  [[nodiscard]] auto getBasicType() const -> const BasicType&;
  void addId(const std::string& id);
  void setBasicType(BasicType basic_type);
  auto checkSame(const Parameter* target) const -> bool;

 private:
  bool has_var;                      ///< 是否含有`var`关键字
  std::vector<std::string> id_list;  ///< 参数标识符列表
  BasicType basic_type;              ///< 参数基本类型
};

/**
 * @class ConstDecl
 * @brief 表示一个常量声明的ast结点
 *
 * 包含常量的标识符和常量值。
 */
class ConstDecl final : public ASTNode {
 public:
  /// @param id 常量标识符
  /// @param value 常量值
  ConstDecl(std::string id, std::string value) : id(std::move(id)), value(std::move(value)) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getId() const -> const std::string&;
  [[nodiscard]] auto getValue() const -> const std::string&;

 private:
  std::string id;     ///< 常量标识符
  std::string value;  ///< 常量值
};

/**
 * @class VarDecl
 * @brief 表示一个变量声明的ast结点
 *
 * 包含变量的标识符列表和类型。
 */
class VarDecl final : public ASTNode {
 public:
  VarDecl() = default;
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getIdList() const -> const std::vector<std::string>&;
  [[nodiscard]] auto getType() const -> const TypePtr&;
  void addId(const std::string& id);
  void setType(TypePtr type);

 private:
  std::vector<std::string> id_list;  ///< 变量标识符列表
  TypePtr type;                      ///< 变量类型
};

/**
 * @class Type
 * @brief 表示一个类型的ast结点
 *
 * 包含基本类型和区间。
 * 如果区间不为空，则是数组类型。
 */
class Type final : public ASTNode {
 public:
  Type() = default;
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getPeriods() const -> const std::vector<std::pair<std::string, std::string>>&;
  [[nodiscard]] auto getBasicType() const -> const BasicType&;
  void addPeriod(const std::string& lower, const std::string& upper);
  void setBasicType(BasicType basic_type);

 private:
  std::vector<std::pair<std::string, std::string>> periods;  ///< 区间列表
  BasicType basic_type;                                      ///< 基本类型
};

/**
 * @class Variable
 * @brief 表示一个变量的ast结点
 *
 * 包含变量的标识符和表达式列表。
 */
class Variable final : public ASTNode {
 public:
  /// @param id 变量标识符
  explicit Variable(std::string id) : id(std::move(id)) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getId() const -> const std::string&;
  [[nodiscard]] auto getExpressions() const -> const std::vector<ExpressionPtr>&;
  void addExpression(ExpressionPtr expression);
  auto checkSame(const Variable* target) const -> bool;

 private:
  std::string id;                          ///< 变量标识符
  std::vector<ExpressionPtr> expressions;  ///< 表达式列表
};

/**
 * @class Statement
 * @brief 表示一个语句的ast结点
 *
 * 所有语句都继承自此类。
 */
class Statement : public ASTNode {};

/**
 * @class NullStatement
 * @brief 表示一个空语句的ast结点
 *
 * 该类表示一个空语句，也就是只有分号的语句。
 */
class NullStatement final : public Statement {
 public:
  void accept(const Visitor& visitor) const override;
};

/**
 * @class Assign
 * @brief 表示一个赋值语句的ast结点
 *
 * 包含赋值号左侧的变量和右侧的表达式。
 */
class Assign final : public Statement {
 public:
  /// @param left 赋值号左侧的变量
  /// @param right 赋值号右侧的表达式
  Assign(VariablePtr left, ExpressionPtr right) : left(std::move(left)), right(std::move(right)) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getLeft() const -> const VariablePtr&;
  [[nodiscard]] auto getRight() const -> const ExpressionPtr&;

 private:
  VariablePtr left;     ///< 赋值号左侧的变量
  ExpressionPtr right;  ///< 赋值号右侧的表达式
};

/**
 * @class ProcedureCall
 * @brief 表示一个过程调用的ast结点
 *
 * 包含过程的标识符和参数列表。
 */
class ProcedureCall final : public Statement {
 public:
  /// @param id 过程标识符
  explicit ProcedureCall(std::string id) : id(std::move(id)) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getId() const -> const std::string&;
  [[nodiscard]] auto getParameters() const -> const std::vector<ExpressionPtr>&;
  void addParameter(ExpressionPtr parameter);
  auto checkSame(const ProcedureCall* target) const -> bool;

 private:
  std::string id;                         ///< 过程标识符
  std::vector<ExpressionPtr> parameters;  ///< 参数列表
};

/**
 * @class CompoundStatement
 * @brief 表示一个复合语句的ast结点
 *
 * 包含多个语句。
 */
class CompoundStatement final : public Statement {
 public:
  CompoundStatement() = default;
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getStatements() const -> const std::vector<StatementPtr>&;
  void addStatement(StatementPtr statement);

 private:
  std::vector<StatementPtr> statements;  ///< 语句列表
};

/**
 * @class If
 * @brief 表示一个if语句的ast结点
 *
 * 包含条件表达式、then语句和else语句。
 */
class If final : public Statement {
 public:
  /// @param condition 条件表达式
  /// @param then_statement then语句
  /// @param else_statement else语句
  If(ExpressionPtr condition, StatementPtr then_statement, StatementPtr else_statement)
      : condition(std::move(condition)),
        then_statement(std::move(then_statement)),
        else_statement(std::move(else_statement)) {}
  /// @param condition 条件表达式
  /// @param then_statement then语句
  If(ExpressionPtr condition, StatementPtr then_statement)
      : condition(std::move(condition)), then_statement(std::move(then_statement)), else_statement(nullptr) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getCondition() const -> const ExpressionPtr&;
  [[nodiscard]] auto getThenStatement() const -> const StatementPtr&;
  [[nodiscard]] auto getElseStatement() const -> const StatementPtr&;

 private:
  ExpressionPtr condition;      ///< 条件表达式
  StatementPtr then_statement;  ///< then语句
  StatementPtr else_statement;  ///< else语句
};

/**
 * @class For
 * @brief 表示一个for语句的ast结点
 *
 * 包含循环变量、下界、上界和循环体。
 */
class For final : public Statement {
 public:
  /// @param id 循环变量
  explicit For(std::string id) : id(std::move(id)) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getId() const -> const std::string&;
  [[nodiscard]] auto getLowerBound() const -> const ExpressionPtr&;
  [[nodiscard]] auto getUpperBound() const -> const ExpressionPtr&;
  [[nodiscard]] auto getStatement() const -> const StatementPtr&;
  void setLowerBound(ExpressionPtr lb);
  void setUpperBound(ExpressionPtr ub);
  void setStatement(StatementPtr statement);

 private:
  std::string id;          ///< 循环变量
  ExpressionPtr lb, ub;    ///< 下界和上界
  StatementPtr statement;  ///< 循环体
};

/**
 * @class While
 * @brief 表示一个while语句的ast结点
 *
 * 包含条件表达式和循环体。
 */
class While final : public Statement {
 public:
  /// @param condition 条件表达式
  /// @param statement 循环体
  While(ExpressionPtr condition, StatementPtr statement)
      : condition(std::move(condition)), statement(std::move(statement)) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getCondition() const -> const ExpressionPtr&;
  [[nodiscard]] auto getStatement() const -> const StatementPtr&;

 private:
  ExpressionPtr condition;  ///< 条件表达式
  StatementPtr statement;   ///< 循环体
};

/**
 * @class Read
 * @brief 表示一个read语句的ast结点
 *
 * 包含多个变量，每个变量对应一个输入。
 */
class Read final : public Statement {
 public:
  Read() = default;
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getVariables() const -> const std::vector<VariablePtr>&;
  void addVariable(VariablePtr variable);

 private:
  std::vector<VariablePtr> variables;  ///< 变量列表
};

/**
 * @class Write
 * @brief 表示一个write语句的ast结点
 *
 * 包含多个表达式，每个表达式对应一个输出。
 */
class Write final : public Statement {
 public:
  Write() = default;
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getExpressions() const -> const std::vector<ExpressionPtr>&;
  void addExpression(ExpressionPtr expression);

 private:
  std::vector<ExpressionPtr> expressions;  ///< 表达式列表
};

enum class FactorType {
  NUMBER,       // 数字
  WITH_UMINUS,  // 带前缀负号
  WITH_PLUS,    // 带前缀正号
  WITH_NOT,     // 带前缀not
  VARIABLE,     // 变量类型
  EXPRESSION,   // 表达式类型
  FUNCTION,     // 函数类型
  BOOLEAN,      // 布尔类型
  UNKNOWN,      // 可能是variable或function，无法在语法分析时确定
};

/**
 * @class Break
 * @brief 表示一个break语句的ast结点
 */
class Break final : public Statement {
 public:
  Break() = default;
  void accept(const Visitor& visitor) const override;
};

/**
 * @class Factor
 * @brief 表示一个因子的ast结点
 *
 * 包含因子的类型和具体值。
 */
class Factor final : public ASTNode {
 public:
  /// @param type 因子类型
  explicit Factor(FactorType type) : type(type) {}
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getType() const -> const FactorType&;
  [[nodiscard]] auto getValue() const -> const std::variant<std::string, ASTNodePtr>&;
  template <typename T>
  std::enable_if_t<std::is_same_v<T, std::string> || std::is_same_v<T, const char*> || std::is_same_v<T, ASTNodePtr>,
                   void>
  setValue(T value) {
    this->value = std::move(value);
  }
  auto checkSame(const Factor* t) const -> bool;

 private:
  FactorType type;                              ///< 因子类型
  std::variant<std::string, ASTNodePtr> value;  ///< 因子值，是一个可变量，可以是字符串或ASTNode指针
};

/**
 * @class Term
 * @brief 表示一个项的ast结点
 *
 * 包含第一个因子和后续因子列表，其中后续因子与第一个因子之间有乘法、除法、取模等操作符。
 */
class Term final : public ASTNode {
 public:
  Term() = default;
  enum class MulOp { TIMES, RDIV, DIV, MOD, AND };
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getFirstFactor() const -> const FactorPtr&;
  [[nodiscard]] auto getFactors() const -> const std::vector<std::pair<MulOp, FactorPtr>>&;
  void setFirstFactor(FactorPtr firstFactor);
  void addFactor(MulOp op, FactorPtr factor);
  auto checkSame(const Term* t) const -> bool;

 private:
  FactorPtr firstFactor;                             ///< 第一个因子
  std::vector<std::pair<MulOp, FactorPtr>> factors;  ///< 后续因子列表
};

/**
 * @class SimpleExpression
 * @brief 表示一个简单表达式的ast结点
 *
 * 包含第一个项和后续项列表，其中后续项与第一个项之间有加法、减法、或等操作符。
 */
class SimpleExpression final : public ASTNode {
 public:
  SimpleExpression() = default;
  enum class AddOp { PLUS, MINUS, OR };
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getFirstTerm() const -> const TermPtr&;
  [[nodiscard]] auto getTerms() const -> const std::vector<std::pair<AddOp, TermPtr>>&;
  void setFirstTerm(TermPtr firstTerm);
  void addTerm(AddOp op, TermPtr term);
  auto checkSame(const SimpleExpression* t) const -> bool;

 private:
  TermPtr firstTerm;                             ///< 第一个项
  std::vector<std::pair<AddOp, TermPtr>> terms;  ///< 后续项列表
};

/**
 * @class Expression
 * @brief 表示一个表达式的ast结点
 *
 * 包含左侧简单表达式和右侧简单表达式，以及它们之间的关系操作符。
 */
class Expression final : public ASTNode {
 public:
  Expression() = default;
  enum class RelOp { GT, EQ, LE, GE, LT, NE };
  void accept(const Visitor& visitor) const override;
  [[nodiscard]] auto getLeft() const -> const SimpleExpressionPtr&;
  [[nodiscard]] auto getRight() const -> const std::optional<std::pair<RelOp, SimpleExpressionPtr>>&;
  void setLeft(SimpleExpressionPtr left);
  void setRight(RelOp op, SimpleExpressionPtr right);
  auto checkSame(const Expression* t) const -> bool;

 private:
  SimpleExpressionPtr left;                                    ///< 左侧简单表达式
  std::optional<std::pair<RelOp, SimpleExpressionPtr>> right;  ///< 右侧简单表达式，可能为空
};

#endif  // AST_HPP
