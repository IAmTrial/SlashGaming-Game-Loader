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

#ifndef SGGL_ARGS_PARSER_H_
#define SGGL_ARGS_PARSER_H_

#include <stddef.h>
#include <windows.h>

#include <mdc/std/wchar.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ParsedArgs {
  const wchar_t* game_path;
  const wchar_t* game_args;

  const wchar_t** inject_library_paths;
  size_t inject_library_paths_capacity;
  size_t inject_library_paths_count;

  size_t num_instances;

  const wchar_t* knowledge_library_path;
};

#define PARSED_ARGS_UNINIT { 0 }

extern const struct ParsedArgs ParsedArgs_kUninit;

struct ParsedArgs* ParsedArgs_InitFromArgv(
    struct ParsedArgs* args,
    int argc,
    const wchar_t* const* argv,
    size_t num_libraries);
void ParsedArgs_Deinit(struct ParsedArgs* args);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* SGGL_ARGS_PARSER_H_ */
