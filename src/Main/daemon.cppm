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
 *  - File: daemon.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
module;
#include <windows.h>

export module daemon;

import std;
import utils;
import core;
import ipc;
import ipc_client;

export namespace daemon {

// Compile-time logging configuration
// Set to 0 to completely remove all logging code from release builds
#ifndef DAEMON_LOG_LEVEL
#ifdef NDEBUG
#define DAEMON_LOG_LEVEL 1  // Release: errors only
#else
#define DAEMON_LOG_LEVEL 4  // Debug: all logs
#endif
#endif

// Logging macros based on compile-time log level
#define DAEMON_LOG(level, msg)         \
  do {                                 \
    if (DAEMON_LOG_LEVEL >= (level)) { \
      safePrintLn(msg);                \
    }                                  \
  } while (0)

#define DAEMON_LOG_ERROR(msg) DAEMON_LOG(1, msg)
#define DAEMON_LOG_WARN(msg) DAEMON_LOG(2, msg)
#define DAEMON_LOG_INFO(msg) DAEMON_LOG(3, msg)
#define DAEMON_LOG_DEBUG(msg) DAEMON_LOG(4, msg)

// Output capture helper using Windows pipes
class OutputCapture {
 public:
  explicit OutputCapture(size_t pipe_buffer_size = 65536,
                         size_t read_buffer_size = 16384)
      : pipe_buffer_size_(pipe_buffer_size),
        read_buffer_size_(read_buffer_size) {
    // Create pipes for stdout and stderr
    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;

    // Create stdout pipe
    if (!CreatePipe(&stdout_read_, &stdout_write_, &sa,
                    static_cast<DWORD>(pipe_buffer_size_))) {
      DWORD err = GetLastError();
      HANDLE debug_handle = GetStdHandle(STD_OUTPUT_HANDLE);
      std::wstring msg = L"[Daemon ERROR] CreatePipe stdout failed: " +
                         std::to_wstring(err) + L"\n";
      WriteConsoleW(debug_handle, msg.c_str(), static_cast<DWORD>(msg.size()),
                    nullptr, nullptr);
      return;
    }
    SetHandleInformation(stdout_read_, HANDLE_FLAG_INHERIT, 0);

    // Create stderr pipe
    if (!CreatePipe(&stderr_read_, &stderr_write_, &sa,
                    static_cast<DWORD>(pipe_buffer_size_))) {
      CloseHandle(stdout_read_);
      CloseHandle(stdout_write_);
      DWORD err = GetLastError();
      HANDLE debug_handle = GetStdHandle(STD_OUTPUT_HANDLE);
      std::wstring msg = L"[Daemon ERROR] CreatePipe stderr failed: " +
                         std::to_wstring(err) + L"\n";
      WriteConsoleW(debug_handle, msg.c_str(), static_cast<DWORD>(msg.size()),
                    nullptr, nullptr);
      return;
    }
    SetHandleInformation(stderr_read_, HANDLE_FLAG_INHERIT, 0);

    // Hook console.cppm output functions
    set_output_capture_handles(stdout_write_, stderr_write_, true);

    active_ = true;
  }

  ~OutputCapture() {
    if (!active_) return;

    // Close any remaining handles (should be cleaned up by finish())
    if (stdout_read_) CloseHandle(stdout_read_);
    if (stderr_read_) CloseHandle(stderr_read_);
    if (stdout_write_) CloseHandle(stdout_write_);
    if (stderr_write_) CloseHandle(stderr_write_);
  }

  std::string get_stdout() const { return stdout_content_; }
  std::string get_stderr() const { return stderr_content_; }
  bool is_active() const { return active_; }

