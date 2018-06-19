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
/*****************************************************************************
 *
 *   File:         Parse.h
 *
 *   Project:	   CDE
 *
 *   Description:  This file contains the external function definitions
 *                 for the Front Panel keyword-value parsing routines.
 *
 *
 ****************************************************************************/


#ifndef _parse_h
#define _parse_h



extern Boolean StringToString (char *, void **);
extern Boolean StringToInt (char *, void **);
extern Boolean StringToBoolean (char *, void **);
extern Boolean StringToResolution (char *, void **);
extern Boolean StringToColorUse (char *, void **);
extern Boolean StringToControlBehavior (char *, void **);
extern Boolean StringToGeometry (char *, void **);
extern Boolean StringToAction (char *, void **);
extern Boolean StringToControlType (char *, void **);
extern Boolean StringToMonitorType (char *, void **);
extern Boolean StringToControlContainerType (char *, void **);
extern Boolean StringToPositionHints (char *, void **);
extern Boolean StringToFileName (char *, void **);
extern void FreeString (void **);
extern void FreeGeometry (void **);
extern void FreeAction (void **);




#endif /* _parse_h */
/*  DON"T ADD ANYTHING AFTER THIS #endif  */
