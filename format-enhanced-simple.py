#!/usr/bin/env python3
"""
简化版命名风格检查工具
检查C++代码是否符合Google C++命名规范
"""

import os
import sys
import re
import argparse

# Google C++命名风格正则表达式
GOOGLE_PATTERNS = {
    'class': re.compile(r'^\s*(class|struct|enum(?:\s+class)?)\s+([A-Z][a-zA-Z0-9_]*)'),
    'function': re.compile(r'^\s*(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?([a-z][a-zA-Z0-9_]*)\s*\([^)]*\)\s*(?:const\s*)?[={;]'),
    'variable': re.compile(r'^\s*(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?([a-z][a-z0-9_]*)(?:\s*[=;,\[)])'),
    'constant': re.compile(r'^\s*(?:constexpr\s+)?(?:[a-zA-Z_][a-zA-Z0-9_<>:]*\s+)?(k[A-Z][a-zA-Z0-9_]*|[A-Z][A-Z0-9_]*)(?:\s*=|\s*;)'),
    'macro': re.compile(r'^\s*#\s*define\s+([A-Z][A-Z0-9_]*)\b'),
}

def is_pascal_case(name):
    """检查是否为PascalCase"""
    return name[0].isupper() and any(c.islower() for c in name[1:])

def is_camel_case(name):
    """检查是否为camelCase"""
    return name[0].islower() and any(c.isupper() for c in name[1:])

def is_snake_case(name):
    """检查是否为snake_case"""
    return all(c.islower() or c.isdigit() or c == '_' for c in name)

def is_upper_snake_case(name):
    """检查是否为UPPER_SNAKE_CASE"""
    return all(c.isupper() or c.isdigit() or c == '_' for c in name)

def check_file(file_path):
    """检查单个文件的命名风格"""
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
            match = GOOGLE_PATTERNS['class'].search(line)
            if match:
                type_name, name = match.groups()
                if not is_pascal_case(name):
                    issues.append({
                        'line': line_num,
                        'type': 'class',
                        'name': name,
                        'expected': 'PascalCase',
                        'actual': name,
                        'suggestion': name[0].upper() + ''.join(c.lower() if i > 0 else c for i, c in enumerate(name[1:]))
                    })
            
            # 检查函数名
            match = GOOGLE_PATTERNS['function'].search(line)
            if match:
                name = match.group(1)
                # 跳过构造函数和析构函数
                if not name.startswith('~') and 'operator' not in line:
                    if not is_camel_case(name):
                        # 尝试转换为camelCase
                        suggestion = name[0].lower()
                        next_upper = False
                        for c in name[1:]:
                            if c == '_':
                                next_upper = True
                            elif next_upper:
                                suggestion += c.upper()
                                next_upper = False
                            else:
                                suggestion += c.lower()
                        
                        issues.append({
                            'line': line_num,
                            'type': 'function',
                            'name': name,
                            'expected': 'camelCase',
                            'actual': name,
                            'suggestion': suggestion
                        })
            
            # 检查变量名
            match = GOOGLE_PATTERNS['variable'].search(line)
            if match:
                name = match.group(1)
                if not is_snake_case(name):
                    # 尝试转换为snake_case
                    suggestion = ''
                    for i, c in enumerate(name):
                        if c.isupper() and i > 0:
                            suggestion += '_' + c.lower()
                        else:
                            suggestion += c.lower()
                    
                    issues.append({
                        'line': line_num,
                        'type': 'variable',
                        'name': name,
                        'expected': 'snake_case',
                        'actual': name,
                        'suggestion': suggestion
                    })
            
            # 检查常量名
            match = GOOGLE_PATTERNS['constant'].search(line)
            if match:
                name = match.group(1)
                if name.startswith('k'):
                    if not (name[1].isupper() and any(c.islower() for c in name[2:])):
                        issues.append({
                            'line': line_num,
                            'type': 'constant',
                            'name': name,
                            'expected': 'kConstantName',
                            'actual': name,
                            'suggestion': 'k' + name[1:].capitalize() if len(name) > 1 else name
                        })
                else:
                    if not is_upper_snake_case(name):
                        issues.append({
                            'line': line_num,
                            'type': 'constant',
                            'name': name,
                            'expected': 'UPPER_SNAKE_CASE',
                            'actual': name,
                            'suggestion': name.upper()
                        })
            
            # 检查宏名
            match = GOOGLE_PATTERNS['macro'].search(line)
            if match:
                name = match.group(1)
                if not is_upper_snake_case(name):
                    issues.append({
                        'line': line_num,
                        'type': 'macro',
                        'name': name,
                        'expected': 'UPPER_SNAKE_CASE',
                        'actual': name,
                        'suggestion': name.upper()
                    })
    
    except Exception as e:
        print(f"检查文件 {file_path} 时出错: {e}", file=sys.stderr)
    
    return issues