  void finish() {
    if (!active_) return;

    // Unhook console.cppm output functions
    set_output_capture_handles(nullptr, nullptr, false);

    // Close write handles to signal EOF to readers
    if (stdout_write_) {
      CloseHandle(stdout_write_);
      stdout_write_ = nullptr;
    }
    if (stderr_write_) {
      CloseHandle(stderr_write_);
      stderr_write_ = nullptr;
    }

    // Read all available data from pipes with optimized memory allocation
    if (stdout_read_) {
      // Pre-reserve space to reduce allocations
      stdout_content_.reserve(4096);  // Initial 4KB reservation

      std::vector<char> buffer(read_buffer_size_);
      DWORD bytes_read;

      while (true) {
        if (!ReadFile(stdout_read_, buffer.data(),
                      static_cast<DWORD>(buffer.size() - 1), &bytes_read,
                      nullptr) ||
            bytes_read == 0) {
          break;
        }
        // Optimize append with explicit size hint
        stdout_content_.append(buffer.data(), bytes_read);
      }

      // Shrink to fit if we over-allocated significantly
      if (stdout_content_.capacity() > stdout_content_.size() * 2) {
        stdout_content_.shrink_to_fit();
      }

      CloseHandle(stdout_read_);
      stdout_read_ = nullptr;
    }

    if (stderr_read_) {
      stderr_content_.reserve(4096);  // Initial 4KB reservation

      std::vector<char> buffer(read_buffer_size_);
      DWORD bytes_read;

      while (true) {
        if (!ReadFile(stderr_read_, buffer.data(),
                      static_cast<DWORD>(buffer.size() - 1), &bytes_read,
                      nullptr) ||
            bytes_read == 0) {
          break;
        }
        stderr_content_.append(buffer.data(), bytes_read);
      }

      if (stderr_content_.capacity() > stderr_content_.size() * 2) {
        stderr_content_.shrink_to_fit();
      }

      CloseHandle(stderr_read_);
      stderr_read_ = nullptr;
    }
  }

 private:
  HANDLE stdout_read_ = nullptr;
  HANDLE stdout_write_ = nullptr;
  HANDLE stderr_read_ = nullptr;
  HANDLE stderr_write_ = nullptr;
  std::string stdout_content_;
  std::string stderr_content_;
  bool active_ = false;
  size_t pipe_buffer_size_;
  size_t read_buffer_size_;
};

// Daemon configuration
struct DaemonConfig {
  int idle_timeout = 300;    // 5 minutes idle timeout
  int max_concurrent = 10;   // Maximum concurrent requests
  int request_timeout = 30;  // Request timeout in seconds

  // Logging levels: 0=none, 1=errors only, 2=warnings, 3=info, 4=debug
  int log_level = 1;  // Default: errors only

  // Buffer sizes
  size_t pipe_buffer_size = 65536;  // 64KB pipe buffer
  size_t read_buffer_size = 16384;  // 16KB read buffer

  // Performance tuning
  int read_timeout_ms = 100;             // Read timeout in milliseconds
  bool enable_performance_stats = true;  // Enable performance tracking
};

// Daemon statistics (read-only, for external access)
struct DaemonStatsSnapshot {
  uint64_t total_requests{0};
  uint64_t successful_requests{0};
  uint64_t failed_requests{0};
  uint64_t bytes_captured{0};
  uint64_t average_response_time_us{0};
  std::chrono::system_clock::time_point last_activity;
};

// Daemon statistics (internal, with atomic counters)
struct DaemonStats {
  std::atomic<uint64_t> total_requests{0};
  std::atomic<uint64_t> successful_requests{0};
  std::atomic<uint64_t> failed_requests{0};
  std::atomic<uint64_t> bytes_captured{0};  // Total bytes captured
  std::atomic<uint64_t> average_response_time_us{
      0};  // Average response time in microseconds
  std::chrono::system_clock::time_point last_activity;

  // Delete copy constructor and copy assignment
  DaemonStats(const DaemonStats&) = delete;
  DaemonStats& operator=(const DaemonStats&) = delete;

  // Default constructor
  DaemonStats() = default;

  // Move constructor
  DaemonStats(DaemonStats&&) noexcept = default;

  // Move assignment
  DaemonStats& operator=(DaemonStats&&) noexcept = default;

  // Get snapshot (thread-safe)
  DaemonStatsSnapshot get_snapshot() const {
    DaemonStatsSnapshot snapshot;
    snapshot.total_requests = total_requests.load();
    snapshot.successful_requests = successful_requests.load();
    snapshot.failed_requests = failed_requests.load();
    snapshot.bytes_captured = bytes_captured.load();
    snapshot.average_response_time_us = average_response_time_us.load();
    snapshot.last_activity = last_activity;
    return snapshot;
  }
};

// Active command tracking
struct ActiveCommand {
  std::string id;
  std::string command;
  std::chrono::system_clock::time_point start_time;
  HANDLE process_handle = nullptr;
};

// Perform warmup by executing common commands
inline std::optional<bool> perform_warmup_commands() {
  DAEMON_LOG_INFO(L"[Daemon Warmup] Executing warmup commands...");

  // Common commands to warm up
  std::vector<std::string> warmup_commands = {
      "pwd",       // Current directory
      "whoami",    // Current user
      "hostname",  // Host name
      "date"       // Current date/time
  };

  // Use CommandRegistry directly for warmup (avoiding IPC loopback)
  bool all_success = true;

  for (const auto& cmd : warmup_commands) {
    std::vector<std::string_view> args;
    try {
      // Capture output to prevent printing to console
      OutputCapture capture(65536, 16384);
      CommandRegistry::dispatch(cmd, args);
      capture.finish();
    } catch (const std::exception& e) {
      all_success = false;
    } catch (...) {
      all_success = false;
    }
  }

  DAEMON_LOG_INFO(L"[Daemon Warmup] Warmup completed");
  return all_success;
}

// Daemon manager
class DaemonManager {
 public:
  DaemonManager(const DaemonConfig& config = DaemonConfig{})
      : config_(config) {}

