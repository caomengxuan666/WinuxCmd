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
 *  - File: cksum.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for cksum with GNU parity options.
/// @Version: 0.2.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "pch/pch.h"
// include other header after pch.h
#include <wincrypt.h>

#include "core/command_macros.h"

#pragma comment(lib, "advapi32.lib")

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr CKSUM_OPTIONS = std::array{
    // [GNU]
    OPTION("-a", "--algorithm",
           "select the digest type to use. TYPE is sysv, bsd, or crc (default)",
           STRING_TYPE),
    // [GNU]
    OPTION("-c", "--check", "read and verify checksums from FILE", STRING_TYPE),
    // [GNU]
    OPTION("", "--ignore-missing",
           "don't fail or report status for missing files"),
    // [GNU]
    OPTION("-q", "--quiet",
           "don't print OK for each successfully verified file"),
    // [GNU]
    OPTION("-s", "--status",
           "don't output anything, status code shows success"),
    // [GNU]
    OPTION("-w", "--warn", "warn about improperly formatted checksum lines"),
    // [GNU]
    OPTION("", "--strict",
           "exit non-zero for improperly formatted checksum lines"),
    // [GNU]
    OPTION("", "--tag",
           "create a BSD-style checksum (the default for compatibility)"),
    // [GNU]
    OPTION("", "--untagged",
           "create a reversed style checksum, without digest type"),
    // [GNU]
    OPTION("-z", "--zero",
           "end each output line with NUL, not newline, and disable file name "
           "escaping"),
    // [GNU]
    OPTION("", "--raw", "print a raw binary digest, not hexadecimal"),
    // [GNU]
    OPTION("", "--base64", "print a base64-encoded digest"),
    // [GNU]
    OPTION("-l", "--length",
           "digest length in bits; must not exceed the maximum for the blake2 "
           "algorithm and must be a multiple of 8",
           STRING_TYPE),
    // [GNU]
    OPTION("", "--debug", "print debug output"),
    // [GNU] -r: create a reversed style checksum, without digest type
    OPTION("-r", "", "create a reversed style checksum, without digest type")};

