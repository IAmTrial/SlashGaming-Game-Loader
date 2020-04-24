/**
 * SlashGaming Game Loader
 * Copyright (C) 2018-2020  Mir Drualga
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

#include <wchar.h>

#include "error_handling.h"
#include "wide_macro.h"

static const wchar_t* kCreateProcessErrorFormat =
    L"%ls could not be started, with error code %x.";

static void InitCommandLine(wchar_t* cmd_line, const struct Args* args) {
  /* Surround the game path in quotes to handle paths with whitespace. */
  wcscpy(cmd_line, L"\"");
  wcscat(cmd_line, args->game_path);
  wcscat(cmd_line, L"\"");

  if (args->game_args != NULL) {
    wcscat(cmd_line, L" ");
    wcscat(cmd_line, args->game_args);
  }
}

static void ShowMessageCreateProcessError(
    const struct Args* args,
    DWORD last_error
) {
  switch (last_error) {
    case 0x2: {
      ExitOnGeneralFailure(
          L"Game executable could not be found.",
          L"Could Not Start Game"
      );
      break;
    }

    default: {
      ExitOnWindowsFunctionFailureWithLastError(
          L"CreateProcess",
          last_error
      );
    }
  }

  exit(0);
}

void StartGame(
    PROCESS_INFORMATION* processes_infos,
    const struct Args* args
) {
  size_t i;

  wchar_t* full_cmd_line;
  size_t full_cmd_line_len;

  HANDLE open_process_result;

  BOOL is_create_process_success;

  STARTUPINFOW startup_info = { 0 };
  startup_info.cb = sizeof(startup_info);

  /* Add 2 due to surrounding quotes. */
  full_cmd_line_len = wcslen(args->game_path) + 2;

  if (args->game_args != NULL) {
    /* Add 1 due to space. */
    full_cmd_line_len += wcslen(args->game_args) + 1;
  }

  full_cmd_line = malloc((full_cmd_line_len + 1) * sizeof(full_cmd_line[0]));

  if (full_cmd_line == NULL) {
    ExitOnAllocationFailure();
  }

  /* Create the desired processes. */
  for (i = 0; i < args->num_instances; i += 1) {
    /*
    * CreateProcessW can modify the cmd line string, so a copy must be
    * made every time an instance needs to be made.
    */
    InitCommandLine(full_cmd_line, args);

    is_create_process_success = CreateProcessW(
        args->game_path,
        full_cmd_line,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &startup_info,
        &processes_infos[i]
    );

    if (!is_create_process_success) {
      ShowMessageCreateProcessError(args, GetLastError());
    }
  }

  /* Wait until the processes are started. */
  for (i = 0; i < args->num_instances; i += 1) {
    do {
      open_process_result = OpenProcess(
          PROCESS_QUERY_INFORMATION,
          FALSE,
          processes_infos[i].dwProcessId
      );

      Sleep(100);
    } while (open_process_result == NULL);

    CloseHandle(open_process_result);
  }

free_full_cmd_line:
  free(full_cmd_line);
}

void StartGameSuspended(
    PROCESS_INFORMATION* processes_infos,
    const struct Args* args
) {
  size_t i;

  wchar_t* full_cmd_line;
  size_t full_cmd_line_len;

  BOOL is_create_process_success;

  STARTUPINFOW startup_info = { 0 };
  startup_info.cb = sizeof(startup_info);

  /* Add 2 due to surrounding quotes. */
  full_cmd_line_len = wcslen(args->game_path) + 2;

  if (args->game_args != NULL) {
    /* Add 1 due to space. */
    full_cmd_line_len += wcslen(args->game_args) + 1;
  }

  full_cmd_line = malloc((full_cmd_line_len + 1) * sizeof(full_cmd_line[0]));

  if (full_cmd_line == NULL) {
    ExitOnAllocationFailure();
  }

  /* Create the desired processes. */
  for (i = 0; i < args->num_instances; i += 1) {
    /*
    * CreateProcessW can modify the cmd line string, so a copy must be
    * made every time an instance needs to be made.
    */
    InitCommandLine(full_cmd_line, args);

    is_create_process_success = CreateProcessW(
        args->game_path,
        full_cmd_line,
        NULL,
        NULL,
        TRUE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &startup_info,
        &processes_infos[i]
    );

    if (!is_create_process_success) {
      ShowMessageCreateProcessError(args, GetLastError());
    }
  }

free_full_cmd_line:
  free(full_cmd_line);
}
