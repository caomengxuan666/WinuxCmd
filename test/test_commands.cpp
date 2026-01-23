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
 *  - File: test_commands.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
// Test file for WinuxCmd commands
#include <windows.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// Helper function to run a command and capture its output
std::string runCommand(const std::string& command) {
  std::string result;
  char buffer[128];
  // Replace forward slashes with backslashes for Windows
  std::string windowsCommand = command;
  size_t pos = 0;
  while ((pos = windowsCommand.find('/', pos)) != std::string::npos) {
    windowsCommand.replace(pos, 1, "\\");
    pos += 1;
  }
  // Remove leading ./ if present
  if (windowsCommand.substr(0, 2) == ".\\") {
    windowsCommand = windowsCommand.substr(2);
  }
  // Add log output
  std::cout << "Running command: " << windowsCommand << std::endl;
  FILE* pipe = _popen(windowsCommand.c_str(), "r");
  if (!pipe) {
    std::cout << "Failed to open pipe for command: " << windowsCommand
              << std::endl;
    return "ERROR";
  }
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  int status = _pclose(pipe);
  std::cout << "Command exit status: " << status << std::endl;
  std::cout << "Command output: " << result << std::endl;
  return result;
}

// Helper function to create a test file
void createTestFile(const std::string& path, const std::string& content) {
  std::ofstream file(path);
  file << content;
  file.close();
}

// Helper function to read a file's content
std::string readFile(const std::string& path) {
  std::ifstream file(path);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();
  return content;
}

// Helper function to remove a directory recursively
void removeDirectoryRecursive(const std::string& path) {
  if (fs::exists(path)) {
    fs::remove_all(path);
  }
}

// Test cases for cp command
TEST_CASE("cp command tests", "[cp]") {
  // Clean up before test
  removeDirectoryRecursive("source_dir");
  removeDirectoryRecursive("dest_dir");
  removeDirectoryRecursive("dest_dir_recursive");
  removeDirectoryRecursive("target_dir");
  fs::remove("source_file.txt");
  fs::remove("dest_file.txt");
  fs::remove("existing_file.txt");

  // Test 1: Basic cp (single file to file)
  SECTION("Basic cp (single file to file)") {
    createTestFile("source_file.txt", "Hello, World!");
    runCommand("./cmake-build-release/cp.exe source_file.txt dest_file.txt");
    REQUIRE(fs::exists("dest_file.txt"));
    REQUIRE(readFile("dest_file.txt") == "Hello, World!");
  }

  // Test 2: cp with -r option (recursive)
  SECTION("cp with -r option (recursive)") {
    fs::create_directory("source_dir");
    createTestFile("source_dir/file_in_dir.txt", "Content in dir");
    runCommand("./cmake-build-release/cp.exe -r source_dir dest_dir_recursive");
    REQUIRE(fs::exists("dest_dir_recursive"));
    REQUIRE(fs::exists("dest_dir_recursive/file_in_dir.txt"));
    REQUIRE(readFile("dest_dir_recursive/file_in_dir.txt") == "Content in dir");
  }

  // Test 3: cp with -i option (interactive)
  SECTION("cp with -i option (interactive)") {
    createTestFile("source_file.txt", "New content");
    createTestFile("existing_file.txt", "Existing content");
    // Use echo 'y' to answer yes to the prompt
    runCommand(
        "echo y | ./cmake-build-release/cp.exe -i source_file.txt "
        "existing_file.txt");
    REQUIRE(readFile("existing_file.txt") == "New content");
  }

  // Test 4: cp with -f option (force)
  SECTION("cp with -f option (force)") {
    createTestFile("source_file.txt", "New content");
    createTestFile("existing_file.txt", "Existing content");
    runCommand(
        "./cmake-build-release/cp.exe -f source_file.txt existing_file.txt");
    REQUIRE(readFile("existing_file.txt") == "New content");
  }

  // Test 5: cp with -v option (verbose)
  SECTION("cp with -v option (verbose)") {
    createTestFile("source_file.txt", "Hello, World!");
    std::string output = runCommand(
        "./cmake-build-release/cp.exe -v source_file.txt dest_file.txt");
    REQUIRE(output.find("'source_file.txt' -> 'dest_file.txt'") !=
            std::string::npos);
    REQUIRE(fs::exists("dest_file.txt"));
  }

  // Test 6: cp with -t option (target directory)
  SECTION("cp with -t option (target directory)") {
    fs::create_directory("target_dir");
    createTestFile("source_file.txt", "Hello, World!");
    createTestFile("source_file2.txt", "Hello, World! 2");
    runCommand(
        "./cmake-build-release/cp.exe -t target_dir source_file.txt "
        "source_file2.txt");
    REQUIRE(fs::exists("target_dir/source_file.txt"));
    REQUIRE(fs::exists("target_dir/source_file2.txt"));
  }

  // Clean up after test
  removeDirectoryRecursive("source_dir");
  removeDirectoryRecursive("dest_dir_recursive");
  removeDirectoryRecursive("target_dir");
  fs::remove("source_file.txt");
  fs::remove("source_file2.txt");
  fs::remove("dest_file.txt");
  fs::remove("existing_file.txt");
}

