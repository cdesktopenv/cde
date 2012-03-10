/* $XConsortium: qualify.c /main/3 1995/10/27 16:14:33 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#include <stdio.h>
#include <string.h>

/*********************************************************************
 * qualifyWithFirst
 *
 * takes:   an unqualified filename like foo.txt, and
 *          a colon-separated list of pathnames, such as 
 *                /etc/opt/dt:/opt/dt/config
 *
 * returns: a fully qualified filename.  Space for the filename
 *          has been allocated off the heap.  It is the responsibility
 *          of the calling function to dispose of the space.
 **********************************************************************/

char * qualifyWithFirst
  (
  char * filename,
  char * searchPath
  )
{
char * paths = strdup(searchPath);
char * savepaths = paths;
char * path;
char * chance;
FILE * f;

  /* assert that the arguments cannot be NULL */

  if (filename == NULL || searchPath == NULL)
    return NULL;

  while (1) {

    /* if there is a :, zero it */

    if ((path = strchr(paths, ':')) != NULL)
      *path = 0;

    /* allocate space and create the qualified filename */

    chance = (char *)malloc(strlen(paths) + strlen(filename) + 2);
    sprintf(chance,"%s/%s",paths,filename);

    /* see if it is there by opening it for reading */

    if (f = fopen(chance,"r")) {

      /* it's there so close it, .... */

      fclose(f);

      /* ... restore the colon, .... */

      if (path)
	*path = ':';

      /* return the fully qualified filename */

      free(savepaths);
      return chance;
    }

    free(chance);

    /* reached the end of the list of paths */

    if (path == NULL)
      break;

    /* try the next path */

    paths = path + 1;
  }
  free(savepaths);
  return NULL;
}
