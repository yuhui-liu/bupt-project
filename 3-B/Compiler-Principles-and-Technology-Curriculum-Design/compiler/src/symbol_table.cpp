/**
 * @file symbol_table.cpp
 * @brief 符号表和类型定义实现
 * @details 包括符号表、作用域栈、类型检查等功能的实现
 */
#include "symbol_table.hpp"

#include <iostream>
#include <utility>

/**
 * @brief 报告具有特定类型、消息和行号的语义错误。
 *
 * 此函数将详细的语义错误消息输出到标准错误流中。
 * 错误消息包括错误类型、可选的行号以及自定义消息。
 *
 * @param type 语义错误的类型，由 `ErrorType` 枚举表示。
 * @param message 提供有关错误的详细信息的描述性消息。
 * @param line_number 错误发生的行号。如果值小于或等于0，则输出中不会包含行号。
 */
void SemanticError::report(ErrorType type, const std::string& message, int line_number) {
  std::string error_type_str;
  switch (type) {
    case ErrorType::DUPLICATE_DEFINITION:
      error_type_str = "Redefinition";
      break;
    case ErrorType::UNDEFINED_SYMBOL:
      error_type_str = "Undefined symbol";
      break;
    case ErrorType::SCOPE_VIOLATION:
      error_type_str = "Scope violation";
      break;
    case ErrorType::VAR_PARAM_ERROR:
      error_type_str = "Var Parameter error";
      break;
    case ErrorType::TYPE_MISMATCH:
      error_type_str = "Type mismatch";
      break;
    case ErrorType::ARRAY_INDEX_OUT_OF_BOUNDS:
      error_type_str = "Array out of bounds";
      break;
    case ErrorType::CONSTANT_ASSIGNMENT:
      error_type_str = "Constant assignment error";
      break;
    case ErrorType::OTHER_ERROR:
      error_type_str = "Other error";
      break;
  }

  std::cerr << "[Semantic Error] " << error_type_str;
  if (line_number > 0) {
    std::cerr << " in " << line_number << " line";
  }
  std::cerr << ": " << message << std::endl;
}

/**
 * @brief 创建一个基本类型的符号类型。
 *
 * 此函数用于创建一个表示基本类型的 `SymbolType` 对象。
 *
 * @param base_kind 基本类型的种类，表示为 `BaseTypeKind` 枚举值。
 * @return 一个指向新创建的 `SymbolType` 对象的共享指针。
 */
auto SymbolType::createBasicType(BaseTypeKind base_kind) -> std::shared_ptr<SymbolType> {
  auto type = std::make_shared<SymbolType>();
  type->kind = TypeKind::BASIC;
  type->type_data = BaseType{base_kind};
  return type;
}

/**
 * @brief 创建一个数组类型的符号类型。
 *
 * @param dims 数组的维度信息，每个维度由一个包含下界和上界的整数对表示。
 * @param element_type 数组的元素类型，表示数组中每个元素的类型。
 * @return std::shared_ptr<SymbolType> 返回一个表示数组类型的SymbolType智能指针。
 *
 * 此函数根据提供的维度信息和元素类型，创建并返回一个表示数组类型的SymbolType对象。
 * 数组的总元素个数通过遍历维度信息计算得出。
 */
auto SymbolType::createArrayType(const std::vector<std::pair<int, int>>& dims, std::shared_ptr<SymbolType> element_type)
    -> std::shared_ptr<SymbolType> {
  auto type = std::make_shared<SymbolType>();
  type->kind = TypeKind::ARRAY;

  // 计算数组的总元素个数
  int total_count = 1;
  for (const auto& d : dims) {
    const int lower = d.first;
    const int upper = d.second;
    total_count *= (upper - lower + 1);
  }

  ArrayType arr{dims, std::move(element_type)};
  type->type_data = arr;
  return type;
}

/**
 * @brief 创建一个可调用类型（函数或过程）的符号类型。
 *
 * @param is_function 指定是否为函数类型。如果为 false，则为过程类型。
 * @param return_type 函数的返回类型。如果是过程类型，此参数应为 nullptr。
 * @param parameters 参数列表，包含每个参数的符号条目。
 * @param local_scope 本地作用域的符号表，用于嵌套函数或过程。
 * @return 返回一个表示可调用类型的 SymbolType 智能指针。
 */
auto SymbolType::createCallableType(bool is_function, std::shared_ptr<SymbolType> return_type,
                                    const std::vector<SymbolEntryPtr>& parameters,
                                    std::shared_ptr<SymbolTable> local_scope) -> std::shared_ptr<SymbolType> {
  auto type = std::make_shared<SymbolType>();
  type->kind = is_function ? TypeKind::FUNCTION : TypeKind::PROCEDURE;

  // procedure 的 return_type 为 nullptr
  CallableType callable{std::move(return_type), parameters, std::move(local_scope)};
  type->type_data = callable;
  return type;
}

/**
 * @brief SymbolEntry 构造函数，用于初始化符号表条目。
 *
 * @param name 符号的名称。
 * @param type 符号的类型，使用智能指针管理的 SymbolType 对象。
 * @param scope_level 符号所在的作用域级别。
 *
 * @note 构造函数中一些成员变量被初始化为默认值：
 *       - is_constant 默认为 false，表示符号是否为常量。
 *       - is_reference 默认为 false，表示符号是否为引用。
 *       - line_number 默认为 0，表示符号所在的行号。
 *       - return_type 默认为 " "，表示符号的返回类型。
 */
