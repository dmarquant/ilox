#pragma once

#include "expr.h"
#include "token.h"
#include <variant>
#include <vector>

using namespace std;


using Stmt = variant<
  struct BlockStmt,
  struct ExpressionStmt,
  struct IfStmt,
  struct PrintStmt,
  struct VarStmt,
  struct WhileStmt
>;

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


struct IfStmt {
  Expr* condition;
  Stmt* thenBranch;
  Stmt* elseBranch;

  IfStmt(Expr* condition, Stmt* thenBranch, Stmt* elseBranch) 
    : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
};

struct VarStmt {
  string name;
  Expr* initializer;

  VarStmt(string name, Expr* initializer) : name(name), initializer(initializer) {}
};

struct WhileStmt {
  Expr* condition;
  Stmt* body;

  WhileStmt(Expr* condition, Stmt* body) : condition(condition), body(body) {}
};

BlockStmt::BlockStmt(vector<Stmt*> statements) : statements(statements) {}
