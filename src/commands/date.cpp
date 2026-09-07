/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: date.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
/// @Description: Implementation for date.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "core/command_macros.h"
#include "pch/pch.h"

#pragma comment(lib, "advapi32.lib")
import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

/**
 * @brief DATE command options definition
 *
 * This array defines all the options supported by the date command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 * - @a -d, @a --date: Display time described by STRING, not 'now' [IMPLEMENTED]
 * - @a -u, @a --utc: Coordinated Universal Time (UTC) [IMPLEMENTED]
 * - @a -R, @a --rfc-2822: Output RFC 2822 compliant date string [IMPLEMENTED]
 * - @a -I, @a --iso-8601: Output ISO 8601 date/time [IMPLEMENTED]
 * - @a --rfc-3339: Output RFC 3339 date/time [IMPLEMENTED]
 * - @a -r, @a --reference: Display the last modification time of FILE
 * [IMPLEMENTED]
 * - @a +FORMAT: Output formatted date string [IMPLEMENTED]
 */
auto constexpr DATE_OPTIONS = std::array{
    // [DIFFERS]
    OPTION("-d", "--date", "display time described by STRING, not 'now'",
           STRING_TYPE),
    // [DIFFERS]
    OPTION("-u", "--utc", "Coordinated Universal Time (UTC)"),
    // [DIFFERS]
    OPTION("-R", "--rfc-email", "output RFC 5322 compliant date string"),
    // [DIFFERS]
    OPTION("", "--rfc-2822", "output RFC 2822 compliant date string"),
    // [DIFFERS]
    OPTION("-I", "--iso-8601", "output ISO 8601 date/time",
           OPTIONAL_STRING_TYPE),
    // [DIFFERS]
    OPTION("", "--rfc-3339", "output RFC 3339 date/time", STRING_TYPE),
    // [DIFFERS]
    OPTION("-r", "--reference", "display the last modification time of FILE",
           STRING_TYPE),
    // [DIFFERS]
    OPTION("-f", "--file", "display date strings from DATEFILE, one per line",
           STRING_TYPE),
    // [DIFFERS]
    OPTION("", "--universal", "alias for --utc"),
    // [GNU] --debug: annotate the parsed date, and warn about dubious usage
    OPTION("", "--debug",
           "annotate the parsed date, and warn about dubious usage to stderr"),
    // [GNU] --resolution: output the available resolution of timestamps
    OPTION("", "--resolution", "output the available resolution of timestamps"),
    // [GNU] --set: set time described by STRING
    OPTION("", "--set", "set time described by STRING", STRING_TYPE),
    // [DIFFERS] GNU: -s, --set=STRING; WinuxCmd: -s and --set are separate
    // entries
    OPTION("-s", "", "set time described by STRING", STRING_TYPE)};

namespace date_pipeline {
namespace cp = core::pipeline;

struct TimeValue {
  FILETIME utc{};
  SYSTEMTIME display{};
  bool utc_display = false;
};

auto trim_copy(std::string s) -> std::string {
  auto first = s.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) return {};
  auto last = s.find_last_not_of(" \t\r\n");
  return s.substr(first, last - first + 1);
}

auto lower_copy(std::string s) -> std::string {
  std::ranges::transform(s, s.begin(), [](unsigned char ch) {
    return static_cast<char>(std::tolower(ch));
  });
  return s;
}

auto is_digits(std::string_view s) -> bool {
  return !s.empty() && std::ranges::all_of(s, [](unsigned char ch) {
    return std::isdigit(ch) != 0;
  });
}

auto parse_int(std::string_view s) -> std::optional<int> {
  if (s.starts_with('+')) s.remove_prefix(1);
  int value = 0;
  auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
  if (ec != std::errc() || ptr != s.data() + s.size()) return std::nullopt;
  return value;
}

auto days_in_month(int year, int month) -> int {
  static constexpr int days[] = {31, 28, 31, 30, 31, 30,
                                 31, 31, 30, 31, 30, 31};
  if (month != 2) return days[month - 1];
  bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
  return leap ? 29 : 28;
}

