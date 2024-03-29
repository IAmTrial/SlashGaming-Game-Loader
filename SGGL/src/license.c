/**
 * SlashGaming Game Loader
 * Copyright (C) 2018-2021  Mir Drualga
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

#include <stddef.h>
#include <stdio.h>

#include <mdc/std/wchar.h>

/**
 * External
 */

const wchar_t* const License_kText[] = {
    L"SlashGaming Game Loader",
    L"Copyright (C) 2018-2021  Mir Drualga",
    L"",
    L"This program is free software: you can redistribute it and/or modify",
    L"it under the terms of the GNU Affero General Public License as published",
    L"by the Free Software Foundation, either version 3 of the License, or",
    L"(at your option) any later version.",
    L"",
    L"This program is distributed in the hope that it will be useful,",
    L"but WITHOUT ANY WARRANTY; without even the implied warranty of",
    L"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
    L"GNU Affero General Public License for more details.",
    L"",
    L"You should have received a copy of the GNU Affero General Public License",
    L"along with this program.  If not, see <http://www.gnu.org/licenses/>.",
    L"",
    L"Additional permissions under GNU Affero General Public License version 3",
    L"section 7 have also been granted; please check the LICENSE file for more",
    L"information."
};

void License_PrintText(void) {
  size_t i;

  for (i = 0; i < License_kTextCount; ++i) {
    wprintf(L"%ls\n", License_kText[i]);
  }
}
