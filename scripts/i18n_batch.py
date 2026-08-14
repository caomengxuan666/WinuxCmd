#!/usr/bin/env python3
"""Extract and translate WinuxCmd catalogs through an OpenAI-compatible API."""

from __future__ import annotations

import argparse
import ast
import json
import os
import re
import hashlib
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SUFFIXES = {".cpp", ".cppm", ".h", ".hpp"}
# The configured OpenAI-compatible gateway exposes this model under the LUNA
# name.  An environment override is useful for testing another gateway model.
DEFAULT_MODEL = os.environ.get("OPENAI_I18N_MODEL", "gpt-5.6-luna")

# Hand-maintained help blocks that bypass REGISTER_COMMAND's generic formatter.
# Keep these stable IDs and review their translations manually.
MANUAL_MESSAGES = {
    "common.usage": "Usage:",
    "common.options": "OPTIONS:",
    "common.exit_status": "EXIT STATUS:",
    "common.exit.ok": "if OK,",
    "common.exit.minor": "if minor problems,",
    "common.exit.serious": "if serious trouble.",
    "common.about": "is a Windows implementation of GNU CoreUtils for Linux-Windows developers and AI coding assistants.",
    "main.subtitle": "Windows Compatible Linux Command Set",
    "main.available_commands": "Available Commands:",
    "main.help_tip": "Tip: Use 'winuxcmd <command> --help' for command-specific help.",
    "main.error.no_help_topic": "winuxcmd: no help topic for '{}'",
    "main.error.help_too_many_topics": "winuxcmd: help accepts at most one command name",
    "core.error.command_not_found": "winuxcmd: command not found: {}",
    "core.error.invalid_option_context": "{}: option used in invalid context -- {}",
    "utils.file.error.not_directory": "cannot open '{}' for reading: Not a directory",
    "utils.file.error.is_directory": "cannot open '{}' for reading: Is a directory",
    "utils.file.error.open": "cannot open '{}' for reading: {}",
    "utils.file.error.read": "error reading '{}'",
    "utils.file.error.read_stdin": "error reading from standard input",
    "utils.pager.end": "(END) ",
    "utils.pager.lines": "lines ",
    "utils.pager.controls": "  SPACE/f:next  j/k:line  b:back  /:search  n/N  g/G  q",
    "common.error.missing_operand": "missing operand",
    "common.error.missing_after": "missing operand after '{}'",
    "common.error.extra_operand": "extra operand '{}'",
    "common.error.invalid_argument": "invalid argument '{}'",
    "common.error.reading": "error reading '{}'",
    "common.try_help": "Try '{}' --help for more information.",
    "command.top.custom_help": (
        "Usage: top [options]\n"
        "  -b, --batch        Batch mode\n"
        "  -d, --delay DELAY  Update interval (default: 3s)\n"
        "  -n, --iterations N Exit after N iterations\n"
        "  -o, --field-sort F Sort by CPU|MEM|TIME|PID|NAME\n"
        "      --rows N       Limit number of displayed processes\n"
        "      --help         Show help\n"
        "  -v, --version      Show version\n"
    ),
    "command.mpicalc.custom_help": (
        "+   add           [0] := [1] + [0]          {-1}\n"
        "-   subtract      [0] := [1] - [0]          {-1}\n"
        "*   multiply      [0] := [1] * [0]          {-1}\n"
        "/   divide        [0] := [1] / [0]          {-1}\n"
        "%   modulo        [0] := [1] % [0]          {-1}\n"
        "<   left shift    [0] := [0] << 1           {0}\n"
        ">   right shift   [0] := [0] >> 1           {0}\n"
        "++  increment     [0] := [0]++              {0}\n"
        "--  decrement     [0] := [0]--              {0}\n"
        "m   multiply mod  [0] := [2] * [1] mod [0]  {-2}\n"
        "^   power mod     [0] := [2] ^ [1] mod [0]  {-2}\n"
        "G   gcd           [0] := gcd([1],[0])       {-1}\n"
        "i   remove item   [0] := [1]                {-1}\n"
        "d   dup item      [-1] := [0]               {+1}\n"
        "r   reverse       [0] := [1], [1] := [0]    {0}\n"
        "b   # of bits     [0] := nbits([0])         {0}\n"
        "P   prime check   [0] := is_prime([0])?1:0  {0}\n"
        "c   clear stack\n"
        "p   print top item\n"
        "f   print the stack\n"
        "#   ignore until end of line\n"
        "?   print this help\n"
    ),
    "command.tzset.custom_help": (
        "Usage: tzset [OPTION]\n\n"
        "Print POSIX-compatible timezone ID from current Windows timezone setting\n\n"
        "Options:\n"
        "      --help               output usage information and exit.\n"
        "  -V, --version            output version information and exit.\n\n"
        "Use tzset to set your TZ variable. In POSIX-compatible shells like bash,\n"
        "dash, mksh, or zsh:\n\n"
        "      export TZ=$(tzset)\n\n"
        "In csh-compatible shells like tcsh:\n\n"
        "      setenv TZ `tzset`\n"
    ),
    "command.wpm.custom_help": (
        "Usage: wpm <command> [args] [options]\n\n"
        "Commands:\n"
        "  links list|rebuild|remove     manage WinuxCmd hardlinks\n"
        "  clean [cache|staging|all]    remove transient downloads and staging\n"
        "  index status|update           inspect or refresh local index\n"
        "  source list|use|add           manage index sources\n"
        "  list                          list indexed packages and install state\n"
        "  search <query>                search names, commands, categories, licenses\n"
        "  info <package>                show package metadata\n"
        "  install <package>...          install one or more packages\n"
        "  installed                     list packages present in this root\n"
        "  update winuxcmd               update WinuxCmd from local index\n\n"
        "Options:\n"
        "  -r, --root <dir>              manage a specific WinuxCmd root\n"
        "  -s, --source <name>           use a specific index source\n"
        "  -a, --all                     show index-only packages in list output\n"
        "  -f, --force                   overwrite existing files when safe\n"
        "  -n, --dry-run                 show planned changes without writing\n"
        "  -v, --verbose                 print detailed progress\n"
        "      --help                    display this help and exit\n"
        "  -V, --version                 output version information and exit\n"
    ),
    "command.chown.error.unsupported_ownership": "chown: changing ownership is not supported on Windows",
    "command.wpm.error.invalid_json": "wpm: invalid JSON: {}",
    "command.wpm.error.clean": "wpm: failed to clean '{}': {}",
    "command.wpm.error.materialize": "wpm: failed to install '{}' to '{}': {}",
    "command.wpm.error.stage": "wpm: failed to stage exe: {}",
    "command.wpm.status.clean_empty": "wpm: {} is already clean",
    "command.wpm.status.clean_dry_run": "wpm: would remove {} ({} in {} files)",
    "command.wpm.status.cleaned_detail": "wpm: cleaned {} ({} in {} files)",
    "command.wpm.status.install_summary": "wpm: install summary: requested={} failed={}",
    "command.wpm.status.keep_running": "wpm: keeping running executable: {}",
    "command.wpm.error.refuse_directory": "wpm: refusing to replace directory: {}",
    "command.wpm.error.remove": "wpm: failed to remove '{}': {}",
    "command.wpm.status.remove_legacy": "remove legacy link {}",
    "command.wpm.status.removed_legacy": "removed legacy link {}",
    "command.wpm.error.remove_legacy": "wpm: failed to remove legacy link '{}': {}",
    "command.wpm.error.executable_missing": "wpm: winuxcmd.exe not found in root: {}",
    "command.wpm.status.link": "link {} -> {}",
    "command.wpm.status.linked": "linked {}",
    "command.wpm.error.create_link": "wpm: failed to create hard link '{}': {}",
    "command.wpm.status.links_summary": "wpm: links created={} unchanged={} stale_removed={} failed={}",
    "command.wpm.status.remove": "remove {}",
    "command.wpm.status.links_removed": "wpm: links removed={} failed={}",
    "command.wpm.error.usage.links": "wpm: usage: wpm links list|rebuild|remove",
    "command.wpm.error.usage.index": "wpm: usage: wpm index status|update",
    "command.wpm.error.usage.source": "wpm: usage: wpm source list|use <name>|add <name> <url>",
    "command.wpm.error.usage.cache": "wpm: usage: wpm cache clean [cache|staging|all]",
    "command.wpm.error.usage.clean": "wpm: usage: wpm clean [cache|staging|all]",
    "command.wpm.error.usage.info": "wpm: usage: wpm info <package>",
    "command.wpm.error.usage.install": "wpm: usage: wpm install <package>...",
    "command.wpm.error.usage.update": "wpm: usage: wpm update winuxcmd",
    "command.wpm.version": "wpm {}",
    "command.wpm.error.unknown_command": "wpm: unknown command: {}",
    "command.wpm.error.exception": "wpm: {}",
    "common.error.invalid_input": "invalid input",
    "common.error.read_file": "error reading from file",
    "common.error.read_input": "error reading input",
    "common.error.read_stdin": "error reading from standard input",
    "common.error.missing_file": "missing file operand",
    "common.error.invalid_block_size": "invalid block size",
    "common.error.invalid_length": "invalid length",
    "common.error.invalid_range": "invalid range",
    "common.error.invalid_input_range": "invalid input range",
    "common.error.invalid_wrap": "invalid wrap size",
    "common.error.invalid_line_count": "invalid line count",
    "common.error.invalid_regex": "invalid regular expression",
    "common.error.target_directory": "target is not a directory",
    "common.error.create_directory": "cannot create directory",
    "common.error.open_read": "cannot open for reading",
    "common.error.open_write": "cannot open for writing",
    "common.error.read_metadata": "cannot read source metadata",
    "common.error.write_metadata": "cannot write destination metadata",
    "common.error.preserve_timestamps": "cannot preserve timestamps",
    "common.error.preserve_attributes": "cannot preserve attributes",
    "common.error.create_backup": "cannot create backup for destination",
    "common.error.same_file": "source and destination are the same file",
    "common.error.hash_data": "failed to hash data",
    "common.error.crypto_context": "failed to acquire cryptographic context",
    "common.error.hash_object": "failed to create hash object",
    "common.error.hash_value": "failed to get hash value",
    "common.error.no_such_file": "No such file or directory",
    "common.error.cannot_open": "cannot open '{}'",
    "common.error.cannot_access": "cannot access '{}'",
    "common.error.cannot_stat": "cannot stat '{}'",
    "common.error.cannot_create": "cannot create '{}'",
    "common.error.write": "error writing '{}'",
    "common.error.invalid_mode": "invalid mode: '{}'",
    "common.error.invalid_group": "invalid group: '{}'",
    "common.error.invalid_user": "invalid user: '{}'",
    "common.error.invalid_spec": "invalid spec: '{}'",
    "common.error.invalid_encoding": "invalid encoding '{}'",
    "common.error.invalid_type": "invalid type '{}'",
    "common.error.invalid_device_type": "invalid device type '{}'",
    "common.error.invalid_suffix": "invalid suffix '{}'",
    "common.error.invalid_time_interval": "invalid time interval '{}'",
    "common.error.invalid_char_class": "invalid character class '{}'",
    "common.error.unknown_registry_root": "unknown registry root '{}'",
    "common.error.cannot_copy": "cannot copy '{}'",
    "common.error.cannot_delete_file": "cannot delete source file '{}'",
    "common.error.cannot_copy_directory": "cannot copy directory '{}'",
    "common.error.cannot_delete_directory": "cannot delete source directory '{}'",
    "common.error.create_hardlink": "failed to create hard link '{}'",
    "common.error.failed_access": "failed to access '{}'",
    "common.error.create_symlink": "failed to create symbolic link '{}'",
    "common.error.failed_remove": "failed to remove '{}'",
    "common.error.open_script": "cannot open script file '{}'",
    "command.dd.error.open_input": "dd: failed to open '{}': {}",
}


