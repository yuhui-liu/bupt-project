/**
 * @file  parser.cpp
 * @brief Parser 类的实现。
 */

#include "parser.hpp"

#include <memory>
#include <string>
#include <vector>

#include "ast.hpp"
#include "exception.hpp"
#include "token.hpp"

/**
 * 解析程序并返回一个包含所有常量和过程的Program对象。
 *
 * @return std::shared_ptr<Program> 包含所有常量和过程的Program对象。
 * @throws ParserException 如果在所有过程之后没有遇到EOF，则抛出异常。
 */
auto Parser::parse() -> std::shared_ptr<Program> {
  auto program = std::make_shared<Program>();
  while (check(TokenType::IDENTIFIER)) {
    program->addConstant(constant());
  }
  while (match(TokenType::PROCEDURE)) {
    program->addProcedure(procedure());
  }
  if (!isEnd()) {
    throw ParserException("Except an EOF after all procedures.", getToken().line);
  }
  return program;
}

/**
 * @brief 解析常量定义
 *
 * 该函数解析一个常量定义语句，期望的语法格式为：
 * <identifier> = <string>
 *
 * @return ASTNodePtr 返回一个指向常量定义节点的智能指针
 * @throws std::runtime_error 如果语法不符合预期，将抛出异常
 */
auto Parser::constant() -> ASTNodePtr {
  auto id = consume(TokenType::IDENTIFIER, "Except an id.").value;
  consume(TokenType::EQUAL, "Except an '='.");
  auto value = consume(TokenType::STRING, "Except a string.").value;
  return std::make_unique<ConstantDefine>(id, value);
}

/**
 * 解析一个过程。
 *
 * 该函数期望以下的标记序列：
 * - 一个表示过程名称的标识符。
 * - 一个左大括号 '{' 表示过程体的开始。
 * - 一系列构成过程体的语句。
 * - 一个右大括号 '}' 表示过程体的结束。
 *
 * @return 一个指向解析后的Procedure AST节点的唯一指针。
 * @throws std::runtime_error 如果未找到预期的标记，将抛出异常。
 */
auto Parser::procedure() -> ASTNodePtr {
  auto procedure = std::make_unique<Procedure>(consume(TokenType::IDENTIFIER, "Except an id.").value);
  consume(TokenType::LBRACE, "Except a '{'.");
  while (!check(TokenType::RBRACE)) {
    procedure->addStatement(statement());
  }
  consume(TokenType::RBRACE, "Except a '}'.");
  return procedure;
}

/**
 * @brief 解析并返回一个语句节点。
 *
 * 该函数根据当前的标记类型解析不同类型的语句节点。
 * - 如果标记类型为 SAY，则解析并返回一个 Say 语句节点。
 * - 如果标记类型为 LISTEN，则解析并返回一个 Listen 语句节点。
 * - 如果标记类型为 LOOKUP，则解析并返回一个 Lookup 语句节点。
 * - 如果标记类型为 EXIT，则返回一个 Exit 语句节点。
 * - 否则，假定它是一个跳转语句，并返回一个 Jump 语句节点。
 *
 * @return ASTNodePtr 指向解析后的语句节点的智能指针。
 */
auto Parser::statement() -> ASTNodePtr {
  if (match(TokenType::SAY)) {
    return say();
  }
  if (match(TokenType::LISTEN)) {
    return listen();
  }
  if (match(TokenType::LOOKUP)) {
    return lookup();
  }
  if (match(TokenType::EXIT)) {
    return std::make_unique<Exit>();
  }
  // Otherwise, suppose it's a jump
  return std::make_unique<Jump>(consume(TokenType::IDENTIFIER, "Except an id.").value);
}

/**
 * @brief 解析并生成一个表示“say”语句的抽象语法树节点。
 *
 * 该函数创建一个新的 Say 节点，并将解析的内容添加到该节点中。
 * 首先，它会解析第一个标识符或字符串并添加到 Say 节点中。
 * 然后，它会检查是否存在逗号分隔的其他内容，如果存在，则继续解析并添加到 Say 节点中。
 *
 * @return ASTNodePtr 指向生成的 Say 节点的智能指针。
 */
auto Parser::say() -> ASTNodePtr {
  auto say = std::make_unique<Say>();
  say->addContent(idOrString());
  while (match(TokenType::COMMA)) {
    say->addContent(idOrString());
  }
  return say;
}

/**
 * @brief 解析 "listen" 语句。
 *
 * 该函数解析一个 "listen" 语句，该语句包含一个时间限制和一个或多个子句。
 *
 * @return ASTNodePtr 返回解析后的 AST 节点。
 *
 * @throws std::runtime_error 如果解析过程中遇到语法错误，将抛出运行时错误。
 *
 * 解析步骤：
 * 1. 消费 "timelimit" 关键字。
 * 2. 消费一个数字，作为时间限制。
 * 3. 消费左大括号 '{'。
 * 4. 循环解析子句，直到遇到右大括号 '}'。
 * 5. 消费右大括号 '}'。
 */
auto Parser::listen() -> ASTNodePtr {
  consume(TokenType::TIMELIMIT, R"(Except "timelimit" after "listen".)");
  auto listen =
      std::make_unique<Listen>(std::stoi(consume(TokenType::NUMBER, "Except a number after \"timelimit\".").value));
  consume(TokenType::LBRACE, "Except a '{'.");
  while (!check(TokenType::RBRACE)) {
    listen->addClause(clause());
  }
  consume(TokenType::RBRACE, "Except a '}'.");
  return listen;
}

