#pragma once

#include "token.h"
#include <unordered_map>

using namespace std;


struct Environment {
  Environment* parent = nullptr;
  unordered_map<string, Value> values;

  void define(const string& name, Value value) {
    values[name] = value;
  }

  bool has(const string& name) {
    if (values.contains(name)) {
      return true;
    } else if (parent) {
      return parent->has(name);
    } else {
      return false;
    }
  }

  optional<Value> get(const string& name) {
    if (values.contains(name)) {
      return values[name];
    } else if (parent) {
      return parent->get(name);
    } else {
      return nullopt;
    }
  }
};
