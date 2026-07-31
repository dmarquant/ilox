#pragma once

#include "expr.h"
#include "stmt.h"
#include "environment.h"
#include "builtins.h"

using namespace std;

static ClockBuiltin clockBuiltin;
static TostrBuiltin tostrBuiltin;
static PrintBuiltin printBuiltin;

struct Interpreter {
  string error = "";

  Environment globals;
  Environment* environment;

  Interpreter() {
    globals.define("clock", &clockBuiltin);
    globals.define("tostr", &tostrBuiltin);
    globals.define("print", &printBuiltin); // TODO: Remove the print keyword, otherwise this is not used
  }

  Value eval(Expr* expr) {
    return visit(*this, *expr);
  }

  void execute(const Stmt* stmt) {
    return visit(*this, *stmt);
  }

  void operator () (const WhileStmt& whileStmt) {
    while (true) {
      Value conditionVal = eval(whileStmt.condition);
      if (!isTruthy(conditionVal)) {
        break;
      } else {
        execute(whileStmt.body);
      }
    }
  }

  void operator () (const IfStmt& ifStmt) {
    Value conditionVal = eval(ifStmt.condition);
    if (isTruthy(conditionVal)) {
      execute(ifStmt.thenBranch);
    } else if (ifStmt.elseBranch) {
      execute(ifStmt.elseBranch);
    }
  }

  void operator () (const BlockStmt& block) {
    Environment scopeEnvironment{ .parent = environment };
    environment = &scopeEnvironment;
    for (const Stmt* stmt : block.statements) {
      execute(stmt);
    }
    environment = scopeEnvironment.parent;
  }

  void operator () (const PrintStmt& stmt) {
    Value val = eval(stmt.expr);
    cout << val << endl;
  }

  void operator () (const ExpressionStmt& stmt) {
    Value val = eval(stmt.expr);
    // Could do nothing, since expressions can't have sideeffects (yet)
  }

  void operator () (const VarStmt& stmt) {

    Value val = stmt.initializer ? eval(stmt.initializer) : nullptr;
    environment->define(stmt.name, val);
  }

  Value operator() (const AssignmentExpr& expr) {
    if (environment->has(expr.name)) {
      Value val = eval(expr.val);
      environment->assign(expr.name, val);
      return val;
    } else {
      setError("Undefined variable '" + expr.name + "'");
      return nullptr;
    }
  }

  Value operator() (const BinaryExpr& expr) {
    Value left = eval(expr.left);

    if (expr.op.type == TokenType::AND) {
      if (!isTruthy(left)) {
        return false;
      } else {
        return isTruthy(eval(expr.right));
      }
    }

    if (expr.op.type == TokenType::OR) {
      if (isTruthy(left)) {
        return true;
      } else {
        return isTruthy(eval(expr.right));
      }
    }

    Value right = eval(expr.right);

    switch (expr.op.type) {
      case TokenType::COMMA:
        return right;
      case TokenType::EQUAL_EQUAL:
        return left == right;
      case TokenType::BANG_EQUAL:
        return left != right;
      case TokenType::GREATER:
        if (checkNumbers(expr.op, left, right)) {
          return get<double>(left) > get<double>(right);
        } else {
          return nullptr;
        }
      case TokenType::GREATER_EQUAL:
        if (checkNumbers(expr.op, left, right)) {
          return get<double>(left) >= get<double>(right);
        } else {
          return nullptr;
        }
      case TokenType::LESS:
        if (checkNumbers(expr.op, left, right)) {
          return get<double>(left) < get<double>(right);
        } else {
          return nullptr;
        }
      case TokenType::LESS_EQUAL:
        if (checkNumbers(expr.op, left, right)) {
          return get<double>(left) <= get<double>(right);
        } else {
          return nullptr;
        }
      case TokenType::MINUS:
        if (checkNumbers(expr.op, left, right)) {
          return get<double>(left) - get<double>(right);
        } else {
          return nullptr;
        }
      case TokenType::SLASH:
        if (checkNumbers(expr.op, left, right)) {
          return get<double>(left) / get<double>(right);
        } else {
          return nullptr;
        }
      case TokenType::STAR:
        if (checkNumbers(expr.op, left, right)) {
          return get<double>(left) * get<double>(right);
        } else {
          return nullptr;
        }
        

      case TokenType::PLUS:
        if (holds_alternative<double>(left) && holds_alternative<double>(right)) {
          return get<double>(left) + get<double>(right);
        } else if (holds_alternative<string>(left) && holds_alternative<string>(right)) {
          return get<string>(left) + get<string>(right);
        } else {
          setError("Operands for '" + expr.op.lexeme + "' must both be numbers or strings");
        }
        break;

      default:
        break;
    }
    return nullptr;
  }

  Value operator() (const GroupingExpr& expr) {
    return eval(expr.expr);
  }

  Value operator() (const LiteralExpr& expr) {
    return expr.value;
  }

  Value operator() (const VarExpr& expr) {
    auto val = environment->get(expr.name);
    if (val.has_value()) {
      return val.value();
    } else {
      setError("Variable '" + expr.name + "' is not defined");
      return nullptr;
    }
  }

  Value operator() (const UnaryExpr& expr) {
    Value right = eval(expr.expr);

    switch (expr.op.type) {
      case TokenType::MINUS:
        if (checkNumber(expr.op, right)) {
          return -get<double>(right);
        } else {
          return nullptr;
        }
        break;

      case TokenType::BANG:
        return !isTruthy(right);
        break;

      default:
        return nullptr;
    }
  }

  Value operator() (const CallExpr& expr) {
    Value callee = eval(expr.callee);
    if (!holds_alternative<Function*>(callee)) {
      setError("Can only call functions and classes");
      return nullptr;
    }

    vector<Value> vals;
    for (Expr* arg : expr.arguments) {
      vals.push_back(eval(arg));
    }

    if (vals.size() != get<Function*>(callee)->arity()) {
      setError("Number of arguments doesn't match");
      return nullptr;
    }

    return get<Function*>(callee)->call(this, vals);
  }

  void setError(string err) {
    // Only report the first error
    if (error.empty()) {
      error = err;
    }
  }

  bool checkNumber(Token op, Value val) {
    if (!holds_alternative<double>(val)) {
      setError("Operand for '" + op.lexeme + "' must be a number");
      return false;
    } else {
      return true;
    }
  }

  bool checkNumbers(Token op, Value left, Value right) {
    if (!holds_alternative<double>(left) || !holds_alternative<double>(right)) {
      setError("Operands for '" + op.lexeme + "' must be numbers");
      return false;
    } else {
      return true;
    }
  }

  bool isTruthy(Value value) {
    if (holds_alternative<nullptr_t>(value)) {
      return false;
    } else if (holds_alternative<bool>(value)) {
      return get<bool>(value);
    } else {
      return true;
    }
  }
};
