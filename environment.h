#pragma once

#include "token.h"
#include <unordered_map>

using namespace std;


struct Environment {
  unordered_map<string, Value> values;

  void define(const string& name, Value value) {
    values[name] = value;
  }

  bool has(const string& name) {
    return values.contains(name);
  }

  optional<Value> get(const string& name) {
    if (values.contains(name)) {
      return values[name];
    }
    return nullopt;
  }
};