// Test cases for rm command
TEST_CASE("rm command tests", "[rm]") {
  // Clean up before test
  removeDirectoryRecursive("source_dir");
  removeDirectoryRecursive("nested_dir");
  removeDirectoryRecursive("empty_dir");
  fs::remove("source_file.txt");
  fs::remove("file1.txt");
  fs::remove("file2.txt");
  fs::remove("file3.txt");
  fs::remove("file4.txt");

  // Test 1: Basic rm (single file)
  SECTION("Basic rm (single file)") {
    createTestFile("source_file.txt", "Hello, World!");
    REQUIRE(fs::exists("source_file.txt"));
    runCommand("./cmake-build-release/rm.exe source_file.txt");
    REQUIRE(!fs::exists("source_file.txt"));
  }

  // Test 2: rm with -rfv option (recursive, force, verbose)
  SECTION("rm with -rfv option (recursive, force, verbose)") {
    fs::create_directories("nested_dir/dir1/dir2");
    createTestFile("nested_dir/file1.txt", "Content 1");
    createTestFile("nested_dir/dir1/file2.txt", "Content 2");
    createTestFile("nested_dir/dir1/dir2/file3.txt", "Content 3");
    REQUIRE(fs::exists("nested_dir"));
    // Add log output
    std::cout << "Before rm -rfv: nested_dir exists = "
              << fs::exists("nested_dir") << std::endl;
    std::cout << "Before rm -rfv: nested_dir/file1.txt exists = "
              << fs::exists("nested_dir/file1.txt") << std::endl;
    std::cout << "Before rm -rfv: nested_dir/dir1/file2.txt exists = "
              << fs::exists("nested_dir/dir1/file2.txt") << std::endl;
    std::cout << "Before rm -rfv: nested_dir/dir1/dir2/file3.txt exists = "
              << fs::exists("nested_dir/dir1/dir2/file3.txt") << std::endl;
    // Run rm command and capture output
    std::string output =
        runCommand("./cmake-build-release/rm.exe -rfv nested_dir");
    std::cout << "rm -rfv output: " << output << std::endl;
    // Add a short delay to allow the operating system to complete the deletion
    Sleep(100);
    // Add log output
    std::cout << "After rm -rfv: nested_dir exists = "
              << fs::exists("nested_dir") << std::endl;
    if (fs::exists("nested_dir")) {
      std::cout << "After rm -rfv: nested_dir contents:" << std::endl;
      for (const auto& entry : fs::directory_iterator("nested_dir")) {
        std::cout << "  " << entry.path().string() << std::endl;
      }
    }
    REQUIRE(!fs::exists("nested_dir"));
  }

  // Test 3: rm with -d option (remove empty directory)
  SECTION("rm with -d option (remove empty directory)") {
    fs::create_directory("empty_dir");
    REQUIRE(fs::exists("empty_dir"));
    runCommand("./cmake-build-release/rm.exe -d empty_dir");
    REQUIRE(!fs::exists("empty_dir"));
  }

  // Test 4: rm with -f option (force)
  SECTION("rm with -f option (force)") {
    createTestFile("source_file.txt", "Hello, World!");
    REQUIRE(fs::exists("source_file.txt"));
    runCommand("./cmake-build-release/rm.exe -f source_file.txt");
    REQUIRE(!fs::exists("source_file.txt"));
  }

  // Test 5: rm with -v option (verbose)
  SECTION("rm with -v option (verbose)") {
    createTestFile("source_file.txt", "Hello, World!");
    std::string output =
        runCommand("./cmake-build-release/rm.exe -v source_file.txt");
    REQUIRE(output.find("removed 'source_file.txt'") != std::string::npos);
    REQUIRE(!fs::exists("source_file.txt"));
  }

  // Clean up after test
  removeDirectoryRecursive("source_dir");
  removeDirectoryRecursive("nested_dir");
  removeDirectoryRecursive("empty_dir");
  fs::remove("source_file.txt");
  fs::remove("file1.txt");
  fs::remove("file2.txt");
  fs::remove("file3.txt");
  fs::remove("file4.txt");
}

