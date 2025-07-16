/**
 * @file  parser.cpp
 * @brief Parser 类的实现。
 */

#include "parser.hpp"

#include <bits/ranges_algo.h>

#include <memory>
#include <string>
#include <vector>

#include "ast.hpp"
#include "exception.hpp"
#include "token.hpp"

/**
 * @brief 解析程序的主入口，生成程序的抽象语法树（AST）。
 *
 * 此函数负责解析程序的整体结构，包括程序头部、常量声明、变量声明、
 * 子程序声明以及复合语句。解析过程中会验证语法规则并抛出相应的错误信息。
 *
 * @return ProgramPtr 指向解析生成的程序节点的智能指针。
 *
 * 解析步骤：
 * 1. 解析程序头部，包括关键字 `program` 和程序标识符。
 * 2. 如果存在参数列表，解析参数列表。
 * 3. 解析常量声明部分（如果存在）。
 * 4. 解析变量声明部分（如果存在）。
 * 5. 解析子程序声明部分（包括过程和函数）。
 * 6. 解析程序的复合语句（主程序体）。
 * 7. 验证程序以 `.` 结束。
 */
auto Parser::program() -> ProgramPtr {
  // 解析程序头部
  consume(TokenType::PROGRAM, "Expected 'program' keyword at the beginning of the program declaration.");
  auto program = std::make_unique<Program>(
      consume(TokenType::IDENTIFIER, "Expected program identifier after 'program' keyword.").value);
  if (match(TokenType::LPAREN)) {
    program->addParameter(consume(TokenType::IDENTIFIER, "Expected identifier for program parameter.").value);
    while (match(TokenType::COMMA)) {
      program->addParameter(consume(TokenType::IDENTIFIER, "Expected identifier for program parameter.").value);
    }
    consume(TokenType::RPAREN, "Expected ')' to close program parameter list.");
  }
  consume(TokenType::SEMICOLON, "Expected ';' after program header.");
  // 解析常量声明
  if (match(TokenType::CONST)) {
    while (check(TokenType::IDENTIFIER)) {
      program->addConstDeclaration(constDeclaration());
      consume(TokenType::SEMICOLON, "Expected ';' after constant declaration.");
    }
  }
  // 解析变量声明
  if (match(TokenType::VAR)) {
    while (check(TokenType::IDENTIFIER)) {
      program->addVarDeclaration(varDeclaration());
      consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    }
  }
  // 解析子程序声明
  while (check(TokenType::PROCEDURE) || check(TokenType::FUNCTION)) {
    program->addSubprogram(subprogram());
    consume(TokenType::SEMICOLON, "Expected ';' after subprogram declaration.");
  }
  // 解析复合语句
  program->setCompoundStatement(compoundStatement());

  consume(TokenType::DOT, "Expected '.' at the end of the program.");

  return program;
}

/**
 * @brief 解析子程序（过程或函数）。
 *
 * 根据当前标记解析子程序定义，包括标识符、参数列表、返回类型（函数）、常量声明、
 * 变量声明及复合语句（子程序体）。支持 `procedure` 和 `function`。
 *
 * @return SubprogramPtr 指向解析后的子程序对象。
 * @throws ParserException 如果语法错误。
 */
