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

#include "knowledge_library.h"

#include <stdio.h>

#include "error_handling.h"

static HMODULE knowledge_library;

typedef void (*InitFunctionType)(const wchar_t*, size_t);
static InitFunctionType init_func_ptr;

typedef void (*DeinitFunctionType)(const PROCESS_INFORMATION*, size_t);
static DeinitFunctionType deinit_func_ptr;

typedef void (*PrintGameInfoFunctionType)(void);
static PrintGameInfoFunctionType print_game_info_func_ptr;

typedef int (*InjectLibrariesToProcessesFunctionType)(
    const wchar_t**,
    size_t,
    const PROCESS_INFORMATION*,
    size_t
);
static InjectLibrariesToProcessesFunctionType
inject_libraries_to_processes_func_ptr;

void Knowledge_Init(
    const wchar_t* knowledge_library_path,
    const wchar_t* game_path,
    size_t game_path_len
) {
  DWORD last_error;

  if (knowledge_library_path == NULL) {
    return;
  }

  knowledge_library = LoadLibraryW(knowledge_library_path);

  if (knowledge_library == NULL) {
    last_error = GetLastError();

    if (last_error == 0x7E) {
      return;
    }

    ExitOnWindowsFunctionFailureWithLastError(
        L"LoadLibraryW",
        last_error
    );
  }

  /* Load all of the Knowledge functions. */
  init_func_ptr = (InitFunctionType) GetProcAddress(
      knowledge_library,
      "Knowledge_Init"
  );

  if (init_func_ptr == NULL) {
    printf("Unable to load Knowledge_Init. \n");
  }

  deinit_func_ptr = (DeinitFunctionType) GetProcAddress(
      knowledge_library,
      "Knowledge_Deinit"
  );

  if (deinit_func_ptr == NULL) {
    printf("Unable to load Knowledge_Deinit. \n");
  }

  print_game_info_func_ptr = (PrintGameInfoFunctionType) GetProcAddress(
      knowledge_library,
      "Knowledge_PrintGameInfo"
  );

  if (print_game_info_func_ptr == NULL) {
    printf("Unable to load Knowledge_PrintGameInfo. \n");
  }

  inject_libraries_to_processes_func_ptr =
      (InjectLibrariesToProcessesFunctionType) GetProcAddress(
          knowledge_library,
          "Knowledge_InjectLibrariesToProcesses"
      );

  if (inject_libraries_to_processes_func_ptr == NULL) {
    printf("Unable to load Knowledge_InjectLibrariesToProcesses. \n");
  }

  /* Call Knowledge's init function if it exists. */
  if (init_func_ptr != NULL) {
    init_func_ptr(game_path, game_path_len);
  }
}

void Knowledge_Deinit(
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances
) {
  BOOL free_library_result;

  /* Call Knowledge's deinit function if it exists. */
  if (deinit_func_ptr != NULL) {
    deinit_func_ptr(processes_infos, num_instances);
  }

  /* Set all of the function pointers to NULL. */
  init_func_ptr = NULL;
  deinit_func_ptr = NULL;
  print_game_info_func_ptr = NULL;
  inject_libraries_to_processes_func_ptr = NULL;

  if (knowledge_library == NULL) {
    return;
  }

  free_library_result = FreeLibrary(knowledge_library);

  if (!free_library_result) {
    ExitOnWindowsFunctionFailureWithLastError(
        L"FreeLibrary",
        GetLastError()
    );
  }
}

void Knowledge_PrintGameInfo(void) {
  if (print_game_info_func_ptr == NULL) {
    return;
  }

  print_game_info_func_ptr();
}

int Knowledge_InjectLibrariesToProcesses(
    const wchar_t** libraries_to_inject,
    size_t num_libraries,
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances
) {
  if (inject_libraries_to_processes_func_ptr == NULL) {
    return 0;
  }

  return inject_libraries_to_processes_func_ptr(
      libraries_to_inject,
      num_libraries,
      processes_infos,
      num_instances
  );
}
