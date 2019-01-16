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

#include <windows.h>
#include <clocale>
#include <array>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <unordered_set>

#include <boost/format.hpp>
#include <boost/scope_exit.hpp>
#include "config_reader.h"
#include "extern_import.h"
#include "game_loader.h"
#include "library_injector.h"
#include "license.h"
#include "time_checker.h"

namespace sgexe {

extern "C" int
main(
    int argc,
    const char* argv[]
) {
  std::setlocale(LC_ALL, "en_US.UTF-8");

  // Print the license notice.
  license::PrintLicenseNotice();
  for (int i = 0; i < 79; i++) {
    std::cout << '-';
  }
  std::cout << std::endl;

  // ShowWindow(GetConsoleWindow(), SW_HIDE);
  timechecker::EnforceTimeStamp();

  // Load the injectable library to retrieve necessary information.
  std::filesystem::path version_detector_path =
      config::GetVersionDetectorLibraryPath();
  if (!std::filesystem::exists(version_detector_path)) {
    std::wstring full_message = (
        boost::wformat(L"The file %s could not be found.")
            % version_detector_path.c_str()
    ).str();

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"File Not Found",
        MB_OK | MB_ICONERROR
    );
    std::exit(0);
  }

  // Log the current game version (for users).
  HMODULE dll_handle = LoadLibraryW(version_detector_path.c_str());
  std::string_view game_version_text = GetGameVersionText(dll_handle);
  std::cout << "Game version is: " << game_version_text << std::endl;

  // Create a new process.
  std::filesystem::path game_executable_path =
      GetGameExecutableFileName(dll_handle);
  PROCESS_INFORMATION process_info = StartGame(game_executable_path);

  BOOST_SCOPE_EXIT(&process_info) {
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
  } BOOST_SCOPE_EXIT_END

  // Inject the library, after reading all files.
  std::vector inject_libraries_paths = config::GetInjectDllsPaths();
  if (InjectLibraries(inject_libraries_paths, process_info)) {
    std::cout << "All libraries have been successfully injected." << std::endl;
  } else {
    std::cout << "Some or all libraries failed to inject." << std::endl;
  }

  Sleep(500);

  return 0;
}

} // namespace sgexe
