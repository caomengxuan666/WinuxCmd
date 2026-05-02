#!/usr/bin/env python3
"""
WinuxCmd Google C++风格统一工具
增强版代码格式化工具 - 支持格式化和命名风格检查
"""

import os
import sys
import re
import argparse
import subprocess
import multiprocessing
import fnmatch
from pathlib import Path

# ============================================================================
# Google C++命名风格正则表达式
# ============================================================================

GOOGLE_NAMING_PATTERNS = {
    # 类名: PascalCase (MyClass, MyStruct)
    'class': re.compile(r'^\s*(class|struct|enum(?:\s+class)?)\s+([A-Z][a-zA-Z0-9_]*)'),
    
    # 函数名: camelCase (myFunction, calculateTotal)
    'function': re.compile(r'^\s*(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?([a-z][a-zA-Z0-9_]*)\s*\([^)]*\)\s*(?:const\s*)?[={;]'),
    
    # 变量名: snake_case (my_variable, file_name)
    'variable': re.compile(r'^\s*(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?([a-z][a-z0-9_]*)(?:\s*[=;,\[)])'),
    
    # 常量: kConstantName 或 UPPER_SNAKE_CASE (kMaxSize, MAX_BUFFER)
    'constant': re.compile(r'^\s*(?:constexpr\s+)?(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?(k[A-Z][a-zA-Z0-9_]*|[A-Z][A-Z0-9_]*)(?:\s*=|\s*;)'),
    
    # 宏: UPPER_SNAKE_CASE (MAX_SIZE, CHECK_CONDITION)
    'macro': re.compile(r'^\s*#\s*define\s+([A-Z][A-Z0-9_]*)\b'),
}

# ============================================================================
# 工具函数
# ============================================================================

def find_clang_format():
    """查找clang-format可执行文件"""
    # 首先检查环境变量
    if "CLANG_FORMAT" in os.environ:
        clang_format = os.environ["CLANG_FORMAT"]
        if os.path.exists(clang_format):
            return clang_format
    
    # 检查系统PATH
    for name in ["clang-format", "clang-format.exe"]:
        try:
            subprocess.run(
                [name, "--version"],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
            )
            return name
        except (subprocess.CalledProcessError, FileNotFoundError):
            continue
    
    return None

