#pragma once

#include "expr.h"

using namespace std;


struct Interpreter : public Visitor {
  Value value = nullptr;

  string error = "";

  Value eval(Expr* expr) {
    expr->accept(this);
    return value;
  }

  void setError(string err) {
    // Only report the first error
    if (error.empty()) {
      error = err;
    }
  }

  void visitBinaryExpr(BinaryExpr* expr) override {
    Value left = eval(expr->left.get());
    Value right = eval(expr->right.get());

    switch (expr->op.type) {
      case TokenType::COMMA:
        value = right;
        break;
      case TokenType::EQUAL_EQUAL:
        value = left == right;
        break;
      case TokenType::BANG_EQUAL:
        value = left != right;
        break;
      case TokenType::GREATER:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left) > get<double>(right);
        } else {
          value = nullptr;
        }
        break;
      case TokenType::GREATER_EQUAL:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left) >= get<double>(right);
        } else {
          value = nullptr;
        }
        break;
      case TokenType::LESS:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left) < get<double>(right);
        } else {
          value = nullptr;
        }
        break;
      case TokenType::LESS_EQUAL:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left) <= get<double>(right);
        } else {
          value = nullptr;
        }
        break;
      case TokenType::MINUS:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left) - get<double>(right);
        } else {
          value = nullptr;
        }
        break;
      case TokenType::SLASH:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left) / get<double>(right);
        } else {
          value = nullptr;
        }
        break;
      case TokenType::STAR:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left) * get<double>(right);
        } else {
          value = nullptr;
        }
        break;

      case TokenType::PLUS:
        if (holds_alternative<double>(left) && holds_alternative<double>(right)) {
          value = get<double>(left) + get<double>(right);
        } else if (holds_alternative<string>(left) && holds_alternative<string>(right)) {
          value = get<string>(left) + get<string>(right);
        } else {
          setError("Operands for '" + expr->op.lexeme + "' must both be numbers or strings");
        }
        break;

      default:
        value = nullptr;
        break;
    }
  }

  void visitGroupingExpr(GroupingExpr* expr) override {
    eval(expr->expr.get());
  }

  void visitUnaryExpr(UnaryExpr* expr) override {
    Value right = eval(expr->expr.get());

    switch (expr->op.type) {
      case TokenType::MINUS:
        if (checkNumber(expr->op, right)) {
          value = -get<double>(right);
        } else {
          value = nullptr;
        }
        break;

      case TokenType::BANG:
        value = !isTruthy(value);
        break;

      default:
        value = nullptr;
    }
  }

  void visitLiteralExpr(LiteralExpr* expr) override {
    value = expr->value;
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
