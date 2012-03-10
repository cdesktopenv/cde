/*
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * $XConsortium: SmProperty.h /main/2 1996/02/08 11:28:28 barstow $
 */

/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmProperty.h
 **
 **  Project:     DT Session Manager (dtsession)
 **
 *****************************************************************************
 *************************************<+>*************************************/
#ifndef _smproperty_h
#define _smproperty_h

Status GetStandardProperties(
        Window 			window,
	int			screen,
        int 			*argc,			/* RETURNED */
        char 			***argv,		/* RETURNED */
        char 			**clientMachine,	/* RETURNED */
	Boolean 		*xsmpClient);		/* RETURNED */

#endif /*_smproperty_h */
