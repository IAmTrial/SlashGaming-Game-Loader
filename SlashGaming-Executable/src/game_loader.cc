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
 *  it with any program (or a modified version of that program and its
 *  libraries), containing parts covered by the terms of an incompatible
 *  license, the licensors of this Program grant you additional permission
 *  to convey the resulting work.
 */

#include "game_loader.h"

#include <windows.h>
#include <filesystem>
#include <iostream>

#include <boost/format.hpp>

namespace sgexe {
namespace {

constexpr std::wstring_view kGameFileNotFoundErrorMessage =
    L"%s could not be found.";

constexpr std::wstring_view kCreateErrorMessage =
    L"%s could not be started, with error code %x.";

} // namespace

bool
StartGame(
    std::filesystem::path game_file_path,
    PROCESS_INFORMATION* process_info_out_ptr
) {
  if (!std::filesystem::exists(game_file_path)) {
    std::wstring full_message = (
        boost::wformat(kGameFileNotFoundErrorMessage.data())
            % game_file_path
    ).str();

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"Game Executable Not Found",
        MB_OK | MB_ICONERROR
    );
    std::exit(0);
  }

  STARTUPINFOW startup_info = { };
  startup_info.cb = sizeof(startup_info);

  BOOL is_create_process_success = CreateProcessW(
      game_file_path.c_str(),
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
    std::wstring full_message = (
        boost::wformat(kCreateErrorMessage.data())
            % game_file_path.c_str()
            % GetLastError()
    ).str();

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"Could Not Start Game",
        MB_OK | MB_ICONERROR
    );
    std::exit(0);
  }

  // Wait until the process is started.
  return WaitForInputIdle(process_info_out_ptr->hProcess, INFINITE) == 0;
}

bool
StartGameSuspended(
    std::filesystem::path game_file_path,
    PROCESS_INFORMATION* process_info_out_ptr
) {
  if (!std::filesystem::exists(game_file_path)) {
    MessageBoxW(
        nullptr,
        L"Game.exe could not be found.",
        L"Game Executable Not Found",
        MB_OK | MB_ICONERROR
    );
    std::exit(0);
  }

  STARTUPINFOW startup_info = { };
  startup_info.cb = sizeof(startup_info);

  BOOL is_create_process_success = CreateProcessW(
      game_file_path.c_str(),
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
    std::wstring full_message = (
        boost::wformat(kCreateErrorMessage.data())
            % game_file_path.c_str()
            % GetLastError()
    ).str();

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"Could Not Start Game",
        MB_OK | MB_ICONERROR
    );
    std::exit(0);
  }

  return true;
}

} // namespace sgexe
