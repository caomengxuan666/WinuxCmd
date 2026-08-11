/*
 *  Copyright © 2026 WinuxCmd
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
 *  - File: basenc.cpp
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for basenc command (multiple encodings).
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr BASENC_OPTIONS = std::array{
    OPTION("-d", "--decode", "decode data", BOOL_TYPE),
    OPTION("-i", "--ignore-garbage",
           "when decoding, ignore non-alphabet characters", BOOL_TYPE),
    OPTION("-w", "--wrap",
           "wrap encoded lines after COLS character (default 76). Use 0 to "
           "disable line wrapping",
           INT_TYPE),
    OPTION("-b", "", "legacy baseN selector", STRING_TYPE),
    OPTION("", "--base64", "same as base64 program", BOOL_TYPE),
    OPTION("", "--base64url", "file- and URL-safe base64", BOOL_TYPE),
    OPTION("", "--base58", "base58 encoding", BOOL_TYPE),
    OPTION("", "--base32", "same as base32 program", BOOL_TYPE),
    OPTION("", "--base32hex", "extended hex alphabet base32", BOOL_TYPE),
    OPTION("", "--base16", "hexadecimal encoding", BOOL_TYPE),
    OPTION("", "--base2lsbf", "bit string, least significant bit first",
           BOOL_TYPE),
    OPTION("", "--base2msbf", "bit string, most significant bit first",
           BOOL_TYPE),
    OPTION("", "--z85", "Z85 encoding", BOOL_TYPE)};

namespace basenc_pipeline {

constexpr std::string_view BASE64_ALPHABET =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr std::string_view BASE64URL_ALPHABET =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
constexpr std::string_view BASE32_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
constexpr std::string_view BASE32HEX_ALPHABET =
    "0123456789ABCDEFGHIJKLMNOPQRSTUV";
constexpr std::string_view BASE16_ALPHABET = "0123456789ABCDEF";
constexpr std::string_view BASE58_ALPHABET =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
constexpr std::string_view Z85_ALPHABET =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/"
    "*?&<>()[]{}@%$#";

enum class Encoding {
  Base64,
  Base64Url,
  Base58,
  Base32,
  Base32Hex,
  Base16,
  Base2Lsbf,
  Base2Msbf,
  Z85,
};

struct Config {
  bool decode = false;
  bool ignore_garbage = false;
  int wrap = 76;
  Encoding encoding = Encoding::Base64;
  std::string file = "-";
};

auto read_input(std::string_view filename)
    -> std::expected<std::string, std::string> {
  return file_io::read_all_input(filename);
}

auto apply_wrap(std::string_view input, int wrap) -> std::string {
  if (wrap <= 0 || input.empty()) return std::string(input);

  const auto width = static_cast<size_t>(wrap);
  std::string wrapped;
  wrapped.reserve(input.size() + ((input.size() - 1) / width));
  for (size_t i = 0; i < input.size(); i += width) {
    if (!wrapped.empty()) wrapped.push_back('\n');
    wrapped.append(input.data() + i, std::min(width, input.size() - i));
  }
  return wrapped;
}

auto encode_base64(std::string_view input, std::string_view alphabet, int wrap)
    -> std::string {
  auto data = std::span<const uint8_t>(
      reinterpret_cast<const uint8_t*>(input.data()), input.size());
  return encoding::base64_encode(data, alphabet, wrap);
}
auto decode_base64(std::string_view input, std::string_view alphabet,
                   bool ignore_garbage)
    -> std::expected<std::string, std::string> {
  std::string clean;
  clean.reserve(input.size());
  bool saw_padding = false;
  int padding = 0;

  for (unsigned char c : input) {
    if (c == '\n' || c == '\r') continue;

    if (alphabet.find(static_cast<char>(c)) != std::string_view::npos) {
      if (saw_padding) return std::unexpected("invalid input");
      clean.push_back(static_cast<char>(c));
      continue;
    }

    if (c == '=') {
      saw_padding = true;
      ++padding;
      if (padding > 2) return std::unexpected("invalid input");
      clean.push_back('=');
      continue;
    }

    if (!ignore_garbage) return std::unexpected("invalid input");
  }

  const size_t data_chars = clean.find('=');
  const size_t encoded_chars =
      data_chars == std::string::npos ? clean.size() : data_chars;

  if (clean.size() % 4 == 1 || encoded_chars % 4 == 1) {
    return std::unexpected("invalid input");
  }
  if (padding > 0 && clean.size() % 4 != 0) {
    return std::unexpected("invalid input");
  }
  if ((padding == 1 && encoded_chars % 4 != 3) ||
      (padding == 2 && encoded_chars % 4 != 2)) {
    return std::unexpected("invalid input");
  }

  std::string output;
  output.reserve((encoded_chars / 4) * 3 + 2);
  uint32_t accumulator = 0;
  int bits = 0;

  for (char c : clean.substr(0, encoded_chars)) {
    accumulator = (accumulator << 6) | static_cast<uint32_t>(alphabet.find(c));
    bits += 6;

    if (bits >= 8) {
      bits -= 8;
      output.push_back(static_cast<char>((accumulator >> bits) & 0xff));
    }
  }

  if (bits > 0 && (accumulator & ((uint32_t{1} << bits) - 1)) != 0) {
    return std::unexpected("invalid input");
  }

  return output;
}

auto encode_base32(std::string_view input, std::string_view alphabet, int wrap)
    -> std::string {
  auto data = std::span<const uint8_t>(
      reinterpret_cast<const uint8_t*>(input.data()), input.size());
  return encoding::base32_encode(data, alphabet, wrap);
}
auto decode_base32(std::string_view input, std::string_view alphabet,
                   bool ignore_garbage)
    -> std::expected<std::string, std::string> {
  std::string clean;
  clean.reserve(input.size());
  bool saw_padding = false;
  int padding = 0;

  for (unsigned char c : input) {
    if (c == '\n' || c == '\r') continue;

    char upper = static_cast<char>(std::toupper(c));
    if (alphabet.find(upper) != std::string_view::npos) {
      if (saw_padding) return std::unexpected("invalid input");
      clean.push_back(upper);
      continue;
    }

    if (c == '=') {
      saw_padding = true;
      ++padding;
      if (padding > 6) return std::unexpected("invalid input");
      clean.push_back('=');
      continue;
    }

    if (!ignore_garbage) return std::unexpected("invalid input");
  }

  const size_t data_chars = clean.find('=');
  const size_t encoded_chars =
      data_chars == std::string::npos ? clean.size() : data_chars;
  const size_t encoded_mod = encoded_chars % 8;

  if (encoded_mod == 1 || encoded_mod == 3 || encoded_mod == 6) {
    return std::unexpected("invalid input");
  }
  if (padding > 0 && clean.size() % 8 != 0) {
    return std::unexpected("invalid input");
  }
  if ((padding == 1 && encoded_mod != 7) ||
      (padding == 3 && encoded_mod != 5) ||
      (padding == 4 && encoded_mod != 4) ||
      (padding == 6 && encoded_mod != 2) || (padding == 2 || padding == 5)) {
    return std::unexpected("invalid input");
  }

  std::string output;
  output.reserve((encoded_chars * 5) / 8);
  uint32_t accumulator = 0;
  int bits = 0;

  for (char c : clean.substr(0, encoded_chars)) {
    accumulator = (accumulator << 5) | static_cast<uint32_t>(alphabet.find(c));
    bits += 5;

    if (bits >= 8) {
      bits -= 8;
      output.push_back(static_cast<char>((accumulator >> bits) & 0xff));
    }
  }

  if (bits > 0 && (accumulator & ((uint32_t{1} << bits) - 1)) != 0) {
    return std::unexpected("invalid input");
  }

  return output;
}

auto encode_base16(std::string_view input, int wrap) -> std::string {
  std::string result;
  result.reserve(input.size() * 2);

  for (unsigned char c : input) {
    result.push_back(BASE16_ALPHABET[c >> 4]);
    result.push_back(BASE16_ALPHABET[c & 0x0f]);
  }

  return apply_wrap(result, wrap);
}

auto decode_base16(std::string_view input, bool ignore_garbage)
    -> std::expected<std::string, std::string> {
  std::string output;
  int high_nibble = -1;

  auto hex_value = [](unsigned char c) -> int {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
  };

  for (unsigned char c : input) {
    if (c == '\n' || c == '\r') continue;

    int value = hex_value(c);
    if (value < 0) {
      if (ignore_garbage) continue;
      return std::unexpected("invalid input");
    }

    if (high_nibble < 0) {
      high_nibble = value;
    } else {
      output.push_back(static_cast<char>((high_nibble << 4) | value));
      high_nibble = -1;
    }
  }

  if (high_nibble >= 0) return std::unexpected("invalid input");
  return output;
}

auto encode_base2(std::string_view input, bool least_significant_first,
                  int wrap) -> std::string {
  std::string result;
  result.reserve(input.size() * 8);

  for (unsigned char c : input) {
    if (least_significant_first) {
      for (int bit = 0; bit < 8; ++bit) {
        result.push_back(((c >> bit) & 1) ? '1' : '0');
      }
    } else {
      for (int bit = 7; bit >= 0; --bit) {
        result.push_back(((c >> bit) & 1) ? '1' : '0');
      }
    }
  }

  return apply_wrap(result, wrap);
}

auto decode_base2(std::string_view input, bool least_significant_first,
                  bool ignore_garbage)
    -> std::expected<std::string, std::string> {
  std::string bits;
  bits.reserve(input.size());

  for (unsigned char c : input) {
    if (c == '\n' || c == '\r') continue;
    if (c == '0' || c == '1') {
      bits.push_back(static_cast<char>(c));
      continue;
    }
    if (!ignore_garbage) return std::unexpected("invalid input");
  }

  if (bits.size() % 8 != 0) return std::unexpected("invalid input");

  std::string output;
  output.reserve(bits.size() / 8);
  for (size_t i = 0; i < bits.size(); i += 8) {
    uint8_t value = 0;
    for (int bit = 0; bit < 8; ++bit) {
      if (bits[i + bit] != '1') continue;
      if (least_significant_first) {
        value |= static_cast<uint8_t>(1u << bit);
      } else {
        value |= static_cast<uint8_t>(1u << (7 - bit));
      }
    }
    output.push_back(static_cast<char>(value));
  }

  return output;
}

auto encode_base58(std::string_view input, int wrap) -> std::string {
  auto bytes = std::span<const unsigned char>(
      reinterpret_cast<const unsigned char*>(input.data()), input.size());

  size_t leading_zeroes = 0;
  while (leading_zeroes < bytes.size() && bytes[leading_zeroes] == 0) {
    ++leading_zeroes;
  }

  std::vector<unsigned char> digits(
      (bytes.size() - leading_zeroes) * 138 / 100 + 1);
  size_t used = 0;
  for (size_t i = leading_zeroes; i < bytes.size(); ++i) {
    unsigned int carry = bytes[i];
    size_t j = 0;
    for (auto it = digits.rbegin();
         (carry != 0 || j < used) && it != digits.rend(); ++it, ++j) {
      carry += static_cast<unsigned int>(*it) << 8;
      *it = static_cast<unsigned char>(carry % 58);
      carry /= 58;
    }
    used = j;
  }

  auto first =
      digits.begin() + static_cast<std::ptrdiff_t>(digits.size() - used);
  while (first != digits.end() && *first == 0) ++first;

  std::string result(leading_zeroes, '1');
  result.reserve(leading_zeroes + static_cast<size_t>(digits.end() - first));
  for (; first != digits.end(); ++first) {
    result.push_back(BASE58_ALPHABET[*first]);
  }
  return apply_wrap(result, wrap);
}

auto decode_base58(std::string_view input, bool ignore_garbage)
    -> std::expected<std::string, std::string> {
  std::array<int, 256> map;
  map.fill(-1);
  for (size_t i = 0; i < BASE58_ALPHABET.size(); ++i) {
    map[static_cast<unsigned char>(BASE58_ALPHABET[i])] = static_cast<int>(i);
  }

  std::string clean;
  clean.reserve(input.size());
  for (unsigned char c : input) {
    if (c == '\n' || c == '\r') continue;
    if (map[c] >= 0) {
      clean.push_back(static_cast<char>(c));
      continue;
    }
    if (!ignore_garbage) return std::unexpected("invalid input");
  }

  size_t leading_zeroes = 0;
  while (leading_zeroes < clean.size() && clean[leading_zeroes] == '1') {
    ++leading_zeroes;
  }

  std::vector<unsigned char> bytes(
      (clean.size() - leading_zeroes) * 733 / 1000 + 1);
  size_t used = 0;
  for (size_t i = leading_zeroes; i < clean.size(); ++i) {
    unsigned int carry =
        static_cast<unsigned int>(map[static_cast<unsigned char>(clean[i])]);
    size_t j = 0;
    for (auto it = bytes.rbegin();
         (carry != 0 || j < used) && it != bytes.rend(); ++it, ++j) {
      carry += static_cast<unsigned int>(*it) * 58;
      *it = static_cast<unsigned char>(carry & 0xff);
      carry >>= 8;
    }
    used = j;
  }

  auto first = bytes.begin() + static_cast<std::ptrdiff_t>(bytes.size() - used);
  while (first != bytes.end() && *first == 0) ++first;

  std::string result(leading_zeroes, '\0');
  result.reserve(leading_zeroes + static_cast<size_t>(bytes.end() - first));
  for (; first != bytes.end(); ++first) {
    result.push_back(static_cast<char>(*first));
  }
  return result;
}

auto encode_z85(std::string_view input, int wrap)
    -> std::expected<std::string, std::string> {
  if (input.size() % 4 != 0) {
    return std::unexpected("invalid input length for z85 encoding");
  }

  std::string result;
  result.reserve((input.size() / 4) * 5);
  for (size_t i = 0; i < input.size(); i += 4) {
    uint32_t value = 0;
    for (size_t j = 0; j < 4; ++j) {
      value = (value << 8) | static_cast<unsigned char>(input[i + j]);
    }

    char encoded[5];
    for (int j = 4; j >= 0; --j) {
      encoded[j] = Z85_ALPHABET[value % 85];
      value /= 85;
    }
    result.append(encoded, 5);
  }
  return apply_wrap(result, wrap);
}

auto decode_z85(std::string_view input, bool ignore_garbage)
    -> std::expected<std::string, std::string> {
  std::array<int, 256> map;
  map.fill(-1);
  for (size_t i = 0; i < Z85_ALPHABET.size(); ++i) {
    map[static_cast<unsigned char>(Z85_ALPHABET[i])] = static_cast<int>(i);
  }

  std::string clean;
  clean.reserve(input.size());
  for (unsigned char c : input) {
    if (c == '\n' || c == '\r') continue;
    if (map[c] >= 0) {
      clean.push_back(static_cast<char>(c));
      continue;
    }
    if (!ignore_garbage) return std::unexpected("invalid input");
  }

  if (clean.size() % 5 != 0) {
    return std::unexpected("invalid input length for z85 decoding");
  }

  std::string result;
  result.reserve((clean.size() / 5) * 4);
  for (size_t i = 0; i < clean.size(); i += 5) {
    uint64_t value = 0;
    for (size_t j = 0; j < 5; ++j) {
      value = value * 85 + static_cast<uint32_t>(
                               map[static_cast<unsigned char>(clean[i + j])]);
    }
    if (value > std::numeric_limits<uint32_t>::max()) {
      return std::unexpected("invalid input");
    }

    for (int shift = 24; shift >= 0; shift -= 8) {
      result.push_back(static_cast<char>((value >> shift) & 0xff));
    }
  }
  return result;
}

auto parse_legacy_selector(std::string_view selector)
    -> std::expected<Encoding, std::string> {
  if (selector == "64" || selector == "base64") return Encoding::Base64;
  if (selector == "64url" || selector == "base64url")
    return Encoding::Base64Url;
  if (selector == "58" || selector == "base58") return Encoding::Base58;
  if (selector == "32" || selector == "base32") return Encoding::Base32;
  if (selector == "32hex" || selector == "base32hex")
    return Encoding::Base32Hex;
  if (selector == "16" || selector == "base16" || selector == "hex")
    return Encoding::Base16;
  if (selector == "2" || selector == "base2" || selector == "base2msbf" ||
      selector == "bin") {
    return Encoding::Base2Msbf;
  }
  if (selector == "base2lsbf") return Encoding::Base2Lsbf;
  if (selector == "z85") return Encoding::Z85;

  return std::unexpected("invalid encoding '" + std::string(selector) + "'");
}

auto build_config(const CommandContext<BASENC_OPTIONS.size()>& ctx)
    -> std::expected<Config, std::string> {
  Config cfg;
  cfg.decode = ctx.get<bool>("--decode", false) || ctx.get<bool>("-d", false);
  cfg.ignore_garbage =
      ctx.get<bool>("--ignore-garbage", false) || ctx.get<bool>("-i", false);
  cfg.wrap = ctx.get<int>("--wrap", 76);

  if (cfg.wrap < 0) return std::unexpected("invalid wrap size");

  int selector_count = 0;
  auto select = [&](Encoding encoding) {
    cfg.encoding = encoding;
    ++selector_count;
  };

  if (ctx.has("-b")) {
    auto parsed = parse_legacy_selector(ctx.get<std::string>("-b", ""));
    if (!parsed) return std::unexpected(parsed.error());
    select(*parsed);
  }
  if (ctx.get<bool>("--base64", false)) select(Encoding::Base64);
  if (ctx.get<bool>("--base64url", false)) select(Encoding::Base64Url);
  if (ctx.get<bool>("--base58", false)) select(Encoding::Base58);
  if (ctx.get<bool>("--base32", false)) select(Encoding::Base32);
  if (ctx.get<bool>("--base32hex", false)) select(Encoding::Base32Hex);
  if (ctx.get<bool>("--base16", false)) select(Encoding::Base16);
  if (ctx.get<bool>("--base2lsbf", false)) select(Encoding::Base2Lsbf);
  if (ctx.get<bool>("--base2msbf", false)) select(Encoding::Base2Msbf);
  if (ctx.get<bool>("--z85", false)) select(Encoding::Z85);

  if (selector_count == 0) return std::unexpected("missing encoding type");
  if (selector_count > 1) {
    return std::unexpected("multiple encoding options specified");
  }

  SmallVector<std::string, 16> files;
  for (auto arg : ctx.positionals) {
    std::string file_arg(arg);
    if (contains_wildcard(file_arg)) {
      auto glob_result = glob_expand(file_arg);
      if (glob_result.expanded) {
        for (const auto& file : glob_result.files) {
          files.push_back(wstring_to_utf8(file));
        }
        continue;
      }
    }
    files.push_back(file_arg);
  }

  if (files.size() > 1) {
    return std::unexpected("extra operand '" + files[1] + "'");
  }
  if (!files.empty()) cfg.file = files[0];

  return cfg;
}

auto process(const Config& cfg, std::string_view input)
    -> std::expected<std::string, std::string> {
  switch (cfg.encoding) {
    case Encoding::Base64:
      if (cfg.decode)
        return decode_base64(input, BASE64_ALPHABET, cfg.ignore_garbage);
      return encode_base64(input, BASE64_ALPHABET, cfg.wrap);
    case Encoding::Base64Url:
      if (cfg.decode)
        return decode_base64(input, BASE64URL_ALPHABET, cfg.ignore_garbage);
      return encode_base64(input, BASE64URL_ALPHABET, cfg.wrap);
    case Encoding::Base32:
      if (cfg.decode)
        return decode_base32(input, BASE32_ALPHABET, cfg.ignore_garbage);
      return encode_base32(input, BASE32_ALPHABET, cfg.wrap);
    case Encoding::Base32Hex:
      if (cfg.decode)
        return decode_base32(input, BASE32HEX_ALPHABET, cfg.ignore_garbage);
      return encode_base32(input, BASE32HEX_ALPHABET, cfg.wrap);
    case Encoding::Base16:
      if (cfg.decode) return decode_base16(input, cfg.ignore_garbage);
      return encode_base16(input, cfg.wrap);
    case Encoding::Base2Lsbf:
      if (cfg.decode) return decode_base2(input, true, cfg.ignore_garbage);
      return encode_base2(input, true, cfg.wrap);
    case Encoding::Base2Msbf:
      if (cfg.decode) return decode_base2(input, false, cfg.ignore_garbage);
      return encode_base2(input, false, cfg.wrap);
    case Encoding::Base58:
      if (cfg.decode) return decode_base58(input, cfg.ignore_garbage);
      return encode_base58(input, cfg.wrap);
    case Encoding::Z85:
      if (cfg.decode) return decode_z85(input, cfg.ignore_garbage);
      return encode_z85(input, cfg.wrap);
  }

  return std::unexpected("invalid encoding");
}

auto run(const Config& cfg) -> int {
  auto content_result = read_input(cfg.file);
  if (!content_result) {
    safeErrorPrintLn("basenc: " + content_result.error());
    return 1;
  }

  auto output = process(cfg, *content_result);
  if (!output) {
    safeErrorPrintLn("basenc: " + output.error());
    return 1;
  }

  if (cfg.decode) {
    safePrint(*output);
  } else {
    if (!output->empty()) output->push_back('\n');
    safePrint(*output);
  }

  return 0;
}

}  // namespace basenc_pipeline

REGISTER_COMMAND(
    basenc, "basenc", "basenc ENCODING [OPTION]... [FILE]",
    "Encode or decode FILE, or standard input, using the selected encoding.\n"
    "With no FILE, or when FILE is -, read standard input. ENCODING is one of\n"
    "--base64, --base64url, --base32, --base32hex, --base16, --base2lsbf,\n"
    "--base2msbf, --base58, or --z85.",
    "  basenc --base64 file.txt\n"
    "  echo 'Hello' | basenc --base64url\n"
    "  basenc --base32 -d encoded.txt\n"
    "  basenc --base16 file.bin",
    "base64(1), base32(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
    BASENC_OPTIONS) {
  using namespace basenc_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    safeErrorPrintLn("basenc: " + cfg_result.error());
    if (cfg_result.error().starts_with("extra operand '")) {
      safeErrorPrintLn("Try 'basenc --help' for more information.");
    }
    return 1;
  }

  return run(*cfg_result);
}
