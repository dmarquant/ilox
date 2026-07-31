#pragma once

#include <iostream>
#include "token.h"

using namespace std;

using Expr = variant<
  struct AssignmentExpr,
  struct BinaryExpr,
  struct CallExpr,
  struct GroupingExpr,
  struct LiteralExpr,
  struct VarExpr,
  struct UnaryExpr
>;

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

struct CallExpr {
  Expr* callee;
  vector<Expr*> arguments;

  CallExpr(Expr* callee, vector<Expr*> arguments);
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

CallExpr::CallExpr(Expr* callee, vector<Expr*> arguments) : callee(callee), arguments(arguments) {}
