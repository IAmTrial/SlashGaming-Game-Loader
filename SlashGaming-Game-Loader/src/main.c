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

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "args_parser.h"
#include "error_handling.h"
#include "game_loader.h"
#include "help_printer.h"
#include "library_injector.h"
#include "license.h"

int wmain(int argc, const wchar_t** argv) {
  size_t i;
  struct Args args;
  PROCESS_INFORMATION* processes_infos;
  int is_inject_libraries_success;
  BOOL is_close_handle_success;

  memset(&args, 0, sizeof(args));

  /* Print the license notice. */
  PrintLicenseNotice();

  for (i = 0; i < 79; i += 1) {
    printf("-");
  }
  printf("\n");

  /* Validate args. */
  if (!ValidateArgs(argc, argv)) {
    PrintHelp(argv[0]);

    return 0;
  }

  /* Parse args. */
  ParseArgs(&args, argc, argv);

  /* Print out parsed args to standard out. */
  printf("Now loading game from path... \n");
  wprintf(L"%ls \n\n", args.game_path);

  if (args.game_args != NULL) {
    printf("Command line arguments to pass into the game: \n");
    wprintf(L"%ls \n\n", args.game_args);
  }

  if (args.num_libraries > 0) {
    printf("Libraries to inject: \n");

    wprintf(L"%ls", args.libraries_to_inject[0]);

    for (i = 1; i < args.num_libraries; i += 1) {
      wprintf(L", %ls", args.libraries_to_inject[i]);
    }

    printf("\n\n");
  }

  printf("Number of instances to open: %d \n\n", args.num_instances);

  /* Create the new processes. */
  processes_infos = malloc(args.num_instances * sizeof(processes_infos[0]));
  memset(processes_infos, 0, args.num_instances * sizeof(processes_infos[0]));
  StartGameSuspended(processes_infos, &args);

  /* Inject the library, after reading all files. */
  is_inject_libraries_success = InjectLibrariesToProcesses(
      args.libraries_to_inject,
      args.num_libraries,
      processes_infos,
      args.num_instances
  );

  if (is_inject_libraries_success) {
    printf("All libraries have been successfully injected. \n");
  } else {
    printf("Some or all libraries failed to inject. \n");
  }

  /* Resume process. */
  for (i = 0; i < args.num_instances; i += 1) {
    ResumeThread(processes_infos[i].hThread);

    /* Close process and thread handles. */
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
  DestructArgs(&args);

#ifdef NDEBUG
  Sleep(500);
#else
  getc(stdin);
#endif /* NDEBUG */

  return 0;
}