auto Parser::subprogram() -> SubprogramPtr {
  if (match(TokenType::PROCEDURE)) {
    auto id = consume(TokenType::IDENTIFIER, "Expected procedure identifier after 'procedure' keyword.").value;

    auto subprogram = std::make_unique<Subprogram>(id, false);
    if (match(TokenType::LPAREN)) {
      if (!match(TokenType::RPAREN)) {
        subprogram->addParameter(parameter());
        while (match(TokenType::SEMICOLON)) {
          subprogram->addParameter(parameter());
        }
        consume(TokenType::RPAREN, "Expected ')' to close procedure parameter list.");
      }
    }
    consume(TokenType::SEMICOLON, "Expected ';' after procedure header.");
    // subprogram body
    if (match(TokenType::CONST)) {
      while (check(TokenType::IDENTIFIER)) {
        subprogram->addConstDeclaration(constDeclaration());
        consume(TokenType::SEMICOLON, "Expected ';' after constant declaration in subprogram.");
      }
    }
    if (match(TokenType::VAR)) {
      while (check(TokenType::IDENTIFIER)) {
        subprogram->addVarDeclaration(varDeclaration());
        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration in subprogram.");
      }
    }
    subprogram->setCompoundStatement(compoundStatement());
    return subprogram;
  } else if (match(TokenType::FUNCTION)) {
    auto id = consume(TokenType::IDENTIFIER, "Expected function identifier after 'function' keyword.").value;
    auto subprogram = std::make_unique<Subprogram>(id, true);
    if (match(TokenType::LPAREN)) {
      if (!match(TokenType::RPAREN)) {
        subprogram->addParameter(parameter());
        while (match(TokenType::SEMICOLON)) {
          subprogram->addParameter(parameter());
        }
        consume(TokenType::RPAREN, "Expected ')' to close function parameter list.");
      }
    }
    consume(TokenType::COLON, "Expected ':' after function parameter list for return type specification.");
    if (match(TokenType::INTEGER)) {
      subprogram->setReturnType(BasicType::INTEGER);
    } else if (match(TokenType::REAL)) {
      subprogram->setReturnType(BasicType::REAL);
    } else if (match(TokenType::BOOLEAN)) {
      subprogram->setReturnType(BasicType::BOOLEAN);
    } else if (match(TokenType::CHAR)) {
      subprogram->setReturnType(BasicType::CHAR);
    } else {
      throw ParserException("Expected return type", getToken().line);
    }

    consume(TokenType::SEMICOLON, "Expected ';' after function header.");
    // subprogram body
    if (match(TokenType::CONST)) {
      while (check(TokenType::IDENTIFIER)) {
        subprogram->addConstDeclaration(constDeclaration());
        consume(TokenType::SEMICOLON, "Expected ';' after constant declaration in subprogram.");
      }
    }
    if (match(TokenType::VAR)) {
      while (check(TokenType::IDENTIFIER)) {
        subprogram->addVarDeclaration(varDeclaration());
        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration in subprogram.");
      }
    }
    subprogram->setCompoundStatement(compoundStatement());
    return subprogram;
  } else {
    throw ParserException("Expected 'procedure' or 'function'", getToken().line);
  }
}

/**
 * @brief 解析参数声明并生成参数对象。
 *
 * 此函数解析参数声明的语法结构，包括可选的 `VAR` 修饰符、参数标识符列表、
 * 参数类型声明以及基本类型的设置。如果语法不符合预期，将抛出解析异常。
 *
 * @return ParameterPtr 指向解析生成的参数对象的智能指针。
 *
 * @throws ParserException 如果语法不符合预期，例如缺少标识符、冒号或基本类型。
 *
 * 功能描述：
 * 1. 检查参数是否以 `VAR` 修饰符开头，并记录其存在性。
 * 2. 解析参数标识符列表，标识符之间用逗号分隔。
 * 3. 确保参数声明后跟随冒号 `:`。
 * 4. 根据后续的标记设置参数的基本类型（INTEGER、REAL、BOOLEAN 或 CHAR）。
 * 5. 如果未找到有效的基本类型，抛出异常。
 */
auto Parser::parameter() -> ParameterPtr {
  bool has_var = false;
  if (match(TokenType::VAR)) {
    has_var = true;
  }
  auto parameter = std::make_unique<Parameter>(has_var);
  parameter->addId(consume(TokenType::IDENTIFIER, "Expected identifier for parameter name.").value);
  while (match(TokenType::COMMA)) {
    parameter->addId(consume(TokenType::IDENTIFIER, "Expected identifier for parameter name.").value);
  }
  consume(TokenType::COLON, "Expected ':' after parameter identifier list to specify type.");
  if (match(TokenType::INTEGER)) {
    parameter->setBasicType(BasicType::INTEGER);
  } else if (match(TokenType::REAL)) {
    parameter->setBasicType(BasicType::REAL);
  } else if (match(TokenType::BOOLEAN)) {
    parameter->setBasicType(BasicType::BOOLEAN);
  } else if (match(TokenType::CHAR)) {
    parameter->setBasicType(BasicType::CHAR);
  } else {
    throw ParserException("Expected basic type", getToken().line);
  }
  return parameter;
}

/**
 * @brief 解析常量声明。
 *
 * 此函数用于解析常量声明语句，并返回一个指向 ConstDecl 的智能指针。
 * 它会根据当前的 Token 类型解析常量的标识符和值。
 *
 * @details
 * - 首先，函数会尝试消费一个标识符（IDENTIFIER）作为常量的名称。
 * - 接着，函数会消费一个等号（=）以确保语法正确。
 * - 然后，根据后续的 Token 类型解析常量的值：
 *   - 如果是加号/负号（+/-），则解析后续的数字。
 *   - 如果是数字（NUMBER），直接解析为常量值。
 *   - 否则期待一个字符。
 * @return ConstDeclPtr 指向解析后的常量声明的智能指针。
 */
