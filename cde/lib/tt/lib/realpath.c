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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: realpath.c /main/3 1995/10/23 09:49:50 rswiston $ 			 				 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <strings.h>		
#include <errno.h>

/* This is used when the operating system  does not  provide us  with
 * a realpath implementation.  It's named differently so there is
 * no name clash between it and the system version of realpath.
 */

char *
_tt_internal_realpath(pathname, finalpath)
char *pathname;
char *finalpath;
{
  struct stat sbuf;
  char curpath[MAXPATHLEN],
  workpath[MAXPATHLEN],
  linkpath[MAXPATHLEN],
  namebuf[MAXPATHLEN],
  *where,
  *ptr,
  *last;
  int len;
  int nlink = 0;	/* To keep track of loops in the path. */

   strcpy(curpath, pathname);

   if (*pathname != '/') {
#if defined(__STDC__) && defined(hpux)
      if (!getcwd(workpath)) {
#else
      if (!getwd(workpath)) {	
#endif
      strcpy(finalpath, ".");
         return(NULL);
       } 
    } else *workpath = NULL;

   /* curpath is the path we're still resolving      */
   /* linkpath is the path a symbolic link points to */
   /* workpath is the path we've resolved            */

 loop:
   where = curpath;
   while (*where != NULL) {
      if (!strcmp(where, ".")) {
         where++;
         continue;
       }

      /* deal with "./" */
      if (!strncmp(where, "./", 2)) {
         where += 2;
         continue;
       }

      /* deal with "../" */
      if (!strncmp(where, "../", 3)) {
         where += 3;
         ptr = last = workpath;
         while (*ptr) {
            if (*ptr == '/') last = ptr;
            ptr++;
	  }
         *last = NULL;
         continue;
       }

      ptr = strchr(where, '/');
      if (!ptr)
         ptr = where + strlen(where) - 1;
      else
         *ptr = NULL;

      strcpy(namebuf, workpath);

      for (last = namebuf; *last; last++) continue;

      /* tack on a trailing, or leading, `/` */
      if (last == namebuf || *--last != '/') {
      	strcat(namebuf, "/");
      }

      strcat(namebuf, where);

      where = ++ptr;
      if (lstat(namebuf, &sbuf) == -1) {
         strcpy(finalpath, namebuf);
         return(NULL);
       }
      
      if ((sbuf.st_mode & S_IFLNK) == S_IFLNK) {
	 /* Look for loop in path such as link to self. */
	 nlink++;			 
	 if (nlink > MAXSYMLINKS) {
		 errno = ELOOP;
		 return (NULL);
	 }
         len = readlink(namebuf, linkpath, MAXPATHLEN);
         if (len == 0) {
            strcpy(finalpath, namebuf);
            return(NULL);
	  }
         *(linkpath + len) = NULL; /* readlink doesn't null-terminate result */
         if (*linkpath == '/') *workpath = NULL;
         if (*where) {
            strcat(linkpath, "/");
            strcat(linkpath, where);
	  }
         strcpy(curpath, linkpath);
         goto loop;
       }

      if ((sbuf.st_mode & S_IFDIR) == S_IFDIR) {
         strcpy(workpath, namebuf);
         continue;
       }

      if (*where) {
         strcpy(finalpath, namebuf);
         return(NULL);  /* path/notadir/morepath */
       } else
         strcpy(workpath, namebuf);
    }
   strcpy(finalpath, workpath);
   return(finalpath);

}

