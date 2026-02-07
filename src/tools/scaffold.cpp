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
  moduleContent += "#include \"core/command_macros.h\"\n\n";
  moduleContent += "export module cmd:" + cmdName + ";\n\n";
  moduleContent += "import std;\n";
  moduleContent += "import core;\n";
  moduleContent += "import utils;\n";
  moduleContent += "namespace fs = std::filesystem;\n\n";
  moduleContent += "using cmd::meta::OptionMeta;\n";
  moduleContent += "using cmd::meta::OptionType;\n\n";

  // Convert command name to uppercase for constant name
  std::string upperCmdName = cmdName;
  for (auto &c : upperCmdName) {
    c = std::toupper(c);
  }

  // Add constants namespace
  moduleContent += "// ======================================================\n";
  moduleContent += "// Constants\n";
  moduleContent += "// ======================================================\n";
  moduleContent += "namespace " + cmdName + "_constants {\n";
  moduleContent += "  // Add constants here\n";
  moduleContent += "}\n\n";

  // Add options
  moduleContent += "// ======================================================\n";
  moduleContent += "// Options (constexpr)\n";
  moduleContent += "// ======================================================\n\n";
  moduleContent += "export auto constexpr " + upperCmdName + "_OPTIONS =\n";
  moduleContent += "    std::array{OPTION(\"-v\", \"--verbose\", \"explain what is being done\"),\n";
  moduleContent += "               // Add more options here\n";
  moduleContent += "               // Example with argument: OPTION(\"-n\", \"--number\", \"specify a number\", INT_TYPE)\n";
  moduleContent += "              };\n\n";

  // Add pipeline components
  moduleContent += "// ======================================================\n";
  moduleContent += "// Pipeline components\n";
  moduleContent += "// ======================================================\n";
  moduleContent += "namespace " + cmdName + "_pipeline {\n";
  moduleContent += "   namespace cp=core::pipeline;\n";
  moduleContent += "  \n";
  moduleContent += "  // ----------------------------------------------\n";
  moduleContent += "  // 1. Validate arguments\n";
  moduleContent += "  // ----------------------------------------------\n";
  moduleContent += "  auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {\n";
  moduleContent += "    if (args.empty()) return std::unexpected(\"no arguments provided\");\n";
  moduleContent += "    std::vector<std::string> paths;\n";
  moduleContent += "    for (auto arg : args) {\n";
  moduleContent += "      paths.push_back(std::string(arg));\n";
  moduleContent += "    }\n";
  moduleContent += "    return paths;\n";
  moduleContent += "  }\n\n";
  moduleContent += "  // ----------------------------------------------\n";
  moduleContent += "  // 2. Main pipeline\n";
  moduleContent += "  // ----------------------------------------------\n";
  moduleContent += "  template<size_t N>\n";
  moduleContent += "  auto process_command(const CommandContext<N>& ctx)\n";
  moduleContent += "      -> cp::Result<std::vector<std::string>>\n";
  moduleContent += "  {\n";
  moduleContent += "    return validate_arguments(ctx.positionals)\n";
  moduleContent += "        // Add more pipeline steps here\n";
  moduleContent += "        ;\n";
  moduleContent += "  }\n\n";
  moduleContent += "}\n\n";

  // Add command registration
  moduleContent += "// ======================================================\n";
  moduleContent += "// Command registration\n";
  moduleContent += "// ======================================================\n\n";
  moduleContent += "REGISTER_COMMAND(" + cmdName + ",\n";
  moduleContent += "                 /* name */\n";
  moduleContent += "                 \"" + cmdName + "\",\n\n";
  moduleContent += "                 /* synopsis */\n";
  moduleContent += "                 \"TODO: Add command synopsis\",\n\n";
  moduleContent += "                 /* description */\n";
  moduleContent += "                 \"TODO: Add command description. With no arguments, "
                   "this command does nothing. With arguments, this command processes "
                   "them.\",\n\n";
  moduleContent += "                 /* examples */\n";
  moduleContent += "                 \"  " + cmdName + "                      TODO: Add example\\n"
                   "  " + cmdName + " -v                   TODO: Add example with verbose\",\n\n";
  moduleContent += "                 /* see_also */\n";
  moduleContent += "                 \"TODO: Add see also\",\n\n";
  moduleContent += "                 /* author */\n";
  moduleContent += "                 \"TODO: Add your name\",\n\n";
  moduleContent += "                 /* copyright */\n";
  moduleContent += "                 \"Copyright © 2026 WinuxCmd\",\n\n";
  moduleContent += "                 /* options */\n";
  moduleContent += "                 " + upperCmdName + "_OPTIONS\n";
  moduleContent += ") {\n";
  moduleContent += "  using namespace " + cmdName + "_pipeline;\n\n";
  moduleContent += "  auto result = process_command(ctx);\n";
  moduleContent += "  if (!result) {\n";
  moduleContent += "    cp::report_error(result, L\"" + cmdName + "\");\n";
  moduleContent += "    return 1;\n";
  moduleContent += "  }\n\n";
  moduleContent += "  auto paths = *result;\n\n";
  moduleContent += "  // TODO: Implement command logic here\n\n";
  moduleContent += "  // Example: Print paths\n";
  moduleContent += "  for (const auto& path : paths) {\n";
  moduleContent += "    if (ctx.get<bool>(\"--verbose\", false)) {\n";
  moduleContent += "      safePrintLn(L\"Processing: \" + utf8_to_wstring(path));\n";
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
  std::cout << "3. Update pipeline components as needed" << std::endl;
  std::cout << "4. Rebuild the project" << std::endl;

  return 0;
}
