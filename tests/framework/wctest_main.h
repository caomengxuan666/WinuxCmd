#pragma once
#include "wctest.h"

#define WC_TEST_MAIN()                    \
  int main() {                            \
    for (auto& t : wctest::registry()) {  \
      t.fn();                             \
    }                                     \
    return wctest::failures == 0 ? 0 : 1; \
  }