auto valid_system_time(const SYSTEMTIME &st) -> bool {
  if (st.wYear < 1601 || st.wMonth < 1 || st.wMonth > 12) return false;
  if (st.wDay < 1 || st.wDay > days_in_month(st.wYear, st.wMonth)) {
    return false;
  }
  return st.wHour <= 23 && st.wMinute <= 59 && st.wSecond <= 59;
}

auto filetime_to_ticks(const FILETIME &ft) -> unsigned long long {
  ULARGE_INTEGER uli{};
  uli.LowPart = ft.dwLowDateTime;
  uli.HighPart = ft.dwHighDateTime;
  return uli.QuadPart;
}

auto ticks_to_filetime(unsigned long long ticks) -> FILETIME {
  ULARGE_INTEGER uli{};
  uli.QuadPart = ticks;
  return FILETIME{uli.LowPart, uli.HighPart};
}

auto add_seconds(FILETIME ft, long long seconds) -> FILETIME {
  constexpr long long ticks_per_second = 10000000LL;
  auto ticks = static_cast<long long>(filetime_to_ticks(ft));
  ticks += seconds * ticks_per_second;
  return ticks_to_filetime(static_cast<unsigned long long>(ticks));
}

auto utc_system_time_to_filetime(const SYSTEMTIME &utc)
    -> std::optional<FILETIME> {
  if (!valid_system_time(utc)) return std::nullopt;
  FILETIME ft{};
  if (!SystemTimeToFileTime(&utc, &ft)) return std::nullopt;
  return ft;
}

auto local_system_time_to_filetime(const SYSTEMTIME &local)
    -> std::optional<FILETIME> {
  if (!valid_system_time(local)) return std::nullopt;

  TIME_ZONE_INFORMATION tzi{};
  GetTimeZoneInformation(&tzi);

  SYSTEMTIME utc{};
  if (!TzSpecificLocalTimeToSystemTime(&tzi, &local, &utc)) {
    return std::nullopt;
  }

  FILETIME ft{};
  if (!SystemTimeToFileTime(&utc, &ft)) return std::nullopt;
  return ft;
}

auto filetime_to_system_time(const FILETIME &ft, bool use_utc)
    -> std::optional<SYSTEMTIME> {
  SYSTEMTIME st{};
  if (use_utc) {
    if (!FileTimeToSystemTime(&ft, &st)) return std::nullopt;
    return st;
  }

  FILETIME local_ft{};
  if (!FileTimeToLocalFileTime(&ft, &local_ft)) return std::nullopt;
  if (!FileTimeToSystemTime(&local_ft, &st)) return std::nullopt;
  return st;
}

auto parse_epoch_time(std::string_view s) -> std::optional<FILETIME> {
  if (s.empty() || s.front() != '@') return std::nullopt;
  long long seconds = 0;
  auto rest = s.substr(1);
  auto [ptr, ec] =
      std::from_chars(rest.data(), rest.data() + rest.size(), seconds);
  if (ec != std::errc() || ptr != rest.data() + rest.size())
    return std::nullopt;

  SYSTEMTIME epoch{};
  epoch.wYear = 1970;
  epoch.wMonth = 1;
  epoch.wDay = 1;
  auto ft = utc_system_time_to_filetime(epoch);
  if (!ft) return std::nullopt;
  return add_seconds(*ft, seconds);
}

