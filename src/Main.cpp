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
 *  permission to convey the resulting work.
 *
 *  If you modify this Program, or any covered work, by linking or combining
 *  it with Diablo II: Lord of Destruction (or a modified version of that
 *  game and its libraries), containing parts covered by the terms of
 *  Blizzard End User License Agreement, the licensors of this Program grant
 *  you additional permission to convey the resulting work.
 */

#include <windows.h>
#include <array>
#include <iostream>

#include "GameLoader.h"
#include "LibraryInjector.h"
#include "TimeChecker.h"

namespace slashgaming {

namespace {

constexpr const std::array license_text = {
    "SlashGaming Diablo II Game Loader",
    "Copyright (C) 2018  Mir Drualga",
    "",
    "This program is free software: you can redistribute it and/or modify",
    "it under the terms of the GNU Affero General Public License as published",
    "by the Free Software Foundation, either version 3 of the License, or",
    "(at your option) any later version.",
    "",
    "This program is distributed in the hope that it will be useful,",
    "but WITHOUT ANY WARRANTY; without even the implied warranty of",
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
    "GNU Affero General Public License for more details.",
    "",
    "You should have received a copy of the GNU Affero General Public License",
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.",
    "",
    "Additional permissions under GNU Affero General Public License version 3",
    "section 7 have also been granted; please check the LICENSE file for more",
    "information."
};

void PrintLicenseNotice() {
  for (const auto& line : license_text) {
    std::cout << line << std::endl;
  }

  std::cout << std::endl;
}

} // namespace

extern "C" int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                LPSTR lpCmdLine, int nCmdShow) {
  PrintLicenseNotice();
  std::cout << "----------" << std::endl;

  // ShowWindow(GetConsoleWindow(), SW_HIDE);
  timechecker::EnforceTimeStamp();

  // Create a new process, waiting for its full initialization before the
  // startGame function can return.
  PROCESS_INFORMATION process_info;
  StartGame(&process_info);

  std::unordered_set<std::string_view> library_paths = {};

  // Inject libraries, after reading all files.
  if (InjectLibraries(library_paths, &process_info)) {
    std::cout << "All libraries have been successfully injected." << std::endl;
  } else {
    std::cout << "Some or all libraries failed to inject." << std::endl;
  }

  Sleep(500);

  return 0;
}

} // namespace slashgaming
