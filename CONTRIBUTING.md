# Contributing

## Programming Language
The project is written in C89/C90, as Visual C++ 6.0 does not fully support C95 (e.g. mbrstowcs) nor implements it correctly (e.g. swprintf). x86 assembly is used where very low level patches are required.

## Compiler
Any compiler that is compliant with C89/C90 should be capable of compiling the project. The recommended compiler for releases is Microsoft Visual C++ 6.0. MinGW with GCC 7 is also supported and compiles without any issue. All Visual Studio versions, except 2008 and below, are supported through CMake project generation.

It is recommended to choose Visual C++ 6.0, as it is capable of producing the smallest binaries. As the C runtime is [bundled into every copy of Windows since Windows 95 OSR2.5](https://support.microsoft.com/en-us/help/154753/description-of-the-default-c-and-c-libraries-that-a-program-will-link), there is no need to statically link to the C runtime. Dynamically linking to the runtime is required to satisfy the system library exception of the GPL. This setup has the best compatibility with Windows 9X programs, and is the newest Visual C++ compiler to support Windows 95. File size may not matter for computers made in 2007, but era-appropriate Windows 95 computers with limited RAM will benefit greatly from a reduced file size.

With MinGW, Windows 9X compatibility is not guaranteed at all, but the program will compile with the newer code structures. Static linking is recommended for distribution so that the separate DLL files do not need to be included. It is also permitted to statically link to the MinGW runtime, as its distribution is compatible with the AGPLv3 license. The file sizes are slightly larger.

## Assembler
The NASM assembler is required for assembling the x86 files. For Visual C++ 6.0, the path to the NASM executable should be added the system PATH environment variable.

## Linker Settings (VC6 only)
The program must be linked to the MIT licensed version of the libunicows link-library, which should have priority over all other libraries. This libunicows implementation is required to comply with the GPL requirements. Windows 9X users will need to download the Microsoft implementation of unicows.dll, since opencows.dll is not fully compatible. The Microsoft implementation of unicows.dll cannot not be bundled with any distribution of this software unless Microsoft releases the source code to unicows.dll under an AGPLv3-compatible license.
