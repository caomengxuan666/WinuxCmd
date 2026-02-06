/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: ls.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
/// @Description: Implemention for ls.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

module;
#include "core/auto_flags.h"
#include "core/command_macros.h"
#include "pch/pch.h"
export module commands.ls;

import std;
import core;
import utils;

/**
 * @brief LS command options definition
 *
 * This array defines all the options supported by the ls command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 * - @a -a, @a --all: Do not ignore entries starting with . [IMPLEMENTED]
 * - @a -A, @a --almost-all: Do not list implied . and .. [IMPLEMENTED]
 * - @a -b, @a --escape: Print C-style escapes for nongraphic characters [TODO]
 * - @a -B, @a --ignore-backups: Do not list implied entries ending with ~
 * [TODO]
 * - @a -c: With -lt: sort by, and show, ctime; with -l: show ctime and sort by
 * name; otherwise: sort by ctime, newest first [TODO]
 * - @a -C: List entries by columns [IMPLEMENTED]
 * - @a -d, @a --directory: List directories themselves, not their contents
 * [TODO]
 * - @a -f: List all entries in directory order [TODO]
 * - @a -F, @a --classify: Append indicator (one of *=>@|) to entries [TODO]
 * - @a -g: Like -l, but do not list owner [IMPLEMENTED]
 * - @a -h, @a --human-readable: With -l and -s, print sizes like 1K 234M 2G
 * etc. [IMPLEMENTED]
 * - @a -i, @a --inode: Print the index number of each file [TODO]
 * - @a -k, @a --kibibytes: Default to 1024-byte blocks for file system usage
 * [TODO]
 * - @a -L, @a --dereference: When showing file information for a symbolic link,
 * show information for the file the link references [TODO]
 * - @a -l, @a --long-list: Use a long listing format [IMPLEMENTED]
 * - @a -m: Fill width with a comma separated list of entries [TODO]
 * - @a -n, @a --numeric-uid-gid: Like -l, but list numeric user and group IDs
 * [TODO]
 * - @a -N, @a --literal: Print entry names without quoting [TODO]
 * - @a -o: Like -l, but do not list group information [IMPLEMENTED]
 * - @a -p, @a --indicator-style=slash: Append / indicator to directories [TODO]
 * - @a -q, @a --hide-control-chars: Print ? instead of nongraphic characters
 * [TODO]
 * - @a -Q, @a --quote-name: Enclose entry names in double quotes [TODO]
 * - @a -r, @a --reverse: Reverse order while sorting [IMPLEMENTED]
 * - @a -R, @a --recursive: List subdirectories recursively [TODO]
 * - @a -s, @a --size: Print the allocated size of each file, in blocks [TODO]
 * - @a -S: Sort by file size, largest first [TODO]
 * - @a -t: Sort by time, newest first [TODO]
 * - @a -T, @a --tabsize: Assume tab stops at each COLS instead of 8
 * [IMPLEMENTED]
 * - @a -u: With -lt: sort by, and show, access time; with -l: show access time
 * and sort by name; otherwise: sort by access time, newest first [TODO]
 * - @a -U: Do not sort; list entries in directory order [TODO]
 * - @a -v: Natural sort of (version) numbers within text [TODO]
 * - @a -w, @a --width: Set output width to COLS. 0 means no limit [IMPLEMENTED]
 * - @a -x: List entries by lines instead of by columns [TODO]
 * - @a -X: Sort alphabetically by entry extension [TODO]
 * - @a -Z, @a --context: Print any security context of each file [TODO]
 * - @a -1: List one file per line [IMPLEMENTED]
 */
