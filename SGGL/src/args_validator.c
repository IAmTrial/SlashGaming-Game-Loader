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

#include <mdc/std/assert.h>
#include <mdc/std/wchar.h>

struct ArgsValidationResults {
  int is_game_path_found;
  int is_game_args_found;
  int is_num_instances_found;
  int is_knowledge_library_path_found;
  size_t num_libraries;
};

#define ARGS_VALIDATION_RESULTS_UNINIT { 0 }

/**
 * Validation function
 */

typedef int ArgValidationFunc(
    struct ArgsValidationResults* results,
    int* i_arg,
    int argc,
    const wchar_t* const* argv);

static int IsGamePathValid(
    struct ArgsValidationResults* results,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  size_t path_length;

  if (results->is_game_path_found) {
    return 0;
  }

  if (*i_arg >= argc - 1) {
    return 0;
  }

  path_length = wcslen(argv[*i_arg + 1]);
  if (path_length <= 0) {
    return 0;
  }

  results->is_game_path_found = 1;
  ++(*i_arg);

  return 1;
}

static int IsGameArgValid(
    struct ArgsValidationResults* results,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  size_t args_length;

  if (results->is_game_args_found) {
    return 0;
  }

  if (*i_arg >= argc - 1) {
    return 0;
  }

  args_length = wcslen(argv[*i_arg + 1]);
  if (args_length <= 0) {
    return 0;
  }

  results->is_game_args_found = 1;
  ++(*i_arg);

  return 1;
}

static int IsInjectLibraryPathValid(
    struct ArgsValidationResults* results,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  size_t path_length;

  if (*i_arg >= argc - 1) {
    return 0;
  }

  path_length = wcslen(argv[*i_arg + 1]);
  if (path_length <= 0) {
    return 0;
  }

  results->num_libraries += 1;
  ++(*i_arg);

  return 1;
}

static int IsKnowledgeLibraryPathValid(
    struct ArgsValidationResults* results,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  size_t path_length;

  if (results->is_knowledge_library_path_found) {
    return 0;
  }

  if (*i_arg >= argc - 1) {
    return 0;
  }

  path_length = wcslen(argv[*i_arg + 1]);
  if (path_length <= 0) {
    return 0;
  }

  results->is_knowledge_library_path_found = 1;
  ++(*i_arg);

  return 1;
}

static int IsNumInstancesValid(
    struct ArgsValidationResults* results,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  size_t i_str;

  if (results->is_num_instances_found) {
    return 0;
  }

  if (*i_arg >= argc - 1) {
    return 0;
  }

  for (i_str = 0; argv[*i_arg + 1][i_str] != L'\0'; ++i_str) {
    if (!iswdigit(argv[*i_arg + 1][i_str])) {
      return 0;
    }
  }

  results->is_num_instances_found = 1;
  ++(*i_arg);

  return 1;
}

/**
 * Validation table
 */

struct ArgsValidationFuncTableEntry {
  const wchar_t* key;
  ArgValidationFunc* value;
};

static int ArgsValidationFuncTableEntry_CompareKey(
    const struct ArgsValidationFuncTableEntry* entry1,
    const struct ArgsValidationFuncTableEntry* entry2) {
  return wcscmp(entry1->key, entry2->key);
}

static int ArgsValidationFuncTableEntry_CompareKeyAsVoid(
    const void* entry1,
    const void* entry2) {
  return ArgsValidationFuncTableEntry_CompareKey(entry1, entry2);
}

static const struct ArgsValidationFuncTableEntry
kArgsValidationFuncSortedTable[] = {
    { L"--game", &IsGamePathValid },
    { L"--gameargs", &IsGameArgValid },
    { L"--knowledge", &IsKnowledgeLibraryPathValid },
    { L"--library", &IsInjectLibraryPathValid },
    { L"--num-instances", &IsNumInstancesValid },

    { L"-a", &IsGameArgValid },
    { L"-g", &IsGamePathValid },
    { L"-k", &IsKnowledgeLibraryPathValid },
    { L"-l", &IsInjectLibraryPathValid },
    { L"-n", &IsNumInstancesValid },
};

enum {
  kArgsValidationFuncSortedTableCount = sizeof(kArgsValidationFuncSortedTable)
      / sizeof(kArgsValidationFuncSortedTable[0])
};

static int IsArgValid(
    struct ArgsValidationResults* results,
    int* i_arg,
    int argc,
    const wchar_t* const* argv) {
  const struct ArgsValidationFuncTableEntry* search_result;

  search_result = bsearch(
      &argv[*i_arg],
      kArgsValidationFuncSortedTable,
      kArgsValidationFuncSortedTableCount,
      sizeof(kArgsValidationFuncSortedTable[0]),
      &ArgsValidationFuncTableEntry_CompareKeyAsVoid);

  if (search_result == NULL) {
    return 0;
  }

  assert(search_result->value != NULL);

  return search_result->value(results, i_arg, argc, argv);
}

/**
 * External
 */

int ArgsValidator_IsValid(
    int argc,
    const wchar_t* const* argv,
    size_t* num_libraries) {
  int i_arg;
  struct ArgsValidationResults results =
      ARGS_VALIDATION_RESULTS_UNINIT;

  if (argc < 3) {
    return 0;
  }

  if (num_libraries == NULL) {
    num_libraries = &results.num_libraries;
  }

  *num_libraries = 0;

  for (i_arg = 1; i_arg < argc; ++i_arg) {
    if (!IsArgValid(&results, &i_arg, argc, argv)) {
      return 0;
    }
  }

  *num_libraries = results.num_libraries;

  return results.is_game_path_found;
}
