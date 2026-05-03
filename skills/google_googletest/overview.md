# Overview

GoogleTest is Google's C++ testing framework that implements the xUnit architecture. It provides automatic test discovery, a rich set of assertions, and support for death tests, parameterized tests, and type-parameterized tests.

**When to use:**
- Unit testing C++ code
- Integration testing with mock objects (via GoogleMock)
- Testing error handling with death tests
- Running the same test logic with different inputs or types

**When not to use:**
- For testing non-C++ code
- When you need a lightweight header-only framework (consider Catch2)
- For performance benchmarking (use Google Benchmark instead)

**Key design principles:**
- Tests are automatically discovered via macros
- Fatal failures (ASSERT_*) stop the current test
- Non-fatal failures (EXPECT_*) allow the test to continue
- Test fixtures provide shared setup/teardown
- Death tests run in a separate process