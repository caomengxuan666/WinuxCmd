/*
 *  Copyright © 2026 WinuxCmd
 */
#include "core/command_macros.h"
#include "pch/pch.h"
import std;
import core;
import utils;
import container;

auto constexpr TSORT_OPTIONS =
    // [GNU] option
    std::array{OPTION("", "", "topological sort", STRING_TYPE)};

REGISTER_COMMAND(tsort,
                 /* cmd_name */ "tsort",
                 /* cmd_synopsis */ "tsort [FILE]",
                 /* cmd_desc */ "Perform topological sort.",
                 /* examples */ "tsort deps.txt",
                 /* see_also */ "sort",
                 /* author */ "WinuxCmd",
                 /* copyright */ "Copyright © 2026 WinuxCmd",
                 /* options */ TSORT_OPTIONS) {
  std::string input;

  if (ctx.positionals.empty()) {
    // Read from stdin
    input.assign(std::istreambuf_iterator<char>(std::cin),
                 std::istreambuf_iterator<char>());
  } else {
    // Read from file
    std::string filename = std::string(ctx.positionals[0]);
    std::wstring wfilename = utf8_to_wstring(filename);
    HANDLE hFile =
        CreateFileW(wfilename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE) {
      safeErrorPrintLn("tsort: cannot open '" + filename + "'");
      return 1;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(hFile, &fileSize);
    input.resize(fileSize.QuadPart);
    DWORD bytesRead;
    ReadFile(hFile, &input[0], static_cast<DWORD>(fileSize.QuadPart),
             &bytesRead, nullptr);
    CloseHandle(hFile);
    input.resize(bytesRead);
  }

  if (input.empty()) {
    safeErrorPrintLn("tsort: missing input");
    return 1;
  }

  std::istringstream iss(input);
  std::string line;
  std::set<std::string> nodes;
  std::map<std::string, std::vector<std::string>> graph;

  while (std::getline(iss, line)) {
    std::istringstream line_ss(line);
    std::string node, dep;
    if (line_ss >> node >> dep) {
      graph[node].push_back(dep);
      nodes.insert(node);
      nodes.insert(dep);
    }
  }

  std::map<std::string, int> remaining;
  for (const auto& n : nodes) remaining[n] = 0;
  for (const auto& [node, deps] : graph)
    for (const auto& dependency : deps) ++remaining[dependency];
  std::set<std::string> ready;
  for (const auto& [node, degree] : remaining)
    if (degree == 0) ready.insert(node);
  size_t emitted = 0;
  while (!ready.empty()) {
    auto node = *ready.begin();
    ready.erase(ready.begin());
    safePrintLn(node);
    ++emitted;
    for (const auto& dependency : graph[node])
      if (--remaining[dependency] == 0) ready.insert(dependency);
  }
  if (emitted != nodes.size()) {
    safeErrorPrintLn("tsort: input contains a loop");
    return 1;
  }

  return 0;
}
