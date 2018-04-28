/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: nl_hack.h /main/5 1996/11/15 18:12:32 drk $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef __NL_HACK_H_
#define __NL_HACK_H_

#include	<ctype.h>

#ifdef NO_NLS16

# define ADVANCE(p)     (++(p))
# define CHARAT(p)      (*(unsigned char *)(p))
# define CHARADV(p)     (*(unsigned char *)(p)++)
# define WCHAR(c, p)    (*(unsigned char *)(p) = c)
# define WCHARADV(c, p) (*(unsigned char *)(p)++ = c)

#else

# include <locale.h>
# include <stdlib.h>

/* These two globals are needed for the following macros to work.
 * The definitions are neither "extern" nor "static".  This keeps both
 * C and C++ from complaining about unused variables, keeps the linker
 * from complaining about multiply-defined symbols, and creates only a
 * single instance of each var per program (rather than one per *.o).
 * This is also why both variables are uninitialized arrays.
 *
 * These macros imitate the original HP NLS16 equivalents so that rest
 * of the Codelibs code does not need to be modified.  The original
 * definitions are below within the "#ifdef NLS16" section.
 *
 * The original HP NLS16 assumes 2 byte multi-byte characters. It is
 * generalized for all multi-byte characters.
 */

/* wchar_t __nlh_char[1]; */

# define __NLH_WIDTH(p)	(mblen(p, MB_CUR_MAX) > 1 ? mblen(p, MB_CUR_MAX) : 1)
# define __NLH_CHAR(p)	\
	(mbtowc(__nlh_char, p, MB_CUR_MAX) < 0 ? *p : __nlh_char[0])

# define ADVANCE(p)	((p) += __NLH_WIDTH(p))

# define CHARAT(p)	(__NLH_CHAR(p))

# define CHARADV(p)	(__NLH_CHAR(p),	\
			 (p) += __NLH_WIDTH(p), __nlh_char[0])

# define WCHAR(c, p)	(wctomb(p, (wchar_t)c), c)

# define WCHARADV(c, p)	(WCHAR(c, p), ADVANCE(p))

#endif

#endif /* __NL_HACK_H_ */
