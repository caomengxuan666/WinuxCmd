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
 *  - File: readline.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
module;
#include "pch/pch.h"
export module readline;
import std;
import utils;

// ──────────────────────────────────────────────────────────────────────────────
// Public types
// ──────────────────────────────────────────────────────────────────────────────

/// A single completion item shown in the suggestion popup.
export struct CompletionItem {
  std::string text;  ///< Full text to insert (e.g. "kill -9")
  std::string hint;  ///< Short description shown to the right (e.g. "强制杀死进程")
};

/// Callback that returns completions for the current input buffer.
export using Completer =
    std::function<std::vector<CompletionItem>(std::string_view)>;

// ──────────────────────────────────────────────────────────────────────────────
// Internal rendering helpers
// ──────────────────────────────────────────────────────────────────────────────
namespace {

constexpr int  MAX_COMPLETIONS  = 8;    // max rows shown in popup
constexpr int  CMD_COLUMN_WIDTH = 24;   // column at which hint text starts
// Win32 colour attributes
constexpr WORD ATTR_SUGGESTION  = FOREGROUND_GREEN | FOREGROUND_BLUE |
                                   FOREGROUND_INTENSITY;   // bright cyan
constexpr WORD ATTR_SELECTED    = BACKGROUND_BLUE  |
                                   FOREGROUND_RED   | FOREGROUND_GREEN |
                                   FOREGROUND_BLUE  | FOREGROUND_INTENSITY;

struct RenderState {
  COORD input_start{0, 0};  // cursor position right after the prompt
  int   prev_lines = 0;     // how many completion rows were drawn last time
};

void scrollUp(HANDLE hOut, SHORT width, SHORT height, SHORT lines,
              WORD fillAttr) noexcept {
  if (lines <= 0 || width <= 0 || height <= 0) return;
  if (lines >= height) lines = static_cast<SHORT>(height - 1);
  if (lines <= 0) return;

  SMALL_RECT src{
      0,
      lines,
      static_cast<SHORT>(width - 1),
      static_cast<SHORT>(height - 1),
  };
  COORD     dst{0, 0};
  CHAR_INFO fill{};
  fill.Char.UnicodeChar = L' ';
  fill.Attributes = fillAttr;
  ScrollConsoleScreenBufferW(hOut, &src, nullptr, dst, &fill);
}

// Move cursor safely (clamp to buffer extent).
void moveTo(HANDLE hOut, SHORT x, SHORT y) noexcept {
  SetConsoleCursorPosition(hOut, {x, y});
}

// Erase `count` characters starting at (x, y) using the default attribute.
void eraseChars(HANDLE hOut, COORD pos, DWORD count, WORD attr) noexcept {
  DWORD written = 0;
  FillConsoleOutputCharacterW(hOut, L' ', count, pos, &written);
  FillConsoleOutputAttribute(hOut, attr, count, pos, &written);
}

// Write text at a specific screen position (no cursor advance side-effect
// because WriteConsoleOutputCharacterW does not move the cursor).
void writeAt(HANDLE hOut, COORD pos, const std::wstring& text,
             WORD attr) noexcept {
  if (text.empty()) return;
  DWORD written = 0;
  FillConsoleOutputAttribute(hOut, attr, (DWORD)text.size(), pos, &written);
  WriteConsoleOutputCharacterW(hOut, text.c_str(), (DWORD)text.size(), pos,
                               &written);
}

// Redraw the input line and the completion popup.
void redraw(HANDLE hOut, RenderState& state, const std::string& buffer,
            const std::vector<CompletionItem>& completions,
            int selected) noexcept {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hOut, &csbi);
  const SHORT W = csbi.dwSize.X;
  const SHORT H = csbi.dwSize.Y;
  const WORD  defaultAttr = csbi.wAttributes;

  // ── 1. Clear previous input text (from input_start to end-of-line) ──
  {
    DWORD clear = (DWORD)(W - state.input_start.X);
    if (clear > 0)
      eraseChars(hOut, state.input_start, clear, defaultAttr);
  }

  // ── 2. Clear previous completion rows ──
  for (int i = 1; i <= state.prev_lines; ++i) {
    SHORT rowY = state.input_start.Y + (SHORT)i;
    if (rowY >= H) break;
    eraseChars(hOut, {0, rowY}, (DWORD)W, defaultAttr);
  }

