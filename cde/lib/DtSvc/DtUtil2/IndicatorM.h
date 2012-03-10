/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*****************************<+>*************************************
 *********************************************************************
 **
 **   File:     IndicatorM.h
 **
 **   RCS:	$XConsortium: IndicatorM.h /main/3 1995/10/26 15:23:53 rswiston $
 **   Project:  DT 
 **
 **   Description: Defines indicator messages
 **
 **   (c) Copyright 1990 by Hewlett-Packard Company
 **
 *********************************************************************
 *****************************<+>*************************************/
#ifndef _IndicatorM_h
#define _IndicatorM_h

/*
 * BMS Messaging definitions
 */

/* _DtMessage for turning on the activity indicator */

#define DtACTIVITY_NOTIFICATION	"ACTIVITY_NOTIFICATION"

/* _DtMessage for turning off the activity indicator */

#define DtACTIVITY_DONE_NOTIFICATION	"ACTIVITY_DONE_NOTIFICATION"

#endif /* _IndicatorM_h */
/* Do not add anything after this endif. */
