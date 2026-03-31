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
 *  - File: ipc_client.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
module;

#include <chrono>
#include <string>
#include <vector>
#include <optional>

export module ipc_client;

import ipc;
import utils;


#define IPC_LOG(msg) do { if (enable_logging_) { safePrintLn(msg); } } while(0)

import std;
import utils;
import ipc;

export namespace ipc_client {

// Global log level control
inline bool g_enable_ipc_logging = false;  // Disabled by default for performance

// Client configuration
struct ClientConfig {
  int connect_timeout_ms = 100;  // Connect timeout
  int request_timeout_sec = 30;  // Request timeout
  bool auto_retry = false;       // Auto retry on failure
  int max_retries = 3;           // Maximum retry attempts
  bool enable_logging = false;   // Enable IPC logging
};

// IPC client
class IpcClient {
 public:
  IpcClient(const ClientConfig& config = ClientConfig{})
      : config_(config), enable_logging_(config.enable_logging) {}

  // Check if daemon is available (fast check)
  static bool is_daemon_available() {
    std::string pipe_name = ipc::get_pipe_name();

    // Use WaitNamedPipe instead of CreateFile to avoid triggering connection
    // WaitNamedPipe checks if a pipe instance is available without connecting
    BOOL result = WaitNamedPipeA(pipe_name.c_str(), 0);  // 0 = immediate return

    return result != FALSE;
  }

  // Execute command via daemon (fast fail)
    std::optional<ipc::Response> execute_via_daemon(const ipc::Request& req) {
      auto start_time = std::chrono::high_resolution_clock::now();

      // Log: Attempting to connect to daemon
      // safePrintLn(L"[IPC Client] Checking daemon availability...");

      // Quick check if daemon is available
      bool available = is_daemon_available();
      auto check_time = std::chrono::high_resolution_clock::now();
      auto check_duration = std::chrono::duration_cast<std::chrono::milliseconds>(check_time - start_time).count();

      if (!available) {
        // Log: Daemon not available
        IPC_LOG(L"[IPC Client] Daemon not available");
        return std::nullopt;
      }

      // Log: Daemon available, attempting connection
      IPC_LOG(L"[IPC Client] Daemon available, attempting connection...");

      // Try to connect
      ipc::NamedPipe pipe;
      bool connected = pipe.connect(ipc::get_pipe_name(), config_.connect_timeout_ms);
      auto connect_time = std::chrono::high_resolution_clock::now();
      auto connect_duration = std::chrono::duration_cast<std::chrono::milliseconds>(connect_time - start_time).count();

      if (!connected) {
        // Log: Connection failed
        IPC_LOG(L"[IPC Client] Connection failed after " + std::to_wstring(connect_duration) + L"ms");
        return std::nullopt;
      }

      // Log: Connected successfully
      IPC_LOG(L"[IPC Client] Connected successfully in " + std::to_wstring(connect_duration) + L"ms");

      // Send request
      std::string serialized = ipc::serialize_request(req);
      auto send_time = std::chrono::high_resolution_clock::now();
      if (!pipe.write_message(ipc::MessageType::REQUEST, serialized)) {
        // Log: Failed to send request
        IPC_LOG(L"[IPC Client] Failed to send request");
        return std::nullopt;
      }

      // Log: Request sent, waiting for response
        IPC_LOG(L"[IPC Client] Request sent, waiting for response...");

      // Wait for response with timeout
      auto response = read_response_with_timeout(pipe);
      auto response_time = std::chrono::high_resolution_clock::now();
      auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(response_time - start_time).count();

      if (!response) {
        // Log: Response timeout or failure
        IPC_LOG(L"[IPC Client] Response timeout or failure after " + std::to_wstring(total_duration) + L"ms");
        return std::nullopt;
      }

      // Log: Response received with details
      IPC_LOG(L"[IPC Client] Response received in " + std::to_wstring(total_duration) + L"ms");
      IPC_LOG(L"[IPC Client] Response success: " + std::to_wstring(response->success ? 1 : 0));
      IPC_LOG(L"[IPC Client] Response exit code: " + std::to_wstring(response->exit_code));
      IPC_LOG(L"[IPC Client] Response stdout size: " + std::to_wstring(response->stdout_text.size()) + L" bytes");
      IPC_LOG(L"[IPC Client] Response stderr size: " + std::to_wstring(response->stderr_text.size()) + L" bytes");
      if (!response->stdout_text.empty()) {
        IPC_LOG(L"[IPC Client] Response stdout preview: " + std::wstring(response->stdout_text.begin(), response->stdout_text.begin() + (std::min)(size_t(50), response->stdout_text.size())));
      }
      return response;
    }
  // Execute command with retries
  std::optional<ipc::Response> execute_with_retry(const ipc::Request& req) {
    std::optional<ipc::Response> result;
    int attempts = 0;

    while (attempts <= config_.max_retries) {
      result = execute_via_daemon(req);
      if (result) {
        break;
      }

      attempts++;
      if (attempts <= config_.max_retries && config_.auto_retry) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }

    return result;
  }

