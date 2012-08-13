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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $TOG: TermPrimOSDepI.h /main/4 1998/03/16 14:41:55 mgreess $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */
/******************************************************************************

    this is a list of OS dependent defines that are available...

    XOR_CAPS_LOCK		xor caps lock and shift so that lock+shift
				acts as a noop for alpha keys
    NO_MESSAGE_CATALOG		enables the message catalog code
    USE_TIOCCONS		use TIOCCONS iocto for -C option
    USE_SRIOCSREDIR		use SRIOCSREDIR ioctl for -C option
    SETENV_LINES_AND_COLS	set $LINES and $COLUMNS
    HAS_SETRESUID		os supports setresuid(2) - first choice
    HAS_SETEUID			os supports seteuid(2) - second choice
    HAS_SETREUID		os supports setreuid(2) - second choice
    USE_STREAMS			use streams as opposed to ptys - first choice
    USE_STREAMS_TTCOMPAT	use ttcompat streams mod
    USE_STREAMS_BUFMOD		use bofmod streams mod
    USE_PTYS			use ptys - second choice
    USE_CSWIDTH			use the csWidth resource to initialize
				multi-byte processing in ldterm
    USE_SETCSMAP		use setcsmap() for multi-byte processing
    USE_SUN_WCWIDTH_PATCH	use SUN wcwidth() workaround
    USE_TIOCBREAK		use TIOCBREAK ioctl() to send RS232 break
    USE_TCSBRK			use TCBRK ioctl() to send RS232 break
    USE_TCSBREAK		use TCBREAK ioctl() to send RS232 break
    USE_TCSENDBREAK		use tiocbreak() to send RS232 break

******************************************************************************/

#ifndef	_Dt_TermPrimOSDepI_h
#define	_Dt_TermPrimOSDepI_h

#ifdef  ALPHA_ARCHITECTURE
# define USE_TIOCCONS            /* use tioccons for -C          */
# define HAS_SETEUID             /* seteuid available            */
# define HAS_SETREUID            /* setreuid available           */
# define USE_PTYS                /* use ptys                     */
# define USE_TCSBRK              /* use TCSBRK ioctl()           */
#endif /* ALPHA_ARCHITECTURE */

#ifdef	HP_ARCHITECTURE
# define XOR_CAPS_LOCK		/* xor caps lock and shift	*/
# define USE_TIOCCONS		/* use tioccons for -C		*/
# define SETENV_LINES_AND_COLS	/* set $LINES and $COLUMNS	*/
# define HAS_SETRESUID		/* setresuid available		*/

# if OSMAJORVERSION > 9
#  define USE_STREAMS		/* use streams			    */
#  define USE_CSWIDTH		/* use the csWidth resource to      */
			        /* initialize multi-byte processing */
				/* in ldterm                        */
#  define USE_TCSENDBREAK	/* use tiocbreak()		*/
# else	/* OSMAJORVERSION > 9 */
#  define USE_PTYS		/* use ptys			*/
#  define USE_TIOCBREAK		/* use TIOCBREAK ioctl()	*/
# endif	/* OSMAJORVERSION > 9 */
#endif /* HP_ARCHITECTURE */

#ifdef  LINUX_ARCHITECTURE
# define USE_TIOCCONS            /* use tioccons for -C          */
# define HAS_SETEUID             /* seteuid available            */
# define HAS_SETREUID            /* setreuid available           */
# define USE_TCSENDBREAK	/* use tiocbreak()		*/
#endif /* LINUX_ARCHITECTURE */

#ifdef  CSRG_BASED
# define HAS_SETEUID             /* seteuid available            */
# define HAS_SETREUID            /* setreuid available           */
# define USE_TCSENDBREAK	/* use tiocbreak()		*/
#endif /* CSRG_BASED */

#ifdef	SUN_ARCHITECTURE
# define XOR_CAPS_LOCK		/* xor caps lock and shift	*/
# define USE_SRIOCSREDIR	/* use SRIOCSREDIR ioctl for -C	*/
# define USE_STREAMS		/* use streams			*/
# define USE_STREAMS_TTCOMPAT	/* use ttcompat streams mod	*/
# define USE_STREAMS_BUFMOD	/* use bofmod streams mod	*/
# define USE_TCSENDBREAK	/* use tiocbreak()		*/
# if (_XOPEN_VERSION == 3)
#  define USE_SUN_WCWIDTH_PATCH
#  define wcwidth(w)  sun_wcwidth(w)
# endif /* (_XOPEN_VERSION == 3) */
#endif /* SUN_ARCHITECTURE */

#ifdef	IBM_ARCHITECTURE
/* this seems to be #define'ed in the world of imake... */
# define USE_TIOCCONS		/* use tioccons for -C		*/
# define HAS_SETEUID		/* seteuid available		*/
# define HAS_SETREUID		/* setreuid available		*/
# define USE_PTYS		/* use ptys			*/
# define USE_SETCSMAP		/* use setcsmap() for multi-byte*/
# define USE_TCSENDBREAK	/* use tiocbreak()		*/
#endif /* IBM_ARCHITECTURE */

#ifdef	USL_ARCHITECTURE
# define XOR_CAPS_LOCK		/* xor caps lock and shift	*/
# define USE_STREAMS		/* use streams			*/
# define USE_STREAMS_TTCOMPAT	/* use ttcompat streams mod	*/
# define USE_TCSENDBREAK	/* use tiocbreak()		*/
#endif	/* USL_ARCHITECTURE */

#ifdef	UXP_ARCHITECTURE
# define XOR_CAPS_LOCK		/* xor caps lock and shift	*/
# define USE_STREAMS		/* use streams			*/
# define USE_STREAMS_TTCOMPAT	/* use ttcompat streams mod	*/
# define USE_TCSENDBREAK	/* use tiocbreak()		*/
#endif	/* UXP_ARCHITECTURE */

#endif	/* _Dt_TermPrimOSDepI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
