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

#include "game_loader.h"

#include <windows.h>
#include <iostream>

namespace sgd2gexe {

bool
StartGame(
    PROCESS_INFORMATION* process_info_out_ptr
) {
  STARTUPINFOW startup_info = { };
  startup_info.cb = sizeof(startup_info);

  BOOL is_create_process_success = CreateProcessW(
      L"Game.exe",
      GetCommandLineW(),
      nullptr,
      nullptr,
      true,
      0,
      nullptr,
      nullptr,
      &startup_info,
      process_info_out_ptr
  );

  // Create the desired process.
  if (!is_create_process_success) {
    MessageBoxW(
        nullptr,
        L"Game.exe could not be found.",
        L"Game executable not found.",
        MB_OK | MB_ICONERROR
    );
    std::exit(0);
  }

  // Wait until the process is started.
  return WaitForInputIdle(process_info_out_ptr->hProcess, INFINITE) == 0;
}

bool StartGameSuspended(PROCESS_INFORMATION* process_info_out_ptr) {
  STARTUPINFOW startup_info = { };
  startup_info.cb = sizeof(startup_info);

  BOOL is_create_process_success = CreateProcessW(L"Game.exe",
      GetCommandLineW(),
      nullptr,
      nullptr,
      true,
      CREATE_SUSPENDED,
      nullptr,
      nullptr,
      &startup_info,
      process_info_out_ptr
  );

  // Create the desired process.
  if (!is_create_process_success) {
    MessageBoxW(
        nullptr,
        L"Game.exe could not be found.",
        L"Game executable not found.",
        MB_OK | MB_ICONERROR
    );
    std::exit(0);
  }

  // Wait until the process is started.
  return true;
}

} // namespace sgd2gexe
