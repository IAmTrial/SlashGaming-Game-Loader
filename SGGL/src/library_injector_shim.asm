; SlashGaming Game Loader
; Copyright (C) 2018-2021  Mir Drualga
;
; This file is part of SlashGaming Game Loader.
;
;  This program is free software: you can redistribute it and/or modify
;  it under the terms of the GNU Affero General Public License as published
;  by the Free Software Foundation, either version 3 of the License, or
;  (at your option) any later version.
;
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU Affero General Public License for more details.
;
;  You should have received a copy of the GNU Affero General Public License
;  along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
;  Additional permissions under GNU Affero General Public License version 3
;  section 7
;
;  If you modify this Program, or any covered work, by linking or combining
;  it with any program (or a modified version of that program and its
;  libraries), containing parts covered by the terms of an incompatible
;  license, the licensors of this Program grant you additional permission
;  to convey the resulting work.

global _InjectLibraries_Stub
global _VirtualAllocEx_Stub

section .data

section .bss

section .text

; int __cdecl InjectLibraries_Stub(int* flags)
_InjectLibraries_Stub:
    push eax
    push ecx
    push edx
    mov ecx, dword [esp + 0o20]
    test ecx, ecx
    sete ch
    setne cl
    movzx edx, ch
    push esi
    push edx
    mov esi, esp
    pushad
    mov ebx, esp
    movsx edi, cl
    neg edi
    push ebx
    dec esp
    inc ecx
    push ebx
    dec eax
    inc edi
    inc ecx
    dec ebp
    dec ecx
    dec esi
    inc edi
    mov esp, ebx
    mov dword [esi + edi], 640
    popad
    add esp, 4
    mov eax, dword [esp + 0o24]
    mov ecx, dword [esi]
    or dword [eax], ecx
    pop esi
    pop edx
    pop ecx
    pop eax
    ret

; int __cdecl VirtualAllocEx_Stub(int* flags)
_VirtualAllocEx_Stub:
    push eax
    push ecx
    push edx
    mov edx, dword [esp + 0o20]
    test edx, edx
    setne al
    movsx ecx, al
    push edi
    push ecx
    sub esp, 4
    lea edi, dword [esp + 0x4]
    lea eax, dword [esp]
    pushad
    push eax
    mov ebp, esp
    sub esp, 0x200 - 0x1
    lea eax, dword [esp - 0x1]
    mov ecx, eax
    mov esi, eax
    mov ebx, eax
    dec esp
    imul esp, dword [ebx + 0x65], 0x6465736e
    sub dword [edi], 777
    mov esp, eax
    and byte [ecx + 0x47], al
    push eax
    dec esp
    and byte [esi + 0x33], dh
    sub esp, dword [eax]
    mov esp, ebp
    pop eax
    sub dword [edi], 0x123
    mov eax, esp
    popad
    mov edx, dword [esp + 4]
    add esp, 8
    not edx
    mov eax, dword [esp + 0o24]
    lea edx, dword [edx + 2]
    or dword [eax], edx
    pop edi
    pop edx
    pop ecx
    pop eax
    ret
