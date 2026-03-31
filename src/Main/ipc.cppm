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
 *  - File: ipc.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
export module ipc;

import std;
import utils;
import core;

export namespace ipc {

// Protocol version
constexpr int PROTOCOL_VERSION = 1;

// Message types
enum class MessageType : uint32_t { REQUEST = 1, RESPONSE = 2, CONTROL = 3 };

// Error codes
enum class ErrorCode {
  TIMEOUT,
  PIPE_BROKEN,
  INVALID_REQUEST,
  VERSION_MISMATCH,
  COMMAND_FAILED,
  DAEMON_BUSY,
  SHUTDOWN
};

// Convert ErrorCode to string
inline std::string error_code_to_string(ErrorCode code) {
  switch (code) {
    case ErrorCode::TIMEOUT:
      return "TIMEOUT";
    case ErrorCode::PIPE_BROKEN:
      return "PIPE_BROKEN";
    case ErrorCode::INVALID_REQUEST:
      return "INVALID_REQUEST";
    case ErrorCode::VERSION_MISMATCH:
      return "VERSION_MISMATCH";
    case ErrorCode::COMMAND_FAILED:
      return "COMMAND_FAILED";
    case ErrorCode::DAEMON_BUSY:
      return "DAEMON_BUSY";
    case ErrorCode::SHUTDOWN:
      return "SHUTDOWN";
    default:
      return "UNKNOWN";
  }
}

// Request message
struct Request {
  int version = PROTOCOL_VERSION;
  std::string id;
  std::string type;  // "execute" | "stream" | "list_commands"
  std::string command;
  std::vector<std::string> args;
  std::string cwd;
  std::unordered_map<std::string, std::string> env;

  struct Options {
    bool stream_output = false;
    bool interactive = false;
    int timeout = 30;
    int priority = 0;
  };
  Options options;
};

// Response message
struct Response {
  std::string id;
  int exit_code = 0;
  std::string stdout_text;
  std::string stderr_text;
  bool success = true;

  // For list_commands response
  std::vector<std::string> commands;

  struct Error {
    std::string code;
    std::string message;
  };
  std::optional<Error> error;

  bool stream_end = true;
  bool is_partial = false;
};

// Control signal
struct ControlSignal {
  std::string target_id;
  std::string signal;  // "SIGINT" | "SIGTERM" | "SIGKILL"
};

// Message header (8 bytes)
struct MessageHeader {
  uint32_t length;  // Total message length including header
  uint32_t type;    // MessageType
};

// Named pipe name generator
inline std::string get_pipe_name() {
  DWORD session_id = ::WTSGetActiveConsoleSessionId();
  return "\\\\.\\pipe\\WinuxCmd_" + std::to_string(session_id);
}

// Serialize request to JSON
inline std::string serialize_request(const Request& req) {
  nlohmann::json root;
  root["version"] = req.version;
  root["id"] = req.id;
  root["type"] = req.type;
  root["command"] = req.command;
  root["args"] = req.args;
  root["cwd"] = req.cwd;
  root["env"] = req.env;

  nlohmann::json opts;
  opts["stream_output"] = req.options.stream_output;
  opts["interactive"] = req.options.interactive;
  opts["timeout"] = req.options.timeout;
  opts["priority"] = req.options.priority;
  root["options"] = opts;

  return root.dump();
}

// Serialize response to JSON
inline std::string serialize_response(const Response& resp) {
  nlohmann::json root;
  root["id"] = resp.id;
  root["exit_code"] = resp.exit_code;
  root["stdout"] = resp.stdout_text;
  root["stderr"] = resp.stderr_text;
  root["success"] = resp.success;

  // Serialize commands if present (for list_commands response)
  if (!resp.commands.empty()) {
    root["commands"] = resp.commands;
  }

  if (resp.error) {
    nlohmann::json err;
    err["code"] = resp.error->code;
    err["message"] = resp.error->message;
    root["error"] = err;
  }

  root["stream_end"] = resp.stream_end;
  root["is_partial"] = resp.is_partial;

  return root.dump();
}

// Deserialize request from JSON
inline std::optional<Request> deserialize_request(const std::string& json_str) {
  try {
    nlohmann::json root = nlohmann::json::parse(json_str);
    if (!root.is_object()) {
      return std::nullopt;
    }

    Request req;
    req.version = root["version"].get<int>();
    req.id = root["id"].get<std::string>();
    req.type = root["type"].get<std::string>();
    req.command = root["command"].get<std::string>();

    if (root["args"].is_array()) {
      for (const auto& arg : root["args"]) {
        req.args.push_back(arg.get<std::string>());
      }
    }

    req.cwd = root["cwd"].get<std::string>();

    if (root["env"].is_object()) {
      for (auto it = root["env"].begin(); it != root["env"].end(); ++it) {
        req.env[it.key()] = it.value().get<std::string>();
      }
    }

    if (root["options"].is_object()) {
      auto opts = root["options"];
      req.options.stream_output = opts["stream_output"].get<bool>();
      req.options.interactive = opts["interactive"].get<bool>();
      req.options.timeout = opts["timeout"].get<int>();
      req.options.priority = opts["priority"].get<int>();
    }

    return req;
  } catch (...) {
    return std::nullopt;
  }
}

// Deserialize response from JSON
inline std::optional<Response> deserialize_response(
    const std::string& json_str) {
  try {
    nlohmann::json root = nlohmann::json::parse(json_str);
    if (!root.is_object()) {
      return std::nullopt;
    }

    Response resp;
    resp.id = root["id"].get<std::string>();
    resp.exit_code = root["exit_code"].get<int>();
    resp.stdout_text = root["stdout"].get<std::string>();
    resp.stderr_text = root["stderr"].get<std::string>();
    resp.success = root["success"].get<bool>();

    // Parse commands if present (for list_commands response)
    if (root.contains("commands") && root["commands"].is_array()) {
      for (const auto& cmd : root["commands"]) {
        resp.commands.push_back(cmd.get<std::string>());
      }
    }

    if (root["error"].is_object()) {
      Response::Error err;
      err.code = root["error"]["code"].get<std::string>();
      err.message = root["error"]["message"].get<std::string>();
      resp.error = err;
    }

    resp.stream_end = root["stream_end"].get<bool>();
    resp.is_partial = root["is_partial"].get<bool>();

    return resp;
  } catch (...) {
    return std::nullopt;
  }
}

// Named pipe wrapper
class NamedPipe {
 public:
  NamedPipe() = default;
  ~NamedPipe() { close(); }

