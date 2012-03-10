/* $XConsortium: Resource.h /main/4 1995/10/30 13:12:06 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Resource.h
 **
 **   Project:     DT 3.0
 **
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _resource_h
#define _resource_h

/* External Interface */


extern void GetUserFontResource(int);
extern void GetSysFontResource(int);

extern void GetApplicationResources( void ) ;
extern void AddResourceConverters( void ) ;


#endif /* _resource_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
