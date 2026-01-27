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

std::string generateCommandModule(const std::string& cmdName) {
  std::string moduleContent = "";
  // Add Author Infomation
  moduleContent += "/// @Author: TODO: fill in your name\n";
  moduleContent += "/// @contributors: \n";
  moduleContent += "///   - contributor1 <email1@example.com>\n";
  moduleContent += "///   - contributor2 <email2@example.com>\n";
  moduleContent += "///   - contributor3 <email3@example.com>\n";
  moduleContent += "///   - description\n";
  moduleContent += "/// @Description: TODO: Add command description\n";
  moduleContent += "/// @Version: 0.1.0\n";
  moduleContent += "/// @License: MIT\n";
  moduleContent += "/// @Copyright: Copyright © 2026 WinuxCmd\n";
  moduleContent += "module;\n\n";
  moduleContent += "#include <cstdio>\n";
  moduleContent += "#include \"core/command_macros.h\"\n\n";
  moduleContent += "export module commands." + cmdName + ";\n\n";
  moduleContent += "import std;\n";
  moduleContent += "import core.dispatcher;\n";
  moduleContent += "import core.cmd_meta;\n";
  moduleContent += "import core.opt;\n\n";
  moduleContent += "namespace fs = std::filesystem;\n\n";
  moduleContent += "using namespace std;\n\n";

  // Convert command name to uppercase for constant name
  std::string upperCmdName = cmdName;
  for (auto& c : upperCmdName) {
    c = std::toupper(c);
  }

  moduleContent +=
      "constexpr auto " + upperCmdName + "_OPTIONS = std::array{\n";
  moduleContent +=
      "    OPTION(\"-v\", \"--verbose\", \"explain what is being done\"),\n";
  moduleContent += "    // Add more options here\n";
  moduleContent += "};\n\n";
  moduleContent += "// Auto-generated lookup table for options from " + upperCmdName + "_OPTIONS\n";
  moduleContent += "constexpr auto OPTION_HANDLERS = generate_option_handlers(" + upperCmdName + "_OPTIONS);\n\n";
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
  moduleContent += "    // Option flags for " + cmdName + " command\n";
  moduleContent += "    struct " + cmdName + "Options {\n";
  moduleContent += "        bool verbose = false; // -v, --verbose\n";
  moduleContent += "        // Add more options here\n";
  moduleContent += "    };\n\n";
  moduleContent += "    // Parse command options\n";
  moduleContent +=
      "    auto parseOptions = [&](std::span<std::string_view> args) -> " +
      cmdName + "Options {\n";
  moduleContent += "        " + cmdName + "Options options;\n";
  moduleContent += "        size_t pos = 1; // Skip command name\n";
  moduleContent += "        \n";
  moduleContent += "        while (pos < args.size()) {\n";
  moduleContent += "            std::string arg = std::string(args[pos]);\n";
  moduleContent += "            \n";
  moduleContent += "            if (arg == \"--\") {\n";
  moduleContent += "                ++pos;\n";
  moduleContent += "                break;\n";
  moduleContent += "            }\n";
  moduleContent += "            \n";
  moduleContent += "            if (arg.starts_with(\"--\")) {\n";
  moduleContent += "                // Check long options\n";
  moduleContent += "                bool found = false;\n";
  moduleContent += "                for (const auto& handler : OPTION_HANDLERS) {\n";
  moduleContent += "                    if (handler.long_opt && arg == handler.long_opt) {\n";
  moduleContent += "                        found = true;\n";
  moduleContent += "                        \n";
  moduleContent += "                        if (arg == \"--verbose\") {\n";
  moduleContent += "                            options.verbose = true;\n";
  moduleContent += "                        }\n";
  moduleContent += "                        // Handle more options here\n";
  moduleContent += "                        \n";
  moduleContent += "                        break;\n";
  moduleContent += "                    }\n";
  moduleContent += "                }\n";
  moduleContent += "                \n";
  moduleContent += "                if (!found) {\n";
  moduleContent += "                    fprintf(stderr, \"" + cmdName + ": invalid option '%s'\\\\n\", arg.c_str());\n";
  moduleContent += "                    return options;\n";
  moduleContent += "                }\n";
  moduleContent += "                \n";
  moduleContent += "                ++pos;\n";
  moduleContent += "            } else if (arg.starts_with(\"-\")) {\n";
  moduleContent += "                // Check short options\n";
  moduleContent += "                for (size_t j = 1; j < arg.size(); ++j) {\n";
  moduleContent += "                    char opt_char = arg[j];\n";
  moduleContent += "                    bool found = false;\n";
  moduleContent += "                    \n";
  moduleContent += "                    for (const auto& handler : OPTION_HANDLERS) {\n";
  moduleContent += "                        if (handler.short_opt == opt_char) {\n";
  moduleContent += "                            found = true;\n";
  moduleContent += "                            \n";
  moduleContent += "                            if (opt_char == 'v') {\n";
  moduleContent += "                                options.verbose = true;\n";
  moduleContent += "                            }\n";
  moduleContent += "                            // Handle more options here\n";
  moduleContent += "                            \n";
  moduleContent += "                            break;\n";
  moduleContent += "                        }\n";
  moduleContent += "                    }\n";
  moduleContent += "                    \n";
  moduleContent += "                    if (!found) {\n";
  moduleContent += "                        fprintf(stderr, \"" + cmdName + ": invalid option -- '%c'\\\\n\", opt_char);\n";
  moduleContent += "                        return options;\n";
  moduleContent += "                    }\n";
  moduleContent += "                }\n";
  moduleContent += "                \n";
  moduleContent += "                ++pos;\n";
  moduleContent += "            } else {\n";
  moduleContent += "                break;\n";
  moduleContent += "            }\n";
  moduleContent += "        }\n";
  moduleContent += "        \n";
  moduleContent += "        return options;\n";
  moduleContent += "    }\n";
  moduleContent += "\n";
  moduleContent += "    // Get remaining arguments (files/directories)\n";
  moduleContent += "    std::vector<std::string> paths;\n";
  moduleContent += "    for (size_t i = 1; i < args.size(); ++i) {\n";
  moduleContent += "        std::string arg = std::string(args[i]);\n";
  moduleContent += "        if (arg == \"--\") {\n";
  moduleContent +=
      "            for (size_t j = i + 1; j < args.size(); ++j) {\n";
  moduleContent += "                paths.push_back(std::string(args[j]));\n";
  moduleContent += "            }\n";
  moduleContent += "            break;\n";
  moduleContent += "        }\n";
  moduleContent += "        if (!arg.starts_with(\"-\")) {\n";
  moduleContent += "            paths.push_back(arg);\n";
  moduleContent += "        }\n";
  moduleContent += "    }\n\n";
  moduleContent += "    // Parse options\n";
  moduleContent += "    auto options = parseOptions(args);\n\n";
  moduleContent += "    // TODO: Implement command logic here\n\n";
  moduleContent += "    if (paths.empty()) {\n";
  moduleContent +=
      "        fprintf(stderr, \"" + cmdName + ": missing operand\\\\n\");\n";
  moduleContent += "        return 1;\n";
  moduleContent += "    }\n\n";
  moduleContent += "    // Example: Print paths\n";
  moduleContent += "    for (const auto& path : paths) {\n";
  moduleContent += "        if (options.verbose) {\n";
  moduleContent +=
      "            std::cout << \"Processing: \" << path << std::endl;\n";
  moduleContent += "        }\n";
  moduleContent += "        // TODO: Process path\n";
  moduleContent += "    }\n\n";
  moduleContent += "    return 0;\n";
  moduleContent += "}\n";

  return moduleContent;
}

int main(int argc, char* argv[]) {
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
  for (auto& c : displayCmdName) {
    c = std::toupper(c);
  }
  std::cout << "2. Add options to " << displayCmdName << "_OPTIONS array"
            << std::endl;
  std::cout << "3. Update " << cmdName << "Options struct if needed"
            << std::endl;
  std::cout << "4. Rebuild the project" << std::endl;

  return 0;
}