auto parse_timezone_suffix(std::string &s) -> std::optional<int> {
  std::string lowered = lower_copy(s);
  if (lowered.ends_with(" utc") || lowered.ends_with(" gmt")) {
    s = trim_copy(s.substr(0, s.size() - 4));
    return 0;
  }
  if (lowered.ends_with("z") && s.size() > 1 &&
      std::isdigit(static_cast<unsigned char>(s[s.size() - 2]))) {
    s = trim_copy(s.substr(0, s.size() - 1));
    return 0;
  }
  if (s.size() < 5) return std::nullopt;

  size_t pos = s.size() - 5;
  if (s[pos] == '+' || s[pos] == '-') {
    std::string_view hh{s.data() + pos + 1, 2};
    std::string_view mm{s.data() + pos + 3, 2};
    if (is_digits(hh) && is_digits(mm)) {
      int hours = *parse_int(hh);
      int minutes = *parse_int(mm);
      if (hours <= 23 && minutes <= 59) {
        int sign = s[pos] == '-' ? -1 : 1;
        s = trim_copy(s.substr(0, pos));
        return sign * (hours * 60 + minutes);
      }
    }
  }

  if (s.size() >= 6) {
    pos = s.size() - 6;
    if ((s[pos] == '+' || s[pos] == '-') && s[pos + 3] == ':') {
      std::string_view hh{s.data() + pos + 1, 2};
      std::string_view mm{s.data() + pos + 4, 2};
      if (is_digits(hh) && is_digits(mm)) {
        int hours = *parse_int(hh);
        int minutes = *parse_int(mm);
        if (hours <= 23 && minutes <= 59) {
          int sign = s[pos] == '-' ? -1 : 1;
          s = trim_copy(s.substr(0, pos));
          return sign * (hours * 60 + minutes);
        }
      }
    }
  }

  return std::nullopt;
}

auto parse_fixed_date_time(std::string input) -> std::optional<FILETIME> {
  input = trim_copy(input);
  if (auto epoch = parse_epoch_time(input)) return epoch;

  auto tz_offset = parse_timezone_suffix(input);
  std::ranges::replace(input, 'T', ' ');

  std::string date_part = input;
  std::string time_part;
  if (auto space = input.find(' '); space != std::string::npos) {
    date_part = input.substr(0, space);
    time_part = trim_copy(input.substr(space + 1));
  }

  int year = 0;
  int month = 0;
  int day = 0;
  if (date_part.size() == 10 && (date_part[4] == '-' || date_part[4] == '/') &&
      date_part[7] == date_part[4]) {
    auto y = parse_int(std::string_view(date_part).substr(0, 4));
    auto m = parse_int(std::string_view(date_part).substr(5, 2));
    auto d = parse_int(std::string_view(date_part).substr(8, 2));
    if (!y || !m || !d) return std::nullopt;
    year = *y;
    month = *m;
    day = *d;
  } else if (date_part.size() == 8 && is_digits(date_part)) {
    year = *parse_int(std::string_view(date_part).substr(0, 4));
    month = *parse_int(std::string_view(date_part).substr(4, 2));
    day = *parse_int(std::string_view(date_part).substr(6, 2));
  } else {
    return std::nullopt;
  }

  int hour = 0;
  int minute = 0;
  int second = 0;
  if (!time_part.empty()) {
    std::vector<std::string_view> pieces;
    std::string_view tv = time_part;
    while (true) {
      auto colon = tv.find(':');
      pieces.push_back(tv.substr(0, colon));
      if (colon == std::string_view::npos) break;
      tv.remove_prefix(colon + 1);
    }
    if (pieces.size() < 2 || pieces.size() > 3) return std::nullopt;
    auto h = parse_int(pieces[0]);
    auto m = parse_int(pieces[1]);
    auto sec =
        pieces.size() == 3 ? parse_int(pieces[2]) : std::optional<int>{0};
    if (!h || !m || !sec) return std::nullopt;
    hour = *h;
    minute = *m;
    second = *sec;
  }

  SYSTEMTIME st{};
  st.wYear = static_cast<WORD>(year);
  st.wMonth = static_cast<WORD>(month);
  st.wDay = static_cast<WORD>(day);
  st.wHour = static_cast<WORD>(hour);
  st.wMinute = static_cast<WORD>(minute);
  st.wSecond = static_cast<WORD>(second);

  if (tz_offset) {
    auto ft = utc_system_time_to_filetime(st);
    if (!ft) return std::nullopt;
    return add_seconds(*ft, -static_cast<long long>(*tz_offset) * 60);
  }
  return local_system_time_to_filetime(st);
}

auto timezone_offset_minutes(const FILETIME &utc, bool use_utc) -> int {
  if (use_utc) return 0;
  FILETIME local_ft{};
  if (!FileTimeToLocalFileTime(&utc, &local_ft)) return 0;
  auto diff = static_cast<long long>(filetime_to_ticks(local_ft)) -
              static_cast<long long>(filetime_to_ticks(utc));
  return static_cast<int>(diff / (10000000LL * 60));
}