SymbolEntry::SymbolEntry(std::string name, std::shared_ptr<SymbolType> type, int scope_level)
    : name(std::move(name)),
      type(std::move(type)),
      scope_level(scope_level),
      is_constant(false),
      is_reference(false),
      line_number(0),
      return_type(" ") {}

/**
 * @brief 构造函数，用于初始化符号表。
 *
 * @param scope_level 当前符号表的作用域级别。
 * @param parent_table 指向父符号表的共享指针，用于表示当前符号表的父级关系。
 */
SymbolTable::SymbolTable(const int scope_level, const std::shared_ptr<SymbolTable>& parent_table)
    : current_scope(scope_level), parent(parent_table) {}

/**
 * @brief 向符号表中插入一个符号条目。
 *
 * 如果符号表中已经存在具有相同名称的符号条目，则报告重复定义错误并返回 false。
 * 否则，将符号条目插入符号表并返回 true。
 *
 * @param entry 要插入的符号条目，包含符号的名称和其他相关信息。
 * @return bool 如果插入成功返回 true；如果发生重复定义错误返回 false。
 *
 * @note 如果发生重复定义错误，会调用 SemanticError::report 方法报告错误。
 */
auto SymbolTable::insert(const SymbolEntryPtr& entry) -> bool {
  auto it = entries.find(entry->name);
  if (it != entries.end()) {
    // 重复定义错误
    SemanticError::report(SemanticError::ErrorType::DUPLICATE_DEFINITION,
                          "identifier '" + entry->name + "' is already defined in the current scope",
                          entry->line_number);
    return false;
  }
  entries[entry->name] = entry;
  return true;
}

/**
 * @brief 在符号表中查找指定名称的条目
 *
 * @param name 要查找的符号名称
 * @return std::optional<SymbolEntryPtr> 如果找到匹配的符号则返回包含符号指针的optional，否则返回空optional
 */
auto SymbolTable::lookup(const std::string& name) const -> std::optional<SymbolEntryPtr> {
  if (const auto it = entries.find(name); it != entries.end()) {
    return it->second;
  }
  return std::nullopt;
}

/**
 * @brief 递归查找符号表中的符号项
 *
 * 首先在当前符号表中查找，如果找不到则递归地在父符号表中查找
 *
 * @param name 要查找的符号名
 * @return std::optional<SymbolEntryPtr> 如果找到则返回符号项指针，否则返回空
 */
auto SymbolTable::recursiveLookup(const std::string& name) const -> std::optional<SymbolEntryPtr> {
  if (auto entry = lookup(name)) {
    return entry;
  }
  if (const auto parent_ptr = parent.lock()) {
    return parent_ptr->recursiveLookup(name);
  }
  return std::nullopt;
}

/**
 * @brief 获取当前符号表的作用域
 *
 * @return int 返回当前作用域的标识符，该标识符为一个整数值
 */
auto SymbolTable::getCurrentScope() const -> int { return current_scope; }

/**
 * @brief 将一个新的符号表作用域压入栈中。
 *
 * 此函数会创建一个新的符号表，并将其添加到作用域栈的顶部。
 * 如果当前栈为空，则新符号表的父作用域为 nullptr；
 * 否则，新符号表的父作用域为当前栈顶的符号表。
 * 新符号表的层级根据栈的大小自动设置。
 */
void ScopeStack::push() {
  SymbolTablePtr parent = stack.empty() ? nullptr : stack.back();
  // 以栈的大小为新的作用域层级
  stack.push_back(std::make_shared<SymbolTable>(static_cast<int>(stack.size()), parent));
}

/**
 * @brief 从作用域栈中弹出顶部作用域。
 *
 * 如果作用域栈不为空，则移除栈顶的作用域。
 * 如果作用域栈为空，则此函数不执行任何操作。
 */
void ScopeStack::pop() {
  if (!stack.empty()) {
    stack.pop_back();
  }
}

/**
 * @brief 获取当前作用域的符号表。
 *
 * 如果作用域栈为空，则返回空指针；否则，返回栈顶的符号表。
 *
 * @return 当前作用域的符号表指针，如果栈为空则返回 nullptr。
 */
auto ScopeStack::current() const -> SymbolTablePtr { return stack.empty() ? nullptr : stack.back(); }

/**
 * @brief 判断作用域栈是否为空。
 *
 * @return 如果作用域栈为空，则返回 true；否则返回 false。
 */
auto ScopeStack::empty() const -> bool { return stack.empty(); }

/**
 * @brief 判断符号是否在当前作用域栈中可访问。
 *
 * @param symbol 指向符号条目的智能指针。
 * @return true 如果符号在当前作用域或外部作用域中可访问。
 * @return false 如果符号不可访问。
 *
 * @details
 * 此函数通过比较符号的作用域级别和当前作用域级别来判断符号的可访问性。
 * - 如果符号的作用域级别小于或等于当前作用域级别，则符号可访问。
 * - 全局符号（作用域级别为0）对所有作用域可见。
 * - 局部符号仅对定义它的作用域及其嵌套作用域可见。
 *
 * @note 如果作用域栈为空，则任何符号都不可访问。
 */
auto ScopeStack::isAccessible(const SymbolEntryPtr& symbol) const -> bool {
  if (stack.empty()) return false;

  // 获取符号的作用域级别
  const int symbol_scope = symbol->scope_level;

  // 获取当前作用域级别
  const int current_scope = stack.back()->getCurrentScope();

  // 如果符号的作用域级别小于或等于当前作用域级别，则可访问
  // 全局符号（作用域级别为0）对所有作用域可见
  // 局部符号只对定义它的作用域和内部嵌套作用域可见
  return symbol_scope <= current_scope;
}