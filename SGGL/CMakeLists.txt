# SlashGaming Game Loader
# Copyright (C) 2018-2021  Mir Drualga
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Additional permissions under GNU Affero General Public License version 3
# section 7
#
# If you modify this Program, or any covered work, by linking or combining
# it with any program (or a modified version of that program and its
# libraries), containing parts covered by the terms of an incompatible
# license, the licensors of this Program grant you additional permission
# to convey the resulting work.

cmake_minimum_required(VERSION 3.10)

# Name of the project, also is the name of the file

project(SGGL)

# Define requirements for C

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED true)

# Enable NASM

set(CMAKE_ASM_NASM_OBJECT_FORMAT win32)
enable_language(ASM_NASM)

# Remove MinGW compiled binary "lib" prefix

if (MINGW)
    set(CMAKE_IMPORT_LIBRARY_PREFIX "")
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
endif (MINGW)

# List all of the source files here

set(RESOURCE_FILES
    "resource/resource.rc"
    "resource/slashgaming_game_loader.ico"
)

set(SOURCE_FILES
    ${RESOURCE_FILES}

    "src/library_injector_shim.asm"

    "src/args_parser.c"
    "src/args_validator.c"
    "src/game_loader.c"
    "src/help_printer.c"
    "src/knowledge_library.c"
    "src/library_injector.c"
    "src/license.c"
    "src/main.c"

    "src/args_parser.h"
    "src/args_validator.h"
    "src/game_loader.h"
    "src/help_printer.h"
    "src/knowledge_library.h"
    "src/library_injector.h"
    "src/license.h"
)

# Output EXE

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    libMDCc
    shlwapi
)
add_dependencies(${PROJECT_NAME} libMDCc)
