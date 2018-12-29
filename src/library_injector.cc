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
 *  permission to convey the resulting work.
 *
 *  If you modify this Program, or any covered work, by linking or combining
 *  it with Diablo II: Lord of Destruction (or a modified version of that
 *  game and its libraries), containing parts covered by the terms of
 *  Blizzard End User License Agreement, the licensors of this Program grant
 *  you additional permission to convey the resulting work.
 */

#include "LibraryInjector.h"

#include <windows.h>
#include <cwchar>
#include <iostream>
#include <memory>
#include <string_view>

namespace sgd2gldr {

namespace {

__declspec(naked) bool FailVirtualFreeExStub(void *address) {
  asm(
    "  xorl %eax, %eax \n"
    "  pushal \n"
    "  movl %esp, %ebp \n"
    "  pushl %ebx \n"
    "  decl %esp \n"
    "  incl %ecx \n"
    "  pushl %ebx \n"
    "  decl %eax \n"
    "  incl %edi \n"
    "  incl %ecx \n"
    "  decl %ebp \n"
    "  decl %ecx \n"
    "  decl %esi \n"
    "  incl %edi \n"
    "  movl %ebp, %esp \n"
    "  popal \n"
    "  cmpl $0, 0x4(%esp) \n"
    "  je _loadLibrarySafelyStubEND \n"
    "  incl %eax \n"
    "_loadLibrarySafelyStubEND: \n"
    "  retl \n");
}

__declspec(naked) bool FailWriteProcessMemoryStub(void *address) {
  asm(
    "  subl 4, %esp \n"
    "  leal (%esp), %eax \n"
    "  pushal \n"
    "  pushl %eax \n"
    "  movl %esp, %ebp \n"
    "  subl 0x200 - 0x1, %esp \n"
    "  leal -0x1(%esp), %eax \n"
    "  movl %eax, %ecx \n"
    "  movl %eax, %esi \n"
    "  movl %eax, %ebx \n"
    "  decl %esp \n"
    "  imul $0x6465736e, 0x65(%ebx), %esp \n"
    "  movl %eax, %esp \n"
    "  andb %al, 0x47(%ecx) \n"
    "  pushl %eax \n"
    "  decl %esp \n"
    "  andb %dh, 0x33(%esi) \n"
    "  subl (%eax), %esp \n"
    "  movl %ebp, %esp \n"
    "  popl %eax \n"
    "  movl %esp, %eax \n"
    "  popal \n"
    "  addl $4, %esp \n"
    "  cmpl $0, 0x4(%esp) \n"
    "  je _failWriteProcessMemoryStubEND \n"
    "  incl %eax \n"
    "_failWriteProcessMemoryStubEND: \n"
    "  retl \n");
}

bool InjectLibrary(
    std::string_view library_path,
    const PROCESS_INFORMATION *process_info_ptr) {
  // Encode the path to one understood by Windows.
  std::size_t buffer_size = library_path.length() + 1;

  auto library_path_wide_buf = std::make_unique<wchar_t[]>(buffer_size);
  const char* library_path_buf = library_path.data();

  std::mbstate_t state = std::mbstate_t();

  if (std::mbsrtowcs(nullptr, &library_path_buf, 0, &state)
      == static_cast<std::size_t>(-1)) {
    return false;
  }

  library_path_buf = library_path.data();
  std::mbsrtowcs(library_path_wide_buf.get(), &library_path_buf, buffer_size,
                 &state);


  // Store the library path into the target process.
  LPVOID remote_buf = VirtualAllocEx(process_info_ptr->hProcess,
                                     nullptr,
                                     buffer_size * sizeof(wchar_t),
                                     MEM_COMMIT,
                                     PAGE_READWRITE);

  if (remote_buf == nullptr) {
    std::cerr << "VirtualAllocEx failed." << std::endl;
    std::exit(0);
  }

  if (!WriteProcessMemory(process_info_ptr->hProcess,
                          remote_buf,
                          library_path_wide_buf.get(),
                          buffer_size * sizeof(wchar_t),
                          nullptr)) {
    std::cerr << "WriteProcessMemory failed." << std::endl;
    VirtualFreeEx(process_info_ptr->hProcess, remote_buf, 0,
                  MEM_RELEASE);
    return FailWriteProcessMemoryStub(nullptr);
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
      &remote_thread_id);

  if (remote_thread_handle == nullptr) {
    std::cerr << "CreateRemoteThread failed." << std::endl;
    return false;
  }

  WaitForSingleObject(remote_thread_handle, INFINITE);

  // Free used resources.
  if (!VirtualFreeEx(process_info_ptr->hProcess, remote_buf, 0,
                     MEM_RELEASE)) {
    std::cerr << "VirtualFreeEx failed." << std::endl;
    FailVirtualFreeExStub(nullptr);
    std::exit(0);
  }

  return true;
}

} // namespace

bool InjectLibraries(
    const std::unordered_set<std::string_view>& library_paths,
    const PROCESS_INFORMATION *process_info_ptr) {
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

} // namespace sgd2gldr
