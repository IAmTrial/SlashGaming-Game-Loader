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

#include "config.h"

#include <windows.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/printf.h>
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

  // Get the global entry.
  if (!config.HasMember(kGlobalEntryKey.data())
      || !config[kGlobalEntryKey.data()].IsObject()) {
    config.AddMember(
        rapidjson::StringRef(kGlobalEntryKey.data()),
        rapidjson::Value(rapidjson::kObjectType),
        config.GetAllocator()
    );
  }

  auto& global_entry = config[kGlobalEntryKey.data()];

  // Get the main entry.
  if (!config.HasMember(kMainEntryKey.data())
      || !config[kMainEntryKey.data()].IsObject()) {
    config.AddMember(
        rapidjson::StringRef(kMainEntryKey.data()),
        rapidjson::Value(rapidjson::kObjectType),
        config.GetAllocator()
    );
  }

  auto& main_entry = config[kMainEntryKey.data()];

  // Get the metadata entry.
  if (!main_entry.HasMember(kMetaDataKey.data())
      || !main_entry[kMetaDataKey.data()].IsObject()) {
    main_entry.AddMember(
        rapidjson::StringRef(kMetaDataKey.data()),
        rapidjson::Value(rapidjson::Value(rapidjson::kObjectType)),
        config.GetAllocator()
    );
  }

  auto& metadata_entry = main_entry[kMetaDataKey.data()];

  // Check that the actual config version is less than or equal to the expected
  // config version. If the actual is larger, then do not add any new entries.
  // If there are any breaking config changes, then the program will most
  // likely crash.
  if (!metadata_entry.HasMember(kMajorVersionAKey.data())
      || !metadata_entry[kMajorVersionAKey.data()].IsInt()
      || metadata_entry[kMajorVersionAKey.data()].GetInt() < kMajorVersionAValue) {
    metadata_entry.AddMember(
        rapidjson::StringRef(kMajorVersionAKey.data()),
        rapidjson::Value(kMajorVersionAValue),
        config.GetAllocator()
    );
    metadata_entry.AddMember(
        rapidjson::StringRef(kMajorVersionBKey.data()),
        rapidjson::Value(0),
        config.GetAllocator()
    );
    metadata_entry.AddMember(
        rapidjson::StringRef(kMinorVersionAKey.data()),
        rapidjson::Value(0),
        config.GetAllocator()
    );
    metadata_entry.AddMember(
        rapidjson::StringRef(kMinorVersionBKey.data()),
        rapidjson::Value(0),
        config.GetAllocator()
    );
  } else if (metadata_entry[kMajorVersionAKey.data()].GetInt() > kMajorVersionAValue) {
    return true;
  }

  if (!metadata_entry.HasMember(kMajorVersionBKey.data())
      || !metadata_entry[kMajorVersionBKey.data()].IsInt()
      || metadata_entry[kMajorVersionBKey.data()].GetInt() < kMajorVersionBValue) {
    metadata_entry.AddMember(
        rapidjson::StringRef(kMajorVersionBKey.data()),
        rapidjson::Value(kMajorVersionBValue),
        config.GetAllocator()
    );
    metadata_entry.AddMember(
        rapidjson::StringRef(kMinorVersionAKey.data()),
        rapidjson::Value(0),
        config.GetAllocator()
    );
    metadata_entry.AddMember(
        rapidjson::StringRef(kMinorVersionBKey.data()),
        rapidjson::Value(0),
        config.GetAllocator()
    );
  } else if (metadata_entry[kMajorVersionBKey.data()].GetInt() > kMajorVersionBValue) {
    return true;
  }

  if (!metadata_entry.HasMember(kMinorVersionAKey.data())
      || !metadata_entry[kMinorVersionAKey.data()].IsInt()
      || metadata_entry[kMinorVersionAKey.data()].GetInt() < kMinorVersionAValue) {
    metadata_entry.AddMember(
        rapidjson::StringRef(kMinorVersionAKey.data()),
        rapidjson::Value(kMinorVersionAValue),
        config.GetAllocator()
    );
    metadata_entry.AddMember(
        rapidjson::StringRef(kMinorVersionBKey.data()),
        rapidjson::Value(0),
        config.GetAllocator()
    );
  } else if (metadata_entry[kMinorVersionAKey.data()].GetInt() > kMinorVersionAValue) {
    return true;
  }

  if (!metadata_entry.HasMember(kMinorVersionBKey.data())
      || !metadata_entry[kMinorVersionBKey.data()].IsInt()
      || metadata_entry[kMinorVersionBKey.data()].GetInt() < kMinorVersionBValue) {
    metadata_entry.AddMember(
        rapidjson::StringRef(kMinorVersionBKey.data()),
        rapidjson::Value(kMinorVersionBValue),
        config.GetAllocator()
    );
  } else if (metadata_entry[kMinorVersionBKey.data()].GetInt() > kMinorVersionBValue) {
    return true;
  }

  // The user's config version is less or equal, so add defaults if missing.
  if (!global_entry.HasMember(kConfigTabWidth.data())
      || !global_entry[kConfigTabWidth.data()].IsInt()
      || global_entry[kConfigTabWidth.data()].GetInt() < 0) {
    global_entry.AddMember(
        rapidjson::StringRef(kConfigTabWidth.data()),
        rapidjson::Value(kDefaultConfigTabWidthValue),
        config.GetAllocator()
    );
  }

  if (!main_entry.HasMember(kVersionDetectorLibraryKey.data())
      || !main_entry[kVersionDetectorLibraryKey.data()].IsString()) {
    main_entry.AddMember(
        rapidjson::StringRef(kVersionDetectorLibraryKey.data()),
        rapidjson::Value(rapidjson::StringRef(kDefaultVersionDetectorLibraryValue.data())),
        config.GetAllocator()
    );
  }

  if (!main_entry.HasMember(kInjectDllsKey.data())
      || !main_entry[kInjectDllsKey.data()].IsArray()) {
    main_entry.AddMember(
        rapidjson::StringRef(kInjectDllsKey.data()),
        rapidjson::Value(rapidjson::kArrayType),
        config.GetAllocator()
    );
  }

  if (std::ofstream config_stream(config_path);
      config_stream) {
    rapidjson::OStreamWrapper config_writer(config_stream);
    rapidjson::PrettyWriter pretty_config_writer(config_writer);
    pretty_config_writer.SetIndent(' ', global_entry[kConfigTabWidth.data()].GetInt());

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
