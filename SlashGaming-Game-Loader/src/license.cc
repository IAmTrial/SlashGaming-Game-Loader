/**
 * SlashGaming Game Loader
 * Copyright (C) 2018-2019  Mir Drualga
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
 *  it with any program (or a modified version of that program and its
 *  libraries), containing parts covered by the terms of an incompatible
 *  license, the licensors of this Program grant you additional permission
 *  to convey the resulting work.
 */

#include "license.h"

#include <array>
#include <string_view>

#include <fmt/printf.h>

namespace sgexe::license {
namespace {

constexpr const std::array<std::string_view, 19> kLicenseLines = {
    u8"SlashGaming Game Loader",
    u8"Copyright (C) 2018-2019  Mir Drualga",
    u8"",
    u8"This program is free software: you can redistribute it and/or modify",
    u8"it under the terms of the GNU Affero General Public License as published",
    u8"by the Free Software Foundation, either version 3 of the License, or",
    u8"(at your option) any later version.",
    u8"",
    u8"This program is distributed in the hope that it will be useful,",
    u8"but WITHOUT ANY WARRANTY; without even the implied warranty of",
    u8"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
    u8"GNU Affero General Public License for more details.",
    u8"",
    u8"You should have received a copy of the GNU Affero General Public License",
    u8"along with this program.  If not, see <http://www.gnu.org/licenses/>.",
    u8"",
    u8"Additional permissions under GNU Affero General Public License version 3",
    u8"section 7 have also been granted; please check the LICENSE file for more",
    u8"information."
};

} // namespace

void
PrintLicenseNotice(
    void
) {
  for (const auto& line : kLicenseLines) {
    fmt::printf("%s \n", line);
  }
}

} // namespace sgexe