  // Send stream request (for long-running commands)
  bool send_stream_request(const ipc::Request& req,
                           std::function<void(const ipc::Response&)> callback) {
    if (!is_daemon_available()) {
      return false;
    }

    ipc::NamedPipe pipe;
    if (!pipe.connect(ipc::get_pipe_name(), config_.connect_timeout_ms)) {
      return false;
    }

    // Send request
    std::string serialized = ipc::serialize_request(req);
    if (!pipe.write_message(ipc::MessageType::REQUEST, serialized)) {
      return false;
    }

    // Receive stream of responses
    while (true) {
      auto response = read_response_with_timeout(pipe);
      if (!response) {
        return false;
      }

      // Call callback
      callback(*response);

      // Check if stream ended
      if (response->stream_end) {
        break;
      }
    }

    return true;
  }

  // Send control signal
  bool send_control_signal(const ipc::ControlSignal& signal) {
    if (!is_daemon_available()) {
      return false;
    }

    ipc::NamedPipe pipe;
    if (!pipe.connect(ipc::get_pipe_name(), config_.connect_timeout_ms)) {
      return false;
    }

    // Serialize control signal as a special request
    nlohmann::json root;
    root["id"] = ipc::generate_request_id();
    root["type"] = "control";
    root["target_id"] = signal.target_id;
    root["signal"] = signal.signal;

    std::string serialized = root.dump();
    if (!pipe.write_message(ipc::MessageType::CONTROL, serialized)) {
      return false;
    }

    return true;
  }

  // Get configuration
  const ClientConfig& get_config() const { return config_; }

  // Set configuration
  void set_config(const ClientConfig& config) { config_ = config; }

 private:
  ClientConfig config_;
  bool enable_logging_;  // Control logging for this instance

  // Read response with timeout
  std::optional<ipc::Response> read_response_with_timeout(
      ipc::NamedPipe& pipe) {
    auto deadline = std::chrono::steady_clock::now() +
                    std::chrono::seconds(config_.request_timeout_sec);

    while (std::chrono::steady_clock::now() < deadline) {
      auto msg_result = pipe.read_message();
      if (!msg_result) {
        // Wait a bit and retry
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
      }

      if (msg_result->first != ipc::MessageType::RESPONSE) {
        return std::nullopt;
      }

      // Deserialize response
      auto resp = ipc::deserialize_response(msg_result->second);
      if (!resp) {
        return std::nullopt;
      }

      return resp;
    }

    // Timeout
    return std::nullopt;
  }
};

// Convenience function: create request and execute
inline std::optional<ipc::Response> quick_execute(
    const std::string& command, const std::vector<std::string>& args = {},
    const std::string& cwd = "", const ClientConfig& config = ClientConfig{}) {
  ipc::Request req;
  req.id = ipc::generate_request_id();
  req.type = "execute";
  req.command = command;
  req.args = args;
  req.cwd = cwd;

  IpcClient client(config);
  return client.execute_via_daemon(req);
}

}  // namespace ipc_client

#undef IPC_LOG
