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
 *  - File: install.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for install.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd
// *** SIMPLIFIED IMPLEMENTATION - Some features may not be fully supported ***

#include "pch/pch.h"
//include other header after pch.h
#include "core/command_macros.h"

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr INSTALL_OPTIONS = std::array{
    OPTION("-b", "--backup", "make a backup of each existing destination file", BOOL_TYPE),
    OPTION("-c", "", "ignored (for compatibility with old Unix versions)", BOOL_TYPE),
    OPTION("-C", "", "ignored (for compatibility with old Unix versions)", BOOL_TYPE),
    OPTION("-d", "--directory", "treat all arguments as directory names", BOOL_TYPE),
    OPTION("-D", "", "create all leading components of DEST except the last", BOOL_TYPE),
    OPTION("-g", "--group", "set group ownership", STRING_TYPE),
    OPTION("-m", "--mode", "set permission mode", STRING_TYPE),
    OPTION("-o", "--owner", "set ownership", STRING_TYPE),
    OPTION("-p", "--preserve-timestamps", "apply access/modification times of SOURCE files", BOOL_TYPE),
    OPTION("-s", "--strip", "strip symbol tables", BOOL_TYPE),
    OPTION("-S", "--suffix", "override the usual backup suffix", STRING_TYPE),
    OPTION("-v", "--verbose", "print the name of each directory as it is created", BOOL_TYPE)
    // --target-directory (not implemented)
};

namespace install_pipeline {
namespace cp = core::pipeline;

struct Config {
  bool backup = false;
  bool directory_mode = false;
  bool preserve_timestamps = false;
  bool strip = false;
  bool verbose = false;
  std::string backup_suffix = "~";
  std::string group;
  std::string mode;
  std::string owner;
  std::string target_dir;
  SmallVector<std::string, 64> sources;
  SmallVector<std::string, 64> targets;
};

auto build_config(const CommandContext<INSTALL_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;
  cfg.backup = ctx.get<bool>("--backup", false) || ctx.get<bool>("-b", false);
  cfg.directory_mode = ctx.get<bool>("--directory", false) || ctx.get<bool>("-d", false);
  cfg.preserve_timestamps = ctx.get<bool>("--preserve-timestamps", false) || ctx.get<bool>("-p", false);
  cfg.strip = ctx.get<bool>("--strip", false) || ctx.get<bool>("-s", false);
  cfg.verbose = ctx.get<bool>("--verbose", false) || ctx.get<bool>("-v", false);

  auto group_opt = ctx.get<std::string>("--group", "");
  if (group_opt.empty()) {
    group_opt = ctx.get<std::string>("-g", "");
  }
  cfg.group = group_opt;

  auto mode_opt = ctx.get<std::string>("--mode", "");
  if (mode_opt.empty()) {
    mode_opt = ctx.get<std::string>("-m", "");
  }
  cfg.mode = mode_opt;

  auto owner_opt = ctx.get<std::string>("--owner", "");
  if (owner_opt.empty()) {
    owner_opt = ctx.get<std::string>("-o", "");
  }
  cfg.owner = owner_opt;

  auto suffix_opt = ctx.get<std::string>("--suffix", "");
  if (suffix_opt.empty()) {
    suffix_opt = ctx.get<std::string>("-S", "");
  }
  if (!suffix_opt.empty()) {
    cfg.backup_suffix = suffix_opt;
  }

  for (auto arg : ctx.positionals) {
    std::string file_arg(arg);
    if (contains_wildcard(file_arg)) {
      auto glob_result = glob_expand(file_arg);
      if (glob_result.expanded) {
        for (const auto& file : glob_result.files) {
          cfg.sources.push_back(wstring_to_utf8(file));
        }
        continue;
      }
    }
    cfg.sources.push_back(file_arg);
  }

  if (cfg.sources.empty()) {
    return std::unexpected("missing file operand");
  }

  return cfg;
}

auto run(const Config& cfg) -> int {



  // If directory mode, create all specified directories

  if (cfg.directory_mode) {

    for (const auto& dir : cfg.sources) {

      if (cfg.verbose) {

        safePrint("install: creating directory '");

        safePrint(dir);

        safePrintLn("'");

      }

      if (!CreateDirectoryA(dir.c_str(), NULL)) {

        DWORD error = GetLastError();

        if (error != ERROR_ALREADY_EXISTS) {

          safePrint("install: cannot create directory '");

          safePrint(dir);

          safePrintLn("'");

          return 1;

        }

      }

    }

    return 0;

  }



  // Determine last argument as target



  



    if (cfg.sources.size() < 2) {



      return 1;



    }



  



    SmallVector<std::string, 64> sources = cfg.sources;



    std::string target = sources.back();



    sources.pop_back();



  



    // Check if target is a directory or file



    DWORD attrs = GetFileAttributesA(target.c_str());



    bool target_is_dir = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_DIRECTORY);



  



    // If target is not a directory and we have multiple sources, assume target is a directory



    if (!target_is_dir && sources.size() > 1) {



      target_is_dir = true;



    }

  // Copy files
  for (const auto& source : sources) {
    std::string dest = target;
    
    if (target_is_dir) {
      // Append source filename to target directory
      size_t last_slash = source.find_last_of("/\\");
      std::string filename = (last_slash != std::string::npos) ? 
                            source.substr(last_slash + 1) : source;
      
      if (dest.back() != '\\' && dest.back() != '/') {
        dest += "\\";
      }
      dest += filename;
    }

    // Create backup if file exists and backup requested
    if (cfg.backup) {
      DWORD dest_attrs = GetFileAttributesA(dest.c_str());
      if (dest_attrs != INVALID_FILE_ATTRIBUTES) {
        std::string backup_path = dest + cfg.backup_suffix;
        if (MoveFileExA(dest.c_str(), backup_path.c_str(), MOVEFILE_REPLACE_EXISTING)) {
          if (cfg.verbose) {
            safePrint("created backup: ");
            safePrintLn(backup_path);
          }
        }
      }
    }

    // Copy file
    if (cfg.verbose) {
      safePrint("installing: ");
      safePrint(source);
      safePrint(" -> ");
      safePrintLn(dest);
    }

    if (!CopyFileA(source.c_str(), dest.c_str(), FALSE)) {
      safePrint("install: cannot copy '");
      safePrint(source);
      safePrint("' to '");
      safePrint(dest);
      safePrintLn("'");
      return 1;
    }
  }

  return 0;
}

}  // namespace install_pipeline

REGISTER_COMMAND(install, "install",
                 "install [OPTION]... [-T] SOURCE DEST\n"
                 "  install [OPTION]... SOURCE... DIRECTORY\n"
                 "  install [OPTION]... -t DIRECTORY SOURCE...",
                 "Copy files and set attributes.\n"
                 "\n"
                 "Note: This is a simplified Windows implementation.\n"
                 "Advanced features like mode, owner, group, and strip are\n"
                 "not fully supported on Windows.",
                 "  install source.txt dest.txt\n"
                 "  install -b file.txt backup/\n"
                 "  install -v src/*.txt /target/\n"
                 "  install -d /tmp/dir",
                 "cp(1), mv(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", INSTALL_OPTIONS) {
  using namespace install_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"install");
    return 1;
  }

  return run(*cfg_result);
}