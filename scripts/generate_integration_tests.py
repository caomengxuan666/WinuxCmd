#!/usr/bin/env python3
"""
WinuxCmd Integration Test Generator
Automatically generate pipeline integration test cases via permutations and combinations
"""
import itertools
from typing import List, Dict, Tuple

# ============================================================================
# Command Definitions: Parameter templates for each command
# ============================================================================

COMMANDS = {
    # Basic commands
    "cat": {
        "description": "Concatenate and display file contents",
        "args": ["input.txt"],
        "optional_args": [""],
    },
    "head": {
        "description": "Display the beginning of a file",
        "args": ["-n", "5"],
        "optional_args": ["-n", "3", "-n", "10", ""],
    },
    "tail": {
        "description": "Display the end of a file",
        "args": ["-n", "5"],
        "optional_args": ["-n", "3", "-n", "10", ""],
    },
    "wc": {
        "description": "Count lines, words, and bytes",
        "args": ["-l"],
        "optional_args": ["-l", "-w", "-c", "-m", ""],
    },
    "grep": {
        "description": "Search for text patterns",
        "args": ["apple"],
        "optional_args": ["apple", "banana", "ERROR", "-i", "apple", "-i", "error", "-v", "apple", ""],
    },
    "sort": {
        "description": "Sort text lines",
        "args": [],
        "optional_args": ["-r", "-n", "-u", ""],
    },
    "uniq": {
        "description": "Filter duplicate lines",
        "args": [],
        "optional_args": ["-c", "-d", "-u", ""],
    },
    "cut": {
        "description": "Extract fields",
        "args": ["-f", "1"],
        "optional_args": ["-f", "1", "-f", "2", "-f", "1,2", "-f", "1-3", "-d", ",", "-f", "1"],
    },
    "sed": {
        "description": "Stream editor",
        "args": ["s/apple/APPLE/"],
        "optional_args": ["s/apple/APPLE/", "s/^/PREFIX: /", "s/$/: SUFFIX/", "d", ""],
    },
    "tee": {
        "description": "Read and write",
        "args": ["output.txt"],
        "optional_args": ["output.txt", "tee_output.txt", ""],
    },
    "xargs": {
        "description": "Build and execute commands",
        "args": ["echo"],
        "optional_args": ["echo", "-n", "1", "-P", "2", "echo"],
    },
}

# ============================================================================
# Test Data Definitions
# ============================================================================

TEST_DATA = {
    "sample": "apple\nbanana\ncherry\ndate\nelderberry\nfig\ngrape\n",
    "numbers": "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n",
    "log": "[INFO] Starting process\n[ERROR] File not found\n[INFO] Retrying\n[ERROR] Timeout occurred\n[INFO] Completed\n",
    "csv": "Alice,25,Engineer\nBob,30,Designer\nCharlie,28,Developer\n",
    "duplicates": "apple\napple\nbanana\nbanana\ncherry\napple\n",
    "mixed": "hello\n123\nworld\n456\ntest\n",
}

# ============================================================================
# Code Generator
# ============================================================================

