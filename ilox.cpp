#include "ilox.h"

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
