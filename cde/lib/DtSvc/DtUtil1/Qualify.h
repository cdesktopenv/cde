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
 * File:         Qualify.h
 * RCS:          $XConsortium: Qualify.h /main/3 1995/10/26 15:10:03 rswiston $
 * Description:  Public header file for the Qualify routine
 * Project:      DT Runtime Library
 * Language:     C
 * Package:      N/A
 *
 *   (c) Copyright 1993 by Hewlett-Packard Company
 *
 *****************************************************************************/

/*********************************************************************
 * _DtQualifyWithFirst
 *
 * takes:   an unqualified filename like foo.txt, and
 *          a colon-separated list of pathnames, such as 
 *                /etc/dt:/usr/dt/config
 *
 * returns: a fully qualified filename.  Space for the filename
 *          has been allocated off the heap using malloc.  It is 
 *          the responsibility of the calling function to dispose 
 *          of the space using free.
 *
 * example: ...
 *          char * filename;
 *          ...
 *          filename = _DtQualifyWithFirst("configFile",
 *                          "/foo/first/location:/foo/second/choice");
 *          < use filename >
 *          free(filename);
 *
 **********************************************************************/

#ifndef _Dt_Qualify_h
#define _Dt_Qualify_h


#  ifdef __cplusplus
extern "C" {
#  endif

extern char * _DtQualifyWithFirst
  (
   char *,  /* file to locate */
   char *   /* list of colon-separated paths in which to look */
  );

#  ifdef __cplusplus
}
#  endif

#endif /* _Dt_Qualify_h */

/* DON'T ADD ANYTHING AFTER THIS #endif */
