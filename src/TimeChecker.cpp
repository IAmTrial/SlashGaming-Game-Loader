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

#include "TimeChecker.h"

#include <windows.h>
#include <chrono>
#include <iostream>
#include <regex>
#include <unordered_map>

std::chrono::system_clock TimeChecker::convertStringDate(
        std::string_view dateString) {
    static const std::unordered_map<std::string, int> monthValues = {
        { "Jan", 0 }, { "Feb", 1 }, { "Mar", 2 }, { "Apr", 3 }, { "May", 4 },
        { "Jun", 5 }, { "Jul", 6 }, { "Aug", 7 }, { "Sep", 8 }, { "Oct", 9 },
        { "Nov", 10 }, { "Dec", 11 }
    };

    std::cout << dateString.data() <<std::endl;
    const std::regex COMPILE_DATE_REGEX("(\\s+) (\\d+) (\\d+)");
    std::cmatch matches;

    struct tm timeInfo;
    if (!std::regex_match(dateString.data(), matches, COMPILE_DATE_REGEX)) {
        return std::chrono::system_clock();
    }

    std::cout << matches[0] << std::endl;
    std::cout << matches[1] << std::endl;
    std::cout << matches[2] << std::endl;
    std::cout << matches[3] << std::endl;
}

void TimeChecker::enforceTimeStamp() {
    if constexpr (!ENFORCE_TIMESTAMP) {
        return;
    }

    std::cout << "Timestamp is enforced, meaning that this program " <<
        "will cease to function on " << COMPILATION_DATE << "." << std::endl;
    std::cout << "This means that you have received a version of the " <<
        "program not meant for public release." << std::endl;

    if (!isExecutionPermitted()) {
        MessageBoxW(nullptr, L"Date of execution exceeds timestamp limit.",
            L"Execution Date Exceeded", MB_OK | MB_ICONERROR);
        std::exit(0);
    }
}

bool TimeChecker::isExecutionPermitted() {
    convertStringDate(COMPILATION_DATE);
    return true;
}
