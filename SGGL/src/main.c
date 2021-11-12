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

#include "args_parser.h"
#include "args_validator.h"
#include "error_handling.h"
#include "game_loader.h"
#include "help_printer.h"
#include "knowledge_library.h"
#include "library_injector.h"
#include "license.h"

int wmain(int argc, const wchar_t** argv) {
  size_t i;

  size_t num_libraries;
  struct Args args;
  PROCESS_INFORMATION* processes_infos;
  int is_inject_libraries_success;
  BOOL is_close_handle_success;

  int is_knowledge_override_inject;

  memset(&args, 0, sizeof(args));

  /* Print the license notice. */
  License_PrintText();

  for (i = 0; i < 79; i += 1) {
    printf("-");
  }
  printf("\n");

  /* Validate args. */
  if (!ArgsValidator_IsValid(argc, argv, &num_libraries)) {
    Help_PrintText(argv[0]);
    printf("\nPress enter to exit... \n");
    getc(stdin);

    return 0;
  }

  /* Parse args. */
  Args_InitFromArgv(&args, argc, argv, num_libraries);

  /* Initialize Knowledge library, if specified. */
  if (args.knowledge_library_path != NULL) {
    wprintf(
        L"Loading Knowledge library from %ls \n",
        args.knowledge_library_path
    );
    Knowledge_Init(
        args.knowledge_library_path,
        args.game_path,
        args.game_path_len
    );
    printf("\n");
  }

  /* Print out the game info, handled by Knowledge. */
  Knowledge_PrintGameInfo();

  /* Print out parsed args to standard out. */
  printf("Now loading game from path... \n");
  wprintf(L"%ls \n\n", args.game_path);

  if (args.game_args != NULL) {
    printf("Command line arguments to pass into the game: \n");
    wprintf(L"%ls \n\n", args.game_args);
  }

  if (args.inject_library_paths_count > 0) {
    printf("Libraries to inject: \n");

    for (i = 0; i < args.inject_library_paths_count; i += 1) {
      wprintf(L"%ls \n", args.inject_library_paths[i]);
    }

    printf("\n");
  }

  printf("Number of instances to open: %d \n", args.num_instances);

  /* Create the new processes. */
  processes_infos = malloc(args.num_instances * sizeof(processes_infos[0]));

  if (processes_infos == NULL) {
    ExitOnAllocationFailure();
  }

  memset(processes_infos, 0, args.num_instances * sizeof(processes_infos[0]));
  StartGameSuspended(processes_infos, &args);

  printf("%u game instance(s) have been opened. \n\n", args.num_instances);

  /* Inject the library, after reading all files. */
  is_knowledge_override_inject = Knowledge_InjectLibrariesToProcesses(
      args.inject_library_paths,
      args.inject_library_paths_count,
      processes_infos,
      args.num_instances
  );

  if (!is_knowledge_override_inject) {
    is_inject_libraries_success = InjectLibrariesToProcesses(
        args.inject_library_paths,
        args.inject_library_paths_count,
        processes_infos,
        args.num_instances
    );
  } else {
    is_inject_libraries_success = is_knowledge_override_inject;
  }

  if (is_inject_libraries_success) {
    printf("All libraries have been successfully injected. \n\n");
  } else {
    printf("Some or all libraries failed to inject. \n\n");
  }

  /* Resume processes. */
  printf("Resuming processes... \n\n");

  for (i = 0; i < args.num_instances; i += 1) {
    ResumeThread(processes_infos[i].hThread);
  }

  /* Before closing handles, have Knowledge cleanup anything it needs to. */
knowledge_deinit:
  Knowledge_Deinit(processes_infos, args.num_instances);

  /* Close process and thread handles. */
  for (i = 0; i < args.num_instances; i += 1) {
    is_close_handle_success = CloseHandle(processes_infos[i].hProcess);

    if (!is_close_handle_success) {
      ExitOnWindowsFunctionFailureWithLastError(
          L"CloseHandle",
          GetLastError()
      );
    }

    is_close_handle_success = CloseHandle(processes_infos[i].hThread);

    if (!is_close_handle_success) {
      ExitOnWindowsFunctionFailureWithLastError(
          L"CloseHandle",
          GetLastError()
      );
    }
  }

free_args:
  Args_Deinit(&args);

  printf("Done. \n\n");

#ifdef NDEBUG
  Sleep(500);
#else
  getc(stdin);
#endif /* NDEBUG */

  return 0;
}
