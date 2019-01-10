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

#include "extern_import.h"

#include <windows.h>
#include <cstdlib>

#include <boost/format.hpp>

const char* GetGameExecutableFileName(
    HMODULE dll_handle
) {
  using FuncType = const char*(*)(void);
  static FuncType func = reinterpret_cast<FuncType>(
      GetProcAddress(
          dll_handle,
          __func__
      )
  );

  if (func == nullptr) {
    std::wstring full_message = (
        boost::wformat(L"GetProcAddress failed in %s, with error code %x.")
            % __func__
            % GetLastError()
    ).str();

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"GetProcAddress Failed",
        MB_OK | MB_ICONERROR
    );

    std::exit(0);
  }

  return func();
}

const char* GetGameVersionText(
    HMODULE dll_handle
) {
  using FuncType = const char*(*)(void);
  static FuncType func = reinterpret_cast<FuncType>(
      GetProcAddress(
          dll_handle,
          __func__
      )
  );

  if (func == nullptr) {
    return "Unidentified Version";
  }

  return func();
}
