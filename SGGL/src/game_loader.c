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

#include "game_loader.h"

#include <windows.h>

#include <mdc/error/exit_on_error.h>
#include <mdc/std/wchar.h>
#include <mdc/wchar_t/filew.h>

#include "args_parser.h"

static void InitCommandLine(
    wchar_t* cmd_line,
    const struct ParsedArgs* args) {
  /* Surround the game path in quotes to handle paths with whitespace. */
  wcscpy(cmd_line, L"\"");
  wcscat(cmd_line, args->game_path);
  wcscat(cmd_line, L"\"");

  if (args->game_args != NULL) {
    wcscat(cmd_line, L" ");
    wcscat(cmd_line, args->game_args);
  }
}

static void ExitOnCreateProcessError(
    const wchar_t* file_path,
    unsigned int line,
    const struct ParsedArgs* args,
    DWORD last_error) {
  switch (last_error) {
    case 0x2: {
      Mdc_Error_ExitOnGeneralError(
          L"Error",
          L"Game executable %ls could not be found.",
          file_path,
          line,
          args->game_path);
      break;
    }

    default: {
      Mdc_Error_ExitOnWindowsFunctionError(
          file_path,
          line,
          L"CreateProcessW",
          last_error);
      break;
    }
  }
}

static void StartGameWithParams(
    PROCESS_INFORMATION* processes_infos,
    const struct ParsedArgs* args,
    DWORD creation_flags,
    LPVOID environment,
    const wchar_t* current_directory_path) {
  size_t i;

  /* CreateProcessW's lpCommandLine char limit is 32,767. */
  wchar_t full_cmd_line[32767];
  STARTUPINFOW startup_info = { 0 };

  startup_info.cb = sizeof(startup_info);

  /* Create the desired processes. */
  for (i = 0; i < args->num_instances; ++i) {
    BOOL is_create_process_success;

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
        creation_flags,
        environment,
        current_directory_path,
        &startup_info,
        &processes_infos[i]);

    if (!is_create_process_success) {
      ExitOnCreateProcessError(
          __FILEW__,
          __LINE__,
          args,
          GetLastError());
      goto bad_return;
    }
  }

  return;

bad_return:
  return;
}

/**
 * External
 */

void GameLoader_StartGame(
    PROCESS_INFORMATION* processes_infos,
    const struct ParsedArgs* args) {
  size_t i;

  StartGameWithParams(
      processes_infos,
      args,
      0,
      NULL,
      NULL);

  /* Wait until the processes are started. */
  for (i = 0; i < args->num_instances; ++i) {
    HANDLE open_process_result;

    do {
      open_process_result = OpenProcess(
          PROCESS_QUERY_INFORMATION,
          FALSE,
          processes_infos[i].dwProcessId);

      Sleep(100);
    } while (open_process_result == NULL);

    CloseHandle(open_process_result);
  }
}

void GameLoader_StartGameSuspended(
    PROCESS_INFORMATION* processes_infos,
    const struct ParsedArgs* args) {
  StartGameWithParams(
      processes_infos,
      args,
      CREATE_SUSPENDED,
      NULL,
      NULL);
}
