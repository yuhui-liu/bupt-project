/**
 * @file    exception.hpp
 * @brief   与异常相关的类的定义。
 * @details 包括词法分析、语法分析、语义分析、退出等异常类的定义。
 */
#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

/**
 * @class BaseException
 * @brief 异常基类
 * @details 定义了异常基类，LexerException、ParserException继承自此类
 */
class BaseException : public std::runtime_error {
 public:
  /// @param message 异常信息
  /// @param line 行号
  explicit BaseException(const std::string& message, const std::size_t line = 0)
      : std::runtime_error(message), line(line) {}

  auto getLine() const noexcept -> std::size_t { return line; }

  /// @return 异常信息
  auto what() const noexcept -> const char* override {
    detailed_message = std::runtime_error::what();
    return detailed_message.c_str();
  }

 private:
  std::size_t line;
  mutable std::string detailed_message;
};

/**
 * @class LexerException
 * @brief 词法分析异常
 * @details 定义了词法分析异常类，继承自BaseException
 */
class LexerException final : public BaseException {
 public:
  explicit LexerException(const std::string& message, const std::size_t line = 0) : BaseException(message, line) {}
};

/**
 * @class ParserException
 * @brief 语法分析异常
 * @details 定义了语法分析异常类，继承自BaseException
 */
class ParserException final : public BaseException {
 public:
  explicit ParserException(const std::string& message, const std::size_t line = 0) : BaseException(message, line) {}
};

/**
 * @class SemanticAnalysisException
 * @brief 语义分析异常
 * @details 定义了语义分析异常类，继承自std::runtime_error
 */
class SemanticAnalysisException final : public std::runtime_error {
 public:
  explicit SemanticAnalysisException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @class ExitException
 * @brief 退出异常
 * @details 定义了退出异常类，继承自std::runtime_error。并非真正的异常，而是用于程序退出的信号。
 */
class ExitException final : public std::runtime_error {
 public:
  explicit ExitException(const std::string& message) : std::runtime_error(message) {}
};

#endif  // EXCEPTION_HPP
