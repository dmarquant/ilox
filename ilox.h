#include <iostream>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string_view>
#include <vector>

#include "expr.h"
#include "stmt.h"
#include "util.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "interpreter.h"

using namespace std;

bool hadError = false;

void report(int line, string where, string message) {
  cout << "[line " << line << "] Error" << where << ": " << message << '\n';
  hadError = true;
}

void error(int line, string message) {
  report(line, "", message);
}

void run(Interpreter* interpreter, string src) {
  // Expressions are leaked for now. To fix that I can simply add an arena allocator
  // for the parser and discard the whole thing once I'm done.
  vector<Token> tokens = scanTokens(src);
  optional<vector<Stmt*>> result = parseStatements(tokens);
  if (result.has_value()) {
    if (!interpreter->run(result.value())) {
      cout << "Error while evaluating: " << interpreter->error << endl;
    }
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

  Interpreter interpreter;
  run(&interpreter, src.value());

  if (hadError)
    exit(65);
}

void runPrompt() {
  string line;

  Interpreter interpreter;
  while (getline(cin, line)) {
    run(&interpreter, line);
    hadError = false;
  }
}

