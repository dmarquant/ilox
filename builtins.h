#pragma once

#include "token.h"
#include <chrono>

using namespace std;


struct ClockBuiltin : public Function {
  int arity() override {
    return 0;
  };

  Value call(Interpreter* interpreter, const vector<Value>& arguments) override {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now.time_since_epoch()).count();
  }
};

struct TostrBuiltin : public Function {
  int arity() override {
    return 1;
  };

  Value call(Interpreter* interpreter, const vector<Value>& arguments) override {
    assert(arguments.size() == 1);

    stringstream s;
    s << arguments[0];
    return s.str();
  }
};

struct PrintBuiltin : public Function {
  int arity() override {
    return 1;
  };

  Value call(Interpreter* interpreter, const vector<Value>& arguments) override;
};