constexpr auto LS_OPTIONS = std::array{
    OPTION("-a", "--all", "do not ignore entries starting with ."),
    OPTION("-A", "--almost-all", "do not list implied . and .."),
    OPTION("-b", "--escape", "print C-style escapes for nongraphic characters"),
    OPTION("-B", "--ignore-backups",
           "do not list implied entries ending with ~"),
    OPTION("-c", "",
           "with -lt: sort by, and show, ctime; with -l: show ctime and sort "
           "by name; otherwise: sort by ctime, newest first"),
    OPTION("-C", "", "list entries by columns"),
    OPTION("-d", "--directory",
           "list directories themselves, not their contents"),
    OPTION("-f", "", "list all entries in directory order"),
    OPTION("-F", "--classify", "append indicator (one of */=>@|) to entries"),
    OPTION("-g", "", "like -l, but do not list owner"),
    OPTION("-h", "--human-readable",
           "with -l and -s, print sizes like 1K 234M 2G etc."),
    OPTION("-i", "--inode", "print the index number of each file"),
    OPTION("-k", "--kibibytes",
           "default to 1024-byte blocks for file system usage"),
    OPTION("-L", "--dereference",
           "when showing file information for a symbolic link, show "
           "information for the file the link references"),
    OPTION("-l", "--long-list", "use a long listing format"),
    OPTION("-m", "", "fill width with a comma separated list of entries"),
    OPTION("-n", "--numeric-uid-gid",
           "like -l, but list numeric user and group IDs"),
    OPTION("-N", "--literal", "print entry names without quoting"),
    OPTION("-o", "", "like -l, but do not list group information"),
    OPTION("-p", "--indicator-style=slash",
           "append / indicator to directories"),
    OPTION("-q", "--hide-control-chars",
           "print ? instead of nongraphic characters"),
    OPTION("-Q", "--quote-name", "enclose entry names in double quotes"),
    OPTION("-r", "--reverse", "reverse order while sorting"),
    OPTION("-R", "--recursive", "list subdirectories recursively"),
    OPTION("-s", "--size", "print the allocated size of each file, in blocks"),
    OPTION("-S", "", "sort by file size, largest first"),
    OPTION("-t", "", "sort by time, newest first"),
    OPTION("-T", "--tabsize", "assume tab stops at each COLS instead of 8"),
    OPTION(
        "-u", "",
        "with -lt: sort by, and show, access time; with -l: show access time "
        "and sort by name; otherwise: sort by access time, newest first"),
    OPTION("-U", "", "do not sort; list entries in directory order"),
    OPTION("-v", "", "natural sort of (version) numbers within text"),
    OPTION("-w", "--width", "set output width to COLS. 0 means no limit"),
    OPTION("-x", "", "list entries by lines instead of by columns"),
    OPTION("-X", "", "sort alphabetically by entry extension"),
    OPTION("-Z", "--context", "print any security context of each file"),
    OPTION("-1", "", "list one file per line")
};

// Auto-generated lookup table for options from LS_OPTIONS
constexpr auto OPTION_HANDLERS =
        generate_option_handlers(LS_OPTIONS, "--tabsize", "--width");

// clang-format off
CREATE_AUTO_FLAGS_CLASS(LsOptions,
    DECLARE_NUMERIC_OPTION(int, tab_size, 8)
    DECLARE_NUMERIC_OPTION(int, width, 0)

    // Define all flags
    DEFINE_FLAG(long_format, 0)
    DEFINE_FLAG(show_all, 1)
    DEFINE_FLAG(almost_all, 2)
    DEFINE_FLAG(escape, 3)
    DEFINE_FLAG(ignore_backups, 4)
    DEFINE_FLAG(sort_by_ctime, 5)
    DEFINE_FLAG(columns, 6)
    DEFINE_FLAG(directory, 7)
    DEFINE_FLAG(directory_order, 8)
    DEFINE_FLAG(classify, 9)
    DEFINE_FLAG(no_owner, 10)
    DEFINE_FLAG(human_readable, 11)
    DEFINE_FLAG(show_inode, 12)
    DEFINE_FLAG(kibibytes, 13)
    DEFINE_FLAG(dereference, 14)
    DEFINE_FLAG(comma_separated, 15)
    DEFINE_FLAG(numeric_uid_gid, 16)
    DEFINE_FLAG(literal, 17)
    DEFINE_FLAG(no_group, 18)
    DEFINE_FLAG(indicator_slash, 19)
    DEFINE_FLAG(hide_control_chars, 20)
    DEFINE_FLAG(quote_name, 21)
    DEFINE_FLAG(reverse_order, 22)
    DEFINE_FLAG(recursive, 23)
    DEFINE_FLAG(show_size, 24)
    DEFINE_FLAG(sort_by_size, 25)
    DEFINE_FLAG(sort_by_time, 26)
    DEFINE_FLAG(sort_by_atime, 27)
    DEFINE_FLAG(no_sort, 28)
    DEFINE_FLAG(natural_sort, 29)
    DEFINE_FLAG(lines, 30)
    DEFINE_FLAG(sort_by_extension, 31)
    DEFINE_FLAG(show_context, 32)
    DEFINE_FLAG(one_per_line, 33)
)
// clang-format on

