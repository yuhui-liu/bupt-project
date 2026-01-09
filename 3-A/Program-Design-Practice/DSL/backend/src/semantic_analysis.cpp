/**
 * @file   semantic_analysis.cpp
 * @brief  SemanticAnalysis 类的实现。
 */

#include "semantic_analysis.hpp"

#include <memory>

#include "ast.hpp"
#include "exception.hpp"

/**
 * @brief 语义分析类的构造函数。
 * @param program 指向 Program 对象的智能指针。
 * @throw SemanticAnalysisException 如果常量或过程重定义，或者没有 'main' 过程。
 *
 * 将常量存储在 std::map 中，键为常量名，值为常量值。
 * 将过程存储在 std::map 中，键为过程名，值为过程在程序中的索引。
 */
SemanticAnalysis::SemanticAnalysis(const std::shared_ptr<Program> &program) {
  // check redefined constant
  for (const auto &c : program->getConstants()) {
    auto id = dynamic_cast<ConstantDefine *>(c.get())->getId();
    const auto value = dynamic_cast<ConstantDefine *>(c.get())->getValue();
    if (constants.contains(id)) {
      throw SemanticAnalysisException("Redefine constant: " + id);
    }
    constants[id] = value;
  }
  // check redefined procedure
  // return the index of the procedure
  std::size_t idx = 0;
  for (auto &&p : program->getProcedures()) {
    auto id = dynamic_cast<Procedure *>(p.get())->getId();
    if (procedures.contains(id)) {
      throw SemanticAnalysisException("Redefine procedure: " + id);
    }
    procedures[id] = idx++;
  }
  if (!procedures.contains("main")) {
    throw SemanticAnalysisException("No 'main' procedure");
  }
}

auto SemanticAnalysis::getConstants() const -> std::map<std::string, std::string> { return constants; }

auto SemanticAnalysis::getProcedures() const -> std::map<std::string, std::size_t> { return procedures; }