auto format_zone_offset(int minutes, bool colon) -> std::string {
  char buf[8]{};
  char sign = minutes < 0 ? '-' : '+';
  int total = std::abs(minutes);
  int hours = total / 60;
  int mins = total % 60;
  if (colon) {
    snprintf(buf, sizeof(buf), "%c%02d:%02d", sign, hours, mins);
  } else {
    snprintf(buf, sizeof(buf), "%c%02d%02d", sign, hours, mins);
  }
  return buf;
}

auto timezone_name(const FILETIME &utc, bool use_utc) -> std::string {
  if (use_utc) return "UTC";
  int offset = timezone_offset_minutes(utc, false);
  if (offset == 0) return "UTC";
  if (offset == 8 * 60) return "CST";
  if (offset == -5 * 60) return "EST";
  if (offset == -4 * 60) return "EDT";
  if (offset == -6 * 60) return "CST";
  if (offset == -7 * 60) return "MST";
  if (offset == -8 * 60) return "PST";
  return format_zone_offset(offset, false);
}

auto day_of_year(const SYSTEMTIME &st) -> int {
  int day = st.wDay;
  for (int month = 1; month < st.wMonth; ++month) {
    day += days_in_month(st.wYear, month);
  }
  return day;
}

auto epoch_seconds(const FILETIME &ft) -> long long {
  SYSTEMTIME epoch{};
  epoch.wYear = 1970;
  epoch.wMonth = 1;
  epoch.wDay = 1;
  auto epoch_ft = utc_system_time_to_filetime(epoch).value();
  auto diff = static_cast<long long>(filetime_to_ticks(ft)) -
              static_cast<long long>(filetime_to_ticks(epoch_ft));
  return diff / 10000000LL;
}

auto append_number(std::string &out, int value, int width, char fill = '0') {
  char buf[32]{};
  snprintf(buf, sizeof(buf), fill == ' ' ? "%*d" : "%0*d", width, value);
  out += buf;
}

