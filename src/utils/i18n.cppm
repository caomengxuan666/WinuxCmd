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

std::string legacy_key(std::string_view text) {
  // FNV-1a keeps legacy call sites source-compatible while giving extracted
  // literals a stable key independent of their file or line number.
  uint64_t hash = 14695981039346656037ull;
  for (const unsigned char ch : text) {
    hash ^= ch;
    hash *= 1099511628211ull;
  }
  return "legacy." + std::format("{:016x}", hash);
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

export std::string translate_error(std::string_view error) {
  if (error == "missing operand") {
    return translate("common.error.missing_operand", error);
  }
  constexpr std::array<std::pair<std::string_view, std::string_view>, 4>
      patterns{{{"missing operand after '", "common.error.missing_after"},
                {"extra operand '", "common.error.extra_operand"},
                {"invalid argument '", "common.error.invalid_argument"},
                {"error reading '", "common.error.reading"}}};
  for (const auto [prefix, key] : patterns) {
    if (!error.starts_with(prefix) || error.back() != '\'') continue;
    const auto value = error.substr(prefix.size(), error.size() - prefix.size() - 1);
    if (key == "common.error.missing_after") {
      return ::winux::i18n::format(key, "missing operand after '{}'", value);
    }
    if (key == "common.error.extra_operand") {
      return ::winux::i18n::format(key, "extra operand '{}'", value);
    }
    if (key == "common.error.invalid_argument") {
      return ::winux::i18n::format(key, "invalid argument '{}'", value);
    }
    return ::winux::i18n::format(key, "error reading '{}'", value);
  }
  return std::string(error);
}

export std::string translate_help_hint(std::string_view text) {
  constexpr std::string_view prefix = "Try '";
  constexpr std::string_view suffix = " --help for more information.";
  if (!text.starts_with(prefix) || !text.ends_with(suffix)) {
    return std::string(text);
  }
  const auto command = text.substr(
      prefix.size(), text.size() - prefix.size() - suffix.size());
  if (command.empty() || command.find_first_of("' \t\r\n") !=
                             std::string_view::npos) {
    return std::string(text);
  }
  return format("common.try_help", "Try '{}' --help for more information.",
                command);
}

export std::string translate_legacy(std::string_view text) {
  return translate(legacy_key(text), text);
}

export bool has_catalog() { return !catalog().messages.empty(); }

}  // namespace winux::i18n
