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
/* $XConsortium: udccom.h /main/5 1996/07/19 20:31:51 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include "udcopt.h"

/* default path of dtaddcpf */
#define DFLTPATH 	"/usr/dt/lib/dtudcfonted/dtaddcpf"
#define DFLTCMD		"dtaddcpf"

/* commands for exchange font format */
#define SNFTOBDF_CMD	"dtgpftobdf"
#define BDFTOPCF_CMD	"bdftopcf"

/* default path for commands */
#define SNFTOBDF	"/usr/dt/lib/dtudcfonted/dtgpftobdf"
#ifndef FONTC
#define BDFTOPCF        "/usr/bin/X11/bdftopcf"
#else
#define BDFTOPCF        FONTC
#endif
#define	PCFTOBDF	SNFTOBDF

/* suffix of font file */
#define BDFSUFFIX	".bdf"
#define PCFSUFFIX	".pcf"

/* font information file */
#define DTUDCFONTS		"DTUDCFONTS"

#ifndef DTUDCFONTSLISTDIR
#define	DTUDCFONTSLISTDIR	"/usr/dt/config"
#endif

/* default font path */
#ifndef	DTUDCFONTPATH
#define	DTUDCFONTPATH		"/usr/lib/X11/fonts/misc"
#endif

/* defines for UDC fonts */
#define	FONTS_LIST_FILE		"fonts.list"
#define	UDFLIST_DEF		"udflist.def"

#define	FONTSDIR		"fonts.dir"