// Test cases for mv command
TEST_CASE("mv command tests", "[mv]") {
  // Clean up before test
  fs::remove("source_file.txt");
  fs::remove("dest_file.txt");
  fs::remove("new_file.txt");
  fs::remove("existing_file.txt");
  removeDirectoryRecursive("target_dir");

  // Test 1: Basic mv (single file to file)
  SECTION("Basic mv (single file to file)") {
    createTestFile("source_file.txt", "Hello, World!");
    runCommand("./cmake-build-release/mv.exe source_file.txt dest_file.txt");
    REQUIRE(!fs::exists("source_file.txt"));
    REQUIRE(fs::exists("dest_file.txt"));
    REQUIRE(readFile("dest_file.txt") == "Hello, World!");
  }

  // Test 2: mv with -i option (interactive)
  SECTION("mv with -i option (interactive)") {
    createTestFile("existing_file.txt", "Existing content");
    createTestFile("new_file.txt", "New content");
    // Use echo 'y' to answer yes to the prompt
    runCommand(
        "echo y | ./cmake-build-release/mv.exe -i new_file.txt "
        "existing_file.txt");
    REQUIRE(!fs::exists("new_file.txt"));
    REQUIRE(fs::exists("existing_file.txt"));
    REQUIRE(readFile("existing_file.txt") == "New content");
  }

  // Test 3: mv with -f option (force)
  SECTION("mv with -f option (force)") {
    createTestFile("existing_file.txt", "Existing content");
    createTestFile("new_file.txt", "New content");
    runCommand(
        "./cmake-build-release/mv.exe -f new_file.txt existing_file.txt");
    REQUIRE(!fs::exists("new_file.txt"));
    REQUIRE(fs::exists("existing_file.txt"));
    REQUIRE(readFile("existing_file.txt") == "New content");
  }

  // Test 4: mv with -n option (no clobber)
  SECTION("mv with -n option (no clobber)") {
    createTestFile("existing_file.txt", "Existing content");
    createTestFile("new_file.txt", "New content");
    runCommand(
        "./cmake-build-release/mv.exe -n new_file.txt existing_file.txt");
    REQUIRE(fs::exists("new_file.txt"));  // Source file should still exist
    REQUIRE(fs::exists("existing_file.txt"));
    REQUIRE(readFile("existing_file.txt") ==
            "Existing content");  // Existing file should not be overwritten
  }

  // Test 5: mv with -v option (verbose)
  SECTION("mv with -v option (verbose)") {
    createTestFile("source_file.txt", "Hello, World!");
    std::string output = runCommand(
        "./cmake-build-release/mv.exe -v source_file.txt dest_file.txt");
    REQUIRE(output.find("'source_file.txt' -> 'dest_file.txt'") !=
            std::string::npos);
    REQUIRE(!fs::exists("source_file.txt"));
    REQUIRE(fs::exists("dest_file.txt"));
  }

  // Test 6: mv with -t option (target directory)
  SECTION("mv with -t option (target directory)") {
    fs::create_directory("target_dir");
    createTestFile("source_file.txt", "Hello, World!");
    runCommand("./cmake-build-release/mv.exe -t target_dir source_file.txt");
    REQUIRE(!fs::exists("source_file.txt"));
    REQUIRE(fs::exists("target_dir/source_file.txt"));
  }

  // Clean up after test
  fs::remove("source_file.txt");
  fs::remove("dest_file.txt");
  fs::remove("new_file.txt");
  fs::remove("existing_file.txt");
  removeDirectoryRecursive("target_dir");
}

// Test cases for cat command
TEST_CASE("cat command tests", "[cat]") {
  // Clean up before test
  fs::remove("test_file.txt");

  // Test 1: Basic cat (single file)
  SECTION("Basic cat (single file)") {
    createTestFile("test_file.txt", "Line 1\nLine 2\nLine 3");
    std::string output =
        runCommand("./cmake-build-release/cat.exe test_file.txt");
    REQUIRE(output == "Line 1\nLine 2\nLine 3\n");
  }

  // Test 2: cat with -E option (show ends)
  SECTION("cat with -E option (show ends)") {
    createTestFile("test_file.txt", "Line 1\nLine 2\nLine 3");
    std::string output =
        runCommand("./cmake-build-release/cat.exe -E test_file.txt");
    REQUIRE(output == "Line 1$\nLine 2$\nLine 3$\n");
  }

  // Test 3: cat with -n option (number all lines)
  SECTION("cat with -n option (number all lines)") {
    createTestFile("test_file.txt", "Line 1\nLine 2\nLine 3");
    std::string output =
        runCommand("./cmake-build-release/cat.exe -n test_file.txt");
    REQUIRE(output.find("     1\tLine 1") != std::string::npos);
    REQUIRE(output.find("     2\tLine 2") != std::string::npos);
    REQUIRE(output.find("     3\tLine 3") != std::string::npos);
  }

  // Test 4: cat with -b option (number nonblank lines)
  SECTION("cat with -b option (number nonblank lines)") {
    createTestFile("test_file.txt", "Line 1\n\nLine 3");
    std::string output =
        runCommand("./cmake-build-release/cat.exe -b test_file.txt");
    REQUIRE(output.find("     1\tLine 1") != std::string::npos);
    REQUIRE(output.find("\n") != std::string::npos);
    REQUIRE(output.find("     2\tLine 3") != std::string::npos);
  }

  // Clean up after test
  fs::remove("test_file.txt");
}

