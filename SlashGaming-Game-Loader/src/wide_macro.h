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

#ifndef SGGL_WIDE_MACRO_H_
#define SGGL_WIDE_MACRO_H_

#ifndef MAPI_CAT
#define MAPI_CAT(a, b) a##b
#endif /* MAPI_CAT */

#ifndef MAPI_WIDE_LIT
#define MAPI_WIDE_LIT(s) MAPI_CAT(L, s)
#endif /* MAPI_WIDE_LIT */

#ifndef __FILEW__
#if !defined(_MSC_VER)
#define __FILEW__ MAPI_WIDE_LIT(__FILE__)
#endif /* defined(_MSC_VER) */
#endif /* __FILEW__ */

#endif /* SGGL_WIDE_MACRO_H_ */
