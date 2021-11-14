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

#include "knowledge_library.h"

#include <stddef.h>
#include <stdio.h>
#include <windows.h>

#include <mdc/error/exit_on_error.h>
#include <mdc/std/wchar.h>
#include <mdc/wchar_t/filew.h>

static HMODULE knowledge_library;

typedef void InitFuncType(const wchar_t* game_path);
static InitFuncType* init_func_ptr;

typedef void DeinitFuncType(
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances);
static DeinitFuncType* deinit_func_ptr;

typedef void PrintGameInfoFuncType(void);
static PrintGameInfoFuncType* print_game_info_func_ptr;

typedef int InjectLibrariesToProcessesFuncType(
    const wchar_t** libraries_to_inject,
    size_t num_libraries,
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances);
static InjectLibrariesToProcessesFuncType*
inject_libraries_to_processes_func_ptr;

/**
 * External
 */

void Knowledge_Init(
    const wchar_t* knowledge_library_path,
    const wchar_t* game_path) {
  if (knowledge_library_path == NULL) {
    return;
  }

  knowledge_library = LoadLibraryW(knowledge_library_path);
  if (knowledge_library == NULL) {
    DWORD last_error;

    last_error = GetLastError();
    if (last_error == ERROR_MOD_NOT_FOUND) {
      return;
    }

    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"LoadLibraryW",
        last_error);
    goto bad_return;
  }

  /* Load all of the Knowledge functions. */
  init_func_ptr = (InitFuncType*)GetProcAddress(
      knowledge_library,
      "Knowledge_Init");

  if (init_func_ptr == NULL) {
    wprintf(L"Unable to load Knowledge_Init.\n");
  }

  deinit_func_ptr = (DeinitFuncType*)GetProcAddress(
      knowledge_library,
      "Knowledge_Deinit");

  if (deinit_func_ptr == NULL) {
    wprintf(L"Unable to load Knowledge_Deinit.\n");
  }

  print_game_info_func_ptr = (PrintGameInfoFuncType*)GetProcAddress(
      knowledge_library,
      "Knowledge_PrintGameInfo");

  if (print_game_info_func_ptr == NULL) {
    wprintf(L"Unable to load Knowledge_PrintGameInfo.\n");
  }

  inject_libraries_to_processes_func_ptr =
      (InjectLibrariesToProcessesFuncType*)GetProcAddress(
          knowledge_library,
          "Knowledge_InjectLibrariesToProcesses");

  if (inject_libraries_to_processes_func_ptr == NULL) {
    wprintf(L"Unable to load Knowledge_InjectLibrariesToProcesses.\n");
  }

  /* Call Knowledge's init function if it exists. */
  if (init_func_ptr != NULL) {
    init_func_ptr(game_path);
  }

  return;

bad_return:
  return;
}

void Knowledge_Deinit(
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances) {
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
    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"FreeLibrary",
        GetLastError());
    goto bad_return;
  }

  return;

bad_return:
  return;
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
    size_t num_instances) {
  if (inject_libraries_to_processes_func_ptr == NULL) {
    return 0;
  }

  return inject_libraries_to_processes_func_ptr(
      libraries_to_inject,
      num_libraries,
      processes_infos,
      num_instances);
}