def json_load(path: Path) -> dict:
    with path.open(encoding="utf-8") as stream:
        value = json.load(stream)
    if not isinstance(value, dict) or not isinstance(value.get("messages"), dict):
        raise ValueError(f"{path}: expected an object with a messages object")
    return value


def string_value(text: str) -> str:
    parts = re.findall(r'"(?:\\.|[^"\\])*"', text, re.S)
    if not parts:
        return ""
    return "".join(ast.literal_eval(part) for part in parts)


def strip_comments(text: str) -> str:
    return re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S)


def split_args(text: str) -> list[str]:
    result, start, depth = [], 0, 0
    quote = False
    escape = False
    for index, char in enumerate(text):
        if quote:
            if escape:
                escape = False
            elif char == "\\":
                escape = True
            elif char == '"':
                quote = False
            continue
        if char == '"':
            quote = True
        elif char in "([{":
            depth += 1
        elif char in ")]}":
            depth -= 1
        elif char == "," and depth == 0:
            result.append(text[start:index].strip())
            start = index + 1
    result.append(text[start:].strip())
    return result


def calls(source: str, name: str) -> list[str]:
    result = []
    for match in re.finditer(rf"\b{re.escape(name)}\s*\(", source):
        start = match.end()
        depth, quote, escape = 1, False, False
        for index in range(start, len(source)):
            char = source[index]
            if quote:
                if escape:
                    escape = False
                elif char == "\\":
                    escape = True
                elif char == '"':
                    quote = False
            elif char == '"':
                quote = True
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
                if depth == 0:
                    result.append(source[start:index])
                    break
    return result


