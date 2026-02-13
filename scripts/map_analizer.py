#!/usr/bin/env python3
"""
MSVC Map File Profiler
==========================================
Complete analysis tool for MSVC .map files with visualization and deep inspection.

Features:
    - Precise symbol size calculation using section table
    - 18-category classification with no double counting
    - Template instantiation hotspot analysis
    - COMDAT folding estimation (/OPT:ICF)
    - STL overhead attribution per object file
    - Console output with ASCII progress bars
    - CSV export for all symbols
    - JSON export with summary and optional full symbol list
    - Matplotlib charts (pie chart, bar chart)
    - Standalone HTML report with ECharts
    - Multi-file comparison
"""

import re
import sys
import os
import argparse
import json
import csv
from collections import defaultdict
from pathlib import Path

# ============================================================================
# Optional dependencies
# ============================================================================
try:
    import matplotlib.pyplot as plt
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False

try:
    import colorama
    from colorama import Fore, Style
    colorama.init()
    HAS_COLORAMA = True
except ImportError:
    HAS_COLORAMA = False


# ============================================================================
# Constants & Classification Rules
# ============================================================================

# Category priority: symbols are only assigned to the FIRST matching category
CATEGORY_PRIORITY = [
    ("exception",           ["throw", "catch", "ehstate", "xthrow", "uncaught",
                             "_tls", "seh_", "CxxFrameHandler"]),
    ("rtti_vtable",        ["??_7", "UEAA", "RTTI", "type_info", "`vftable'",
                            "`RTTI Complete Object Locator'"]),
    ("std_function",       ["_Func_impl", "lambda", "operator()", "?R", "?A0x"]),
    ("unordered_map",      ["unordered_map", "_Hash", "_Umap", "registry", "_HashMap"]),
    ("stl_string",         ["?$basic_string", "?$char_traits", "?$allocator"]),
    ("stl_vector",         ["?$vector", "?$_Vector", "?$_Reallocate"]),
    ("stl_other",          ["?$list", "?$deque", "?$set", "?$map", "?$_Tree", "?$_List"]),
    ("module_metadata",    ["?__CxxModule", "cppm.obj"]),
    ("crt_startup",        ["_crt_", "__scrt_", "mainCRTStartup", "WinMainCRTStartup"]),
    ("thread_local",       ["_tls_", "TLS", "thread_local"]),
    ("guard",             ["_guard", "__guard", "___guard"]),
    ("global_ctor",       ["`dynamic initializer'", "`dynamic atexit destructor'"]),
    ("virtual_inline",    ["*::`vcall'", "*::`scalar deleting destructor'"]),
    ("code_my",           []),  # Business logic - matched after excluding libs
    ("code_lib",          []),  # Static library code (CRT, etc)
    ("data",             ["?g_", "?s_", "?_"]),
    ("other",            []),
]

CATEGORY_NAMES = [c[0] for c in CATEGORY_PRIORITY]
CATEGORY_PATTERNS = [c[1] for c in CATEGORY_PRIORITY]

# Object files that are considered part of the C/C++ runtime
LIB_OBJ_KEYWORDS = ["libc", "libvcruntime", "libucrt", "msvcrt", "crt"]


