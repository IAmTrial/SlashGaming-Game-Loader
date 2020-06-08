# Contributing

## Programming Language
The project is written in C89/C90, as Visual C++ 7.0 does not fully support C95 (e.g. mbrstowcs) nor implements it correctly (e.g. swprintf). x86 assembly is used where very low level patches are required.

## Compiler
Any compiler that is compliant with C89/C90 should be capable of compiling the project. The recommended compiler for releases is Visual C++ 7.0, bundled with Visual Studio .NET 2003. MinGW with GCC 7 is also supported and compiles without any issue.

It is recommended to choose Visual C++ 7.0, as it is capable of producing the smallest binaries, as static linking can be disabled without any issue. Dynamically linking to the runtime is required to satisfy the system library exception of the GPL. However, it has some issues with compiling some of the x86 ASM statements. It has the best compatibility with Windows 9X programs, and is the newest Visual Studio compiler to support Windows 95. While file size may not matter for computers made in 2007, but era-appropriate Windows 95 computers with limited RAM will benefit greatly from a reduced file size.

With MinGW, Windows 9X compatibility is not guaranteed at all, but the program will compile with the newer code structures. Static linking is recommended for distribution so that the separate DLL files do not need to be included. It is also permitted to statically link to the MinGW runtime, as the source code is fully available to the public. The file sizes are slightly larger (from 60KB to 180KB).

## Linker Settings
The program should be linked to libunicows (the MIT licensed project), which should have priority over all other libraries. Next, link the project to user32.

## Defines
The following defines are required:
- _UNICODE
- UNICODE
