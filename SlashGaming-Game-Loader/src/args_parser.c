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

#include "args_parser.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include <windows.h>

static void AddLibrary(struct Args* args, const wchar_t* arg) {
  /* Resize if needed. */
  if (args->num_libraries == args->libraries_capacity) {
    args->libraries_capacity *= 2;
    args->libraries_to_inject = realloc(
        args->libraries_to_inject,
        args->libraries_capacity * sizeof(args->libraries_to_inject[0])
    );
  }

  /* Add the library to the list of libraries. */
  args->libraries_to_inject[args->num_libraries] = arg;
  args->num_libraries += 1;
}

int ValidateArgs(int argc, const wchar_t* const* argv) {
  int i;
  int is_game_path_found = 0;
  int is_game_args_found = 0;

  if (argc < 3) {
    return 0;
  }

  for (i = 1; i < argc; i += 1) {
    if (wcscmp(argv[i], L"--game") == 0
        || wcscmp(argv[i], L"-g") == 0) {
      if (i >= argc - 1) {
        return 0;
      }

      if (is_game_path_found) {
        return 0;
      }

      is_game_path_found = 1;
      i += 1;
    } else if (wcscmp(argv[i], L"--gameargs") == 0
        || wcscmp(argv[i], L"-a") == 0) {
      if (i >= argc - 1) {
        return 0;
      }

      if (is_game_args_found) {
        return 0;
      }

      is_game_args_found = 1;
      i += 1;
    } else if (wcscmp(argv[i], L"--library") == 0
        || wcscmp(argv[i], L"-l") == 0) {
      if (i >= argc - 1) {
        return 0;
      }
      i += 1;
    }
  }

  return is_game_path_found;
}

void ParseArgs(struct Args* args, int argc, const wchar_t* const* argv) {
  int i;

  assert(argc >= 3);

  args->libraries_capacity = 4;
  args->num_libraries = 0;
  args->libraries_to_inject = malloc(
      args->libraries_capacity * sizeof(args->libraries_to_inject[0])
  );

  for (i = 1; i < argc; i += 1) {
    if (wcscmp(argv[i], L"--game") == 0
        || wcscmp(argv[i], L"-g") == 0) {
      /* Point to the game path of the game executable. */
      args->game_path = argv[i + 1];
      i += 1;
    } else if (wcscmp(argv[i], L"--gameargs") == 0
        || wcscmp(argv[i], L"-a") == 0) {
      /* Point to the game args */
      args->game_args = argv[i + 1];
      i += 1;
    } else if (wcscmp(argv[i], L"--library") == 0
        || wcscmp(argv[i], L"-l") == 0) {
      /* Manage all points to libraries that will be injected. */
      AddLibrary(args, argv[i + 1]);

      i += 1;
    }
  }
}

void DestructArgs(struct Args* args) {
  free(args->libraries_to_inject);
  args->num_libraries = 0;
  args->libraries_capacity = 0;
}