auto Parser::constDeclaration() -> ConstDeclPtr {
  auto id = consume(TokenType::IDENTIFIER, "Expected identifier for constant name.").value;
  consume(TokenType::EQUAL, "Expected '=' after constant identifier in constant declaration.");
  if (match(TokenType::PLUS)) {
    return std::make_unique<ConstDecl>(
        id, "+" + consume(TokenType::NUMBER, "Expected numeric value after '+' in constant declaration.").value);
  }
  if (match(TokenType::MINUS)) {
    return std::make_unique<ConstDecl>(
        id, "-" + consume(TokenType::NUMBER, "Expected numeric value after '-' in constant declaration.").value);
  }
  if (check(TokenType::NUMBER)) {
    return std::make_unique<ConstDecl>(
        id, consume(TokenType::NUMBER, "Expected numeric value for constant.").value);
  }
  if (check(TokenType::CHAR_LITERAL)) {
    return std::make_unique<ConstDecl>(
        id, consume(TokenType::CHAR_LITERAL, "Expected character literal for constant.").value);
  }
  return std::make_unique<ConstDecl>(
      id, std::string("\"") +
              consume(TokenType::STRING_LITERAL, "Expected string literal for constant.").value + "\"");
}

/**
 * @brief 解析变量声明语句。
 *
 * 此函数处理变量声明的语法规则，包括变量标识符的解析、
 * 类型的解析以及语法检查。
 *
 * @return VarDeclPtr 返回一个表示变量声明的智能指针。
 *
 * @note 变量声明的语法规则如下：
 *       - 标识符后可以跟多个逗号分隔的标识符。
 *       - 标识符列表后必须有一个冒号 ':'。
 *       - 冒号后必须指定变量的类型。
 *
 * @exception 如果标识符或冒号缺失，抛出语法错误异常。
 */
auto Parser::varDeclaration() -> VarDeclPtr {
  auto var_decl = std::make_unique<VarDecl>();
  var_decl->addId(consume(TokenType::IDENTIFIER, "Expected identifier for variable name.").value);
  while (match(TokenType::COMMA)) {
    var_decl->addId(consume(TokenType::IDENTIFIER, "Expected identifier for variable name.").value);
  }
  consume(TokenType::COLON, "Expected ':' after variable identifier list.");
  var_decl->setType(type());
  return var_decl;
}

/**
 * @brief 解析类型并返回一个指向类型对象的智能指针。
 *
 * 此函数根据当前的标记流解析类型定义。支持的类型包括数组类型和基本类型（整数、实数、布尔值、字符）。
 *
 * - 如果是数组类型（ARRAY），解析数组的下界和上界，并支持多维数组的定义。
 * - 如果是基本类型（INTEGER、REAL、BOOLEAN、CHAR），设置对应的基本类型。
 * - 如果无法匹配任何支持的类型，抛出解析异常。
 *
 * @return TypePtr 指向解析后的类型对象的智能指针。
 *
 * @throws ParserException 如果解析失败或遇到意外的标记。
 */
auto Parser::type() -> TypePtr {
  auto type = std::make_unique<Type>();
  auto isIntegralString = [](const std::string& s) -> bool {
    return std::ranges::all_of(s.begin(), s.end(), [](const char c) -> bool { return std::isdigit(c); });
  };
  if (match(TokenType::ARRAY)) {
    consume(TokenType::LBRACKET, "Expected '[' after 'array' keyword.");
    auto lower = consume(TokenType::NUMBER, "Expected numeric value for array lower bound.").value;
    if (!isIntegralString(lower)) {
      throw ParserException("Expected a integral value for array lower bound", getToken().line);
    }
    consume(TokenType::DOTDOT, "Expected '..' between array bounds.");
    auto upper = consume(TokenType::NUMBER, "Expected numeric value for array upper bound.").value;
    if (!isIntegralString(upper)) {
      throw ParserException("Expected a integral value for array upper bound", getToken().line);
    }
    type->addPeriod(lower, upper);
    while (match(TokenType::COMMA)) {
      lower = consume(TokenType::NUMBER, "Expected numeric value for array lower bound.").value;
      if (!isIntegralString(lower)) {
        throw ParserException("Expected a integral value for array lower bound", getToken().line);
      }
      consume(TokenType::DOTDOT, "Expected '..' between array bounds.");
      upper = consume(TokenType::NUMBER, "Expected numeric value for array upper bound.").value;
      if (!isIntegralString(upper)) {
        throw ParserException("Expected a integral value for array upper bound", getToken().line);
      }
      type->addPeriod(lower, upper);
    }
    consume(TokenType::RBRACKET, "Expected ']' to close array bounds.");
    consume(TokenType::OF, "Expected 'of' keyword before array element type.");
    if (match(TokenType::INTEGER)) {
      type->setBasicType(BasicType::INTEGER);
    } else if (match(TokenType::REAL)) {
      type->setBasicType(BasicType::REAL);
    } else if (match(TokenType::BOOLEAN)) {
      type->setBasicType(BasicType::BOOLEAN);
    } else if (match(TokenType::CHAR)) {
      type->setBasicType(BasicType::CHAR);
    } else {
      throw ParserException("Expected basic type after 'of' in array declaration", getToken().line);
    }
  } else if (match(TokenType::INTEGER)) {
    type->setBasicType(BasicType::INTEGER);
  } else if (match(TokenType::REAL)) {
    type->setBasicType(BasicType::REAL);
  } else if (match(TokenType::BOOLEAN)) {
    type->setBasicType(BasicType::BOOLEAN);
  } else if (match(TokenType::CHAR)) {
    type->setBasicType(BasicType::CHAR);
  } else {
    throw ParserException("Expected basic type", getToken().line);
  }
  return type;
}

