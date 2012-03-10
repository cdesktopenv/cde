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
/* $XConsortium: hpversion.h /main/5 1996/08/30 15:22:49 drk $ */
/* -*-C-*-
****************************************************************
*/
/* The following macro should be invoked once by each of the X11 components
   to provide standard revision information. */
#ifndef	lint
#define	version_tag(FILE_VERSION) \
static char _hp_merge_version[] = "@(#)" FILE_VERSION;
#else	/* lint */
#define	version_tag(FILE_VERSION)
#endif	/* lint */

/* Any revision information other than the standard 'hpversion' macro shoud
   be conditionally compiled using the VERBOSE_REV_INFO constant so that it
   can be excluded for official X11 releases. To set VERBOSE_REV_INFO, comment
   out one of the following 2 blocks. */

/* This block turns on VERBOSE_REV_INFO */
/*
#ifndef lint
#define VERBOSE_REV_INFO  1
#else
#ifdef VERBOSE_REV_INFO
#undef VERBOSE_REV_INFO
#endif
#endif
*/

/* This block turns off VERBOSE_REV_INFO */
#ifdef VERBOSE_REV_INFO		
#undef VERBOSE_REV_INFO
#endif