def load_gitignore(root_dir):
    """加载.gitignore模式"""
    gitignore_path = os.path.join(root_dir, ".gitignore")
    patterns = []
    
    if os.path.exists(gitignore_path):
        with open(gitignore_path, "r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                
                # 跳过否定模式
                if line.startswith("!"):
                    continue
                
                # 处理绝对路径模式
                if line.startswith("/"):
                    patterns.append(line[1:])  # 移除开头的/
                else:
                    patterns.append(line)
                    # 添加**/pattern用于子目录匹配
                    patterns.append("**/" + line)
                
                # 处理目录模式（以/结尾）
                if line.endswith("/"):
                    patterns.append(line + "**")
                    if not line.startswith("/"):
                        patterns.append("**/" + line + "**")
    
    return patterns

def is_ignored(path, root_dir, ignore_patterns):
    """检查文件是否被.gitignore忽略"""
    try:
        rel_path = os.path.relpath(path, root_dir).replace("\\", "/")
    except ValueError:
        # 处理不同驱动器的情况
        return False
    
    # 手动解析.gitignore模式
    for pattern in ignore_patterns:
        if fnmatch.fnmatch(rel_path, pattern):
            return True
        # 同时检查文件名
        if fnmatch.fnmatch(os.path.basename(path), pattern):
            return True
    
    return False

def find_cpp_files(root_dir, suffixes=None):
    """查找所有C++文件"""
    if suffixes is None:
        suffixes = [".h", ".cpp", ".hpp", ".cc", ".cxx", ".hxx", ".C", ".cppm"]
    
    ignore_patterns = load_gitignore(root_dir)
    files = []
    
    for dirpath, dirnames, filenames in os.walk(root_dir):
        # 排除third_party目录
        dirnames[:] = [d for d in dirnames if "third_party" not in d and "third-party" not in d]
        
        for filename in filenames:
            # 检查文件扩展名
            if not any(filename.endswith(suffix) for suffix in suffixes):
                continue
            
            file_path = os.path.join(dirpath, filename)
            
            # 检查是否被忽略
            if is_ignored(file_path, root_dir, ignore_patterns):
                continue
            
            files.append(file_path)
    
    return files

# ============================================================================
# 命名风格检查
# ============================================================================

def is_pascal_case(name):
    """检查是否为PascalCase"""
    return name and name[0].isupper() and any(c.islower() for c in name[1:])

def is_camel_case(name):
    """检查是否为camelCase"""
    return name and name[0].islower() and any(c.isupper() for c in name[1:])

def is_snake_case(name):
    """检查是否为snake_case"""
    return name and all(c.islower() or c.isdigit() or c == '_' for c in name)

def is_upper_snake_case(name):
    """检查是否为UPPER_SNAKE_CASE"""
    return name and all(c.isupper() or c.isdigit() or c == '_' for c in name)

def check_naming_style(file_path):
    """检查文件的命名风格"""
    issues = []
    
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        for line_num, line in enumerate(lines, 1):
            # 跳过注释和空行
            stripped = line.strip()
            if not stripped or stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*'):
                continue
            
            # 检查类名
            match = GOOGLE_NAMING_PATTERNS['class'].search(line)
            if match:
                type_name, name = match.groups()
                if not is_pascal_case(name):
                    issues.append({
                        'line': line_num,
                        'type': 'class',
                        'name': name,
                        'expected': 'PascalCase',
                        'message': f'{type_name}名 "{name}" 应该使用PascalCase'
                    })
            
            # 检查函数名
            match = GOOGLE_NAMING_PATTERNS['function'].search(line)
            if match:
                name = match.group(1)
                # 跳过构造函数和析构函数
                if not name.startswith('~') and 'operator' not in line:
                    if not is_camel_case(name):
                        issues.append({
                            'line': line_num,
                            'type': 'function',
                            'name': name,
                            'expected': 'camelCase',
                            'message': f'函数名 "{name}" 应该使用camelCase'
                        })
            
            # 检查变量名
            match = GOOGLE_NAMING_PATTERNS['variable'].search(line)
            if match:
                name = match.group(1)
                if not is_snake_case(name):
                    issues.append({
                        'line': line_num,
                        'type': 'variable',
                        'name': name,
                        'expected': 'snake_case',
                        'message': f'变量名 "{name}" 应该使用snake_case'
                    })
            
            # 检查常量名
            match = GOOGLE_NAMING_PATTERNS['constant'].search(line)
            if match:
                name = match.group(1)
                if name.startswith('k'):
                    if not (name[1].isupper() and any(c.islower() for c in name[2:])):
                        issues.append({
                            'line': line_num,
                            'type': 'constant',
                            'name': name,
                            'expected': 'kConstantName',
                            'message': f'常量 "{name}" 应该使用kConstantName格式'
                        })
                else:
                    if not is_upper_snake_case(name):
                        issues.append({
                            'line': line_num,
                            'type': 'constant',
                            'name': name,
                            'expected': 'UPPER_SNAKE_CASE',
                            'message': f'常量 "{name}" 应该使用UPPER_SNAKE_CASE格式'
                        })
            
            # 检查宏名
            match = GOOGLE_NAMING_PATTERNS['macro'].search(line)
            if match:
                name = match.group(1)
                if not is_upper_snake_case(name):
                    issues.append({
                        'line': line_num,
                        'type': 'macro',
                        'name': name,
                        'expected': 'UPPER_SNAKE_CASE',
                        'message': f'宏 "{name}" 应该使用UPPER_SNAKE_CASE格式'
                    })
    
    except Exception as e:
        print(f"检查文件 {file_path} 时出错: {e}", file=sys.stderr)
    
    return issues

# ============================================================================
# 格式化功能
# ============================================================================

def format_file(file_path, clang_format):
    """格式化单个文件"""
    try:
        result = subprocess.run(
            [clang_format, "-i", file_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        return result.returncode == 0
    except Exception as e:
        print(f"格式化文件 {file_path} 时出错: {e}", file=sys.stderr)
        return False

def check_format(file_path, clang_format):
    """检查文件是否需要格式化"""
    try:
        result = subprocess.run(
            [clang_format, "--dry-run", "--Werror", file_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        return result.returncode == 0
    except Exception as e:
        print(f"检查文件 {file_path} 时出错: {e}", file=sys.stderr)
        return False

# ============================================================================
# 主函数
# ============================================================================

def main():
    parser = argparse.ArgumentParser(
        description='WinuxCmd Google C++风格统一工具',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
使用示例:
  # 检查命名风格
  %(prog)s --check-naming --verbose
  
  # 检查代码格式
  %(prog)s --check-format
  
  # 应用格式化
  %(prog)s --format
  
  # 生成完整报告
  %(prog)s --check-naming --check-format --output report.md
        """
    )
    
    parser.add_argument('path', nargs='?', default='.', 
                       help='要处理的目录（默认: 当前目录）')
    
    # 命名风格检查选项
    naming_group = parser.add_argument_group('命名风格检查')
    naming_group.add_argument('--check-naming', action='store_true',
                            help='检查命名风格是否符合Google C++规范')
    naming_group.add_argument('--fix-naming', action='store_true',
                            help='自动修复命名风格（实验性）')
    
    # 代码格式化选项
    format_group = parser.add_argument_group('代码格式化')
    format_group.add_argument('--check-format', action='store_true',
                            help='检查代码格式是否需要调整')
    format_group.add_argument('--format', action='store_true',
                            help='应用代码格式化')
    
    # 输出选项
    output_group = parser.add_argument_group('输出选项')
    output_group.add_argument('--output', '-o', 
                            help='输出报告文件路径')
    output_group.add_argument('--verbose', '-v', action='store_true',
                            help='详细输出模式')
    output_group.add_argument('--quiet', '-q', action='store_true',
                            help='安静模式，只输出错误')
    
    args = parser.parse_args()
    
    # 验证参数
    if not (args.check_naming or args.fix_naming or args.check_format or args.format):
        print("错误: 请指定至少一个操作（--check-naming, --format等）", file=sys.stderr)
        parser.print_help()
        sys.exit(1)
    
    # 获取绝对路径
    root_dir = os.path.abspath(args.path)
    if not os.path.exists(root_dir):
        print(f"错误: 路径不存在: {root_dir}", file=sys.stderr)
        sys.exit(1)
    
    # 查找C++文件
    if not args.quiet:
        print(f"搜索C++文件在: {root_dir}")
    
    files = find_cpp_files(root_dir)
    
    if not files:
        print("错误: 未找到C++文件", file=sys.stderr)
        sys.exit(1)
    
    if not args.quiet:
        print(f"找到 {len(files)} 个C++文件")
    
    # 检查clang-format
    clang_format = None
    if args.check_format or args.format:
        clang_format = find_clang_format()
        if not clang_format:
            print("错误: clang-format未找到，请安装或设置CLANG_FORMAT环境变量", file=sys.stderr)
            sys.exit(1)
        
        if not args.quiet:
            try:
                version = subprocess.run(
                    [clang_format, "--version"],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                ).stdout.strip()
                print(f"使用: {version}")
            except:
                pass
    
    # 执行命名风格检查
    naming_issues = []
    if args.check_naming or args.fix_naming:
        if not args.quiet:
            print("\n" + "="*60)
            print("检查命名风格...")
            print("="*60)
        
        for file_path in files:
            issues = check_naming_style(file_path)
            if issues:
                naming_issues.append((file_path, issues))
                if args.verbose:
                    print(f"\n{file_path}:")
                    for issue in issues:
                        print(f"  Line {issue['line']}: {issue['message']}")
        
        if naming_issues:
            total_issues = sum(len(issues) for _, issues in naming_issues)
            print(f"\n发现 {total_issues} 个命名风格问题:")
            
            # 按类型统计
            issue_types = {}
            for _, issues in naming_issues:
                for issue in issues:
                    issue_type = issue['type']
                    issue_types[issue_type] = issue_types.get(issue_type, 0) + 1
            
            for issue_type, count in sorted(issue_types.items()):
                print(f"  {issue_type}: {count} 个")
        else:
            print("\n✅ 所有文件的命名风格都符合Google C++规范！")
    
    # 执行代码格式检查
    format_issues = []
    if args.check_format:
        if not args.quiet:
            print("\n" + "="*60)
            print("检查代码格式...")
            print("="*60)
        
        needs_formatting = []
        for file_path in files:
            if not check_format(file_path, clang_format):
                needs_formatting.append(file_path)
                if args.verbose:
                    print(f"  {file_path}: 需要格式化")
        
        if needs_formatting:
            print(f"\n发现 {len(needs_formatting)} 个文件需要格式化:")
            for file_path in needs_formatting:
                print(f"  {file_path}")
            format_issues = needs_formatting
        else:
            print("\n✅ 所有文件都已经正确格式化！")
    
    # 应用代码格式化
    if args.format:
        if not args.quiet:
            print("\n" + "="*60)
            print("应用代码格式化...")
            print("="*60)
        
        successful = 0
        failed = 0
        
        # 使用进程池并行处理
        with multiprocessing.Pool(processes=multiprocessing.cpu_count()) as pool:
            results = pool.starmap(format_file, [(f, clang_format) for f in files])
        
        successful = sum(results)
        failed = len(results) - successful
        
        print(f"\n格式化完成:")
        print(f"  ✅ 成功: {successful} 个文件")
        if failed > 0:
            print(f"  ❌ 失败: {failed} 个文件")
    
    # 生成报告
    if args.output and (naming_issues or format_issues):
        with open(args.output, 'w', encoding='utf-8') as f:
            f.write("# Google C++风格检查报告\n\n")
            f.write(f"检查目录: {root_dir}\n")
            f.write(f"检查时间: {subprocess.run(['date'], capture_output=True, text=True).stdout}")
            f.write(f"文件总数: {len(files)}\n\n")
            
            if naming_issues:
                total_naming = sum(len(issues) for _, issues in naming_issues)
                f.write("## 命名风格问题\n")
                f.write(f"发现问题: {total_naming} 个\n\n")
                
                # 按类型统计
                issue_types = {}
                for _, issues in naming_issues:
                    for issue in issues:
                        issue_type = issue['type']
                        issue_types[issue_type] = issue_types.get(issue_type, 0) + 1
                
                f.write("### 问题分类\n")
                for issue_type, count in sorted(issue_types.items()):
                    f.write(f"- {issue_type}: {count} 个\n")
                
                f.write("\n### 详细问题列表\n")
                for file_path, issues in naming_issues:
                    f.write(f"\n#### {file_path}\n")
                    for issue in issues:
                        f.write(f"- **Line {issue['line']}**: {issue['message']}\n")
            
            if format_issues:
                f.write("\n## 代码格式问题\n")
                f.write(f"需要格式化的文件: {len(format_issues)} 个\n\n")
                f.write("### 文件列表\n")
                for file_path in format_issues:
                    f.write(f"- {file_path}\n")
            
            if not naming_issues and not format_issues:
                f.write("## 检查结果\n")
                f.write("✅ 所有检查通过！代码符合Google C++风格规范。\n")
        
        if not args.quiet:
            print(f"\n报告已保存到: {args.output}")
    
    # 退出状态
    if naming_issues or format_issues:
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == '__main__':
    main()