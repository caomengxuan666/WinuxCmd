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
 *  - File: temp_dir.h
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#pragma once
#include <windows.h>

#include <filesystem>
#include <fstream>
#include <string>

/**
 * @brief Temporary directory management for tests
 *
 * RAII wrapper for temporary directories that automatically
 * creates a unique temporary directory and cleans it up
 * when destroyed. Provides convenient methods for file I/O
 * within the temporary directory.
 */
struct TempDir {
  std::filesystem::path path;  ///< Path to the temporary directory

  /**
   * @brief Create a new temporary directory
   *
   * Uses Windows API to generate a unique temporary directory
   * name and creates the directory structure.
   */
  TempDir() {
    wchar_t base[MAX_PATH];
    // Get system temporary directory path
    GetTempPathW(MAX_PATH, base);

    wchar_t name[MAX_PATH];
    // Generate unique temporary file name (we'll use it as directory name)
    GetTempFileNameW(base, L"wct", 0, name);
    DeleteFileW(name);  // Remove the temporary file

    path = name;
    std::filesystem::create_directory(path);  // Create directory instead
  }

  /**
   * @brief Automatically clean up temporary directory
   *
   * Recursively removes all files and subdirectories
   * in the temporary directory when object is destroyed.
   */
  ~TempDir() {
    std::error_code ec;
    std::filesystem::remove_all(path, ec);  // Ignore errors during cleanup
  }

  /**
   * @brief Get wide string path to temporary directory
   *
   * @return std::wstring Wide string representation of directory path
   */
  [[nodiscard]]
  std::wstring wpath() const {
    return path.wstring();
  }

  /**
   * @brief Write text content to a file in the temporary directory
   *
   * Creates parent directories as needed and writes the content
   * in binary mode to preserve exact bytes.
   *
   * @param rel Relative path within temporary directory
   * @param content Text content to write
   */
  void write(const std::string &rel, const std::string &content) const {
    auto p = path / rel;
    std::filesystem::create_directories(p.parent_path());

    std::ofstream ofs(p, std::ios::binary);
    ofs.write(content.data(), content.size());
  }

  /**
   * @brief Write binary data to a file in the temporary directory
   *
   * Creates parent directories as needed and writes raw bytes.
   *
   * @param rel Relative path within temporary directory
   * @param data Vector of bytes to write
   */
  void write_bytes(const std::string &rel,
                   const std::vector<char> &data) const {
    auto p = path / rel;
    std::filesystem::create_directories(p.parent_path());

    std::ofstream ofs(p, std::ios::binary);
    ofs.write(data.data(), data.size());
  }

  /**
   * @brief Read text content from a file in the temporary directory
   *
   * Reads the entire file content as a string using binary mode
   * to preserve exact bytes.
   *
   * @param rel Relative path within temporary directory
   * @return std::string File content
   */
  std::string read(const std::string &rel) const {
    auto p = path / rel;
    std::ifstream ifs(p, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       std::istreambuf_iterator<char>());
  }

  /**
   * @brief Create a subdirectory within the temporary directory
   *
   * Creates a directory at the specified relative path, creating
   * parent directories as needed.
   *
   * @param rel Relative path of the directory to create
   */
  void mkdir(const std::string &rel) const {
    auto p = path / rel;
    std::filesystem::create_directories(p);
  }
};
