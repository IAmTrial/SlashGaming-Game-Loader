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

#include "args_parser.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <windows.h>

#include <mdc/error/exit_on_error.h>
#include <mdc/malloc/malloc.h>
#include <mdc/wchar_t/filew.h>

/**
 * External
 */

int Args_IsValid(
    int argc,
    const wchar_t* const* argv,
    size_t* num_libraries) {
  int i_arg;
  size_t i_str;
  size_t arg_value_len;
  size_t temp_num_libraries;
  int is_game_path_found = 0;
  int is_game_args_found = 0;
  int is_num_instances_found = 0;
  int is_knowledge_library_path_found = 0;

  if (argc < 3) {
    return 0;
  }

  if (num_libraries == NULL) {
    num_libraries = &temp_num_libraries;
  }

  *num_libraries = 0;

  for (i_arg = 1; i_arg < argc; ++i_arg) {
    if (wcscmp(argv[i_arg], L"--game") == 0
        || wcscmp(argv[i_arg], L"-g") == 0) {
      if (i_arg >= argc - 1) {
        return 0;
      }

      arg_value_len = wcslen(argv[i_arg + 1]);

      if (arg_value_len <= 0) {
        return 0;
      }

      if (is_game_path_found) {
        return 0;
      }

      is_game_path_found = 1;
      ++i_arg;
    } else if (wcscmp(argv[i_arg], L"--gameargs") == 0
        || wcscmp(argv[i_arg], L"-a") == 0) {
      if (i_arg >= argc - 1) {
        return 0;
      }

      arg_value_len = wcslen(argv[i_arg + 1]);

      if (arg_value_len <= 0) {
        return 0;
      }

      if (is_game_args_found) {
        return 0;
      }

      is_game_args_found = 1;
      ++i_arg;
    } else if (wcscmp(argv[i_arg], L"--library") == 0
        || wcscmp(argv[i_arg], L"-l") == 0) {
      if (i_arg >= argc - 1) {
        return 0;
      }

      arg_value_len = wcslen(argv[i_arg + 1]);

      if (arg_value_len <= 0) {
        return 0;
      }

      ++(*num_libraries);

      ++i_arg;
    } else if (wcscmp(argv[i_arg], L"--num-instances") == 0
        || wcscmp(argv[i_arg], L"-n") == 0) {
      if (i_arg >= argc - 1) {
        return 0;
      }

      if (is_num_instances_found) {
        return 0;
      }

      for (i_str = 0; argv[i_arg + 1][i_str] != L'\0'; ++i_str) {
        if (!iswdigit(argv[i_arg + 1][i_str])) {
          return 0;
        }
      }

      is_num_instances_found = 1;
    } else if (wcscmp(argv[i_arg], L"--knowledge") == 0
        || wcscmp(argv[i_arg], L"-k") == 0) {
      if (i_arg >= argc - 1) {
        return 0;
      }

      arg_value_len = wcslen(argv[i_arg + 1]);

      if (arg_value_len <= 0) {
        return 0;
      }

      if (is_knowledge_library_path_found) {
        return 0;
      }

      is_knowledge_library_path_found = 1;
      ++i_arg;
    }
  }

  return is_game_path_found;
}

void Args_InitFromArgv(
    struct Args* args,
    int argc,
    const wchar_t* const* argv,
    size_t num_libraries) {
  int i_arg;

  assert(argc >= 3);

  args->inject_library_paths_count = 0;
  args->inject_library_paths = Mdc_malloc(
      num_libraries * sizeof(args->inject_library_paths[0]));
  if (args->inject_library_paths == NULL) {
    Mdc_Error_ExitOnMemoryAllocError(__FILEW__, __LINE__);
    goto bad_return;
  }

  args->num_instances = 1;

  for (i_arg = 1; i_arg < argc; ++i_arg) {
    if (wcscmp(argv[i_arg], L"--game") == 0
        || wcscmp(argv[i_arg], L"-g") == 0) {
      /* Point to the game path of the game executable. */
      args->game_path = argv[i_arg + 1];
      args->game_path_len = wcslen(args->game_path);

      ++i_arg;
    } else if (wcscmp(argv[i_arg], L"--gameargs") == 0
        || wcscmp(argv[i_arg], L"-a") == 0) {
      /* Point to the game args */
      args->game_args = argv[i_arg + 1];
      args->game_args_len = wcslen(args->game_args);

      ++i_arg;
    } else if (wcscmp(argv[i_arg], L"--library") == 0
        || wcscmp(argv[i_arg], L"-l") == 0) {
      /* Manage all points to libraries that will be injected. */
      const wchar_t** inject_library_path;

      inject_library_path = &args->inject_library_paths[
          args->inject_library_paths_count];
      *inject_library_path = argv[i_arg + 1];

      args->inject_library_paths_count += 1;

      ++i_arg;
    } else if (wcscmp(argv[i_arg], L"--num-instances") == 0
        || wcscmp(argv[i_arg], L"-n") == 0) {
      /* Determine number of instances to open. */
      args->num_instances = wcstoul(argv[i_arg + 1], NULL, 10);

      /*
      * Check that the number of instances to open is at least 1.
      */
      args->num_instances = (args->num_instances >= 1)
          ? args->num_instances
          : 1;

      /*
      * Prevent opening more than 8 instances at once. Doing so
      * prevents the user from accidental resource hogging.
      */
      args->num_instances = (args->num_instances <= 8)
          ? args->num_instances
          : 8;

      ++i_arg;
    } else if (wcscmp(argv[i_arg], L"--knowledge") == 0
        || wcscmp(argv[i_arg], L"-k") == 0) {
      /* Point to the Knowledge library path */
      args->knowledge_library_path = argv[i_arg + 1];

      ++i_arg;
    }
  }

  return;

bad_return:
  return;
}

void Args_Deinit(struct Args* args) {
  args->game_path = NULL;
  args->game_args = NULL;

  Mdc_free(args->inject_library_paths);
  args->inject_library_paths_count = 0;

  args->num_instances = 0;

  args->knowledge_library_path = NULL;
}