class IntegrationTestGenerator:
    def __init__(self):
        self.test_count = 0
        self.tests = []

    def generate_test_name(self, cmd1: str, cmd2: str, suffix: str = "") -> str:
        """Generate test name"""
        name = f"{cmd1}_to_{cmd2}"
        if suffix:
            name += f"_{suffix}"
        return name

    def format_wstring_list(self, args: List[str]) -> str:
        """Format C++ wide string list"""
        if not args:
            return "{}"
        return "{" + ", ".join(f'L"{arg}"' for arg in args) + "}"

    def generate_test_case_2cmd(self, cmd1: str, cmd2: str, args1: List[str], args2: List[str],
                                 test_data: str, test_file: str = "input.txt", suffix: str = ""):
        """Generate two-command pipeline test"""
        self.test_count += 1
        test_name = self.generate_test_name(cmd1, cmd2, suffix)

        escaped_data = test_data.replace('"', '\\"').replace('\n', '\\n')
        test_code = f"""
TEST(pipeline_2cmd, {test_name}) {{
  TempDir tmp;
  tmp.write("{test_file}", "{escaped_data}");
  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"{cmd1}.exe", {self.format_wstring_list(args1)});
  p.add(L"{cmd2}.exe", {self.format_wstring_list(args2)});
  auto r = p.run();
  EXPECT_EXIT_CODE(r, 0);
  TEST_LOG("{cmd1} -> {cmd2} output", r.stdout_text);
}}
"""
        self.tests.append(test_code)

    def generate_test_case_3cmd(self, cmd1: str, cmd2: str, cmd3: str,
                                 args1: List[str], args2: List[str], args3: List[str],
                                 test_data: str, test_file: str = "input.txt", suffix: str = ""):
        """Generate three-command pipeline test"""
        self.test_count += 1
        test_name = f"{cmd1}_to_{cmd2}_to_{cmd3}_{suffix}" if suffix else f"{cmd1}_to_{cmd2}_to_{cmd3}"

        escaped_data = test_data.replace('"', '\\"').replace('\n', '\\n')
        test_code = f"""
TEST(pipeline_3cmd, {test_name}) {{
  TempDir tmp;
  tmp.write("{test_file}", "{escaped_data}");
  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"{cmd1}.exe", {self.format_wstring_list(args1)});
  p.add(L"{cmd2}.exe", {self.format_wstring_list(args2)});
  p.add(L"{cmd3}.exe", {self.format_wstring_list(args3)});
  auto r = p.run();
  EXPECT_EXIT_CODE(r, 0);
  TEST_LOG("{cmd1} -> {cmd2} -> {cmd3} output", r.stdout_text);
}}
"""
        self.tests.append(test_code)

    def generate_all_2cmd_combinations(self):
        """Generate all two-command pipeline combinations"""
        print("Generating two-command pipeline tests...")

        cmd_names = list(COMMANDS.keys())
        count = 0
        max_tests = 200  # Generate up to 200 tests

        for i, cmd1 in enumerate(cmd_names):
            for cmd2 in cmd_names:
                if cmd1 == cmd2:
                    continue  # Skip same command combinations

                if count >= max_tests:
                    break

                cmd1_def = COMMANDS[cmd1]
                cmd2_def = COMMANDS[cmd2]

                # Use sample data
                args1 = cmd1_def["args"] if cmd1_def["args"] else []
                args2 = cmd2_def["args"] if cmd2_def["args"] else []

                # Commands that need file input when in first position
                # These commands need input from somewhere (file or stdin)
                input_commands = ["cat", "head", "tail", "grep", "sort", "uniq", "cut", "sed", "wc"]

                # Special handling: ensure first command has file parameter if needed
                # For commands that read from stdin/file, add input.txt when in first position
                if cmd1 in input_commands:
                    if cmd1 == "cat":
                        # cat special case: if no args or no file specified
                        if not args1 or not any(arg.endswith(".txt") for arg in args1):
                            args1 = ["input.txt"] + args1
                    else:
                        # head, tail, grep, etc. need file parameter
                        args1 = args1 + ["input.txt"]

                # Second command in pipeline should read from stdin
                # Remove file parameters but keep required arguments
                if cmd2 in input_commands:
                    if cmd2 == "cat":
                        # cat reads from stdin, no args needed
                        args2 = []
                    elif cmd2 in ["head", "tail", "sort", "uniq", "wc"]:
                        # These commands can work with just their args (like -n 5) on stdin
                        # Keep their args but remove any file parameters
                        args2 = [arg for arg in args2 if not arg.endswith(".txt")]
                    elif cmd2 in ["grep", "cut", "sed"]:
                        # These commands need their required arguments (pattern, etc.)
                        # Keep all non-file arguments
                        args2 = [arg for arg in args2 if not arg.endswith(".txt")]
                    else:
                        args2 = []

                self.generate_test_case_2cmd(
                    cmd1, cmd2, args1, args2, TEST_DATA["sample"],
                    "input.txt", suffix=f"basic_{count}"
                )

                # Generate variations with optional args
                # Parse optional_args into pairs (flag, value) or single flags
                def parse_optional_args(opt_list):
                    """Parse optional args list into valid argument combinations"""
                    result = []
                    i = 0
                    while i < len(opt_list):
                        arg = opt_list[i]
                        if not arg:
                            # Empty string means no optional args
                            result.append([])
                            i += 1
                        elif arg.startswith("-"):
                            # This is a flag, check if next item is its value
                            if i + 1 < len(opt_list) and not opt_list[i + 1].startswith("-"):
                                # Pair: flag and value
                                result.append([arg, opt_list[i + 1]])
                                i += 2
                            else:
                                # Standalone flag
                                result.append([arg])
                                i += 1
                        else:
                            # Non-flag value (like grep pattern)
                            result.append([arg])
                            i += 1
                    return result

                opt_list1 = parse_optional_args(cmd1_def["optional_args"])
                opt_list2 = parse_optional_args(cmd2_def["optional_args"])

                # Generate combinations of optional args
                for opt_pair1 in opt_list1[:3]:  # Limit to 3 variations
                    for opt_pair2 in opt_list2[:3]:
                        if count >= max_tests:
                            break
                        # Skip if both are empty (no optional args)
                        if not opt_pair1 and not opt_pair2:
                            continue

                        args1_opt = cmd1_def["args"] if cmd1_def["args"] else []
                        args2_opt = cmd2_def["args"] if cmd2_def["args"] else []

                        # Commands that need file input when in first position
                        input_commands = ["cat", "head", "tail", "grep", "sort", "uniq", "cut", "sed", "wc"]

                        # Add file parameter for first command if needed
                        if cmd1 in input_commands:
                            if cmd1 == "cat":
                                if not args1_opt or not any(arg.endswith(".txt") for arg in args1_opt):
                                    args1_opt = ["input.txt"] + args1_opt
                            else:
                                args1_opt = args1_opt + ["input.txt"]

                        # Second command reads from stdin
                        # Remove file parameters but keep required arguments
                        if cmd2 in input_commands:
                            if cmd2 == "cat":
                                args2_opt = []
                            elif cmd2 in ["head", "tail", "sort", "uniq", "wc"]:
                                args2_opt = [arg for arg in args2_opt if not arg.endswith(".txt")]
                            elif cmd2 in ["grep", "cut", "sed"]:
                                args2_opt = [arg for arg in args2_opt if not arg.endswith(".txt")]
                            else:
                                args2_opt = []

                        count += 1
                        # Create suffix from opt pairs
                        opt1_str = "_".join(opt_pair1) if opt_pair1 else "none"
                        opt2_str = "_".join(opt_pair2) if opt_pair2 else "none"
                        # Sanitize suffix
                        opt1_clean = ''.join(c if c.isalnum() else '_' for c in opt1_str)
                        opt2_clean = ''.join(c if c.isalnum() else '_' for c in opt2_str)
                        suffix_opt = f"opt_{count}_{opt1_clean}_{opt2_clean}"

                        self.generate_test_case_2cmd(
                            cmd1, cmd2, args1_opt + opt_pair1, args2_opt + opt_pair2,
                            TEST_DATA["sample"], "input.txt",
                            suffix=suffix_opt
                        )

                count += 1

            if count >= max_tests:
                break

    def generate_all_3cmd_combinations(self):
        """Generate all three-command pipeline combinations"""
        print("Generating three-command pipeline tests...")

        # Select common commands for three-command combinations
        common_cmds = ["cat", "grep", "head", "sort", "tail", "wc"]
        count = 0
        max_tests = 100  # Generate up to 100 tests

        for i, cmd1 in enumerate(common_cmds):
            for cmd2 in common_cmds:
                if cmd1 == cmd2:
                    continue
                for cmd3 in common_cmds:
                    if cmd2 == cmd3 or cmd1 == cmd3:
                        continue

                    if count >= max_tests:
                        break

                    cmd1_def = COMMANDS[cmd1]
                    cmd2_def = COMMANDS[cmd2]
                    cmd3_def = COMMANDS[cmd3]

                    # Use sample data
                    args1 = cmd1_def["args"] if cmd1_def["args"] else []
                    args2 = cmd2_def["args"] if cmd2_def["args"] else []
                    args3 = cmd3_def["args"] if cmd3_def["args"] else []

                    # Special handling
                    if cmd1 == "cat" and not args1:
                        args1 = ["input.txt"]

                    self.generate_test_case_3cmd(
                        cmd1, cmd2, cmd3, args1, args2, args3,
                        TEST_DATA["sample"], "input.txt"
                    )

                    count += 1

                if count >= max_tests:
                    break

            if count >= max_tests:
                break

    def generate_real_world_tests(self):
        """Generate real-world scenario tests"""
        print("Generating real-world scenario tests...")

        # Log analysis
        self.generate_test_case_3cmd(
            "cat", "grep", "wc",
            ["input.txt"], ["ERROR"], ["-l"],
            TEST_DATA["log"], "log.txt", suffix="log_errors"
        )

        # Data processing
        self.generate_test_case_3cmd(
            "cat", "sort", "uniq",
            ["input.txt"], [], ["-c"],
            TEST_DATA["duplicates"], "data.txt", suffix="sort_uniq_count"
        )

        # CSV processing
        self.generate_test_case_3cmd(
            "cat", "grep", "cut",
            ["input.txt"], ["Engineer"], ["-d", ",", "-f", "1"],
            TEST_DATA["csv"], "employees.csv", suffix="csv_extract"
        )

        # Multiple grep filters
        self.generate_test_case_3cmd(
            "cat", "grep", "sort",
            ["input.txt"], ["apple"], [],
            TEST_DATA["sample"], "data.txt", suffix="grep_sort"
        )

        # Count lines after filtering
        self.generate_test_case_3cmd(
            "cat", "grep", "wc",
            ["input.txt"], ["banana"], ["-l"],
            TEST_DATA["sample"], "data.txt", suffix="grep_wc"
        )

        # Sort and deduplicate
        self.generate_test_case_3cmd(
            "cat", "sort", "uniq",
            ["input.txt"], ["-r"], [],
            TEST_DATA["sample"], "data.txt", suffix="sort_reverse_uniq"
        )

    def generate_stress_tests(self):
        """Generate stress tests"""
        print("Generating stress tests...")

        # Long pipeline
        self.generate_test_case_3cmd(
            "cat", "sort", "uniq",
            ["input.txt"], [], [],
            TEST_DATA["sample"], "large.txt", suffix="long_pipeline"
        )

        # Multiple grep filters
        self.generate_test_case_3cmd(
            "cat", "grep", "wc",
            ["input.txt"], ["a"], ["-l"],
            TEST_DATA["sample"], "multi_grep.txt", suffix="multi_filter"
        )

        # Complex data transformation
        self.generate_test_case_3cmd(
            "cat", "sed", "sort",
            ["input.txt"], ["s/apple/APPLE/"], [],
            TEST_DATA["sample"], "transform.txt", suffix="sed_sort"
        )

        # Number processing
        self.generate_test_case_3cmd(
            "cat", "head", "wc",
            ["input.txt"], ["-n", "3"], ["-l"],
            TEST_DATA["numbers"], "numbers.txt", suffix="head_wc"
        )

    def generate_edge_case_tests(self):
        """Generate edge case tests"""
        print("Generating edge case tests...")

        # Empty data handling
        self.generate_test_case_2cmd(
            "cat", "wc", [], [], "",
            "empty.txt", suffix="empty_data"
        )

        # Single line
        self.generate_test_case_2cmd(
            "cat", "head", [], ["-n", "1"],
            "single_line\n", "single.txt", suffix="single_line"
        )

        # No matches
        self.generate_test_case_2cmd(
            "cat", "grep", [], ["xyz123"],
            TEST_DATA["sample"], "input.txt", suffix="no_match"
        )

        # Case insensitive
        self.generate_test_case_2cmd(
            "cat", "grep", [], ["-i", "APPLE"],
            TEST_DATA["sample"], "input.txt", suffix="case_insensitive"
        )

    def generate_header(self):
        """Generate file header"""
        return """// ============================================================================
// WinuxCmd Integration Tests - Auto Generated
// Generated by scripts/generate_integration_tests.py
// DO NOT modify this file manually
// ============================================================================

#include "framework/winuxtest.h"

// ============================================================================
// Test Data Helper Functions
// ============================================================================

inline std::string get_sample_data() {
  return "apple\\nbanana\\ncherry\\ndate\\nelderberry\\nfig\\ngrape\\n";
}

inline std::string get_log_data() {
  return "[INFO] Starting process\\n[ERROR] File not found\\n[INFO] Retrying\\n[ERROR] Timeout occurred\\n[INFO] Completed\\n";
}

inline std::string get_csv_data() {
  return "Alice,25,Engineer\\nBob,30,Designer\\nCharlie,28,Developer\\n";
}

inline std::string get_numbered_data() {
  return "1\\n2\\n3\\n4\\n5\\n6\\n7\\n8\\n9\\n10\\n";
}

inline std::string get_duplicate_data() {
  return "apple\\napple\\nbanana\\nbanana\\ncherry\\napple\\n";
}

inline std::string get_mixed_data() {
  return "hello\\n123\\nworld\\n456\\ntest\\n";
}

// ============================================================================
// Two-Command Pipeline Tests
// ============================================================================

"""

    def generate_footer(self):
        """Generate file footer"""
        return f"""
// ============================================================================
// Test Statistics
// ============================================================================
// Total generated: {self.test_count} test cases
// ============================================================================
"""

    def generate_file(self, output_path: str):
        """Generate complete test file"""
        print(f"Starting to generate integration test file: {output_path}")
        print("=" * 60)

        # Generate all tests
        self.generate_all_2cmd_combinations()
        self.generate_all_3cmd_combinations()
        self.generate_real_world_tests()
        self.generate_stress_tests()
        self.generate_edge_case_tests()

        print("=" * 60)
        print(f"Generation complete! Total {self.test_count} test cases generated")

        # Write to file
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(self.generate_header())
            for test in self.tests:
                f.write(test)
            f.write(self.generate_footer())

        print(f"File written to: {output_path}")


# ============================================================================
# Main Program
# ============================================================================

if __name__ == "__main__":
    import sys

    output_file = "tests/integration/pipeline_integration_test.cpp"

    if len(sys.argv) > 1:
        output_file = sys.argv[1]

    generator = IntegrationTestGenerator()
    generator.generate_file(output_file)

    print("\nDone!")