  // Start daemon mode
  int run() {
    pipe_name_ = ipc::get_pipe_name();
    stats_.last_activity = std::chrono::system_clock::now();

    safePrintLn(L"WinuxCmd daemon starting...");
    safePrintLn(L"Pipe name: " + utf8_to_wstring(pipe_name_));

    // Perform warmup to register all commands
    perform_warmup_commands();

    while (running_) {
      // Check idle timeout
      check_idle_timeout();

      // Wait for client connection
      ipc::NamedPipe pipe;
      if (!pipe.create_and_wait(pipe_name_)) {
        safePrintLn(L"Failed to create named pipe");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }

      // Handle client request
      handle_client(pipe);

      // Update last activity
      stats_.last_activity = std::chrono::system_clock::now();
    }

    safePrintLn(L"WinuxCmd daemon shutting down...");
    return 0;
  }

  // Stop daemon
  void stop() { running_ = false; }

  // Get statistics (returns a snapshot)
  DaemonStatsSnapshot get_stats() const { return stats_.get_snapshot(); }

 private:
  DaemonConfig config_;
  DaemonStats stats_;
  std::string pipe_name_;
  std::atomic<bool> running_{true};
  std::mutex active_commands_mutex_;
  std::unordered_map<std::string, ActiveCommand> active_commands_;

  // Check idle timeout
  void check_idle_timeout() {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                       now - stats_.last_activity)
                       .count();

    if (elapsed > config_.idle_timeout) {
      safePrintLn(L"Daemon idle timeout, shutting down...");
      running_ = false;
    }
  }

  // Handle client request
  void handle_client(ipc::NamedPipe& pipe) {
    // Read request
    auto msg_result = pipe.read_message();
    if (!msg_result || msg_result->first != ipc::MessageType::REQUEST) {
      return;
    }

    stats_.total_requests.fetch_add(1);

    // Deserialize request
    auto req_opt = ipc::deserialize_request(msg_result->second);
    if (!req_opt) {
      ipc::Response resp;
      resp.id = "unknown";
      resp.success = false;
      resp.error = {"INVALID_REQUEST", "Failed to deserialize request"};
      pipe.write_message(ipc::MessageType::RESPONSE,
                         ipc::serialize_response(resp));
      stats_.failed_requests.fetch_add(1);
      return;
    }

    auto& req = *req_opt;

    // Check version compatibility
    if (req.version != ipc::PROTOCOL_VERSION) {
      ipc::Response resp;
      resp.id = req.id;
      resp.success = false;
      resp.error = {"VERSION_MISMATCH",
                    "Client version " + std::to_string(req.version) +
                        " is not compatible with daemon version " +
                        std::to_string(ipc::PROTOCOL_VERSION)};
      pipe.write_message(ipc::MessageType::RESPONSE,
                         ipc::serialize_response(resp));
      stats_.failed_requests.fetch_add(1);
      return;
    }

    // Execute command
    auto response = execute_command(req);

    // Send response
    pipe.write_message(ipc::MessageType::RESPONSE,
                       ipc::serialize_response(response));

    if (response.success) {
      stats_.successful_requests.fetch_add(1);
    } else {
      stats_.failed_requests.fetch_add(1);
    }
  }

