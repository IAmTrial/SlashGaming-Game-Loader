/**
 * SlashDiablo Game Loader
 * Copyright (C) 2018  Mir Drualga
 *
 * This file is part of SlashDiablo Game Loader.
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
 *  it with Diablo II (or a modified version of that game and its
 *  libraries), containing parts covered by the terms of Blizzard End User
 *  License Agreement, the licensors of this Program grant you additional
 *  permission to convey the resulting work.
 *
 *  If you modify this Program, or any covered work, by linking or combining
 *  it with Diablo II: Lord of Destruction (or a modified version of that
 *  game and its libraries), containing parts covered by the terms of
 *  Blizzard End User License Agreement, the licensors of this Program grant
 *  you additional permission to convey the resulting work.
 */

#pragma once

#ifndef LIBRARYINJECTOR_H
#define LIBRARYINJECTOR_H

#include <windows.h>

#include <string>
#include <string_view>

class LibraryInjector {
public:
    LibraryInjector(std::wstring_view,
        const PROCESS_INFORMATION& pProcessInformation);
    LibraryInjector(LibraryInjector&& libraryInjector) = default;
    ~LibraryInjector();

    bool injectLibrary();

    static bool injectLibraries(
        const PROCESS_INFORMATION& pProcessInformation);
private:
    std::wstring libraryPath;
    size_t libraryPathSize;
    const PROCESS_INFORMATION& pProcessInformation;
    wchar_t *pRemoteWChar;
};

bool loadLibraries();
HMODULE loadLibrarySafely(std::wstring_view libraryPath);

#endif // LIBRARYINJECTOR_H
