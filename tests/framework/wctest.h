#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace wctest {
inline void (*before_each)(const char *name) = nullptr;

inline void (*after_each)(const char *name) = nullptr;

struct TestCase {
  const char *name;

  void (*fn)();
};

inline std::vector<TestCase> &registry() {
  static std::vector<TestCase> r;
  return r;
}

struct Registrar {
  Registrar(const char *name, void (*fn)()) {
    registry().push_back({name, fn});
  }
};

inline int failures = 0;

inline void fail(const char *file, int line, const std::string &msg) {
  std::cerr << file << ":" << line << ": " << msg << "\n";
  ++failures;
}
}  // namespace wctest

#define TEST(name)                                         \
  static void test_impl_##name();                          \
  static void test_##name() {                              \
    if (wctest::before_each) wctest::before_each(#name);   \
    test_impl_##name();                                    \
    if (wctest::after_each) wctest::after_each(#name);     \
  }                                                        \
  static wctest::Registrar reg_##name(#name, test_##name); \
  static void test_impl_##name()

#define EXPECT_TRUE(x)                                                        \
  do {                                                                        \
    if (!(x)) wctest::fail(__FILE__, __LINE__, "EXPECT_TRUE(" #x ") failed"); \
  } while (0)

#define EXPECT_EQ(a, b)                                                        \
  do {                                                                         \
    const auto &_a = (a);                                                      \
    const auto &_b = (b);                                                      \
    if (!(_a == _b)) {                                                         \
      std::ostringstream oss;                                                  \
      oss << "EXPECT_EQ failed\n  lhs: [" << _a << "]\n  rhs: [" << _b << "]"; \
      wctest::fail(__FILE__, __LINE__, oss.str());                             \
    }                                                                          \
  } while (0)
