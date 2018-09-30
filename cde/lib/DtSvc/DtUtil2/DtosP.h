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
/* $TOG: DtosP.h /main/4 1998/01/21 16:37:29 mgreess $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifdef REV_INFO
#ifndef lint
static char SCCSID[] = "OSF/Motif: @(#)_DtosP.h	4.16 91/09/12";
#endif /* lint */
#endif /* REV_INFO */
/******************************************************************************
*******************************************************************************
*
*  (c) Copyright 1989, 1990, 1991 OPEN SOFTWARE FOUNDATION, INC.
*  (c) Copyright 1989, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
*  (c) Copyright 1987, 1988, 1989, 1990, 1991 HEWLETT-PACKARD COMPANY
*  ALL RIGHTS RESERVED
*  
*  	THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED
*  AND COPIED ONLY IN ACCORDANCE WITH THE TERMS OF SUCH LICENSE AND
*  WITH THE INCLUSION OF THE ABOVE COPYRIGHT NOTICE.  THIS SOFTWARE OR
*  ANY OTHER COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE
*  AVAILABLE TO ANY OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF THE
*  SOFTWARE IS HEREBY TRANSFERRED.
*  
*  	THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT
*  NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY OPEN SOFTWARE
*  FOUNDATION, INC. OR ITS THIRD PARTY SUPPLIERS  
*  
*  	OPEN SOFTWARE FOUNDATION, INC. AND ITS THIRD PARTY SUPPLIERS,
*  ASSUME NO RESPONSIBILITY FOR THE USE OR INABILITY TO USE ANY OF ITS
*  SOFTWARE .   OSF SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
*  KIND, AND OSF EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES, INCLUDING
*  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE.
*  
*  Notice:  Notwithstanding any other lease or license that may pertain to,
*  or accompany the delivery of, this computer software, the rights of the
*  Government regarding its use, reproduction and disclosure are as set
*  forth in Section 52.227-19 of the FARS Computer Software-Restricted
*  Rights clause.
*  
*  (c) Copyright 1989, 1990, 1991 Open Software Foundation, Inc.  Unpublished - all
*  rights reserved under the Copyright laws of the United States.
*  
*  RESTRICTED RIGHTS NOTICE:  Use, duplication, or disclosure by the
*  Government is subject to the restrictions as set forth in subparagraph
*  (c)(1)(ii) of the Rights in Technical Data and Computer Software clause
*  at DFARS 52.227-7013.
*  
*  Open Software Foundation, Inc.
*  11 Cambridge Center
*  Cambridge, MA   02142
*  (617)621-8700
*  
*  RESTRICTED RIGHTS LEGEND:  This computer software is submitted with
*  "restricted rights."  Use, duplication or disclosure is subject to the
*  restrictions as set forth in NASA FAR SUP 18-52.227-79 (April 1985)
*  "Commercial Computer Software- Restricted Rights (April 1985)."  Open
*  Software Foundation, Inc., 11 Cambridge Center, Cambridge, MA  02142.  If
*  the contract contains the Clause at 18-52.227-74 "Rights in Data General"
*  then the "Alternate III" clause applies.
*  
*  (c) Copyright 1989, 1990, 1991 Open Software Foundation, Inc.
*  ALL RIGHTS RESERVED 
*  
*  
* Open Software Foundation is a trademark of The Open Software Foundation, Inc.
* OSF is a trademark of Open Software Foundation, Inc.
* OSF/Motif is a trademark of Open Software Foundation, Inc.
* Motif is a trademark of Open Software Foundation, Inc.
* DEC is a registered trademark of Digital Equipment Corporation
* DIGITAL is a registered trademark of Digital Equipment Corporation
* X Window System is a trademark of the Massachusetts Institute of Technology
*
*******************************************************************************
******************************************************************************/
#ifndef __DtosP_h
#define __DtosP_h

#ifndef NO_MEMMOVE
# ifndef X_NOT_STDC_ENV
#  include <stdlib.h> /* Needed for MB_CUR_MAX, mbtowc, mbstowcs and mblen */
# endif
#else
# define memmove( p1, p2, p3 )   bcopy( p2, p1, p3 )
#endif

#ifdef BOGUS_MB_MAX  /* some systems don't properly set MB_[CUR|LEN]_MAX */
# undef  MB_LEN_MAX
# define MB_LEN_MAX 1 /* temp fix */
# undef  MB_CUR_MAX
# define MB_CUR_MAX 1 /* temp fix */
#endif /* BOGUS_MB_MAX */

/**********************************************************************/
/* here we duplicate Xtos.h, since we can't include this private file */

#ifdef INCLUDE_ALLOCA_H
# include <alloca.h>
#endif

#ifdef CRAY
# define WORD64
#endif

/* stolen from server/include/os.h */
#ifndef NO_ALLOCA
/*
 * os-dependent definition of local allocation and deallocation
 * If you want something other than XtMalloc/XtFree for ALLOCATE/DEALLOCATE
 * LOCAL then you add that in here.
 */
