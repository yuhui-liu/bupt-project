/**
 * @file   interpreter.cpp
 * @brief  Interpreter 类的实现。
 */

#include "interpreter.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "ast.hpp"
#include "exception.hpp"

/**
 * @brief 访问程序节点
 *
 * 该方法用于访问程序节点，并调用main过程的accept方法。
 *
 * @param program 要访问的程序节点
 */
void Interpreter::visit(const Program& program) const {
  program.getProcedures().at(procedures.at("main"))->accept(*this);
}

void Interpreter::visit(const ConstantDefine&) const {}  // don't need to implement

/**
 * @brief 访问过程节点
 *
 * 该方法用于访问过程节点，并调用每个语句的accept方法。
 *
 * @param procedure 要访问的过程节点
 */
void Interpreter::visit(const Procedure& procedure) const {
  for (const auto& statement : procedure.getStatements()) {
    statement->accept(*this);
  }
}

/**
 * @brief 访问Say节点
 *
 * 该方法用于访问Say节点，并调用每个内容的accept方法。
 * 结束后输出缓冲区的内容。
 *
 * @param say 要访问的Say节点
 */
void Interpreter::visit(const Say& say) const {
  for (const auto& content : say.getContents()) {
    content->accept(*this);
  }
  output();
}

/**
 * @brief 访问Listen节点
 *
 * 该方法用于访问Listen节点，并根据模式获取输入。
 * 然后遍历每个Clause节点，根据输入的状态执行相应的操作。
 * 使用条件变量cv等待输入。
 *
 * @param listen 要访问的Listen节点
 */
void Interpreter::visit(const Listen& listen) const {
  const auto t = listen.getLimit();
  if (mode == "cli") {
    getCliInput();
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait_for(lock, std::chrono::seconds(t));
    real_input_received = input_received;
  } else {
    input_received = false;
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait_for(lock, std::chrono::seconds(t));
    real_input_received = input_received;
  }
  clause_accepted = false;
  for (const auto& clause : listen.getClauses()) {
    clause->accept(*this);
    if (clause_accepted) {
      break;
    }
  }
}

/**
 * @brief 访问Clause节点
 *
 * 该方法用于访问Clause节点，并根据输入的状态执行相应的操作。
 * - 如果没有接收到输入，且Clause节点的类型是TIMEOUT，则执行操作。
 * - 如果接收到输入，且Clause节点的类型是ANYTHING或DEFAULT，则执行操作。
 * - 如果接收到输入，且Clause节点的类型是HAS，则检查输入是否包含指定的模式，如果包含则执行操作。
 *
 * @param clause 要访问的Clause节点
 */
void Interpreter::visit(const Clause& clause) const {
  if (!real_input_received) {
    if (clause.getType() == Clause::Type::TIMEOUT) {
      clauseAction(clause);
    }
  } else {
    if (clause.getType() == Clause::Type::ANYTHING || clause.getType() == Clause::Type::DEFAULT) {
      clauseAction(clause);
    } else if (clause.getType() == Clause::Type::HAS) {
      if (input_buffer.find(getIdOrString(clause.getPattern())) != std::string::npos) {
        clauseAction(clause);
      }
    }
  }
}

/**
 * @brief 执行Clause节点的操作
 *
 * 根据Clause节点的类型执行相应的操作。
 * - 如果是JUMP类型，则跳转到指定的过程。
 * - 如果是WRITETO类型，则将输入写入指定的数据库。
 *
 * @param clause 要执行操作的Clause节点
 *
 * @throw std::runtime_error 如果指定的过程不存在。
 */
void Interpreter::clauseAction(const Clause& clause) const {
  if (clause.getAction() == Clause::Action::JUMP) {
    const auto& id = clause.getBranch();
    if (!procedures.contains(id)) {
      throw std::runtime_error("Undefined identifier: " + id);
    }
    program->getProcedures().at(procedures.at(id))->accept(*this);
  } else {
    const auto& dbName = clause.getDb();
    std::string dbPath;
    if (constants.contains(dbName)) {
      dbPath = constants.at(dbName);
    } else {
      throw std::runtime_error("Undefined identifier: " + dbName);
    }
    if (!dbs.contains(dbPath)) {
      dbs.emplace(dbPath, std::make_unique<SQLiteDB>(dbPath));
    }
    if (!dbs.at(dbPath)->write(uid, {clause.getTable(), clause.getColumn()}, input_buffer)) {
      spdlog::warn("write to fail");
    }
  }
  clause_accepted = true;
}

