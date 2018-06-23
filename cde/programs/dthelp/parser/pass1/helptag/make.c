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
/* $XConsortium: make.c /main/3 1995/11/08 10:10:53 rswiston $ */
/*   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */
/* Functions for pseudo-make features of HP Tag/TeX translator, i.e.,
   for testing which generated files are current and which must be
   recreated.  */
/*
   NOTE : This file is used as a Microsoft Linker overlay, so it cannot
   have functions as pointers, eg. (proc *) f().  Routines that are
   or invoke function pointers should be in tex.c.
*/

#if defined(hpux) || defined(_AIX) || defined(sun)
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <math.h>

#if defined(hpux) || defined(_AIX) || defined(sun)
#undef M_PI  /* M_PI is used by markup, we don't need the math PI */
#endif

#include "userinc.h"
#include "globdec.h"

/* Obsolete */
