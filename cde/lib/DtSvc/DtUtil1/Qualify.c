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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     Qualify.c
 **
 **   RCS:	$XConsortium: Qualify.c /main/3 1995/10/26 15:09:47 rswiston $
 **
 **   Project:  DT
 **
 **   Description: Fully qualify a file with the first path found
 **                in a list of colon-separated paths
 **
 **   (c) Copyright 1993 by Hewlett-Packard Company
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

char * _DtQualifyWithFirst
  (
  char * filename,
  char * searchPath
  )
{
char * paths = searchPath;
char * path;
char * chance;
FILE * f;

  /* assert that the arguments cannot be NULL and cannot be empty */

  if (filename == NULL || searchPath == NULL || 
      filename[0] == 0 || searchPath[0] == 0)
    return NULL;

  while (1) {

    /* if there is a :, zero it */

    if ((path = strchr(paths, ':')) != NULL)
      *path = 0;

    /* allocate space and create the qualified filename */

    chance = (char *)malloc(strlen(paths) + strlen(filename) + 2);
    if (filename[0] == '/')
      sprintf(chance,"%s%s",paths,filename);
    else
      sprintf(chance,"%s/%s",paths,filename);

    /* see if it is there by opening it for reading */

    if (f = fopen(chance,"r")) {
      fclose(f);                    /* it's there so close it, .... */
      if (path)                     /* ... restore the colon, .... */
	*path = ':';
      return chance;                /* return the fully qualified filename */
    }

    free(chance);
    if (path == NULL)               /* reached the end of the list of paths */
      break;
    *path = ':';                    /* restore the colon */
    paths = path + 1;               /* try the next path */
  }
  return NULL;
}

    
