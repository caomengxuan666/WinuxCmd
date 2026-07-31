/*
 *  Copyright © 2026 WinuxCmd
 */

#include <ctime>

#include "core/command_macros.h"
import std;
import core;
import utils;
import container;

auto constexpr CAL_OPTIONS =
    std::array{OPTION("", "", "display calendar", STRING_TYPE)};

namespace {
constexpr int kMonthWidth = 20;
constexpr int kYearColumns = 3;
constexpr int kYearGutter = 3;
constexpr int kYearWidth =
    kYearColumns * kMonthWidth + (kYearColumns - 1) * kYearGutter;
constexpr std::array<std::string_view, 13> kMonthNames = {
    "",     "January", "February",  "March",   "April",    "May",     "June",
    "July", "August",  "September", "October", "November", "December"};

bool parseInt(std::string_view text, int& value) {
  if (text.empty()) return false;
  std::string owned(text);
  size_t consumed = 0;
  try {
    int parsed = std::stoi(owned, &consumed, 10);
    if (consumed != owned.size()) return false;
    value = parsed;
    return true;
  } catch (...) {
    return false;
  }
}

bool isLeapYear(int year) {
  return (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0);
}

int daysInMonth(int year, int month) {
  static constexpr std::array<int, 13> kDays = {0,  31, 28, 31, 30, 31, 30,
                                                31, 31, 30, 31, 30, 31};
  if (month == 2 && isLeapYear(year)) return 29;
  return kDays[static_cast<size_t>(month)];
}

int weekdaySundayZero(int year, int month, int day) {
  static constexpr std::array<int, 12> kOffsets = {0, 3, 2, 5, 0, 3,
                                                   5, 1, 4, 6, 2, 4};
  if (month < 3) --year;
  return (year + year / 4 - year / 100 + year / 400 +
          kOffsets[static_cast<size_t>(month - 1)] + day) %
         7;
}

std::string blanks(size_t count) { return std::string(count, char(32)); }

std::string center(std::string_view text, size_t width) {
  std::string owned(text.substr(0, width));
  if (owned.size() >= width) return owned;
  size_t padding = width - owned.size();
  size_t left = (padding + 1) / 2;
  size_t right = padding - left;
  return blanks(left) + owned + blanks(right);
}

std::string rightAlignedDay(int day, int width) {
  if (day <= 0) return blanks(static_cast<size_t>(width));
  std::string text = std::to_string(day);
  if (text.size() >= static_cast<size_t>(width)) return text;
  return blanks(static_cast<size_t>(width) - text.size()) + text;
}

std::array<std::string, 8> monthLines(int year, int month) {
  std::array<std::string, 8> lines{};
  lines[0] = center(
      std::format("{} {:04d}", kMonthNames[static_cast<size_t>(month)], year),
      kMonthWidth);
  lines[1] = "Su Mo Tu We Th Fr Sa";

  int start = weekdaySundayZero(year, month, 1);
  int last = daysInMonth(year, month);
  int day = 1;
  for (int week = 0; week < 6; ++week) {
    std::string row;
    row.reserve(kMonthWidth);
    for (int dow = 0; dow < 7; ++dow) {
      bool in_month = !(week == 0 && dow < start) && day <= last;
      row += rightAlignedDay(in_month ? day : 0, dow == 0 ? 2 : 3);
      if (in_month) ++day;
    }
    lines[static_cast<size_t>(week + 2)] = row;
  }
  return lines;
}

void printLine(std::string_view line) {
  safePrint(line);
  safePrint("\n");
}

void printMonth(int year, int month) {
  for (const auto& line : monthLines(year, month)) printLine(line);
}

void printYear(int year) {
  printLine(center(std::format("{:04d}", year), kYearWidth));
  safePrint("\n");
  for (int first_month = 1; first_month <= 12; first_month += kYearColumns) {
    std::array<std::array<std::string, 8>, kYearColumns> months = {
        monthLines(year, first_month), monthLines(year, first_month + 1),
        monthLines(year, first_month + 2)};
    for (size_t line = 0; line < months[0].size(); ++line) {
      for (size_t col = 0; col < months.size(); ++col) {
        safePrint(months[col][line]);
        if (col + 1 < months.size()) safePrint(blanks(kYearGutter));
      }
      safePrint("\n");
    }
  }
}
}  // namespace

REGISTER_COMMAND(cal,
                 /* name */
                 "cal",

                 /* synopsis */
                 "cal [OPTION] [[MONTH] YEAR]",

                 /* description */
                 "Display a calendar.\n"
                 "Display a calendar for the specified month or year. If no "
                 "arguments are given, display the current month.",

                 /* examples */
                 "cal\ncal 3 2024",

                 /* see_also */
                 "date(1)",

                 /* author */
                 "WinuxCmd",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 CAL_OPTIONS) {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  int year = timeinfo ? timeinfo->tm_year + 1900 : 1970;
  int month = timeinfo ? timeinfo->tm_mon + 1 : 1;
  bool whole_year = false;

  if (ctx.positionals.size() > 2) {
    safeErrorPrintLn("cal: too many arguments");
    safeErrorPrintLn("Try cal --help for more information.");
    return 1;
  }

  if (ctx.positionals.size() == 1) {
    if (!parseInt(ctx.positionals[0], year)) {
      safeErrorPrintLn(std::format("cal: invalid year {}", ctx.positionals[0]));
      return 1;
    }
    whole_year = true;
  } else if (ctx.positionals.size() == 2) {
    if (!parseInt(ctx.positionals[0], month)) {
      safeErrorPrintLn(
          std::format("cal: invalid month {}", ctx.positionals[0]));
      return 1;
    }
    if (!parseInt(ctx.positionals[1], year)) {
      safeErrorPrintLn(std::format("cal: invalid year {}", ctx.positionals[1]));
      return 1;
    }
  }

  if (year < 1 || year > 9999) {
    safeErrorPrintLn("cal: year must be in range 1..9999");
    return 1;
  }
  if (month < 1 || month > 12) {
    safeErrorPrintLn("cal: month must be in range 1..12");
    return 1;
  }

  if (whole_year) {
    printYear(year);
  } else {
    printMonth(year, month);
  }

  return 0;
}
