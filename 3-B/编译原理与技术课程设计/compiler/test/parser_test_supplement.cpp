
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "ast.hpp"
#include "parser.hpp"
#include "token.hpp"

TEST(ParserTestSupplement, Factor) {
  // -123
  std::vector<Token> tokens;
  tokens.emplace_back(TokenType::MINUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "123", 1);
  Parser parser(tokens);
  auto result = parser.factor();

  auto fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("123");
  auto standard = std::make_unique<Factor>(FactorType::WITH_UMINUS);
  standard->setValue<ASTNodePtr>(std::move(fac1));
  EXPECT_TRUE(standard->checkSame(result.get()));

  // not 1
  tokens.clear();
  tokens.emplace_back(TokenType::NOT, 1);
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  parser = Parser(tokens);
  result = parser.factor();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  standard = std::make_unique<Factor>(FactorType::WITH_NOT);
  standard->setValue<ASTNodePtr>(std::move(fac1));
  EXPECT_TRUE(standard->checkSame(result.get()));

  // func(1)
  tokens.clear();
  tokens.emplace_back(TokenType::IDENTIFIER, "func", 1);
  tokens.emplace_back(TokenType::LPAREN, 1);
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::RPAREN, 1);
  parser = Parser(tokens);
  result = parser.factor();

  EXPECT_TRUE(result->getType() == FactorType::FUNCTION);

  // (1)
  tokens.clear();
  tokens.emplace_back(TokenType::LPAREN, 1);
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::RPAREN, 1);
  parser = Parser(tokens);
  result = parser.factor();

  EXPECT_TRUE(result->getType() == FactorType::EXPRESSION);

  // a[1]
  tokens.clear();
  tokens.emplace_back(TokenType::IDENTIFIER, "a", 1);
  tokens.emplace_back(TokenType::LBRACKET, 1);
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::RBRACKET, 1);
  parser = Parser(tokens);
  result = parser.factor();

  EXPECT_TRUE(result->getType() == FactorType::VARIABLE);

  // a
  tokens.clear();
  tokens.emplace_back(TokenType::IDENTIFIER, "a", 1);
  parser = Parser(tokens);
  result = parser.factor();

  EXPECT_TRUE(result->getType() == FactorType::UNKNOWN);
}

TEST(ParserTestSupplement, Term) {
  // -123 * 456
  std::vector<Token> tokens;
  tokens.emplace_back(TokenType::MINUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "123", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "456", 1);
  auto parser = Parser(tokens);
  auto result = parser.term();

  auto fac_1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac_1->setValue("123");
  auto fac1 = std::make_unique<Factor>(FactorType::WITH_UMINUS);
  fac1->setValue<ASTNodePtr>(std::move(fac_1));
  auto fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("456");
  auto standard = std::make_unique<Term>();
  standard->setFirstFactor(std::move(fac1));
  standard->addFactor(Term::MulOp::TIMES, std::move(fac2));

  // standard->accept(pt);
  // for (auto& a : pt.getResult()) {
  //   std::cout << a << std::endl;
  // }

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 123 / 456
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "123", 1);
  tokens.emplace_back(TokenType::RDIV, 1);
  tokens.emplace_back(TokenType::NUMBER, "456", 1);
  parser = Parser(tokens);
  result = parser.term();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("123");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("456");
  standard = std::make_unique<Term>();
  standard->setFirstFactor(std::move(fac1));
  standard->addFactor(Term::MulOp::RDIV, std::move(fac2));

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 123 div 456
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "123", 1);
  tokens.emplace_back(TokenType::DIV, 1);
  tokens.emplace_back(TokenType::NUMBER, "456", 1);
  parser = Parser(tokens);
  result = parser.term();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("123");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("456");
  standard = std::make_unique<Term>();
  standard->setFirstFactor(std::move(fac1));
  standard->addFactor(Term::MulOp::DIV, std::move(fac2));

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 123 mod 456
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "123", 1);
  tokens.emplace_back(TokenType::MOD, 1);
  tokens.emplace_back(TokenType::NUMBER, "456", 1);
  parser = Parser(tokens);
  result = parser.term();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("123");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("456");
  standard = std::make_unique<Term>();
  standard->setFirstFactor(std::move(fac1));
  standard->addFactor(Term::MulOp::MOD, std::move(fac2));

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 123 and 456
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "123", 1);
  tokens.emplace_back(TokenType::AND, 1);
  tokens.emplace_back(TokenType::NUMBER, "456", 1);
  parser = Parser(tokens);
  result = parser.term();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("123");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("456");
  standard = std::make_unique<Term>();
  standard->setFirstFactor(std::move(fac1));
  standard->addFactor(Term::MulOp::AND, std::move(fac2));

  EXPECT_TRUE(standard->checkSame(result.get()));
}

