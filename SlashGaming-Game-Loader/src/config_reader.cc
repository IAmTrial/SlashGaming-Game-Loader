/**
 * SlashGaming Game Loader
 * Copyright (C) 2018-2019  Mir Drualga
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

#include "config_reader.h"

#include <windows.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>

namespace sgexe::config {
namespace {

const std::filesystem::path kConfigPath = u8"SlashGaming-Config.json";

// Global config entries.
constexpr std::string_view kGlobalEntryKey = u8"!!!Globals!!!";

constexpr std::string_view kConfigTabWidth = u8"Config Tab Width";
constexpr int kDefaultConfigTabWidthValue = 4;

// Main config entries.
constexpr std::string_view kMainEntryKey = u8"SlashGaming Game Loader";
constexpr std::string_view kMetaDataKey = u8"!!!Metadata (Do not modify)!!!";

// Note that this signifies the last version where the config formatting and
// entries were updated. These values do not need to change with respect to API
// file version!
constexpr std::string_view kMajorVersionAKey = u8"Major Version A";
constexpr int kMajorVersionAValue = 1;
constexpr std::string_view kMajorVersionBKey = u8"Major Version B";
constexpr int kMajorVersionBValue = 0;
constexpr std::string_view kMinorVersionAKey = u8"Minor Version A";
constexpr int kMinorVersionAValue = 0;
constexpr std::string_view kMinorVersionBKey = u8"Minor Version B";
constexpr int kMinorVersionBValue = 0;

// Version detector library variable.
constexpr std::string_view kVersionDetectorLibraryKey = u8"Version Detector DLL";
constexpr std::string_view kDefaultVersionDetectorLibraryValue =
    u8"VersionDetector.dll";

// Injected libraries variables.
constexpr std::string_view kInjectDllsKey = u8"Inject Dlls";

void
CreateDefaultConfig(
    const std::filesystem::path& config_file_path
) {
  std::ofstream file_stream(config_file_path);
  file_stream << u8"{}" << std::endl;
}

bool
AddMissingEntries(
    const std::filesystem::path& config_path
) {
  rapidjson::Document config;

  if (std::ifstream config_stream(config_path);
      config_stream) {
    rapidjson::IStreamWrapper config_reader(config_stream);
    config.ParseStream(config_reader);
  } else {
    return false;
  }

  auto& global_entry = config[kGlobalEntryKey.data()];
  if (!global_entry.IsObject()) {
    global_entry.SetObject();
  }

  auto& main_entry = config[kMainEntryKey.data()];
  if (!main_entry.IsObject()) {
    main_entry.SetObject();
  }

  auto& metadata_entry = main_entry[kMetaDataKey.data()];
  if (!metadata_entry.IsObject()) {
    metadata_entry.SetObject();
  }

  // Check that the actual config version is less than or equal to the expected
  // config version. If the actual is larger, then do not add any new entries.
  // If there are any breaking config changes, then the program will most
  // likely crash.
  auto& major_version_a = metadata_entry[kMajorVersionAKey.data()];
  auto& major_version_b = metadata_entry[kMajorVersionBKey.data()];
  auto& minor_version_a = metadata_entry[kMinorVersionAKey.data()];
  auto& minor_version_b = metadata_entry[kMinorVersionBKey.data()];

  if (!major_version_a.IsInt() || major_version_a.GetInt() < kMajorVersionAValue) {
    major_version_a.SetInt(kMajorVersionAValue);
    major_version_b.SetInt(0);
    minor_version_a.SetInt(0);
    minor_version_b.SetInt(0);
  } else if (major_version_a.GetInt() > kMajorVersionAValue) {
    return true;
  }

  if (!major_version_b.IsInt() || major_version_b.GetInt() < kMajorVersionBValue) {
    major_version_b.SetInt(kMajorVersionBValue);
    minor_version_a.SetInt(0);
    minor_version_b.SetInt(0);
  } else if (major_version_b.GetInt() > kMajorVersionBValue) {
    return true;
  }

  if (!minor_version_a.IsInt() || minor_version_a.GetInt() < kMinorVersionAValue) {
    minor_version_a.SetInt(kMinorVersionAValue);
    minor_version_b.SetInt(0);
  } else if (minor_version_a.GetInt() > kMinorVersionAValue) {
    return true;
  }

  if (!minor_version_b.IsInt() || minor_version_b.GetInt() < kMinorVersionBValue) {
    minor_version_b.SetInt(kMinorVersionBValue);
  } else if (minor_version_b.GetInt() > kMinorVersionBValue) {
    return true;
  }

  // The user's config is less or equal, so add defaults if missing.

  if (auto& entry = global_entry[kConfigTabWidth.data()];
      !entry.IsInt()) {
    entry.SetInt(kDefaultConfigTabWidthValue);
  }

  if (auto& entry = main_entry[kVersionDetectorLibraryKey.data()];
      !entry.IsString()) {
    entry.SetString(rapidjson::StringRef(kDefaultVersionDetectorLibraryValue.data()));
  }

  if (auto& entry = main_entry[kInjectDllsKey.data()];
      !entry.IsArray()) {
    entry.SetArray();
  }

  if (std::ofstream config_stream(config_path);
      config_stream) {
    rapidjson::OStreamWrapper config_writer(config_stream);
    rapidjson::PrettyWriter pretty_config_writer(config_writer);
    pretty_config_writer.SetIndent('\t', global_entry[kConfigTabWidth.data()].GetInt());

    config.Accept(pretty_config_writer);
  } else {
    return false;
  }

  return true;
}

rapidjson::Document
ReadConfig(
    void
) {
  if (!std::filesystem::exists(kConfigPath)) {
    CreateDefaultConfig(kConfigPath);
  }

  bool is_missing_entry_added = AddMissingEntries(kConfigPath);
  if (!is_missing_entry_added) {
    return {};
  }

  rapidjson::Document config;
  if (std::ifstream config_stream(kConfigPath);
      config_stream
  ) {
    rapidjson::IStreamWrapper config_reader(config_stream);
    config.ParseStream(config_reader);
  }

  return config;
}

rapidjson::Document&
GetConfig(
    void
) {
  static rapidjson::Document config = ReadConfig();
  return config;
}

} // namespace

std::filesystem::path
GetVersionDetectorLibraryPath(
    void
) {
  rapidjson::Document& config = GetConfig();

  auto& version_detector_path_entry =
      config[kMainEntryKey.data()][kVersionDetectorLibraryKey.data()];
  if (!version_detector_path_entry.IsString()) {
    version_detector_path_entry.SetString(rapidjson::StringRef(kDefaultVersionDetectorLibraryValue.data()));
  }

  std::filesystem::path version_detector_path(
      version_detector_path_entry.GetString()
  );

  return version_detector_path;
}

std::vector<std::filesystem::path>
GetInjectDllsPaths(
    void
) {
  rapidjson::Document& config = GetConfig();

  auto& inject_dlls_paths_entry =
      config[kMainEntryKey.data()][kInjectDllsKey.data()];
  if (!inject_dlls_paths_entry.IsArray()) {
    inject_dlls_paths_entry.SetArray();
  }

  auto inject_dlls_paths_texts_entry = inject_dlls_paths_entry.GetArray();

  std::vector<std::filesystem::path> inject_dlls_paths;

  for (const auto& raw_dll_path_entry : inject_dlls_paths_texts_entry) {
    inject_dlls_paths.push_back(raw_dll_path_entry.GetString());
  }

  return inject_dlls_paths;
}

} // namespace sgexe
