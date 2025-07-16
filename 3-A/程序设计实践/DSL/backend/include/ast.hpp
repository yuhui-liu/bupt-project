/**
 * @file    ast.hpp
 * @brief   抽象语法树（Abstract Syntax Tree）结点类的定义
 * @details 定义了抽象语法树的各个结点
 */

#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * @class   ASTNode
 * @brief   抽象语法树结点
 * @details 定义了抽象语法树的结点，所有结点都继承自此类。包含一个accept方法，用于接受visitor。
 */
class ASTNode {
 public:
  virtual ~ASTNode() = default;
  /// @brief 接受visitor
  virtual void accept(const class Visitor& visitor) const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

/**
 * @class Program
 * @brief 表示程序的结点
 */
class Program final : public ASTNode {
 public:
  /// @brief 添加常量定义
  /// @param constant 常量定义
  void addConstant(ASTNodePtr constant);
  /// @brief 添加过程
  /// @param procedure 过程
  void addProcedure(ASTNodePtr procedure);
  void accept(const Visitor&) const override;
  // getters
  [[nodiscard]] auto getConstants() const -> const auto& { return constants; }
  [[nodiscard]] auto getProcedures() const -> const auto& { return procedures; }

 private:
  /// @brief program含有的常量定义
  std::vector<ASTNodePtr> constants;
  /// @brief program含有的过程
  std::vector<ASTNodePtr> procedures;
};

/**
 * @class ConstantDefine
 * @brief 表示常量定义的结点
 */
class ConstantDefine final : public ASTNode {
 public:
  /// @param id 常量名
  /// @param value 常量值
  /// @brief 构造函数，对于"id=value"进行构造。
  ConstantDefine(std::string id, std::string value) : id(std::move(id)), value(std::move(value)) {}
  void accept(const Visitor&) const override;

  // getters
  [[nodiscard]] auto getId() const -> const auto& { return id; }
  [[nodiscard]] auto getValue() const -> const auto& { return value; }

 private:
  /// @brief 常量名
  std::string id;
  /// @brief 常量值
  std::string value;
};

/**
 * @class Procedure
 * @brief 表示过程的结点
 */
class Procedure final : public ASTNode {
 public:
  /// @param id 过程名
  explicit Procedure(std::string id) : id(std::move(id)) {}
  void accept(const Visitor&) const override;
  /// @brief 添加语句
  void addStatement(ASTNodePtr statement);

  // getters
  [[nodiscard]] auto getStatements() const -> const auto& { return statements; }
  [[nodiscard]] auto getId() const -> const auto& { return id; }

 private:
  /// @brief 过程名
  std::string id;
  /// @brief 过程的语句
  std::vector<ASTNodePtr> statements;
};

/**
 * @class Say
 * @brief 表示say语句的结点
 */
class Say final : public ASTNode {
 public:
  void accept(const Visitor&) const override;
  /// @brief 添加内容
  void addContent(ASTNodePtr content);

  // getters
  [[nodiscard]] auto getContents() const -> const auto& { return content; }

 private:
  /// @brief say语句的内容
  std::vector<ASTNodePtr> content;
};

/**
 * @class Listen
 * @brief 表示listen语句的结点
 */
class Listen final : public ASTNode {
 public:
  /// @param limit listen语句的时间限制
  explicit Listen(const int limit) : limit(limit) {}
  void accept(const Visitor&) const override;
  /// @brief 添加子语句
  void addClause(ASTNodePtr clause);

  // getters
  [[nodiscard]] auto getClauses() const -> const auto& { return clauses; }
  [[nodiscard]] auto getLimit() const -> int { return limit; }