  // Always keep popup below prompt. If there is not enough space,
  // scroll buffer content up to reserve rows under the prompt.
  int desiredRows = std::min((int)completions.size(), MAX_COMPLETIONS);
  if (desiredRows > 0) {
    int availableBelow = H - state.input_start.Y - 1;
    if (availableBelow < desiredRows) {
      SHORT need = static_cast<SHORT>(desiredRows - availableBelow);
      scrollUp(hOut, W, H, need, defaultAttr);
      state.input_start.Y = static_cast<SHORT>(
          std::max(0, (int)state.input_start.Y - (int)need));
    }
  }

  // ── 3. Write buffer text at input_start ──
  moveTo(hOut, state.input_start.X, state.input_start.Y);
  {
    std::wstring wbuf = utf8_to_wstring(buffer);
    if (!wbuf.empty()) {
      DWORD written = 0;
      WriteConsoleW(hOut, wbuf.c_str(), (DWORD)wbuf.size(), &written, nullptr);
    }
  }

  // ── 4. Save the cursor position (this is the user's editing caret) ──
  COORD caretPos;
  {
    CONSOLE_SCREEN_BUFFER_INFO tmp;
    GetConsoleScreenBufferInfo(hOut, &tmp);
    caretPos = tmp.dwCursorPosition;
  }

  // ── 5. Draw completions ──
  int available = H - state.input_start.Y - 1;
  int count = std::min({(int)completions.size(), MAX_COMPLETIONS, available});

  for (int i = 0; i < count; ++i) {
    SHORT rowY = state.input_start.Y + 1 + (SHORT)i;
    if (rowY >= H) break;

    // Build display string: "  <text><padding><hint>"
    std::wstring line = L"  " + utf8_to_wstring(completions[i].text);
    while ((int)line.size() < CMD_COLUMN_WIDTH)
      line += L' ';
    if (!completions[i].hint.empty())
      line += utf8_to_wstring(completions[i].hint);

    // Truncate to console width
    if ((int)line.size() >= W - 1)
      line.resize((size_t)(W - 1));

    WORD attr = (i == selected) ? ATTR_SELECTED : ATTR_SUGGESTION;
    writeAt(hOut, {0, rowY}, line, attr);
  }

  state.prev_lines = count;

  // ── 6. Restore caret ──
  moveTo(hOut, caretPos.X, caretPos.Y);
}

}  // namespace

// ──────────────────────────────────────────────────────────────────────────────
// Public API
// ──────────────────────────────────────────────────────────────────────────────

