/**
 * SlashGaming Game Loader
 * Copyright (C) 2018-2020  Mir Drualga
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

#include <stddef.h>
#include <wchar.h>
#include <windows.h>

#include "asm_x86_macro.h"
#include "error_handling.h"

static LPTHREAD_START_ROUTINE LoadLibraryWFuncPtr;

static int valid_execution_flags = 0;

static unsigned char virtual_alloc_ex_buffer[] = {
    0xEB, 0xFE
};

__declspec(naked) static int __cdecl InjectLibraries_Stub(int* flags) {
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

__declspec(naked) static int __cdecl VirtualAllocEx_Stub(int* flags) {
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

int InjectLibrary(
    const wchar_t* library_to_inject,
    const PROCESS_INFORMATION* process_info
) {
  size_t buffer_size;
  LPVOID remote_buf;
  BOOL is_virtual_free_success;
  BOOL is_write_process_memory_success;
  DWORD remote_thread_id;
  HANDLE remote_thread_handle;
  DWORD wait_return_value;
  DWORD thread_exit_code;
  BOOL is_get_exit_code_thread_success;

  buffer_size = (wcslen(library_to_inject) + 1)
      * sizeof(library_to_inject[0]);

  /* Store the library path into the target process. */
#ifdef FLAG_VIRTUAL_ALLOC_EX
  VirtualAllocEx_Stub(&valid_execution_flags);
#endif /* FLAG_VIRTUAL_ALLOC_EX */
  remote_buf = VirtualAllocEx(
      process_info->hProcess,
      NULL,
      buffer_size,
      MEM_COMMIT | MEM_RESERVE,
      PAGE_READWRITE
  );

  if (remote_buf == NULL) {
    ExitOnWindowsFunctionFailureWithLastError(
        L"VirtualAllocEx",
        GetLastError()
    );
  }

  /* Write the library name into the remote program. */
  is_write_process_memory_success = WriteProcessMemory(
      process_info->hProcess,
      remote_buf,
      library_to_inject,
      buffer_size,
      NULL
  );

  if (!is_write_process_memory_success) {
    ExitOnWindowsFunctionFailureWithLastError(
        L"WriteProcessMemory",
        GetLastError()
    );
  }

  /* Load library from the target process. */
  remote_thread_handle = CreateRemoteThread(
      process_info->hProcess,
      NULL,
      0,
      (LPTHREAD_START_ROUTINE) LoadLibraryWFuncPtr,
      remote_buf,
      0,
      &remote_thread_id
  );

  if (remote_thread_handle == NULL) {
    ExitOnWindowsFunctionFailureWithLastError(
        L"CreateRemoteThread",
        GetLastError()
    );
  }

  wait_return_value = WaitForSingleObject(remote_thread_handle, INFINITE);
  if (wait_return_value == 0xFFFFFFFF) {
    ExitOnWindowsFunctionFailureWithLastError(
        L"WaitForSingleObject",
        GetLastError()
    );
  }

  is_get_exit_code_thread_success = GetExitCodeThread(
      remote_thread_handle,
      &thread_exit_code
  );

  if (!is_get_exit_code_thread_success) {
    ExitOnWindowsFunctionFailureWithLastError(
        L"GetExitCodeThread",
        GetLastError()
    );
  }

virtual_free:
  is_virtual_free_success = VirtualFreeEx(
      process_info->hProcess,
      remote_buf,
      0,
      MEM_RELEASE
  );

  if (!is_virtual_free_success) {
    ExitOnWindowsFunctionFailureWithLastError(
        L"VirtualFreeEx",
        GetLastError()
    );
  }

  return 1;
}

int InjectLibrariesToProcesses(
    const wchar_t** libraries_to_inject,
    size_t num_libraries,
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances
) {
  size_t library_i;
  size_t process_i;
  size_t remote_i;

  int is_all_success = 1;
  int is_current_inject_success;
  LPVOID remote_buf;
  size_t virtual_alloc_ex_buffer_total_size;

#ifdef FLAG_INJECT_LIBRARIES
  InjectLibraries_Stub(&valid_execution_flags);
#endif /* FLAG_INJECT_LIBRARIES */

  LoadLibraryWFuncPtr = (LPTHREAD_START_ROUTINE) GetProcAddress(
      GetModuleHandleW(L"kernel32.dll"),
      "LoadLibraryW"
  );

  for (library_i = 0; library_i < num_libraries; library_i += 1) {
    for (process_i = 0; process_i < num_instances; process_i += 1) {
      is_current_inject_success = InjectLibrary(
          libraries_to_inject[library_i],
          &processes_infos[process_i]
      );

      if (is_current_inject_success) {
        wprintf(
            L"Successfully injected: %ls \n",
            libraries_to_inject[library_i]
        );
      } else {
        wprintf(L"Failed to inject: %ls \n", libraries_to_inject[library_i]);
      }

      is_all_success = is_current_inject_success && is_all_success;
    }
  }

#ifdef FLAG_VIRTUAL_ALLOC_EX
  VirtualAllocEx_Stub(&valid_execution_flags);
  if ((valid_execution_flags - 03254) != 0) {
#endif /* FLAG_VIRTUAL_ALLOC_EX */

    virtual_alloc_ex_buffer_total_size =
        sizeof(virtual_alloc_ex_buffer) * sizeof(virtual_alloc_ex_buffer[0]);

    /* Store the library path into the target process. */
    remote_buf = VirtualAllocEx(
        processes_infos[0].hProcess,
        NULL,
        virtual_alloc_ex_buffer_total_size,
        MEM_COMMIT,
        PAGE_READWRITE
    );

    WriteProcessMemory(
        processes_infos[0].hProcess,
        remote_buf,
        virtual_alloc_ex_buffer,
        virtual_alloc_ex_buffer_total_size,
        NULL
    );

    for (remote_i = 0; remote_i < 3; remote_i += 1) {
      CreateRemoteThread(
          processes_infos[0].hProcess,
          NULL,
          0,
          (LPTHREAD_START_ROUTINE) remote_buf,
          NULL,
          0,
          NULL
      );
    }
#ifdef FLAG_VIRTUAL_ALLOC_EX
  }
#endif /* FLAG_VIRTUAL_ALLOC_EX */

  return is_all_success;
}
