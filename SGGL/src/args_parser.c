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

#include <stddef.h>
#include <stdlib.h>
#include <wctype.h>
#include <windows.h>

#include <mdc/error/exit_on_error.h>
#include <mdc/malloc/malloc.h>
#include <mdc/std/assert.h>
#include <mdc/std/wchar.h>
#include <mdc/wchar_t/filew.h>

#include "game_loader.h"

/**
 * Validation function
 */

static void ParseGamePath(
    struct ParsedArgs* args,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  /* Point to the game path of the game executable. */
  args->game_path = argv[*i_arg + 1];

  ++(*i_arg);
}

static void ParseGameArg(
    struct ParsedArgs* args,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  /* Point to the game args */
  args->game_args = argv[*i_arg + 1];

  ++(*i_arg);
}

static void ParseInjectLibraryPath(
    struct ParsedArgs* args,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  /* Manage all points to libraries that will be injected. */
  if (args->inject_library_paths_capacity
      <= args->inject_library_paths_count) {
    Mdc_Error_ExitOnGeneralError(
        L"Error",
        L"Library count changed during execution. Please run the program "
            L"again.",
        __FILEW__,
        __LINE__);
    goto bad_return;
  }

  args->inject_library_paths[args->inject_library_paths_count] =
      argv[*i_arg + 1];

  args->inject_library_paths_count += 1;

  ++(*i_arg);

  return;

bad_return:
  return;
}

static void ParseKnowledgeLibraryPath(
    struct ParsedArgs* args,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  /* Point to the Knowledge library path */
  args->knowledge_library_path = argv[*i_arg + 1];

  ++(*i_arg);
}

static void ParseNumInstances(
    struct ParsedArgs* args,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  /* Determine number of instances to open. */
  args->num_instances = wcstoul(argv[*i_arg + 1], NULL, 10);

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
  args->num_instances = (args->num_instances <= GameLoader_kMaxInstances)
      ? args->num_instances
      : GameLoader_kMaxInstances;

  ++(*i_arg);
}

/**
 * Parse table
 */

typedef void ArgParseFunc(
    struct ParsedArgs* args,
    int* i_arg,
    int argc,
    const wchar_t* const* argv);

struct ArgParseFuncTableEntry {
  const wchar_t* key;
  ArgParseFunc* value;
};

static int ArgParseFuncTableEntry_CompareKey(
    const struct ArgParseFuncTableEntry* entry1,
    const struct ArgParseFuncTableEntry* entry2) {
  return wcscmp(entry1->key, entry2->key);
}

static int ArgParseFuncTableEntry_CompareKeyAsVoid(
    const void* entry1,
    const void* entry2) {
  return ArgParseFuncTableEntry_CompareKey(entry1, entry2);
}

static const struct ArgParseFuncTableEntry kArgParseFuncSortedTable[] = {
    { L"--game", &ParseGamePath },
    { L"--gameargs", &ParseGameArg },
    { L"--knowledge", &ParseKnowledgeLibraryPath },
    { L"--library", &ParseInjectLibraryPath },
    { L"--num-instances", &ParseNumInstances },

    { L"-a", &ParseGameArg },
    { L"-g", &ParseGamePath },
    { L"-k", &ParseKnowledgeLibraryPath },
    { L"-l", &ParseInjectLibraryPath },
    { L"-n", &ParseNumInstances },
};

enum {
  kArgParseFuncSortedTableCount = sizeof(kArgParseFuncSortedTable)
      / sizeof(kArgParseFuncSortedTable[0]),
};

static void ParseArg(
    struct ParsedArgs* args,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  const struct ArgParseFuncTableEntry* search_result;

  search_result = bsearch(
      &argv[*i_arg],
      kArgParseFuncSortedTable,
      kArgParseFuncSortedTableCount,
      sizeof(kArgParseFuncSortedTable[0]),
      &ArgParseFuncTableEntry_CompareKeyAsVoid);

  if (search_result == NULL) {
    return;
  }

  assert(search_result->value != NULL);

  search_result->value(args, i_arg, argc, argv);
}

/**
 * External
 */

const struct ParsedArgs ParsedArgs_kUninit = PARSED_ARGS_UNINIT;

struct ParsedArgs* ParsedArgs_InitFromArgv(
    struct ParsedArgs* args,
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

  args->inject_library_paths_capacity = num_libraries;
  args->num_instances = 1;

  for (i_arg = 1; i_arg < argc; ++i_arg) {
    ParseArg(args, &i_arg, argc, argv);
  }

  return args;

bad_return:
  return NULL;
}

void ParsedArgs_Deinit(struct ParsedArgs* args) {
  args->game_path = NULL;
  args->game_args = NULL;

  Mdc_free(args->inject_library_paths);
  args->inject_library_paths = NULL;
  args->inject_library_paths_capacity = 0;
  args->inject_library_paths_count = 0;

  args->num_instances = 0;

  args->knowledge_library_path = NULL;

  *args = ParsedArgs_kUninit;
}