auto format_time(const TimeValue &tv, const std::string &format)
    -> std::string {
  const SYSTEMTIME &st = tv.display;
  int zone_minutes = timezone_offset_minutes(tv.utc, tv.utc_display);
  std::string result;
  result.reserve(format.size() * 2);

  for (size_t i = 0; i < format.size(); ++i) {
    if (format[i] == '%' && i + 1 < format.size()) {
      bool colon_zone = false;
      if (format[i + 1] == ':' && i + 2 < format.size()) {
        colon_zone = true;
        ++i;
      }
      char spec = format[++i];

      switch (spec) {
        case '%':
          result += '%';
          break;
        case 'n':
          result += '\n';
          break;
        case 't':
          result += '\t';
          break;
        case 'Y':
          append_number(result, st.wYear, 4);
          break;
        case 'y':
          append_number(result, st.wYear % 100, 2);
          break;
        case 'C':
          append_number(result, st.wYear / 100, 2);
          break;
        case 'm':
          append_number(result, st.wMonth, 2);
          break;
        case 'd':
          append_number(result, st.wDay, 2);
          break;
        case 'e':
          append_number(result, st.wDay, 2, ' ');
          break;
        case 'H':
          append_number(result, st.wHour, 2);
          break;
        case 'I':
          append_number(result, (st.wHour % 12 == 0) ? 12 : st.wHour % 12, 2);
          break;
        case 'M':
          append_number(result, st.wMinute, 2);
          break;
        case 'S':
          append_number(result, st.wSecond, 2);
          break;
        case 'N':
          append_number(result, st.wMilliseconds * 1000000, 9);
          break;
        case 'p':
          result += st.wHour < 12 ? "AM" : "PM";
          break;
        case 'P':
          result += st.wHour < 12 ? "am" : "pm";
          break;
        case 'a':
        case 'A': {
          static const char *weekday_names[] = {
              "Sunday",   "Monday", "Tuesday", "Wednesday",
              "Thursday", "Friday", "Saturday"};
          static const char *weekday_abbr[] = {"Sun", "Mon", "Tue", "Wed",
                                               "Thu", "Fri", "Sat"};
          int day_of_week = st.wDayOfWeek;
          result += (spec == 'a') ? weekday_abbr[day_of_week]
                                  : weekday_names[day_of_week];
          break;
        }
        case 'b':
        case 'h':
        case 'B': {
          static const char *month_names[] = {
              "January",   "February", "March",    "April",
              "May",       "June",     "July",     "August",
              "September", "October",  "November", "December"};
          static const char *month_abbr[] = {"Jan", "Feb", "Mar", "Apr",
                                             "May", "Jun", "Jul", "Aug",
                                             "Sep", "Oct", "Nov", "Dec"};
          result += (spec == 'b') ? month_abbr[st.wMonth - 1]
                                  : month_names[st.wMonth - 1];
          break;
        }
        case 'F':
          result += format_time(tv, "%Y-%m-%d");
          break;
        case 'D':
          result += format_time(tv, "%m/%d/%y");
          break;
        case 'T':
          result += format_time(tv, "%H:%M:%S");
          break;
        case 'R':
          result += format_time(tv, "%H:%M");
          break;
        case 'r':
          result += format_time(tv, "%I:%M:%S %p");
          break;
        case 'c':
          result += format_time(tv, "%a %b %e %H:%M:%S %Y");
          break;
        case 'x':
          result += format_time(tv, "%m/%d/%y");
          break;
        case 'X':
          result += format_time(tv, "%H:%M:%S");
          break;
        case 'j':
          append_number(result, day_of_year(st), 3);
          break;
        case 'u':
          result +=
              static_cast<char>('0' + (st.wDayOfWeek == 0 ? 7 : st.wDayOfWeek));
          break;
        case 'w':
          result += static_cast<char>('0' + st.wDayOfWeek);
          break;
        case 's':
          result += std::to_string(epoch_seconds(tv.utc));
          break;
        case 'z':
          result += format_zone_offset(zone_minutes, colon_zone);
          break;
        case 'Z':
          result += timezone_name(tv.utc, tv.utc_display);
          break;
        default:
          result += '%';
          if (colon_zone) result += ':';
          result += spec;
          break;
      }
    } else {
      result += format[i];
    }
  }

  return result;
}

auto make_time_value(FILETIME utc, bool use_utc) -> std::optional<TimeValue> {
  auto display = filetime_to_system_time(utc, use_utc);
  if (!display) return std::nullopt;
  return TimeValue{utc, *display, use_utc};
}

auto current_time_value(bool use_utc) -> TimeValue {
  FILETIME utc{};
  GetSystemTimeAsFileTime(&utc);
  return make_time_value(utc, use_utc).value();
}

auto read_reference_time(std::string_view path) -> std::optional<FILETIME> {
  WIN32_FILE_ATTRIBUTE_DATA attributes{};
  if (!GetFileAttributesExW(utf8_to_wstring(std::string(path)).c_str(),
                            GetFileExInfoStandard, &attributes)) {
    return std::nullopt;
  }
  return attributes.ftLastWriteTime;
}

