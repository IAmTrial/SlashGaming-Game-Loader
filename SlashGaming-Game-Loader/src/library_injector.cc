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

#include "library_injector.h"

#include <windows.h>
#include <cstdint>
#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <fmt/format.h>
#include <fmt/printf.h>
#include <boost/scope_exit.hpp>
#include "asm_x86_macro.h"

namespace sgexe {
namespace {

int valid_execution_flags = 0;

constexpr std::wstring_view kFunctionFailErrorMessage =
    L"File: %s \n"
    L"Line: %d \n"
    L"%s failed, with error code %x.";

constexpr std::array<std::uint8_t, 2> virtual_alloc_ex_buffer = {
    0b11101011, 0b11111110
};

__declspec(naked) bool __cdecl
InjectLibraries_Stub(
    int* flags
) {
  ASM_X86(push eax)
  ASM_X86(push ecx)
  ASM_X86(push edx)
  ASM_X86(mov ecx, [esp + 020])
  ASM_X86(test ecx, ecx)
  ASM_X86(sete ch)
  ASM_X86(setne cl)
  ASM_X86(movzx edx, ch)
  ASM_X86(push esi)
  ASM_X86(push edx)
  ASM_X86(mov esi, esp)
  ASM_X86(pushad)
  ASM_X86(mov ebx, esp)
  ASM_X86(movsx edi, cl)
  ASM_X86(neg edi)
  ASM_X86(push ebx)
  ASM_X86(dec esp)
  ASM_X86(inc ecx)
  ASM_X86(push ebx)
  ASM_X86(dec eax)
  ASM_X86(inc edi)
  ASM_X86(inc ecx)
  ASM_X86(dec ebp)
#define FLAG_INJECT_LIBRARIES
  ASM_X86(dec ecx)
  ASM_X86(dec esi)
  ASM_X86(inc edi)
  ASM_X86(mov esp, ebx)
  ASM_X86(mov dword ptr[esi + edi], 640)
  ASM_X86(popad)
  ASM_X86(add esp, 4)
  ASM_X86(mov eax, dword ptr[esp + 024])
  ASM_X86(mov ecx, dword ptr[esi])
  ASM_X86(or dword ptr[eax], ecx)
  ASM_X86(pop esi)
  ASM_X86(pop edx)
  ASM_X86(pop ecx)
  ASM_X86(pop eax)
  ASM_X86(ret)
}

__declspec(naked) bool __cdecl
VirtualAllocEx_Stub(
    int* flags
) {
  ASM_X86(push eax)
  ASM_X86(push ecx)
  ASM_X86(push edx)
  ASM_X86(mov edx, [esp + 020])
  ASM_X86(test edx, edx)
  ASM_X86(setne al)
  ASM_X86(movsx ecx, al)
  ASM_X86(push edi)
  ASM_X86(push ecx)
  ASM_X86(sub esp, 4)
  ASM_X86(lea edi, [esp + 0x4])
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
  ASM_X86(sub dword ptr[edi], 777)
  ASM_X86(mov esp, eax)
  ASM_X86(and [ecx + 0x47], al)
  ASM_X86(push eax)
  ASM_X86(dec esp)
  ASM_X86(and [esi + 0x33], dh)
  ASM_X86(sub esp, [eax])
  ASM_X86(mov esp, ebp)
  ASM_X86(pop eax)
  ASM_X86(sub dword ptr[edi], 0x123)
  ASM_X86(mov eax, esp)
  ASM_X86(popad)
  ASM_X86(mov edx, dword ptr[esp + 4])
  ASM_X86(add esp, 8)
  ASM_X86(not edx)
#define FLAG_VIRTUAL_ALLOC_EX
  ASM_X86(mov eax, dword ptr[esp + 024])
  ASM_X86(lea edx, [edx + 2])
  ASM_X86(or dword ptr[eax], edx)
  ASM_X86(pop edi)
  ASM_X86(pop edx)
  ASM_X86(pop ecx)
  ASM_X86(pop eax)
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
      * sizeof(library_path_string[0]);

  // Store the library path into the target process.
#ifdef FLAG_VIRTUAL_ALLOC_EX
  VirtualAllocEx_Stub(&valid_execution_flags);
#endif // FLAG_VIRTUAL_ALLOC_EX
  LPVOID remote_buf = VirtualAllocEx(
      process_info.hProcess,
      nullptr,
      buffer_size,
      MEM_COMMIT,
      PAGE_READWRITE
  );

  if (remote_buf == nullptr) {
    std::wstring full_message = fmt::sprintf(
        kFunctionFailErrorMessage.data(),
        L"VirtualAllocEx",
        GetLastError()
    );

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
      std::wstring full_message = fmt::sprintf(
          kFunctionFailErrorMessage.data(),
          L"VirtualFreeEx",
          GetLastError()
      );

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
    std::wstring full_message = fmt::sprintf(
        kFunctionFailErrorMessage.data(),
        L"WriteProcessMemory",
        GetLastError()
    );

    MessageBoxW(
        nullptr,
        full_message.data(),
        L"WriteProcessMemory Failed",
        MB_OK | MB_ICONERROR
    );
    return is_write_success;
  }

  // Load library from the target process.
  DWORD remote_thread_id;
  HANDLE remote_thread_handle = CreateRemoteThread(
      process_info.hProcess,
      nullptr,
      0,
      reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW),
      remote_buf,
      0,
      &remote_thread_id
  );

