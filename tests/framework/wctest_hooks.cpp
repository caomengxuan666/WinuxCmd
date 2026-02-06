#include <chrono>
#include <iostream>

#include "wctest.h"

namespace {

using clock = std::chrono::steady_clock;

clock::time_point g_start;
int g_failures_before = 0;

void on_before(const char* name) {
  g_failures_before = wctest::failures;
  g_start = clock::now();

  std::cerr << "[ RUN      ] " << name << "\n";
}

void on_after(const char* name) {
  auto end = clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - g_start)
                .count();

  if (wctest::failures == g_failures_before) {
    std::cerr << "[       OK ] " << name << " (" << ms << " ms)\n";
  } else {
    std::cerr << "[  FAILED  ] " << name << " (" << ms << " ms)\n";
  }
}

struct HookInstaller {
  HookInstaller() {
    wctest::before_each = &on_before;
    wctest::after_each = &on_after;
  }
};

static HookInstaller install;

}  // anonymous namespace
