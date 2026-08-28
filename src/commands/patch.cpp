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
 *  - File: patch.cpp
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for patch command.
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

// ======================================================
// Options (constexpr)
// ======================================================

auto constexpr PATCH_OPTIONS = std::array{
    // [GNU] -p defaults to 0, consistent with GNU patch.
    OPTION("-p", "", "strip NUM leading components from file names", INT_TYPE),
    // [GNU] -p, --strip=NUM: strip NUM leading components from file names
    // [GNU] -i, --input=FILE: read patch from FILE
    // [GNU] -R, --reverse: assume patch was created with old and new swapped
    // [GNU] -N, --forward: skip patches that seem to be reversed or already
    // applied
    // [GNU] -b, --backup: make backup files
    // [GNU] --dry-run: don't actually change any files
    // [GNU] -d, --directory=DIR: change to DIR first
    // [GNU] -D, --ifdef=WORD: use WORD to patch files
    // [GNU] -E, --remove-empty-files: remove empty output files
    // [GNU] -f, --force: force this patch, even if it seems reversed
    // [GNU] -F, --fuzz=NUM: set maximum fuzz factor (default 2)
    // [GNU] --forward: skip patches that seem reversed or already applied
    // [GNU] -l, --merge: merge using merge program
    // [GNU] -o, --output=FILE: output to FILE instead of stdout
    // [GNU] --backup-if-mismatch: backup if patch does not match exactly
    // [GNU] --no-backup-if-mismatch: don't backup if patch matches exactly
    // [GNU] -r, --reject-file=FILE: output rejects to FILE
    // [GNU] --reject-format=FORMAT: produce output in FORMAT
    // [GNU] -s, --silent, --quiet: work silently unless an error occurs
    // [GNU] -t, --batch: same as --force, with diagnostics
    // [GNU] -T, --set-time: set the patch's modification time
    // [GNU] --set-utc: set the patch's modification time to UTC
    // [GNU] -u, --unified: interpret the patch as unified diff
    // [GNU] -v, --verbose: print verbose output
    // [GNU] --binary: read and write in binary mode
    // [GNU] --posix: conform to POSIX standard
    // [GNU] --quoting-style=WORD: output file names using WORD style
    // [GNU] --strip-trailing-slashes: strip trailing slashes from file names
    OPTION("-i", "", "read patch from FILE", STRING_TYPE),
    OPTION("-R", "--reverse",
           "assume patch was created with old and new files swapped"),
    OPTION("-N", "--forward",
           "assume patch was created with old and new files swapped"),
    OPTION("-b", "--backup", "back up the original file"),
    OPTION("", "--dry-run", "do not actually change any files"),
    OPTION("-d", "--directory", "change to DIR first", STRING_TYPE),
    OPTION("-D", "--ifdef", "use WORD to patch files", STRING_TYPE),
    OPTION("", "--remove-empty-files", "remove empty output files"),
    OPTION("-f", "--force", "force this patch, even if it seems reversed"),
    OPTION("-F", "--fuzz", "set maximum fuzz factor", INT_TYPE),
    OPTION("-l", "--merge", "merge using merge program"),
    OPTION("-o", "--output", "output to FILE instead of stdout", STRING_TYPE),
    OPTION("", "--backup-if-mismatch",
           "backup if patch does not match exactly"),
    OPTION("", "--no-backup-if-mismatch", "don't backup if patch matches"),
    OPTION("-r", "--reject-file", "output rejects to FILE", STRING_TYPE),
    OPTION("", "--reject-format", "produce output in FORMAT", STRING_TYPE),
    OPTION("-s", "--silent", "work silently unless an error occurs"),
    OPTION("-q", "--quiet", "work silently unless an error occurs"),
    OPTION("-t", "--batch", "same as --force, with diagnostics"),
    OPTION("-T", "--set-time", "set the patch's modification time"),
    OPTION("", "--set-utc", "set the patch's modification time to UTC"),
    OPTION("-u", "--unified", "interpret the patch as unified diff"),
    OPTION("-v", "--verbose", "print verbose output"),
    OPTION("", "--binary", "read and write in binary mode"),
    OPTION("", "--posix", "conform to POSIX standard")};

// ======================================================
// Helper functions
// ======================================================

