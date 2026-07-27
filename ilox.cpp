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
#include "parser.h"

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
  Expr* expr = parseExpr(tokens);
  
  if (expr) {
    AstPrinter p;
    p.printExpr(expr);
    cout << endl;
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
