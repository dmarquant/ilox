#pragma once

#include <memory>
#include <iostream>
#include "token.h"

using namespace std;

struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;

using Expr = variant<BinaryExpr, GroupingExpr, LiteralExpr, UnaryExpr>;

struct BinaryExpr {
  Expr* left;
  Token op;
  Expr* right;

  BinaryExpr(Expr* left, Token op, Expr* right) : left(left), op(op), right(right) {}
};

struct GroupingExpr {
  Expr* expr;

  GroupingExpr(Expr* expr) : expr(expr) {}
};

struct LiteralExpr {
  Value value;

  LiteralExpr(Value val) : value(val) {}
};

struct UnaryExpr {
  Token op;
  Expr* expr;

  UnaryExpr(Token op, Expr* expr) : op(op), expr(expr) {}
};

