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

#include "game_loader.h"

#include <windows.h>
#include <cstdlib>
#include <string>
#include <string_view>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/scope_exit.hpp>

namespace sgexe {
namespace {

constexpr std::wstring_view kGameFileNotFoundErrorMessage =
    L"%s could not be found.";

constexpr std::wstring_view kCreateErrorMessage =
    L"%s could not be started, with error code %x.";

} // namespace

PROCESS_INFORMATION
StartGame(
    const boost::filesystem::path& game_file_path
) {
  if (!boost::filesystem::exists(game_file_path)) {
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

  PROCESS_INFORMATION process_info;

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
      &process_info
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
  HANDLE open_process_result;
  do {
    open_process_result = OpenProcess(
        PROCESS_QUERY_INFORMATION,
        FALSE,
        process_info.dwProcessId
    );

    Sleep(100);
  } while (open_process_result == nullptr);

  BOOST_SCOPE_EXIT(&open_process_result) {
    CloseHandle(open_process_result);
  } BOOST_SCOPE_EXIT_END

  return process_info;
}

PROCESS_INFORMATION
StartGameSuspended(
    const boost::filesystem::path& game_file_path
) {
  if (!boost::filesystem::exists(game_file_path)) {
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

  PROCESS_INFORMATION process_info;

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
      &process_info
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

  return process_info;
}

} // namespace sgexe