TEST(ParserTestSupplement, SimpleExpression) {
  // 1 + 2 * 3 - 4 or 5
  std::vector<Token> tokens;
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  tokens.emplace_back(TokenType::MINUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "4", 1);
  tokens.emplace_back(TokenType::OR, 1);
  tokens.emplace_back(TokenType::NUMBER, "5", 1);
  auto parser = Parser(tokens);
  auto result = parser.simpleExpression();

  auto fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  auto fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  auto fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  auto fac4 = std::make_unique<Factor>(FactorType::NUMBER);
  fac4->setValue("4");
  auto fac5 = std::make_unique<Factor>(FactorType::NUMBER);
  fac5->setValue("5");
  auto term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  auto term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  term2->addFactor(Term::MulOp::TIMES, std::move(fac3));
  auto term3 = std::make_unique<Term>();
  term3->setFirstFactor(std::move(fac4));
  auto term4 = std::make_unique<Term>();
  term4->setFirstFactor(std::move(fac5));
  auto standard = std::make_unique<SimpleExpression>();
  standard->setFirstTerm(std::move(term1));
  standard->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  standard->addTerm(SimpleExpression::AddOp::MINUS, std::move(term3));
  standard->addTerm(SimpleExpression::AddOp::OR, std::move(term4));

  // standard->accept(pt);
  // for (auto& a : pt.getResult()) {
  //   std::cout << a << std::endl;
  // }

  EXPECT_TRUE(standard->checkSame(result.get()));
}