# if defined(__HIGHC__)

#  if HCVERSION < 21003
#   define ALLOCATE_LOCAL(size)	alloca((int)(size))
#pragma on(alloca);
#  else /* HCVERSION >= 21003 */
#   define	ALLOCATE_LOCAL(size)	_Alloca((int)(size))
#  endif /* HCVERSION < 21003 */

#  define DEALLOCATE_LOCAL(ptr)  /* as nothing */

# endif /* defined(__HIGHC__) */


# ifdef __GNUC__
#  ifdef alloca
#  undef alloca
#  endif
#  define alloca __builtin_alloca
#  define ALLOCATE_LOCAL(size) alloca((int)(size))
#  define DEALLOCATE_LOCAL(ptr)  /* as nothing */
# else /* ! __GNUC__ */
/*
 * warning: mips alloca is unsuitable, do not use.
 */
#  if defined(vax) || defined(sun) || defined(stellar)
/*
 * Some System V boxes extract alloca.o from /lib/libPW.a; if you
 * decide that you don't want to use alloca, you might want to fix it here.
 */
char *alloca();
#   define ALLOCATE_LOCAL(size) alloca((int)(size))
#   define DEALLOCATE_LOCAL(ptr)  /* as nothing */
#  endif /* who does alloca */
# endif /* __GNUC__ */

#endif /* NO_ALLOCA */

#ifndef ALLOCATE_LOCAL
# define ALLOCATE_LOCAL(size) XtMalloc((unsigned long)(size))
# define DEALLOCATE_LOCAL(ptr) XtFree((XtPointer)(ptr))
#endif /* ALLOCATE_LOCAL */

/* End of Xtos.h */
/*****************/


/*
 * Default Icon Search Paths
 * 
 * The following are default starter values for XMICONSEARCHPATH and
 * XMICONBMSEARCHPATH, respectively.  Code elsewhere must ensure that paths
 * into the user's home directory occur in front of these paths in the
 * environment variables.  Note the apparently redundant use of ANSI C string
 * constant concatenation; this is necessary in order to avoid the sequence of
 * characters % B %, which form an SCCS id keyword.
 */

#define DTPMSYSDEFAULT						\
	CDE_CONFIGURATION_TOP "/appconfig/icons/%L/%B" "%M.pm:"	\
	CDE_CONFIGURATION_TOP "/appconfig/icons/%L/%B" "%M.bm:"	\
	CDE_CONFIGURATION_TOP "/appconfig/icons/%L/%B:"		\
								\
	CDE_CONFIGURATION_TOP "/appconfig/icons/C/%B" "%M.pm:"	\
	CDE_CONFIGURATION_TOP "/appconfig/icons/C/%B" "%M.bm:"	\
	CDE_CONFIGURATION_TOP "/appconfig/icons/C/%B:"		\
								\
	CDE_INSTALLATION_TOP  "/appconfig/icons/%L/%B" "%M.pm:"	\
	CDE_INSTALLATION_TOP  "/appconfig/icons/%L/%B" "%M.bm:"	\
	CDE_INSTALLATION_TOP  "/appconfig/icons/%L/%B:"		\
								\
	CDE_INSTALLATION_TOP  "/appconfig/icons/C/%B" "%M.pm:"	\
	CDE_INSTALLATION_TOP  "/appconfig/icons/C/%B" "%M.bm:"	\
	CDE_INSTALLATION_TOP  "/appconfig/icons/C/%B"

#define DTBMSYSDEFAULT						\
        CDE_CONFIGURATION_TOP "/appconfig/icons/%L/%B" "%M.bm:"	\
        CDE_CONFIGURATION_TOP "/appconfig/icons/%L/%B" "%M.pm:"	\
        CDE_CONFIGURATION_TOP "/appconfig/icons/%L/%B:"		\
								\
        CDE_CONFIGURATION_TOP "/appconfig/icons/C/%B" "%M.bm:"	\
        CDE_CONFIGURATION_TOP "/appconfig/icons/C/%B" "%M.pm:"	\
        CDE_CONFIGURATION_TOP "/appconfig/icons/C/%B:"		\
								\
        CDE_INSTALLATION_TOP  "/appconfig/icons/%L/%B" "%M.bm:"	\
        CDE_INSTALLATION_TOP  "/appconfig/icons/%L/%B" "%M.pm:"	\
        CDE_INSTALLATION_TOP  "/appconfig/icons/%L/%B:"		\
								\
        CDE_INSTALLATION_TOP  "/appconfig/icons/C/%B" "%M.bm:"	\
        CDE_INSTALLATION_TOP  "/appconfig/icons/C/%B" "%M.pm:"	\
        CDE_INSTALLATION_TOP  "/appconfig/icons/C/%B"


#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations ********/

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* __DtosP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */




