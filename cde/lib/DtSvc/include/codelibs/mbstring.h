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
 * File:	mbstring.h $XConsortium: mbstring.h /main/3 1995/10/26 16:12:25 rswiston $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef	__MBSTRING_H_
#define	__MBSTRING_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined __cplusplus || defined __STDC__

#ifdef __cplusplus2_1
    extern char *_mb_schr(char *str, wchar_t ch);
    extern char *_mb_srchr(char *str, wchar_t ch);
#else /* __cplusplus2_1 */
    extern char *_mb_schr(const char *str, wchar_t ch);
    extern char *_mb_srchr(const char *str, wchar_t ch);
#endif /* __cplusplus2_1 */

#else /* defined __cplusplus || defined __STDC__ */

    extern char *_mb_schr();
    extern char *_mb_srchr();

#endif /* defined __cplusplus || defined __STDC__ */

#ifdef __cplusplus
}

#ifdef __cplusplus2_1
inline const char *_mb_schr(const char *str, wchar_t ch)
{
    return (const char *)_mb_schr((char *)str, ch);
}

inline const char *_mb_srchr(const char *str, wchar_t ch)
{
    return (const char *)_mb_srchr((char *)str, ch);
}
#endif /* __cplusplus2_1 */
#endif /* __cplusplus */

#endif /* __MBSTRING_H_ */
