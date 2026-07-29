#pragma once

#include <iostream>
#include "token.h"

using namespace std;

struct AssignmentExpr;
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct VarExpr;
struct UnaryExpr;

using Expr = variant<AssignmentExpr, BinaryExpr, GroupingExpr, LiteralExpr, VarExpr, UnaryExpr>;

struct AssignmentExpr {
  string name;
  Expr* val;

  AssignmentExpr(string name, Expr* val) : name(name), val(val) {}
};

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

struct VarExpr {
  string name;

  VarExpr(string name) : name(name) {}
};

struct UnaryExpr {
  Token op;
  Expr* expr;

  UnaryExpr(Token op, Expr* expr) : op(op), expr(expr) {}
};

