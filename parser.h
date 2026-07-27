#pragma once

#include "expr.h"
#include "token.h"

using namespace std;

struct Parser {
  vector<Token> tokens;

  int current = 0;
  bool hasError = false;

  Expr* expr() {
    return hasError ? nullptr : comma();
  }

  Expr* comma() {
    Expr* expr = equality();

    while (match(TokenType::COMMA)) {
      Token op = previous();
      Expr* right = comma();
      expr = new BinaryExpr(expr, op, right);
    }
    return expr;
  }

  Expr* equality() {
    Expr* expr = comparison();

    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
      Token op = previous();
      Expr* right = equality();
      expr = new BinaryExpr(expr, op, right);
    }
    return expr;
  }

  Expr* comparison() {
    Expr* expr = term();

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) 
        || match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {

      Token op = previous();
      Expr* right = comparison();
      expr = new BinaryExpr(expr, op, right);
    }
    return expr;
  }

  Expr* term() {
    Expr* expr = factor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
      Token op = previous();
      Expr* right = term();
      expr = new BinaryExpr(expr, op, right);
    }
    return expr;
  }

  Expr* factor() {
    Expr* expr = unary();

    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
      Token op = previous();
      Expr* right = factor();
      expr = new BinaryExpr(expr, op, right);
    }
    return expr;
  }

  Expr* unary() {
    if (match(TokenType::BANG) || match(TokenType::MINUS)) {
      Token op = previous();
      Expr* right = unary();
      return new UnaryExpr(op, right);
    }
    return primary();
  }

  Expr* primary() {
    if (match(TokenType::FALSE)) return new LiteralExpr(false);
    if (match(TokenType::TRUE)) return new LiteralExpr(true);
    if (match(TokenType::NIL)) return new LiteralExpr(nullopt);

    if (match(TokenType::NUMBER) || match(TokenType::STRING)) {
      return new LiteralExpr(previous().literal.value());
    }

    if (match(TokenType::LEFT_PAREN)) {
      Expr* expr = this->expr();
      if (match(TokenType::RIGHT_PAREN)) {
        return new GroupingExpr(expr);
      } else {
        hasError = true;
        error(peek().line, "Expected ')' after expression.");
        return 0;
      }
    }
    hasError = true;
    return 0;
  }

  bool match(TokenType type) {
    if (peek().type == type) {
      advance();
      return true;
    }
    return false;
  }

  bool isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
  }

  Token peek() {
    return tokens[current];
  }

  Token previous() {
    assert(current > 0);
    return tokens[current-1];
  }

  Token advance() {
    if (!isAtEnd()) current++;
    return previous();
  }
};

Expr* parseExpr(const vector<Token>& tokens) {
  Parser p{
    .tokens = tokens
  };
  return p.expr();
}