TEST(ParserTestSupplement, Expression) {
  // 1 + 2 * 3
  std::vector<Token> tokens;
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  auto parser = Parser(tokens);
  auto result = parser.expression();

  auto fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  auto fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  auto fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  auto term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  auto term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  term2->addFactor(Term::MulOp::TIMES, std::move(fac3));
  auto simp_exp1 = std::make_unique<SimpleExpression>();
  simp_exp1->setFirstTerm(std::move(term1));
  simp_exp1->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  auto standard = std::make_unique<Expression>();
  standard->setLeft(std::move(simp_exp1));

  // standard->accept(pt);
  // for (auto& a : pt.getResult()) {
  //   std::cout << a << std::endl;
  // }

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 1 + 2 * 3 == 4
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  tokens.emplace_back(TokenType::EQUAL, 1);
  tokens.emplace_back(TokenType::NUMBER, "4", 1);
  parser = Parser(tokens);
  result = parser.expression();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  auto fac4 = std::make_unique<Factor>(FactorType::NUMBER);
  fac4->setValue("4");
  term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  term2->addFactor(Term::MulOp::TIMES, std::move(fac3));
  auto term3 = std::make_unique<Term>();
  term3->setFirstFactor(std::move(fac4));
  simp_exp1 = std::make_unique<SimpleExpression>();
  simp_exp1->setFirstTerm(std::move(term1));
  simp_exp1->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  auto simp_exp2 = std::make_unique<SimpleExpression>();
  simp_exp2->setFirstTerm(std::move(term3));
  standard = std::make_unique<Expression>();
  standard->setLeft(std::move(simp_exp1));
  standard->setRight(Expression::RelOp::EQ, std::move(simp_exp2));

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 1 + 2 * 3 < 4
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  tokens.emplace_back(TokenType::LT, 1);
  tokens.emplace_back(TokenType::NUMBER, "4", 1);
  parser = Parser(tokens);
  result = parser.expression();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  fac4 = std::make_unique<Factor>(FactorType::NUMBER);
  fac4->setValue("4");
  term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  term2->addFactor(Term::MulOp::TIMES, std::move(fac3));
  term3 = std::make_unique<Term>();
  term3->setFirstFactor(std::move(fac4));
  simp_exp1 = std::make_unique<SimpleExpression>();
  simp_exp1->setFirstTerm(std::move(term1));
  simp_exp1->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  simp_exp2 = std::make_unique<SimpleExpression>();
  simp_exp2->setFirstTerm(std::move(term3));
  standard = std::make_unique<Expression>();
  standard->setLeft(std::move(simp_exp1));
  standard->setRight(Expression::RelOp::LT, std::move(simp_exp2));

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 1 + 2 * 3 <= 4
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  tokens.emplace_back(TokenType::LE, 1);
  tokens.emplace_back(TokenType::NUMBER, "4", 1);
  parser = Parser(tokens);
  result = parser.expression();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  fac4 = std::make_unique<Factor>(FactorType::NUMBER);
  fac4->setValue("4");
  term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  term2->addFactor(Term::MulOp::TIMES, std::move(fac3));
  term3 = std::make_unique<Term>();
  term3->setFirstFactor(std::move(fac4));
  simp_exp1 = std::make_unique<SimpleExpression>();
  simp_exp1->setFirstTerm(std::move(term1));
  simp_exp1->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  simp_exp2 = std::make_unique<SimpleExpression>();
  simp_exp2->setFirstTerm(std::move(term3));
  standard = std::make_unique<Expression>();
  standard->setLeft(std::move(simp_exp1));
  standard->setRight(Expression::RelOp::LE, std::move(simp_exp2));

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 1 + 2 * 3 > 4
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  tokens.emplace_back(TokenType::GT, 1);
  tokens.emplace_back(TokenType::NUMBER, "4", 1);
  parser = Parser(tokens);
  result = parser.expression();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  fac4 = std::make_unique<Factor>(FactorType::NUMBER);
  fac4->setValue("4");
  term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  term2->addFactor(Term::MulOp::TIMES, std::move(fac3));
  term3 = std::make_unique<Term>();
  term3->setFirstFactor(std::move(fac4));
  simp_exp1 = std::make_unique<SimpleExpression>();
  simp_exp1->setFirstTerm(std::move(term1));
  simp_exp1->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  simp_exp2 = std::make_unique<SimpleExpression>();
  simp_exp2->setFirstTerm(std::move(term3));
  standard = std::make_unique<Expression>();
  standard->setLeft(std::move(simp_exp1));
  standard->setRight(Expression::RelOp::GT, std::move(simp_exp2));

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 1 + 2 * 3 >= 4
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  tokens.emplace_back(TokenType::GE, 1);
  tokens.emplace_back(TokenType::NUMBER, "4", 1);
  parser = Parser(tokens);
  result = parser.expression();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  fac4 = std::make_unique<Factor>(FactorType::NUMBER);
  fac4->setValue("4");
  term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  term2->addFactor(Term::MulOp::TIMES, std::move(fac3));
  term3 = std::make_unique<Term>();
  term3->setFirstFactor(std::move(fac4));
  simp_exp1 = std::make_unique<SimpleExpression>();
  simp_exp1->setFirstTerm(std::move(term1));
  simp_exp1->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  simp_exp2 = std::make_unique<SimpleExpression>();
  simp_exp2->setFirstTerm(std::move(term3));
  standard = std::make_unique<Expression>();
  standard->setLeft(std::move(simp_exp1));
  standard->setRight(Expression::RelOp::GE, std::move(simp_exp2));

  EXPECT_TRUE(standard->checkSame(result.get()));

  // 1 + 2 * 3 != 4
  tokens.clear();
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  tokens.emplace_back(TokenType::NE, 1);
  tokens.emplace_back(TokenType::NUMBER, "4", 1);
  parser = Parser(tokens);
  result = parser.expression();

  fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  fac4 = std::make_unique<Factor>(FactorType::NUMBER);
  fac4->setValue("4");
  term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  term2->addFactor(Term::MulOp::TIMES, std::move(fac3));
  term3 = std::make_unique<Term>();
  term3->setFirstFactor(std::move(fac4));
  simp_exp1 = std::make_unique<SimpleExpression>();
  simp_exp1->setFirstTerm(std::move(term1));
  simp_exp1->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  simp_exp2 = std::make_unique<SimpleExpression>();
  simp_exp2->setFirstTerm(std::move(term3));
  standard = std::make_unique<Expression>();
  standard->setLeft(std::move(simp_exp1));
  standard->setRight(Expression::RelOp::NE, std::move(simp_exp2));

  EXPECT_TRUE(standard->checkSame(result.get()));
}

