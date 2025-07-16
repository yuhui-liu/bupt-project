/**
 * @file    argument_parser.cpp
 * @brief   ArgumentParser 类的实现。
 */

#include "argument_parser.hpp"

#include <iostream>
#include <stdexcept>

/// @brief 构造函数，初始化命令行选项描述。
ArgumentParser::ArgumentParser() {
  // clang-format off
  desc.add_options()
    ("help,h", "Show this help.")  ///< 显示帮助信息
    ("mode,m", po::value<std::string>()->default_value("browser"), "Set mode (browser or cli).")  ///< 设置模式（浏览器或命令行）
    ("script,s", po::value<std::string>()->default_value("scripts/sample"), "Set script file.")  ///< 设置脚本文件
    ("userid,u", po::value<int>()->default_value(DEFAULTUID), "Set user id.");  ///< 设置用户id
  // clang-format on
}

/// @brief 解析命令行参数。
/// @param argc 参数个数。
/// @param argv 参数数组。
/// @throws std::invalid_argument 如果模式无效。
void ArgumentParser::parse(const int argc, const char* argv[]) {
  po::store(po::parse_command_line(argc, argv, desc), vm);
  if (vm.contains("help")) {
    std::cout << desc << "\n";
    exit(0);
  }
  mode = vm["mode"].as<std::string>();
  if (mode != "browser" && mode != "cli") {
    throw std::invalid_argument("Invalid mode: " + mode + "\n");
  }
  if (vm.contains("script")) {
    script = vm["script"].as<std::string>();
  }
  if (vm.contains("userid")) {
    uid = vm["userid"].as<int>();
  }
}
