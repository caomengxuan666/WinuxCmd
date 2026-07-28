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
#include <urlmon.h>
#include <winhttp.h>

#include "core/command_macros.h"
#include "pch/pch.h"

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "urlmon.lib")
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
    OPTION("-a", "--all", "show index-only packages in list output"),
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
  "version": "builtin-2026.07.26",
  "updated": "2026-07-26",
  "sources": [
    {
      "name": "official-github-raw",
      "region": "global",
      "priority": 10,
      "description": "Canonical raw GitHub index for the official WinuxCmd WPM source.",
      "homepage": "https://github.com/unixwin/wpm-source",
      "index_urls": [
        "https://raw.githubusercontent.com/unixwin/wpm-source/main/index.json"
      ]
    },
    {
      "name": "official-jsdelivr",
      "region": "global",
      "priority": 15,
      "description": "CDN mirror of the canonical WPM index.",
      "homepage": "https://cdn.jsdelivr.net/gh/unixwin/wpm-source@main/index.json",
      "index_urls": [
        "https://cdn.jsdelivr.net/gh/unixwin/wpm-source@main/index.json"
      ]
    },
    {
      "name": "official-github-release",
      "region": "global",
      "priority": 20,
      "description": "Release-pinned WPM index for stable WinuxCmd packages.",
      "homepage": "https://github.com/unixwin/wpm-source/releases",
      "index_urls": [
        "https://github.com/unixwin/wpm-source/releases/latest/download/wpm-index.json"
      ]
    },
    {
      "name": "official-cn",
      "region": "cn",
      "priority": 30,
      "description": "Reserved China-friendly WPM index mirror.",
      "homepage": "",
      "index_urls": []
    }
  ],
  "packages": []
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
  bool all = false;
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
  if (auto parsed = parse_json_text(kBuiltinIndex)) return *parsed;
  return nlohmann::json::object();
}