TEST(ParserTestSupplement, ExpressionComplex) {
  // (1 + 2) * 3 = 4 + 5
  std::vector<Token> tokens;
  tokens.emplace_back(TokenType::LPAREN, 1);
  tokens.emplace_back(TokenType::NUMBER, "1", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "2", 1);
  tokens.emplace_back(TokenType::RPAREN, 1);
  tokens.emplace_back(TokenType::TIMES, 1);
  tokens.emplace_back(TokenType::NUMBER, "3", 1);
  tokens.emplace_back(TokenType::EQUAL, 1);
  tokens.emplace_back(TokenType::NUMBER, "4", 1);
  tokens.emplace_back(TokenType::PLUS, 1);
  tokens.emplace_back(TokenType::NUMBER, "5", 1);
  auto parser = Parser(tokens);
  auto result = parser.expression();

  auto fac1 = std::make_unique<Factor>(FactorType::NUMBER);
  fac1->setValue("1");
  auto fac2 = std::make_unique<Factor>(FactorType::NUMBER);
  fac2->setValue("2");
  auto fac3 = std::make_unique<Factor>(FactorType::NUMBER);
  fac3->setValue("3");
  auto fac4 = std::make_unique<Factor>(FactorType::NUMBER);
  fac4->setValue("4");
  auto fac5 = std::make_unique<Factor>(FactorType::NUMBER);
  fac5->setValue("5");

  auto term1 = std::make_unique<Term>();
  term1->setFirstFactor(std::move(fac1));
  auto term2 = std::make_unique<Term>();
  term2->setFirstFactor(std::move(fac2));
  auto simp_exp1 = std::make_unique<SimpleExpression>();
  simp_exp1->setFirstTerm(std::move(term1));
  simp_exp1->addTerm(SimpleExpression::AddOp::PLUS, std::move(term2));
  auto exp1 = std::make_unique<Expression>();
  exp1->setLeft(std::move(simp_exp1));
  auto fac_1 = std::make_unique<Factor>(FactorType::EXPRESSION);
  fac_1->setValue<ASTNodePtr>(std::move(exp1));
  auto term_1 = std::make_unique<Term>();
  term_1->setFirstFactor(std::move(fac_1));
  term_1->addFactor(Term::MulOp::TIMES, std::move(fac3));
  auto simp_exp_1 = std::make_unique<SimpleExpression>();
  simp_exp_1->setFirstTerm(std::move(term_1));
  auto standard = std::make_unique<Expression>();
  standard->setLeft(std::move(simp_exp_1));

  auto term3 = std::make_unique<Term>();
  term3->setFirstFactor(std::move(fac4));
  auto term4 = std::make_unique<Term>();
  term4->setFirstFactor(std::move(fac5));
  auto simp_exp2 = std::make_unique<SimpleExpression>();
  simp_exp2->setFirstTerm(std::move(term3));
  simp_exp2->addTerm(SimpleExpression::AddOp::PLUS, std::move(term4));

  standard->setRight(Expression::RelOp::EQ, std::move(simp_exp2));

  EXPECT_TRUE(standard->checkSame(result.get()));
}