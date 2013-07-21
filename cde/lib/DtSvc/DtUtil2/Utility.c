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
/*
 * File:         Utility.c $XConsortium: Utility.c /main/5 1996/06/21 17:20:20 ageorge $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <stdio.h>
#include <sys/types.h>

#ifdef __hpux
#include <ndir.h>
#else
#if defined(sun) || defined(USL) || defined(sco) || defined(__uxp__)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#endif

#include <ctype.h>
#ifdef NLS16
#include <limits.h>
#endif
#include <sys/stat.h>
#include <sys/param.h>		/* MAXPATHLEN, MAXHOSTNAMELEN */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Dt/DtP.h>
#include <Dt/Connect.h>
#include <Dt/FileUtil.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Utility.h>
#include <Dt/UtilityP.h>
#include "DtSvcLock.h"

#include <string.h>

#define TRUE		1
#define FALSE		0


/********    Static Function Declarations    ********/

static char * RemapSpecialDisplayName(
                        char *dispInfo) ;

/********    End Static Function Declarations    ********/


/******************
 *
 * Function Name:  _DtVectorizeInPlace
 *
 * Description:
 *
 *	A "string vector" is an array of pointers to strings.  The
 *	end of the array is marked with a null pointer.  This function
 * 	takes a long string and creates a string vector from it by
 *	allocating the array of pointers, breaking the string up into
 *	sub-strings (based on a specified separator character), and
 *	making the array elements point to the sub-strings;
 *
 *	NOTE that it does this "in place".  If you don't want the original
 *	string disturbed, be sure to make a copy of it before calling
 *	this function.
 *
 *	The memory for the array of pointers is owned by the caller.
 *	It can be safely freed (though this doesn't free the sub-strings).
 *
 * Synopsis:
 *
 *	svector = _DtVectorizeInPlace (string, separator);
 *
 *	char **svector;		The string vector that was created.  The
 *				memory is owned by the caller.
 *	char *string;		A pointer to the string to be vectorized.
 *				THIS STRING WILL BE ALTERED.
 *	char separtor;		The separator character which marks the
 *				ends of the sub-strings.
 *
 ******************/

char * * 
_DtVectorizeInPlace(
        char *string,
        char separator )
{
/* LOCAL VARIABLES */
   
   char **vector, **next_string, *nextc;
   int num_pieces;
   
/* CODE */
   
   /* Count the elements in the string and allocate an appropriate size 
      vector.  There is one more element than separator characters. */
   num_pieces = 1;
   nextc = string;
   while ((nextc = DtStrchr(nextc, separator)))
   {
      num_pieces++;
      nextc++;
   }
   
   vector = (char **) XtMalloc ((Cardinal)(sizeof(char *) * (num_pieces + 1)));
   
   /* Set the first element of the vector to point to the start of
      the string. */
   *vector = string;
   next_string = vector + 1;
   nextc = string;

   /* Parse out each component, terminating it with a NULL */
   while ((nextc = DtStrchr(nextc, separator)))
   {
      *nextc = '\0';
      nextc++;
      *next_string = nextc;
      next_string++;
   }
	
   /* The last pointer in the vector must be set to NULL. */
   *next_string = NULL;
   return (vector);
}

/******************
 *
 * Function Name:  _DtFreeStringVector
 *
 * Description:
 *
 *	A "string vector" is an array of pointers to strings.  
 *
 * Synopsis:
 *
 *	_DtFreeStringVector (stringv);
 *
 *	char **stringv;		The string vector which is freed.
 *
 * NOTE:  this function assumes that "stringv" was created by
 *       "_DtVectorizeInPlace".
 *
 ******************/

void 
_DtFreeStringVector(
        char **stringv )
{

/* CODE */
   
   if (stringv)
   {
      if (stringv[0]) 
         XtFree ((char *)stringv[0]);

      XtFree ((char *)stringv);
   }
}


/*
 * Functions for mapping a display pointer into a display name.  It
 * special cases 'local:' and 'unix:', and maps these into the real
 * host name.
 */

static char *
RemapSpecialDisplayName(
        char *dispInfo )
{
   static char * localHost = NULL;
   char * name;

   _DtSvcProcessLock();
   if (localHost == NULL)
   {
      localHost = XtMalloc((Cardinal)30);
      DtGetShortHostname(localHost, 30);
   }
   _DtSvcProcessUnlock();

   name = XtMalloc((Cardinal)(strlen(localHost) + strlen(dispInfo) + 1));
   (void)strcpy(name, localHost);
   (void)strcat(name, dispInfo);
   return(name);
}

/*
 * Move here from DragUtil.c because the function is useful and DragUtil.c
 * is going away.
 */
char *
_DtGetDisplayName(
        Display *display )
{
   char * name = DisplayString(display);

   if (strncmp("unix:", name, 5) == 0)
      return(RemapSpecialDisplayName(name+4));
   else if (strncmp("local:", name, 6) == 0)
      return(RemapSpecialDisplayName(name+5));
   else if (strncmp(":", name, 1) == 0)
      return(RemapSpecialDisplayName(name));

   return(XtNewString(name));
}

