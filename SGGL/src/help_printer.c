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

#include "help_printer.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <shlwapi.h>

#include <mdc/std/assert.h>
#include <mdc/std/wchar.h>

enum {
  kArgOptionLength = 36,
  kDescriptionLength = 72 - kArgOptionLength - 2
};

#define FORMAT_STRING L"  %-34s %s\n"
#define FORMAT_CONTINUE_STRING L"%36c %s\n"

static void PrintArgHelp(
    const wchar_t* arg_option,
    const wchar_t* description) {
  assert(wcslen(arg_option) < kArgOptionLength);
  assert(wcslen(description) < kDescriptionLength);

  wprintf(FORMAT_STRING, arg_option, description);
}

static void PrintContinuedLine(const wchar_t* description) {
  assert(wcslen(description) < kDescriptionLength);

  wprintf(FORMAT_CONTINUE_STRING, L' ', description);
}

/**
 * External
 */

void Help_PrintText(const wchar_t* program_path) {
  wprintf(L"Usage: %ls [options]\n", PathFindFileNameW(program_path));
  wprintf(L"Options:\n");

  PrintArgHelp(L"-g, --game <program>", L"Game to execute");

  PrintArgHelp(
      L"-a, --gameargs <args>",
      L"Command line arguments to pass to");
  PrintContinuedLine(L"the game");

  PrintArgHelp(
      L"-k, --knowledge <library>",
      L"Path of Knowledge extension");
  PrintContinuedLine(L"library");

  PrintArgHelp(
      L"-l, --library <library>",
      L"Path of library to inject (this");
  PrintContinuedLine(L"option can be repeated for");
  PrintContinuedLine(L"multiple libraries)");

  PrintArgHelp(
      L"-n, --num-instances <count>",
      L"Number of instances to open");
}
