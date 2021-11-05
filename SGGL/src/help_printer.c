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

#include "help_printer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

enum CONSTANTS {
  ARG_OPTION_LEN = 36,
  DESCRIPTION_LEN = 72 - ARG_OPTION_LEN - 2
};

const char* kFormatString = "  %-34s %s \n";
const char* kFormatContinueString = "%36c %s \n";

static void PrintArgHelp(
    const char* arg_option,
    const char* description
) {
  assert(strlen(arg_option) < ARG_OPTION_LEN);
  assert(strlen(description) < DESCRIPTION_LEN);

  printf(kFormatString, arg_option, description);
}

static void PrintContinuedLine(
    const char* description
) {
  assert(strlen(description) < DESCRIPTION_LEN);

  printf(kFormatContinueString, ' ', description);
}

void PrintHelp(const wchar_t* current_program) {
  wprintf(L"Usage: %ls [options] \n", current_program);
  printf("Options: \n");

  PrintArgHelp("-g, --game <program>", "Game to execute");

  PrintArgHelp(
      "-a, --gameargs <args>",
      "Command line arguments to pass to"
  );
  PrintContinuedLine("the game");

  PrintArgHelp(
      "-k, --knowledge <library>",
      "Path of Knowledge extension"
  );
  PrintContinuedLine("library");

  PrintArgHelp(
      "-l, --library <library>",
      "Path of library to inject (this"
  );
  PrintContinuedLine("option can be repeated for");
  PrintContinuedLine("multiple libraries)");

  PrintArgHelp(
      "-n, --num-instances <count>",
      "Number of instances to open"
  );
}