namespace cksum_pipeline {
namespace cp = core::pipeline;

enum class Algorithm { CRC, SYSV, BSD };

struct Config {
  Algorithm algorithm = Algorithm::CRC;
  bool check_mode = false;
  bool ignore_missing = false;
  bool quiet = false;
  bool status = false;
  bool warn = false;
  bool strict = false;
  bool tag_mode = false;
  bool untagged = false;
  bool zero_terminated = false;
  bool raw_output = false;
  bool base64_output = false;
  bool debug = false;
  int digest_length = 0;  // 0 = default for algorithm
  std::string check_file;
  SmallVector<std::string, 64> files;
};

auto build_config(const CommandContext<CKSUM_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;

  auto algo = ctx.get<std::string>("--algorithm", "");
  if (algo.empty()) algo = ctx.get<std::string>("-a", "");
  if (!algo.empty()) {
    if (algo == "sysv" || algo == "SYSV") {
      cfg.algorithm = Algorithm::SYSV;
    } else if (algo == "bsd" || algo == "BSD") {
      cfg.algorithm = Algorithm::BSD;
    } else if (algo == "crc" || algo == "CRC") {
      cfg.algorithm = Algorithm::CRC;
    } else {
      return std::unexpected("invalid argument '" + algo +
                             "' for '--algorithm'\n"
                             "Valid arguments are:\n"
                             "  - 'sysv'\n"
                             "  - 'bsd'\n"
                             "  - 'crc'");
    }
  }

  auto check = ctx.get<std::string>("--check", "");
  if (check.empty()) check = ctx.get<std::string>("-c", "");
  if (!check.empty()) {
    cfg.check_mode = true;
    cfg.check_file = check;
  }

  cfg.ignore_missing = ctx.has("--ignore-missing");
  cfg.quiet = ctx.has("--quiet") || ctx.has("-q");
  cfg.status = ctx.has("--status") || ctx.has("-s");
  cfg.warn = ctx.has("--warn") || ctx.has("-w");
  cfg.strict = ctx.has("--strict");
  cfg.tag_mode = ctx.has("--tag");
  cfg.untagged = ctx.has("--untagged") || ctx.has("-r");
  cfg.zero_terminated = ctx.has("--zero") || ctx.has("-z");
  cfg.raw_output = ctx.has("--raw");
  cfg.base64_output = ctx.has("--base64");
  cfg.debug = ctx.has("--debug");

  auto length_str = ctx.get<std::string>("--length", "");
  if (length_str.empty()) {
    length_str = ctx.get<std::string>("-l", "");
  }
  if (!length_str.empty()) {
    auto [ptr, ec] = std::from_chars(length_str.data(),
                                     length_str.data() + length_str.size(),
                                     cfg.digest_length);
    if (ec != std::errc() || ptr != length_str.data() + length_str.size() ||
        cfg.digest_length <= 0 || cfg.digest_length % 8 != 0) {
      return std::unexpected("invalid length");
    }
  }

  if (cfg.tag_mode && cfg.untagged) {
    return std::unexpected("--tag and --untagged are mutually exclusive");
  }

  for (auto arg : ctx.positionals) {
    std::string file_arg(arg);
    for (const auto& file : expand_file_operand(file_arg)) {
      cfg.files.push_back(file);
    }
  }

  if (cfg.files.empty() && !cfg.check_mode) {
    cfg.files.push_back("-");
  }

  return cfg;
}

struct FileData {
  std::vector<unsigned char> data;
  uint64_t byte_count = 0;
};

auto input_open_error(std::string_view path) -> std::string {
  std::error_code ec;
  if (std::filesystem::is_directory(std::filesystem::u8path(path), ec) && !ec) {
    return "cannot open '" + std::string(path) +
           "' for reading: Is a directory";
  }

  return "cannot open '" + std::string(path) +
         "' for reading: No such file or directory";
}

auto read_file(const std::string& filename) -> cp::Result<FileData> {
  FileData fd;

  if (filename == "-" || filename.empty()) {
    fd.data.assign(std::istreambuf_iterator<char>(std::cin),
                   std::istreambuf_iterator<char>());
    if (std::cin.fail() && !std::cin.eof()) {
      return std::unexpected("error reading from standard input");
    }
  } else {
    std::ifstream f(native_path::normalize_api_operand(filename),
                    std::ios::binary);
    if (!f) {
      return std::unexpected(input_open_error(filename));
    }
    fd.data.assign(std::istreambuf_iterator<char>(f),
                   std::istreambuf_iterator<char>());
    if (f.fail() && !f.eof()) {
      return std::unexpected(std::string("error reading '") + filename + "'");
    }
  }

  fd.byte_count = fd.data.size();
  return fd;
}

auto read_crc_file(const std::string& filename)
    -> cp::Result<portable_digest::PosixCksumResult> {
  std::istream* input = &std::cin;
  std::ifstream file;
  if (filename != "-" && !filename.empty()) {
    file.open(
        std::filesystem::u8path(native_path::normalize_api_operand(filename)),
        std::ios::binary);
    if (!file) {
      return std::unexpected(input_open_error(filename));
    }
    input = &file;
  }

  auto result = portable_digest::posix_cksum_stream(*input);
  if (!result) {
    if (filename == "-" || filename.empty()) {
      return std::unexpected("error reading from standard input");
    }
    return std::unexpected(std::string("error reading '") + filename + "'");
  }
  return *result;
}

auto calculate_sysv(const std::vector<unsigned char>& data)
    -> std::pair<uint32_t, uint32_t> {
  // GNU sysv cksum: sum of all bytes mod 2^32, plus byte count in 512-byte
  // blocks
  uint32_t checksum = 0;
  for (unsigned char byte : data) {
    checksum = (checksum >> 1) + ((checksum & 1) << 15);
    checksum += byte;
    checksum &= 0xFFFF;
  }
  uint32_t blocks =
      static_cast<uint32_t>((data.size() + 511) / 512);  // ceil division
  return {checksum, blocks};
}

auto calculate_bsd(const std::vector<unsigned char>& data)
    -> std::pair<uint32_t, uint32_t> {
  // GNU bsd cksum: 16-bit checksum with rotate, plus byte count in 1024-byte
  // blocks
  uint32_t checksum = 0;
  for (unsigned char byte : data) {
    checksum = (checksum >> 1) + ((checksum & 1) << 15);
    checksum += byte;
    checksum &= 0xFFFF;
  }
  uint32_t blocks =
      static_cast<uint32_t>((data.size() + 1023) / 1024);  // ceil division
  return {checksum, blocks};
}

auto to_hex(const std::vector<unsigned char>& data) -> std::string {
  std::string result;
  result.reserve(data.size() * 2);
  for (unsigned char byte : data) {
    char buf[3];
    snprintf(buf, sizeof(buf), "%02x", byte);
    result += buf;
  }
  return result;
}

auto to_base64(const std::vector<unsigned char>& data) -> std::string {
  static constexpr char table[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string result;
  result.reserve((data.size() + 2) / 3 * 4);
  for (size_t i = 0; i < data.size(); i += 3) {
    unsigned int n = static_cast<unsigned int>(data[i]) << 16;
    if (i + 1 < data.size()) n |= static_cast<unsigned int>(data[i + 1]) << 8;
    if (i + 2 < data.size()) n |= static_cast<unsigned int>(data[i + 2]);
    result += table[(n >> 18) & 0x3F];
    result += table[(n >> 12) & 0x3F];
    result += (i + 1 < data.size()) ? table[(n >> 6) & 0x3F] : '=';
    result += (i + 2 < data.size()) ? table[n & 0x3F] : '=';
  }
  return result;
}

// Convert raw CRC32 bytes for raw/base64 output
auto crc32_to_bytes(uint32_t crc) -> std::vector<unsigned char> {
  return {static_cast<unsigned char>((crc >> 24) & 0xFF),
          static_cast<unsigned char>((crc >> 16) & 0xFF),
          static_cast<unsigned char>((crc >> 8) & 0xFF),
          static_cast<unsigned char>(crc & 0xFF)};
}

// Parse a hex string to bytes
auto hex_to_bytes(const std::string& hex) -> std::vector<unsigned char> {
  auto hex_value = [](char ch) -> unsigned char {
    if (ch >= '0' && ch <= '9') return static_cast<unsigned char>(ch - '0');
    if (ch >= 'a' && ch <= 'f') {
      return static_cast<unsigned char>(ch - 'a' + 10);
    }
    if (ch >= 'A' && ch <= 'F') {
      return static_cast<unsigned char>(ch - 'A' + 10);
    }
    return 0;
  };

  std::vector<unsigned char> bytes;
  for (size_t i = 0; i + 1 < hex.size(); i += 2) {
    bytes.push_back(static_cast<unsigned char>((hex_value(hex[i]) << 4) |
                                               hex_value(hex[i + 1])));
  }
  return bytes;
}

auto output_digest(const Config& cfg, const std::string& digest_hex,
                   uint32_t checksum_or_crc, uint64_t byte_count,
                   const std::string& filename, Algorithm algo) -> void {
  char sep = cfg.zero_terminated ? '\0' : '\n';

  if (cfg.raw_output) {
    // Raw binary digest
    auto bytes = hex_to_bytes(digest_hex);
    fwrite(bytes.data(), 1, bytes.size(), stdout);
    return;
  }

  if (cfg.base64_output) {
    auto bytes = hex_to_bytes(digest_hex);
    safePrint(to_base64(bytes));
    if (filename != "-") {
      safePrint("  ");
      safePrint(filename);
    }
    safePrint(std::string(1, sep));
    return;
  }

  if (cfg.untagged) {
    // Reversed style: CHECKSUM BYTES FILENAME
    safePrint(std::to_string(checksum_or_crc));
    safePrint(" ");
    safePrint(std::to_string(byte_count));
    if (filename != "-") {
      safePrint(" ");
      safePrint(filename);
    }
    safePrint(std::string(1, sep));
    return;
  }

  if (algo == Algorithm::CRC) {
    if (cfg.tag_mode) {
      safePrint("CRC32");
      if (filename != "-") {
        safePrint(" (");
        safePrint(filename);
        safePrint(")");
      }
      safePrint(" = ");
      safePrint(digest_hex);
    } else {
      safePrint(std::to_string(checksum_or_crc));
      safePrint(" ");
      safePrint(std::to_string(byte_count));
      if (filename != "-") {
        safePrint(" ");
        safePrint(filename);
      }
    }
  } else if (algo == Algorithm::SYSV) {
    if (cfg.tag_mode) {
      safePrint("SYSV");
      if (filename != "-") {
        safePrint(" (");
        safePrint(filename);
        safePrint(")");
      }
      safePrint(" = ");
      safePrint(digest_hex);
    } else {
      safePrint(std::to_string(checksum_or_crc));
      safePrint(" ");
      safePrint(std::to_string(byte_count));
      if (filename != "-") {
        safePrint(" ");
        safePrint(filename);
      }
    }
  } else if (algo == Algorithm::BSD) {
    if (cfg.tag_mode) {
      safePrint("BSD");
      if (filename != "-") {
        safePrint(" (");
        safePrint(filename);
        safePrint(")");
      }
      safePrint(" = ");
      safePrint(digest_hex);
    } else {
      safePrint(std::to_string(checksum_or_crc));
      safePrint(" ");
      safePrint(std::to_string(byte_count));
      if (filename != "-") {
        safePrint(" ");
        safePrint(filename);
      }
    }
  }

  safePrint(std::string(1, sep));
}

auto run_check_mode(const Config& cfg) -> int {
  std::istream* input = &std::cin;
  std::ifstream file;
  std::string input_name = "standard input";
  if (!cfg.check_file.empty() && cfg.check_file != "-") {
    file.open(native_path::normalize_api_operand(cfg.check_file));
    if (!file) {
      safeErrorPrint("cksum: " + input_open_error(cfg.check_file) + "\n");
      return 1;
    }
    input = &file;
    input_name = cfg.check_file;
  }

  int total = 0;
  int checked = 0;
  int malformed = 0;
  int unreadable = 0;
  int mismatches = 0;
  std::string line;
  size_t line_number = 0;
  while (std::getline(*input, line)) {
    ++line_number;
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    if (line.empty()) continue;
    ++total;

    // Parse line: expect format like "CRC32 (filename) = hexdigest"
    // or "checksum bytes filename"
    std::string expected_digest, filename;
    std::optional<uint64_t> expected_bytes;

    size_t eq_pos = line.find(" = ");
    if (eq_pos != std::string::npos) {
      // Tag format: ALGO (FILE) = DIGEST
      filename = line.substr(0, eq_pos);
      expected_digest = line.substr(eq_pos + 3);
      // Strip "ALGO (" prefix and ")" suffix from filename
      size_t paren_start = filename.find(" (");
      if (paren_start != std::string::npos) {
        filename = filename.substr(paren_start + 2);
      }
      if (!filename.empty() && filename.back() == ')') {
        filename.pop_back();
      }
    } else {
      // Traditional format: checksum bytes filename
      std::istringstream iss(line);
      std::string checksum_str, bytes_str;
      if (iss >> checksum_str >> bytes_str) {
        // The rest is the filename
        std::getline(iss >> std::ws, filename);
        expected_digest = checksum_str;
        unsigned long long parsed_bytes = 0;
        auto [ptr, ec] =
            std::from_chars(bytes_str.data(),
                            bytes_str.data() + bytes_str.size(), parsed_bytes);
        if (ec == std::errc() && ptr == bytes_str.data() + bytes_str.size()) {
          expected_bytes = parsed_bytes;
        } else {
          expected_bytes.reset();
          filename.clear();
          expected_digest.clear();
        }
      }
    }

    if (filename.empty() || expected_digest.empty()) {
      if (cfg.warn && !cfg.status) {
        safeErrorPrint("cksum: " + input_name + ": " +
                       std::to_string(line_number) +
                       ": improperly formatted checksum line\n");
      }
      ++malformed;
      continue;
    }

    ++checked;

    if (cfg.ignore_missing) {
      std::error_code ec;
      if (!filename.empty() && filename != "-" &&
          !std::filesystem::exists(std::filesystem::u8path(filename), ec)) {
        continue;
      }
    }

    auto crc_result = read_crc_file(filename);
    if (!crc_result) {
      if (!cfg.status) {
        safeErrorPrint("cksum: ");
        safeErrorPrint(std::string(crc_result.error()));
        safeErrorPrint("\n");
      }
      ++unreadable;
      continue;
    }

    uint32_t crc = crc_result->checksum;
    char crc_hex[9];
    snprintf(crc_hex, sizeof(crc_hex), "%08x", crc);

    bool digest_matches =
        expected_digest == crc_hex || expected_digest == std::to_string(crc);
    bool size_matches =
        !expected_bytes.has_value() || *expected_bytes == crc_result->bytes;

    if (digest_matches && size_matches) {
      if (!cfg.zero_terminated && !cfg.quiet && !cfg.status) {
        safePrint(filename + ": OK\n");
      }
    } else {
      if (!cfg.zero_terminated && !cfg.status) {
        safePrint(filename + ": FAILED\n");
      }
      ++mismatches;
    }
  }

  if (checked == 0 && total > 0) {
    if (!cfg.status) {
      safeErrorPrint("cksum: " + input_name +
                     ": no properly formatted checksum lines found\n");
    }
    return 1;
  }

  if (!cfg.status && cfg.warn) {
    if (malformed == 1) {
      safeErrorPrint("cksum: WARNING: 1 line is improperly formatted\n");
    } else if (malformed > 1) {
      safeErrorPrint("cksum: WARNING: " + std::to_string(malformed) +
                     " lines are improperly formatted\n");
    }
  }

  if (!cfg.status) {
    if (unreadable > 0) {
      if (unreadable == 1) {
        safeErrorPrint("cksum: WARNING: 1 listed file could not be read\n");
      } else {
        safeErrorPrint("cksum: WARNING: " + std::to_string(unreadable) +
                       " listed files could not be read\n");
      }
    }
  }

  if (mismatches > 0) {
    int computed = checked - unreadable;
    if (!cfg.status) {
      safeErrorPrint("cksum: WARNING: " + std::to_string(mismatches) + " of " +
                     std::to_string(computed) +
                     " computed checksums did NOT match\n");
    }
    return 1;
  }

  if (cfg.strict && malformed > 0) {
    return 1;
  }

  if (unreadable > 0) {
    return 1;
  }

  return 0;
}

auto run(const Config& cfg) -> int {
  if (cfg.check_mode) {
    return run_check_mode(cfg);
  }

  bool all_ok = true;
  for (const auto& file : cfg.files) {
    if (cfg.algorithm == Algorithm::CRC) {
      auto crc_result = read_crc_file(file);
      if (!crc_result) {
        cp::report_error(crc_result, L"cksum");
        all_ok = false;
        continue;
      }

      if (cfg.debug) {
        safeErrorPrint("cksum: algorithm: crc\n");
        safeErrorPrint("cksum: file: " + file + "\n");
        safeErrorPrint("cksum: bytes: " + std::to_string(crc_result->bytes) +
                       "\n");
      }

      char buf[9];
      snprintf(buf, sizeof(buf), "%08x", crc_result->checksum);
      output_digest(cfg, buf, crc_result->checksum, crc_result->bytes, file,
                    cfg.algorithm);
      continue;
    }

    auto file_data = read_file(file);
    if (!file_data) {
      cp::report_error(file_data, L"cksum");
      all_ok = false;
      continue;
    }

    if (cfg.debug) {
      safeErrorPrint("cksum: algorithm: ");
      switch (cfg.algorithm) {
        case Algorithm::CRC:
          safeErrorPrint("crc");
          break;
        case Algorithm::SYSV:
          safeErrorPrint("sysv");
          break;
        case Algorithm::BSD:
          safeErrorPrint("bsd");
          break;
      }
      safeErrorPrint("\n");
      safeErrorPrint("cksum: file: " + file + "\n");
      safeErrorPrint("cksum: bytes: " + std::to_string(file_data->byte_count) +
                     "\n");
    }

    std::string digest_hex;
    uint32_t display_value = 0;
    uint64_t display_bytes = file_data->byte_count;

    switch (cfg.algorithm) {
      case Algorithm::SYSV: {
        auto [checksum, blocks] = calculate_sysv(file_data->data);
        display_value = checksum;
        display_bytes = blocks;
        // SYSV uses 4-byte big-endian for tag format
        uint32_t val = checksum;
        char buf[9];
        snprintf(buf, sizeof(buf), "%08x", val);
        digest_hex = buf;
        break;
      }
      case Algorithm::BSD: {
        auto [checksum, blocks] = calculate_bsd(file_data->data);
        display_value = checksum;
        display_bytes = blocks;
        uint32_t val = checksum;
        char buf[9];
        snprintf(buf, sizeof(buf), "%08x", val);
        digest_hex = buf;
        break;
      }
      case Algorithm::CRC:
        std::unreachable();
    }

    output_digest(cfg, digest_hex, display_value, display_bytes, file,
                  cfg.algorithm);
  }

  return all_ok ? 0 : 1;
}

}  // namespace cksum_pipeline

REGISTER_COMMAND(
    cksum, "cksum", "cksum [OPTION]... [FILE]...",
    "Print CRC (default), SYSV, or BSD checksum and byte counts of each "
    "FILE.\n"
    "\n"
    "With no FILE, or when FILE is -, read standard input.\n"
    "\n"
    "  -a, --algorithm=TYPE  select the digest type: sysv, bsd, or crc "
    "(default)\n"
    "  -c, --check           read and verify checksums from FILE\n"
    "      --ignore-missing  don't fail or report status for missing files\n"
    "  -q, --quiet           don't print OK for each successfully verified "
    "file\n"
    "  -s, --status          don't output anything, status code shows "
    "success\n"
    "  -w, --warn            warn about improperly formatted checksum lines\n"
    "      --strict          exit non-zero for improperly formatted checksum "
    "lines\n"
    "      --tag             create a BSD-style checksum\n"
    "      --untagged        create a reversed style checksum\n"
    "  -z, --zero            end each output line with NUL\n"
    "      --raw             print a raw binary digest\n"
    "      --base64          print a base64-encoded digest\n"
    "      --length=BITS     digest length in bits\n"
    "      --debug           print debug output",
    "  cksum file.txt\n"
    "  cksum -a sysv file.txt\n"
    "  cksum -a bsd file.txt\n"
    "  cksum --tag file.txt\n"
    "  cksum -c checksums.txt\n"
    "  echo \"test\" | cksum",
    "md5sum(1), sha1sum(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
    CKSUM_OPTIONS) {
  using namespace cksum_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"cksum");
    return 1;
  }

  return run(*cfg_result);
}
