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
/****************************<+>*************************************
 ********************************************************************
 **
 **   File:    Environ_c.c
 **
 **   $XConsortium: Environ_c.c /main/6 1995/12/18 16:31:33 cde-hp $
 **
 **   Project:  libCliSrv Library
 **
 **   Description: Return the value of the DTUSERSESSION environ-
 **                ment variable or create it if it doesn't exist.
 **                If malloc fails, NULL is returned.
 **
 **(c) Copyright 1992,1993,1994 by Hewlett-Packard Company
 **(c) Copyright 1993,1994 International Business Machines Corp.
 **(c) Copyright 1993,1994 Sun Microsystems, Inc.
 **(c) Copyright 1993,1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 **
 ********************************************************************
 ****************************<+>*************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * _DtCliSrvGetDtUserSession()
{
  char * envVar = getenv("DTUSERSESSION");
  char * ret_envVar = NULL;

  /* See if the environment variable exists */

  if (envVar == NULL) {

    /* It doesn't, so it needs to be constructed. Use LOGNAME, which
     * always seems to be set, and DISPLAY, which may or may not be
     * set.
     */

    char pipedata[BUFSIZ];
    char logname_local[8];
    char * logname = getenv("LOGNAME");

    if (logname == NULL) {
      strcpy(logname_local,"generic");
      logname = logname_local;
    }

    /* determine DISPLAY and screen number */

    {
      char   screen[BUFSIZ];
      char * display = NULL;
      char * localDisplayVar = getenv("DISPLAY");

      if (localDisplayVar == NULL) {

	/* run uname to get the display name */

	FILE *pp;
	display = pipedata;

	pp = popen("uname -n", "r");
	if (NULL == pp) {
	  perror("uname -n");
	  return NULL;
	}
	*display = 0;
	fgets(display, BUFSIZ, pp);
	while (isspace(display[strlen(display)-1]))
	  display[strlen(display)-1] = 0;
	pclose(pp);
      }
      else {
	display = malloc(strlen(localDisplayVar) + 1);
	strcpy(display, localDisplayVar);
      }
	  

      /* Now determine the screen number. Throw away .0 */

      {
	char * s = strchr(display,':');
	if (s && strlen(s) < (size_t)BUFSIZ) {
	  strcpy(screen,s+1);
	  *s = 0;
	  if ((s = strchr(screen,'.')) && *(s+1) == '0')
	    *s = 0;
	}
	else {
	  strcpy(screen,"0");
	}
      }
      envVar = malloc(strlen(logname) + strlen(display) + strlen(screen) + 3);
      if (envVar)
	sprintf (envVar, "%s-%s-%s", logname, display, screen);

      return envVar;
    }
  }

  ret_envVar = malloc(strlen(envVar) + 1);
  if (ret_envVar)
    strcpy(ret_envVar, envVar);
  return ret_envVar;

}

#ifdef TEST
int main ()
{
  char * value;

  value = _DtCliSrvGetDtUserSession();

  printf("DTUSERSESSION will be set to: %s\n", value);

  free(value);

  printf("value has been freed\n");
}

/*******************************************************
  Test cases:  DTUSERSESSION   LOGNAME    DISPLAY
  -------------------------------------------------
                    set           -          -
		   unset       userfoo     unset
		   unset       userfoo   hostname:0
		   unset       userfoo   hostname:0.0
		   unset       userfoo   hostname:0.1
********************************************************/
#endif
