#pragma once

#include "expr.h"
#include "stmt.h"
#include "token.h"

using namespace std;

struct Parser {
  vector<Token> tokens;

  int current = 0;
  bool hasError = false;

  vector<Stmt*> parse() {
    vector<Stmt*> statements;
    while (!isAtEnd()) {
      statements.push_back(declaration());
    }
    return statements;    
  }

  Stmt* declaration() {
    if (match(TokenType::VAR)) {
      return varDeclaration();
    } else {
      return stmt();
    }
  }

  Stmt* varDeclaration() {
    if (match(TokenType::IDENTIFIER)) {
      Token name = previous();

      Expr* initializer = nullptr;
      if (match(TokenType::EQUAL)) {
        initializer = expr();
      }

      if (!match(TokenType::SEMICOLON)) {
        hasError = true;
        error(peek().line, "Expected ';' after statement");
        return nullptr;
      }

      return new Stmt(VarStmt(name.lexeme, initializer));
    } else {
      hasError = true;
      error(peek().line, "Expected identifier after 'var'");
      return nullptr;
    }
  }

  Stmt* stmt() {
    if (match(TokenType::PRINT)) return printStmt();
    return exprStmt();
  }

  Stmt* printStmt() {
    Expr* value = expr();

    if (!match(TokenType::SEMICOLON)) {
      hasError = true;
      error(peek().line, "Expected ';' after expression");
      return nullptr;
    }

    return new Stmt(PrintStmt(value));
  }

  Stmt* exprStmt() {
    Expr* value = expr();

    if (!match(TokenType::SEMICOLON)) {
      hasError = true;
      error(peek().line, "Expected ';' after expression");
      return nullptr;
    }

    return new Stmt(ExpressionStmt(value));
  }

  Expr* expr() {
    return hasError ? nullptr : comma();
  }

  Expr* comma() {
    Expr* expr = equality();

    while (match(TokenType::COMMA)) {
      Token op = previous();
      Expr* right = comma();
      *expr = new Expr(BinaryExpr(expr, op, right));
    }
    return expr;
  }

  Expr* equality() {
    Expr* expr = comparison();

    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
      Token op = previous();
      Expr* right = equality();
      expr = new Expr(BinaryExpr(expr, op, right));
    }
    return expr;
  }

  Expr* comparison() {
    Expr* expr = term();

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) 
        || match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {

      Token op = previous();
      Expr* right = comparison();
      expr = new Expr(BinaryExpr(expr, op, right));
    }
    return expr;
  }

  Expr* term() {
    Expr* expr = factor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
      Token op = previous();
      Expr* right = term();
      expr = new Expr(BinaryExpr(expr, op, right));
    }
    return expr;
  }

  Expr* factor() {
    Expr* expr = unary();

    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
      Token op = previous();
      Expr* right = factor();
      expr = new Expr(BinaryExpr(expr, op, right));
    }
    return expr;
  }

  Expr* unary() {
    if (match(TokenType::BANG) || match(TokenType::MINUS)) {
      Token op = previous();
      Expr* right = unary();
      return new Expr(UnaryExpr(op, right));
    }
    return primary();
  }

  Expr* primary() {
    if (match(TokenType::FALSE)) return new Expr(LiteralExpr(false));
    if (match(TokenType::TRUE)) return new Expr(LiteralExpr(true));
    if (match(TokenType::NIL)) return new Expr(LiteralExpr(nullptr));

    if (match(TokenType::NUMBER) || match(TokenType::STRING)) {
      return new Expr(LiteralExpr(previous().literal));
    }

    if (match(TokenType::LEFT_PAREN)) {
      Expr* expr = this->expr();
      if (match(TokenType::RIGHT_PAREN)) {
        return new Expr(GroupingExpr(expr));
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

optional<vector<Stmt*>> parseStatements(const vector<Token>& tokens) {
  Parser p{
    .tokens = tokens
  };
  auto statements = p.parse();
  if (p.hasError) {
    return nullopt;
  } else {
    return statements;
  }
}
