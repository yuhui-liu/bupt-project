/**
 * @file      argument_parser.hpp
 * @brief     ArgumentParser 类的声明。
 */

#ifndef ARGUMENT_PARSER_HPP
#define ARGUMENT_PARSER_HPP

#include <boost/program_options.hpp>
#include <string>

namespace po = boost::program_options;
/**
 * @class   ArgumentParser
 * @brief   解析命令行参数
 * @details 解析命令行参数。包括脚本文件名，运行模式。
 */
class ArgumentParser {
 public:
  ArgumentParser();
  void parse(int argc, const char* argv[]);
  // getters
  [[nodiscard]] auto getScript() const -> const auto& { return script; }
  [[nodiscard]] auto getMode() const -> const auto& { return mode; }
  [[nodiscard]] auto getDesc() const -> const auto& { return desc; }
  [[nodiscard]] auto getUid() const -> const auto& { return uid; }

 private:
  /// @brief 命令行参数描述
  po::options_description desc{"Options"};
  /// @brief 命令行参数存储
  po::variables_map vm;
  /// @brief 运行模式
  std::string mode;
  /// @brief 脚本文件名
  std::string script;
  static constexpr int DEFAULTUID = 101;
  /// @brief 用户id
  int uid{DEFAULTUID};
};

#endif  // ARGUMENT_PARSER_HPP
