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