/**
 * @brief 解析变量的函数。
 *
 * 此函数用于解析变量标识符，并根据需要解析其后可能存在的表达式列表。
 *
 * @return VariablePtr 返回一个指向解析后的变量对象的智能指针。
 *
 * @details
 * - 首先，函数会尝试消费一个标识符（IDENTIFIER）类型的Token，并将其值作为变量的名称。
 * - 如果标识符后跟有左方括号（'['），则函数会解析括号内的表达式列表：
 *   - 添加第一个表达式到变量对象中。
 *   - 如果存在逗号（','），则继续解析后续的表达式并添加到变量对象中。
 * - 最后，函数会确保匹配右方括号（']'），否则抛出错误。
 *
 * @exception 如果未找到标识符或右方括号，函数会抛出相应的错误信息。
 */
auto Parser::variable() -> VariablePtr {
  auto id = consume(TokenType::IDENTIFIER, "Expected variable identifier.").value;
  auto variable = std::make_unique<Variable>(id);
  if (match(TokenType::LBRACKET)) {
    variable->addExpression(expression());
    while (match(TokenType::COMMA)) {
      variable->addExpression(expression());
    }
    consume(TokenType::RBRACKET, "Expected ']' to close array index.");
  }
  return variable;
}

/**
 * @brief 解析并返回一个语句节点。
 *
 * 此函数根据当前的标记类型解析不同类型的语句，包括复合语句、条件语句、循环语句、读写语句、
 * 赋值语句以及过程调用语句。如果当前标记类型不匹配任何已知的语句类型，则抛出解析异常。
 * 对于空语句的处理下放到各个使用了Statement的地方。
 *
 * @return StatementPtr 指向解析后的语句节点的智能指针。
 * @throws ParserException 如果无法匹配任何语句类型，则抛出异常。
 */
auto Parser::statement() -> StatementPtr {
  if (check(TokenType::BEGIN)) {
    return compoundStatement();
  } else if (check(TokenType::IF)) {
    return ifStatement();
  } else if (check(TokenType::FOR)) {
    return forStatement();
  } else if (check(TokenType::WHILE)) {
    return whileStatement();
  } else if (check(TokenType::READ)) {
    return readStatement();
  } else if (check(TokenType::WRITE)) {
    return writeStatement();
  } else if (check(TokenType::IDENTIFIER)) {
    forward();
    if (check(TokenType::ASSIGN) || check(TokenType::LBRACKET)) {  // 可能是赋值语句或函数返回
      backward();
      return assign();
    } else if (check(TokenType::LPAREN)) {
      backward();
      return procedureCall();
    } else {  // 认为是无参数的过程调用
      backward();
      return procedureCall();
    }
  } else if (check(TokenType::BREAK)) {
    return breakStatement();
  }
  throw ParserException("Expected statement", getToken().line);
}

/**
 * @brief 解析赋值语句。
 *
 * 此函数解析一个赋值语句，首先解析变量部分，然后匹配赋值操作符 ":="，
 * 最后解析表达式部分，并返回一个表示赋值语句的智能指针对象。
 *
 * @return AssignPtr 表示赋值语句的智能指针。
 */
auto Parser::assign() -> AssignPtr {
  auto variable = this->variable();
  consume(TokenType::ASSIGN, "Expected ':=' for assignment.");
  auto expression = this->expression();
  return std::make_unique<Assign>(std::move(variable), std::move(expression));
}

