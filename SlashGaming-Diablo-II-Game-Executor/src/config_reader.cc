/**
 * SlashGaming Diablo II Game Loader
 * Copyright (C) 2018  Mir Drualga
 *
 * This file is part of SlashGaming Diablo II Game Loader.
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
 *  permission to convey the resulting work.  This additional permission is
 *  also extended to any combination of expansions, mods, and remasters of
 *  the game.
 */

#include "config_reader.h"

#include <windows.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

namespace sgd2gexe {
namespace {

const std::filesystem::path kConfigPath = "SlashGaming-Config.json";

void
CreateDefaultConfig(
    const std::filesystem::path& config_file_path
) {
  std::ofstream file_stream(config_file_path);
  file_stream << "{}" << std::endl;
}

void
AddMissingEntries(
    const std::filesystem::path& config_path
) {
  nlohmann::json config;
  std::fstream config_stream;

  config_stream.open(config_path, std::ios::in);
  config_stream >> config;
  config_stream.close();

  if (auto& entry = config["!!!Metadata - Do not modify!!!"];
      !entry.is_object()) {
    entry = nlohmann::json::object();
  }

  if (auto& entry = config["!!!Metadata - Do not modify!!!"]["Major Version A"];
      !entry.is_number()) {
    entry = 0;
  }

  if (auto& entry = config["!!!Metadata - Do not modify!!!"]["Major Version B"];
      !entry.is_number()) {
    entry = 2;
  }

  if (auto& entry = config["!!!Metadata - Do not modify!!!"]["Minor Version A"];
      !entry.is_number()) {
    entry = 0;
  }

  if (auto& entry = config["!!!Metadata - Do not modify!!!"]["Minor Version B"];
      !entry.is_number()) {
    entry = 0;
  }

  if (auto& entry = config["SlashGaming Diablo II Loader"];
      !entry.is_object()) {
    entry = nlohmann::json::object();
  }

  if (auto& entry = config["SlashGaming Diablo II Loader"]["Inject DLLs"];
      !entry.is_array()) {
    entry = nlohmann::json::array();
  }

  config_stream.open(config_path, std::ios::out | std::ios::trunc);
  config_stream << std::setw(4) << config << std::endl;
  config_stream.close();
}

} // namespace

std::vector<std::filesystem::path>
GetLibraryPaths(
    void
) {
  if (!std::filesystem::exists(kConfigPath)) {
    CreateDefaultConfig(kConfigPath);
  }

  AddMissingEntries(kConfigPath);

  nlohmann::json config;
  if (std::ifstream config_stream(kConfigPath);
      config_stream
  ) {
    config_stream >> config;
  } else {
    return std::vector<std::filesystem::path>();
  }

  const auto& dll_list = config["SlashGaming Diablo II Loader"]["Inject DLLs"];

  std::vector<std::filesystem::path> library_paths;
  for (const auto& dll : dll_list) {
    if (dll.is_string()) {
      library_paths.push_back(dll.get<std::string>());
    }
  }

  return library_paths;
}

} // namespace sgd2gexe
