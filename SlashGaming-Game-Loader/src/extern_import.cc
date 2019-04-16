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
#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <fmt/printf.h>
#include "wide_macro.h"

namespace sgexe {
namespace {

constexpr std::wstring_view kFunctionFailErrorMessage =
    L"File: %s \n"
    L"Line: %d \n"
    L"\n"
    L"%s failed to get %s, with error code %x.";

} // namespace

std::filesystem::path
GetGameExecutableFilePath(
    HMODULE dll_handle
) {
  using GetFileNameFuncType = char*(*)(char[]);
  using GetFileNameSizeFuncType = std::size_t(*)(void);

  // Initialize the function pointers.
  static GetFileNameFuncType get_file_name_func =
      reinterpret_cast<GetFileNameFuncType>(
          GetProcAddress(
              dll_handle,
              "GetGameExecutableFilePath"
          )
      );

  if (get_file_name_func == nullptr) {
    std::wstring full_message = fmt::sprintf(
        kFunctionFailErrorMessage,
        __FILEW__,
        __LINE__,
        L"GetProcAddress",
        L"GetGameExecutableFilePath",
        GetLastError()
    );

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"GetProcAddress Failed",
        MB_OK | MB_ICONERROR
    );

    std::exit(0);
  }

  static GetFileNameSizeFuncType get_file_name_size_func =
      reinterpret_cast<GetFileNameSizeFuncType>(
          GetProcAddress(
              dll_handle,
              "GetGameExecutableFilePathSize"
          )
      );

  if (get_file_name_size_func == nullptr) {
    std::wstring full_message = fmt::sprintf(
        kFunctionFailErrorMessage,
        __FILEW__,
        __LINE__,
        L"GetProcAddress",
        L"GetGameExecutableFilePathSize",
        GetLastError()
    );

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"GetProcAddress Failed",
        MB_OK | MB_ICONERROR
    );

    std::exit(0);
  }

  // Call the functions.
  std::unique_ptr buffer = std::make_unique<char[]>(
      get_file_name_size_func()
  );

  get_file_name_func(buffer.get());

  return buffer.get();
}

std::string
GetGameName(
    HMODULE dll_handle
) {
  using GetGameNameFuncType = char*(*)(char*);
  using GetGameNameSizeFuncType = std::size_t(*)(void);

  // Initialize the function pointers.
  static GetGameNameFuncType get_name_func =
      reinterpret_cast<GetGameNameFuncType>(
          GetProcAddress(
              dll_handle,
              "GetGameName"
          )
      );

  if (get_name_func == nullptr) {
    return u8"Unidentified Game";
  }

  static GetGameNameSizeFuncType get_name_size_func =
      reinterpret_cast<GetGameNameSizeFuncType>(
          GetProcAddress(
              dll_handle,
              "GetGameNameSize"
          )
      );

  if (get_name_size_func == nullptr) {
    return u8"Unidentified Game";
  }

  // Call the functions.
  std::string game_name(
      get_name_size_func() - 1,
      '\0'
  );

  get_name_func(game_name.data());

  return game_name;
}

std::string
GetGameVersionText(
    HMODULE dll_handle
) {
  using GetTextFuncType = char*(*)(char*);
  using GetTextSizeFuncType = std::size_t(*)(void);

  // Initialize the function pointers.
  static GetTextFuncType get_text_func =
      reinterpret_cast<GetTextFuncType>(
          GetProcAddress(
              dll_handle,
              "GetGameVersionText"
          )
      );

  if (get_text_func == nullptr) {
    return u8"Unidentified Version";
  }

  static GetTextSizeFuncType get_text_size_func =
      reinterpret_cast<GetTextSizeFuncType>(
          GetProcAddress(
              dll_handle,
              "GetGameVersionTextSize"
          )
      );

  if (get_text_size_func == nullptr) {
    return u8"Unidentified Version";
  }

  // Call the functions.
  std::string game_version_text(
      get_text_size_func() - 1,
      '\0'
  );

  get_text_func(game_version_text.data());

  return game_version_text;
}

} // namespace sgexe