REGISTER_COMMAND(
    ls,
    /* cmd_name */ "ls",
    /* cmd_synopsis */ "list directory contents",
    /* cmd_desc */
    "List information about the FILEs (the current directory by default).\n"
    "Sort entries alphabetically if none of -cftuvSUX nor --sort is "
    "specified.\n"
    "\n"
    "With no FILE, list the current directory contents. With a FILE that is a\n"
    "directory, list the files and subdirectories inside that directory.\n"
    "With a FILE that is not a directory, list just that file.\n",
    /* examples */
    "  ls                      List files in current directory\n"
    "  ls -l                   Long listing format\n"
    "  ls -la                  Long listing format including hidden files\n"
    "  ls -lh                  Long listing format with human-readable sizes",
    /* see_also */ "find(1), grep(1), sort(1), wc(1)",
    /* author */ "caomengxuan666",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */
    LS_OPTIONS) {
    /**
     * @brief Parse command line options for ls
     * @param args Command arguments
     * @param options Output parameter for parsed options
     * @param paths Output parameter for paths to process
     * @return true if parsing succeeded, false on error
     */
    auto parseLsOptions = [](std::span<std::string_view> args, LsOptions &options,
                             std::vector<std::string> &paths) -> bool {
        // Helper functions for code reuse
        auto find_handler = [](std::string_view arg,
                               char opt_char = '\0') -> const OptionHandler * {
            for (const auto &handler: OPTION_HANDLERS) {
                if ((!arg.empty() && handler.long_opt && arg == handler.long_opt) ||
                    (opt_char && handler.short_opt == opt_char)) {
                    return &handler;
                }
            }
            return nullptr;
        };

        auto handle_arg_option = [&options](char opt_char, int value) {
            if (opt_char == 'T')
                options.set_tab_size(value);
            else if (opt_char == 'w')
                options.set_width(value);
        };

        auto print_option_error = [](std::string_view arg, char opt_char = '\0') {
            if (!arg.empty()) {
                std::wstring warg = utf8_to_wstring(std::string(arg));
                safeErrorPrintLn(L"ls: invalid option -- '" + warg.substr(2) + L"'");
            } else {
                safeErrorPrintLn(L"ls: invalid option -- '" +
                                 std::wstring(1, opt_char) + L"'");
            }
        };

        auto set_boolean_option = [&options](char opt_char) {
            switch (opt_char) {
                case 'l':
                    options.set_long_format(true);
                    break;
                case 'a':
                    options.set_show_all(true);
                    break;
                case 'A':
                    options.set_almost_all(true);
                    break;
                case 'b':
                    options.set_escape(true);
                    break;
                case 'B':
                    options.set_ignore_backups(true);
                    break;
                case 'c':
                    options.set_sort_by_ctime(true);
                    break;
                case 'C':
                    options.set_columns(true);
                    break;
                case 'd':
                    options.set_directory(true);
                    break;
                case 'f':
                    options.set_directory_order(true);
                    break;
                case 'F':
                    options.set_classify(true);
                    break;
                case 'g':
                    options.set_no_owner(true);
                    break;
                case 'h':
                    options.set_human_readable(true);
                    break;
                case 'i':
                    options.set_show_inode(true);
                    break;
                case 'k':
                    options.set_kibibytes(true);
                    break;
                case 'L':
                    options.set_dereference(true);
                    break;
                case 'm':
                    options.set_comma_separated(true);
                    break;
                case 'n':
                    options.set_numeric_uid_gid(true);
                    break;
                case 'N':
                    options.set_literal(true);
                    break;
                case 'o':
                    options.set_no_group(true);
                    break;
                case 'p':
                    options.set_indicator_slash(true);
                    break;
                case 'q':
                    options.set_hide_control_chars(true);
                    break;
                case 'Q':
                    options.set_quote_name(true);
                    break;
                case 'r':
                    options.set_reverse_order(true);
                    break;
                case 'R':
                    options.set_recursive(true);
                    break;
                case 's':
                    options.set_show_size(true);
                    break;
                case 'S':
                    options.set_sort_by_size(true);
                    break;
                case 't':
                    options.set_sort_by_time(true);
                    break;
                case 'u':
                    options.set_sort_by_atime(true);
                    break;
                case 'U':
                    options.set_no_sort(true);
                    break;
                case 'v':
                    options.set_natural_sort(true);
                    break;
                case 'x':
                    options.set_lines(true);
                    break;
                case 'X':
                    options.set_sort_by_extension(true);
                    break;
                case 'Z':
                    options.set_show_context(true);
                    break;
                case '1':
                    options.set_one_per_line(true);
                    break;
            }
        };

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args[i];

            if (arg.starts_with("--")) {
                // This is a long option
                if (const auto *handler = find_handler(arg)) {
                    if (handler->requires_arg) {
                        if (i + 1 < args.size()) {
                            handle_arg_option(handler->short_opt,
                                              std::stoi(std::string(args[i + 1])));
                            ++i;
                        } else {
                            std::wstring warg(arg.begin(), arg.end());
                            safeErrorPrintLn(L"ls: option '" + warg +
                                             L"' requires an argument");
                            return false;
                        }
                    } else {
                        set_boolean_option(handler->short_opt);
                    }
                } else {
                    print_option_error(arg);
                    return false;
                }
            } else if (arg.starts_with('-')) {
                // This is a short option
                if (arg == "-") {
                    // Single dash, treat as path
                    paths.push_back(std::string(arg));
                    continue;
                }

                // Process option characters
                for (size_t j = 1; j < arg.size(); ++j) {
                    char opt_char = arg[j];
                    if (const auto *handler = find_handler("", opt_char)) {
                        if (handler->requires_arg) {
                            if (j + 1 < arg.size()) {
                                handle_arg_option(opt_char,
                                                  std::stoi(std::string(arg.substr(j + 1))));
                                j = arg.size() - 1;
                            } else if (i + 1 < args.size()) {
                                handle_arg_option(opt_char,
                                                  std::stoi(std::string(args[i + 1])));
                                ++i;
                                break;
                            } else {
                                safeErrorPrintLn(L"ls: option requires an argument -- '" +
                                                 std::wstring(1, opt_char) + L"'");
                                return false;
                            }
                        } else {
                            set_boolean_option(opt_char);
                        }
                    } else {
                        print_option_error("", opt_char);
                        return false;
                    }
                }
            } else {
                // This is a path - add as a separate path
                paths.push_back(std::string(arg));
            }
        }

        // Default to current directory if no paths specified
        if (paths.empty()) {
            paths.push_back(".");
        }

        return true;
    };

    /**
     * @brief Get file permissions string
     * @param find_data WIN32_FIND_DATAW structure
     * @return Permissions string in ls format
     */
    auto getPermissionsString =
            [](const WIN32_FIND_DATAW &find_data) -> std::wstring {
        std::wstring perms = L"----------";

        // Set file type
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            perms[0] = L'd';
        } else if (find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            perms[0] = L'l';
        } else {
            perms[0] = L'-';
        }

        // Set permissions (simplified for Windows)
        perms[1] = L'r'; // Owner read
        perms[2] = L'w'; // Owner write
        perms[3] = L'x'; // Owner execute
        perms[4] = L'r'; // Group read
        perms[5] = L'w'; // Group write
        perms[6] = L'x'; // Group execute
        perms[7] = L'r'; // Other read
        perms[8] = L'w'; // Other write
        perms[9] = L'x'; // Other execute

        return perms;
    };

    /**
     * @brief Get file size string
     * @param find_data WIN32_FIND_DATAW structure
     * @param options ls command options
     * @return File size string
     */
    auto getFileSizeString = [](const WIN32_FIND_DATAW &find_data,
                                const LsOptions &options) -> std::wstring {
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            return L""; // Empty string for directories
        }

        // Calculate file size
        uint64_t fileSize = static_cast<uint64_t>(find_data.nFileSizeLow) |
                            (static_cast<uint64_t>(find_data.nFileSizeHigh) << 32);

        if (options.get_human_readable()) {
            // Human-readable format - use std::wostringstream for safety
            const wchar_t *units[] = {L"B", L"K", L"M", L"G", L"T"};
            int unitIndex = 0;
            double size = static_cast<double>(fileSize);

            while (size >= 1024.0 && unitIndex < 4) {
                size /= 1024.0;
                unitIndex++;
            }

            // Use string stream for formatting
            std::wostringstream oss;
            if (size < 10.0) {
                oss << std::fixed << std::setprecision(1) << size << units[unitIndex];
            } else {
                oss << std::fixed << std::setprecision(0) << size << units[unitIndex];
            }
            return oss.str();
        } else {
            // Regular format - use string stream
            std::wostringstream oss;
            oss << fileSize;
            return oss.str();
        }
    };

    /**
     * @brief Get file modification time string
     * @param find_data WIN32_FIND_DATAW structure
     * @return Modification time string
     */
    auto getModificationTimeString =
            [](const WIN32_FIND_DATAW &find_data) -> std::wstring {
        SYSTEMTIME st;
        FileTimeToSystemTime(&find_data.ftLastWriteTime, &st);

        // Month abbreviations
        const wchar_t *month_abbrs[] = {
            L"", L"Jan", L"Feb", L"Mar", L"Apr",
            L"May", L"Jun", L"Jul", L"Aug", L"Sep",
            L"Oct", L"Nov", L"Dec"
        };

        // Use wostringstream for consistency
        std::wostringstream oss;
        oss << month_abbrs[st.wMonth] << L" ";
        if (st.wDay < 10) {
            oss << L" ";
        }
        oss << st.wDay << L" " << std::setfill(L'0') << std::setw(2) << st.wHour
                << L":" << std::setw(2) << st.wMinute;

        return oss.str();
    };

    /**
     * @brief Get file owner and group information
     * @return Pair of (owner, group) strings
     */
    auto getFileOwnerAndGroup = []() -> std::pair<std::wstring, std::wstring> {
        wchar_t username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        if (!GetUserNameW(username, &username_len)) {
            return {L"user", L"group"};
        }

        std::wstring username_str = username;

        // Extract only username part from DOMAIN\USER
        size_t pos = username_str.find(L'\\');
        if (pos != std::wstring::npos) {
            username_str = username_str.substr(pos + 1);
        }

        // On Windows, group name is same as username for now
        return {username_str, username_str};
    };

    /**
     * @brief Get color for file based on type
     * @param find_data WIN32_FIND_DATAW structure
     * @return Color string
     */
    auto getFileColor = [](const WIN32_FIND_DATAW &find_data) -> std::wstring {
        if (!isTerminalSupportsColor()) {
            return L"";
        }

        std::wstring filename(find_data.cFileName);
        size_t dotPos = filename.rfind(L'.');
        if (dotPos == std::wstring::npos) {
            // No extension
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                return COLOR_DIR;
            }
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
                return COLOR_LINK;
            }
            // Regular file
            return COLOR_FILE;
        }

        std::wstring ext = filename.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

        // === Executable files ===
        static const std::unordered_set<std::wstring> execExts = {
            L"exe", L"com", L"bat", L"cmd", L"msi"
        };
        if (execExts.contains(ext)) {
            return COLOR_EXEC;
        }

        // === Script files ===
        static const std::unordered_set<std::wstring> scriptExts = {
            L"ps1", L"psm1", L"sh", L"bash", L"zsh",
            L"py", L"pl", L"rb", L"js", L"ts", L"lua"
        };
        if (scriptExts.contains(ext)) {
            return COLOR_SCRIPT;
        }

        // === Archives ===
        static const std::unordered_set<std::wstring> archiveExts = {
            L"zip", L"rar", L"7z", L"tar", L"gz", L"bz2",
            L"xz", L"tgz", L"tbz2", L"lz", L"zst"
        };
        if (archiveExts.contains(ext)) {
            return COLOR_ARCHIVE;
        }

        // === Source code ===
        static const std::unordered_set<std::wstring> sourceExts = {
            L"c", L"cpp", L"cxx", L"h", L"hpp", L"rs",
            L"go", L"java", L"cs", L"swift", L"kt",
            L"js", L"ts", L"jsx", L"tsx", L"html", L"css"
        };
        if (sourceExts.contains(ext)) {
            return COLOR_SOURCE;
        }

        // === Media ===
        static const std::unordered_set<std::wstring> mediaExts = {
            L"jpg", L"jpeg", L"png", L"gif", L"bmp", L"webp",
            L"mp3", L"wav", L"flac", L"mp4", L"avi", L"mkv"
        };
        if (mediaExts.contains(ext)) {
            return COLOR_MEDIA;
        }

        // Default: regular file
        return COLOR_FILE;
    };

    auto formatFilenameWithColor = [&](const WIN32_FIND_DATAW &fd,
                                       const std::wstring &name) -> std::wstring {
        if (isTerminalSupportsColor()) {
            return getFileColor(fd) + name + COLOR_RESET;
        }
        return name;
    };

    /**
     * @brief Calculate optimal column layout
     * @param filenames List of filenames
     * @param terminalWidth Terminal width in columns
     * @return Pair of (columns, rows)
     */
    auto calculateColumns = [](const std::vector<std::wstring> &filenames,
                               int terminalWidth) -> std::pair<int, int> {
        if (filenames.empty()) return {0, 0};

        size_t maxDisplayWidth = 0;
        for (const auto &name: filenames) {
            maxDisplayWidth = std::max(maxDisplayWidth, string_display_width(name));
        }

        // Column width = max display width + 2 spaces padding
        int columnWidth = static_cast<int>(maxDisplayWidth) + 2;
        if (columnWidth <= 0) columnWidth = 1;

        int columns = terminalWidth / columnWidth;
        if (columns < 1) columns = 1;

        int rows = (filenames.size() + columns - 1) / columns;
        return {columns, rows};
    };

    // FileInfo struct definition
    struct FileInfo {
        std::wstring name;
        WIN32_FIND_DATAW find_data;
        std::wstring perms;
        std::wstring size;
        std::wstring mtime;
        std::wstring owner;
        std::wstring group;
    };

    /**
     * @brief List directory contents
     * @param path Path to list
     * @param options ls command options
     * @return true if listing succeeded, false on error
     */
    auto listDirectory = [&](const std::string &path,
                             const LsOptions &options) -> bool {
        std::wstring wpath = utf8_to_wstring(path);
        WIN32_FIND_DATAW find_data;
        HANDLE hFind = FindFirstFileW((wpath + L"\\*").c_str(), &find_data);
        if (hFind == INVALID_HANDLE_VALUE) {
            std::wstring wpath_str = utf8_to_wstring(path);
            safeErrorPrintLn(L"ls: cannot access '" + wpath_str +
                             L"': No such file or directory");
            return false;
        }

        std::vector<FileInfo> files;
        do {
            std::wstring filename(find_data.cFileName);
            if (!options.get_show_all() && (filename == L"." || filename == L".."))
                continue;

            FileInfo info;
            info.name = std::move(filename);
            info.find_data = find_data;
            info.perms = getPermissionsString(find_data);
            info.size = getFileSizeString(find_data, options);
            info.mtime = getModificationTimeString(find_data);

            // Get owner and group
            auto [owner, group] = getFileOwnerAndGroup();
            info.owner = owner;
            info.group = group;

            files.push_back(std::move(info));
        } while (FindNextFileW(hFind, &find_data) != 0);
        FindClose(hFind);

        // Sort files
        if (options.get_reverse_order()) {
            std::sort(
                files.rbegin(), files.rend(),
                [](const FileInfo &a, const FileInfo &b) { return a.name > b.name; });
        } else {
            std::ranges::sort(
                files.begin(), files.end(),
                [](const FileInfo &a, const FileInfo &b) { return a.name < b.name; });
        }

        if (options.get_long_format()) {
            // Calculate maximum widths
            size_t max_owner_len = 0;
            size_t max_group_len = 0;
            size_t max_size_len = 0;

            for (const auto &file: files) {
                max_owner_len = std::max(max_owner_len, file.owner.length());
                max_group_len = std::max(max_group_len, file.group.length());
                max_size_len = std::max(max_size_len, file.size.length());
            }

            // Set minimum widths to avoid empty values
            if (max_owner_len == 0) max_owner_len = 1;
            if (max_group_len == 0) max_group_len = 1;
            if (max_size_len == 0) max_size_len = 1;

            // Print files with precise alignment
            for (const auto &file_info: files) {
                std::wstring line;

                // 1. Permissions and link count
                line += file_info.perms;
                line += L' ';
                line += L'1'; // Windows always has 1 link
                line += L' ';

                // 2. Owner (left-aligned)
                line += file_info.owner;
                int owner_padding = static_cast<int>(max_owner_len) -
                                    static_cast<int>(file_info.owner.length());
                for (int i = 0; i < owner_padding; i++) line += L' ';
                line += L' ';

                // 3. Group (left-aligned)
                line += file_info.group;
                int group_padding = static_cast<int>(max_group_len) -
                                    static_cast<int>(file_info.group.length());
                for (int i = 0; i < group_padding; i++) line += L' ';
                line += L' ';

                // 4. File size (right-aligned)
                int size_padding = static_cast<int>(max_size_len) -
                                   static_cast<int>(file_info.size.length());
                for (int i = 0; i < size_padding; i++) line += L' ';
                line += file_info.size;
                line += L' ';

                // 5. Modification time + filename
                line += file_info.mtime;
                line += L' ';
                if (isTerminalSupportsColor()) {
                    line += getFileColor(file_info.find_data);
                }
                line += file_info.name;
                if (isTerminalSupportsColor()) {
                    line += COLOR_RESET;
                }

                safePrintLn(line);
            }
        } else {
            // Brief mode with columns
            std::vector<std::wstring> filenames;
            std::vector<std::wstring> colors;

            for (const auto &file_info: files) {
                filenames.push_back(file_info.name);
                colors.push_back(getFileColor(file_info.find_data));
            }

            // Calculate terminal width
            int terminalWidth = options.get_width() > 0 ? options.get_width() : getTerminalWidth();

            // Compute MAX DISPLAY WIDTH (not .length()!)
            size_t maxDisplayWidth = 0;
            for (const auto &name: filenames) {
                maxDisplayWidth = std::max(maxDisplayWidth, string_display_width(name));
            }

            // Avoid division by zero
            if (maxDisplayWidth == 0) maxDisplayWidth = 1;

            // Column layout
            int columnWidth = static_cast<int>(maxDisplayWidth) + 2; // +2 for spacing
            int columns = terminalWidth / columnWidth;
            if (columns < 1) columns = 1;
            int rows = (filenames.size() + columns - 1) / columns;

            // Print in column-major order
            for (int row = 0; row < rows; ++row) {
                std::wstring row_output;
                for (int col = 0; col < columns; ++col) {
                    int index = col * rows + row;
                    if (index >= filenames.size()) break;

                    // Apply color
                    if (isTerminalSupportsColor()) {
                        row_output += colors[index];
                    }
                    row_output += filenames[index];
                    if (isTerminalSupportsColor()) {
                        row_output += COLOR_RESET;
                    }

                    // Add padding based on DISPLAY WIDTH, not string length
                    size_t currentWidth = string_display_width(filenames[index]);
                    int paddingSpaces = static_cast<int>(maxDisplayWidth + 2 - currentWidth);
                    if (paddingSpaces > 0) {
                        row_output.append(paddingSpaces, L' ');
                    }
                }
                safePrintLn(row_output);
            }
        }

        return true;
    };

    // Main implementation
    LsOptions options;
    std::vector<std::string> paths;

    if (!parseLsOptions(args, options, paths)) {
        return 2; // Invalid option error code
    }

    int result = 0;

    // If there are multiple paths and we're in brief mode (not long format), collect all files for columnar display
    if (paths.size() > 1 && !options.get_long_format() && !options.get_one_per_line()) {
        std::vector<std::wstring> filenames;
        std::vector<std::wstring> colors;

        for (size_t i = 0; i < paths.size(); ++i) {
            std::wstring wpath = utf8_to_wstring(paths[i]);

            // Check if path is a file
            WIN32_FIND_DATAW find_data;
            HANDLE hFind = FindFirstFileW(wpath.c_str(), &find_data);
            bool is_file = (hFind != INVALID_HANDLE_VALUE) &&
                           !(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
            if (hFind != INVALID_HANDLE_VALUE) {
                FindClose(hFind);
            }

            if (is_file) {
                // Add file to the list
                filenames.push_back(find_data.cFileName);
                colors.push_back(getFileColor(find_data));
            } else {
                // If it's a directory, list its contents
                if (paths.size() > 1) {
                    std::wstring wpath_str = utf8_to_wstring(paths[i]);
                    safePrintLn(wpath_str + L":");
                }
                if (!listDirectory(paths[i], options)) {
                    result = 1; // Directory listing error
                }
            }
        }

        // If all paths were files, display them in columns
        if (!filenames.empty()) {
            // Calculate terminal width
            int terminalWidth = options.get_width() > 0 ? options.get_width() : getTerminalWidth();

            // Compute MAX DISPLAY WIDTH (not .length()!)
            size_t maxDisplayWidth = 0;
            for (const auto &name: filenames) {
                maxDisplayWidth = std::max(maxDisplayWidth, string_display_width(name));
            }

            // Avoid division by zero
            if (maxDisplayWidth == 0) maxDisplayWidth = 1;

            // Column layout
            int columnWidth = static_cast<int>(maxDisplayWidth) + 2; // +2 for spacing
            int columns = terminalWidth / columnWidth;
            if (columns < 1) columns = 1;
            int rows = (filenames.size() + columns - 1) / columns;

            // Print in column-major order
            for (int row = 0; row < rows; ++row) {
                std::wstring row_output;
                for (int col = 0; col < columns; ++col) {
                    int index = col * rows + row;
                    if (index >= filenames.size()) break;

                    // Apply color
                    if (isTerminalSupportsColor()) {
                        row_output += colors[index];
                    }
                    row_output += filenames[index];
                    if (isTerminalSupportsColor()) {
                        row_output += COLOR_RESET;
                    }

                    // Add padding based on DISPLAY WIDTH, not string length
                    size_t currentWidth = string_display_width(filenames[index]);
                    int paddingSpaces = static_cast<int>(maxDisplayWidth + 2 - currentWidth);
                    if (paddingSpaces > 0) {
                        row_output.append(paddingSpaces, L' ');
                    }
                }
                safePrintLn(row_output);
            }
        }
    } else {
        // Handle single path or long format normally
        for (size_t i = 0; i < paths.size(); ++i) {
            std::wstring wpath = utf8_to_wstring(paths[i]);

            // Check if path is a file
            WIN32_FIND_DATAW find_data;
            HANDLE hFind = FindFirstFileW(wpath.c_str(), &find_data);
            bool is_file = (hFind != INVALID_HANDLE_VALUE) &&
                           !(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
            if (hFind != INVALID_HANDLE_VALUE) {
                FindClose(hFind);
            }

            if (is_file) {
                // If it's a file, display it directly
                if (options.get_long_format()) {
                    // For long format, get file info
                    HANDLE hFileFind = FindFirstFileW(wpath.c_str(), &find_data);
                    if (hFileFind != INVALID_HANDLE_VALUE) {
                        FileInfo info;
                        info.name = find_data.cFileName;
                        info.find_data = find_data;
                        info.perms = getPermissionsString(find_data);
                        info.size = getFileSizeString(find_data, options);
                        info.mtime = getModificationTimeString(find_data);

                        // Get owner and group
                        auto [owner, group] = getFileOwnerAndGroup();
                        info.owner = owner;
                        info.group = group;

                        // Calculate column widths
                        size_t max_owner_len = info.owner.length();
                        size_t max_group_len = info.group.length();
                        size_t max_size_len = info.size.length();

                        // Print with long format
                        std::wstring line;
                        line += info.perms;
                        line += L' ';
                        line += L'1'; // Windows always has 1 link
                        line += L' ';
                        line += info.owner;
                        int owner_padding = static_cast<int>(max_owner_len) -
                                            static_cast<int>(info.owner.length());
                        for (int j = 0; j < owner_padding; j++) line += L' ';
                        line += L' ';
                        line += info.group;
                        int group_padding = static_cast<int>(max_group_len) -
                                            static_cast<int>(info.group.length());
                        for (int j = 0; j < group_padding; j++) line += L' ';
                        line += L' ';
                        int size_padding = static_cast<int>(max_size_len) -
                                           static_cast<int>(info.size.length());
                        for (int j = 0; j < size_padding; j++) line += L' ';
                        line += info.size;
                        line += L' ';
                        line += info.mtime;
                        line += L' ';
                        // Apply color to the line
                        line += formatFilenameWithColor(find_data, info.name);
                        safePrintLn(line);

                        FindClose(hFileFind);
                    }
                } else {
                    // For brief mode, just print the filename
                    safePrintLn(formatFilenameWithColor(find_data, find_data.cFileName));
                }
            } else {
                // If it's a directory, list its contents
                if (paths.size() > 1) {
                    std::wstring wpath_str = utf8_to_wstring(paths[i]);
                    safePrintLn(wpath_str + L":");
                }
                if (!listDirectory(paths[i], options)) {
                    result = 1; // Directory listing error
                }
            }
            if (i < paths.size() - 1 && !is_file) {
                safePrintLn(L"");
            }
        }
    }

    return result;
}