def extract(source_root: Path) -> dict:
    messages: dict[str, str] = {}
    for path in sorted(source_root.glob("*.cpp")):
        source = path.read_text(encoding="utf-8", errors="replace")
        option_arrays: dict[str, list[tuple[str, str, str]]] = {}
        for match in re.finditer(
            r"auto\s+constexpr\s+(\w+_OPTIONS)\s*=\s*std::array\s*\{(.*?)\};",
            source,
            re.S,
        ):
            options = []
            for body in calls(match.group(2), "OPTION"):
                args = split_args(body)
                if len(args) >= 3:
                    short, long = string_value(args[0]), string_value(args[1])
                    desc = string_value(args[2])
                    option = long.removeprefix("--") or short.removeprefix("-")
                    if option and desc:
                        options.append((option, desc, short + " " + long))
            option_arrays[match.group(1)] = options

        for body in calls(source, "REGISTER_COMMAND"):
            args = split_args(body)
            if len(args) < 4:
                continue
            command = args[0].strip()
            if not re.fullmatch(r"[A-Za-z0-9_]+", command):
                continue
            description = string_value(args[3])
            synopsis = string_value(args[2])
            messages[f"command.{command}.synopsis"] = synopsis
            messages[f"command.{command}.description"] = description
            array_name = strip_comments(args[-1]).strip().split()[-1]
            for option, text, _ in option_arrays.get(array_name, []):
                messages[f"command.{command}.option.{option}"] = text

    messages["common.option.help"] = "display this help and exit"
    messages["common.option.version"] = "output version information and exit"
    # Fixed line-oriented output is localized by utils:console at runtime.
    # Dynamic fragments and structured data remain ordinary output.
    for path in sorted((ROOT / "src").rglob("*")):
        if path.suffix not in SOURCE_SUFFIXES:
            continue
        source = path.read_text(encoding="utf-8", errors="replace")
        for match in re.finditer(
                r"safe(?:Error)?Print(?:Ln)?\(\s*(?:L)?\"((?:\\.|[^\"\\])*)\"\s*\)", source):
            value = string_value('"' + match.group(1) + '"')
            if (value.strip() and not re.fullmatch(
                    r"(?:\\[nrt0abfv]|[\\s\\t\\r\\n.,:;+/=<>|{}()\\[\\]-]+)", value)
                    and not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", value)):
                digest = 14695981039346656037
                for byte in value.encode("utf-8"):
                    digest ^= byte
                    digest = (digest * 1099511628211) & 0xFFFFFFFFFFFFFFFF
                digest = f"{digest:016x}"
                messages[f"legacy.{digest}"] = value
        for match in re.finditer(
                r"std::unexpected\(\s*(?:L)?\"((?:\\.|[^\"\\])*)\"\s*\)", source):
            value = string_value('"' + match.group(1) + '"')
            if value.strip() and any(ch.isalpha() for ch in value):
                digest = 14695981039346656037
                for byte in value.encode("utf-8"):
                    digest ^= byte
                    digest = (digest * 1099511628211) & 0xFFFFFFFFFFFFFFFF
                messages[f"legacy.{digest:016x}"] = value
    messages.update(MANUAL_MESSAGES)
    return {"schema": 1, "locale": "en-US", "messages": dict(sorted(messages.items()))}


def placeholders(text: str) -> list[str]:
    values = re.findall(r"%[-+#0-9.*]*[a-zA-Z]|\{[^{}]*\}|`[^`]+`|(?<![A-Za-z])(?:FILEs?|PATH|OPTION|NUM|ARGs?)(?![A-Za-z])", text)
    return sorted({"FILE" if value == "FILEs" else "ARG" if value == "ARGs" else value for value in values})


def validate(base: dict, translated: dict) -> list[str]:
    errors = []
    base_keys, translated_keys = set(base["messages"]), set(translated["messages"])
    if base_keys != translated_keys:
        errors.append(f"key mismatch: missing={sorted(base_keys-translated_keys)} extra={sorted(translated_keys-base_keys)}")
    for key in sorted(base_keys & translated_keys):
        if placeholders(str(base["messages"][key])) != placeholders(str(translated["messages"][key])):
            errors.append(f"placeholder mismatch: {key}")
        if key.startswith("command.") and ".option." in key:
            original = str(base["messages"][key])
            if any(token in str(translated["messages"][key]) for token in ("--", "-")) and re.search(r"(?:^|\s)-{1,2}[A-Za-z]", original):
                for token in re.findall(r"(?<!\w)-{1,2}[A-Za-z][\w-]*", original):
                    if token not in str(translated["messages"][key]):
                        errors.append(f"option token changed: {key}: {token}")
    return errors


def api_url(path: str) -> str:
    base = os.environ.get("OPENAI_BASE_URL") or os.environ.get("OPENAI_API_BASE")
    if not base:
        raise RuntimeError("OPENAI_BASE_URL or OPENAI_API_BASE is required")
    return base.rstrip("/") + "/" + path.lstrip("/")


def request(path: str, method: str = "GET", body: bytes | None = None, content_type: str = "application/json") -> dict:
    token = os.environ.get("OPENAI_API_KEY")
    if not token:
        raise RuntimeError("OPENAI_API_KEY is not set; load it from your Winuxsh profile")
    req = urllib.request.Request(api_url(path), data=body, method=method, headers={"Authorization": f"Bearer {token}", "Content-Type": content_type})
    try:
        with urllib.request.urlopen(req, timeout=120) as response:
            return json.load(response)
    except urllib.error.HTTPError as error:
        detail = error.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"HTTP {error.code} from {path}: {detail[:1000]}") from error


