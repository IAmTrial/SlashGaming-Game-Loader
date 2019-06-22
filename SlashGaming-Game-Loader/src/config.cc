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
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <mjsoni/rapid_json_config_reader.hpp>

namespace sgexe::config {
namespace {

// Global config entries.
constexpr std::string_view kGlobalEntryKey = u8"!!!Globals!!!";

constexpr std::string_view kConfigTabWidthKey = u8"Config Tab Width";
constexpr int kDefaultConfigTabWidth = 4;

// Main config entries.
constexpr std::string_view kMainEntryKey = u8"SlashGaming Game Loader";
constexpr std::string_view kMetaDataKey = u8"!!!Metadata (Do not modify)!!!";

// Note that this signifies the last version where the config formatting and
// entries were updated. These values do not need to change with respect to API
// file version!
constexpr std::string_view kMajorVersionAKey = u8"Major Version A";
constexpr int kMajorVersionA = 1;
constexpr std::string_view kMajorVersionBKey = u8"Major Version B";
constexpr int kMajorVersionB = 0;
constexpr std::string_view kMinorVersionAKey = u8"Minor Version A";
constexpr int kMinorVersionA = 0;
constexpr std::string_view kMinorVersionBKey = u8"Minor Version B";
constexpr int kMinorVersionB = 0;

// Version detector library variable.
constexpr std::string_view kVersionDetectorLibraryKey = u8"Version Detector DLL";
constexpr std::string_view kDefaultVersionDetectorLibrary =
    u8"VersionDetector.dll";

// Injected libraries variables.
constexpr std::string_view kInjectDllsKey = u8"Inject Dlls";
const std::vector<std::filesystem::path> kDefaultInjectDlls;

std::map<std::string, std::once_flag> once_flags_by_json_keys;

const std::filesystem::path& GetConfigPath() {
  static std::filesystem::path kConfigPath = u8"SlashGaming-Config.json";
  return kConfigPath;
}

template <typename ...Args>
std::once_flag& GetOnceFlag(const Args&... json_keys) {
  // Calculate concat key size.
  std::size_t concat_key_size = 0;
  for (const auto& json_key : { json_keys...}) {
    concat_key_size += json_key.length() + 1;
  }

  // Concat all keys together.
  std::string concat_key(concat_key_size, '\0');

  std::size_t i = 0;
  for (const auto& json_key : { json_keys...}) {
    concat_key += json_key;
    concat_key += static_cast<char>(i + 1);
    
    i = (i + 1) % 0x32;
  }

  return once_flags_by_json_keys[std::move(concat_key)];
}

bool AddMissingConfigEntries(
    mjsoni::RapidJsonConfigReader& config_reader
) {
  // Check that the actual config version is less than or equal to the expected
  // config version. If the actual is larger, then do not add any new entries.
  // If there are any breaking config changes, then the program will most
  // likely crash.
  int major_version_a = config_reader.GetIntOrDefault(
      std::numeric_limits<int>::min(),
      kMainEntryKey,
      kMetaDataKey,
      kMajorVersionAKey
  );

  if (major_version_a < kMajorVersionA) {
    major_version_a = kMajorVersionA;

    config_reader.SetDeepInt(
        kMajorVersionA,
        kMainEntryKey,
        kMetaDataKey,
        kMajorVersionAKey
    );

    config_reader.SetDeepInt(
        std::numeric_limits<int>::min(),
        kMainEntryKey,
        kMetaDataKey,
        kMajorVersionBKey
    );

    config_reader.SetDeepInt(
        std::numeric_limits<int>::min(),
        kMainEntryKey,
        kMetaDataKey,
        kMinorVersionAKey
    );

    config_reader.SetDeepInt(
        std::numeric_limits<int>::min(),
        kMainEntryKey,
        kMetaDataKey,
        kMinorVersionBKey
    );
  } else if (major_version_a > kMajorVersionA) {
    return true;
  }

  int major_version_b = config_reader.GetIntOrDefault(
      std::numeric_limits<int>::min(),
      kMetaDataKey,
      kMajorVersionBKey
  );

  if (major_version_b < kMajorVersionB) {
    major_version_b = kMajorVersionB;

    config_reader.SetDeepInt(
        kMajorVersionB,
        kMainEntryKey,
        kMetaDataKey,
        kMajorVersionBKey
    );

    config_reader.SetDeepInt(
        std::numeric_limits<int>::min(),
        kMainEntryKey,
        kMetaDataKey,
        kMinorVersionAKey
    );

    config_reader.SetDeepInt(
        std::numeric_limits<int>::min(),
        kMainEntryKey,
        kMetaDataKey,
        kMinorVersionBKey
    );
  } else if (major_version_b > kMajorVersionB) {
    return true;
  }

  int minor_version_a = config_reader.GetIntOrDefault(
      std::numeric_limits<int>::min(),
      kMainEntryKey,
      kMetaDataKey,
      kMinorVersionAKey
  );

  if (minor_version_a < kMinorVersionA) {
    minor_version_a = kMinorVersionA;

    config_reader.SetDeepInt(
        kMinorVersionA,
        kMainEntryKey,
        kMetaDataKey,
        kMinorVersionAKey
    );

    config_reader.SetDeepInt(
        std::numeric_limits<int>::min(),
        kMainEntryKey,
        kMetaDataKey,
        kMinorVersionBKey
    );
  } else if (minor_version_a > kMinorVersionA) {
    return true;
  }

  int minor_version_b = config_reader.GetIntOrDefault(
      std::numeric_limits<int>::min(),
      kMainEntryKey,
      kMetaDataKey,
      kMinorVersionBKey
  );

  if (minor_version_b < kMinorVersionB) {
    minor_version_b = kMinorVersionB;

    config_reader.SetDeepInt(
        kMinorVersionB,
        kMainEntryKey,
        kMetaDataKey,
        kMinorVersionBKey
    );
  } else if (minor_version_b > kMinorVersionB) {
    return true;
  }

  // The user's config version is less or equal, so add defaults if missing.
  if (!config_reader.HasInt(kGlobalEntryKey, kConfigTabWidthKey)) {
    config_reader.SetDeepInt(
        kDefaultConfigTabWidth,
        kGlobalEntryKey,
        kConfigTabWidthKey
    );
  }

  if (!config_reader.HasPath(kMainEntryKey, kVersionDetectorLibraryKey)) {
    config_reader.SetDeepPath(
        kDefaultVersionDetectorLibrary,
        kMainEntryKey,
        kVersionDetectorLibraryKey
    );
  }

  if (!config_reader.HasVector(kMainEntryKey, kInjectDllsKey)) {
    config_reader.SetDeepVector(
        kDefaultInjectDlls,
        kMainEntryKey,
        kInjectDllsKey
    );
  }

  int indent_width = config_reader.GetInt(
      kGlobalEntryKey,
      kConfigTabWidthKey
  );

  return config_reader.Write(indent_width);
}

mjsoni::RapidJsonConfigReader ReadConfig(
    const std::filesystem::path& config_file_path
) {
  // Read the config file for processing, if all other conditions are good.
  mjsoni::RapidJsonConfigReader config_reader(config_file_path);

  bool is_read = config_reader.Read();
  if (!is_read) {
    std::wstring failed_to_read_message = fmt::format(
        L"Failed to read config in: {}",
        config_file_path.wstring().data()
    );

    MessageBoxW(
        nullptr,
        failed_to_read_message.data(),
        L"Failed to Read Config",
        MB_OK | MB_ICONERROR
    );

    std::exit(0);
  }

  bool is_missing_entry_added = AddMissingConfigEntries(config_reader);
  if (!is_missing_entry_added) {
    std::wstring failed_to_read_message = fmt::format(
        L"Failed add missing entries to the config in: {}",
        config_file_path.wstring().data()
    );

    MessageBoxW(
        nullptr,
        failed_to_read_message.data(),
        L"Failed to Add Missing Entries to Config",
        MB_OK | MB_ICONERROR
    );

    std::exit(0);
  }

  return config_reader;
}

mjsoni::RapidJsonConfigReader& GetConfigReader() {
  static mjsoni::RapidJsonConfigReader config_reader =
      ReadConfig(GetConfigPath());
  return config_reader;
}

} // namespace

const std::filesystem::path&
GetVersionDetectorLibraryPath() {
  static std::filesystem::path version_detector_library_path;

  std::call_once(
      GetOnceFlag(kMainEntryKey, kVersionDetectorLibraryKey),
      [=] () {
        version_detector_library_path = GetConfigReader().GetPath(
            kMainEntryKey,
            kVersionDetectorLibraryKey
        );
      }
  );

  return version_detector_library_path;
}

const std::vector<std::filesystem::path>&
GetInjectDllsPaths() {
  static std::vector<std::filesystem::path> inject_dlls_paths;

  std::call_once(
      GetOnceFlag(kMainEntryKey, kInjectDllsKey),
      [=] () {
        inject_dlls_paths = GetConfigReader()
            .GetVector<std::filesystem::path>(
                kMainEntryKey,
                kInjectDllsKey
            );
      }
  );

  return inject_dlls_paths;
}

} // namespace sgexe
