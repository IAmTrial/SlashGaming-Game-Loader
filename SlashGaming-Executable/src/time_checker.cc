/**
 * SlashGaming Game Loader
 * Copyright (C) 2018  Mir Drualga
 *
 * This file is part of SlashGaming Game Loader.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Additional permissions under GNU Affero General Public License version 3
 *  section 7
 *
 *  If you modify this Program, or any covered work, by linking or combining
 *  it with Diablo II (or a modified version of that game and its
 *  libraries), containing parts covered by the terms of Blizzard End User
 *  License Agreement, the licensors of this Program grant you additional
 *  permission to convey the resulting work.  This additional permission is
 *  also extended to any combination of expansions, mods, and remasters of
 *  the game.
 */

#include "time_checker.h"

#include <windows.h>
#include <cstdint>
#include <charconv>
#include <chrono>
#include <iostream>
#include <ratio>
#include <regex>
#include <string_view>
#include <utility>

#include <boost/bimap.hpp>
#include <boost/format.hpp>

namespace sgexe::timechecker {

using MonthsDuration = std::chrono::duration<intmax_t, std::ratio<2629746>>;

namespace {

using MonthNameAndValueBimapType = boost::bimap<std::string_view, int>;

constexpr std::string_view kTimestampMessage01 =
    "Timestamp is enforced, meaning that this program will cease "
    "to function %d month(s) after %s.";

constexpr std::string_view kTimestampMessage02 =
    "This means that you have received a version of this "
    "program not meant for public release.";

const boost::bimap<std::string_view, int>&
GetMonthNameAndValueBimap(
    void
) {
  static const std::array<MonthNameAndValueBimapType::value_type, 12>
      month_name_and_value_pairs = { {
          { "Jan", 0 }, { "Feb", 1 }, { "Mar", 2 }, { "Apr", 3 },
          { "May", 4 }, { "Jun", 5 }, { "Jul", 6 }, { "Aug", 7 },
          { "Sep", 8 }, { "Oct", 9 }, { "Nov", 10 }, { "Dec", 11 }
      } };

  static const MonthNameAndValueBimapType month_name_and_values_bimap(
      month_name_and_value_pairs.begin(),
      month_name_and_value_pairs.end()
  );

  return month_name_and_values_bimap;
}

MonthsDuration
GetDaysFromDateString(
    std::string_view date
) {
  const std::regex kCompileDateRegex("(\\w+)\\s+(\\d+)\\s+(\\d+)");
  std::cmatch matches;

  if (!std::regex_match(date.data(), matches, kCompileDateRegex)) {
    return MonthsDuration(0);
  }

  // Calculate number of months from epoch time (Jan 1, 1970).
  int month = GetMonthNameAndValueBimap().left.at(
      matches[1].str()
  );

  std::string year_text = matches[3].str();
  std::size_t year_text_size = year_text.length()
      * sizeof(decltype(year_text)::value_type);

  std::intmax_t year;
  std::from_chars(
      year_text.data(),
      year_text.data() + year_text_size,
      year
  );
  year -= 1970;

  MonthsDuration total_days_duration(month + (year * 12));

  return total_days_duration;
}

bool
IsExecutionPermitted(
    void
) {
  auto compile_month_duration = GetDaysFromDateString(kCompilationDate);

  // If the compilation date could not be parsed, don't allow execution.
  if (compile_month_duration == MonthsDuration(0)) {
    return false;
  }

  // Calculate the difference in months between the two.
  auto today_time_point = std::chrono::system_clock::now();
  auto today_duration = today_time_point.time_since_epoch();
  auto today_month_duration =
      std::chrono::duration_cast<MonthsDuration>(today_duration);

  auto month_difference = today_month_duration - compile_month_duration;

  return (month_difference.count() >= 0)
          && (month_difference.count() <= kAllowedMonthDifference);
}

} // namespace

void
EnforceTimeStamp(
    void
) {
  if constexpr (!kIsEnforceTimestamp) {
    return;
  }

  std::string full_message_01 = (
      boost::format(kTimestampMessage01.data())
          % kAllowedMonthDifference
          % kCompilationDate
  ).str();

  std::cout << full_message_01 << std::endl;
  std::cout << kTimestampMessage02 << std::endl << std::endl;

  if (!IsExecutionPermitted()) {
    MessageBoxW(
        nullptr,
        L"Date of execution exceeds timestamp limit.",
        L"Execution Date Exceeded",
        MB_OK | MB_ICONERROR
    );

    std::exit(0);
  }
}

} // namespace sgexe
