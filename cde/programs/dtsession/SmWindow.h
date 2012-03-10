/* $XConsortium: SmWindow.h /main/4 1995/10/30 09:39:07 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmWindow.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Getting Top Level Window Info for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smwindow_h
#define _smwindow_h
 
/* 
 *  #include statements 
 */


/* 
 *  #define statements 
 */

/* 
 * typedef statements 
 */

/*
 * Structure to hold window and state information to be later written
 * to the output state file
 */

typedef struct
{
    int		termState;
    Window	wid;
} WindowInfo, *WindowInfoPtr;

/*
 * Information returned by application in response to WM_STATE message
 */
typedef struct
{
  int state;
  Window icon;
} WM_STATE;

/*
 *  External variables  
 */


/*  
 *  External Interface  
 */


extern int SaveYourself( WindowInfo ) ;
extern int GetTopLevelWindows( int, WindowInfo **, unsigned int *, 
			       unsigned int *) ;



#endif /*_smwindow_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