/// Read one line interactively, showing a completion popup as the user types.
///
/// @param prompt     Wide-string prompt shown before the cursor (e.g. L"winux> ")
/// @param completer  Callback `vector<CompletionItem>(string_view input)` that
///                   returns relevant suggestions for the current buffer.
/// @returns The accepted UTF-8 string, or "" on Ctrl+C / EOF.
export std::string readInteractiveLine(const std::wstring& prompt,
                                       const Completer&    completer) {
  HANDLE hIn  = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  // ── Verify we are attached to a real console ──
  {
    DWORD mode = 0;
    if (!GetConsoleMode(hIn, &mode) || !GetConsoleMode(hOut, &mode)) {
      // Fallback: plain getline
      std::string line;
      std::getline(std::cin, line);
      return line;
    }
  }

  // ── Save original console modes ──
  DWORD inModeOrig = 0, outModeOrig = 0;
  GetConsoleMode(hIn,  &inModeOrig);
  GetConsoleMode(hOut, &outModeOrig);

  // Raw-ish key input: keep most existing flags and only disable line/echo
  // buffering so we can process keys while preserving host behavior.
  DWORD inModeInteractive = inModeOrig;
  inModeInteractive &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  inModeInteractive |= (ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
                        ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);
  SetConsoleMode(hIn, inModeInteractive);
  // Enable virtual-terminal processing so ANSI codes work in the host.
  SetConsoleMode(hOut, outModeOrig | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

  // ── Print prompt ──
  {
    DWORD written = 0;
    WriteConsoleW(hOut, prompt.c_str(), (DWORD)prompt.size(), &written,
                  nullptr);
  }

  // ── Record where the editable area starts ──
  RenderState state;
  {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    state.input_start = csbi.dwCursorPosition;
  }

  std::string                buffer;
  int                        selected = -1;
  std::vector<CompletionItem> completions;

  // ── Main input loop ──
  while (true) {
    // Compute completions for the current buffer.
    if (completer)
      completions = completer(buffer);
    else
      completions.clear();

    redraw(hOut, state, buffer, completions, selected);

    // Read one raw input event.
    INPUT_RECORD ir;
    DWORD nread = 0;
    if (!ReadConsoleInputW(hIn, &ir, 1, &nread) || nread == 0)
      continue;

    // Ignore everything except key-down events.
    if (ir.EventType != KEY_EVENT || !ir.Event.KeyEvent.bKeyDown)
      continue;

    const auto& ke   = ir.Event.KeyEvent;
    WORD        vk   = ke.wVirtualKeyCode;
    wchar_t     ch   = ke.uChar.UnicodeChar;
    DWORD       ctrl = ke.dwControlKeyState;

    // ── Enter: confirm ──
    if (vk == VK_RETURN) {
      if (selected >= 0 && selected < (int)completions.size())
        buffer = completions[selected].text;
      // Clear popup, advance to next line.
      redraw(hOut, state, buffer, {}, -1);
      DWORD written = 0;
      WriteConsoleW(hOut, L"\r\n", 2, &written, nullptr);
      break;
    }

    // ── Escape: dismiss popup ──
    if (vk == VK_ESCAPE) {
      selected = -1;
      continue;
    }

    // ── Arrow up: move selection up ──
    if (vk == VK_UP) {
      if (!completions.empty()) {
        if (selected <= 0)
          selected = (int)completions.size() - 1;
        else
          --selected;
      }
      continue;
    }

    // ── Arrow down: move selection down ──
    if (vk == VK_DOWN) {
      if (!completions.empty()) {
        if (selected < 0 || selected >= (int)completions.size() - 1)
          selected = 0;
        else
          ++selected;
      }
      continue;
    }

    // ── Tab: accept first / highlighted completion ──
    if (vk == VK_TAB) {
      if (!completions.empty()) {
        int idx = (selected >= 0) ? selected : 0;
        buffer  = completions[idx].text + " ";
        selected = -1;
      }
      continue;
    }

    // ── Backspace ──
    if (vk == VK_BACK) {
      if (!buffer.empty()) {
        // Strip the last UTF-8 code-point (handle multi-byte sequences).
        while (!buffer.empty() && (buffer.back() & 0xC0) == 0x80)
          buffer.pop_back();
        if (!buffer.empty()) buffer.pop_back();
        selected = -1;
      }
      continue;
    }

    // ── Ctrl+C: abort ──
    if (vk == 'C' &&
        (ctrl & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))) {
      redraw(hOut, state, "", {}, -1);
      DWORD written = 0;
      WriteConsoleW(hOut, L"^C\r\n", 4, &written, nullptr);
      SetConsoleMode(hIn, inModeOrig);
      SetConsoleMode(hOut, outModeOrig);
      return "";
    }

    // ── Ctrl+D: EOF / exit ──
    if (vk == 'D' &&
        (ctrl & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))) {
      if (buffer.empty()) {
        redraw(hOut, state, "", {}, -1);
        DWORD written = 0;
        WriteConsoleW(hOut, L"\r\n", 2, &written, nullptr);
        SetConsoleMode(hIn, inModeOrig);
        SetConsoleMode(hOut, outModeOrig);
        return "\x04";  // sentinel for EOF
      }
      continue;
    }

    // ── Printable Unicode character ──
    if (ch >= 0x20 && ch != 0x7F) {
      // Convert wchar_t → UTF-8 and append.
      char mb[4] = {};
      int  len =
          WideCharToMultiByte(CP_UTF8, 0, &ch, 1, mb, 4, nullptr, nullptr);
      for (int i = 0; i < len; ++i) buffer += mb[i];
      selected = -1;
      continue;
    }
  }

  // ── Restore console modes ──
  SetConsoleMode(hIn,  inModeOrig);
  SetConsoleMode(hOut, outModeOrig);
  return buffer;
}
