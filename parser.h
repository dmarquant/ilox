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
    } else if (match(TokenType::FUN)) {
      return functionDeclaration();
    } else {
      return stmt();
    }
  }

  Stmt* functionDeclaration() {
    if (!expect(TokenType::IDENTIFIER, "Expected identifier after 'fun'")) {
      return nullptr;
    }

    string functionName = previous().lexeme;

    if (!expect(TokenType::LEFT_PAREN, "Expected '(' after function name")) {
      return nullptr;
    }

    vector<string> parameterNames;
    if (!match(TokenType::RIGHT_PAREN)) {
      do {
        if (!expect(TokenType::IDENTIFIER, "Expected identifier for parameter definition")) {
          return nullptr;
        }
        parameterNames.push_back(previous().lexeme);

      } while (match(TokenType::COMMA));

      if (!expect(TokenType::RIGHT_PAREN, "Expected ')' after parameter list"))
        return nullptr;
    }

    if (!expect(TokenType::LEFT_BRACE, "Expected function body after paramter list"))
      return nullptr;

    vector<Stmt*> body = blockStmt();

    return new Stmt(FunctionDecl(functionName, parameterNames, body));
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
    if (match(TokenType::LEFT_BRACE)) return new Stmt(BlockStmt(blockStmt()));
    if (match(TokenType::IF)) return ifStmt(); 
    if (match(TokenType::WHILE)) return whileStmt();
    if (match(TokenType::FOR)) return forStmt();
    return exprStmt();
  }

  Stmt* forStmt() {
    if (!expect(TokenType::LEFT_PAREN, "Expected '(' after for"))
      return nullptr;

    Stmt* initializer = nullptr;
    if (match(TokenType::SEMICOLON)) {
      // nothing todo
    } else if (match(TokenType::VAR)) {
      initializer = varDeclaration();
    } else {
      initializer = exprStmt();
    }

    Expr* condition = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
      condition = expr();
    } else {
      condition = new Expr(LiteralExpr(true));
    }

    if (!expect(TokenType::SEMICOLON, "Expected ';' after for condition"))
      return nullptr;

    Expr* increment = nullptr;
    if (peek().type != TokenType::RIGHT_PAREN) {
      increment = expr();      
    }

    if (!expect(TokenType::RIGHT_PAREN, "Expected ')' after for increment"))
      return nullptr;

    Stmt* body = stmt();
    if (increment) {
      body = new Stmt(BlockStmt({body, new Stmt(ExpressionStmt(increment))}));
    }

    body = new Stmt(WhileStmt(condition, body));

    if (initializer) {
      body = new Stmt(BlockStmt({initializer, body}));
    }
    return body;
  }


  Stmt* whileStmt() {
    if (!match(TokenType::LEFT_PAREN)) {
      hasError = true;
      error(peek().line, "Expected '(' after while");
      return nullptr;
    }

    Expr* condition = expr();

    if (!match(TokenType::RIGHT_PAREN)) {
      hasError = true;
      error(peek().line, "Expected ')' after while condition");
      return nullptr;
    }

    Stmt* body = stmt();
    return new Stmt(WhileStmt(condition, body));
  }

  Stmt* ifStmt() {
    if (!match(TokenType::LEFT_PAREN)) {
      hasError = true;
      error(peek().line, "Expected '(' after if");
      return nullptr;
    }

    Expr* condition = expr();

    if (!match(TokenType::RIGHT_PAREN)) {
      hasError = true;
      error(peek().line, "Expected ')' after if condition");
      return nullptr;
    }

    Stmt* thenBranch = stmt();

    Stmt* elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
      elseBranch = stmt();
    }

    return new Stmt(IfStmt(condition, thenBranch, elseBranch));
  }

  vector<Stmt*> blockStmt() {
      vector<Stmt*> statements;
      while (!match(TokenType::RIGHT_BRACE)) {
        statements.push_back(declaration());
      }
      return statements;
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
    return hasError ? nullptr : assignment();
  }

  Expr* assignment() {
    Expr* expr = orExpr();

    if (match(TokenType::EQUAL)) {
      Token equals = previous();
      Expr* value = assignment();

      if (holds_alternative<VarExpr>(*expr)) {
        string name = get<VarExpr>(*expr).name;
        return new Expr(AssignmentExpr(name, value));
      }
      error(equals.line, "Invalid assignment target.");
      return nullptr;
    }
    return expr;
  }

  Expr* orExpr() {
    Expr* expr = andExpr();
    while (match(TokenType::OR)) {
      Token op = previous();
      Expr* right = orExpr();
      expr = new Expr(BinaryExpr(expr, op, right));
    }
    return expr;
  }

  Expr* andExpr() {
    Expr* expr = equality();
    while (match(TokenType::AND)) {
      Token op = previous();
      Expr* right = andExpr();
      expr = new Expr(BinaryExpr(expr, op, right));
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
    return call();
  }

  Expr* call() {
    Expr* callee = primary();
    while (true) {
      if (match(TokenType::LEFT_PAREN)) {
        callee = finishCall(callee);
      } else {
        break;
      }
    }
    return callee;
  }

  Expr* finishCall(Expr* callee) {
    vector<Expr*> arguments;
    if (peek().type != TokenType::RIGHT_PAREN) {
      do {
        arguments.push_back(expr());
      } while (match(TokenType::COMMA));
    }

    if (!expect(TokenType::RIGHT_PAREN, "Expected ')' after argument list"))
      return nullptr;

    return new Expr(CallExpr(callee, arguments));
  }

  Expr* primary() {
    if (match(TokenType::FALSE)) return new Expr(LiteralExpr(false));
    if (match(TokenType::TRUE)) return new Expr(LiteralExpr(true));
    if (match(TokenType::NIL)) return new Expr(LiteralExpr(nullptr));

    if (match(TokenType::NUMBER) || match(TokenType::STRING)) {
      return new Expr(LiteralExpr(previous().literal));
    }

    if (match(TokenType::IDENTIFIER)) {
      return new Expr(VarExpr(previous().lexeme));
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

  bool expect(TokenType type, string msg) {
    if (!match(type)) {
      hasError = true;
      error(peek().line, msg);
      return false;
    } else {
      return true;
    }
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
