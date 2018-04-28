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
/* $XConsortium: DtNlUtils.h /main/4 1996/06/21 17:22:30 ageorge $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/**************************************************************************/
/*                                                                        */
/* Public include file for Dt localization functions.                    */
/*                                                                        */
/**************************************************************************/

#ifdef NLS16

#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <limits.h>
#include <nl_types.h>
#include <langinfo.h>
#endif

#include <X11/Intrinsic.h>


#ifdef NLS16

#define is_multibyte	_DtNl_is_multibyte
extern Boolean _DtNl_is_multibyte;


extern void Dt_nlInit( void ) ;
extern char * Dt_strtok( 
                        char *s1,
                        char *s2) ;
extern char * Dt_strtok_r( 
                        char *s1,
                        char *s2,
                        char **ptr) ;
extern int Dt_strspn( 
                        char *s1,
                        char *s2) ;
extern int Dt_strcspn( 
                        char *s1,
                        char *s2) ;
extern char * Dt_strchr( 
                        char *s,
                        char c) ;
extern char * Dt_strrchr( 
                        char *s,
                        char c) ;
extern void Dt_lastChar( 
                        char *s,
                        char **cptr,
                        int *lenptr) ;
extern int Dt_charCount( 
                        char *s) ;

extern char * _Dt_NextChar(char *s);
extern char * _Dt_PrevChar(const char *start,char *s);
extern int _Dt_isspace(char *s);
extern int _Dt_isdigit(char *s);

#define DtNlInitialize()     (Dt_nlInit())
#define DtStrtok(s1, s2)     (Dt_strtok(s1, s2))
#define DtStrtok_r(s1, s2, ptr)     (Dt_strtok_r(s1, s2, ptr))
#define DtStrspn(s1, s2)     (Dt_strspn(s1, s2))
#define DtStrcspn(s1, s2)    (Dt_strcspn(s1, s2))
#define DtStrchr(s1, c)      (Dt_strchr(s1, c))
#define DtStrrchr(s1, c)     (Dt_strrchr(s1, c))
#define DtLastChar(s1, cp, lp)   (Dt_lastChar(s1, cp, lp))
#define DtCharCount(s1)      (Dt_charCount(s1))
#define DtNextChar(s)	     (is_multibyte?_Dt_NextChar(s):((s)+1))
#define DtPrevChar(st,s)     (is_multibyte?_Dt_PrevChar(st,s):((s)-1))
#define DtIsspace(s)	     (is_multibyte?_Dt_isspace(s):isspace(*(s)))
#define DtIsdigit(s)	     (is_multibyte?_Dt_isdigit(s):isdigit(*(s)))

#else	/* NLS16 */

#define DtNlInitialize()
#define DtStrtok(s1, s2)     (strtok(s1, s2))
#define DtStrtok_r(s1, s2, ptr)     (strtok_r(s1, s2, ptr))
#define DtStrspn(s1, s2)     (strspn(s1, s2))
#define DtStrcspn(s1, s2)    (strcspn(s1, s2))
#define DtStrchr(s1, c)      (strchr(s1, c))
#define DtStrrchr(s1, c)     (strrchr(s1, c))
#define DtLastChar(s1, cp, lp)   {(*cp = s1 + strlen(s1) - 1); *lp = 1;}
#define DtCharCount(s1)      (strlen(s1))
#define DtNextChar(s)	     ((s)+1)
#define DtPrevChar(st,s)     ((s)-1)
#define DtIsspace(s)	     (isspace(*s))
#define DtIsdigit(s)	     (isdigit(*s))
#endif /* NLS16 */

extern char * _DtGetNthChar( 
                        char *s,
                        int n) ;
extern char * _dt_strpbrk(
			char *cs,
			char *ct);
extern int _is_previous_single(
			char *s1,
			char *s2);