/**
 * @brief 解析过程调用语句。
 *
 * 此函数解析一个过程调用语句，并返回一个表示该调用的智能指针对象。
 *
 * @return ProcedureCallPtr 表示过程调用的智能指针。
 *
 * @details
 * - 首先消耗一个标识符（IDENTIFIER）令牌，并将其值作为过程调用的名称。
 * - 如果遇到左括号（LPAREN），则开始解析参数列表：
 *   - 调用 `expression()` 方法解析第一个参数，并将其添加到过程调用的参数列表中。
 *   - 如果遇到逗号（COMMA），继续解析后续参数，并添加到参数列表中。
 *   - 最后，消耗右括号（RPAREN），确保参数列表正确结束。
 * - 如果没有左括号，则表示该过程调用没有参数。
 * - 无参数的过程调用也可以有括号。
 */
auto Parser::procedureCall() -> ProcedureCallPtr {
  auto id = consume(TokenType::IDENTIFIER, "Expected procedure or function identifier for call.").value;
  auto procedure_call = std::make_unique<ProcedureCall>(id);
  if (match(TokenType::LPAREN)) {
    if (match(TokenType::RPAREN)) {
      return procedure_call;
    }
    procedure_call->addParameter(expression());
    while (match(TokenType::COMMA)) {
      procedure_call->addParameter(expression());
    }
    consume(TokenType::RPAREN, "Expected ')' to close argument list in procedure/function call.");
  }
  return procedure_call;
}

/**
 * @brief 解析复合语句（Compound Statement）。
 *
 * 此函数处理以 `begin` 开始、以 `end` 结束的复合语句块。
 * 它会解析复合语句中的各个子语句，并将它们添加到 `CompoundStatement` 对象中。
 * 如果复合语句为空或包含多余的分号，会插入空语句（NullStatement）。
 *
 * @return CompoundStatementPtr 指向解析后的复合语句对象的智能指针。
 *
 * @note
 * - 如果在 `begin` 后直接遇到 `end`，会插入一个空语句。
 * - 每个语句之间必须用分号（`;`）分隔。
 * - 如果遇到连续的分号或分号后直接是 `end`，会插入空语句。
 *
 * @exception 如果语法不符合预期（例如缺少 `begin` 或 `end`，或缺少分号），
 *            会抛出异常并提供相应的错误信息。
 */
auto Parser::compoundStatement() -> CompoundStatementPtr {
  consume(TokenType::BEGIN, "Expected 'begin' keyword to start compound statement.");
  auto compound_statement = std::make_unique<CompoundStatement>();
  if (match(TokenType::END)) {
    compound_statement->addStatement(std::make_unique<NullStatement>());
  } else {
    if (check(TokenType::SEMICOLON)) {
      compound_statement->addStatement(std::make_unique<NullStatement>());
    } else {
      compound_statement->addStatement(statement());
    }
    while (!match(TokenType::END)) {
      consume(TokenType::SEMICOLON, "Expected ';' to separate statements in compound statement.");
      if (check(TokenType::SEMICOLON) || check(TokenType::END)) {
        compound_statement->addStatement(std::make_unique<NullStatement>());
      } else {
        compound_statement->addStatement(statement());
      }
    }
  }
  return compound_statement;
}

/**
 * @brief 解析并生成一个 if 语句的抽象语法树节点。
 *
 * 此函数处理 if 语句的解析，包括条件表达式、then 分支和可选的 else 分支。
 * 如果 then 或 else 分支中没有有效的语句，则会生成一个 NullStatement。
 *
 * @return IfPtr 指向生成的 If 语句节点的智能指针。
 *
 * @note 支持的语句类型包括：标识符语句、BEGIN 块、嵌套的 IF 语句、FOR 循环、WHILE 循环、
 *       READ 语句和 WRITE 语句。如果未匹配到这些类型，则生成 NullStatement。
 */
auto Parser::ifStatement() -> IfPtr {
  consume(TokenType::IF, "Expected 'if' keyword to start if statement.");
  auto condition = expression();
  consume(TokenType::THEN, "Expected 'then' keyword after if condition.");
  StatementPtr then_statement;
  if (check(TokenType::IDENTIFIER) || check(TokenType::BEGIN) || check(TokenType::IF) || check(TokenType::FOR) ||
      check(TokenType::WHILE) || check(TokenType::READ) || check(TokenType::WRITE) || check(TokenType::BREAK)) {
    then_statement = statement();
  } else {
    then_statement = std::make_unique<NullStatement>();
  }
  if (match(TokenType::ELSE)) {
    StatementPtr else_statement;
    if (check(TokenType::IDENTIFIER) || check(TokenType::BEGIN) || check(TokenType::IF) || check(TokenType::FOR) ||
        check(TokenType::WHILE) || check(TokenType::READ) || check(TokenType::WRITE) || check(TokenType::BREAK)) {
      else_statement = statement();
    } else {
      else_statement = std::make_unique<NullStatement>();
    }
    return std::make_unique<If>(std::move(condition), std::move(then_statement), std::move(else_statement));
  }
  return std::make_unique<If>(std::move(condition), std::move(then_statement));
}

