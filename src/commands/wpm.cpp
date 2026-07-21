/*
 *  Copyright (c) 2026 [caomengxuan666]
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
 *  - File: wpm.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Winux Package Manager internal command.
/// @Version: 0.2.0
/// @License: MIT

#include <bcrypt.h>
#include <winhttp.h>

#include "core/command_macros.h"
#include "pch/pch.h"

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "winhttp.lib")

import std;
import core;
import utils;
import version;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr WPM_OPTIONS = std::array{
    OPTION("-r", "--root", "manage a specific WinuxCmd root", STRING_TYPE),
    OPTION("-s", "--source", "use a specific index source", STRING_TYPE),
    OPTION("-f", "--force", "overwrite existing files when safe"),
    OPTION("-n", "--dry-run", "show planned changes without writing"),
    OPTION("-v", "--verbose", "print detailed progress")};

namespace wpm {
namespace fs = std::filesystem;

constexpr std::string_view kVersion = "0.2.0";
constexpr std::string_view kInternalToolName = "wpm";
constexpr std::string_view kBuiltinIndex = R"json(
{
  "schema": 1,
  "name": "official",
  "version": "builtin-2026.07.22",
  "updated": "2026-07-22",
  "sources": [
    {
      "name": "official-github-raw",
      "region": "global",
      "priority": 10,
      "index_urls": [
        "https://raw.githubusercontent.com/unixwin/WinuxCmd/main/wpm-source/index.json"
      ]
    },
    {
      "name": "official-github-release",
      "region": "global",
      "priority": 20,
      "index_urls": [
        "https://github.com/unixwin/WinuxCmd/releases/latest/download/wpm-index.json"
      ]
    },
    {
      "name": "official-cn",
      "region": "cn",
      "priority": 30,
      "index_urls": []
    }
  ],
  "packages": [
    {
      "name": "winuxcmd",
      "version": "0.12.4",
      "description": "WinuxCmd core command set",
      "kind": "core",
      "artifacts": {
        "windows-x64": {
          "type": "zip",
          "sha256": "",
          "urls": [],
          "files": [
            { "from": "winuxcmd.exe", "to": "winuxcmd.exe" }
          ]
        },
        "windows-arm64": {
          "type": "zip",
          "sha256": "",
          "urls": [],
          "files": [
            { "from": "winuxcmd.exe", "to": "winuxcmd.exe" }
          ]
        }
      }
    },
    {
      "name": "jq",
      "version": "",
      "description": "External jq package slot. Fill this from a refreshed index.",
      "kind": "external",
      "artifacts": {}
    },
    {
      "name": "ncat",
      "version": "",
      "description": "External ncat/netcat package slot. Fill this from a refreshed index.",
      "kind": "external",
      "artifacts": {}
    },
    {
      "name": "7zip",
      "version": "",
      "description": "External 7-Zip package slot. Fill this from a refreshed index.",
      "kind": "external",
      "artifacts": {}
    },
    {
      "name": "zstd",
      "version": "",
      "description": "External zstd package slot. Fill this from a refreshed index.",
      "kind": "external",
      "artifacts": {}
    }
  ]
}
)json";

struct HttpResult {
  bool ok = false;
  std::vector<std::byte> data;
  DWORD status = 0;
  std::string error;
};

struct Options {
  fs::path root;
  std::string source;
  bool force = false;
  bool dry_run = false;
  bool verbose = false;
};

struct FileId {
  DWORD volume = 0;
  DWORD high = 0;
  DWORD low = 0;
};

auto lower_ascii(std::string s) -> std::string {
  std::ranges::transform(s, s.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return s;
}

auto starts_with_ci(std::string_view text, std::string_view prefix) -> bool {
  if (text.size() < prefix.size()) return false;
  for (size_t i = 0; i < prefix.size(); ++i) {
    if (std::tolower(static_cast<unsigned char>(text[i])) !=
        std::tolower(static_cast<unsigned char>(prefix[i]))) {
      return false;
    }
  }
  return true;
}

auto exe_suffix(std::string_view name) -> std::string {
  std::string out(name);
  if (!out.ends_with(".exe")) out += ".exe";
  return out;
}

auto current_exe_path() -> fs::path {
  std::wstring buffer(MAX_PATH, L'\0');
  DWORD size = GetModuleFileNameW(nullptr, buffer.data(),
                                  static_cast<DWORD>(buffer.size()));
  while (size == buffer.size()) {
    buffer.resize(buffer.size() * 2, L'\0');
    size = GetModuleFileNameW(nullptr, buffer.data(),
                              static_cast<DWORD>(buffer.size()));
  }
  buffer.resize(size);
  return fs::path(buffer);
}

auto default_root() -> fs::path {
  auto exe = current_exe_path();
  return exe.has_parent_path() ? exe.parent_path() : fs::current_path();
}

auto state_dir(const fs::path& root) -> fs::path { return root / ".wpm"; }
auto cache_dir(const fs::path& root) -> fs::path {
  return state_dir(root) / "cache";
}
auto index_dir(const fs::path& root) -> fs::path {
  return state_dir(root) / "indexes";
}
auto staging_dir(const fs::path& root) -> fs::path {
  return state_dir(root) / "staging";
}
auto backup_dir(const fs::path& root) -> fs::path {
  return state_dir(root) / "backup";
}
auto config_path(const fs::path& root) -> fs::path {
  return state_dir(root) / "config.json";
}
auto local_index_path(const fs::path& root) -> fs::path {
  return index_dir(root) / "official.json";
}
auto bundled_index_path(const fs::path& root) -> fs::path {
  return root / "wpm-source" / "index.json";
}

auto read_text(const fs::path& path) -> std::optional<std::string> {
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) return std::nullopt;
  return std::string(std::istreambuf_iterator<char>(in),
                     std::istreambuf_iterator<char>());
}

auto write_text(const fs::path& path, std::string_view text) -> bool {
  std::error_code ec;
  fs::create_directories(path.parent_path(), ec);
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  if (!out.is_open()) return false;
  out.write(text.data(), static_cast<std::streamsize>(text.size()));
  return out.good();
}

auto write_bytes(const fs::path& path, std::span<const std::byte> data)
    -> bool {
  std::error_code ec;
  fs::create_directories(path.parent_path(), ec);
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  if (!out.is_open()) return false;
  out.write(reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size()));
  return out.good();
}

auto parse_json_text(std::string_view text) -> std::optional<nlohmann::json> {
  try {
    return nlohmann::json::parse(text.begin(), text.end());
  } catch (const std::exception& e) {
    safeErrorPrintLn(std::string("wpm: invalid JSON: ") + e.what());
    return std::nullopt;
  }
}

auto load_config(const fs::path& root) -> nlohmann::json {
  if (auto text = read_text(config_path(root))) {
    if (auto parsed = parse_json_text(*text)) return *parsed;
  }
  return nlohmann::json{{"preferred_source", "auto"},
                        {"region", "auto"},
                        {"user_sources", nlohmann::json::array()}};
}

auto save_config(const fs::path& root, const nlohmann::json& config) -> bool {
  return write_text(config_path(root), config.dump(2));
}

auto load_index(const fs::path& root) -> nlohmann::json {
  if (auto text = read_text(local_index_path(root))) {
    if (auto parsed = parse_json_text(*text)) return *parsed;
  }
  if (auto text = read_text(bundled_index_path(root))) {
    if (auto parsed = parse_json_text(*text)) return *parsed;
  }
  if (auto parsed = parse_json_text(kBuiltinIndex)) return *parsed;
  return nlohmann::json::object();
}

auto merged_sources(const fs::path& root, const nlohmann::json& index)
    -> std::vector<nlohmann::json> {
  std::vector<nlohmann::json> sources;
  if (index.contains("sources") && index["sources"].is_array()) {
    for (const auto& source : index["sources"]) sources.push_back(source);
  }

  auto config = load_config(root);
  if (config.contains("user_sources") && config["user_sources"].is_array()) {
    for (const auto& source : config["user_sources"]) sources.push_back(source);
  }

  std::ranges::sort(
      sources, [](const nlohmann::json& a, const nlohmann::json& b) {
        return a.value("priority", 1000) < b.value("priority", 1000);
      });
  return sources;
}

auto package_array(const nlohmann::json& index) -> std::vector<nlohmann::json> {
  std::vector<nlohmann::json> out;
  if (!index.contains("packages") || !index["packages"].is_array()) return out;
  for (const auto& pkg : index["packages"]) {
    if (pkg.is_object()) out.push_back(pkg);
  }
  return out;
}

auto find_package(const nlohmann::json& index, std::string_view name)
    -> std::optional<nlohmann::json> {
  auto wanted = lower_ascii(std::string(name));
  for (const auto& pkg : package_array(index)) {
    if (lower_ascii(pkg.value("name", "")) == wanted) return pkg;
  }
  return std::nullopt;
}

auto detect_arch_key() -> std::string {
  SYSTEM_INFO info{};
  GetNativeSystemInfo(&info);
  switch (info.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_ARM64:
      return "windows-arm64";
    case PROCESSOR_ARCHITECTURE_AMD64:
    default:
      return "windows-x64";
  }
}

auto file_id(const fs::path& path) -> std::optional<FileId> {
  HANDLE h =
      CreateFileW(path.wstring().c_str(), FILE_READ_ATTRIBUTES,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (h == INVALID_HANDLE_VALUE) return std::nullopt;

  BY_HANDLE_FILE_INFORMATION info{};
  if (!GetFileInformationByHandle(h, &info)) {
    CloseHandle(h);
    return std::nullopt;
  }
  CloseHandle(h);
  return FileId{info.dwVolumeSerialNumber, info.nFileIndexHigh,
                info.nFileIndexLow};
}

auto same_file(const fs::path& a, const fs::path& b) -> bool {
  auto fa = file_id(a);
  auto fb = file_id(b);
  return fa && fb && fa->volume == fb->volume && fa->high == fb->high &&
         fa->low == fb->low;
}

auto normalized_path_key(const fs::path& path) -> std::wstring {
  std::error_code ec;
  fs::path normalized = fs::weakly_canonical(path, ec);
  if (ec) {
    ec.clear();
    normalized = fs::absolute(path, ec);
    if (ec) normalized = path;
  }
  normalized = normalized.lexically_normal();

  std::wstring key = normalized.wstring();
  std::ranges::replace(key, L'/', L'\\');
  std::ranges::transform(key, key.begin(),
                         [](wchar_t ch) { return std::towlower(ch); });
  return key;
}

auto same_path_name(const fs::path& a, const fs::path& b) -> bool {
  return normalized_path_key(a) == normalized_path_key(b);
}

auto win32_error_text(DWORD error) -> std::string {
  LPWSTR buffer = nullptr;
  DWORD size = FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, error, 0, reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);
  if (size == 0 || buffer == nullptr) {
    return std::system_category().message(static_cast<int>(error));
  }
  std::wstring text(buffer, size);
  LocalFree(buffer);
  while (!text.empty() && (text.back() == L'\r' || text.back() == L'\n' ||
                           text.back() == L' ' || text.back() == L'\t')) {
    text.pop_back();
  }
  return wstring_to_utf8(text);
}

auto command_names() -> std::vector<std::string> {
  std::vector<std::string> names;
  for (const auto& [name, desc] : CommandRegistry::getAllCommands()) {
    (void)desc;
    names.emplace_back(name);
  }
  if (std::ranges::find(names, std::string(kInternalToolName)) == names.end()) {
    names.emplace_back(kInternalToolName);
  }
  std::ranges::sort(names);
  names.erase(std::ranges::unique(names).begin(), names.end());
  return names;
}

auto legacy_link_names() -> std::vector<std::string> {
  // Removed built-in commands that may still exist as hardlinks in old
  // installs.
  return {"jq"};
}

auto is_legacy_link_name(std::string_view name) -> bool {
  std::string wanted = exe_suffix(name);
  wanted = lower_ascii(wanted);
  for (const auto& legacy : legacy_link_names()) {
    if (wanted == lower_ascii(exe_suffix(legacy))) return true;
  }
  return false;
}

auto cleanup_link_names() -> std::vector<std::string> {
  auto names = command_names();
  for (const auto& legacy : legacy_link_names()) names.emplace_back(legacy);
  std::ranges::sort(names);
  names.erase(std::ranges::unique(names).begin(), names.end());
  return names;
}

auto remove_link_if_safe(const fs::path& source, const fs::path& target,
                         const fs::path& current, bool force, bool dry_run)
    -> bool {
  std::error_code ec;
  if (!fs::exists(target, ec)) return true;
  if (same_file(source, target)) return true;
  if (same_path_name(current, target)) {
    safePrintLn("wpm: keeping running executable: " + target.string());
    return true;
  }
  if (dry_run) return true;
  if (fs::is_directory(target, ec)) {
    safeErrorPrintLn("wpm: refusing to replace directory: " + target.string());
    return false;
  }
  if (!DeleteFileW(target.wstring().c_str())) {
    safeErrorPrintLn("wpm: failed to remove '" + target.string() +
                     "': " + win32_error_text(GetLastError()));
    return false;
  }
  return true;
}

auto remove_stale_legacy_links(const fs::path& root, const fs::path& source,
                               const fs::path& current, bool dry_run,
                               bool verbose) -> std::pair<int, int> {
  const auto current_names = command_names();
  int removed = 0;
  int failed = 0;

  for (const auto& name : legacy_link_names()) {
    if (std::ranges::find(current_names, name) != current_names.end()) continue;
    fs::path target = root / exe_suffix(name);
    if (same_path_name(current, target)) continue;
    std::error_code ec;
    if (!fs::exists(target, ec)) continue;
    if (!same_file(source, target)) continue;

    if (dry_run) {
      safePrintLn("remove legacy link " + target.string());
      ++removed;
      continue;
    }
    if (DeleteFileW(target.wstring().c_str())) {
      ++removed;
      if (verbose) safePrintLn("removed legacy link " + target.string());
    } else {
      safeErrorPrintLn("wpm: failed to remove legacy link '" + target.string() +
                       "': " + win32_error_text(GetLastError()));
      ++failed;
    }
  }

  return {removed, failed};
}

auto rebuild_links(const fs::path& root, bool force, bool dry_run, bool verbose)
    -> int {
  const fs::path source = fs::exists(root / "winuxcmd.exe")
                              ? root / "winuxcmd.exe"
                              : current_exe_path();
  const fs::path current = current_exe_path();

  if (!fs::exists(source)) {
    safeErrorPrintLn("wpm: winuxcmd.exe not found in root: " + root.string());
    return 1;
  }

  int created = 0;
  int unchanged = 0;
  int failed = 0;
  auto [stale_removed, stale_failed] =
      remove_stale_legacy_links(root, source, current, dry_run, verbose);
  failed += stale_failed;

  for (const auto& name : command_names()) {
    fs::path target = root / exe_suffix(name);
    if (same_file(source, target)) {
      ++unchanged;
      continue;
    }
    if (!remove_link_if_safe(source, target, current, force, dry_run)) {
      ++failed;
      continue;
    }
    if (dry_run) {
      safePrintLn("link " + target.string() + " -> " + source.string());
      ++created;
      continue;
    }
    if (CreateHardLinkW(target.wstring().c_str(), source.wstring().c_str(),
                        nullptr)) {
      ++created;
      if (verbose) safePrintLn("linked " + target.string());
    } else {
      DWORD err = GetLastError();
      if (same_file(source, target)) {
        ++unchanged;
        continue;
      }
      safeErrorPrintLn("wpm: failed to create hard link '" + target.string() +
                       "': " + win32_error_text(err));
      ++failed;
    }
  }

  safePrintLn("wpm: links created=" + std::to_string(created) +
              " unchanged=" + std::to_string(unchanged) +
              " stale_removed=" + std::to_string(stale_removed) +
              " failed=" + std::to_string(failed));
  return failed == 0 ? 0 : 1;
}

auto remove_links(const fs::path& root, bool dry_run) -> int {
  fs::path source = fs::exists(root / "winuxcmd.exe") ? root / "winuxcmd.exe"
                                                      : current_exe_path();
  fs::path current = current_exe_path();
  int removed = 0;
  int failed = 0;

  for (const auto& name : cleanup_link_names()) {
    fs::path target = root / exe_suffix(name);
    if (same_path_name(current, target)) continue;
    std::error_code ec;
    if (!fs::exists(target, ec)) continue;
    if (!same_file(source, target)) continue;
    if (dry_run) {
      safePrintLn("remove " + target.string());
      ++removed;
      continue;
    }
    if (DeleteFileW(target.wstring().c_str())) {
      ++removed;
    } else {
      safeErrorPrintLn("wpm: failed to remove '" + target.string() +
                       "': " + win32_error_text(GetLastError()));
      ++failed;
    }
  }

  safePrintLn("wpm: links removed=" + std::to_string(removed) +
              " failed=" + std::to_string(failed));
  return failed == 0 ? 0 : 1;
}

auto http_get(std::string_view url) -> HttpResult {
  HttpResult result;

  if (starts_with_ci(url, "file://")) {
    std::string raw(url.substr(7));
    if (raw.size() >= 3 && raw[0] == '/' && raw[2] == ':')
      raw.erase(raw.begin());
    std::ifstream in{fs::path(raw), std::ios::binary};
    if (!in.is_open()) {
      result.error = "failed to open local file source";
      return result;
    }
    std::string payload{std::istreambuf_iterator<char>(in),
                        std::istreambuf_iterator<char>()};
    result.data.resize(payload.size());
    std::memcpy(result.data.data(), payload.data(), payload.size());
    result.ok = true;
    result.status = 200;
    return result;
  }

  std::wstring wurl = utf8_to_wstring(std::string(url));
  URL_COMPONENTS parts{};
  parts.dwStructSize = sizeof(parts);
  parts.dwSchemeLength = static_cast<DWORD>(-1);
  parts.dwHostNameLength = static_cast<DWORD>(-1);
  parts.dwUrlPathLength = static_cast<DWORD>(-1);
  parts.dwExtraInfoLength = static_cast<DWORD>(-1);
  if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &parts)) {
    result.error = "failed to parse URL";
    return result;
  }

  std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
  std::wstring path_part;
  if (parts.lpszUrlPath) {
    path_part.assign(parts.lpszUrlPath, parts.dwUrlPathLength);
  }
  if (parts.lpszExtraInfo) {
    path_part.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
  }
  if (path_part.empty()) path_part = L"/";

  bool https = parts.nScheme == INTERNET_SCHEME_HTTPS;
  HINTERNET session =
      WinHttpOpen(L"WinuxCmd-WPM/0.2", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                  WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (!session) {
    result.error = "failed to open WinHTTP session";
    return result;
  }

  HINTERNET connect = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
  if (!connect) {
    result.error = "failed to connect";
    WinHttpCloseHandle(session);
    return result;
  }

  DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
  HINTERNET request = WinHttpOpenRequest(connect, L"GET", path_part.c_str(),
                                         nullptr, WINHTTP_NO_REFERER,
                                         WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
  if (!request) {
    result.error = "failed to create request";
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return result;
  }

  DWORD timeout_ms = 12000;
  WinHttpSetOption(request, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout_ms,
                   sizeof(timeout_ms));
  WinHttpSetOption(request, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout_ms,
                   sizeof(timeout_ms));
  WinHttpSetOption(request, WINHTTP_OPTION_SEND_TIMEOUT, &timeout_ms,
                   sizeof(timeout_ms));

  if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                          WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
      !WinHttpReceiveResponse(request, nullptr)) {
    result.error = "request failed: " + win32_error_text(GetLastError());
    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return result;
  }

  DWORD status = 0;
  DWORD status_size = sizeof(status);
  WinHttpQueryHeaders(request,
                      WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                      WINHTTP_HEADER_NAME_BY_INDEX, &status, &status_size,
                      WINHTTP_NO_HEADER_INDEX);
  result.status = status;
  if (status < 200 || status >= 300) {
    result.error = "HTTP status " + std::to_string(status);
    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return result;
  }

  for (;;) {
    DWORD available = 0;
    if (!WinHttpQueryDataAvailable(request, &available)) break;
    if (available == 0) break;
    size_t old_size = result.data.size();
    result.data.resize(old_size + available);
    DWORD read = 0;
    if (!WinHttpReadData(request, result.data.data() + old_size, available,
                         &read)) {
      result.error = "read failed: " + win32_error_text(GetLastError());
      result.data.clear();
      break;
    }
    result.data.resize(old_size + read);
  }

  result.ok = result.error.empty();
  WinHttpCloseHandle(request);
  WinHttpCloseHandle(connect);
  WinHttpCloseHandle(session);
  return result;
}

auto sha256_file(const fs::path& file) -> std::optional<std::string> {
  std::ifstream in(file, std::ios::binary);
  if (!in.is_open()) return std::nullopt;

  BCRYPT_ALG_HANDLE alg = nullptr;
  BCRYPT_HASH_HANDLE hash = nullptr;
  DWORD object_size = 0;
  DWORD data_size = 0;
  DWORD hash_size = 0;

  if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) !=
      0) {
    return std::nullopt;
  }
  BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH,
                    reinterpret_cast<PUCHAR>(&object_size), sizeof(object_size),
                    &data_size, 0);
  BCryptGetProperty(alg, BCRYPT_HASH_LENGTH,
                    reinterpret_cast<PUCHAR>(&hash_size), sizeof(hash_size),
                    &data_size, 0);

  std::vector<UCHAR> object(object_size);
  std::vector<UCHAR> digest(hash_size);
  if (BCryptCreateHash(alg, &hash, object.data(), object_size, nullptr, 0, 0) !=
      0) {
    BCryptCloseAlgorithmProvider(alg, 0);
    return std::nullopt;
  }

  std::array<char, 8192> buffer{};
  while (in.good()) {
    in.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    auto got = in.gcount();
    if (got > 0) {
      BCryptHashData(hash, reinterpret_cast<PUCHAR>(buffer.data()),
                     static_cast<ULONG>(got), 0);
    }
  }
  BCryptFinishHash(hash, digest.data(), hash_size, 0);
  BCryptDestroyHash(hash);
  BCryptCloseAlgorithmProvider(alg, 0);

  static constexpr char hex[] = "0123456789abcdef";
  std::string out;
  out.reserve(digest.size() * 2);
  for (auto byte : digest) {
    out.push_back(hex[(byte >> 4) & 0x0f]);
    out.push_back(hex[byte & 0x0f]);
  }
  return out;
}

auto verify_sha256(const fs::path& file, std::string expected) -> bool {
  if (expected.empty()) {
    safeErrorPrintLn("wpm: refusing remote artifact without sha256");
    return false;
  }
  expected = lower_ascii(expected);
  auto actual = sha256_file(file);
  if (!actual) {
    safeErrorPrintLn("wpm: failed to calculate sha256 for " + file.string());
    return false;
  }
  if (*actual != expected) {
    safeErrorPrintLn("wpm: sha256 mismatch for " + file.string());
    safeErrorPrintLn("wpm: expected " + expected);
    safeErrorPrintLn("wpm: actual   " + *actual);
    return false;
  }
  return true;
}

auto run_process(const std::wstring& command, const fs::path& cwd = {}) -> int {
  std::wstring mutable_cmd = command;
  STARTUPINFOW si{};
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi{};
  BOOL ok = CreateProcessW(
      nullptr, mutable_cmd.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
      nullptr, cwd.empty() ? nullptr : cwd.wstring().c_str(), &si, &pi);
  if (!ok) return static_cast<int>(GetLastError());
  WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD exit_code = 1;
  GetExitCodeProcess(pi.hProcess, &exit_code);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  return static_cast<int>(exit_code);
}

auto quote(const fs::path& p) -> std::wstring {
  std::wstring s = p.wstring();
  std::wstring out = L"\"";
  for (wchar_t ch : s) {
    if (ch == L'"') out.push_back(L'\\');
    out.push_back(ch);
  }
  out.push_back(L'"');
  return out;
}

auto extract_zip(const fs::path& zip, const fs::path& dest) -> bool {
  std::error_code ec;
  fs::create_directories(dest, ec);
  std::wstring command = L"tar.exe -xf " + quote(zip) + L" -C " + quote(dest);
  int code = run_process(command);
  if (code != 0) {
    safeErrorPrintLn("wpm: zip extraction failed; Windows tar.exe returned " +
                     std::to_string(code));
    return false;
  }
  return true;
}

auto find_file_recursive(const fs::path& root, std::string_view filename)
    -> std::optional<fs::path> {
  std::error_code ec;
  if (!fs::exists(root, ec)) return std::nullopt;
  for (const auto& entry : fs::recursive_directory_iterator(root, ec)) {
    if (ec) break;
    if (!entry.is_regular_file(ec)) continue;
    if (lower_ascii(entry.path().filename().string()) ==
        lower_ascii(std::string(filename))) {
      return entry.path();
    }
  }
  return std::nullopt;
}

auto artifact_for_current_arch(const nlohmann::json& pkg)
    -> std::optional<nlohmann::json> {
  if (!pkg.contains("artifacts") || !pkg["artifacts"].is_object())
    return std::nullopt;
  std::string key = detect_arch_key();
  if (pkg["artifacts"].contains(key)) return pkg["artifacts"][key];
  return std::nullopt;
}

auto artifact_urls(const nlohmann::json& artifact) -> std::vector<std::string> {
  std::vector<std::string> urls;
  if (!artifact.contains("urls")) return urls;
  if (artifact["urls"].is_array()) {
    for (const auto& item : artifact["urls"]) {
      if (item.is_string()) urls.push_back(item.get<std::string>());
      if (item.is_object() && item.contains("url"))
        urls.push_back(item["url"].get<std::string>());
    }
  }
  return urls;
}

auto download_artifact(const fs::path& root, const std::string& package,
                       const nlohmann::json& artifact, bool verbose)
    -> std::optional<fs::path> {
  auto urls = artifact_urls(artifact);
  if (urls.empty()) {
    safeErrorPrintLn("wpm: package '" + package +
                     "' has no URLs in the local index");
    safeErrorPrintLn("wpm: run 'wpm index update' or choose another source");
    return std::nullopt;
  }

  std::string type = artifact.value("type", "exe");
  fs::path out = cache_dir(root) / (package + "." + type);
  for (const auto& url : urls) {
    if (verbose) safePrintLn("wpm: downloading " + url);
    auto result = http_get(url);
    if (!result.ok) {
      safeErrorPrintLn("wpm: download failed from " + url + ": " +
                       result.error);
      continue;
    }
    if (!write_bytes(out, result.data)) {
      safeErrorPrintLn("wpm: failed to write cache file " + out.string());
      continue;
    }
    if (!verify_sha256(out, artifact.value("sha256", ""))) {
      std::error_code ec;
      fs::remove(out, ec);
      continue;
    }
    return out;
  }

  return std::nullopt;
}

auto copy_artifact_files(const fs::path& extracted, const fs::path& root,
                         const nlohmann::json& artifact, bool force,
                         bool dry_run) -> bool {
  if (!artifact.contains("files") || !artifact["files"].is_array()) {
    safeErrorPrintLn("wpm: artifact has no files mapping");
    return false;
  }

  for (const auto& mapping : artifact["files"]) {
    std::string from = mapping.value("from", "");
    std::string to = mapping.value("to", from);
    if (from.empty() || to.empty()) {
      safeErrorPrintLn("wpm: invalid file mapping in artifact");
      return false;
    }

    auto src = find_file_recursive(extracted, from);
    if (!src) {
      safeErrorPrintLn("wpm: extracted file not found: " + from);
      return false;
    }

    fs::path dest = root / to;
    std::error_code ec;
    bool dest_exists = fs::exists(dest, ec);
    bool dest_is_winux_link = dest_exists &&
                              same_file(root / "winuxcmd.exe", dest) &&
                              !same_path_name(root / "winuxcmd.exe", dest);
    bool dest_is_legacy_link = is_legacy_link_name(dest.filename().string());
    if (dest_exists && !dest_is_winux_link && !force &&
        !same_file(*src, dest)) {
      safeErrorPrintLn("wpm: destination exists; use --force: " +
                       dest.string());
      return false;
    }
    if (dest_is_winux_link && !force && !dest_is_legacy_link) {
      safeErrorPrintLn(
          "wpm: destination is a WinuxCmd hardlink; use --force: " +
          dest.string());
      return false;
    }
    if (dry_run) {
      safePrintLn("install " + src->string() + " -> " + dest.string());
      continue;
    }
    fs::create_directories(dest.parent_path(), ec);
    if (dest_is_winux_link) {
      if (!DeleteFileW(dest.wstring().c_str())) {
        safeErrorPrintLn("wpm: failed to break WinuxCmd hardlink '" +
                         dest.string() +
                         "': " + win32_error_text(GetLastError()));
        return false;
      }
    }
    fs::copy_file(*src, dest, fs::copy_options::overwrite_existing, ec);
    if (ec) {
      safeErrorPrintLn("wpm: failed to copy '" + src->string() + "' to '" +
                       dest.string() + "': " + ec.message());
      return false;
    }
  }
  return true;
}

auto install_package(const Options& opts, std::string_view package_name)
    -> int {
  auto index = load_index(opts.root);
  auto pkg = find_package(index, package_name);
  if (!pkg) {
    safeErrorPrintLn("wpm: package not found: " + std::string(package_name));
    return 1;
  }
  auto artifact = artifact_for_current_arch(*pkg);
  if (!artifact) {
    safeErrorPrintLn("wpm: no artifact for " + detect_arch_key());
    return 1;
  }
  auto downloaded = download_artifact(opts.root, pkg->value("name", ""),
                                      *artifact, opts.verbose);
  if (!downloaded) return 1;

  fs::path extracted = staging_dir(opts.root) / pkg->value("name", "package");
  std::error_code ec;
  fs::remove_all(extracted, ec);
  fs::create_directories(extracted, ec);

  std::string type = artifact->value("type", "exe");
  if (type == "zip") {
    if (!extract_zip(*downloaded, extracted)) return 1;
  } else if (type == "exe") {
    fs::copy_file(*downloaded, extracted / downloaded->filename(),
                  fs::copy_options::overwrite_existing, ec);
    if (ec) {
      safeErrorPrintLn("wpm: failed to stage exe: " + ec.message());
      return 1;
    }
  } else {
    safeErrorPrintLn("wpm: unsupported artifact type: " + type);
    return 1;
  }

  if (!copy_artifact_files(extracted, opts.root, *artifact, opts.force,
                           opts.dry_run)) {
    return 1;
  }
  safePrintLn("wpm: installed " +
              pkg->value("name", std::string(package_name)));
  return 0;
}

auto launch_apply_update(const fs::path& staged_exe, const fs::path& root,
                         DWORD parent_pid) -> bool {
  std::wstring cmd = quote(staged_exe) + L" wpm -- __apply-update --root " +
                     quote(root) + L" --payload " + quote(staged_exe) +
                     L" --parent " + std::to_wstring(parent_pid);
  STARTUPINFOW si{};
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi{};
  BOOL ok = CreateProcessW(staged_exe.wstring().c_str(), cmd.data(), nullptr,
                           nullptr, FALSE, CREATE_NO_WINDOW, nullptr,
                           root.wstring().c_str(), &si, &pi);
  if (!ok) {
    safeErrorPrintLn("wpm: failed to launch update helper: " +
                     win32_error_text(GetLastError()));
    return false;
  }
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  return true;
}

auto update_winuxcmd(const Options& opts) -> int {
  auto index = load_index(opts.root);
  auto pkg = find_package(index, "winuxcmd");
  if (!pkg) {
    safeErrorPrintLn("wpm: winuxcmd package missing from index");
    return 1;
  }
  auto artifact = artifact_for_current_arch(*pkg);
  if (!artifact) {
    safeErrorPrintLn("wpm: no winuxcmd artifact for " + detect_arch_key());
    return 1;
  }

  auto downloaded =
      download_artifact(opts.root, "winuxcmd", *artifact, opts.verbose);
  if (!downloaded) return 1;

  fs::path extracted = staging_dir(opts.root) / "winuxcmd-update";
  std::error_code ec;
  fs::remove_all(extracted, ec);
  fs::create_directories(extracted, ec);

  if (artifact->value("type", "zip") == "zip") {
    if (!extract_zip(*downloaded, extracted)) return 1;
  } else {
    fs::copy_file(*downloaded, extracted / "winuxcmd.exe",
                  fs::copy_options::overwrite_existing, ec);
    if (ec) {
      safeErrorPrintLn("wpm: failed to stage winuxcmd.exe: " + ec.message());
      return 1;
    }
  }

  auto staged_exe = find_file_recursive(extracted, "winuxcmd.exe");
  if (!staged_exe) {
    safeErrorPrintLn("wpm: staged winuxcmd.exe not found");
    return 1;
  }

  if (opts.dry_run) {
    safePrintLn("would apply update from " + staged_exe->string());
    return 0;
  }

  if (!launch_apply_update(*staged_exe, opts.root, GetCurrentProcessId())) {
    return 1;
  }
  safePrintLn("wpm: update staged; helper will replace winuxcmd after exit");
  return 0;
}

auto parse_hidden_value(std::span<const std::string_view> args,
                        std::string_view name) -> std::string {
  for (size_t i = 0; i + 1 < args.size(); ++i) {
    if (args[i] == name) return std::string(args[i + 1]);
  }
  return {};
}

auto apply_update(std::span<const std::string_view> args) -> int {
  fs::path root = parse_hidden_value(args, "--root");
  fs::path payload = parse_hidden_value(args, "--payload");
  std::string parent_text = parse_hidden_value(args, "--parent");
  if (root.empty() || payload.empty() || parent_text.empty()) {
    safeErrorPrintLn("wpm: invalid apply-update invocation");
    return 1;
  }

  DWORD parent_pid = static_cast<DWORD>(std::stoul(parent_text));
  HANDLE parent = OpenProcess(SYNCHRONIZE, FALSE, parent_pid);
  if (parent) {
    WaitForSingleObject(parent, 30000);
    CloseHandle(parent);
  }

  std::error_code ec;
  fs::create_directories(backup_dir(root), ec);
  fs::path target = root / "winuxcmd.exe";
  fs::path backup =
      backup_dir(root) /
      ("winuxcmd-" + std::string(WinuxCmd::VERSION_STRING) + ".exe");
  if (fs::exists(target, ec)) {
    fs::copy_file(target, backup, fs::copy_options::overwrite_existing, ec);
    if (ec) {
      safeErrorPrintLn("wpm: failed to backup winuxcmd.exe: " + ec.message());
      return 1;
    }
  }

  remove_links(root, false);
  fs::copy_file(payload, target, fs::copy_options::overwrite_existing, ec);
  if (ec) {
    safeErrorPrintLn("wpm: failed to replace winuxcmd.exe: " + ec.message());
    if (fs::exists(backup, ec)) {
      fs::copy_file(backup, target, fs::copy_options::overwrite_existing, ec);
    }
    return 1;
  }
  return rebuild_links(root, true, false, false);
}

auto update_index(const Options& opts) -> int {
  auto current = load_index(opts.root);
  auto sources = merged_sources(opts.root, current);
  std::string wanted = opts.source.empty() ? load_config(opts.root).value(
                                                 "preferred_source", "auto")
                                           : opts.source;

  for (const auto& source : sources) {
    std::string name = source.value("name", "");
    if (name.empty()) continue;
    if (wanted != "auto" && wanted != name) continue;
    if (!source.contains("index_urls") || !source["index_urls"].is_array())
      continue;
    for (const auto& url_json : source["index_urls"]) {
      if (!url_json.is_string()) continue;
      std::string url = url_json.get<std::string>();
      if (opts.verbose) safePrintLn("wpm: fetching index " + url);
      auto result = http_get(url);
      if (!result.ok) {
        safeErrorPrintLn("wpm: index fetch failed from " + name + ": " +
                         result.error);
        continue;
      }
      std::string text(reinterpret_cast<const char*>(result.data.data()),
                       result.data.size());
      auto parsed = parse_json_text(text);
      if (!parsed || !parsed->contains("packages")) {
        safeErrorPrintLn("wpm: fetched index is missing packages");
        continue;
      }
      if (!write_text(local_index_path(opts.root), parsed->dump(2))) {
        safeErrorPrintLn("wpm: failed to save index");
        return 1;
      }
      auto config = load_config(opts.root);
      config["last_success_source"] = name;
      save_config(opts.root, config);
      safePrintLn("wpm: index updated from " + name);
      return 0;
    }
  }

  safeErrorPrintLn("wpm: no reachable index source");
  return 1;
}

auto print_index_status(const Options& opts) -> int {
  auto index = load_index(opts.root);
  auto packages = package_array(index);
  safePrintLn("WPM index");
  safePrintLn("  root: " + opts.root.string());
  safePrintLn("  local: " + local_index_path(opts.root).string());
  safePrintLn("  bundled: " + bundled_index_path(opts.root).string());
  safePrintLn("  version: " + index.value("version", "unknown"));
  safePrintLn("  packages: " + std::to_string(packages.size()));
  return 0;
}

auto list_sources(const Options& opts) -> int {
  auto index = load_index(opts.root);
  auto config = load_config(opts.root);
  auto preferred = config.value("preferred_source", "auto");
  safePrintLn("WPM sources (preferred: " + preferred + ")");
  for (const auto& source : merged_sources(opts.root, index)) {
    std::string name = source.value("name", "");
    std::string region = source.value("region", "");
    size_t urls =
        source.contains("index_urls") && source["index_urls"].is_array()
            ? source["index_urls"].size()
            : 0;
    safePrintLn("  " + name + " region=" + region +
                " urls=" + std::to_string(urls));
  }
  return 0;
}

auto source_use(const Options& opts, std::string_view name) -> int {
  auto config = load_config(opts.root);
  config["preferred_source"] = std::string(name);
  if (!save_config(opts.root, config)) {
    safeErrorPrintLn("wpm: failed to save source preference");
    return 1;
  }
  safePrintLn("wpm: preferred source set to " + std::string(name));
  return 0;
}

auto source_add(const Options& opts, std::string_view name,
                std::string_view url) -> int {
  auto config = load_config(opts.root);
  if (!config.contains("user_sources") || !config["user_sources"].is_array()) {
    config["user_sources"] = nlohmann::json::array();
  }
  config["user_sources"].push_back(nlohmann::json{
      {"name", std::string(name)},
      {"region", "custom"},
      {"priority", 5},
      {"index_urls", nlohmann::json::array({std::string(url)})}});
  if (!save_config(opts.root, config)) {
    safeErrorPrintLn("wpm: failed to save source");
    return 1;
  }
  safePrintLn("wpm: source added: " + std::string(name));
  return 0;
}

auto list_packages(const Options& opts) -> int {
  auto index = load_index(opts.root);
  for (const auto& pkg : package_array(index)) {
    std::string version = pkg.value("version", "");
    if (!version.empty()) version = " " + version;
    safePrintLn("  " + pkg.value("name", "") + version + " - " +
                pkg.value("description", ""));
  }
  return 0;
}

auto show_info(const Options& opts, std::string_view name) -> int {
  auto index = load_index(opts.root);
  auto pkg = find_package(index, name);
  if (!pkg) {
    safeErrorPrintLn("wpm: package not found: " + std::string(name));
    return 1;
  }
  safePrintLn("Name: " + pkg->value("name", ""));
  safePrintLn("Version: " + pkg->value("version", ""));
  safePrintLn("Kind: " + pkg->value("kind", ""));
  safePrintLn("Description: " + pkg->value("description", ""));
  auto artifact = artifact_for_current_arch(*pkg);
  safePrintLn("Artifact: " +
              std::string(artifact ? detect_arch_key() : "none"));
  if (artifact) {
    safePrintLn("Type: " + artifact->value("type", ""));
    safePrintLn("URLs: " + std::to_string(artifact_urls(*artifact).size()));
  }
  return 0;
}

auto print_usage() -> int {
  safePrintLn("Winux Package Manager " + std::string(kVersion));
  safePrintLn("Usage: wpm <command> [args] [options]");
  safePrintLn("");
  safePrintLn("Commands:");
  safePrintLn("  links list|rebuild|remove     manage WinuxCmd hardlinks");
  safePrintLn("  index status|update           inspect or refresh local index");
  safePrintLn("  source list|use|add           manage index sources");
  safePrintLn("  list                          list indexed packages");
  safePrintLn("  info <package>                show package metadata");
  safePrintLn(
      "  install <package>             install package from local index");
  safePrintLn(
      "  update winuxcmd               update WinuxCmd from local index");
  return 0;
}

auto build_options(const CommandContext<WPM_OPTIONS.size()>& ctx) -> Options {
  Options opts;
  std::string root = ctx.get<std::string>("--root", "");
  if (root.empty()) root = ctx.get<std::string>("-r", "");
  opts.root = root.empty() ? default_root() : fs::path(root);
  opts.source = ctx.get<std::string>("--source", "");
  if (opts.source.empty()) opts.source = ctx.get<std::string>("-s", "");
  opts.force = ctx.get<bool>("--force", false) || ctx.get<bool>("-f", false);
  opts.dry_run =
      ctx.get<bool>("--dry-run", false) || ctx.get<bool>("-n", false);
  opts.verbose =
      ctx.get<bool>("--verbose", false) || ctx.get<bool>("-v", false);
  return opts;
}

auto dispatch(const Options& opts, std::span<const std::string_view> args)
    -> int {
  if (args.empty()) return print_usage();

  if (args[0] == "__apply-update") {
    return apply_update(args.subspan(1));
  }

  if (args[0] == "links") {
    if (args.size() == 1 || args[1] == "list") {
      for (const auto& name : command_names()) safePrintLn(name);
      return 0;
    }
    if (args[1] == "rebuild") {
      return rebuild_links(opts.root, opts.force, opts.dry_run, opts.verbose);
    }
    if (args[1] == "remove") {
      return remove_links(opts.root, opts.dry_run);
    }
    safeErrorPrintLn("wpm: usage: wpm links list|rebuild|remove");
    return 1;
  }

  if (args[0] == "index" || args[0] == "update-index") {
    if (args[0] == "update-index" || (args.size() >= 2 && args[1] == "update"))
      return update_index(opts);
    if (args.size() == 1 || args[1] == "status")
      return print_index_status(opts);
    safeErrorPrintLn("wpm: usage: wpm index status|update");
    return 1;
  }

  if (args[0] == "source") {
    if (args.size() == 1 || args[1] == "list") return list_sources(opts);
    if (args[1] == "use" && args.size() >= 3) return source_use(opts, args[2]);
    if (args[1] == "add" && args.size() >= 4)
      return source_add(opts, args[2], args[3]);
    if (args[1] == "test") return update_index(opts);
    safeErrorPrintLn("wpm: usage: wpm source list|use <name>|add <name> <url>");
    return 1;
  }

  if (args[0] == "list" || args[0] == "search") return list_packages(opts);
  if (args[0] == "info" && args.size() >= 2) return show_info(opts, args[1]);
  if (args[0] == "install" && args.size() >= 2)
    return install_package(opts, args[1]);
  if ((args[0] == "update" || args[0] == "upgrade") && args.size() >= 2 &&
      (args[1] == "winuxcmd" || args[1] == "coreutils")) {
    return update_winuxcmd(opts);
  }
  if (args[0] == "version") {
    safePrintLn("wpm " + std::string(kVersion));
    return 0;
  }

  safeErrorPrintLn("wpm: unknown command: " + std::string(args[0]));
  return 1;
}

}  // namespace wpm

REGISTER_COMMAND(
    wpm, "wpm", "manage WinuxCmd packages and command links",
    "WPM is WinuxCmd's internal package and link manager. It keeps a local "
    "package index, downloads artifacts with WinHTTP, updates WinuxCmd, and "
    "rebuilds command hardlinks without external scripts.",
    "  wpm links rebuild\n"
    "  wpm index update\n"
    "  wpm source list\n"
    "  wpm update winuxcmd",
    "winuxcmd(1), ln(1)", "WinuxCmd Project", "Copyright (c) 2026 WinuxCmd",
    WPM_OPTIONS) {
  try {
    auto opts = wpm::build_options(ctx);
    return wpm::dispatch(
        opts, std::span<const std::string_view>(ctx.positionals.data(),
                                                ctx.positionals.size()));
  } catch (const std::exception& e) {
    safeErrorPrintLn(std::string("wpm: ") + e.what());
    return 1;
  }
}
