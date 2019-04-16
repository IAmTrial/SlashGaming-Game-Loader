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

#ifndef SGEXE_FILEW_MACRO_H_
#define SGEXE_FILEW_MACRO_H_

#ifndef _CRT_WIDE_
#define _CRT_WIDE_(s) L##s
#endif /* _CRT_WIDE_ */

#ifndef _CRT_WIDE
#define _CRT_WIDE(s) _CRT_WIDE_(s)
#endif /* _CRT_WIDE */

#ifndef __FILEW__
#define __FILEW__ _CRT_WIDE(__FILE__)
#endif /* __FILEW__ */

#endif /* SGEXE_FILEW_MACRO_H_ */
