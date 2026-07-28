#pragma once

#include <iostream>
#include "expr.h"
#include "token.h"

using namespace std;

struct PrintStmt {
  Expr* expr;

  PrintStmt(Expr* expr) : expr(expr) {}
};

struct ExpressionStmt {
  Expr* expr;

  ExpressionStmt(Expr* expr) : expr(expr) {}
};

struct VarStmt {
  string name;
  Expr* initializer;

  VarStmt(string name, Expr* initializer) : name(name), initializer(initializer) {}
};

using Stmt = variant<PrintStmt, ExpressionStmt, VarStmt>;

