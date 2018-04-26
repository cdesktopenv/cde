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
/* $TOG: HelposI.h /main/8 1998/07/28 15:38:41 mgreess $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   DtosI.h
 **
 **  Project:  Rivers Project, 
 **
 **  Description:  Internal header file for our Dtos.c module
 **  -----------
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _DtosI_h
#define _DtosI_h


#ifndef NO_MESSAGE_CATALOG
# define _DTGETMESSAGE(set, n, s) _DtHelpGetMessage(set, n, s)
#else
# define _DTGETMESSAGE(set, n, s) s
#endif


/*****************************************************************************
 * Function:	   Boolean _DtHelpOSGetHomeDirName(
 *
 *
 * Parameters:    Output string, size of output string buffer 
 *
 * Return Value:    String.
 *
 *
 * Description: 
 *
 *****************************************************************************/
extern void _DtHelpOSGetHomeDirName(
    String outptr,
    size_t len);

/*****************************************************************************
 * Function:       _DtHelpGetUserSearchPath(
 *
 *
 * Parameters:
 *
 * Return Value:    String, owned by caller.
 *
 *
 * Description:
 *                  Gets the user search path for use
 *                  when searching for a volume.
 *                  Takes path from the environment,
 *                  or uses the default path.
 *
 *****************************************************************************/
String _DtHelpGetUserSearchPath(void);


/*****************************************************************************
 * Function:       _DtHelpGetSystemSearchPath(
 *
 *
 * Parameters:
 *
 * Return Value:    String, owned by caller.
 *
 *
 * Description:
 *                  Gets the system search path for use
 *                  when searching for a volume.
 *                  Takes path from the environment,
 *                  or uses the default path.
 *
 *****************************************************************************/
String _DtHelpGetSystemSearchPath(void);



/*****************************************************************************
 * Function:	   Boolean _DtHelpGetMessage(
 *
 *
 * Parameters:     
 *
 * Return Value:   char *
 *
 *
 * Description:    This function will retreive the requested message from the
 *                 cache proper cache creek message catalog file.
 *
 *****************************************************************************/
extern char *_DtHelpGetMessage(
        int set,
        int n,
        char *s);


/*****************************************************************************
 * Function:	   char * _DtHelpGetLocale(
 *
 *
 * Parameters:     
 *
 * Return Value:   char *
 *
 *
 * Description:    Returns the value of LC_MESSAGES from the environ.
 *                 If that is NULL, returns the value of LANG form the environ.
 *                 If that is NULL, returns NULL.
 *
 *****************************************************************************/
extern char *_DtHelpGetLocale(void);


#endif /* _DtosI_h */
/* Do not add anything after this endif. */