def find_cpp_files(root_dir):
    """查找所有C++文件"""
    cpp_files = []
    for dirpath, _, filenames in os.walk(root_dir):
        # 跳过第三方目录
        if 'third_party' in dirpath or 'third-party' in dirpath:
            continue
        
        for filename in filenames:
            if filename.endswith(('.cpp', '.h', '.hpp', '.cc', '.cxx', '.hxx')):
                file_path = os.path.join(dirpath, filename)
                cpp_files.append(file_path)
    
    return cpp_files

def main():
    parser = argparse.ArgumentParser(description='Google C++命名风格检查工具')
    parser.add_argument('path', nargs='?', default='.', help='要检查的目录或文件')
    parser.add_argument('--output', '-o', help='输出报告文件')
    parser.add_argument('--verbose', '-v', action='store_true', help='详细输出')
    parser.add_argument('--fix', action='store_true', help='自动修复（实验性）')
    
    args = parser.parse_args()
    
    # 确定要检查的文件
    if os.path.isfile(args.path):
        files = [args.path]
    else:
        files = find_cpp_files(args.path)
    
    print(f"找到 {len(files)} 个C++文件")
    
    # 检查所有文件
    all_issues = []
    for file_path in files:
        issues = check_file(file_path)
        if issues:
            all_issues.append((file_path, issues))
            if args.verbose:
                print(f"\n{file_path}:")
                for issue in issues:
                    print(f"  Line {issue['line']}: {issue['type']} '{issue['name']}'")
                    print(f"    期望: {issue['expected']}, 实际: {issue['actual']}")
                    if 'suggestion' in issue:
                        print(f"    建议: {issue['suggestion']}")
    
    # 输出总结
    if all_issues:
        total_issues = sum(len(issues) for _, issues in all_issues)
        print(f"\n{'='*60}")
        print(f"发现 {total_issues} 个命名风格问题:")
        print('='*60)
        
        issue_types = {}
        for file_path, issues in all_issues:
            for issue in issues:
                issue_type = issue['type']
                issue_types[issue_type] = issue_types.get(issue_type, 0) + 1
        
        for issue_type, count in sorted(issue_types.items()):
            print(f"  {issue_type}: {count} 个")
        
        # 输出报告文件
        if args.output:
            with open(args.output, 'w', encoding='utf-8') as f:
                f.write("# Google C++命名风格检查报告\n\n")
                f.write(f"检查目录: {os.path.abspath(args.path)}\n")
                f.write(f"文件总数: {len(files)}\n")
                f.write(f"发现问题: {total_issues}\n\n")
                
                f.write("## 问题分类统计\n")
                for issue_type, count in sorted(issue_types.items()):
                    f.write(f"- {issue_type}: {count} 个\n")
                
                f.write("\n## 详细问题列表\n")
                for file_path, issues in all_issues:
                    f.write(f"\n### {file_path}\n")
                    for issue in issues:
                        f.write(f"- **Line {issue['line']}**: {issue['type']} `{issue['name']}`\n")
                        f.write(f"  - 期望: {issue['expected']}\n")
                        f.write(f"  - 实际: {issue['actual']}\n")
                        if 'suggestion' in issue:
                            f.write(f"  - 建议: `{issue['suggestion']}`\n")
                
                print(f"\n报告已保存到: {args.output}")
        
        if args.fix:
            print("\n注意: 自动修复功能还在开发中")
            print("建议手动修复这些问题")
        
        sys.exit(1)
    else:
        print(f"\n✅ 所有文件的命名风格都符合Google C++规范！")
        sys.exit(0)

if __name__ == '__main__':
    main()