auto parse_date_argument(const std::string &arg) -> std::optional<FILETIME> {
  std::string value = trim_copy(arg);
  std::string lower = lower_copy(value);
  FILETIME now{};
  GetSystemTimeAsFileTime(&now);
  auto relative = [&](long long amount, long long unit) {
    return add_seconds(now, amount * unit);
  };
  std::smatch match;
  const std::regex relative_re(
      R"(^([+-])([0-9]+)\s*(second|seconds|minute|minutes|hour|hours|day|days|week|weeks|fortnight|fortnights|month|months|year|years)$)");
  if (std::regex_match(lower, match, relative_re)) {
    long long amount = std::stoll(match[2].str());
    if (match[1].str() == "-") amount = -amount;
    const auto unit = match[3].str();
    long long seconds = 1;
    if (unit.starts_with("minute"))
      seconds = 60;
    else if (unit.starts_with("hour"))
      seconds = 3600;
    else if (unit.starts_with("day"))
      seconds = 86400;
    else if (unit.starts_with("fortnight"))
      seconds = 1209600;  // 2 weeks
    else if (unit.starts_with("week"))
      seconds = 604800;
    else if (unit.starts_with("month"))
      seconds = 2629746;  // Average month length (30.44 days)
    else if (unit.starts_with("year"))
      seconds = 31557600;  // Average year length (365.25 days)
    return relative(amount, seconds);
  }
  // [GNU] Natural language date support
  if (lower == "now" || lower == "today") return now;
  if (lower == "tomorrow") return relative(1, 86400);
  if (lower == "yesterday") return relative(-1, 86400);

  // "next monday", "next week", etc.
  std::smatch next_match;
  const std::regex next_re(
      R"(^next\s+(monday|tuesday|wednesday|thursday|friday|saturday|sunday|week|month|year)$)");
  if (std::regex_match(lower, next_match, next_re)) {
    std::string unit = next_match[1].str();
    if (unit == "week") return relative(1, 604800);
    if (unit == "month") return relative(1, 2629746);
    if (unit == "year") return relative(1, 31557600);
    // For days of the week, calculate next occurrence
    SYSTEMTIME st{};
    FileTimeToSystemTime(&now, &st);
    int current_dow = st.wDayOfWeek;  // 0=Sunday, 1=Monday, ...
    int target_dow = 0;
    if (unit == "monday")
      target_dow = 1;
    else if (unit == "tuesday")
      target_dow = 2;
    else if (unit == "wednesday")
      target_dow = 3;
    else if (unit == "thursday")
      target_dow = 4;
    else if (unit == "friday")
      target_dow = 5;
    else if (unit == "saturday")
      target_dow = 6;
    else if (unit == "sunday")
      target_dow = 0;
    int days_ahead = (target_dow - current_dow + 7) % 7;
    if (days_ahead == 0) days_ahead = 7;
    return relative(days_ahead, 86400);
  }

  // "last monday", "last week", etc.
  std::smatch last_match;
  const std::regex last_re(
      R"(^last\s+(monday|tuesday|wednesday|thursday|friday|saturday|sunday|week|month|year)$)");
  if (std::regex_match(lower, last_match, last_re)) {
    std::string unit = last_match[1].str();
    if (unit == "week") return relative(-1, 604800);
    if (unit == "month") return relative(-1, 2629746);
    if (unit == "year") return relative(-1, 31557600);
    // For days of the week, calculate last occurrence
    SYSTEMTIME st{};
    FileTimeToSystemTime(&now, &st);
    int current_dow = st.wDayOfWeek;
    int target_dow = 0;
    if (unit == "monday")
      target_dow = 1;
    else if (unit == "tuesday")
      target_dow = 2;
    else if (unit == "wednesday")
      target_dow = 3;
    else if (unit == "thursday")
      target_dow = 4;
    else if (unit == "friday")
      target_dow = 5;
    else if (unit == "saturday")
      target_dow = 6;
    else if (unit == "sunday")
      target_dow = 0;
    int days_back = (current_dow - target_dow + 7) % 7;
    if (days_back == 0) days_back = 7;
    return relative(-days_back, 86400);
  }

  return parse_fixed_date_time(arg);
}

auto normalize_timespec(std::string spec, bool default_date) -> std::string {
  spec = lower_copy(trim_copy(spec));
  if (spec.empty()) return default_date ? "date" : "seconds";
  return spec;
}

auto iso_format_for(std::string spec) -> std::optional<std::string> {
  spec = normalize_timespec(std::move(spec), true);
  if (spec == "date") return "%Y-%m-%d";
  if (spec == "hours") return "%Y-%m-%dT%H%:z";
  if (spec == "minutes") return "%Y-%m-%dT%H:%M%:z";
  if (spec == "seconds") return "%Y-%m-%dT%H:%M:%S%:z";
  if (spec == "ns" || spec == "nanoseconds") return "%Y-%m-%dT%H:%M:%S.%N%:z";
  return std::nullopt;
}

