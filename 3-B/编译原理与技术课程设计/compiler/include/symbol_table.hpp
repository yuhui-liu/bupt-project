/**
 * @file symbol_table.hpp
 * @brief 符号表和类型定义
 */
#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// 前向声明
class SymbolType;
class SymbolEntry;
class SymbolTable;
class ScopeStack;

// 类型别名定义
using ConstantValue = std::variant<int, float, bool, char, std::string, std::vector<int>>;
using SymbolEntryPtr = std::shared_ptr<SymbolEntry>;
using SymbolTablePtr = std::shared_ptr<SymbolTable>;

// 类型种类枚举
enum class TypeKind {
  BASIC,      // integer/real/boolean/char
  ARRAY,      // array
  PROCEDURE,  // procedure
  FUNCTION    // function
};

// 基础类型枚举
enum class BaseTypeKind { INTEGER, REAL, BOOLEAN, CHAR };

// 基础类型结构体
struct BaseType {
  BaseTypeKind base_kind;  // 基础类型种类
};

// 数组类型结构体（示例：可用一组<下界,上界>表示多维下标范围）
struct ArrayType {
  // 为了兼容类似 array [1..10] of integer 或多维，使用一个维度列表
  std::vector<std::pair<int, int>> dimensions;
  std::shared_ptr<SymbolType> element_type;
};

// 统一的可调用类型（过程/函数）
struct CallableType {
  // procedure 对应 return_type = nullptr
  // function 则存放返回类型
  std::shared_ptr<SymbolType> return_type;

  // 参数列表（每个参数是一个符号条目，可在符号条目里标注是否为 reference 等）
  std::vector<SymbolEntryPtr> parameters;

  // 关联的局部符号表
  SymbolTablePtr local_scope;
};

/**
 * @class SymbolType
 * @brief 符号类型类
 *
 * 该类用于表示符号的类型，包括基础类型、数组类型和可调用类型（过程/函数）。
 */
class SymbolType {
 public:
  TypeKind kind;
  std::variant<BaseType, ArrayType, CallableType> type_data;

  // 创建基础类型的工厂方法
  static auto createBasicType(BaseTypeKind base_kind) -> std::shared_ptr<SymbolType>;

  // 创建数组类型的工厂方法（根据dimensions和元素类型）
  static auto createArrayType(const std::vector<std::pair<int, int>>& dims, std::shared_ptr<SymbolType> element_type)
      -> std::shared_ptr<SymbolType>;

  // 创建过程或函数类型的工厂方法
  // 可以根据是否有返回类型来区分 procedure / function
  static auto createCallableType(bool is_function, std::shared_ptr<SymbolType> return_type,
                                 const std::vector<SymbolEntryPtr>& parameters,
                                 std::shared_ptr<SymbolTable> local_scope) -> std::shared_ptr<SymbolType>;

  // 添加虚析构函数以使类成为多态类型
  virtual ~SymbolType() = default;
};

/**
 * @class SymbolEntry
 * @brief 符号条目类
 *
 * 该类用于表示符号表中的每个条目，包括标识符名称、类型、作用域层级等信息。
 */
class SymbolEntry {
 public:
  std::string name;                  // 标识符名称
  std::shared_ptr<SymbolType> type;  // 类型指针
  int scope_level;                   // 作用域层级
  bool is_constant;                  // 是否为常量
  ConstantValue const_value;         // 常量值
  bool is_reference;                 // 是否为引用参数(var参数)
  int line_number;                   // 源码行号
  std::string return_type;           // 返回值类型

  // 构造函数
  SymbolEntry(std::string name, std::shared_ptr<SymbolType> type, int scope_level);
};

/**
 * @class SymbolTable
 * @brief 符号表类
 *
 * 该类用于表示一个作用域的符号表，支持插入、查找和递归查找符号。
 */
class SymbolTable {
 private:
  std::unordered_map<std::string, SymbolEntryPtr> entries;  // 符号条目哈希表
  int current_scope;                                        // 当前作用域层级
  std::weak_ptr<SymbolTable> parent;                        // 父作用域符号表(弱引用)

 public:
  SymbolTable(int scope_level, const std::shared_ptr<SymbolTable>& parent_table);

  // 向当前符号表插入一个符号，如果已存在则报错
  auto insert(const SymbolEntryPtr& entry) -> bool;

  // 在当前符号表中查找符号
  auto lookup(const std::string& name) const -> std::optional<SymbolEntryPtr>;

  // 递归查找符号(包括父作用域)
  auto recursiveLookup(const std::string& name) const -> std::optional<SymbolEntryPtr>;

  // 获取当前作用域层级
  auto getCurrentScope() const -> int;
};

/**
 * @class ScopeStack
 * @brief 作用域栈类
 *
 * 该类用于管理作用域栈，支持进入新作用域、退出当前作用域和检查符号访问权限等操作。
 */
class ScopeStack {
 private:
  std::vector<SymbolTablePtr> stack;  // 符号表栈

 public:
  // 进入新作用域
  void push();

  // 退出当前作用域
  void pop();

  // 获取当前作用域符号表
  [[nodiscard]] auto current() const -> SymbolTablePtr;

  // 检查栈是否为空
  [[nodiscard]] auto empty() const -> bool;

  // 检查符号在当前作用域中是否可访问（作用域违规检查）
  [[nodiscard]] auto isAccessible(const SymbolEntryPtr& symbol) const -> bool;
};

/**
 * @class SemanticError
 * @brief 语义错误处理类
 *
 * 该类用于记录和报告语义错误，包括重复定义、未定义符号、作用域违规等。
 */
class SemanticError {
 public:
  enum class ErrorType {
    DUPLICATE_DEFINITION,       // 重复定义
    UNDEFINED_SYMBOL,           // 未定义符号
    SCOPE_VIOLATION,            // 作用域违规
    VAR_PARAM_ERROR,            // var参数错误
    TYPE_MISMATCH,              // 类型不匹配
    ARRAY_INDEX_OUT_OF_BOUNDS,  // 数组越界
    CONSTANT_ASSIGNMENT,        // 常量赋值
    OTHER_ERROR                 // 其他错误
  };

  // 记录一个错误
  static void report(ErrorType type, const std::string& message, int line_number = 0);
};

#endif  // SYMBOL_TABLE_HPP