#pragma once

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
  virtual void accept(Visitor* visitor) = 0;
};




struct BinaryExpr : public Expr {
  Expr* left;
  Token op;
  Expr* right;

  void accept(Visitor* visitor) override {
    visitor->visitBinaryExpr(this);
  }
};

struct GroupingExpr : public Expr {
  Expr* expr;

  void accept(Visitor* visitor) override {
    visitor->visitGroupingExpr(this);
  }
};

struct LiteralExpr : public Expr {
  Literal value;

  void accept(Visitor* visitor) override {
    visitor->visitLiteralExpr(this);
  }
};

struct UnaryExpr : public Expr {
  Token op;
  Expr* expr;

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
    visit([](const auto& value) {
        cout << value;
    }, expr->value);
  }
};
