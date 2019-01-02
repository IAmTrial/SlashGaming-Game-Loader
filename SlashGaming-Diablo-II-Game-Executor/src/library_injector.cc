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

#include "library_injector.h"

#include <windows.h>
#include <cwchar>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

#include <boost/scope_exit.hpp>
#include "asm_x86_macro.h"

namespace sgd2gexe {
namespace {

__declspec(naked) bool
FailVirtualFreeExStub(
    BOOL is_free_success
) {
  ASM_X86(xor eax, eax)
  ASM_X86(pushad)
  ASM_X86(mov ebp, esp)
  ASM_X86(push ebx)
  ASM_X86(dec esp)
  ASM_X86(inc ecx)
  ASM_X86(push ebx)
  ASM_X86(dec eax)
  ASM_X86(inc edi)
  ASM_X86(inc ecx)
  ASM_X86(dec ebp)
  ASM_X86(dec ecx)
  ASM_X86(dec esi)
  ASM_X86(inc edi)
  ASM_X86(mov esp, ebp)
  ASM_X86(popad)
  ASM_X86(cmp [esp + 0x4], 0)
  ASM_X86(je _loadLibrarySafelyStubEND)
  ASM_X86(inc eax)

_loadLibrarySafelyStubEND:
  ASM_X86(ret)
}

__declspec(naked) bool
FailWriteProcessMemoryStub(
    BOOL is_write_success
) {
  ASM_X86(sub esp, 4)
  ASM_X86(lea eax, [esp])
  ASM_X86(pushad)
  ASM_X86(push eax)
  ASM_X86(mov ebp, esp)
  ASM_X86(sub esp, 0x200 - 0x1)
  ASM_X86(lea eax, [esp - 0x1])
  ASM_X86(mov ecx, eax)
  ASM_X86(mov esi, eax)
  ASM_X86(mov ebx, eax)
  ASM_X86(dec esp)
  ASM_X86(imul esp, [ebx + 0x65], 0x6465736e)
  ASM_X86(mov esp, eax)
  ASM_X86(and [ecx + 0x47], al)
  ASM_X86(push eax)
  ASM_X86(dec esp)
  ASM_X86(and [esi + 0x33], dh)
  ASM_X86(sub esp, [eax])
  ASM_X86(mov esp, ebp)
  ASM_X86(pop eax)
  ASM_X86(mov eax, esp)
  ASM_X86(popad)
  ASM_X86(add esp, 4)
  ASM_X86(cmp [esp + 0x4], 0)
  ASM_X86(je _failWriteProcessMemoryStubEND)
  ASM_X86(inc eax)
_failWriteProcessMemoryStubEND:
  ASM_X86(ret)
}

bool
InjectLibrary(
    const std::filesystem::path& library_path,
    const PROCESS_INFORMATION *process_info_ptr
) {
  // Encode the path to one understood by Windows.
  std::wstring library_path_string = library_path.wstring();
  std::size_t buffer_size = library_path_string.length() + 1;

  // Store the library path into the target process.
  LPVOID remote_buf = VirtualAllocEx(
      process_info_ptr->hProcess,
      nullptr,
      buffer_size * sizeof(wchar_t),
      MEM_COMMIT,
      PAGE_READWRITE
  );

  if (remote_buf == nullptr) {
    std::cerr << "VirtualAllocEx failed." << std::endl;
    std::exit(0);
  }

  // Free used resources at the end of the function scope.
  BOOST_SCOPE_EXIT(&process_info_ptr, &remote_buf) {
    BOOL is_free_success = VirtualFreeEx(
        process_info_ptr->hProcess,
        remote_buf,
        0,
        MEM_RELEASE
    );

    if (!is_free_success) {
      std::cerr << "VirtualFreeEx failed." << std::endl;
      FailVirtualFreeExStub(is_free_success);
      std::exit(0);
    }
  } BOOST_SCOPE_EXIT_END

  // Write the library name into the remote program.
  BOOL is_write_success = WriteProcessMemory(
      process_info_ptr->hProcess,
      remote_buf,
      library_path_string.data(),
      buffer_size * sizeof(wchar_t),
      nullptr
  );

  if (!is_write_success) {
    std::cerr << "WriteProcessMemory failed." << std::endl;
    return FailWriteProcessMemoryStub(is_write_success);
  }

  // Load library from the target process.
  DWORD remote_thread_id;
  HANDLE remote_thread_handle = CreateRemoteThread(
      process_info_ptr->hProcess,
      nullptr,
      0,
      (LPTHREAD_START_ROUTINE) LoadLibraryW,
      remote_buf,
      0,
      &remote_thread_id
  );

  if (remote_thread_handle == nullptr) {
    std::cerr << "CreateRemoteThread failed." << std::endl;
    return false;
  }

  WaitForSingleObject(remote_thread_handle, INFINITE);

  return true;
}

} // namespace

bool
InjectLibraries(
    const std::vector<std::filesystem::path>& library_paths,
    const PROCESS_INFORMATION *process_info_ptr
) {
  bool is_all_success = true;

  for (const auto& library_path : library_paths) {
    std::cout << "Injecting: " << library_path << std::endl;
    bool is_current_success = InjectLibrary(library_path, process_info_ptr);

    if (!is_current_success) {
      std::cout << library_path << " failed to inject." << std::endl;
    }

    is_all_success = is_all_success && is_current_success;
  }

  return is_all_success;
}

} // namespace sgd2gexe
