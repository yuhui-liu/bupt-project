#include <spdlog/spdlog.h>
#include <third-party/dotenv.h>

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "argument_parser.hpp"
#include "ast.hpp"
#include "exception.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"
#include "network.hpp"
#include "parser.hpp"
#include "semantic_analysis.hpp"
#include "websocketstreamfactory.hpp"

auto main(int argc, const char* argv[]) -> int {
  // Parse arguments.
  ArgumentParser argParser;
  try {
    argParser.parse(argc, argv);
  } catch (std::exception& e) {
    spdlog::error(e.what());
    std::cout << argParser.getDesc();
    exit(EXIT_FAILURE);
  }
  auto mode = argParser.getMode();
  // Load script.
  auto script = std::ifstream(argParser.getScript());
  if (script.fail()) {
    spdlog::error("Failed to load script: {}", argParser.getScript());
    exit(EXIT_FAILURE);
  }
  std::ostringstream scriptStr;
  scriptStr << script.rdbuf();

  // Parse the script.
  std::shared_ptr<Program> program;
  std::map<std::string, std::string> constants;
  std::map<std::string, std::size_t> procedures;
  try {
    Lexer lexer(scriptStr.str());
    lexer.scan();
    Parser parser(lexer.getTokens());
    program = parser.parse();
    SemanticAnalysis sa(program);
    constants = sa.getConstants();
    procedures = sa.getProcedures();
  } catch (const LexerException& e) {
    spdlog::error("Error while lexing, at line {}: {}", e.getLine(), e.what());
    exit(EXIT_FAILURE);
  } catch (const ParserException& e) {
    spdlog::error("Error while parsing, at line {}: {}", e.getLine(), e.what());
    exit(EXIT_FAILURE);
  } catch (const SemanticAnalysisException& e) {
    spdlog::error("Error while semantic analysising: {}", e.what());
    exit(EXIT_FAILURE);
  } catch (const std::exception& e) {
    spdlog::error("Unexpected error: {}", e.what());
    exit(EXIT_FAILURE);
  }
  spdlog::info("Successfully build AST.");

  if (argParser.getMode() == "browser") {
    spdlog::info("Running in browser mode.");
    try {
      // 初始化I/O上下文
      net::io_context ioc;

      // 从环境变量中获取端口号
      dotenv::init();
      auto port = std::stoi(dotenv::getenv("PORT", "8080"));

      // 创建WebSocket服务器
      tcp::endpoint endpoint(tcp::v4(), port);
      WebSocketServer server(ioc, endpoint, program, constants, procedures, std::make_shared<WebSocketStreamFactory>());

      spdlog::info("WebSocket Server listening on ws://localhost:{}", port);

      server.run();
      ioc.run();
    } catch (const std::exception& e) {
      spdlog::error(e.what());
      exit(EXIT_FAILURE);
    }
  } else {
    spdlog::info("Running in console mode.");
    try {
      Interpreter interpreter(program, mode, constants, procedures);
      interpreter.setUid(argParser.getUid());
      program->accept(interpreter);
    } catch (const ExitException& e) {
      spdlog::info("Program exited.");
    } catch (const std::exception& e) {
      spdlog::error(e.what());
      exit(EXIT_FAILURE);
    }
  }

  spdlog::info("BYE-BYE!");
  return EXIT_SUCCESS;
}