/**
 * @brief 访问Lookup节点
 *
 * 该方法用于访问Lookup节点，并从指定的数据库中查询值。
 *
 * @param lookup 要访问的Lookup节点
 *
 * @throw std::runtime_error 如果查询的数据库不存在。
 */
void Interpreter::visit(const Lookup& lookup) const {
  const auto& dbName = lookup.getDb();
  std::string dbPath;
  if (constants.contains(dbName)) {
    dbPath = constants.at(dbName);
  } else {
    throw std::runtime_error("Undefined identifier: " + dbName);
  }
  if (!dbs.contains(dbPath)) {
    dbs.emplace(dbPath, std::make_unique<SQLiteDB>(dbPath));
  }
  auto res = dbs.at(dbPath)->query(uid, {lookup.getTable(), lookup.getValue()});
  constants.emplace(lookup.getValue(), res);
}

/**
 * @brief 访问Exit节点
 *
 * 该方法用于访问Exit节点，并抛出ExitException异常以退出程序。
 *
 * @throw ExitException 总是抛出此异常以退出程序。
 */
void Interpreter::visit(const Exit& /*unused*/) const { throw ExitException("exit"); }

/**
 * @brief 访问Jump节点
 *
 * 该方法用于访问Jump节点，并跳转到指定的过程。
 *
 * @param jump 要访问的Jump节点
 *
 * @throw std::runtime_error 如果指定的过程不存在。
 */
void Interpreter::visit(const Jump& jump) const {
  const auto& id = jump.getId();
  if (!procedures.contains(id)) {
    throw std::runtime_error("Undefined identifier: " + id);
  }
  program->getProcedures().at(procedures.at(id))->accept(*this);
}

/**
 * @brief 访问Identifier节点
 *
 * 该方法用于访问Identifier节点，并将其值添加到输出缓冲区。
 *
 * @param id 要访问的Identifier节点
 *
 * @throw std::runtime_error 如果指定的标识符未定义。
 */
void Interpreter::visit(const Identifier& id) const {
  if (constants.contains(id.getId())) {
    output_buffer += constants.at(id.getId());
  } else {
    throw std::runtime_error("Undefined identifier: " + id.getId());
  }
}

/**
 * @brief 访问String节点
 *
 * 该方法用于访问String节点，并将其值添加到输出缓冲区。
 *
 * @param s 要访问的String节点
 */
void Interpreter::visit(const String& s) const { output_buffer += s.getString(); }

/**
 * @brief 输出缓冲区内容
 *
 * 针对不同的模式，调用不同的方法输出缓冲区内容。
 * - 如果是CLI模式，则直接输出到标准输出。
 * - 如果是浏览器模式，则调用send回调函数发送到浏览器。
 */
void Interpreter::output() const {
  if (mode == "cli") {
    std::cout << output_buffer << std::endl;
  } else {
    send(output_buffer);
    spdlog::info("Send: {}", output_buffer);
  }
  output_buffer.clear();
}

/**
 * @brief 获取CLI输入
 *
 * 在新的线程中尝试获取CLI输入，并将其存储到输入缓冲区。
 * 得到输入后，设置输入接收标志为true，并通知等待的cv。
 * 如果有之前的输入正在读取，则不再获取输入。
 */
void Interpreter::getCliInput() const {
  if (!is_reading) {
    std::thread([this] {
      input_buffer.clear();
      is_reading = true;
      input_received = false;
      std::string tmp;
      std::getline(std::cin, tmp);
      std::lock_guard<std::mutex> lock(mtx);
      input_buffer = tmp;
      is_reading = false;
      input_received = true;
      cv.notify_one();
    }).detach();
  }
}

/**
 * @brief 设置网络输入
 *
 * 设置网络输入，并通知等待的cv。此方法供外部调用。
 *
 * @param input 要设置的输入
 */
void Interpreter::setInputFromNetwork(const std::string& input) const {
  std::lock_guard<std::mutex> lock(mtx);
  input_received = true;
  input_buffer = input;
  cv.notify_one();
}

/**
 * @brief 获取标识符或字符串
 *
 * 根据节点的类型返回标识符或字符串。
 *
 * @param p 要获取的节点
 *
 * @return 标识符或字符串
 */
auto Interpreter::getIdOrString(const ASTNodePtr& p) -> std::string {
  if (const auto* const id = dynamic_cast<Identifier*>(p.get())) {
    return id->getId();
  }
  const auto* const s = dynamic_cast<String*>(p.get());
  return s->getString();
}
