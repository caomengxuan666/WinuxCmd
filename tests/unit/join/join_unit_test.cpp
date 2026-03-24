/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: join_unit_test.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(join, join_basic) {
  TempDir tmp;
  tmp.write("file1.txt", "1 apple\n2 banana\n3 cherry\n");
  tmp.write("file2.txt", "1 red\n2 yellow\n3 red\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"join.exe", {L"file1.txt", L"file2.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  // Should join on the first field
  EXPECT_TRUE(r.stdout_text.find("1 apple red") != std::string::npos ||
              r.stdout_text.find("1 apple\tred") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("2 banana yellow") != std::string::npos ||
              r.stdout_text.find("2 banana\tyellow") != std::string::npos);
}

TEST(join, join_single_file) {
  TempDir tmp;
  tmp.write("file1.txt", "1 apple\n2 banana\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"join.exe", {L"file1.txt", L"file1.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  // Should match with itself
  EXPECT_TRUE(r.stdout_text.find("1 apple") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("2 banana") != std::string::npos);
}