// Test cases for ls command
TEST_CASE("ls command tests", "[ls]") {
  // Clean up before test
  removeDirectoryRecursive("test_dir");
  removeDirectoryRecursive("test_dir2");
  fs::remove("test_file1.txt");
  fs::remove("test_file2.txt");
  fs::remove(".hidden_file.txt");

  // Create test directory and files
  fs::create_directory("test_dir");
  fs::create_directory("test_dir2");
  createTestFile("test_file1.txt", "Content 1");
  createTestFile("test_file2.txt", "Content 2");
  createTestFile(".hidden_file.txt", "Hidden content");
  createTestFile("test_dir/file_in_dir.txt", "Content in dir");

  // Test 1: Basic ls
  SECTION("Basic ls") {
    std::string output = runCommand("./cmake-build-release/ls.exe");
    REQUIRE(output.find("test_file1.txt") != std::string::npos);
    REQUIRE(output.find("test_file2.txt") != std::string::npos);
    REQUIRE(output.find("test_dir") != std::string::npos);
    REQUIRE(output.find("test_dir2") != std::string::npos);
    REQUIRE(output.find(".hidden_file.txt") ==
            std::string::npos);  // Hidden file should not be listed
  }

  // Test 2: ls with -a option (show all files including hidden)
  SECTION("ls with -a option (show all files including hidden)") {
    std::string output = runCommand("./cmake-build-release/ls.exe -a");
    REQUIRE(output.find("test_file1.txt") != std::string::npos);
    REQUIRE(output.find(".hidden_file.txt") !=
            std::string::npos);  // Hidden file should be listed
    REQUIRE(output.find(".") !=
            std::string::npos);  // Current directory should be listed
    REQUIRE(output.find("..") !=
            std::string::npos);  // Parent directory should be listed
  }

  // Test 3: ls with -l option (long listing)
  SECTION("ls with -l option (long listing)") {
    std::string output = runCommand("./cmake-build-release/ls.exe -l");
    REQUIRE(output.find("test_file1.txt") != std::string::npos);
    REQUIRE(output.find("test_file2.txt") != std::string::npos);
  }

  // Test 4: ls with -r option (reverse order)
  SECTION("ls with -r option (reverse order)") {
    std::string output = runCommand("./cmake-build-release/ls.exe -r");
    // The exact order may vary, but we can check that both files are present
    REQUIRE(output.find("test_file1.txt") != std::string::npos);
    REQUIRE(output.find("test_file2.txt") != std::string::npos);
  }

  // Test 5: ls with -R option (recursive)
  SECTION("ls with -R option (recursive)") {
    std::string output = runCommand("./cmake-build-release/ls.exe -R");
    REQUIRE(output.find("test_file1.txt") != std::string::npos);
    REQUIRE(output.find("test_dir") != std::string::npos);
    REQUIRE(output.find("file_in_dir.txt") != std::string::npos);
  }

  // Clean up after test
  removeDirectoryRecursive("test_dir");
  removeDirectoryRecursive("test_dir2");
  fs::remove("test_file1.txt");
  fs::remove("test_file2.txt");
  fs::remove(".hidden_file.txt");
}

// Test cases for mkdir command
TEST_CASE("mkdir command tests", "[mkdir]") {
  // Clean up before test
  removeDirectoryRecursive("test_dir");
  removeDirectoryRecursive("nested/dir/structure");

  // Test 1: Basic mkdir
  SECTION("Basic mkdir") {
    runCommand("./cmake-build-release/mkdir.exe test_dir");
    REQUIRE(fs::exists("test_dir"));
    REQUIRE(fs::is_directory("test_dir"));
  }

  // Test 2: mkdir with -p option (create parent directories)
  SECTION("mkdir with -p option (create parent directories)") {
    runCommand("./cmake-build-release/mkdir.exe -p nested/dir/structure");
    REQUIRE(fs::exists("nested/dir/structure"));
    REQUIRE(fs::is_directory("nested/dir/structure"));
  }

  // Test 3: mkdir with -v option (verbose)
  SECTION("mkdir with -v option (verbose)") {
    std::string output =
        runCommand("./cmake-build-release/mkdir.exe -v test_dir");
    REQUIRE(output.find("created directory 'test_dir'") != std::string::npos);
  }

  // Clean up after test
  removeDirectoryRecursive("test_dir");
  removeDirectoryRecursive("nested");
}