auto rfc3339_format_for(std::string spec) -> std::optional<std::string> {
  spec = normalize_timespec(std::move(spec), false);
  if (spec == "date") return "%Y-%m-%d";
  if (spec == "seconds") return "%Y-%m-%d %H:%M:%S%:z";
  if (spec == "ns" || spec == "nanoseconds") return "%Y-%m-%d %H:%M:%S.%N%:z";
  return std::nullopt;
}

}  // namespace date_pipeline

REGISTER_COMMAND(
    date, "date", "print or set the system date and time",
    "Display the current time in the given FORMAT, or set the system date.\n"
    "\n"
    "FORMAT controls the output. Interpreted sequences are:\n"
    "  %Y   Year (4 digits)\n"
    "  %y   Year (2 digits)\n"
    "  %m   Month (01-12)\n"
    "  %d   Day (01-31)\n"
    "  %H   Hour (00-23)\n"
    "  %I   Hour (01-12)\n"
    "  %M   Minute (00-59)\n"
    "  %S   Second (00-60)\n"
    "  %F   Full date; same as %Y-%m-%d\n"
    "  %T   Time; same as %H:%M:%S\n"
    "  %z   Numeric time zone offset\n"
    "  %p   AM/PM\n"
    "  %a   Abbreviated weekday name\n"
    "  %A   Full weekday name\n"
    "  %b   Abbreviated month name\n"
    "  %B   Full month name",
    "  date                    Display current date and time\n"
    "  date +'%Y-%m-%d'        Display date in YYYY-MM-DD format\n"
    "  date +'%H:%M:%S'        Display time in HH:MM:SS format\n"
    "  date -u                 Display UTC time\n"
    "  date -R                 Display RFC email format\n"
    "  date -r FILE            Display FILE modification time\n"
    "  date -Iseconds          Display ISO 8601 format",
    "cal(1)", "caomengxuan666", "Copyright © 2026 WinuxCmd", DATE_OPTIONS) {
  using namespace date_pipeline;

  if (ctx.has("--set") || ctx.has("-s")) {
    std::string set_arg = ctx.get<std::string>("--set", "");
    if (set_arg.empty()) set_arg = ctx.get<std::string>("-s", "");
    auto parsed = parse_date_argument(set_arg);
    if (!parsed) {
      safeErrorPrint("date: invalid date '");
      safeErrorPrint(set_arg);
      safeErrorPrintLn("'");
      return 1;
    }

    // Convert FILETIME (UTC) to SYSTEMTIME (UTC), then to local time
    // because SetSystemTime expects local time
    SYSTEMTIME utc_st{};
    if (!FileTimeToSystemTime(&*parsed, &utc_st)) {
      safeErrorPrintLn("date: cannot convert date");
      return 1;
    }
    // Convert UTC SYSTEMTIME to FILETIME, then to local FILETIME, then to
    // SYSTEMTIME
    FILETIME utc_ft{};
    if (!SystemTimeToFileTime(&utc_st, &utc_ft)) {
      safeErrorPrintLn("date: cannot convert time");
      return 1;
    }
    FILETIME local_ft{};
    if (!FileTimeToLocalFileTime(&utc_ft, &local_ft)) {
      safeErrorPrintLn("date: cannot convert to local time");
      return 1;
    }
    SYSTEMTIME local_st2{};
    if (!FileTimeToSystemTime(&local_ft, &local_st2)) {
      safeErrorPrintLn("date: cannot convert time");
      return 1;
    }
    if (!SetSystemTime(&local_st2)) {
      safeErrorPrintLn(
          "date: cannot set system time (administrator privileges required)");
      return 1;
    }
    return 0;
  }

  if (ctx.has("--resolution")) {
    // FILETIME timestamps have a fixed 100 ns resolution on Windows.
    safePrintLn("100ns");
    return 0;
  }

  // [GNU] --universal: alias for --utc
  bool use_utc = ctx.get<bool>("-u", false) || ctx.get<bool>("--utc", false) ||
                 ctx.get<bool>("--universal", false);
  bool rfc2822 = ctx.get<bool>("-R", false) ||
                 ctx.get<bool>("--rfc-email", false) ||
                 ctx.get<bool>("--rfc-2822", false);

  FILETIME selected_time{};
  std::string reference_path = ctx.get<std::string>("--reference", "");
  if (reference_path.empty()) reference_path = ctx.get<std::string>("-r", "");
  if (!reference_path.empty()) {
    auto reference_time = read_reference_time(reference_path);
    if (!reference_time) {
      safeErrorPrint("date: failed to get modification time of '");
      safeErrorPrint(reference_path);
      safeErrorPrint("'\n");
      return 1;
    }
    selected_time = *reference_time;
  }

  std::string date_arg = ctx.get<std::string>("--date", "");
  if (date_arg.empty()) date_arg = ctx.get<std::string>("-d", "");
  if (!date_arg.empty()) {
    auto parsed = parse_date_argument(date_arg);
    if (!parsed) {
      safeErrorPrint("date: invalid date '");
      safeErrorPrint(date_arg);
      safeErrorPrint("'\n");
      return 1;
    }
    selected_time = *parsed;
    if (ctx.has("--debug")) {
      safeErrorPrint("date: parsed date ");
      safeErrorPrintLn(date_arg);
    }
  } else if (reference_path.empty()) {
    GetSystemTimeAsFileTime(&selected_time);
  }

  auto tv = make_time_value(selected_time, use_utc);
  if (!tv) {
    safeErrorPrint("date: failed to convert time\n");
    return 1;
  }

  std::string format = "%a %b %e %H:%M:%S %Z %Y";

  for (auto arg : ctx.positionals) {
    std::string_view arg_sv = arg;
    if (arg_sv.starts_with("+")) {
      format = std::string(arg_sv.substr(1));
      break;
    }
  }

  if (rfc2822) {
    format = "%a, %d %b %Y %H:%M:%S %z";
  }

  std::string iso_arg = ctx.get<std::string>("--iso-8601", "");
  if (!ctx.has("--iso-8601")) iso_arg = ctx.get<std::string>("-I", "");
  if (ctx.has("--iso-8601") || ctx.has("-I")) {
    auto iso_format = iso_format_for(iso_arg);
    if (!iso_format) {
      safeErrorPrint("date: invalid argument '");
      safeErrorPrint(iso_arg);
      safeErrorPrint("' for '--iso-8601'\n");
      return 1;
    }
    format = *iso_format;
  }

  std::string rfc3339_arg = ctx.get<std::string>("--rfc-3339", "");
  if (!rfc3339_arg.empty()) {
    auto rfc3339_format = rfc3339_format_for(rfc3339_arg);
    if (!rfc3339_format) {
      safeErrorPrint("date: invalid argument '");
      safeErrorPrint(rfc3339_arg);
      safeErrorPrint("' for '--rfc-3339'\n");
      return 1;
    }
    format = *rfc3339_format;
  }

  // [GNU] -f/--file: display date strings from DATEFILE, one per line
  std::string date_file = ctx.get<std::string>("--file", "");
  if (date_file.empty()) date_file = ctx.get<std::string>("-f", "");
  if (!date_file.empty()) {
    std::ifstream ifs(date_file);
    if (!ifs) {
      safeErrorPrint("date: cannot open '");
      safeErrorPrint(date_file);
      safeErrorPrintLn("'");
      return 1;
    }
    std::string line;
    bool first = true;
    while (std::getline(ifs, line)) {
      auto parsed = parse_date_argument(line);
      if (!parsed) {
        safeErrorPrint("date: invalid date '");
        safeErrorPrint(line);
        safeErrorPrintLn("'");
        return 1;
      }
      auto file_tv = make_time_value(*parsed, use_utc);
      if (!file_tv) {
        safeErrorPrintLn("date: failed to convert time");
        return 1;
      }
      if (!first) safePrint("\n");
      first = false;
      safePrint(format_time(*file_tv, format));
    }
    safePrint("\n");
    return 0;
  }

  std::string output = format_time(*tv, format);
  safePrintLn(output);

  return 0;
}