namespace {
// Parse hunk header
struct Hunk {
  int old_start = 0;
  int old_count = 0;
  int new_start = 0;
  int new_count = 0;
  std::vector<std::string> old_lines;
  std::vector<std::string> new_lines;
};

// Write lines to file
bool write_file_lines(const std::string& filename,
                      const std::vector<std::string>& lines) {
  std::wstring wfilename = utf8_to_wstring(filename);
  HANDLE hFile = CreateFileW(wfilename.c_str(), GENERIC_WRITE, 0, nullptr,
                             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (hFile == INVALID_HANDLE_VALUE) {
    return false;
  }

  std::string content;
  for (const auto& line : lines) {
    content += line + "\n";
  }

  DWORD bytesWritten;
  WriteFile(hFile, content.data(), static_cast<DWORD>(content.size()),
            &bytesWritten, nullptr);
  CloseHandle(hFile);

  return true;
}

// Parse unified diff hunk
bool parse_hunk(const std::string& line, Hunk& hunk) {
  if (line.length() < 4 || line.substr(0, 2) != "@@") {
    return false;
  }

  // Parse @@ -old_start,old_count +new_start,new_count @@
  size_t at_pos = line.find("@@", 2);
  if (at_pos == std::string::npos) return false;

  std::string hunk_info = line.substr(3, at_pos - 3);
  size_t plus_pos = hunk_info.find('+');
  if (plus_pos == std::string::npos) return false;

  std::string old_part = hunk_info.substr(0, plus_pos);
  std::string new_part = hunk_info.substr(plus_pos + 1);

  // Trim whitespace
  while (!old_part.empty() &&
         std::isspace(static_cast<unsigned char>(old_part.back()))) {
    old_part.pop_back();
  }
  while (!new_part.empty() &&
         std::isspace(static_cast<unsigned char>(new_part.back()))) {
    new_part.pop_back();
  }

  // Parse old_start and old_count
  size_t comma_pos = old_part.find(',');
  if (comma_pos == std::string::npos) {
    try {
      hunk.old_start = std::stoi(old_part);
      hunk.old_count = 1;
    } catch (...) {
      return false;
    }
  } else {
    try {
      hunk.old_start = std::stoi(old_part.substr(0, comma_pos));
      hunk.old_count = std::stoi(old_part.substr(comma_pos + 1));
    } catch (...) {
      return false;
    }
  }

  // Convert to absolute values (patch format uses - to indicate old file)
  if (hunk.old_start < 0) hunk.old_start = -hunk.old_start;
  if (hunk.old_count < 0) hunk.old_count = -hunk.old_count;

  // Parse new_start and new_count
  comma_pos = new_part.find(',');
  if (comma_pos == std::string::npos) {
    try {
      hunk.new_start = std::stoi(new_part);
      hunk.new_count = 1;
    } catch (...) {
      return false;
    }
  } else {
    try {
      hunk.new_start = std::stoi(new_part.substr(0, comma_pos));
      hunk.new_count = std::stoi(new_part.substr(comma_pos + 1));
    } catch (...) {
      return false;
    }
  }

  return true;
}

// Apply a hunk, allowing GNU-style context fuzz around its edges.
bool apply_hunk(std::vector<std::string>& lines, const Hunk& source,
                bool reverse, int fuzz, int& used_offset) {
  Hunk hunk = source;
  if (reverse) {
    std::swap(hunk.old_lines, hunk.new_lines);
    std::swap(hunk.old_count, hunk.new_count);
  }
  if (hunk.old_start <= 0 || fuzz < 0) return false;

  const int requested = hunk.old_start - 1;
  const int max_fuzz =
      std::min(fuzz, static_cast<int>(hunk.old_lines.size() / 2));
  for (int trim = 0; trim <= max_fuzz; ++trim) {
    const size_t match_count =
        hunk.old_lines.size() - static_cast<size_t>(trim * 2);
    for (int offset = -static_cast<int>(lines.size());
         offset <= static_cast<int>(lines.size()); ++offset) {
      const int candidate = requested + offset + trim;
      if (candidate < 0 || candidate + static_cast<int>(match_count) >
                               static_cast<int>(lines.size()))
        continue;
      bool match = true;
      for (size_t i = 0; i < match_count; ++i) {
        if (lines[static_cast<size_t>(candidate) + i] !=
            hunk.old_lines[static_cast<size_t>(trim) + i]) {
          match = false;
          break;
        }
      }
      if (!match) continue;
      auto begin = lines.begin() + candidate;
      lines.erase(begin, begin + static_cast<std::ptrdiff_t>(match_count));
      lines.insert(lines.begin() + candidate, hunk.new_lines.begin(),
                   hunk.new_lines.end());
      used_offset = offset;
      return true;
    }
  }
  return false;
}

// Backup file
bool backup_file(const std::string& filename, const std::string& backup_ext) {
  std::wstring wsrc = utf8_to_wstring(filename);
  std::wstring wdst = utf8_to_wstring(filename + backup_ext);

  return CopyFileW(wsrc.c_str(), wdst.c_str(), FALSE) != FALSE;
}

// Strip path components
std::string strip_path(const std::string& path, int components) {
  if (components <= 0) return path;

  size_t pos = 0;
  int count = 0;

  // Find position after the Nth path separator
  for (size_t i = 0; i < path.length() && count < components; ++i) {
    if (path[i] == '/' || path[i] == '\\') {
      count++;
      pos = i + 1;  // Position after separator
    }
  }

  return (pos < path.length()) ? path.substr(pos) : path;
}
}  // namespace

// ======================================================
// Main command implementation
// ======================================================

REGISTER_COMMAND(
    patch,
    /* cmd_name */ "patch",
    /* cmd_synopsis */ "patch [OPTION]... [ORIGFILE [PATCHFILE]]",
    /* cmd_desc */
    "Apply a diff file to an original.\n"
    "Apply a diff file (patch) to an original file, creating a patched "
    "version.\n"
    "Supports unified diff format. Can also read patch from standard input.",
    /* examples */
    "  patch < changes.diff\n"
    "  patch -p1 < changes.diff\n"
    "  patch -i patch.txt original.c\n"
    "  patch -p0 -b .orig < changes.diff",
    /* see_also */ "diff, diff3",
    /* author */ "WinuxCmd",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */ PATCH_OPTIONS) {
  int strip_components = ctx.get<bool>("-p", false)
                             ? std::stoi(ctx.get<std::string>("-p", ""))
                             : 0;
  std::string patch_file = ctx.get<std::string>("-i", "");
  bool reverse =
      ctx.get<bool>("-R", false) || ctx.get<bool>("--reverse", false);
  const bool force =
      ctx.get<bool>("-f", false) || ctx.get<bool>("--force", false) ||
      ctx.get<bool>("-t", false) || ctx.get<bool>("--batch", false);
  const int fuzz =
      (ctx.has("-F") || ctx.has("--fuzz"))
          ? ctx.has("-F") ? ctx.get<int>("-F", 2) : ctx.get<int>("--fuzz", 2)
          : 2;
  constexpr const char* backup_ext = ".orig";
  std::string output_file = ctx.get<std::string>("-o", "");
  if (output_file.empty()) output_file = ctx.get<std::string>("--output", "");
  std::string reject_file = ctx.get<std::string>("-r", "");
  if (reject_file.empty())
    reject_file = ctx.get<std::string>("--reject-file", "");
  const bool silent =
      ctx.get<bool>("-s", false) || ctx.get<bool>("--silent", false) ||
      ctx.get<bool>("-q", false) || ctx.get<bool>("--quiet", false);
  bool dry_run = ctx.get<bool>("--dry-run", false);

  // [DIFFERS] Additional options for full runtime coverage.
  const bool forward = ctx.has("-N") || ctx.has("--forward");
  std::string directory = ctx.get<std::string>("-d", "");
  if (directory.empty()) directory = ctx.get<std::string>("--directory", "");
  const bool remove_empty = ctx.has("--remove-empty-files");
  std::string ifdef_word = ctx.get<std::string>("-D", "");
  if (ifdef_word.empty()) ifdef_word = ctx.get<std::string>("--ifdef", "");
  const bool merge_mode = ctx.has("-l") || ctx.has("--merge");
  const bool backup_if_mismatch = ctx.has("--backup-if-mismatch");
  const bool no_backup_if_mismatch = ctx.has("--no-backup-if-mismatch");
  const std::string reject_format = ctx.get<std::string>("--reject-format", "");
  const bool set_time = ctx.has("-T") || ctx.has("--set-time");
  const bool set_utc = ctx.has("--set-utc");
  const bool unified = ctx.has("-u") || ctx.has("--unified");
  const bool verbose = ctx.has("-v") || ctx.has("--verbose");
  const bool binary_mode = ctx.has("--binary");
  const bool posix_mode = ctx.has("--posix");

  // Options not supported on this Windows port
  if (!ifdef_word.empty()) {
    safeErrorPrintLn("patch: --ifdef/-D is not supported on Windows");
    return 1;
  }
  if (merge_mode) {
    safeErrorPrintLn("patch: --merge/-l is not supported on Windows");
    return 1;
  }
  if (!reject_format.empty()) {
    safeErrorPrintLn("patch: --reject-format is not supported on Windows");
    return 1;
  }
  if (set_time) {
    safeErrorPrintLn("patch: --set-time/-T is not supported on Windows");
    return 1;
  }
  if (set_utc) {
    safeErrorPrintLn("patch: --set-utc is not supported on Windows");
    return 1;
  }
  if (binary_mode) {
    safeErrorPrintLn("patch: --binary is not supported on Windows");
    return 1;
  }
  if (posix_mode) {
    safeErrorPrintLn("patch: --posix is not supported on Windows");
    return 1;
  }

  // --forward overrides --reverse ([DIFFERS] GNU behaviour).
  if (forward) {
    reverse = false;
  }

  // [DIFFERS] -u/--unified is accepted silently; this implementation always
  // parses unified diff format.

  if (verbose) {
    safePrintLn("patch: strip " + std::to_string(strip_components) +
                " leading components");
  }

  std::string patch_content;

  // Read patch file
  if (!patch_file.empty()) {
    std::wstring wfile = utf8_to_wstring(patch_file);
    HANDLE hFile =
        CreateFileW(wfile.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
      safeErrorPrintLn("patch: cannot open patch file '" + patch_file + "'");
      return 1;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(hFile, &fileSize);
    patch_content.resize(fileSize.QuadPart);
    DWORD bytesRead;
    ReadFile(hFile, patch_content.data(), static_cast<DWORD>(fileSize.QuadPart),
             &bytesRead, nullptr);
    CloseHandle(hFile);
  } else {
    if (verbose) {
      safePrintLn("patch: reading patch from standard input");
    }
    patch_content = std::string(std::istreambuf_iterator<char>(std::cin),
                                std::istreambuf_iterator<char>());
  }

  if (patch_content.empty()) {
    safeErrorPrintLn("patch: no patch input");
    return 1;
  }

  // Parse patch
  std::istringstream iss(patch_content);
  std::string line;
  std::string target_file;
  std::vector<Hunk> hunks;
  Hunk current_hunk;
  bool in_hunk = false;

  while (std::getline(iss, line)) {
    // Remove trailing CR
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    // Parse file header
    if (line.length() >= 4 && line.substr(0, 4) == "--- ") {
      // Old file
      size_t space = line.find(' ');
      if (space != std::string::npos && space < line.length()) {
        size_t next_space = line.find_first_of(" \t", space + 1);
        if (next_space != std::string::npos) {
          std::string file = line.substr(space + 1, next_space - space - 1);
          target_file = strip_path(file, strip_components);
        } else {
          // No second space, take everything after first space
          std::string file = line.substr(space + 1);
          target_file = strip_path(file, strip_components);
        }
      }
    } else if (line.length() >= 4 && line.substr(0, 4) == "+++ ") {
      // New file
      size_t space = line.find(' ');
      if (space != std::string::npos && space < line.length()) {
        size_t next_space = line.find_first_of(" \t", space + 1);
        if (next_space != std::string::npos) {
          std::string file = line.substr(space + 1, next_space - space - 1);
          if (target_file.empty()) {
            target_file = strip_path(file, strip_components);
          }
        } else {
          // No second space, take everything after first space
          std::string file = line.substr(space + 1);
          if (target_file.empty()) {
            target_file = strip_path(file, strip_components);
          }
        }
      }
    } else {
      Hunk parsed_hunk;
      if (!parse_hunk(line, parsed_hunk)) {
        if (in_hunk) {
          if (line.empty() || line[0] == ' ' || line[0] == '+' ||
              line[0] == '-') {
            if (line[0] == ' ' || line[0] == '-') {
              current_hunk.old_lines.push_back(line.substr(1));
            }
            if (line[0] == ' ' || line[0] == '+') {
              current_hunk.new_lines.push_back(line.substr(1));
            }
          } else {
            hunks.push_back(current_hunk);
            current_hunk = Hunk();
            in_hunk = false;
          }
        }
        continue;
      }
      if (in_hunk) {
        hunks.push_back(current_hunk);
      }
      current_hunk = std::move(parsed_hunk);
      in_hunk = true;
    }
  }

  // Add last hunk
  if (in_hunk) {
    hunks.push_back(current_hunk);
  }

  if (target_file.empty()) {
    safeErrorPrintLn("patch: cannot find target file in patch");
    return 1;
  }

  // [DIFFERS] -d/--directory: prepend directory to the target path.
  if (!directory.empty()) {
    target_file = directory + "/" + target_file;
    // Normalise backslashes introduced by Windows paths.
    for (char& c : target_file) {
      if (c == '\\') c = '/';
    }
  }

  if (verbose) {
    safePrintLn("patch: patching file '" + target_file + "'");
  }

  // Read original file
  std::vector<std::string> lines = read_file_lines(target_file);

  if (lines.empty() && !hunks.empty()) {
    safeErrorPrintLn("patch: cannot open target file '" + target_file + "'");
    return 1;
  }

  // Backup file if requested.
  // [DIFFERS] --backup-if-mismatch / --no-backup-if-mismatch control
  // conditional backup.  When --backup-if-mismatch is set, backup only when
  // the patch does not apply cleanly (fuzz > 0 or offset != 0).
  // --no-backup-if-mismatch disables conditional backup entirely.
  const bool do_backup = ctx.get<bool>("-b", false);
  // Defer backup to post-hunk-apply when conditional logic is active.

  // Apply hunks
  int applied = 0;
  int failed = 0;

  int cumulative_offset = 0;
  std::vector<Hunk> rejected;
  for (const auto& original_hunk : hunks) {
    Hunk hunk = original_hunk;
    hunk.old_start += cumulative_offset;
    int used_offset = 0;
    if (apply_hunk(lines, hunk, reverse, fuzz, used_offset)) {
      applied++;
      cumulative_offset += hunk.new_count - hunk.old_count + used_offset;
    } else {
      failed++;
      rejected.push_back(original_hunk);
      if (!silent) {
        safeErrorPrintLn("patch: hunk FAILED at line " +
                         std::to_string(original_hunk.old_start));
      }
    }
  }

  // [DIFFERS] Determine whether a mismatch occurred for conditional backup.
  // A mismatch means either offset drift (cumulative_offset != 0) or any
  // hunk that could not be applied at all.
  const bool had_mismatch = (backup_if_mismatch || !no_backup_if_mismatch)
                                ? ((cumulative_offset != 0) || (failed > 0))
                                : false;

  // [DIFFERS] Conditional backup per --backup-if-mismatch /
  // --no-backup-if-mismatch.
  if (do_backup && !dry_run) {
    if (no_backup_if_mismatch) {
      // --no-backup-if-mismatch: never backup when -b is used.
      // (GNU: only backup if patch does NOT match exactly)
    } else if (backup_if_mismatch) {
      // --backup-if-mismatch: backup only when there is a mismatch.
      if (had_mismatch) {
        backup_file(target_file, backup_ext);
      }
    } else {
      // Default: always backup when -b is used.
      backup_file(target_file, backup_ext);
    }
  }

  // GNU -r writes rejected hunks as a patch that can be fixed manually.
  if (!reject_file.empty() && !rejected.empty()) {
    std::vector<std::string> reject_lines;
    for (const auto& hunk : rejected) {
      reject_lines.push_back("@@ -" + std::to_string(hunk.old_start) + "," +
                             std::to_string(hunk.old_count) + " +" +
                             std::to_string(hunk.new_start) + "," +
                             std::to_string(hunk.new_count) + " @@");
      for (const auto& line : hunk.old_lines)
        reject_lines.push_back("-" + line);
      for (const auto& line : hunk.new_lines)
        reject_lines.push_back("+" + line);
    }
    if (!write_file_lines(reject_file, reject_lines)) {
      safeErrorPrintLn("patch: cannot write reject file '" + reject_file + "'");
      return 1;
    }
  }

  if (!dry_run && (failed == 0 || force)) {
    const std::string destination =
        output_file.empty() ? target_file : output_file;
    // [DIFFERS] --remove-empty-files: if the patched content is empty,
    // delete the output file rather than writing an empty one.
    if (remove_empty && lines.empty()) {
      std::wstring wpath = utf8_to_wstring(destination);
      DeleteFileW(wpath.c_str());
      if (verbose) {
        safePrintLn("patch: removed empty file '" + destination + "'");
      }
    } else {
      if (!write_file_lines(destination, lines)) {
        safeErrorPrintLn("patch: cannot write to file '" + destination + "'");
        return 1;
      }
    }
  }

  if (!silent) {
    safePrintLn("patch: " + std::to_string(applied) + " hunks applied, " +
                std::to_string(failed) + " failed");
  }

  return (failed > 0 && !force) ? 1 : 0;
}
