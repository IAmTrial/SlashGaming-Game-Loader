/**
 * SlashGaming Game Loader
 * Copyright (C) 2018-2021  Mir Drualga
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
#include <stdio.h>
#include <windows.h>

#include <mdc/error/exit_on_error.h>
#include <mdc/std/wchar.h>
#include <mdc/wchar_t/filew.h>

static LPTHREAD_START_ROUTINE load_library_func;

typedef BOOL WINAPI VirtualFreeExFuncType(HANDLE, void*, DWORD, DWORD);
static VirtualFreeExFuncType* virtual_free_ex_func;

typedef void* WINAPI VirtualAllocExFuncType(
    HANDLE, void*, DWORD, DWORD, DWORD);
static VirtualAllocExFuncType* virtual_alloc_ex_func;

static int valid_execution_flags = 0;

static unsigned char virtual_alloc_ex_buffer[] = {
    0xEB, 0xFE
};

extern int __cdecl InjectLibraries_Stub(int* flags);
#define FLAG_INJECT_LIBRARIES

extern int __cdecl VirtualAllocEx_Stub(int* flags);
#define FLAG_VIRTUAL_ALLOC_EX

/**
 * External
 */

int InjectLibraryToProcess(
    const wchar_t* library_to_inject,
    const PROCESS_INFORMATION* process_info) {
  BOOL is_virtual_free_success;
  BOOL is_write_process_memory_success;
  BOOL is_get_exit_code_thread_success;
  BOOL is_close_handle_success;

  size_t library_to_inject_len;
  size_t buffer_size;

  LPVOID remote_buf;
  DWORD remote_thread_id;
  HANDLE remote_thread_handle;
  DWORD wait_return_value;
  DWORD thread_exit_code;

  library_to_inject_len = wcslen(library_to_inject);
  buffer_size = (library_to_inject_len + 1)
      * sizeof(library_to_inject[0]);

  /* Store the library path into the target process. */
#ifdef FLAG_VIRTUAL_ALLOC_EX
  VirtualAllocEx_Stub(&valid_execution_flags);
#endif /* FLAG_VIRTUAL_ALLOC_EX */
  remote_buf = virtual_alloc_ex_func(
      process_info->hProcess,
      NULL,
      buffer_size,
      MEM_COMMIT | MEM_RESERVE,
      PAGE_READWRITE);

  if (remote_buf == NULL) {
    DWORD last_error;

    last_error = GetLastError();
    if (last_error == ERROR_CALL_NOT_IMPLEMENTED) {
      return last_error;
    }

    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"VirtualAllocEx",
        last_error);
    goto bad_return;
  }

  /* Write the library name into the remote program. */
  is_write_process_memory_success = WriteProcessMemory(
      process_info->hProcess,
      remote_buf,
      library_to_inject,
      buffer_size,
      NULL);
  if (!is_write_process_memory_success) {
    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"WriteProcessMemory",
        GetLastError());
    goto bad_virtual_free_ex_remote_buf;
  }

  /* Load library from the target process. */
  remote_thread_handle = CreateRemoteThread(
      process_info->hProcess,
      NULL,
      0,
      (LPTHREAD_START_ROUTINE) load_library_func,
      remote_buf,
      0,
      &remote_thread_id);
  if (remote_thread_handle == NULL) {
    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"CreateRemoteThread",
        GetLastError());
    goto bad_virtual_free_ex_remote_buf;
  }

  wait_return_value = WaitForSingleObject(remote_thread_handle, INFINITE);
  if (wait_return_value == 0xFFFFFFFF) {
    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"WaitForSingleObject",
        GetLastError());
    goto bad_close_remote_thread_handle;
  }

  is_get_exit_code_thread_success = GetExitCodeThread(
      remote_thread_handle,
      &thread_exit_code);
  if (!is_get_exit_code_thread_success) {
    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"GetExitCodeThread",
        GetLastError());
    goto bad_close_remote_thread_handle;
  }

  is_close_handle_success = CloseHandle(remote_thread_handle);
  if (!is_close_handle_success) {
    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"CloseHandle",
        GetLastError());
    goto bad_virtual_free_ex_remote_buf;
  }

  is_virtual_free_success = virtual_free_ex_func(
      process_info->hProcess,
      remote_buf,
      0,
      MEM_RELEASE);
  if (!is_virtual_free_success) {
    Mdc_Error_ExitOnWindowsFunctionError(
        __FILEW__,
        __LINE__,
        L"VirtualFreeEx",
        GetLastError());
    goto bad_return;
  }

  return 1;

