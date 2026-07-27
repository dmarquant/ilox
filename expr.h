#pragma once

#include <memory>
#include <iostream>
#include "token.h"

using namespace std;

struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;

struct Visitor {
  virtual void visitBinaryExpr(BinaryExpr* expr) = 0;
  virtual void visitGroupingExpr(GroupingExpr* expr) = 0;
  virtual void visitLiteralExpr(LiteralExpr* expr) = 0;
  virtual void visitUnaryExpr(UnaryExpr* expr) = 0;
};

struct Expr {
  virtual ~Expr() = default;
  virtual void accept(Visitor* visitor) = 0;
};

struct BinaryExpr : public Expr {
  unique_ptr<Expr> left;
  Token op;
  unique_ptr<Expr> right;

  BinaryExpr(Expr* left, Token op, Expr* right) : left(left), op(op), right(right) {}

  void accept(Visitor* visitor) override {
    visitor->visitBinaryExpr(this);
  }
};

struct GroupingExpr : public Expr {
  unique_ptr<Expr> expr;

  GroupingExpr(Expr* expr) : expr(expr) {}

  void accept(Visitor* visitor) override {
    visitor->visitGroupingExpr(this);
  }
};

struct LiteralExpr : public Expr {
  optional<Literal> value;

  LiteralExpr(optional<Literal> val) {
    this->value = val;
  }

  void accept(Visitor* visitor) override {
    visitor->visitLiteralExpr(this);
  }
};

struct UnaryExpr : public Expr {
  Token op;
  unique_ptr<Expr> expr;

  UnaryExpr(Token op, Expr* expr) : op(op), expr(expr) {}

  void accept(Visitor* visitor) override {
    visitor->visitUnaryExpr(this);
  }
};

struct AstPrinter : public Visitor {
  void printExpr(Expr* expr) {
    expr->accept(this);
  }

  void visitBinaryExpr(BinaryExpr* expr) override {
    cout << '(' << expr->op.lexeme << ' ';
    expr->left->accept(this);
    cout << ' ';
    expr->right->accept(this);
    cout << ')';
  }

  void visitGroupingExpr(GroupingExpr* expr) override {
    cout << "(group ";
    expr->expr->accept(this);
    cout << ')';
  }

  void visitUnaryExpr(UnaryExpr* expr) override {
    cout << '(' << expr->op.lexeme << ' ';
    expr->expr->accept(this);
    cout << ')';
  }

  void visitLiteralExpr(LiteralExpr* expr) override {
    if (expr->value.has_value()) {
      visit([](const auto& value) {
          cout << boolalpha << value;
      }, expr->value.value());
    } else {
      cout << "nil";
    }
  }
};