auto merged_sources(const fs::path& root, const nlohmann::json& index)
    -> std::vector<nlohmann::json> {
  std::vector<nlohmann::json> sources;
  std::unordered_set<std::string> builtin_names;
  if (auto builtin = parse_json_text(kBuiltinIndex);
      builtin && builtin->contains("sources") &&
      (*builtin)["sources"].is_array()) {
    for (const auto& source : (*builtin)["sources"]) {
      if (!source.is_object()) continue;
      sources.push_back(source);
      auto name = source.value("name", "");
      if (!name.empty()) builtin_names.insert(name);
    }
  }

  if (index.contains("sources") && index["sources"].is_array()) {
    for (const auto& source : index["sources"]) {
      if (!source.is_object()) continue;
      auto name = source.value("name", "");
      if (!name.empty() && builtin_names.contains(name)) continue;
      sources.push_back(source);
    }
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

auto allow_progress_bar_output() -> bool {
  auto terminal = get_terminal_info();
  return terminal.is_tty || GetEnvironmentVariableA("WT_SESSION", nullptr, 0) > 0 ||
         GetEnvironmentVariableA("WINUXSH_WPM_PROGRESS", nullptr, 0) > 0;
}

class UrlmonProgressCallback : public IBindStatusCallback {
 public:
  UrlmonProgressCallback(std::string label, bool allow_progress_bar)
      : label_(std::move(label)), allow_progress_bar_(allow_progress_bar) {}

  auto finish() -> void {
    if (progress_) {
      progress_->finish();
      progress_.reset();
    }
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** object) override {
    if (!object) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IBindStatusCallback) {
      *object = static_cast<IBindStatusCallback*>(this);
      return S_OK;
    }
    *object = nullptr;
    return E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE AddRef() override { return 1; }

  ULONG STDMETHODCALLTYPE Release() override { return 1; }

  HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD, IBinding*) override {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE GetPriority(LONG*) override { return E_NOTIMPL; }

  HRESULT STDMETHODCALLTYPE OnLowResource(DWORD) override { return S_OK; }

  HRESULT STDMETHODCALLTYPE OnProgress(ULONG progress, ULONG progress_max,
                                       ULONG, LPCWSTR) override {
    if (label_.empty()) return S_OK;

    if (allow_progress_bar_ && progress_max > 0) {
      if (!progress_) {
        progress_ = std::make_unique<ProgressBar>(100, label_, 36);
      }
      int percent = static_cast<int>(
          std::min<ULONG>(100, (progress * 100) / progress_max));
      if (percent != last_percent_) {
        progress_->update(percent);
        last_percent_ = percent;
      }
    } else if (!announced_) {
      safePrintLn(label_);
      announced_ = true;
    }
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT, LPCWSTR) override {
    finish();
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD* bind_flags,
                                        BINDINFO* bind_info) override {
    if (bind_flags) *bind_flags = 0;
    if (bind_info) {
      auto size = bind_info->cbSize;
      std::memset(bind_info, 0, sizeof(BINDINFO));
      bind_info->cbSize = size ? size : sizeof(BINDINFO);
    }
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD, DWORD, FORMATETC*,
                                            STGMEDIUM*) override {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID, IUnknown*) override {
    return S_OK;
  }

 private:
  std::string label_;
  bool allow_progress_bar_;
  bool announced_ = false;
  int last_percent_ = -1;
  std::unique_ptr<ProgressBar> progress_;
};

auto http_get_urlmon(std::wstring_view url,
                     std::string_view progress_label = {}) -> HttpResult {
  HttpResult result;

  std::array<wchar_t, MAX_PATH> temp_dir{};
  DWORD temp_len =
      GetTempPathW(static_cast<DWORD>(temp_dir.size()), temp_dir.data());
  if (temp_len == 0 || temp_len >= temp_dir.size()) {
    result.error = "URLMon fallback failed to get temp path";
    return result;
  }

  std::array<wchar_t, MAX_PATH> temp_file{};
  if (GetTempFileNameW(temp_dir.data(), L"wpm", 0, temp_file.data()) == 0) {
    result.error = "URLMon fallback failed to create temp file";
    return result;
  }

  UrlmonProgressCallback progress(std::string(progress_label),
                                  allow_progress_bar_output());
  auto* callback = progress_label.empty()
                       ? nullptr
                       : static_cast<IBindStatusCallback*>(&progress);
  HRESULT hr = URLDownloadToFileW(nullptr, std::wstring(url).c_str(),
                                  temp_file.data(), 0, callback);
  progress.finish();
  if (FAILED(hr)) {
    DeleteFileW(temp_file.data());
    result.error =
        std::format("URLMon fallback failed: 0x{:08x}",
                    static_cast<unsigned int>(static_cast<ULONG>(hr)));
    return result;
  }

  std::ifstream in{fs::path(temp_file.data()), std::ios::binary};
  if (!in.is_open()) {
    DeleteFileW(temp_file.data());
    result.error = "URLMon fallback failed to read temp file";
    return result;
  }

  std::string payload{std::istreambuf_iterator<char>(in),
                      std::istreambuf_iterator<char>()};
  result.data.resize(payload.size());
  std::memcpy(result.data.data(), payload.data(), payload.size());
  result.ok = true;
  result.status = 200;
  DeleteFileW(temp_file.data());
  return result;
}

auto human_size(unsigned long long bytes) -> std::string {
  constexpr std::array<std::string_view, 5> units = {"B", "KiB", "MiB", "GiB",
                                                     "TiB"};
  double value = static_cast<double>(bytes);
  size_t unit = 0;
  while (value >= 1024.0 && unit + 1 < units.size()) {
    value /= 1024.0;
    ++unit;
  }
  if (unit == 0) return std::to_string(bytes) + " B";
  return std::format("{:.1f} {}", value, units[unit]);
}

auto env_var_present(const char* name) -> bool {
  return GetEnvironmentVariableA(name, nullptr, 0) > 0;
}

auto response_header(HINTERNET request, DWORD query)
    -> std::optional<std::wstring> {
  DWORD size = 0;
  SetLastError(ERROR_SUCCESS);
  WinHttpQueryHeaders(request, query, WINHTTP_HEADER_NAME_BY_INDEX,
                      WINHTTP_NO_OUTPUT_BUFFER, &size,
                      WINHTTP_NO_HEADER_INDEX);
  if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || size == 0) {
    return std::nullopt;
  }

  std::wstring value(size / sizeof(wchar_t), L'\0');
  if (!WinHttpQueryHeaders(request, query, WINHTTP_HEADER_NAME_BY_INDEX,
                           value.data(), &size, WINHTTP_NO_HEADER_INDEX)) {
    return std::nullopt;
  }
  value.resize(size / sizeof(wchar_t));
  while (!value.empty() && value.back() == L'\0') value.pop_back();
  return value;
}

auto redirect_url(std::wstring_view location, bool https, std::wstring_view host,
                  INTERNET_PORT port) -> std::string {
  auto location_utf8 = wstring_to_utf8(location);
  if (starts_with_ci(location_utf8, "http://") ||
      starts_with_ci(location_utf8, "https://")) {
    return location_utf8;
  }

  std::wstring absolute = https ? L"https:" : L"http:";
  if (location.starts_with(L"//")) {
    absolute += std::wstring(location);
  } else {
    absolute += L"//";
    absolute += host;
    bool default_port =
        (https && port == INTERNET_DEFAULT_HTTPS_PORT) ||
        (!https && port == INTERNET_DEFAULT_HTTP_PORT);
    if (!default_port) absolute += L":" + std::to_wstring(port);
    if (!location.empty() && location.front() != L'/') absolute += L"/";
    absolute += std::wstring(location);
  }
  return wstring_to_utf8(absolute);
}

auto http_get(std::string_view url, std::string_view progress_label = {},
              int redirects_remaining = 5) -> HttpResult {
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
    auto fallback = http_get_urlmon(wurl, progress_label);
    return fallback.ok ? fallback : result;
  }

  HINTERNET connect = WinHttpConnect(session, host.c_str(), parts.nPort, 0);
  if (!connect) {
    result.error = "failed to connect";
    WinHttpCloseHandle(session);
    auto fallback = http_get_urlmon(wurl, progress_label);
    return fallback.ok ? fallback : result;
  }

  DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
  HINTERNET request = WinHttpOpenRequest(connect, L"GET", path_part.c_str(),
                                         nullptr, WINHTTP_NO_REFERER,
                                         WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
  if (!request) {
    result.error = "failed to create request";
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    auto fallback = http_get_urlmon(wurl, progress_label);
    return fallback.ok ? fallback : result;
  }

  DWORD timeout_ms = 12000;
  WinHttpSetOption(request, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout_ms,
                   sizeof(timeout_ms));
  WinHttpSetOption(request, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout_ms,
                   sizeof(timeout_ms));
  WinHttpSetOption(request, WINHTTP_OPTION_SEND_TIMEOUT, &timeout_ms,
                   sizeof(timeout_ms));
  DWORD redirect_policy = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
  WinHttpSetOption(request, WINHTTP_OPTION_REDIRECT_POLICY, &redirect_policy,
                   sizeof(redirect_policy));

  if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                          WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
      !WinHttpReceiveResponse(request, nullptr)) {
    result.error = "request failed: " + win32_error_text(GetLastError());
    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    auto fallback = http_get_urlmon(wurl, progress_label);
    return fallback.ok ? fallback : result;
  }

  DWORD status = 0;
  DWORD status_size = sizeof(status);
  WinHttpQueryHeaders(request,
                      WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                      WINHTTP_HEADER_NAME_BY_INDEX, &status, &status_size,
                      WINHTTP_NO_HEADER_INDEX);
  result.status = status;
  if (status >= 300 && status < 400 && redirects_remaining > 0) {
    auto location = response_header(request, WINHTTP_QUERY_LOCATION);
    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    if (location && !location->empty()) {
      return http_get(redirect_url(*location, https, host, parts.nPort),
                      progress_label, redirects_remaining - 1);
    }
    result.error = "HTTP redirect without Location header";
    return result;
  }
  if (status < 200 || status >= 300) {
    result.error = "HTTP status " + std::to_string(status);
    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    auto fallback = http_get_urlmon(wurl, progress_label);
    return fallback.ok ? fallback : result;
  }

  std::optional<unsigned long long> expected_bytes;
  std::array<wchar_t, 64> content_length{};
  DWORD content_length_size =
      static_cast<DWORD>(content_length.size() * sizeof(wchar_t));
  if (WinHttpQueryHeaders(request, WINHTTP_QUERY_CONTENT_LENGTH,
                          WINHTTP_HEADER_NAME_BY_INDEX, content_length.data(),
                          &content_length_size, WINHTTP_NO_HEADER_INDEX)) {
    wchar_t* end = nullptr;
    auto value = std::wcstoull(content_length.data(), &end, 10);
    if (end != content_length.data()) expected_bytes = value;
  }

  auto terminal = get_terminal_info();
  bool allow_progress_bar = terminal.is_tty || env_var_present("WT_SESSION") ||
                            env_var_present("WINUXSH_WPM_PROGRESS");
  bool show_progress = !progress_label.empty() && allow_progress_bar &&
                       expected_bytes && *expected_bytes > 0;
  std::unique_ptr<ProgressBar> progress;
  int last_percent = -1;
  if (show_progress) {
    progress = std::make_unique<ProgressBar>(
        100, std::string(progress_label) + " (" +
                 human_size(static_cast<long long>(*expected_bytes)) + ")",
        36);
    progress->update(0);
    last_percent = 0;
  } else if (!progress_label.empty()) {
    std::string message(progress_label);
    if (expected_bytes && *expected_bytes > 0) {
      message += " (" + human_size(*expected_bytes) + ")";
    }
    safePrintLn(message);
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
    if (progress && expected_bytes) {
      auto percent = static_cast<int>(
          std::min<unsigned long long>(
              100, (static_cast<unsigned long long>(result.data.size()) *
                    100) /
                       *expected_bytes));
      if (percent != last_percent) {
        progress->update(percent);
        last_percent = percent;
      }
    }
  }

  if (progress) {
    if (result.error.empty()) {
      progress->finish();
    } else {
      safePrintLn("");
    }
  }

  if (result.error.empty() && expected_bytes &&
      result.data.size() != *expected_bytes) {
    result.error = "incomplete download: expected " +
                   std::to_string(*expected_bytes) + " bytes, got " +
                   std::to_string(result.data.size());
    result.data.clear();
  }

  result.ok = result.error.empty();
  WinHttpCloseHandle(request);
  WinHttpCloseHandle(connect);
  WinHttpCloseHandle(session);
  if (!result.ok) {
    auto fallback = http_get_urlmon(wurl, progress_label);
    return fallback.ok ? fallback : result;
  }
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

auto find_artifact_file(const fs::path& root, std::string_view from)
    -> std::optional<fs::path> {
  fs::path requested = fs::path(std::string(from));
  std::error_code ec;
  auto direct = root / requested;
  if (fs::is_regular_file(direct, ec)) return direct;
  return find_file_recursive(root, requested.filename().string());
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

auto join_json_string_array(const nlohmann::json& object, std::string_view key)
    -> std::string {
  if (!object.contains(std::string(key)) ||
      !object[std::string(key)].is_array()) {
    return "";
  }

  std::vector<std::string> values;
  for (const auto& item : object[std::string(key)]) {
    if (item.is_string()) values.push_back(item.get<std::string>());
  }

  std::string out;
  for (const auto& value : values) {
    if (!out.empty()) out += ", ";
    out += value;
  }
  return out;
}

auto artifact_install_state(const nlohmann::json& pkg) -> std::string {
  auto artifact = artifact_for_current_arch(pkg);
  if (!artifact) return "metadata-only (no current-arch artifact)";
  if (artifact_urls(*artifact).empty())
    return "metadata-only (no download URLs)";
  if (artifact->value("sha256", "").empty())
    return "metadata-only (missing sha256)";
  if (!artifact->contains("files") || !(*artifact)["files"].is_array() ||
      (*artifact)["files"].empty())
    return "metadata-only (missing files mapping)";
  return "ready";
}

auto package_state_label(const nlohmann::json& pkg) -> std::string {
  return artifact_install_state(pkg) == "ready" ? "ready" : "index-only";
}

auto contains_ci(std::string_view text, std::string_view needle) -> bool {
  if (needle.empty()) return true;
  return lower_ascii(std::string(text))
             .find(lower_ascii(std::string(needle))) != std::string::npos;
}

auto package_matches_query(const nlohmann::json& pkg, std::string_view query)
    -> bool {
  if (query.empty()) return true;
  if (contains_ci(pkg.value("name", ""), query)) return true;
  if (contains_ci(pkg.value("description", ""), query)) return true;
  if (contains_ci(pkg.value("category", ""), query)) return true;
  if (contains_ci(pkg.value("license", ""), query)) return true;
  return contains_ci(join_json_string_array(pkg, "commands"), query);
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
    auto result = http_get(url, "wpm: downloading " + package);
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
    std::string to = mapping.contains("to") && mapping["to"].is_string()
                         ? mapping["to"].get<std::string>()
                         : fs::path(from).filename().string();
    if (from.empty() || to.empty()) {
      safeErrorPrintLn("wpm: invalid file mapping in artifact");
      return false;
    }

    auto src = find_artifact_file(extracted, from);
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
      safePrintLn("would copy " + src->string() + " -> " + dest.string());
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

auto update_index(const Options& opts) -> int;

auto refresh_index_once(const Options& opts, std::string_view reason) -> bool {
  safePrintLn("wpm: " + std::string(reason) +
              "; updating index from configured sources");
  if (update_index(opts) == 0) return true;
  safeErrorPrintLn("wpm: automatic index update failed");
  safeErrorPrintLn(
      "wpm: run 'wpm index update' to retry or 'wpm source list' "
      "to inspect sources");
  return false;
}

auto install_package(const Options& opts, std::string_view package_name)
    -> int {
  auto index = load_index(opts.root);
  auto pkg = find_package(index, package_name);
  bool refreshed = false;
  if (!pkg) {
    refreshed = refresh_index_once(opts, "package not found in local index");
    if (refreshed) {
      index = load_index(opts.root);
      pkg = find_package(index, package_name);
    }
  }
  if (!pkg) {
    safeErrorPrintLn("wpm: package not found after index update: " +
                     std::string(package_name));
    return 1;
  }

  auto state = artifact_install_state(*pkg);
  if (state != "ready" && !refreshed) {
    refreshed = refresh_index_once(opts, "package metadata is not installable");
    if (refreshed) {
      index = load_index(opts.root);
      pkg = find_package(index, package_name);
      if (pkg) state = artifact_install_state(*pkg);
    }
  }
  if (state != "ready") {
    safeErrorPrintLn("wpm: package is not installable for " +
                     detect_arch_key() + ": " + state);
    safeErrorPrintLn(
        "wpm: update the source index artifact urls, sha256, and "
        "files mapping");
    return 1;
  }

  auto artifact = artifact_for_current_arch(*pkg);
  if (!artifact) return 1;
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
  if (opts.dry_run) {
    safePrintLn("wpm: dry-run complete; would install " +
                pkg->value("name", std::string(package_name)));
  } else {
    safePrintLn("wpm: installed " +
                pkg->value("name", std::string(package_name)));
  }
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
  bool refreshed = false;
  if (!pkg) {
    refreshed = refresh_index_once(opts, "winuxcmd missing from local index");
    if (refreshed) {
      index = load_index(opts.root);
      pkg = find_package(index, "winuxcmd");
    }
  }
  if (!pkg) {
    safeErrorPrintLn("wpm: winuxcmd package missing after index update");
    return 1;
  }

  auto state = artifact_install_state(*pkg);
  if (state != "ready" && !refreshed) {
    refreshed =
        refresh_index_once(opts, "winuxcmd metadata is not installable");
    if (refreshed) {
      index = load_index(opts.root);
      pkg = find_package(index, "winuxcmd");
      if (pkg) state = artifact_install_state(*pkg);
    }
  }
  if (state != "ready") {
    safeErrorPrintLn("wpm: winuxcmd is not installable for " +
                     detect_arch_key() + ": " + state);
    return 1;
  }

  auto artifact = artifact_for_current_arch(*pkg);
  if (!artifact) return 1;
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
  safePrintLn("  fallback: builtin");
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
    std::string description = source.value("description", "");
    if (!description.empty()) safePrintLn("    " + description);
    if (!opts.verbose) continue;
    std::string homepage = source.value("homepage", "");
    if (!homepage.empty()) safePrintLn("    homepage: " + homepage);
    if (source.contains("index_urls") && source["index_urls"].is_array()) {
      for (const auto& url : source["index_urls"]) {
        if (url.is_string()) safePrintLn("    url: " + url.get<std::string>());
      }
    }
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

auto print_package_summary(const nlohmann::json& pkg) -> void {
  std::string version = pkg.value("version", "");
  if (!version.empty()) version = " " + version;
  std::string commands = join_json_string_array(pkg, "commands");
  if (!commands.empty()) commands = " [" + commands + "]";
  std::string category = pkg.value("category", "");
  if (!category.empty()) category = " {" + category + "}";
  safePrintLn("  [" + package_state_label(pkg) + "] " + pkg.value("name", "") +
              version + commands + category + " - " +
              pkg.value("description", ""));
}

auto list_packages(const Options& opts, std::string_view query = {}) -> int {
  auto index = load_index(opts.root);
  int matched = 0;
  int hidden = 0;
  bool ready_only = query.empty() && !opts.all;
  for (const auto& pkg : package_array(index)) {
    if (!package_matches_query(pkg, query)) continue;
    if (ready_only && package_state_label(pkg) != "ready") {
      ++hidden;
      continue;
    }
    print_package_summary(pkg);
    ++matched;
  }
  if (!query.empty() && matched == 0) {
    safePrintLn("wpm: no packages matched '" + std::string(query) + "'");
    safePrintLn(
        "wpm: run 'wpm index update' to refresh the independent "
        "wpm-source index");
  } else if (query.empty() && matched == 0) {
    safePrintLn("wpm: no packages in the local index");
    safePrintLn(
        "wpm: run 'wpm index update' to fetch the independent "
        "wpm-source index");
  } else if (ready_only && hidden > 0) {
    safePrintLn("wpm: hidden " + std::to_string(hidden) +
                " index-only packages; use --all to show placeholders");
  }
  return 0;
}

auto show_info(const Options& opts, std::string_view name) -> int {
  auto index = load_index(opts.root);
  auto pkg = find_package(index, name);
  if (!pkg) {
    if (refresh_index_once(opts, "package not found in local index")) {
      index = load_index(opts.root);
      pkg = find_package(index, name);
    }
  }
  if (!pkg) {
    safeErrorPrintLn("wpm: package not found after index update: " +
                     std::string(name));
    return 1;
  }
  safePrintLn("Name: " + pkg->value("name", ""));
  safePrintLn("Version: " + pkg->value("version", ""));
  safePrintLn("Kind: " + pkg->value("kind", ""));
  safePrintLn("Category: " + pkg->value("category", ""));
  safePrintLn("License: " + pkg->value("license", ""));
  safePrintLn("Commands: " + join_json_string_array(*pkg, "commands"));
  safePrintLn("Description: " + pkg->value("description", ""));
  safePrintLn("Install state: " + artifact_install_state(*pkg));
  auto artifact = artifact_for_current_arch(*pkg);
  safePrintLn("Artifact: " +
              std::string(artifact ? detect_arch_key() : "none"));
  if (artifact) {
    safePrintLn("Type: " + artifact->value("type", ""));
    safePrintLn("URLs: " + std::to_string(artifact_urls(*artifact).size()));
    std::string sha256 = artifact->value("sha256", "");
    safePrintLn("SHA256: " +
                std::string(sha256.empty() ? "missing" : "present"));
    size_t file_count =
        artifact->contains("files") && (*artifact)["files"].is_array()
            ? (*artifact)["files"].size()
            : 0;
    safePrintLn("Files: " + std::to_string(file_count));
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
  safePrintLn(
      "  list                          list indexed packages and install "
      "state");
  safePrintLn(
      "  search <query>                search names, commands, categories, "
      "licenses");
  safePrintLn("  info <package>                show package metadata");
  safePrintLn(
      "  install <package>             install package from local index");
  safePrintLn(
      "  update winuxcmd               update WinuxCmd from local index");
  safePrintLn("");
  safePrintLn("Options:");
  safePrintLn(
      "  -r, --root <dir>              manage a specific WinuxCmd root");
  safePrintLn("  -s, --source <name>           use a specific index source");
  safePrintLn(
      "  -a, --all                     show index-only packages in list "
      "output");
  safePrintLn(
      "  -f, --force                   overwrite existing files when safe");
  safePrintLn(
      "  -n, --dry-run                 show planned changes without writing");
  safePrintLn("  -v, --verbose                 print detailed progress");
  safePrintLn("  -h, --help                    display this help and exit");
  safePrintLn(
      "  -V, --version                 output version information and exit");
  return 0;
}

auto build_options(const CommandContext<WPM_OPTIONS.size()>& ctx) -> Options {
  Options opts;
  std::string root = ctx.get<std::string>("--root", "");
  if (root.empty()) root = ctx.get<std::string>("-r", "");
  opts.root = root.empty() ? default_root() : fs::path(root);
  opts.source = ctx.get<std::string>("--source", "");
  if (opts.source.empty()) opts.source = ctx.get<std::string>("-s", "");
  opts.all = ctx.get<bool>("--all", false) || ctx.get<bool>("-a", false);
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

  if (args[0] == "list") return list_packages(opts);
  if (args[0] == "search")
    return list_packages(opts, args.size() >= 2 ? args[1] : std::string_view{});
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
