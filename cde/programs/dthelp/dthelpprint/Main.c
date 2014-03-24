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
#ifdef DOC
/*===================================================================
$FILEBEG$:   Main.c
$COMPONENT$: dthelpprint
$PROJECT$:   Cde1
$SYSTEM$:    HPUX 9.0; AIX 3.2; SunOS 5.3
$REVISION$:  $TOG: Main.c /main/6 1998/04/06 13:16:42 mgreess $
$CHGLOG$:    
$COPYRIGHT$:
   (c) Copyright 1993, 1994 Hewlett-Packard Company
   (c) Copyright 1993, 1994 International Business Machines Corp.
   (c) Copyright 1993, 1994 Sun Microsystems, Inc.
   (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
==$END$==============================================================*/
#endif /*DOC*/

/*
 * Draws several different width lines on the same y axis
 * to determine how the lines are drawn.
 */
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>

#include <Dt/Help.h>

#include "HelpPrintI.h"


/*======== default values ==============*/

/*======== helper values ===============*/
#define EOS             '\0'
#define EMPTY_STR	s_EmptyStr

#define MASET	6	/* message catalog set */

static char s_EmptyStr[1] = { EOS };


#if DOC
/*===================================================================
$FUNBEG$:  main()
$1LINER$:  main() routine of helpprint
$DESCRIPT$:
$WARNING$:
main() "knows" that we aren't using Xt in this program, and
sets the Display->db field explicitly.  That field is a Display 
structure member not used by X.  It is used by Xt; if Xt is used 
by dthelpprint, this part of the code must be changed.
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

int main(
    int		argc,
    char * *	argv)
{       /*$CODE$*/
   XrmDatabase   appDB = NULL;
   _DtHPrOptions options;
   Display *     dpy = NULL;
   int           helpType;
   int           status;

   /* init first option */
   options.programName = argv[0];

   /********************************************
      *	Read resources and open printer
      *****************************************/

   setlocale(LC_ALL,EMPTY_STR);  /* set to user's desired locale */

   _DtHPrBuildResourceDb(&argc, argv, &appDB, &dpy);
  
   _DtHPrGetResources(appDB,&options);
   
   if (dpy) 
      XrmSetDatabase(dpy,appDB); /* WARNING: Xt also uses the db member of the 
                                    Display structure.  if Xt is used by 
                                    dthelpprint, this part of the code must be 
                                    changed. */
  
   if (options.debugHelpPrint)
   {
      XrmPutFileDatabase(appDB,"db.dthelpprint"); /* dump rsrc for debug */
      system("/bin/cat db.dthelpprint|/bin/fgrep -i print");
   }

   /****************************************
     *	Evaluate helpType and activate printing
     *************************************/
   helpType = atoi(options.helpType);

   if (helpType == DtHELP_TYPE_STRING)
   {
      status = _DtHPrPrintStringData(dpy, &options);
      exit(status);                /* EXIT */
   }
   else if (helpType == DtHELP_TYPE_DYNAMIC_STRING)
   {
      status = _DtHPrPrintDynamicStringData(dpy, &options);
      exit(status);              /* EXIT */
   }
   else if (helpType == DtHELP_TYPE_MAN_PAGE)
   {
      status = _DtHPrPrintManPage(dpy, &options);
      exit(status);              /* EXIT */
   }
   else if (helpType == DtHELP_TYPE_FILE)
   {
      status = _DtHPrPrintHelpFile(dpy, &options);
      exit(status);              /* EXIT */
   }
   else if (helpType == DtHELP_TYPE_TOPIC)
   {
      status = _DtHPrPrintHelpTopic(dpy,&options);
      exit(status);                  /* EXIT */
   }
   else
   {
      fprintf(
	stderr,
	_DTGETMESSAGE(MASET, 1, "%s Error: Illegal helpType %d.\n"), 
	argv[0], helpType);
      exit(1);                       /* EXIT */
   }
   exit(1);                          /* EXIT */
} /*$END$*/

