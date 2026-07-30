Interpreter of Lox language from the book Crafting Interpreters using C++.

# Building and running

Build with `bash build.sh` and compile + run with `bash run.sh`.

# Implementation details

`std::variant` is used to implement polymorphism of AST nodes. The visitor 
pattern is used to implement the interpretation.

The build is kept simple by using a single compilation unit and header only
components.

Exceptions are avoided, honestly leading to wonky error handling.
`std::expected` could be used to build better error handling but
it would be more verbose.

