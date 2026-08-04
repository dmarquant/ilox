#include "ilox.h"

#include <sstream>

using namespace std;

int main(int argc, char** argv) {
  const char* path = "examples/hello.lox";
  const auto src = readFile(path);
  if (!src) {
    cout << "Could not read source file '" << path << "'\n";
    exit(1);
  }

  stringstream stream;

  Interpreter interpreter(stream);
  run(&interpreter, src.value());

  cout << "Output was: " << stream.str() << endl;
}
