#pragma once

#include <assert.h>
#include <vector>
#include <unordered_map>

#include "ilox.h"
#include "token.h"

using namespace std;

unordered_map<string, TokenType> keywords = {
  {"and", TokenType::AND},
  {"class", TokenType::CLASS},
  {"else", TokenType::ELSE},
  {"false", TokenType::FALSE},
  {"for", TokenType::FOR},
  {"fun", TokenType::FUN},
  {"if", TokenType::IF},
  {"nil", TokenType::NIL},
  {"or", TokenType::OR},
  {"print", TokenType::PRINT},
  {"return", TokenType::RETURN},
  {"super", TokenType::SUPER},
  {"this", TokenType::THIS},
  {"true", TokenType::TRUE},
  {"var", TokenType::VAR},
  {"while", TokenType::WHILE},
};

struct Scanner {
  string src;
  vector<Token> tokens;

  int start = 0;
  int current = 0;
  int line = 1;

  vector<Token> scan() {
    while (!isAtEnd()) {
      start = current;
      scanToken();
    }

    tokens.push_back({.type = TokenType::END_OF_FILE, .lexeme = "", .literal = nullptr, .line = line});
    return tokens;
  }

  void scanToken() {
    char c = advance();
    switch (c) {
      case '(': addToken(TokenType::LEFT_PAREN); break;
      case ')': addToken(TokenType::RIGHT_PAREN); break;
      case '{': addToken(TokenType::LEFT_BRACE); break;
      case '}': addToken(TokenType::RIGHT_BRACE); break;
      case ',': addToken(TokenType::COMMA); break;
      case '.': addToken(TokenType::DOT); break;
      case '-': addToken(TokenType::MINUS); break;
      case '+': addToken(TokenType::PLUS); break;
      case ';': addToken(TokenType::SEMICOLON); break;
      case '*': addToken(TokenType::STAR); break;

      case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
      case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
      case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
      case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;

      case '/':
        if (match('/')) {
          while (peek() != '\n' && !isAtEnd()) advance();
        } else {
          addToken(TokenType::SLASH);
        }
        break;

      case '"': str(); break;

      case ' ':
      case '\r':
      case '\t':
        break;

      case '\n':
        line++;
        break;

      default:
        if (isDigit(c)) {
          number();
        } else if (isAlpha(c)) {
          identifier();
        } else {
          error(line, string("Unexpected character: '") + c + "'");
        }
        break;
    }
  }

  void identifier() {
    while (isAlphaNumeric(peek())) advance();

    string text = src.substr(start, current-start);
    auto it = keywords.find(text);
    if (it == keywords.end()) {
      addToken(TokenType::IDENTIFIER);
    } else {
      addToken(it->second);
    }
  }

  void number() {
    while (isDigit(peek()))
      advance();

    if (peek() == '.' && isDigit(peekNext())) {
      advance();

      while (isDigit(peek()))
        advance();
    }
    
    string text = src.substr(start, current-start);
    double value = stod(text);
    addToken(TokenType::NUMBER, value);
  }

  void str() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n') line++;
      advance();
    }
    
    if (isAtEnd()) {
      error(line, "Unterminated string.");
      return;
    }

    advance(); 

    string value = src.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
  }

  char advance() {
    assert(!isAtEnd());
    return src[current++];
  }

  char peek() {
    if (isAtEnd()) return '\0';
    return src[current];
  }

  char peekNext() {
    if (current + 1 >= src.length()) return '\0';
    return src[current+1];
  }

  void addToken(TokenType type) {
    addToken(type, nullptr);
  }

  bool match(char expected) {
    if (isAtEnd()) return false;
    if (src[current] != expected) return false;

    current++;
    return true;
  }

  void addToken(TokenType type, Value literal) {
    string text = src.substr(start, current-start);
    tokens.push_back({.type = type, .lexeme = text, .literal = literal, .line = line});
  }

  bool isDigit(char c) {
    return '0' <= c && c <= '9';
  }

  bool isAlpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
  }

  bool isAlphaNumeric(char c) {
    return isDigit(c) || isAlpha(c);
  }

  bool isAtEnd() {
    return current >= src.length();
  }
};

vector<Token> scanTokens(const string& src) {
  Scanner sc = {
    .src = src,
  };

  return sc.scan();
}
