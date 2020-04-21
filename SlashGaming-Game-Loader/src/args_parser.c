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

int ValidateArgs(int argc, wchar_t** argv) {
  return argc >= 2;
}

void ParseArgs(struct Args* args, int argc, wchar_t** argv) {
  int i;
  size_t total_args_len;

  assert(argc >= 2);

  /* Copy the game path of the game executable. */
  wcscpy(args->game_path, argv[1]);

  if (argc == 2) {
    args->cmd_args[0] = '\0';

    return;
  }

  /* Determine the length of the args. */
  total_args_len = 0;

  for (i = 1; i < argc; i += 1) {
    total_args_len += wcslen(argv[i]);
  }

  total_args_len += argc - 2;

  /* Copy the args */
  args->cmd_args[0] = L'\0';

  for (i = 1; i < argc; i += 1) {
    wcscat(args->cmd_args, argv[i]);
    wcscat(args->cmd_args, L" ");
  }

  args->cmd_args[total_args_len] = L'\0';
}
