/**
 * SlashDiablo Game Loader
 * Copyright (C) 2018 Mir Drualga
 *
 *  This file is part of SlashDiablo Game Loader.
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
 */

#include <windows.h>
#include <iostream>

#include "LibraryLoader.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow) {
    std::cout << "SlashDiablo Game Loader - Copyright (C) 2018 Mir Drualga" << std::endl;
    std::cout << "This program is free software, licensed under the" << std::endl;
    std::cout << "Affero General Public License, version 3 or greater." << std::endl;

    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    HMODULE gameHandle = LoadLibrary("Game.exe");
    loadLibraries();

    return 0;
}
