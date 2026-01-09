/**
 * @file interpreter.hpp
 * @brief Interpreter 类的声明。
 */
#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <utility>

#include "ast.hpp"
#include "sqlite_interface.hpp"
#include "visitor.hpp"

/**
 * @class   Interpreter
 * @brief   解释器类。解释程序并执行。
 * @details 解释器类，继承自 Visitor 类，用于解释程序并执行。
 */
class Interpreter final : public Visitor {
 public:
  /**
   * @brief 构造一个新的 Interpreter 对象。
   * @param program 待解释的程序。
   * @param mode 运行模式。可能的值为 "cli" 或 "browser"。
   * @param c 常量表。
   * @param p 过程表。
   */
  Interpreter(std::shared_ptr<Program> program, std::string mode, std::map<std::string, std::string> c,
              std::map<std::string, std::size_t> p)
      : mode(std::move(mode)), constants(std::move(c)), procedures(std::move(p)), program(std::move(program)) {}

  // Visitor 接口声明
  void visit(const Program&) const override;
  void visit(const ConstantDefine&) const override;
  void visit(const Procedure&) const override;
  void visit(const Say&) const override;
  void visit(const Listen&) const override;
  void visit(const Clause&) const override;
  void visit(const Lookup&) const override;
  void visit(const Exit&) const override;
  void visit(const Jump&) const override;
  void visit(const Identifier&) const override;
  void visit(const String&) const override;
  // Visitor 接口声明结束

  /// @brief 设置 send 回调函数
  void setSend(std::function<void(const std::string&)> s) { send = std::move(s); }
  void setInputFromNetwork(const std::string& input) const;

  /// @brief 设置用户id
  void setUid(const int userId) { uid = userId; }

 private:
  void clauseAction(const Clause& clause) const;
  /// @brief 标识当前Clause是否被接受
  mutable bool clause_accepted{false};

  /// @brief 当前运行模式
  std::string mode;
  /// @brief 常量表
  mutable std::map<std::string, std::string> constants;
  /// @brief 过程表
  std::map<std::string, std::size_t> procedures;
  /// @brief 待解释的程序
  std::shared_ptr<Program> program;

  void getCliInput() const;
  /// @brief 输入缓冲区
  mutable std::string input_buffer;
  /// @brief 收到输入标志
  mutable bool input_received{false};
  /// @brief 真实输入接收标志，用于同步时防止混淆
  mutable bool real_input_received{false};
  /// @brief 正在读取输入标志
  mutable std::atomic<bool> is_reading{false};
  /// @brief 互斥量，用于保护输入缓冲区
  mutable std::mutex mtx;
  /// @brief 条件变量，用于等待输入
  mutable std::condition_variable cv;

  void output() const;
  /// @brief 输出缓冲区
  mutable std::string output_buffer;

  /// @brief 数据库连接
  mutable std::map<std::string, std::unique_ptr<SQLiteDB>> dbs;

  static auto getIdOrString(const ASTNodePtr& p) -> std::string;
  /// @brief 回调函数，用于向网络发送数据
  std::function<void(const std::string&)> send;
  static constexpr int DEFAULTUID = 101;
  /// @brief 用户id
  int uid{DEFAULTUID};
};

#endif  // INTERPRETER_HPP
