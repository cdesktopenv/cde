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
/* $XConsortium: dtksh.c /main/3 1995/11/01 15:53:19 rswiston $ */
/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */


#include "stdio.h"
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#define CONSTCHAR (const char *)
#define TRUE 1
#define FALSE 0

#ifndef DTKSHBINDIR
#define DTKSHBINDIR "/usr/bin"
#endif

static int
FileExists(dir, file)
char *dir, *file;
{
	struct stat sbuf;
	char path[1024];

	sprintf(path, "%s/%s", dir, file);
	return(stat(path, &sbuf) != -1);
}

/*
 * Bootstrap dtksh by calling xmcoeksh and forcing it to execute
 * an rc file that calls the dtksh init function and does some
 * other minor housekeeping.
 *
 * The rc file then sees if there was a previous user rc file (in $_HOLDENV_)
 * and if so executes it.
 */

int
main(argc, argv)
int argc;
char *argv[];
{
	char *env;
	char *bindir = NULL;
	char *executable, *envfile;
	char *buf;
	char envbuf[1024];

	/*
	 * Set the ENV variable to the standard dtksh rc file, which
	 * will do a libdirload of the dtksh shared object file and add all
	 * the dtksh commands and widgets to the exksh.  If the user already
	 * had an ENV file, then set the variable _HOLDENV_ to it so the
	 * standard dtksh rc file can execute it later.
	 */
	env = getenv((const char *)"ENV");
	buf = (char *)malloc((env ? strlen(env) : 0) + 12);
	strcpy(buf, "_HOLDENV_=");
	strcat(buf, env ? env : "");
	putenv(buf); 

	executable = "xmcoeksh";
	envfile = "xmcoeksh.rc";

	/*
	 * Search order for DTKSH binaries:
	 *
	 * 1. if $DTKSHBINDIR is set, use that path if it exists.
	 * 2. if the hard-wired #define DTKSHBINDIR is set and is a 
	 *    readable directory, use it.
	 * 3. punt.
	 */

	if ((bindir = getenv((const char *)"DTKSHBINDIR")) != NULL) {
 	   if ((!FileExists(bindir, executable)) ||
	       (!FileExists(bindir, envfile)))
           {
	      bindir = NULL;
           }
	}

	if (bindir == NULL) 
        {
           bindir = DTKSHBINDIR;

 	   if ((!FileExists(bindir, executable)) ||
	       (!FileExists(bindir, envfile)))
           {
	      bindir = NULL;
           }
	}

	if (bindir == NULL) {
  	   fprintf(stderr, 
                   "dtksh: bootstrap failed.  Unable to locate both\nxmcoeksh and xmcoeksh.rc in the same directory.\n Try setting $DTKSHBINDIR.\n");
	   exit(1);
	}
	sprintf(envbuf, "DTKSHBINDIR=%s", bindir);
	putenv(strdup(envbuf));

	sprintf(envbuf, "ENV=%s/%s", bindir, envfile);
	putenv(strdup(envbuf));
	sprintf(envbuf, "%s/%s", bindir, executable);
	execv(envbuf, argv);

	fprintf(stderr, "dtksh: Bootstrap of dtksh failed: '%s'\n", envbuf);
	perror("Reason");
	return(1);	/* failure */
}
