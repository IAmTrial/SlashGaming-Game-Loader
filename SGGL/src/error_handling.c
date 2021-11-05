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

#include "error_handling.h"

#include <stdlib.h>
#include <windows.h>
#include <wchar.h>

/**
 * Error string lengths, including null-terminator.
 */
enum {
  ERROR_MESSAGE_LENGTH = 1024,
  ERROR_CAPTION_LENGTH = 256
};

static const wchar_t* kFunctionFailErrorFormat =
    L"The function %ls failed with error code %X.";

static const wchar_t* kGeneralFailErrorFormat =
    L"%ls";

void ExitOnGeneralFailure(
    const wchar_t* message,
    const wchar_t* caption
) {
#ifndef NDEBUG
  wchar_t full_message[ERROR_MESSAGE_LENGTH];

  swprintf(
      full_message,
      sizeof(full_message) / sizeof(full_message[0]),
      kGeneralFailErrorFormat,
      message
  );

  MessageBoxW(
      NULL,
      full_message,
      caption,
      MB_OK | MB_ICONERROR
  );
#endif /* NDEBUG */

  exit(EXIT_FAILURE);
}

void ExitOnAllocationFailure(void) {
  ExitOnGeneralFailure(
      L"Allocation function failed.",
      L"Memory Allocation Failed"
  );
}

void ExitOnWindowsFunctionFailureWithLastError(
    const wchar_t* function_name,
    DWORD last_error
) {
#ifndef NDEBUG
  wchar_t full_message[ERROR_MESSAGE_LENGTH];
  wchar_t message_box_caption[ERROR_CAPTION_LENGTH];

  swprintf(
      full_message,
      sizeof(full_message) / sizeof(full_message[0]),
      kFunctionFailErrorFormat,
      function_name,
      last_error
  );

  swprintf(
      message_box_caption,
      sizeof(message_box_caption) / sizeof(message_box_caption[0]),
      L"%ls Failed",
      function_name
  );

  MessageBoxW(
      NULL,
      full_message,
      message_box_caption,
      MB_OK | MB_ICONERROR
  );
#endif /* NDEBUG */

  exit(EXIT_FAILURE);
}
