# Overview

Pystring is a C++ library that provides Python-style string manipulation functions for `std::string`. It implements common Python string methods like `split()`, `join()`, `strip()`, `replace()`, `find()`, `count()`, `startswith()`, `endswith()`, `lower()`, and `upper()` without requiring a Python interpreter.

**When to use:**
- When you need Python-like string operations in C++ projects
- In environments where both C++ and Python are used (e.g., game engines, VFX pipelines)
- When standard C++ string functions are insufficient for your needs

**When NOT to use:**
- For simple string operations that `std::string` already handles well
- In performance-critical code where you need maximum control over memory allocation
- When you can use C++20's `std::string` improvements or Boost.StringAlgo

**Key design:**
- All functions operate on `std::string` objects
- Functions return results by value or modify passed containers
- Consistent with Python's string API for familiarity
- Header-only implementation (single header file)