#!/usr/bin/env bash
set -u
set -o pipefail

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
CORPUS="$ROOT/corpus"
ONLY=""
REPORT="$ROOT/report.md"
WINUXCMD_BIN="${WINUXCMD_BIN:-}"
WINUXCMD_EXE="${WINUXCMD_EXE:-winuxcmd.exe}"
GNU_BIN="${GNU_BIN:-}"
BASELINE="$ROOT/baseline.json"
WHITELIST="$ROOT/whitelist.txt"

while [ "$#" -gt 0 ]; do
  case "$1" in
    --corpus) CORPUS=$2; shift 2 ;;
    --report) REPORT=$2; shift 2 ;;
    --only) ONLY=$2; shift 2 ;;
    --baseline) BASELINE=$2; shift 2 ;;
    -h|--help) printf '%s\n' 'runner.sh [--corpus DIR] [--report FILE] [--only CMD]'; exit 0 ;;
    *) echo "unknown option: $1" >&2; exit 2 ;;
  esac
done

find_oracle() {
  if [ -n "$GNU_BIN" ] && [ -x "$GNU_BIN/$1" ]; then printf '%s/%s' "$GNU_BIN" "$1"; else command -v "$1" 2>/dev/null || true; fi
}

absolute_command() {
  local value=$1
  if [[ "$value" =~ ^[A-Za-z]:[\\/] ]]; then
    value=$(cygpath -u "$value")
  fi
  if [[ "$value" == */* ]]; then
    if [[ "$value" != /* ]]; then value="$PWD/$value"; fi
    (CDPATH= cd -- "$(dirname "$value")" && printf '%s/%s' "$PWD" "$(basename "$value")")
  else
    command -v "$value" 2>/dev/null || true
  fi
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
  if [ -z "$cmd" ]; then
    rm -rf "$work"
    printf '%s\tSKIP\n' "${case_file#$ROOT/}"
    return 0
  fi
  [ -z "$setup" ] || (cd "$work" && printf '%b' "$setup" | sh)
  local wout="$work/w.out" werr="$work/w.err" gout="$work/g.out" gerr="$work/g.err"
  local wrc grc wcmd gcmd bucket case_args
  wcmd="$WINUXCMD_EXE"
  if [ -n "$WINUXCMD_BIN" ]; then
    if [ -x "$WINUXCMD_BIN/$cmd.exe" ]; then
      wcmd="$WINUXCMD_BIN/$cmd.exe"
    else
      wcmd="$WINUXCMD_BIN/$WINUXCMD_EXE"
    fi
  fi
  gcmd=$(find_oracle "$cmd")
  if [ ! -x "$gcmd" ]; then
    rm -rf "$work"
    printf '%s\tSKIP\n' "${case_file#$ROOT/}"
    return 0
  fi
  wcmd=$(absolute_command "$wcmd")
  gcmd=$(absolute_command "$gcmd")
  if [ -z "$wcmd" ] || [ -z "$gcmd" ]; then
    rm -rf "$work"
    printf '%s\tSKIP\n' "${case_file#$ROOT/}"
    return 0
  fi
  case_args=$args
  # Case arguments are intentionally shell-like and come from the checked-in
  # corpus. Keep setup and both command sides in the same isolated directory.
  (cd "$work" && printf '%b' "$stdin" | MSYS2_ARG_CONV_EXCL='*' MSYS_NO_PATHCONV=1 timeout "$timeout" "$wcmd" $case_args >"$wout" 2>"$werr"); wrc=$?
  (cd "$work" && printf '%b' "$stdin" | MSYS2_ARG_CONV_EXCL='*' MSYS_NO_PATHCONV=1 timeout "$timeout" "$gcmd" $case_args >"$gout" 2>"$gerr"); grc=$?
  if [ "$wrc" -ne "$grc" ]; then
    bucket=EXIT_DIFF
  elif cmp -s "$wout" "$gout" && cmp -s "$werr" "$gerr"; then
    bucket=PASS
  elif cmp -s <(tr -d '\r' < "$wout") <(tr -d '\r' < "$gout") &&
       cmp -s <(tr -d '\r' < "$werr") <(tr -d '\r' < "$gerr"); then
    bucket=CRLF_DIFF
  else
    bucket=OUT_DIFF
  fi
  printf '%s\t%s\n' "${case_file#$ROOT/}" "$bucket"
  rm -rf "$work"
}

oracle_cmd=$(find_oracle cat)
oracle=$({ "$oracle_cmd" --version 2>/dev/null || true; } | head -1 | tr -d '\r')
if [ -z "$oracle" ]; then
  echo "ERROR: GNU coreutils oracle not found" >&2
  exit 2
fi
case_count=$(find "$CORPUS" -type f -name '*.case' | wc -l)
if [ "$case_count" -eq 0 ]; then
  echo "ERROR: differential corpus is empty: $CORPUS" >&2
  exit 2
fi
printf '# Differential report\n\nOracle: `%s`\n\n| Case | Result |\n|---|---|\n' "$oracle" > "$REPORT"
results=$(mktemp)
find "$CORPUS" -type f -name '*.case' | sort | while IFS= read -r file; do
  [ -n "$ONLY" ] && [ "$(basename "$(dirname "$file")")" != "$ONLY" ] && continue
  run_case "$file"
done | tee "$results" >> "$REPORT"
printf '\n## Summary\n\n' >> "$REPORT"
awk -F '\t' '{ total++; count[$2]++ } END { for (k in count) printf "- %s: %d/%d\n", k, count[k], total }' "$results" >> "$REPORT"
printf '\n## By command\n\n| Command | PASS | Executed | Rate |\n|---|---:|---:|---:|\n' >> "$REPORT"
awk -F '\t' '
  {
    split($1, parts, "/"); command = parts[2];
    executed[command]++;
    if ($2 == "PASS") passed[command]++;
  }
  END {
    for (command in executed) {
      rate = executed[command] ? (100 * passed[command] / executed[command]) : 0;
      printf "| %s | %d | %d | %.1f%% |\n", command, passed[command] + 0,
             executed[command], rate;
    }
  }' "$results" | sort >> "$REPORT"
total=$(wc -l < "$results")
pass=$(awk -F '\t' '$2 == "PASS" { count++ } END { print count + 0 }' "$results")
skip=$(awk -F '\t' '$2 ~ /^SKIP/ { count++ } END { print count + 0 }' "$results")
diffs=$((total - pass - skip))
printf '\nCases executed: %d\nPASS: %d\nSKIP: %d\nDifferences: %d\n' "$total" "$pass" "$skip" "$diffs" >> "$REPORT"
if [ "$total" -eq 0 ]; then
  echo "ERROR: no cases selected (corpus=$CORPUS only=$ONLY)" >&2
  rm -f "$results"
  exit 2
fi
if [ "$diffs" -gt 0 ]; then
  echo "ERROR: $diffs differential case(s) did not match" >&2
  rm -f "$results"
  exit 1
fi
rm -f "$results"
