/**
 * SlashGaming Game Loader
 * Copyright (C) 2018  Mir Drualga
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

#include "library_injector.h"

#include <windows.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <boost/format.hpp>
#include <boost/scope_exit.hpp>
#include "asm_x86_macro.h"

namespace sgexe {
namespace {

constexpr std::wstring_view kFunctionFailErrorMessage =
    L"%s failed, with error code %x.";

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
  ASM_X86(add ebp, 1)
  ASM_X86(mov esp, ebp)
  ASM_X86(popad)
  ASM_X86(cmp dword ptr[esp + 0x4], 0)
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
  ASM_X86(cmp dword ptr[esp + 0x4], 0)
  ASM_X86(je _failWriteProcessMemoryStubEND)
  ASM_X86(inc eax)
_failWriteProcessMemoryStubEND:
  ASM_X86(ret)
}

} // namespace

bool
InjectLibrary(
    const std::filesystem::path& library_path,
    const PROCESS_INFORMATION& process_info
) {
  // Encode the path to one understood by Windows.
  std::wstring library_path_string = library_path.wstring();
  std::size_t buffer_size = (library_path_string.length() + 1)
      * sizeof(decltype(library_path_string)::value_type);

  // Store the library path into the target process.
  LPVOID remote_buf = VirtualAllocEx(
      process_info.hProcess,
      nullptr,
      buffer_size,
      MEM_COMMIT,
      PAGE_READWRITE
  );

  if (remote_buf == nullptr) {
    std::wstring full_message = (
        boost::wformat(kFunctionFailErrorMessage.data())
        % "VirtualAllocEx"
        % GetLastError()
    ).str();

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"VirtualAllocEx Failed",
        MB_OK | MB_ICONERROR
    );
    std::exit(0);
  }

  // Free used resources at the end of the function scope.
  BOOST_SCOPE_EXIT(&process_info, &remote_buf) {
    BOOL is_free_success = VirtualFreeEx(
        process_info.hProcess,
        remote_buf,
        0,
        MEM_RELEASE
    );

    if (!is_free_success) {
      std::wstring full_message = (
          boost::wformat(kFunctionFailErrorMessage.data())
          % "VirtualFreeEx"
          % GetLastError()
      ).str();

      FailVirtualFreeExStub(is_free_success);
      MessageBoxW(
          nullptr,
          full_message.data(),
          L"VirtualFreeEx Failed",
          MB_OK | MB_ICONERROR
      );
      std::exit(0);
    }
  } BOOST_SCOPE_EXIT_END

  // Write the library name into the remote program.
  BOOL is_write_success = WriteProcessMemory(
      process_info.hProcess,
      remote_buf,
      library_path_string.data(),
      buffer_size,
      nullptr
  );

  if (!is_write_success) {
    std::wstring full_message = (
        boost::wformat(kFunctionFailErrorMessage.data())
        % "WriteProcessMemory"
        % GetLastError()
    ).str();

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"WriteProcessMemory Failed",
        MB_OK | MB_ICONERROR
    );
    return FailWriteProcessMemoryStub(is_write_success);
  }

  // Load library from the target process.
  DWORD remote_thread_id;
  HANDLE remote_thread_handle = CreateRemoteThread(
      process_info.hProcess,
      nullptr,
      0,
      (LPTHREAD_START_ROUTINE) LoadLibraryW,
      remote_buf,
      0,
      &remote_thread_id
  );

  if (remote_thread_handle == nullptr) {
    std::wstring full_message = (
        boost::wformat(kFunctionFailErrorMessage.data())
        % "CreateRemoteThread"
        % GetLastError()
    ).str();

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"CreateRemoteThread Failed",
        MB_OK | MB_ICONERROR
    );
    return false;
  }

  WaitForSingleObject(remote_thread_handle, INFINITE);

  return true;
}

} // namespace sgexe
