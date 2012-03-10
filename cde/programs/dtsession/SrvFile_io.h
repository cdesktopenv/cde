/* $XConsortium: SrvFile_io.h /main/4 1995/10/30 09:40:10 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SrvFile_io.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   File I/O for the color server
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _srvfile_h
#define _srvfile_h
 
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
 *  External variables  
 */


/*  
 *  External Interface  
 */



extern void SwitchAItoPS( 
                            struct _palette *) ;
extern struct _palette * GetPaletteDefinition( 
                            Display *dpy,
                            int     screen_number,
                            char    *palette) ;


#endif /*_srvfile_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
