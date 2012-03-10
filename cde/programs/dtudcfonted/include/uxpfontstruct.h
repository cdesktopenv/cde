/* $XConsortium: uxpfontstruct.h /main/1 1995/09/14 20:52:27 cde-fuj2 $ */
/*
 *  All Rights Reserved, Copyright (c) FUJITSU LIMITED 1995
 *
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 *
 *  Authors: Seiya Miyazaki     FUJITSU LIMITED
 *           Hiroyuki Chiba     FUJITSU LIMITED
 *
 */

#ifndef FEFONTSTRUCT_H
#define FEFONTSTRUCT_H 1

/* 
 * for use in version
 */

#define FE_PREFIXS	"FEF/"
#define FE_PREFIXL	4


/*
 * for use in permission.
 */

#define FE_DISPLAY_FONT	0x01
#define FE_PRINTER_FONT	0x02

/*
 * uxp Extended Font Structure
 */

typedef struct _FeFontInfo {
    char		version[32];	/* version */
    char		copyright[64];	/* Copyright */
    unsigned int	permission;	/* purpose for use */ 
    char		reserve[24];
    int			privatesize;	/* size of private area */
} FeFontInfoRec;

typedef struct _FeFontInfo *FeFontInfoPtr;

#endif /* FEFONTSTRUCT_H */
