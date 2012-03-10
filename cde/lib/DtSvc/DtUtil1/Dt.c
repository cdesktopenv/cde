/* $XConsortium: Dt.c /main/4 1995/12/04 10:04:04 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*********************************************************************
 *
 *	File:		Dt.c
 *
 *	Description:	Repository for libDtSvc-wide information.
 *
 *********************************************************************/

#include <Dt/Dt.h>

/* CDE Version information */

externaldef(dtversion) const int DtVersion = DtVERSION_NUMBER;
externaldef(dtversionstring) const char* DtVersionString = DtVERSION_STRING;


