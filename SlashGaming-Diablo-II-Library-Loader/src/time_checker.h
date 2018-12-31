/**
 * SlashGaming Diablo II Game Loader
 * Copyright (C) 2018  Mir Drualga
 *
 * This file is part of SlashGaming Diablo II Game Loader.
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

#ifndef SGD2GLDR_TIME_CHECKER_H_
#define SGD2GLDR_TIME_CHECKER_H_

#include <chrono>
#include <string_view>

namespace sgd2gldr::timechecker {

  constexpr bool kIsEnforceTimestamp = false;
  constexpr std::string_view kCompilationDate = __DATE__;
  constexpr int kAllowedMonthDifference = 1;

  void EnforceTimeStamp();

} // namespace sgd2gldr::timechecker

#endif // SGD2GLDR_TIME_CHECKER_H_
