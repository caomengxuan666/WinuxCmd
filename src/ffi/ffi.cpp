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
 *  - File: ffi.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */

#ifndef WINUX_FFI_EXPORTS
#define WINUX_FFI_EXPORTS
#endif

#include "ffi.h"
import std;
import utils;
import core;
import ipc;
import ipc_client;
import daemon;
import version;

namespace {
// Global daemon handle (optional, for manual management)
std::unique_ptr<daemon::DaemonManager> g_daemon;
bool g_daemon_initialized = false;

// Helper function to copy string to C buffer
char* copy_string_to_buffer(const std::string& str, size_t* size) {
  if (str.empty()) {
    if (size) *size = 0;
    return nullptr;
  }

  char* buffer = new char[str.size()];
  std::memcpy(buffer, str.data(), str.size());

  if (size) *size = str.size();
  return buffer;
}

}  // namespace

extern "C" {

int winux_init_daemon(void) {
  try {
    if (g_daemon_initialized) {
      return 0;  // Already initialized
    }

    g_daemon = std::make_unique<daemon::DaemonManager>();
    g_daemon_initialized = true;

    return 0;
  } catch (const std::exception& e) {
    return -1;
  } catch (...) {
    return -1;
  }
}

int winux_execute(const char* command, const char** args, int arg_count,
                  const char* cwd, char** output, char** error,
                  size_t* output_size, size_t* error_size) {
  // Initialize output parameters
  if (output) *output = nullptr;
  if (error) *error = nullptr;
  if (output_size) *output_size = 0;
  if (error_size) *error_size = 0;

  // Validate inputs
  if (!command || command[0] == '\0') {
    return -1;
  }

  try {
    // Build request
    ipc::Request req;
    req.id = ipc::generate_request_id();
    req.type = "execute";
    req.command = command;

    // Add arguments
    if (args && arg_count > 0) {
      req.args.reserve(arg_count);
      for (int i = 0; i < arg_count; ++i) {
        if (args[i]) {
          req.args.push_back(args[i]);
        }
      }
    }

    // Set working directory
    if (cwd && cwd[0] != '\0') {
      req.cwd = cwd;
    }

    // Check if daemon is available
    if (!ipc_client::IpcClient::is_daemon_available()) {
      // Try to start daemon automatically
      if (!g_daemon_initialized) {
        if (winux_init_daemon() != 0) {
          if (error)
            *error = copy_string_to_buffer("Failed to initialize daemon",
                                           error_size);
          return -1;
        }
      }
    }

    // Execute via daemon
    ipc_client::IpcClient client;
    auto response = client.execute_via_daemon(req);

    if (!response) {
      if (error)
        *error = copy_string_to_buffer("Daemon execution failed or timed out",
                                       error_size);
      return -1;
    }

    // Copy output
    if (output && !response->stdout_text.empty()) {
      *output = copy_string_to_buffer(response->stdout_text, output_size);
    }

    // Copy error
    if (error && !response->stderr_text.empty()) {
      *error = copy_string_to_buffer(response->stderr_text, error_size);
    }

    return response->exit_code;

  } catch (const std::exception& e) {
    if (error) *error = copy_string_to_buffer(e.what(), error_size);
    return -1;
  } catch (...) {
    if (error)
      *error = copy_string_to_buffer("Unknown error occurred", error_size);
    return -1;
  }
}

void winux_free_buffer(char* buffer) {
  if (buffer) {
    delete[] buffer;
  }
}

int winux_is_daemon_available(void) {
  return ipc_client::IpcClient::is_daemon_available() ? 1 : 0;
}

const char* winux_get_version(void) { return WinuxCmd::VERSION_STRING; }

int winux_get_protocol_version(void) { return ipc::PROTOCOL_VERSION; }

}  // extern "C"