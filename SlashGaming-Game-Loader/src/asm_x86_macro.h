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

/**
 * Warning: This header should never be used in any public interface!
 */

#ifndef SGGL_ASM_X86_MACRO_H_
#define SGGL_ASM_X86_MACRO_H_

#if defined(_MSC_VER)

#define ASM_X86_01(a) \
    __asm { \
      a \
    }

#define ASM_X86_02(a, b) \
    __asm { \
      a, b \
    }

#define ASM_X86_03(a, b, c) \
    __asm { \
      a, b, c \
    }

#define ASM_X86_FUNC(name) name

#define ASM_X86_LABEL(name) name:

#else

#define ASM_X86_01(a) \
    __asm( \
        #a \
    );

#define ASM_X86_02(a, b) \
    __asm( \
        #a ", " #b \
    );

#define ASM_X86_03(a, b, c) \
    __asm( \
        #a ", " #b ", " #c \
    );

#define ASM_X86_FUNC(name) _##name

#define ASM_X86_LABEL(name) ASM_X86(name:)

#endif

#endif /* SGGL_ASM_X86_MACRO_H_ */