  // Connect to existing pipe (client)
  bool connect(const std::string& name, int timeout_ms = 100) {
    hPipe_ = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                         OPEN_EXISTING, 0, nullptr);

    if (hPipe_ == INVALID_HANDLE_VALUE) {
      return false;
    }

    return true;
  }

  // Create and wait for connection (server)
  bool create_and_wait(const std::string& name) {
    hPipe_ =
        CreateNamedPipeA(name.c_str(), PIPE_ACCESS_DUPLEX,
                         PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                         PIPE_UNLIMITED_INSTANCES,
                         65536,  // Output buffer size
                         65536,  // Input buffer size
                         0,      // Default timeout
                         nullptr);

    if (hPipe_ == INVALID_HANDLE_VALUE) {
      return false;
    }

    BOOL result = ConnectNamedPipe(hPipe_, nullptr);
    if (!result) {
      DWORD error = GetLastError();
      if (error != ERROR_PIPE_CONNECTED) {
        CloseHandle(hPipe_);
        hPipe_ = INVALID_HANDLE_VALUE;
        return false;
      }
    }
    return true;
  }

  // Write message
  bool write_message(MessageType type, const std::string& content) {
    if (hPipe_ == INVALID_HANDLE_VALUE) {
      return false;
    }

    MessageHeader header;
    header.length = sizeof(header) + content.size();
    header.type = static_cast<uint32_t>(type);

    DWORD written = 0;
    if (!WriteFile(hPipe_, &header, sizeof(header), &written, nullptr)) {
      return false;
    }

    if (!WriteFile(hPipe_, content.data(), content.size(), &written, nullptr)) {
      return false;
    }

    return true;
  }

  // Read message
  std::optional<std::pair<MessageType, std::string>> read_message() {
    if (hPipe_ == INVALID_HANDLE_VALUE) {
      return std::nullopt;
    }

    MessageHeader header;
    DWORD read = 0;
    if (!ReadFile(hPipe_, &header, sizeof(header), &read, nullptr)) {
      return std::nullopt;
    }

    if (read != sizeof(header)) {
      return std::nullopt;
    }

    std::string content(header.length - sizeof(header), '\0');
    if (!ReadFile(hPipe_, content.data(), content.size(), &read, nullptr)) {
      return std::nullopt;
    }

    if (read != content.size()) {
      return std::nullopt;
    }

    MessageType type = static_cast<MessageType>(header.type);
    return {{type, content}};
  }

  // Close pipe
  void close() {
    if (hPipe_ != INVALID_HANDLE_VALUE) {
      CloseHandle(hPipe_);
      hPipe_ = INVALID_HANDLE_VALUE;
    }
  }

  bool is_valid() const { return hPipe_ != INVALID_HANDLE_VALUE; }

 private:
  HANDLE hPipe_ = INVALID_HANDLE_VALUE;
};

// Generate unique request ID
inline std::string generate_request_id() {
  static std::atomic<uint64_t> counter{0};
  auto now = std::chrono::system_clock::now();
  auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now.time_since_epoch())
                       .count();
  return std::to_string(timestamp) + "_" + std::to_string(counter.fetch_add(1));
}

}  // namespace ipc