# ============================================================================
# Core Parser
# ============================================================================
class MapParser:
    """MSVC .map file parser with section table and symbol resolution."""

    SECTION_LINE_RE = re.compile(
        r'^\s*([0-9A-F]{4}):([0-9A-F]{8})\s+([0-9A-F]{6,8}H?)\s+(\S+)\s+(\S+)',
        re.IGNORECASE
    )

    SYMBOL_LINE_RE = re.compile(
        r'^\s*([0-9A-F]{4}):([0-9A-F]{8})\s+(\S+?)(?:\s+([0-9A-F]{8,16}|\S+))?\s+f?\s+([i\s])?\s*(.*?\.(?:obj|lib|o|a))?',
        re.IGNORECASE
    )

    def __init__(self, map_path):
        self.map_path = Path(map_path)
        self.sections = {}        # (section_index, start_offset) -> {length, name, class}
        self.symbols = []         # Raw symbols without size
        self.symbols_with_size = []  # Symbols with calculated size

    def parse(self):
        """Parse map file and calculate symbol sizes."""
        with open(self.map_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        self._parse_sections(content)
        self._parse_publics(content)
        self._calculate_sizes()
        return self

    def _parse_sections(self, content):
        """Extract section table information."""
        lines = content.splitlines()
        in_section_table = False

        for line in lines:
            if line.startswith(" Start         Length     Name                   Class"):
                in_section_table = True
                continue
            if not in_section_table:
                continue
            if not line.strip() or line.startswith(" ---"):
                continue

            m = self.SECTION_LINE_RE.match(line)
            if m:
                section = m.group(1)
                start = m.group(2)
                length_str = m.group(3).rstrip('H')
                length = int(length_str, 16)
                name = m.group(4)
                key = (section, start)
                self.sections[key] = {
                    'length': length,
                    'name': name,
                    'class': m.group(5)
                }
            else:
                if line and not line[0].isspace():
                    in_section_table = False

    def _parse_publics(self, content):
        """Extract public symbols from 'Publics by Value' section."""
        marker = "Address         Publics by Value"
        if marker not in content:
            marker = "Publics by Value"
        if marker not in content:
            print("❌ ERROR: Cannot find symbol table in map file!")
            return

        start = content.find(marker)
        end = content.find("entry point at", start)
        if end == -1:
            end = len(content)

        symbol_text = content[start:end]
        lines = symbol_text.split('\n')[1:]

        for line in lines:
            line = line.strip()
            if not line or line.startswith('---'):
                continue

            m = self.SYMBOL_LINE_RE.match(line)
            if m:
                section = m.group(1)
                offset = m.group(2)
                name = m.group(3)
                rva = m.group(4) or ''
                obj = m.group(6) or 'unknown'
                obj_name = Path(obj).name if obj != 'unknown' else 'unknown'

                self.symbols.append({
                    'name': name,
                    'section': section,
                    'offset': offset,
                    'offset_int': int(offset, 16),
                    'rva': rva,
                    'obj': obj_name,
                    'obj_full': obj,
                    'line': line.strip()
                })

    def _calculate_sizes(self):
        """Calculate symbol sizes using section boundaries and symbol order."""
        # Group symbols by section
        sec_map = defaultdict(list)
        for sym in self.symbols:
            sec_map[sym['section']].append(sym)

        # Process each section
        for sec, symlist in sec_map.items():
            symlist.sort(key=lambda x: x['offset_int'])

            # Find section info
            sec_len = 0
            sec_start = 0
            for (s, start), info in self.sections.items():
                if s == sec:
                    sec_len = info['length']
                    sec_start = int(start, 16)
                    break

            if sec_len == 0:
                for sym in symlist:
                    sym['size'] = 0
                continue

            # Calculate size as distance to next symbol
            for i in range(len(symlist) - 1):
                size = symlist[i+1]['offset_int'] - symlist[i]['offset_int']
                symlist[i]['size'] = size
                symlist[i]['size_kb'] = size / 1024.0

            # Last symbol: use remaining section space
            if symlist:
                last = symlist[-1]
                last['size'] = max(0, sec_len - (last['offset_int'] - sec_start))
                last['size_kb'] = last['size'] / 1024.0

        # Flatten and filter
        for symlist in sec_map.values():
            self.symbols_with_size.extend(symlist)

        # Filter out zero-size and abnormally large (>1MB) symbols
        self.symbols_with_size = [s for s in self.symbols_with_size
                                  if 0 < s.get('size', 0) < 1024 * 1024]


# ============================================================================
# Symbol Classifier
# ============================================================================
class Classifier:
    """Classify symbols into categories based on name patterns."""

    @staticmethod
    def classify(symbol):
        """Return category name for the given symbol."""
        name = symbol['name']
        obj = symbol['obj']

        # Match against prioritized patterns
        for cat_name, patterns in zip(CATEGORY_NAMES, CATEGORY_PATTERNS):
            for p in patterns:
                if p.lower() in name.lower():
                    return cat_name

        # Special: module metadata (C++20 modules)
        if 'cppm.obj' in obj:
            return 'module_metadata'

        # Business logic vs library code
        if not any(lib in obj for lib in LIB_OBJ_KEYWORDS):
            return 'code_my'
        else:
            return 'code_lib'

        return 'other'


# ============================================================================
# Statistics Aggregator
# ============================================================================
class StatsAggregator:
    """Aggregate statistics by category and object file."""

    def __init__(self, symbols):
        self.symbols = symbols
        self.by_category = defaultdict(lambda: {'size': 0, 'count': 0})
        self.by_obj = defaultdict(lambda: {'size': 0, 'count': 0})
        self.by_obj_detailed = defaultdict(list)
        self._aggregate()

    def _aggregate(self):
        """Process all symbols and build statistics."""
        for sym in self.symbols:
            cat = Classifier.classify(sym)
            size = sym['size']
            obj = sym['obj']

            self.by_category[cat]['size'] += size
            self.by_category[cat]['count'] += 1

            self.by_obj[obj]['size'] += size
            self.by_obj[obj]['count'] += 1
            self.by_obj_detailed[obj].append(sym)

    def get_category_stats(self):
        """Return statistics per category in KB."""
        return {k: {'size_kb': v['size'] / 1024, 'count': v['count']}
                for k, v in self.by_category.items()}

    def get_top_obj(self, n=15):
        """Return top N object files by size."""
        sorted_obj = sorted(self.by_obj.items(),
                            key=lambda x: x[1]['size'],
                            reverse=True)
        return sorted_obj[:n]


# ============================================================================
# Extension Analyzers
# ============================================================================
class ExtensionAnalyzer:
    """Deep analysis extensions."""

    @staticmethod
    def template_hotspots(symbols, threshold=2):
        """
        Find frequently instantiated templates across object files.

        Templates with same simplified signature are considered duplicates
        that could be unified with explicit instantiation.
        """
        template_stats = defaultdict(lambda: {
            'count': 0,
            'objs': set(),
            'sizes': [],
            'example': ''
        })

        for sym in symbols:
            name = sym['name']
            if '?$' in name or '<' in name:
                # Normalize type parameters to detect duplicates
                simplified = re.sub(r'\?[A-Z0-9]+@', '?T@', name)
                simplified = re.sub(r'\$[0-9A-F]+', '', simplified)
                key = simplified[:120]

                template_stats[key]['count'] += 1
                template_stats[key]['objs'].add(sym['obj'])
                template_stats[key]['sizes'].append(sym['size'])
                if not template_stats[key]['example']:
                    template_stats[key]['example'] = name

        # Sort by repetition count
        hot = sorted(template_stats.items(),
                     key=lambda x: x[1]['count'],
                     reverse=True)

        print("\n🔥 TEMPLATE INSTANTIATION HOTSPOTS (repeat >= {})".format(threshold))
        print("-" * 80)
        total_saved = 0

        for i, (key, stat) in enumerate(hot[:20]):
            if stat['count'] >= threshold:
                avg_size = sum(stat['sizes']) / len(stat['sizes']) / 1024
                total_size = sum(stat['sizes']) / 1024
                potential_save = (stat['count'] - 1) * (sum(stat['sizes']) / len(stat['sizes']))
                total_saved += potential_save

                print(f"{i+1:2}. Repeated {stat['count']:3} times | "
                      f"Total {total_size:7.2f}KB | Avg {avg_size:5.2f}KB")

                obj_list = list(stat['objs'])[:5]
                print(f"      Objects: {', '.join(obj_list)}{'...' if len(stat['objs']) > 5 else ''}")

        print(f"\n    💡 Potential saving with explicit instantiation: {total_saved/1024:.2f}KB")

    @staticmethod
    def estimate_icf_savings(symbols):
        """
        Estimate potential size reduction with /OPT:ICF (COMDAT folding).

        Symbols with same section, size, and normalized name are considered
        identical and could be folded by the linker.
        """
        groups = defaultdict(list)

        for sym in symbols:
            if sym['size'] == 0:
                continue

            # Remove address offsets to detect identical functions
            simple_name = re.sub(r'[0-9A-F]{8,16}', '', sym['name'])
            simple_name = re.sub(r'\?\?_[0-9]', '', simple_name)
            key = (sym['section'], sym['size'], simple_name[:50])
            groups[key].append(sym)

        saved = 0
        collapsed = 0

        print("\n🗜️  COMDAT FOLDING ESTIMATE (/OPT:ICF)")
        print("-" * 80)

        for key, symlist in groups.items():
            if len(symlist) > 1:
                repeat = len(symlist) - 1
                size = symlist[0]['size']
                saved += repeat * size
                collapsed += repeat
                print(f"  Fold {repeat:2} instances, save {repeat * size / 1024:7.2f}KB  {symlist[0]['name'][:60]}")

        print(f"\n    💡 Estimated saving: {saved / 1024:.2f}KB ({collapsed} symbols)")

    @staticmethod
    def stl_per_obj(stats):
        """
        Attribute STL instantiation costs to specific object files.

        Shows which business logic modules are pulling in STL code,
        helping prioritize optimization efforts.
        """
        stl_cats = {'stl_string', 'stl_vector', 'stl_other', 'unordered_map', 'std_function'}
        per_obj = defaultdict(float)

        for obj, symlist in stats.by_obj_detailed.items():
            for sym in symlist:
                cat = Classifier.classify(sym)
                if cat in stl_cats:
                    per_obj[obj] += sym['size']

        sorted_obj = sorted(per_obj.items(), key=lambda x: x[1], reverse=True)

        print("\n📎 STL INSTANTIATION BY OBJECT FILE")
        print("-" * 80)

        for obj, sz in sorted_obj[:15]:
            print(f"  {sz/1024:7.2f}KB  {obj}")

        total_stl = sum(per_obj.values()) / 1024
        print(f"\n    💡 Total STL overhead from business logic: {total_stl:.2f}KB")
        return per_obj

    @staticmethod
    def export_html_report(parser, stats, args, output_path):
        """
        Generate standalone HTML report with interactive charts.

        Uses ECharts CDN, no additional dependencies required.
        Includes category pie chart, object file bar chart, and template hotspots.
        """
        cat_stats = stats.get_category_stats()
        total_kb = sum(s['size_kb'] for s in cat_stats.values())

        cat_data = [{'name': k, 'value': round(v['size_kb'], 2)}
                    for k, v in cat_stats.items()]
        obj_data = [{'name': o[:30], 'value': round(st['size']/1024, 2)}
                    for o, st in stats.get_top_obj(20)]

        # Get template hotspots for HTML report
        template_hotspots = []
        hotspots_raw = ExtensionAnalyzer._get_template_hotspots_raw(stats.symbols, top=10)
        template_hotspots = [{
            'name': h['name'][:60],
            'count': h['count'],
            'total_kb': round(h['total_kb'], 2)
        } for h in hotspots_raw]

        html_content = f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>MSVC Map Analysis - {parser.map_path.name}</title>
    <script src="https://cdn.jsdelivr.net/npm/echarts@5/dist/echarts.min.js"></script>
    <style>
        body {{ font-family: 'Segoe UI', Arial, sans-serif; margin: 20px; background: #f5f5f5; }}
        .container {{ max-width: 1200px; margin: auto; background: white; padding: 20px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }}
        h1 {{ color: #333; border-bottom: 2px solid #4CAF50; padding-bottom: 10px; }}
        h2 {{ color: #555; margin-top: 30px; }}
        .summary {{ background: #e8f5e9; padding: 15px; border-radius: 5px; margin: 20px 0; }}
        .chart-container {{ display: flex; flex-wrap: wrap; justify-content: space-around; }}
        .chart-box {{ width: 45%; min-width: 400px; height: 400px; margin-bottom: 30px; }}
        table {{ border-collapse: collapse; width: 100%; margin-top: 20px; }}
        th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        th {{ background-color: #4CAF50; color: white; }}
        tr:nth-child(even) {{ background-color: #f2f2f2; }}
        .footer {{ text-align: right; color: #666; margin-top: 30px; font-style: italic; }}
    </style>
</head>
<body>
<div class="container">
    <h1>📊 MSVC Map Analysis Report - {parser.map_path.name}</h1>
    
    <div class="summary">
        <strong>Total Code Size:</strong> {total_kb:.2f}KB &nbsp;|&nbsp;
        <strong>Symbol Count:</strong> {len(parser.symbols_with_size)} &nbsp;|&nbsp;
        <strong>Analysis Time:</strong> {__import__('datetime').datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
    </div>
    
    <div class="chart-container">
        <div id="pieChart" class="chart-box"></div>
        <div id="barChart" class="chart-box"></div>
    </div>
    
    <h2>📌 Template Hotspots (Top 10)</h2>
    <table>
        <thead>
            <tr>
                <th>Template Signature</th>
                <th>Repeat Count</th>
                <th>Total Size (KB)</th>
            </tr>
        </thead>
        <tbody>
            {''.join(f"<tr><td>{h['name']}</td><td>{h['count']}</td><td>{h['total_kb']}</td></tr>"
                     for h in template_hotspots)}
        </tbody>
    </table>
    
    <h2>📦 Top 20 Object Files</h2>
    <table>
        <thead>
            <tr>
                <th>Object File</th>
                <th>Size (KB)</th>
            </tr>
        </thead>
        <tbody>
            {''.join(f"<tr><td>{d['name']}</td><td>{d['value']}</td></tr>" for d in obj_data)}
        </tbody>
    </table>
    
    <div class="footer">
        Generated by MSVC Map Profiler - Ultimate Edition
    </div>
</div>

<script>
    // Pie chart
    var pieChart = echarts.init(document.getElementById('pieChart'));
    var pieOption = {{
        title: {{ text: 'Size by Category', left: 'center' }},
        tooltip: {{ trigger: 'item' }},
        series: [
            {{
                name: 'Size (KB)',
                type: 'pie',
                radius: '50%',
                data: {json.dumps(cat_data)},
                emphasis: {{
                    itemStyle: {{
                        shadowBlur: 10,
                        shadowOffsetX: 0,
                        shadowColor: 'rgba(0,0,0,0.5)'
                    }}
                }}
            }}
        ]
    }};
    pieChart.setOption(pieOption);
    
    // Bar chart
    var barChart = echarts.init(document.getElementById('barChart'));
    var barOption = {{
        title: {{ text: 'Top 20 Object Files', left: 'center' }},
        tooltip: {{ trigger: 'axis' }},
        xAxis: {{
            type: 'category',
            data: {json.dumps([d['name'] for d in obj_data])},
            axisLabel: {{ rotate: 45 }}
        }},
        yAxis: {{
            type: 'value',
            name: 'Size (KB)'
        }},
        series: [{{
            data: {json.dumps([d['value'] for d in obj_data])},
            type: 'bar',
            itemStyle: {{ color: '#4CAF50' }}
        }}]
    }};
    barChart.setOption(barOption);
</script>
</body>
</html>"""

        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(html_content)
        print(f"✅ HTML report generated: {output_path}")

    @staticmethod
    def _get_template_hotspots_raw(symbols, top=10):
        """Internal helper to get raw template hotspot data."""
        template_stats = defaultdict(lambda: {'count': 0, 'sizes': [], 'name': ''})

        for sym in symbols:
            name = sym['name']
            if '?$' in name or '<' in name:
                simplified = re.sub(r'\?[A-Z0-9]+@', '?T@', name)
                simplified = re.sub(r'\$[0-9A-F]+', '', simplified)
                key = simplified[:120]
                template_stats[key]['count'] += 1
                template_stats[key]['sizes'].append(sym['size'])
                if not template_stats[key]['name']:
                    template_stats[key]['name'] = name

        hot = sorted(template_stats.items(),
                     key=lambda x: x[1]['count'],
                     reverse=True)[:top]

        result = []
        for key, stat in hot:
            result.append({
                'name': stat['name'][:80],
                'count': stat['count'],
                'total_kb': sum(stat['sizes']) / 1024
            })
        return result


# ============================================================================
# Reporter
# ============================================================================
class Reporter:
    """Generate console, CSV, JSON, and chart reports."""

    def __init__(self, parser, stats, args):
        self.parser = parser
        self.stats = stats
        self.args = args

    def print_console(self):
        """Print colored console report with ASCII progress bars."""
        if HAS_COLORAMA and not self.args.no_color:
            red = Fore.RED
            green = Fore.GREEN
            yellow = Fore.YELLOW
            cyan = Fore.CYAN
            reset = Style.RESET_ALL
        else:
            red = green = yellow = cyan = reset = ''

        print(f"\n{cyan}🔍 ANALYSIS REPORT: {self.parser.map_path}{reset}")
        print("=" * 80)

        # Category statistics
        cat_stats = self.stats.get_category_stats()
        total_kb = sum(s['size_kb'] for s in cat_stats.values())
        sorted_cat = sorted(cat_stats.items(),
                            key=lambda x: x[1]['size_kb'],
                            reverse=True)

        print(f"\n{cyan}📊 CATEGORY SIZE RANKING{reset}")
        print("-" * 80)

        for cat, stat in sorted_cat:
            pct = (stat['size_kb'] / total_kb * 100) if total_kb else 0
            bar_len = int(stat['size_kb'] / total_kb * 50) if total_kb else 0
            bar = '█' * bar_len + '░' * (50 - bar_len)
            size_str = f"{stat['size_kb']:>8.2f}KB"
            print(f"{bar} {size_str} {pct:5.1f}%  {cat:20} ({stat['count']} symbols)")

        # High-risk categories
        print(f"\n{red}🔥 HIGH-RISK OPTIMIZATION TARGETS{reset}")
        print("-" * 80)
        targets = ['std_function', 'unordered_map', 'stl_string', 'stl_vector', 'stl_other']
        for t in targets:
            if t in cat_stats:
                kb = cat_stats[t]['size_kb']
                if kb > 5:
                    print(f"  {t:20}: {kb:>8.2f}KB  ✅ Immediate action recommended")

        # Business logic
        my_code = cat_stats.get('code_my', {}).get('size_kb', 0)
        print(f"\n{green}💻 BUSINESS LOGIC{reset}")
        print("-" * 80)
        print(f"  Total business code: {my_code:.2f}KB  ({my_code/total_kb*100:.1f}%)")

        # Top object files
        print(f"\n{yellow}📦 TOP 15 OBJECT FILES{reset}")
        print("-" * 80)
        top_obj = self.stats.get_top_obj(15)
        for i, (obj, st) in enumerate(top_obj, 1):
            size_kb = st['size'] / 1024
            print(f"{i:2}. {size_kb:>8.2f}KB  {obj}")

        # Overall statistics
        print(f"\n{cyan}📈 OVERALL STATISTICS{reset}")
        print("-" * 80)
        print(f"  Symbols parsed    : {len(self.parser.symbols_with_size)}")
        print(f"  Total code size   : {total_kb:.2f}KB")
        print(f"  Average symbol size: {total_kb/len(self.parser.symbols_with_size):.2f}KB")

        # Extension analyses
        if not self.args.skip_template:
            ExtensionAnalyzer.template_hotspots(self.parser.symbols_with_size)
        if not self.args.skip_icf:
            ExtensionAnalyzer.estimate_icf_savings(self.parser.symbols_with_size)
        if not self.args.skip_stl_per_obj:
            ExtensionAnalyzer.stl_per_obj(self.stats)

        # Action items summary
        print(f"\n{green}💡 ACTION ITEMS{reset}")
        print("-" * 80)
        if 'std_function' in cat_stats and cat_stats['std_function']['size_kb'] > 10:
            print(f"  • Remove std::function: save {cat_stats['std_function']['size_kb']:.1f}KB → "
                  f"Use function pointers or static dispatch")
        if 'unordered_map' in cat_stats and cat_stats['unordered_map']['size_kb'] > 10:
            print(f"  • Remove unordered_map: save {cat_stats['unordered_map']['size_kb']:.1f}KB → "
                  f"Replace with std::map or linear search")
        if 'stl_string' in cat_stats and cat_stats['stl_string']['size_kb'] > 50:
            print(f"  • STL string overhead: {cat_stats['stl_string']['size_kb']:.1f}KB → "
                  f"Add explicit instantiation, use string_view")
        if not self.args.skip_icf:
            print(f"  • Enable /OPT:ICF linker option: zero-cost saving")
        print()

    def export_csv(self, output_path):
        """Export all symbols to CSV file."""
        with open(output_path, 'w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerow(['Name', 'Size(KB)', 'Category', 'Object', 'Section', 'Offset', 'RVA'])

            for sym in self.parser.symbols_with_size:
                cat = Classifier.classify(sym)
                writer.writerow([
                    sym['name'],
                    round(sym['size_kb'], 3),
                    cat,
                    sym['obj'],
                    sym['section'],
                    sym['offset'],
                    sym['rva']
                ])

        print(f"✅ CSV report saved: {output_path}")

    def export_json(self, output_path):
        """Export JSON summary with optional full symbol list."""
        cat_stats = self.stats.get_category_stats()
        total_kb = sum(s['size_kb'] for s in cat_stats.values())

        summary = {
            'map_file': str(self.parser.map_path),
            'total_symbols': len(self.parser.symbols_with_size),
            'total_size_kb': round(total_kb, 2),
            'category': {},
            'top_obj': []
        }

        for cat, stat in cat_stats.items():
            summary['category'][cat] = {
                'size_kb': round(stat['size_kb'], 2),
                'count': stat['count'],
                'percentage': round(stat['size_kb'] / total_kb * 100, 1) if total_kb else 0
            }

        for obj, st in self.stats.get_top_obj(20):
            summary['top_obj'].append({
                'object': obj,
                'size_kb': round(st['size'] / 1024, 2),
                'count': st['count']
            })

        if self.args.full_json:
            summary['symbols'] = []
            for sym in self.parser.symbols_with_size:
                summary['symbols'].append({
                    'name': sym['name'],
                    'size_kb': round(sym['size_kb'], 3),
                    'category': Classifier.classify(sym),
                    'object': sym['obj'],
                    'section': sym['section'],
                    'offset': sym['offset'],
                    'rva': sym['rva']
                })

        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(summary, f, indent=2, ensure_ascii=False)

        print(f"✅ JSON report saved: {output_path}")

    def generate_charts(self, output_prefix):
        """Generate matplotlib charts (pie chart and bar chart)."""
        if not HAS_MATPLOTLIB:
            print("⚠️ matplotlib not installed, skipping charts")
            return

        cat_stats = self.stats.get_category_stats()
        total_kb = sum(s['size_kb'] for s in cat_stats.values())

        # Prepare data for pie chart
        labels, sizes = [], {}
        for cat, stat in cat_stats.items():
            pct = stat['size_kb'] / total_kb * 100
            if pct >= 1.0:
                labels.append(f"{cat}\n({stat['size_kb']:.1f}KB)")
                sizes.append(stat['size_kb'])
            else:
                if 'Others' not in labels:
                    labels.append('Others')
                    sizes.append(stat['size_kb'])
                else:
                    idx = labels.index('Others')
                    sizes[idx] += stat['size_kb']

        # Pie chart
        plt.figure(figsize=(10, 6))
        plt.pie(sizes, labels=labels, autopct='%1.1f%%', startangle=90)
        plt.title(f"Code Size by Category - {self.parser.map_path.name}")
        plt.axis('equal')
        pie_path = f"{output_prefix}_pie.png"
        plt.savefig(pie_path, dpi=120)
        plt.close()
        print(f"✅ Pie chart saved: {pie_path}")

        # Bar chart - Top 15 object files
        top_obj = self.stats.get_top_obj(15)
        obj_names = [os.path.basename(o[0])[:30] for o in top_obj]
        obj_sizes = [o[1]['size'] / 1024 for o in top_obj]

        plt.figure(figsize=(12, 6))
        plt.barh(range(len(obj_names)), obj_sizes, color='steelblue')
        plt.yticks(range(len(obj_names)), obj_names)
        plt.xlabel('Size (KB)')
        plt.title(f"Top 15 Object Files - {self.parser.map_path.name}")
        plt.gca().invert_yaxis()

        bar_path = f"{output_prefix}_top_obj.png"
        plt.tight_layout()
        plt.savefig(bar_path, dpi=120)
        plt.close()
        print(f"✅ Bar chart saved: {bar_path}")


# ============================================================================
# Multi-file comparison
# ============================================================================
def compare_maps(map_files, args):
    """Compare multiple map files side by side."""
    if len(map_files) < 2:
        return

    print("\n📊 MULTI-FILE COMPARISON")
    print("=" * 80)

    results = []
    for mf in map_files:
        parser = MapParser(mf).parse()
        stats = StatsAggregator(parser.symbols_with_size)
        cat_stats = stats.get_category_stats()
        total_kb = sum(s['size_kb'] for s in cat_stats.values())
        results.append({
            'file': Path(mf).name,
            'total_kb': total_kb,
            'cat': cat_stats
        })

    # Print header
    header = f"{'Category':30} " + " ".join([f"{r['file']:>12}" for r in results])
    print(header)
    print("-" * 80)

    # Print category rows
    all_cats = set()
    for r in results:
        all_cats.update(r['cat'].keys())

    for cat in sorted(all_cats):
        row = f"{cat:30} "
        for r in results:
            kb = r['cat'].get(cat, {}).get('size_kb', 0)
            row += f"{kb:>12.2f} "
        print(row)

    # Delta analysis
    print("\nDELTA ANALYSIS:")
    base = results[0]
    for other in results[1:]:
        diff = other['total_kb'] - base['total_kb']
        print(f"{other['file']} vs {base['file']}: {'+' if diff > 0 else ''}{diff:.2f}KB")

        for cat in all_cats:
            diff_cat = other['cat'].get(cat, {}).get('size_kb', 0) - \
                       base['cat'].get(cat, {}).get('size_kb', 0)
            if abs(diff_cat) > 10:
                print(f"    {cat}: {'+' if diff_cat > 0 else ''}{diff_cat:.2f}KB")


# ============================================================================
# Main entry point
# ============================================================================
def main():
    parser = argparse.ArgumentParser(
        description="MSVC Map File Profiler - Ultimate Edition",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic analysis with all extensions
  python map_profiler.py release/app.map
  
  # Generate HTML report
  python map_profiler.py app.map --html report.html
  
  # Export CSV and JSON
  python map_profiler.py app.map --csv symbols.csv --json summary.json
  
  # Compare two versions
  python map_profiler.py old.map new.map
  
  # Skip expensive extensions for quick analysis
  python map_profiler.py app.map --skip-template --skip-icf
        """
    )

    parser.add_argument('map_files', nargs='+', help='One or more .map files to analyze')
    parser.add_argument('--csv', help='Export all symbols to CSV file')
    parser.add_argument('--json', help='Export JSON summary')
    parser.add_argument('--full-json', action='store_true',
                        help='Include full symbol list in JSON export')
    parser.add_argument('--chart', help='Prefix for chart files (e.g., "output/chart")')
    parser.add_argument('--html', help='Generate interactive HTML report')
    parser.add_argument('--threshold', type=float, default=1.0,
                        help='Symbol size threshold in KB (default: 1.0)')
    parser.add_argument('--no-color', action='store_true',
                        help='Disable colored output')
    parser.add_argument('--skip-template', action='store_true',
                        help='Skip template hotspot analysis')
    parser.add_argument('--skip-icf', action='store_true',
                        help='Skip COMDAT folding estimation')
    parser.add_argument('--skip-stl-per-obj', action='store_true',
                        help='Skip STL attribution analysis')

    args = parser.parse_args()

    # Single file analysis
    if len(args.map_files) == 1:
        map_file = args.map_files[0]
        print(f"\n🔍 Analyzing: {map_file}")

        map_parser = MapParser(map_file).parse()
        stats = StatsAggregator(map_parser.symbols_with_size)
        reporter = Reporter(map_parser, stats, args)

        reporter.print_console()

        if args.csv:
            reporter.export_csv(args.csv)
        if args.json:
            reporter.export_json(args.json)
        if args.chart:
            reporter.generate_charts(args.chart)
        if args.html:
            ExtensionAnalyzer.export_html_report(map_parser, stats, args, args.html)

    # Multi-file comparison
    else:
        compare_maps(args.map_files, args)


if __name__ == '__main__':
    main()
