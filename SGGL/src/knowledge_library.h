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

#ifndef SGGL_KNOWLEDGE_LIBRARY_H_
#define SGGL_KNOWLEDGE_LIBRARY_H_

#include <stddef.h>
#include <wchar.h>
#include <windows.h>

void Knowledge_Init(
    const wchar_t* knowledge_library_path,
    const wchar_t* game_path,
    size_t game_path_len
);

void Knowledge_Deinit(
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances
);

void Knowledge_PrintGameInfo(void);

int Knowledge_InjectLibrariesToProcesses(
    const wchar_t** libraries_to_inject,
    size_t num_libraries,
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances
);

#endif /* SGGL_KNOWLEDGE_LIBRARY_H_ */
