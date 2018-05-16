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
/* $XConsortium: config.h /main/4 1996/06/11 17:36:26 cde-hal $ */

#ifndef _config_h
#define _config_h 1

#ifdef OLIAS_LITTLE_ENDIAN
# define MMDB_LITTLE_ENDIAN  // i386
#else
# ifdef OLIAS_BIG_ENDIAN
#  define MMDB_BIG_ENDIAN     // sun, rs/6000, hp
# else
#  ifdef OLIAS_DEFAULT_ENDIAN
#   ifdef MMDB_BIG_ENDIAN
#   undef MMDB_BIG_ENDIAN
#   endif
#   ifdef MMDB_LITTLE_ENDIAN
#   undef MMDB_LITTLE_ENDIAN
#   endif
#  endif
# endif
#endif

#endif

