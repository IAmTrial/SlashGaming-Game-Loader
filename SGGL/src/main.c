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

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <mdc/error/exit_on_error.h>
#include <mdc/std/wchar.h>
#include <mdc/wchar_t/filew.h>

#include "args_parser.h"
#include "args_validator.h"
#include "game_loader.h"
#include "help_printer.h"
#include "knowledge_library.h"
#include "library_injector.h"
#include "license.h"

int wmain(int argc, const wchar_t** argv) {
  size_t i;

  size_t num_libraries;
  struct ParsedArgs args = PARSED_ARGS_UNINIT;
  struct ParsedArgs* init_args_result;
  PROCESS_INFORMATION processes_infos[GameLoader_kMaxInstances];
  int is_inject_libraries_success;
  int is_knowledge_override_inject;

  /* Print the license notice. */
  License_PrintText();

  for (i = 0; i < 72; ++i) {
    wprintf(L"-");
  }
  wprintf(L"\n");

  /* Validate args. */
  if (!ArgsValidator_IsValid(argc, argv, &num_libraries)) {
    Help_PrintText(argv[0]);
    wprintf(L"\nPress enter to exit...\n");
    getc(stdin);

    return 0;
  }

  /* Parse args. */
  init_args_result = ParsedArgs_InitFromArgv(&args, argc, argv, num_libraries);
  if (init_args_result == NULL) {
    goto bad_return;
  }

  /* Initialize Knowledge library, if specified. */
  if (args.knowledge_library_path != NULL) {
    wprintf(
        L"Loading Knowledge library from %ls\n",
        args.knowledge_library_path);
    Knowledge_Init(args.knowledge_library_path, args.game_path);
    wprintf(L"\n");
  }

  /* Print out the game info, handled by Knowledge. */
  Knowledge_PrintGameInfo();

  /* Print out parsed args to standard out. */
  wprintf(L"Now loading game from path...\n");
  wprintf(L"%ls\n\n", args.game_path);

  if (args.game_args != NULL) {
    wprintf(L"Command line arguments to pass into the game:\n");
    wprintf(L"%ls\n\n", args.game_args);
  }

  if (args.inject_library_paths_count > 0) {
    wprintf(L"Libraries to inject:\n");

    for (i = 0; i < args.inject_library_paths_count; ++i) {
      wprintf(L"%ls\n", args.inject_library_paths[i]);
    }

    wprintf(L"\n");
  }

  wprintf(L"Number of instances to open: %d\n", args.num_instances);

  /* Create the new processes. */
  GameLoader_StartGameSuspended(processes_infos, &args);

  wprintf(L"%u game instance(s) have been opened.\n\n", args.num_instances);

  /* Inject the library, after reading all files. */
  is_knowledge_override_inject = Knowledge_InjectLibrariesToProcesses(
      args.inject_library_paths,
      args.inject_library_paths_count,
      processes_infos,
      args.num_instances);

  if (!is_knowledge_override_inject) {
    is_inject_libraries_success = LibraryInjector_InjectToProcesses(
        args.inject_library_paths,
        args.inject_library_paths_count,
        processes_infos,
        args.num_instances);
  } else {
    is_inject_libraries_success = is_knowledge_override_inject;
  }

  if (is_inject_libraries_success) {
    wprintf(L"All libraries have been successfully injected.\n\n");
  } else {
    wprintf(L"Some or all libraries failed to inject.\n\n");
  }

  /* Resume processes. */
  wprintf(L"Resuming processes...\n\n");

  for (i = 0; i < args.num_instances; ++i) {
    ResumeThread(processes_infos[i].hThread);
  }

  /* Before closing handles, have Knowledge cleanup anything it needs to. */
  Knowledge_Deinit(processes_infos, args.num_instances);

  /* Close process and thread handles. */
  for (i = 0; i < args.num_instances; ++i) {
    BOOL is_close_handle_success;

    is_close_handle_success = CloseHandle(processes_infos[i].hProcess);
    if (!is_close_handle_success) {
      Mdc_Error_ExitOnWindowsFunctionError(
          __FILEW__,
          __LINE__,
          L"CloseHandle",
          GetLastError());
      goto bad_deinit_args;
    }

    is_close_handle_success = CloseHandle(processes_infos[i].hThread);
    if (!is_close_handle_success) {
      Mdc_Error_ExitOnWindowsFunctionError(
          __FILEW__,
          __LINE__,
          L"CloseHandle",
          GetLastError());
      goto bad_deinit_args;
    }
  }

  ParsedArgs_Deinit(&args);

  wprintf(L"Done. \n\n");

#ifdef NDEBUG
  Sleep(500);
#else
  getc(stdin);
#endif /* NDEBUG */

  return 0;

bad_deinit_args:
  ParsedArgs_Deinit(&args);

bad_return:
  return 1;
}
