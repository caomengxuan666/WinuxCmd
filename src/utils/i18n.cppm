module;

#include <windows.h>

export module utils:i18n;

import std;
import :json;

namespace winux::i18n {

namespace {
std::filesystem::path executable_root() {
  std::wstring buffer(MAX_PATH, L'\0');
  DWORD size = GetModuleFileNameW(nullptr, buffer.data(),
                                  static_cast<DWORD>(buffer.size()));
  while (size == buffer.size()) {
    buffer.resize(buffer.size() * 2, L'\0');
    size = GetModuleFileNameW(nullptr, buffer.data(),
                              static_cast<DWORD>(buffer.size()));
  }
  buffer.resize(size);
  auto path = std::filesystem::path(buffer);
  return path.has_parent_path() ? path.parent_path() : std::filesystem::current_path();
}

std::string environment_value(const char* name) {
  const char* value = std::getenv(name);
  return value ? std::string(value) : std::string{};
}

std::string normalize_locale(std::string locale) {
  for (char& ch : locale) {
    if (ch == '_') ch = '-';
  }
  return locale;
}

std::optional<nlohmann::json> read_catalog(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) return std::nullopt;
  std::string text{std::istreambuf_iterator<char>(input),
                   std::istreambuf_iterator<char>{}};
  try {
    auto catalog = nlohmann::json::parse(text);
    if (!catalog.is_object() || !catalog.value("messages", nlohmann::json::object()).is_object()) {
      return std::nullopt;
    }
    return catalog;
  } catch (const std::exception&) {
    return std::nullopt;
  }
}

struct Catalog {
  bool enabled = false;
  std::string locale;
  nlohmann::json messages = nlohmann::json::object();
};

const Catalog& catalog() {
  static const Catalog loaded = [] {
    Catalog result;
    result.locale = normalize_locale(environment_value("WINUX_LANG"));
    if (result.locale.empty() || result.locale == "off" ||
        result.locale == "none" || result.locale == "C") {
      result.locale.clear();
      return result;
    }
    result.enabled = true;

    auto root = executable_root() / ".wpm" / "i18n";
    auto path = root / result.locale / "catalog.json";
    auto parsed = read_catalog(path);
    if (!parsed && result.locale.find('-') != std::string::npos) {
      auto language = result.locale.substr(0, result.locale.find('-'));
      parsed = read_catalog(root / language / "catalog.json");
    }
    if (parsed) result.messages = std::move(*parsed)["messages"];
    return result;
  }();
  return loaded;
}
}  // namespace

export std::string locale() { return catalog().locale; }

export std::string translate(std::string_view key, std::string_view fallback) {
  const auto& messages = catalog().messages;
  auto it = messages.find(std::string(key));
  if (it == messages.end() || !it->is_string()) return std::string(fallback);
  return it->get<std::string>();
}

export template <typename... Args>
std::string format(std::string_view key, std::string_view fallback,
                   Args&&... args) {
  const auto message = translate(key, fallback);
  if constexpr (sizeof...(Args) == 0) {
    return message;
  } else {
    try {
      return std::vformat(message, std::make_format_args(args...));
    } catch (const std::format_error&) {
      // A malformed external catalog entry must never terminate a command.
      return std::vformat(fallback, std::make_format_args(args...));
    }
  }
}

export bool has_catalog() { return !catalog().messages.empty(); }

}  // namespace winux::i18n