  // Execute command
  ipc::Response execute_command(const ipc::Request& req) {
    auto start_time = std::chrono::high_resolution_clock::now();

    ipc::Response resp;
    resp.id = req.id;
    // Handle list_commands request
    if (req.type == "list_commands") {
      DAEMON_LOG_DEBUG(L"[Daemon] Listing all commands");
      auto all_commands = CommandRegistry::getAllCommands();
      for (const auto& [name, _] : all_commands) {
        resp.commands.push_back(std::string(name));
      }

      resp.success = true;

      return resp;
    }

    // Set working directory

    if (!req.cwd.empty()) {
      std::error_code ec;

      std::filesystem::current_path(req.cwd, ec);

      if (ec) {
        resp.success = false;

        resp.error = {"COMMAND_FAILED",

                      "Failed to set working directory: " + ec.message()};

        DAEMON_LOG_ERROR(L"[Daemon] Failed to set working directory: " +
                         utf8_to_wstring(ec.message()));

        return resp;
      }
    }

    // Prepare arguments

    std::vector<std::string_view> args;

    args.reserve(req.args.size());

    for (const auto& arg : req.args) {
      args.push_back(arg);
    }

    // Capture output

    OutputCapture capture(config_.pipe_buffer_size, config_.read_buffer_size);

    if (!capture.is_active()) {
      resp.success = false;

      resp.error = {"CAPTURE_FAILED", "Failed to initialize output capture"};

      DAEMON_LOG_ERROR(L"[Daemon] Failed to initialize output capture");

      return resp;
    }

    // Execute command using dispatcher

    DAEMON_LOG_DEBUG(L"[Daemon] Executing command: " +
                     utf8_to_wstring(req.command));

    int exit_code = CommandRegistry::dispatch(req.command, args);

    // Finish capture and get output

    capture.finish();

    resp.stdout_text = capture.get_stdout();

    resp.stderr_text = capture.get_stderr();

    // Set response

    resp.exit_code = exit_code;

    resp.success = (exit_code == 0);

    if (!resp.success) {
      resp.error = {"COMMAND_FAILED", "Command exited with non-zero code: " +

                                          std::to_string(exit_code)};

      DAEMON_LOG_WARN(L"[Daemon] Command '" + utf8_to_wstring(req.command) +
                      L"' failed with exit code: " +
                      std::to_wstring(exit_code));

    } else {
      DAEMON_LOG_DEBUG(L"[Daemon] Command '" + utf8_to_wstring(req.command) +
                       L"' completed successfully, output size: " +
                       std::to_wstring(resp.stdout_text.size()) + L" bytes");
    }

    // Update performance statistics

    if (config_.enable_performance_stats) {
      auto end_time = std::chrono::high_resolution_clock::now();

      auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(
                             end_time - start_time)
                             .count();

      stats_.total_requests.fetch_add(1);

      stats_.bytes_captured.fetch_add(resp.stdout_text.size() +
                                      resp.stderr_text.size());

      // Update average response time using exponential moving average

      uint64_t current_avg = stats_.average_response_time_us.load();

      uint64_t new_avg =
          (current_avg * 9 + static_cast<uint64_t>(duration_us)) /
          10;  // 0.9 weight on old value

      stats_.average_response_time_us.store(new_avg);
    }

    return resp;
  }
};

// Global daemon instance
inline std::unique_ptr<DaemonManager> g_daemon;

// Start daemon mode
inline int start_daemon(const DaemonConfig& config = DaemonConfig{}) {
  g_daemon = std::make_unique<DaemonManager>(config);
  return g_daemon->run();
}

// Stop daemon
inline void stop_daemon() {
  if (g_daemon) {
    g_daemon->stop();
  }
}

// Check if daemon is running (by checking if pipe exists)
inline bool is_daemon_running() {
  std::string pipe_name = ipc::get_pipe_name();
  HANDLE hPipe = CreateFileA(pipe_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                             nullptr, OPEN_EXISTING, 0, nullptr);

  if (hPipe == INVALID_HANDLE_VALUE) {
    return false;
  }

  CloseHandle(hPipe);
  return true;
}

// Warmup daemon - start daemon and preload common commands
inline std::optional<bool> warmup_daemon(
    const DaemonConfig& config = DaemonConfig{}) {
  DAEMON_LOG_INFO(L"[Daemon Warmup] Starting warmup process...");

  // Check if daemon is already running
  if (is_daemon_running()) {
    DAEMON_LOG_INFO(L"[Daemon Warmup] Daemon already running");
    // Daemon exists, just warm up with commands
    return perform_warmup_commands();
  }

  // Daemon not running, start it
  DAEMON_LOG_INFO(L"[Daemon Warmup] Starting daemon...");
  std::thread daemon_thread([&config]() { daemon::start_daemon(config); });
  daemon_thread.detach();

  // Wait for daemon to be ready
  int retries = 50;  // 5 seconds with 100ms intervals
  while (retries-- > 0) {
    if (is_daemon_running()) {
      DAEMON_LOG_INFO(L"[Daemon Warmup] Daemon started successfully");
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  if (!is_daemon_running()) {
    DAEMON_LOG_ERROR(L"[Daemon Warmup] Failed to start daemon");
    return std::nullopt;
  }

  // Give daemon a moment to fully initialize
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // Perform warmup with common commands
  return perform_warmup_commands();
}

}  // namespace daemon
