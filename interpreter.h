#pragma once

#include "expr.h"

using namespace std;


struct Interpreter : public Visitor {
  Value value = nullopt;

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
          value = get<double>(left.value()) > get<double>(right.value());
        } else {
          value = nullopt;
        }
        break;
      case TokenType::GREATER_EQUAL:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left.value()) >= get<double>(right.value());
        } else {
          value = nullopt;
        }
        break;
      case TokenType::LESS:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left.value()) < get<double>(right.value());
        } else {
          value = nullopt;
        }
        break;
      case TokenType::LESS_EQUAL:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left.value()) <= get<double>(right.value());
        } else {
          value = nullopt;
        }
        break;
      case TokenType::MINUS:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left.value()) - get<double>(right.value());
        } else {
          value = nullopt;
        }
        break;
      case TokenType::SLASH:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left.value()) / get<double>(right.value());
        } else {
          value = nullopt;
        }
        break;
      case TokenType::STAR:
        if (checkNumbers(expr->op, left, right)) {
          value = get<double>(left.value()) * get<double>(right.value());
        } else {
          value = nullopt;
        }
        break;

      case TokenType::PLUS:
        if (holds_alternative<double>(left.value()) && holds_alternative<double>(right.value())) {
          value = get<double>(left.value()) + get<double>(right.value());
        } else if (holds_alternative<string>(left.value()) && holds_alternative<string>(right.value())) {
          value = get<string>(left.value()) + get<string>(right.value());
        } else {
          setError("Operands for '" + expr->op.lexeme + "' must both be numbers or strings");
        }
        break;

      default:
        value = nullopt;
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
          value = -get<double>(right.value());
        } else {
          value = nullopt;
        }
        break;

      case TokenType::BANG:
        value = !isTruthy(value);
        break;

      default:
        value = nullopt;
    }
  }

  void visitLiteralExpr(LiteralExpr* expr) override {
    value = expr->value;
  }

  bool checkNumber(Token op, Value val) {
    if (!val.has_value() || !holds_alternative<double>(val.value())) {
      setError("Operand for '" + op.lexeme + "' must be a number");
      return false;
    } else {
      return true;
    }
  }

  bool checkNumbers(Token op, Value left, Value right) {
    if (!left.has_value() || !right.has_value() 
        || !holds_alternative<double>(left.value()) || !holds_alternative<double>(right.value())) {
      setError("Operands for '" + op.lexeme + "' must be numbers");
      return false;
    } else {
      return true;
    }
  }

  bool isTruthy(Value value) {
    if (value.has_value()) {
      if (holds_alternative<bool>(value.value())) {
        return get<bool>(value.value());
      } else {
        return true;
      }
    } else {
      return false;
    }
  }
};
