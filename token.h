#pragma once

#include <iostream>
#include <string>
#include <variant>

struct Interpreter;

using namespace std;

enum class TokenType {
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,

    IDENTIFIER, STRING, NUMBER,

    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    END_OF_FILE
};

constexpr std::string_view tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_PAREN:    return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:   return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE:    return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:   return "RIGHT_BRACE";
        case TokenType::COMMA:         return "COMMA";
        case TokenType::DOT:           return "DOT";
        case TokenType::MINUS:         return "MINUS";
        case TokenType::PLUS:          return "PLUS";
        case TokenType::SEMICOLON:     return "SEMICOLON";
        case TokenType::SLASH:         return "SLASH";
        case TokenType::STAR:          return "STAR";

        case TokenType::BANG:          return "BANG";
        case TokenType::BANG_EQUAL:    return "BANG_EQUAL";
        case TokenType::EQUAL:         return "EQUAL";
        case TokenType::EQUAL_EQUAL:   return "EQUAL_EQUAL";
        case TokenType::GREATER:       return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS:          return "LESS";
        case TokenType::LESS_EQUAL:    return "LESS_EQUAL";

        case TokenType::IDENTIFIER:    return "IDENTIFIER";
        case TokenType::STRING:        return "STRING";
        case TokenType::NUMBER:        return "NUMBER";

        case TokenType::AND:           return "AND";
        case TokenType::CLASS:         return "CLASS";
        case TokenType::ELSE:          return "ELSE";
        case TokenType::FALSE:         return "FALSE";
        case TokenType::FUN:           return "FUN";
        case TokenType::FOR:           return "FOR";
        case TokenType::IF:            return "IF";
        case TokenType::NIL:           return "NIL";
        case TokenType::OR:            return "OR";
        case TokenType::RETURN:        return "RETURN";
        case TokenType::SUPER:         return "SUPER";
        case TokenType::THIS:          return "THIS";
        case TokenType::TRUE:          return "TRUE";
        case TokenType::VAR:           return "VAR";
        case TokenType::WHILE:         return "WHILE";

        case TokenType::END_OF_FILE:   return "END_OF_FILE";
    }
    return "UNKNOWN";
}


using Value = variant<bool, double, string, struct Function*, nullptr_t>;

struct Function {
  virtual int arity() = 0;

  virtual Value call(Interpreter* interpreter, const vector<Value>& arguments) = 0;
};

struct Token {
    TokenType type;
    string lexeme;
    Value literal;
    int line;
};

ostream& operator<<(ostream& os, const Value& value) {
  if (holds_alternative<nullptr_t>(value)) {
    os << "nil";
  } else {
    os << boolalpha;
    visit([&os](const auto& value) { os << value; }, value);
  }
  return os;
}