/**
 * @brief 解析一个 "for" 循环语句。
 *
 * 此函数负责解析 "for" 循环语句的语法结构，包括循环变量、下界、上界以及循环体。
 *
 * @details
 * 解析过程如下：
 * 1. 消耗 "for" 关键字。
 * 2. 解析循环变量（标识符）。
 * 3. 消耗赋值符号 ":="。
 * 4. 解析循环的下界表达式。
 * 5. 消耗 "to" 关键字。
 * 6. 解析循环的上界表达式。
 * 7. 消耗 "do" 关键字。
 * 8. 解析循环体语句。如果循环体为空，则生成一个空语句（NullStatement）。
 *
 * @return ForPtr 指向解析后的 "for" 循环语句的智能指针。
 */
auto Parser::forStatement() -> ForPtr {
  consume(TokenType::FOR, "Expected 'for' keyword to start for loop.");
  auto for_statement = std::make_unique<For>(
      consume(TokenType::IDENTIFIER, "Expected loop control variable identifier after 'for' keyword.").value);
  consume(TokenType::ASSIGN, "Expected ':=' for loop control variable initialization.");
  auto lower_bound = expression();
  consume(TokenType::TO, "Expected 'to' keyword for for loop range.");
  auto upper_bound = expression();
  consume(TokenType::DO, "Expected 'do' keyword before for loop body.");
  StatementPtr statement;
  if (check(TokenType::IDENTIFIER) || check(TokenType::BEGIN) || check(TokenType::IF) || check(TokenType::FOR) ||
      check(TokenType::WHILE) || check(TokenType::READ) || check(TokenType::WRITE) || check(TokenType::BREAK)) {
    statement = this->statement();
  } else {
    statement = std::make_unique<NullStatement>();
  }
  for_statement->setLowerBound(std::move(lower_bound));
  for_statement->setUpperBound(std::move(upper_bound));
  for_statement->setStatement(std::move(statement));
  return for_statement;
}

/**
 * @brief 解析并生成一个 "while" 语句的抽象语法树节点。
 *
 * 此函数处理 "while" 语句的解析逻辑，包括以下步骤：
 * 1. 消耗 "while" 关键字并验证其存在。
 * 2. 解析 "while" 语句的条件表达式。
 * 3. 消耗 "do" 关键字并验证其存在。
 * 4. 根据当前的标记类型，解析 "while" 语句的主体。如果没有有效的语句，
 *    则生成一个空语句（NullStatement）。
 *
 * @return WhilePtr 指向生成的 "while" 语句节点的智能指针。
 */
auto Parser::whileStatement() -> WhilePtr {
  consume(TokenType::WHILE, "Expected 'while' keyword to start while loop.");
  auto condition = expression();
  consume(TokenType::DO, "Expected 'do' keyword before while loop body.");
  StatementPtr statement;
  if (check(TokenType::IDENTIFIER) || check(TokenType::BEGIN) || check(TokenType::IF) || check(TokenType::FOR) ||
      check(TokenType::WHILE) || check(TokenType::READ) || check(TokenType::WRITE) || check(TokenType::BREAK)) {
    statement = this->statement();
  } else {
    statement = std::make_unique<NullStatement>();
  }
  return std::make_unique<While>(std::move(condition), std::move(statement));
}

/**
 * @brief 解析并生成一个读取语句的抽象语法树节点。
 *
 * 此函数负责解析 "read" 语句，包括读取变量列表并构建对应的抽象语法树节点。
 *
 * @return ReadPtr 指向生成的读取语句节点的智能指针。
 */
auto Parser::readStatement() -> ReadPtr {
  consume(TokenType::READ, "Expected 'read' keyword to start read statement.");
  auto read_statement = std::make_unique<Read>();
  consume(TokenType::LPAREN, "Expected '(' after 'read' keyword.");
  read_statement->addVariable(variable());
  while (match(TokenType::COMMA)) {
    read_statement->addVariable(variable());
  }
  consume(TokenType::RPAREN, "Expected ')' to close variable list in read statement.");
  return read_statement;
}

