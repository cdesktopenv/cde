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