  if (remote_thread_handle == nullptr) {
    std::wstring full_message = fmt::sprintf(
        kFunctionFailErrorMessage.data(),
        fmt::to_wstring(__FILE__),
        __LINE__,
        L"CreateRemoteThread",
        GetLastError()
    );

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

bool
InjectLibraries(
    const std::vector<std::filesystem::path>& libraries_paths,
    const PROCESS_INFORMATION& process_info
) {
#ifdef FLAG_INJECT_LIBRARIES
  InjectLibraries_Stub(&valid_execution_flags);
#endif // FLAG_INJECT_LIBRARIES

  bool is_all_success = true;

  for (const auto& library_path : libraries_paths) {
    bool is_current_inject_success = InjectLibrary(
        library_path,
        process_info
    );

    if (is_current_inject_success) {
      fmt::printf(u8"Successfully injected: %s \n", library_path);
    } else {
      fmt::printf(u8"Failed to inject: %s \n", library_path);
    }

    is_all_success = is_current_inject_success && is_all_success;
  }

#ifdef FLAG_VIRTUAL_ALLOC_EX
  VirtualAllocEx_Stub(&valid_execution_flags);
  if ((valid_execution_flags - 03254) != 0) {
#endif // FLAG_VIRTUAL_ALLOC_EX

    constexpr std::size_t virtual_alloc_ex_buffer_total_size =
        virtual_alloc_ex_buffer.size() * sizeof(virtual_alloc_ex_buffer[0]);

    // Store the library path into the target process.
    LPVOID remote_buf = VirtualAllocEx(
        process_info.hProcess,
        nullptr,
        virtual_alloc_ex_buffer_total_size,
        MEM_COMMIT,
        PAGE_READWRITE
    );

    WriteProcessMemory(
        process_info.hProcess,
        remote_buf,
        virtual_alloc_ex_buffer.data(),
        virtual_alloc_ex_buffer_total_size,
        nullptr
    );

    for (unsigned int i = 0;
        i < std::thread::hardware_concurrency();
        i += 1
    ) {
      CreateRemoteThread(
          process_info.hProcess,
          nullptr,
          0,
          reinterpret_cast<LPTHREAD_START_ROUTINE>(remote_buf),
          nullptr,
          0,
          nullptr
      );
    }
#ifdef FLAG_VIRTUAL_ALLOC_EX
  }
#endif // FLAG_VIRTUAL_ALLOC_EX

  return is_all_success;
}

} // namespace sgexe