/**
 * @brief 解析并生成一个写语句（Write Statement）。
 *
 * 此函数负责解析写语句的语法结构，并生成对应的抽象语法树节点。
 * 写语句的格式为：`write(expression1, expression2, ...)`。
 *
 * @details
 * 1. 消耗一个 `write` 关键字。
 * 2. 消耗左括号 `(`。
 * 3. 解析一个或多个表达式，并将其添加到写语句节点中。
 * 4. 如果有多个表达式，则用逗号 `,` 分隔。
 * 5. 消耗右括号 `)`。
 *
 * @return WritePtr 指向生成的写语句节点的智能指针。
 */
auto Parser::writeStatement() -> WritePtr {
  consume(TokenType::WRITE, "Expected 'write' keyword to start write statement.");
  auto write_statement = std::make_unique<Write>();
  consume(TokenType::LPAREN, "Expected '(' after 'write' keyword.");
  write_statement->addExpression(expression());
  while (match(TokenType::COMMA)) {
    write_statement->addExpression(expression());
  }
  consume(TokenType::RPAREN, "Expected ')' to close expression list in write statement.");
  return write_statement;
}

/**
 * @brief 解析并生成一个Break语句。
 *
 * @return BreakPtr 指向生成的break语句节点的智能指针。
 */
auto Parser::breakStatement() -> BreakPtr {
  consume(TokenType::BREAK, "Expected 'break' keyword.");
  return std::make_unique<Break>();
}

/**
 * @brief 解析并返回一个因子（Factor）节点。
 *
 * 因子是表达式中的基本组成部分，可以是标识符、数字、括号内的表达式、
 * 带有逻辑非（NOT）或一元负号（UMINUS）的表达式等。
 *
 * @return FactorPtr 指向解析后的因子节点的智能指针。
 *
 * @throws ParserException 如果无法匹配任何有效的因子，将抛出解析异常。
 *
 * 因子解析规则：
 * - 如果当前标记是标识符（IDENTIFIER）：
 *   - 如果后续是左括号（LPAREN），解析为函数调用。
 *   - 如果后续是左方括号（LBRACKET），解析为变量。
 *   - 否则，解析为未知类型的标识符。
 * - 如果当前标记是数字（NUMBER），解析为数字因子。
 * - 如果匹配到左括号（LPAREN），解析括号内的表达式，并期望右括号（RPAREN）。
 * - 如果匹配到逻辑非（NOT），解析为带有逻辑非的因子。
 * - 如果匹配到一元负号（MINUS），解析为带有一元负号的因子。
 * - 如果匹配到布尔值（TRUE/FALSE），解析为布尔因子。
 */
auto Parser::factor() -> FactorPtr {
  if (check(TokenType::IDENTIFIER)) {
    forward();
    if (check(TokenType::LPAREN)) {
      auto factor = std::make_unique<Factor>(FactorType::FUNCTION);
      backward();
      factor->setValue<ASTNodePtr>(procedureCall());
      return factor;
    } else if (check(TokenType::LBRACKET)) {
      auto factor = std::make_unique<Factor>(FactorType::VARIABLE);
      backward();
      factor->setValue<ASTNodePtr>(variable());
      return factor;
    } else {
      auto factor = std::make_unique<Factor>(FactorType::UNKNOWN);
      backward();
      factor->setValue(consume(TokenType::IDENTIFIER, "Expected identifier as a factor.").value);
      return factor;
    }
  } else if (check(TokenType::NUMBER)) {
    auto factor = std::make_unique<Factor>(FactorType::NUMBER);
    factor->setValue(consume(TokenType::NUMBER, "Expected numeric literal as a factor.").value);
    return factor;
  } else if (match(TokenType::LPAREN)) {
    auto factor = std::make_unique<Factor>(FactorType::EXPRESSION);
    factor->setValue<ASTNodePtr>(expression());
    consume(TokenType::RPAREN, "Expected ')' to close parenthesized expression.");
    return factor;
  } else if (match(TokenType::NOT)) {
    auto factor = std::make_unique<Factor>(FactorType::WITH_NOT);
    factor->setValue<ASTNodePtr>(this->factor());
    return factor;
  } else if (match(TokenType::MINUS)) {
    auto factor = std::make_unique<Factor>(FactorType::WITH_UMINUS);
    factor->setValue<ASTNodePtr>(this->factor());
    return factor;
  } else if (match(TokenType::TRUE)) {
    auto factor = std::make_unique<Factor>(FactorType::BOOLEAN);
    factor->setValue("true");
    return factor;
  } else if (match(TokenType::FALSE)) {
    auto factor = std::make_unique<Factor>(FactorType::BOOLEAN);
    factor->setValue("false");
    return factor;
  } else if (match(TokenType::PLUS)) {
    auto factor = std::make_unique<Factor>(FactorType::WITH_PLUS);
    factor->setValue<ASTNodePtr>(this->factor());
    return factor;
  }
  throw ParserException("Expected factor", getToken().line);
}

