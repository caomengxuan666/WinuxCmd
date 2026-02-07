/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: scaffold.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

std::string generateCommandModule(const std::string &cmdName) {
  std::string moduleContent = "";
  // Add Author Infomation
  moduleContent += "/// @Author: TODO: fill in your name\n";
  moduleContent += "/// @contributors: \n";
  moduleContent += "///   - contributor1 <name> <email2@example.com>\n";
  moduleContent += "///   - contributor2 <name> <email2@example.com>\n";
  moduleContent += "///   - contributor3 <name> <email3@example.com>\n";
  moduleContent += "///   - description: \n";
  moduleContent += "/// @Description: TODO: Add command description\n";
  moduleContent += "/// @Version: 0.1.0\n";
  moduleContent += "/// @License: MIT\n";
  moduleContent += "/// @Copyright: Copyright © 2026 WinuxCmd\n";
  moduleContent += "module;\n\n";
  moduleContent += "#include \"pch/pch.h\"\n";
  moduleContent += "#include \"core/command_macros.h\"\n";
  moduleContent += "#include \"core/auto_flags.h\"\n\n";
  moduleContent += "export module commands." + cmdName + ";\n\n";
  moduleContent += "import std;\n";
  moduleContent += "import core;\n";
  moduleContent += "import utils;\n";
  moduleContent += "namespace fs = std::filesystem;\n\n";

  // Convert command name to uppercase for constant name
  std::string upperCmdName = cmdName;
  for (auto &c : upperCmdName) {
    c = std::toupper(c);
  }

  moduleContent +=
      "constexpr auto " + upperCmdName + "_OPTIONS = std::array{\n";
  moduleContent +=
      "    OPTION(\"-v\", \"--verbose\", \"explain what is being done\"),\n";
  moduleContent += "    // Add more options here\n";
  moduleContent += "};\n\n";
  moduleContent += "// Auto-generated lookup table for options from " +
                   upperCmdName + "_OPTIONS\n";
  moduleContent +=
      "constexpr auto OPTION_HANDLERS = generate_option_handlers(" +
      upperCmdName + "_OPTIONS);\n\n";
  moduleContent += "CREATE_AUTO_FLAGS_CLASS(" + cmdName + "Options,\n";
  moduleContent += "    // Define all flags\n";
  moduleContent += "    DEFINE_FLAG(verbose, 0)          // -v, --verbose\n";
  moduleContent += "    // Add more flags here\n";
  moduleContent += ")\n\n";
  moduleContent += "REGISTER_COMMAND(\n";
  moduleContent += "    " + cmdName + ",\n";
  moduleContent += "    /* cmd_name */ \"" + cmdName + "\",\n";
  moduleContent += "    /* cmd_synopsis */ \"TODO: Add command synopsis\",\n";
  moduleContent +=
      "    /* cmd_desc */ \"TODO: Add command description. With no arguments, "
      "this command does nothing. With arguments, this command processes "
      "them.\",\n";
  moduleContent += "    /* examples */ \"  " + cmdName +
                   "                      TODO: Add example\\n  " + cmdName +
                   " -v                   TODO: Add example with verbose\",\n";
  moduleContent += "    /* see_also */ \"TODO: Add see also\",\n";
  moduleContent += "    /* author */ \"TODO: Add your name\",\n";
  moduleContent += "    /* copyright */ \"Copyright © 2026 WinuxCmd\",\n";
  moduleContent += "    /* options */\n";
  moduleContent += "    " + upperCmdName + "_OPTIONS\n";
  moduleContent += ") {\n";
  moduleContent += "  /**\n";
  moduleContent +=
      "   * @brief Parse command line options for " + cmdName + "\n";
  moduleContent += "   * @param args Command arguments\n";
  moduleContent += "   * @param options Output parameter for parsed options\n";
  moduleContent += "   * @param paths Output parameter for paths to process\n";
  moduleContent += "   * @return true if parsing succeeded, false on error\n";
  moduleContent += "   */\n";
  moduleContent +=
      "  auto parse" + upperCmdName +
      "Options = [](std::span<std::string_view> args, " + cmdName +
      "Options& options, std::vector<std::string>& paths) -> bool {\n";
  moduleContent +=
      "    // Helper function to find option handler by long option or short "
      "option\n";
  moduleContent +=
      "    auto find_handler = [](std::string_view arg, char opt_char = '\\0') "
      "-> const OptionHandler* {\n";
  moduleContent += "      for (const auto& handler : OPTION_HANDLERS) {\n";
  moduleContent +=
      "        if ((!arg.empty() && handler.long_opt && arg == "
      "handler.long_opt) ||\n";
  moduleContent +=
      "            (opt_char && handler.short_opt == opt_char)) {\n";
  moduleContent += "          return &handler;\n";
  moduleContent += "        }\n";
  moduleContent += "      }\n";
  moduleContent += "      return nullptr;\n";
  moduleContent += "    }\n\n";
  moduleContent += "    // Helper function to print option error\n";
  moduleContent +=
      "    auto print_option_error = [](std::string_view arg, char opt_char = "
      "'\\0') {\n";
  moduleContent += "      if (!arg.empty()) {\n";
  moduleContent += "        std::wstring warg(arg.begin(), arg.end());\n";
  moduleContent += "        safeErrorPrintLn(L\"" + cmdName +
                   ": invalid option -- '\" + warg.substr(2) + L\"'\");\n";
  moduleContent += "      } else {\n";
  moduleContent +=
      "        safeErrorPrintLn(L\"" + cmdName +
      ": invalid option -- '\" + std::wstring(1, opt_char) + L\"'\");\n";
  moduleContent += "      }\n";
  moduleContent += "    }\n\n";
  moduleContent += "    // Helper function to set boolean option\n";
  moduleContent +=
      "    auto set_boolean_option = [&options](char opt_char) {\n";
  moduleContent += "      switch (opt_char) {\n";
  moduleContent += "        case 'v':\n";
  moduleContent += "          options.set_verbose(true);\n";
  moduleContent += "          break;\n";
  moduleContent += "        // Add more options here\n";
  moduleContent += "      }\n";
  moduleContent += "    }\n\n";
  moduleContent += "    for (size_t i = 0; i < args.size(); ++i) {\n";
  moduleContent += "      auto arg = args[i];\n\n";
  moduleContent += "      if (arg.starts_with(\"--\")) {\n";
  moduleContent += "        // This is a long option\n";
  moduleContent += "        const auto* handler = find_handler(arg);\n";
  moduleContent += "        if (handler) {\n";
  moduleContent += "          if (handler->requires_arg) {\n";
  moduleContent += "            // Handle options that require arguments\n";
  moduleContent += "            if (i + 1 < args.size()) {\n";
  moduleContent += "              // TODO: Handle argument options\n";
  moduleContent += "              ++i;\n";
  moduleContent += "            } else {\n";
  moduleContent += "              std::wostringstream oss;\n";
  moduleContent +=
      "              oss << L\"" + cmdName +
      ": option '\" << arg.data() << L\"' requires an argument\";\n";
  moduleContent += "              safeErrorPrintLn(oss.str());\n";
  moduleContent += "              return false;\n";
  moduleContent += "            }\n";
  moduleContent += "          } else {\n";
  moduleContent += "            set_boolean_option(handler->short_opt);\n";
  moduleContent += "          }\n";
  moduleContent += "        } else {\n";
  moduleContent += "          print_option_error(arg);\n";
  moduleContent += "          return false;\n";
  moduleContent += "        }\n";
  moduleContent += "      } else if (arg.starts_with(\"-\")) {\n";
  moduleContent += "        // This is a short option\n";
  moduleContent += "        if (arg == \"-\") {\n";
  moduleContent += "          // Single dash, treat as path\n";
  moduleContent += "          paths.push_back(std::string(arg));\n";
  moduleContent += "          continue;\n";
  moduleContent += "        }\n\n";
  moduleContent += "        // Process option characters\n";
  moduleContent += "        for (size_t j = 1; j < arg.size(); ++j) {\n";
  moduleContent += "          char opt_char = arg[j];\n";
  moduleContent +=
      "          const auto* handler = find_handler(\"\", opt_char);\n";
  moduleContent += "          if (handler) {\n";
  moduleContent += "            if (handler->requires_arg) {\n";
  moduleContent += "              // Handle options that require arguments\n";
  moduleContent += "              if (j + 1 < arg.size()) {\n";
  moduleContent += "                // TODO: Handle argument options\n";
  moduleContent += "                j = arg.size() - 1;\n";
  moduleContent += "              } else if (i + 1 < args.size()) {\n";
  moduleContent += "                // TODO: Handle argument options\n";
  moduleContent += "                ++i;\n";
  moduleContent += "              } else {\n";
  moduleContent += "                std::wostringstream oss;\n";
  moduleContent +=
      "                oss << L\"" + cmdName +
      ": option requires an argument -- '\" << opt_char << L\"'\";\n";
  moduleContent += "                safeErrorPrintLn(oss.str());\n";
  moduleContent += "                return false;\n";
  moduleContent += "              }\n";
  moduleContent += "            } else {\n";
  moduleContent += "              set_boolean_option(opt_char);\n";
  moduleContent += "            }\n";
  moduleContent += "          } else {\n";
  moduleContent += "            print_option_error(\"\", opt_char);\n";
  moduleContent += "            return false;\n";
  moduleContent += "          }\n";
  moduleContent += "        }\n";
  moduleContent += "      } else {\n";
  moduleContent += "        // This is a path\n";
  moduleContent += "        paths.push_back(std::string(arg));\n";
  moduleContent += "      }\n";
  moduleContent += "    }\n\n";
  moduleContent += "    if (paths.empty()) {\n";
  moduleContent +=
      "      safeErrorPrintLn(L\"" + cmdName + ": missing operand\\n\");\n";
  moduleContent += "      return false;\n";
  moduleContent += "    }\n\n";
  moduleContent += "    return true;\n";
  moduleContent += "  }\n\n";
  moduleContent += "  // Main implementation\n";
  moduleContent += "  " + cmdName + "Options options;\n";
  moduleContent += "  std::vector<std::string> paths;\n\n";
  moduleContent +=
      "  if (!parse" + upperCmdName + "Options(args, options, paths)) {\n";
  moduleContent += "    return 2;  // Invalid option error code\n";
  moduleContent += "  }\n\n";
  moduleContent += "  // TODO: Implement command logic here\n\n";
  moduleContent += "  // Example: Print paths\n";
  moduleContent += "  for (const auto& path : paths) {\n";
  moduleContent += "    if (options.get_verbose()) {\n";
  moduleContent +=
      "      safePrintLn(L\"Processing: \" + utf8_to_wstring(path));\n";
  moduleContent += "    }\n";
  moduleContent += "    // TODO: Process path\n";
  moduleContent += "  }\n\n";
  moduleContent += "  return 0;\n";
  moduleContent += "}\n";

  return moduleContent;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <command-name>" << std::endl;
    return 1;
  }

  // Check if running from project root directory
  fs::path currentPath = fs::current_path();
  bool isProjectRoot = fs::exists(currentPath / "src") &&
                       fs::exists(currentPath / "CMakeLists.txt");

  if (!isProjectRoot) {
    std::cerr
        << "Error: Please run this command from the project root directory!"
        << std::endl;
    std::cerr << "Usage: .\\scaffold.exe <command-name>" << std::endl;
    return 1;
  }

  std::string cmdName = argv[1];

  // Generate module content
  std::string moduleContent = generateCommandModule(cmdName);

  // Create output file path
  fs::path outputPath = currentPath / "src" / "commands" / (cmdName + ".cppm");

  // Check if file already exists
  if (fs::exists(outputPath)) {
    std::cerr << "Error: File " << outputPath.string() << " already exists!"
              << std::endl;
    return 1;
  }

  // Create directory if it doesn't exist
  fs::create_directories(outputPath.parent_path());

  // Write module content to file
  std::ofstream outFile(outputPath);
  if (!outFile) {
    std::cerr << "Error: Could not create file " << outputPath.string()
              << std::endl;
    return 1;
  }

  outFile << moduleContent;
  outFile.close();

  std::cout << "Successfully created command module: " << outputPath.string()
            << std::endl;
  std::cout << "Next steps: " << std::endl;
  std::cout << "1. Add command logic to " << cmdName << ".cppm" << std::endl;
  std::string displayCmdName = cmdName;
  for (auto &c : displayCmdName) {
    c = std::toupper(c);
  }
  std::cout << "2. Add options to " << displayCmdName << "_OPTIONS array"
            << std::endl;
  std::cout << "3. Update " << cmdName << "Options struct if needed"
            << std::endl;
  std::cout << "4. Rebuild the project" << std::endl;

  return 0;
}
