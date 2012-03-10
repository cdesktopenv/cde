/* $XConsortium: Parse.h /main/4 1995/11/01 11:28:34 rswiston $ */
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