def translate_batch(model: str, locale: str, items: list[tuple[str, str]], retries: int) -> dict[str, str]:
    prompt = {"locale": locale, "messages": dict(items)}
    body = {"model": model, "temperature": 0, "response_format": {"type": "json_object"}, "messages": [
        {"role": "system", "content": "Translate every value to the requested locale. Return only one JSON object mapping the exact input keys to translated strings. Preserve command options, placeholders, backticks, technical tokens, and newlines exactly. Never add or remove keys."},
        {"role": "user", "content": json.dumps(prompt, ensure_ascii=False)},
    ]}
    for attempt in range(retries + 1):
        try:
            result = request("chat/completions", "POST", json.dumps(body, ensure_ascii=False).encode())
            content = result["choices"][0]["message"]["content"]
            translations = json.loads(content)
            if not isinstance(translations, dict) or set(translations) != set(dict(items)):
                raise ValueError("model returned a different key set")
            if not all(isinstance(value, str) for value in translations.values()):
                raise ValueError("model returned a non-string translation")
            for key, original in items:
                if placeholders(original) != placeholders(translations[key]):
                    raise ValueError(f"placeholder mismatch: {key}")
            return translations
        except (KeyError, TypeError, ValueError, RuntimeError, urllib.error.URLError) as error:
            if attempt == retries:
                raise RuntimeError(f"batch failed after {retries + 1} attempts: {error}") from error
            time.sleep(2 ** attempt)
    raise AssertionError("unreachable")


