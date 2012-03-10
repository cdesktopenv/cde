/*
 * File:         bmsglob.c $XConsortium: bmsglob.c /main/4 1996/06/21 17:35:08 ageorge $
 * Language:     C
 *
 * (c) Copyright 1990, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifdef __osf__
/* #define SBSTDINC_H_NO_INCLUDE */
#define SBSTDINC_H_NO_REDEFINE
#endif

#include <bms/sbport.h>
#include <bms/bms.h>

char *XeToolClass = NULL;

/* --------- */
/* context.c */
/* --------- */
XeString        XeProgName = (XeString) "<unknown program name>";

/*-------------- */
/* SPC/spc-obj.c */
/*-------------- */
int SPC_Initialized=FALSE;

/*-------------- */
/* SPC/spc-error.c */
/*-------------- */
int XeSPCErrorNumber = 0;
FILE *spc_logF = (FILE*)NULL;

/*-------------- */
/* SPC/spc-env.c */
/*-------------- */
XeString spc_user_environment_file=XeString_NULL;

/*-------------- */
/* SPC/spc-proto.c */
/*-------------- */
FILE *SPC_Print_Protocol = (FILE*)NULL;
