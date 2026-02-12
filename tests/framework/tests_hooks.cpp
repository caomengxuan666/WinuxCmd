/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: tests_hooks.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/tests_hooks.h"

#include <chrono>
#include <iostream>

#include "framework/temp_dir.h"
#include "framework/tests_utils.h"

namespace wctest {
namespace {
std::vector<Hook>& before_hooks_impl() {
  static std::vector<Hook> instance;
  return instance;
}

std::vector<Hook>& after_hooks_impl() {
  static std::vector<Hook> instance;
  return instance;
}

class TimerHook {
  thread_local static std::chrono::steady_clock::time_point start_time;

 public:
  static void before(const char* group, const char* test_name) {
    (void)group;
    (void)test_name;
    start_time = std::chrono::steady_clock::now();
  }

  static void after(const char* group, const char* test_name) {
    (void)group;
    (void)test_name;
    auto end = std::chrono::steady_clock::now();
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time)
            .count();

    std::cout << "(" << ms << " ms) ";
  }
};

thread_local std::chrono::steady_clock::time_point TimerHook::start_time;

class TempDirHook {
 public:
  static void before(const char* group, const char* test_name) {
    (void)group;
    (void)test_name;
    make_temp_dir(test_name);
  }

  static void after(const char* group, const char* test_name) {
    (void)group;
    (void)test_name;
    clear_temp_dir();
  }
};
}  // namespace

std::vector<Hook>& before_hooks() { return before_hooks_impl(); }

std::vector<Hook>& after_hooks() { return after_hooks_impl(); }

void add_before_each(Hook h) { before_hooks_impl().push_back(h); }

void add_after_each(Hook h) { after_hooks_impl().push_back(h); }

void install_default_hooks() {
  static std::once_flag once_flag;
  std::call_once(once_flag, []() {
    add_before_each(
        [](const char* g, const char* n) { TimerHook::before(g, n); });
    add_after_each(
        [](const char* g, const char* n) { TimerHook::after(g, n); });
    add_before_each(
        [](const char* g, const char* n) { TempDirHook::before(g, n); });
    add_after_each(
        [](const char* g, const char* n) { TempDirHook::after(g, n); });
  });
}

namespace hooks {
void timer_before(const char* group, const char* test_name) {
  TimerHook::before(group, test_name);
}

void timer_after(const char* group, const char* test_name) {
  TimerHook::after(group, test_name);
}

void temp_dir_before(const char* group, const char* test_name) {
  TempDirHook::before(group, test_name);
}

void temp_dir_after(const char* group, const char* test_name) {
  TempDirHook::after(group, test_name);
}
}  // namespace hooks

namespace {
/**
 * @brief Automatic initializer for default hooks
 *
 * Uses global constructor to ensure default hooks are installed
 * before any tests run, providing consistent behavior out of the box.
 */
class GlobalHookInitializer {
 public:
  GlobalHookInitializer() { install_default_hooks(); }
};

// Global instance ensures initialization before main()
GlobalHookInitializer global_hook_initializer;
}  // namespace
}  // namespace wctest
