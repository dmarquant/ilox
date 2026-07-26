#include <iostream>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string_view>
#include <vector>

#include "expr.h"
#include "ilox.h"
#include "scanner.h"
#include "token.h"

using namespace std;

bool hadError = false;


void report(int line, string where, string message) {
  cout << "[line " << line << "] Error" << where << ": " << message << '\n';
  hadError = true;
}

void error(int line, string message) {
  report(line, "", message);
}

void run(string src) {
  vector<Token> tokens = scanTokens(src);
  for (Token token : tokens) {
    cout << token << '\n';
  }
}

optional<string> readFile(string path) {
  filesystem::path filePath = path;
  ifstream file(filePath);

  if (!file.is_open()) {
    return optional<string>();
  }

  ostringstream buffer;
  buffer << file.rdbuf();

  return buffer.str();
}

void runFile(string path) {
  const auto src = readFile(path);
  if (!src) {
    cout << "Could not read source file '" << path << "'\n";
    exit(1);
  }
  run(src.value());

  if (hadError)
    exit(65);
}

void runPrompt() {
  string line;

  while (getline(cin, line)) {
    run(line);
    hadError = false;
  }
}

int main(int argc, char** argv) {
  LiteralExpr* l = new LiteralExpr();
  l->value = 43.1;

  UnaryExpr* u = new UnaryExpr();
  u->op = { .type = TokenType::PLUS, .lexeme = "+" };
  u->expr = l;
  
  AstPrinter p;
  p.printExpr(u);
  cout << endl;

  if (argc > 2) {
    cout << "Usage: jlox [script]\n";
    return 64;
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    runPrompt();
  }
  return 0;
}