 private:
  /// @brief listen语句的子语句
  std::vector<ASTNodePtr> clauses;
  /// @brief listen语句的时间限制
  int limit;
};

/**
 * @class Clause
 * @brief 表示listen语句的子语句的结点
 */
class Clause final : public ASTNode {
 public:
  /// @brief 子语句的类型枚举
  enum class Type { HAS, ANYTHING, TIMEOUT, DEFAULT };
  /// @brief 子语句的动作枚举
  enum class Action { WRITETO, JUMP };
  /// @brief 子语句的类型
  Type type;
  /// @brief 子语句的动作
  Action action;

  // setters
  void setType(Type type);
  void setAction(Action action);
  void setBranch(const std::string& branch);
  void setPattern(ASTNodePtr pattern);
  void setDb(const std::string& db);
  void setTable(const std::string& table);
  void setColumn(const std::string& column);

  // getters
  [[nodiscard]] auto getType() const -> const auto& { return type; }
  [[nodiscard]] auto getAction() const -> const auto& { return action; }
  [[nodiscard]] auto getBranch() const -> const auto& { return branch; }
  [[nodiscard]] auto getPattern() const -> const auto& { return pattern; }
  [[nodiscard]] auto getDb() const -> const auto& { return db; }
  [[nodiscard]] auto getTable() const -> const auto& { return table; }
  [[nodiscard]] auto getColumn() const -> const auto& { return column; }

  void accept(const Visitor&) const override;

 private:
  /// @brief 当动作为JUMP时，branch为跳转的过程
  std::string branch;
  /// @brief 当动作为WRITETO时，db为写入的数据库
  std::string db;
  /// @brief 当动作为WRITETO时，table为写入的表
  std::string table;
  /// @brief 当动作为WRITETO时，column为写入的列
  std::string column;
  /// @brief 当类型为HAS时，pattern为匹配的模式
  ASTNodePtr pattern;
};

/**
 * @class Lookup
 * @brief 表示lookup语句的结点
 */
class Lookup final : public ASTNode {
 public:
  /// @param value lookup语句查询的值
  /// @param db lookup语句查询的数据库
  /// @param table lookup语句查询的表
  Lookup(std::string value, std::string db, std::string table)
      : value(std::move(value)), db(std::move(db)), table(std::move(table)) {}
  void accept(const Visitor&) const override;

  // getters
  [[nodiscard]] auto getValue() const -> const auto& { return value; }
  [[nodiscard]] auto getDb() const -> const auto& { return db; }
  [[nodiscard]] auto getTable() const -> const auto& { return table; }

 private:
  /// @brief lookup语句查询的值
  std::string value;
  /// @brief lookup语句查询的数据库
  std::string db;
  /// @brief lookup语句查询的表
  std::string table;
};

/**
 * @class Exit
 * @brief 表示exit语句的结点
 */
class Exit final : public ASTNode {
 public:
  void accept(const Visitor&) const override;
};

/**
 * @class Jump
 * @brief 表示jump语句的结点
 */
class Jump final : public ASTNode {
 public:
  /// @param id jump语句跳转的过程
  explicit Jump(std::string id) : id(std::move(id)) {}
  void accept(const Visitor&) const override;

  // getters
  [[nodiscard]] auto getId() const -> const auto& { return id; }

 private:
  /// @brief jump语句跳转的过程
  std::string id;
};

/**
 * @class Identifier
 * @brief 表示Identifier的结点
 */
class Identifier final : public ASTNode {
 public:
  /// @param id 标识符的名字
  explicit Identifier(std::string id) : id(std::move(id)) {}
  void accept(const Visitor&) const override;

  // getters
  [[nodiscard]] auto getId() const -> const auto& { return id; }

 private:
  /// @brief 标识符的名字
  std::string id;
};

/**
 * @class String
 * @brief 表示字符串字面量的结点
 */
class String final : public ASTNode {
 public:
  /// @param s 字符串字面量
  explicit String(std::string s) : s(std::move(s)) {}
  void accept(const Visitor&) const override;

  // getters
  [[nodiscard]] auto getString() const -> const auto& { return s; }

 private:
  /// @brief 字符串字面量
  std::string s;
};
#endif  // AST_HPP