/**
 * @brief 解析一个子句。
 *
 * 该函数解析一个子句，并根据匹配的Token类型设置子句的类型和相应的属性。
 *
 * @return ASTNodePtr 返回解析后的子句节点。
 *
 * @throws ParserException 当子句不符合预期格式时抛出异常。
 */
auto Parser::clause() -> ASTNodePtr {
  auto clause = std::make_unique<Clause>();
  if (match(TokenType::ANYTHING)) {
    clause->setType(Clause::Type::ANYTHING);
  } else if (match(TokenType::HAS)) {
    clause->setType(Clause::Type::HAS);
  } else if (match(TokenType::TIMEOUT)) {
    clause->setType(Clause::Type::TIMEOUT);
  } else if (match(TokenType::DEFAULT)) {
    clause->setType(Clause::Type::DEFAULT);
  } else {
    throw ParserException("Except a clause inside \"listen\" statement.", getToken().line);
  }
  if (clause->getType() == Clause::Type::HAS) {
    clause->setPattern(idOrString());
  }
  consume(TokenType::QUESTION, "Except a '?'.");
  if (match(TokenType::WRITETO)) {
    clause->setAction(Clause::Action::WRITETO);
    const auto db = consume(TokenType::IDENTIFIER, "Except an id.").value;
    consume(TokenType::COLON, "Except an ':'.");
    const auto table = consume(TokenType::IDENTIFIER, "Except an id.").value;
    consume(TokenType::COLON, "Except a ':'.");
    const auto column = consume(TokenType::IDENTIFIER, "Except an id.").value;
    clause->setDb(db);
    clause->setTable(table);
    clause->setColumn(column);
  } else {
    clause->setAction(Clause::Action::JUMP);
    clause->setBranch(consume(TokenType::IDENTIFIER, "Except an id.").value);
  }
  return clause;
}

/**
 * @brief 解析并生成一个 Lookup 节点。
 *
 * 该函数解析一个 lookup 语句，并生成相应的 AST 节点。具体的语法格式如下：
 *
 * ```
 * lookup_statement ::= IDENTIFIER "in" IDENTIFIER ":" IDENTIFIER
 * ```
 *
 * @return ASTNodePtr 指向生成的 Lookup 节点的智能指针。
 *
 * @throws std::runtime_error 如果解析过程中遇到语法错误，将抛出运行时错误。
 */
auto Parser::lookup() -> ASTNodePtr {
  auto value = consume(TokenType::IDENTIFIER, "Except an id.").value;
  consume(TokenType::IN, R"(Except "in" in a "lookup" statement.)");
  auto db = consume(TokenType::IDENTIFIER, "Except an id.").value;
  consume(TokenType::COLON, "Except a ':'.");
  auto table = consume(TokenType::IDENTIFIER, "Except an id.").value;
  return std::make_unique<Lookup>(value, db, table);
}

/**
 * @brief 解析标识符或字符串。
 *
 * 如果当前标记是标识符，则解析并返回标识符节点。
 * 否则，解析并返回字符串节点。
 *
 * @return ASTNodePtr 指向解析后的抽象语法树节点的智能指针。
 */
auto Parser::idOrString() -> ASTNodePtr {
  if (check(TokenType::IDENTIFIER)) {
    return id();
  }
  return std::make_unique<String>(consume(TokenType::STRING, "Except a string.").value);
}

/**
 * @brief 解析标识符并返回一个AST节点指针。
 *
 * 该函数从输入流中消费一个标识符，并将其封装在一个Identifier对象中，
 * 然后返回一个指向该对象的唯一指针。
 *
 * @return ASTNodePtr 指向Identifier对象的唯一指针。
 */
auto Parser::id() -> ASTNodePtr {
  return std::make_unique<Identifier>(consume(TokenType::IDENTIFIER, "Except an id.").value);
}

/**
 * @brief 检查当前 token 是否与给定类型匹配，如果匹配则前进到下一个 token。
 *
 * @param type 要匹配的 token 类型。
 * @return 如果当前 token 与给定类型匹配则返回 true，否则返回 false。
 */
auto Parser::match(const TokenType& type) -> bool {
  if (check(type)) {
    forward();
    return true;
  }
  return false;
}

/**
 * @brief 检查当前令牌是否为指定类型。
 *
 * @param type 要检查的令牌类型。
 * @return 如果当前令牌是指定类型且未到达文件末尾，则返回 true；否则返回 false。
 */
auto Parser::check(const TokenType& type) const -> bool {
  if (isEnd()) {
    return false;
  }
  return getToken().type == type;
}

/**
 * @brief 检查当前是否已经到达文件末尾。
 *
 * @return 如果当前已经到达文件末尾，则返回 true；否则返回 false。
 */
auto Parser::isEnd() const -> bool { return getToken().type == TokenType::EOF_; }

/**
 * @brief 获取当前 token。
 *
 * @return const Token& 当前 token。
 */
auto Parser::getToken() const -> const Token& { return tokens[current]; }

/// @brief 前进到下一个 token。
void Parser::forward() { current++; }

/// @brief 后退到上一个 token。
void Parser::backward() { current--; }

/**
 * @brief 消费一个指定类型的 token。
 *
 * 如果当前 token 的类型与给定类型不匹配，则抛出异常。
 *
 * @param type 要消费的 token 类型。
 * @param msg 错误消息。
 * @return 消费的 token。
 */
auto Parser::consume(const TokenType& type, const std::string& msg) -> Token {
  if (check(type)) {
    return tokens[current++];
  }
  throw ParserException(msg, getToken().line);
}
