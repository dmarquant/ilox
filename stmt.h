#pragma once

#include "expr.h"
#include "token.h"
#include <variant>
#include <vector>

using namespace std;

struct BlockStmt;
struct ExpressionStmt;
struct PrintStmt;
struct VarStmt;

using Stmt = variant<BlockStmt, PrintStmt, ExpressionStmt, VarStmt>;

struct BlockStmt {
  vector<Stmt*> statements;

  BlockStmt(vector<Stmt*> statements);
};

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

BlockStmt::BlockStmt(vector<Stmt*> statements) : statements(statements) {}
