#pragma once

#include "expr.h"

using namespace std;


struct Interpreter : public Visitor {
  Value value = nullopt;

  Value eval(Expr* expr) {
    expr->accept(this);
    return value;
  }

  void visitBinaryExpr(BinaryExpr* expr) override {
    Value left = eval(expr->left.get());
    Value right = eval(expr->right.get());

    switch (expr->op.type) {
      case TokenType::EQUAL_EQUAL:
        value = left == right;
        break;
      case TokenType::BANG_EQUAL:
        value = left != right;
        break;
      case TokenType::GREATER:
        value = get<double>(left.value()) > get<double>(right.value());
        break;
      case TokenType::GREATER_EQUAL:
        value = get<double>(left.value()) >= get<double>(right.value());
        break;
      case TokenType::LESS:
        value = get<double>(left.value()) < get<double>(right.value());
        break;
      case TokenType::LESS_EQUAL:
        value = get<double>(left.value()) <= get<double>(right.value());
        break;
      case TokenType::MINUS:
        value = get<double>(left.value()) - get<double>(right.value());
        break;
      case TokenType::SLASH:
        value = get<double>(left.value()) / get<double>(right.value());
        break;
      case TokenType::STAR:
        value = get<double>(left.value()) * get<double>(right.value());
        break;

      case TokenType::PLUS:
        if (holds_alternative<double>(left.value()) && holds_alternative<double>(right.value())) {
          value = get<double>(left.value()) + get<double>(right.value());
        } else if (holds_alternative<string>(left.value()) && holds_alternative<string>(right.value())) {
          value = get<string>(left.value()) + get<string>(right.value());
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
        value = -get<double>(right.value());
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
