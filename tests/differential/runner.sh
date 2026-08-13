#!/usr/bin/env bash
set -u

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
CORPUS="$ROOT/corpus"
ONLY=""
REPORT="$ROOT/report.md"
WINUXCMD_BIN="${WINUXCMD_BIN:-}"
WINUXCMD_EXE="${WINUXCMD_EXE:-winuxcmd.exe}"
GNU_BIN="${GNU_BIN:-}"

while [ "$#" -gt 0 ]; do
  case "$1" in
    --corpus) CORPUS=$2; shift 2 ;;
    --report) REPORT=$2; shift 2 ;;
    --only) ONLY=$2; shift 2 ;;
    -h|--help) printf '%s\n' 'runner.sh [--corpus DIR] [--report FILE] [--only CMD]'; exit 0 ;;
    *) exit 2 ;;
  esac
done

find_oracle() {
  if [ -n "$GNU_BIN" ] && [ -x "$GNU_BIN/$1" ]; then printf '%s/%s' "$GNU_BIN" "$1"; else command -v "$1" 2>/dev/null || true; fi
}

run_case() {
  local case_file=$1 work line block cmd args timeout setup stdin
  work=$(mktemp -d) || return 2
  cmd=""; args=""; timeout=10; setup=""; stdin=""; block=""
  while IFS= read -r line || [ -n "$line" ]; do
    if [ -n "$block" ]; then
      if [[ "$line" == "  "* ]]; then
        if [ "$block" = setup ]; then setup+="${line#  }\n"; else stdin+="${line#  }\n"; fi
        continue
      fi
      block=""
    fi
    case "$line" in
      cmd:*) cmd=${line#cmd: };;
      args:*) args=${line#args: };;
      timeout:*) timeout=${line#timeout: };;
      setup:\ \|) block=setup;;
      stdin:\ \|) block=stdin;;
    esac
  done < "$case_file"
  if [ -z "$cmd" ]; then rm -rf "$work"; echo "SKIP missing cmd: $case_file"; return 0; fi
  [ -z "$setup" ] || (cd "$work" && printf '%b' "$setup" | sh)
  local wout="$work/w.out" gout="$work/g.out" wrc grc wcmd gcmd bucket
  wcmd="$WINUXCMD_EXE"; [ -n "$WINUXCMD_BIN" ] && wcmd="$WINUXCMD_BIN/$WINUXCMD_EXE"
  gcmd=$(find_oracle "$cmd")
  if [ ! -x "$gcmd" ]; then rm -rf "$work"; echo "SKIP no GNU oracle for $cmd"; return 0; fi
  printf '%b' "$stdin" | MSYS2_ARG_CONV_EXCL='*' MSYS_NO_PATHCONV=1 timeout "$timeout" "$wcmd" $args >"$wout" 2>/dev/null; wrc=$?
  printf '%b' "$stdin" | MSYS2_ARG_CONV_EXCL='*' MSYS_NO_PATHCONV=1 timeout "$timeout" "$gcmd" $args >"$gout" 2>/dev/null; grc=$?
  if [ "$wrc" -ne "$grc" ]; then bucket=EXIT_DIFF; elif cmp -s "$wout" "$gout"; then bucket=PASS; elif [ "$(tr -d '\r' < "$wout")" = "$(tr -d '\r' < "$gout")" ]; then bucket=CRLF_DIFF; else bucket=OUT_DIFF; fi
  printf '%s\t%s\n' "${case_file#$ROOT/}" "$bucket"
  rm -rf "$work"
}

oracle=$({ find_oracle coreutils --version || true; } | head -1 | tr -d '\r')
printf '# Differential report\n\nOracle: `%s`\n\n| Case | Result |\n|---|---|\n' "$oracle" > "$REPORT"
results=$(mktemp)
find "$CORPUS" -type f -name '*.case' | sort | while IFS= read -r file; do
  [ -n "$ONLY" ] && [ "$(basename "$(dirname "$file")")" != "$ONLY" ] && continue
  run_case "$file"
done | tee "$results" >> "$REPORT"
printf '\n## Summary\n\n' >> "$REPORT"
awk -F '\t' '{ total++; count[$2]++ } END { for (k in count) printf "- %s: %d/%d\n", k, count[k], total }' "$results" >> "$REPORT"
rm -f "$results"