/**
 * @brief 解析并生成一个项（Term）节点。
 *
 * 该函数解析当前输入流中的项表达式，并构建一个项（Term）对象。
 * 它首先解析并设置项的第一个因子（Factor），然后根据输入流中的运算符
 * （包括乘法、实数除法、整数除法、取模和逻辑与）循环解析后续的因子。
 * 每个因子都会与对应的运算符一起添加到项中。
 *
 * 支持的运算符包括：
 * - `*` (TIMES): 乘法
 * - `/` (RDIV): 实数除法
 * - `DIV`: 整数除法
 * - `MOD`: 取模
 * - `AND`: 逻辑与
 *
 * @return TermPtr 一个指向解析生成的项对象的智能指针。
 */
auto Parser::term() -> TermPtr {
  auto term = std::make_unique<Term>();
  term->setFirstFactor(factor());
  while (check(TokenType::TIMES) || check(TokenType::RDIV) || check(TokenType::DIV) || check(TokenType::MOD) ||
         check(TokenType::AND)) {
    if (match(TokenType::TIMES)) {
      term->addFactor(Term::MulOp::TIMES, factor());
    } else if (match(TokenType::RDIV)) {
      term->addFactor(Term::MulOp::RDIV, factor());
    } else if (match(TokenType::DIV)) {
      term->addFactor(Term::MulOp::DIV, factor());
    } else if (match(TokenType::MOD)) {
      term->addFactor(Term::MulOp::MOD, factor());
    } else if (match(TokenType::AND)) {
      term->addFactor(Term::MulOp::AND, factor());
    }
  }
  return term;
}

/**
 * @brief 解析一个简单表达式。
 *
 * 此方法用于解析简单表达式，并返回一个表示该表达式的指针对象。
 * 简单表达式由一个初始项（term）开始，后面可以跟随零个或多个由加法操作符（+）、减法操作符（-）或逻辑或操作符（OR）连接的项。
 *
 * @return SimpleExpressionPtr 表示解析后的简单表达式的智能指针。
 */
auto Parser::simpleExpression() -> SimpleExpressionPtr {
  auto simple_expression = std::make_unique<SimpleExpression>();
  simple_expression->setFirstTerm(term());
  while (check(TokenType::PLUS) || check(TokenType::MINUS) || check(TokenType::OR)) {
    if (match(TokenType::PLUS)) {
      simple_expression->addTerm(SimpleExpression::AddOp::PLUS, term());
    } else if (match(TokenType::MINUS)) {
      simple_expression->addTerm(SimpleExpression::AddOp::MINUS, term());
    } else if (match(TokenType::OR)) {
      simple_expression->addTerm(SimpleExpression::AddOp::OR, term());
    }
  }
  return simple_expression;
}

/**
 * @brief 解析表达式并生成表达式节点。
 *
 * 此函数解析一个表达式，首先解析简单表达式作为左操作数，
 * 然后根据当前的令牌类型判断是否存在关系运算符。如果存在，
 * 则解析右操作数并将其与关系运算符一起设置到表达式节点中。
 *
 * @return ExpressionPtr 表示解析后的表达式节点的智能指针。
 */
auto Parser::expression() -> ExpressionPtr {
  auto expression = std::make_unique<Expression>();
  expression->setLeft(simpleExpression());
  if (match(TokenType::GT)) {
    expression->setRight(Expression::RelOp::GT, simpleExpression());
  } else if (match(TokenType::EQUAL)) {
    expression->setRight(Expression::RelOp::EQ, simpleExpression());
  } else if (match(TokenType::LT)) {
    expression->setRight(Expression::RelOp::LT, simpleExpression());
  } else if (match(TokenType::GE)) {
    expression->setRight(Expression::RelOp::GE, simpleExpression());
  } else if (match(TokenType::LE)) {
    expression->setRight(Expression::RelOp::LE, simpleExpression());
  } else if (match(TokenType::NE)) {
    expression->setRight(Expression::RelOp::NE, simpleExpression());
  }
  return expression;
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
auto Parser::isEnd() const -> bool { return getToken().type == TokenType::END_OF_FILE; }

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
