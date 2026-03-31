/*
 * MIT License
 *
 * Copyright (c) 2024-2026 WinuxCmd contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: ffi.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */

#include "ffi.h"
#include <cstring>
#include <string>
#include <vector>
#include "Main/output_capture.h"
#include "core/dispatcher.h"

namespace {

// Helper function to copy string to C buffer
char* copy_string_to_buffer(const std::string& str, size_t* size) {
  if (str.empty()) {
    if (size) *size = 0;
    return nullptr;
  }

  char* buffer = new char[str.size() + 1];
  std::memcpy(buffer, str.data(), str.size());
  buffer[str.size()] = '\0';

  if (size) *size = str.size();
  return buffer;
}

}  // namespace

extern "C" {

int winux_execute(const char* command, const char** args, int arg_count,
                  const char* cwd, char** output, char** error,
                  size_t* output_size, size_t* error_size) {
  if (!command) {
    if (output) *output = nullptr;
    if (output_size) *output_size = 0;
    if (error) *error = nullptr;
    if (error_size) *error_size = 0;
    return 1;
  }

  if (output) *output = nullptr;
  if (output_size) *output_size = 0;
  if (error) *error = nullptr;
  if (error_size) *error_size = 0;

  try {
    // Capture output
    OutputCapture capture(65536, 16384);

    // Build argument list
    std::vector<std::string_view> arg_list;
    if (args && arg_count > 0) {
      for (int i = 0; i < arg_count; ++i) {
        if (args[i]) {
          arg_list.push_back(args[i]);
        }
      }
    }

    // Execute command directly
    CommandRegistry::dispatch(command, arg_list);

    // Finish capture
    capture.finish();

    // Copy output
    if (output) {
      *output = copy_string_to_buffer(capture.get_output(), output_size);
    }

    if (error) {
      *error = copy_string_to_buffer(capture.get_error(), error_size);
    }

    return 0;
  } catch (const std::exception& e) {
    if (error) {
      std::string error_msg = std::string("Exception: ") + e.what();
      *error = copy_string_to_buffer(error_msg, error_size);
    }
    return 1;
  } catch (...) {
    if (error) {
      *error = copy_string_to_buffer("Unknown exception", error_size);
    }
    return 1;
  }
}

void winux_free_buffer(char* buffer) {
  if (buffer) {
    delete[] buffer;
  }
}

void winux_free_commands_array(char** commands, int count) {
  if (!commands) return;

  for (int i = 0; i < count; ++i) {
    if (commands[i]) {
      delete[] commands[i];
    }
  }

  delete[] commands;
}

const char* winux_get_version() {
  return WINUXCMD_VERSION;
}

int winux_get_all_commands(char*** commands, int* count) {
  if (commands) *commands = nullptr;
  if (count) *count = 0;

  try {
    auto cmd_list = CommandRegistry::getAllCommands();
    if (cmd_list.empty()) {
      return 0;
    }

    char** cmd_array = new char*[cmd_list.size()];
    for (size_t i = 0; i < cmd_list.size(); ++i) {
      cmd_array[i] = copy_string_to_buffer(std::string(cmd_list[i].first), nullptr);
    }

    if (commands) *commands = cmd_array;
    if (count) *count = static_cast<int>(cmd_list.size());

    return 0;
  } catch (...) {
    return -1;
  }
}

}  // extern "C"