def cmd_translate(args: argparse.Namespace) -> None:
    if not args.model:
        raise RuntimeError("no model configured; pass --model or set OPENAI_I18N_MODEL")
    base = json_load(Path(args.input))
    output_path = Path(args.output)
    checkpoint = output_path.with_suffix(output_path.suffix + ".partial")
    translated = {}
    if args.seed:
        seed = json_load(Path(args.seed))
        translated.update({key: value for key, value in seed["messages"].items()
                           if key in base["messages"]})
    if checkpoint.exists():
        partial = json_load(checkpoint)
        translated = partial["messages"]
    pending = [(key, text) for key, text in sorted(base["messages"].items()) if key not in translated]
    batches = [pending[i:i + args.batch_size] for i in range(0, len(pending), args.batch_size)]
    print(f"translating {len(pending)} messages in {len(batches)} batches; concurrency={args.concurrency}")
    with ThreadPoolExecutor(max_workers=args.concurrency) as pool:
        futures = {pool.submit(translate_batch, args.model, args.locale, batch, args.retries): batch for batch in batches}
        for index, future in enumerate(as_completed(futures), 1):
            batch_result = future.result()
            translated.update(batch_result)
            checkpoint.write_text(json.dumps({"schema": 1, "locale": args.locale, "messages": dict(sorted(translated.items()))}, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            print(f"completed {index}/{len(batches)} batches ({len(translated)} messages)")
    result = {"schema": 1, "locale": args.locale, "messages": dict(sorted(translated.items()))}
    errors = validate(base, result)
    if errors:
        raise RuntimeError("validation failed:\n" + "\n".join(errors))
    output_path.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    checkpoint.unlink(missing_ok=True)
    print(f"wrote {len(translated)} translations to {output_path}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    extract_parser = sub.add_parser("extract")
    extract_parser.add_argument("--source", default=str(ROOT / "src" / "commands"))
    extract_parser.add_argument("--output", required=True)
    extract_parser.set_defaults(func=lambda a: Path(a.output).write_text(json.dumps(extract(Path(a.source)), ensure_ascii=False, indent=2) + "\n", encoding="utf-8"))
    translate = sub.add_parser("translate")
    translate.add_argument("--input", required=True)
    translate.add_argument("--output", required=True)
    translate.add_argument("--seed")
    translate.add_argument("--locale", required=True)
    translate.add_argument("--model", default=DEFAULT_MODEL)
    translate.add_argument("--batch-size", type=int, default=40)
    translate.add_argument("--concurrency", type=int, default=2)
    translate.add_argument("--retries", type=int, default=3)
    translate.set_defaults(func=cmd_translate)
    validate_parser = sub.add_parser("validate"); validate_parser.add_argument("--base", required=True); validate_parser.add_argument("--translated", required=True); validate_parser.set_defaults(func=lambda a: (print("valid") if not (errors := validate(json_load(Path(a.base)), json_load(Path(a.translated)))) else (_ for _ in ()).throw(RuntimeError("\n".join(errors)))))
    args = parser.parse_args()
    try:
        args.func(args)
        return 0
    except (OSError, ValueError, RuntimeError, urllib.error.URLError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