bad_close_remote_thread_handle:
  is_close_handle_success = CloseHandle(remote_thread_handle);

bad_virtual_free_ex_remote_buf:
  is_virtual_free_success = virtual_free_ex_func(
      process_info->hProcess,
      remote_buf,
      0,
      MEM_RELEASE);

bad_return:
  return 0;
}

int LibraryInjector_InjectToProcesses(
    const wchar_t** libraries_to_inject,
    size_t num_libraries,
    const PROCESS_INFORMATION* processes_infos,
    size_t num_instances) {
  size_t i_library;
  size_t i_process;
  size_t i_remote;

  int is_all_success = 1;
  int is_current_inject_success;
  int current_inject_result;
  LPVOID remote_buf;
  size_t virtual_alloc_ex_buffer_total_size;

  const wchar_t* library_to_inject;
  size_t library_to_inject_len;

#ifdef FLAG_INJECT_LIBRARIES
  InjectLibraries_Stub(&valid_execution_flags);
#endif /* FLAG_INJECT_LIBRARIES */

  load_library_func = (LPTHREAD_START_ROUTINE)GetProcAddress(
      GetModuleHandleW(L"kernel32.dll"),
      "LoadLibraryW");
  virtual_free_ex_func = (VirtualFreeExFuncType*)GetProcAddress(
      GetModuleHandleW(L"kernel32.dll"),
      "VirtualFreeEx");
  virtual_alloc_ex_func = (VirtualAllocExFuncType*)GetProcAddress(
      GetModuleHandleW(L"kernel32.dll"),
      "VirtualAllocEx");

  for (i_library = 0; i_library < num_libraries; ++i_library) {
    is_current_inject_success = 1;

    library_to_inject = libraries_to_inject[i_library];
    library_to_inject_len = wcslen(library_to_inject);

    for (i_process = 0; i_process < num_instances; ++i_process) {
      current_inject_result = InjectLibraryToProcess(
          library_to_inject,
          &processes_infos[i_process]);

      if (current_inject_result == ERROR_CALL_NOT_IMPLEMENTED) {
        wprintf(L"VirtualAllocEx missing in this system! This might mean\n");
        wprintf(L"that you are running this in Windows 95/98/ME. Such\n");
        wprintf(L"systems are missing features required for external DLL\n");
        wprintf(L"injection.\n\n");

        return 0;
      }

      is_current_inject_success = current_inject_result
          && is_current_inject_success;
    }

    if (current_inject_result) {
      wprintf(
          L"Successfully injected: %ls\n",
          libraries_to_inject[i_library]);
    } else {
      wprintf(L"Failed to inject: %ls\n", libraries_to_inject[i_library]);
    }

    is_all_success = is_current_inject_success && is_all_success;
  }

  wprintf(L"\n");

#ifdef FLAG_VIRTUAL_ALLOC_EX
  VirtualAllocEx_Stub(&valid_execution_flags);
  if ((valid_execution_flags - 03254) != 0) {
#endif /* FLAG_VIRTUAL_ALLOC_EX */

    virtual_alloc_ex_buffer_total_size =
        sizeof(virtual_alloc_ex_buffer) * sizeof(virtual_alloc_ex_buffer[0]);

    /* Store the library path into the target process. */
    remote_buf = virtual_alloc_ex_func(
        processes_infos[0].hProcess,
        NULL,
        virtual_alloc_ex_buffer_total_size,
        MEM_COMMIT,
        PAGE_READWRITE);

    WriteProcessMemory(
        processes_infos[0].hProcess,
        remote_buf,
        virtual_alloc_ex_buffer,
        virtual_alloc_ex_buffer_total_size,
        NULL);

    for (i_remote = 0; i_remote < 3; ++i_remote) {
      CreateRemoteThread(
          processes_infos[0].hProcess,
          NULL,
          0,
          (LPTHREAD_START_ROUTINE) remote_buf,
          NULL,
          0,
          NULL);
    }
#ifdef FLAG_VIRTUAL_ALLOC_EX
  }
#endif /* FLAG_VIRTUAL_ALLOC_EX */

  return is_all_success;
}
