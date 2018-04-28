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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: CoEd.C /main/3 1995/10/20 17:04:04 rswiston $ 			 				
/*
 * CoEd.cc
 *
 * Copyright (c) 1991,1993 by Sun Microsystems.
 */

#include <stdlib.h>
#include <unistd.h>
#include <Tt/tttk.h>
#include <CoEd.h>
#include "CoEditor.h"
#include "CoEdTextBuffer.h"

XtAppContext	myContext;
Widget		myTopWidget	= 0;
Display	       *myDpy;
char	       *myAltMediaType	= "ISO_Latin_1";
int		abortCode	= 0;
Tt_pattern     *sessPats	= 0;
int		timeOutFactor	= 1000;
int		maxBuffers	= 1000;
int	       *pArgc;
char	      **globalArgv;

const char     *ToolName	= "CoEd";
const char     *usage 		=
"Usage: CoEd [-p01] [-w n] [-t n] [-m media_type] [file]\n"
"	-p	print ToolTalk procid\n"
"	-0	do not open an initial composition window\n"
"	-1	be a single-buffer editor\n"
"	-w	sleep for n seconds before coming up\n"
"	-t	use n as timeout factor, in milliseconds (default: 1000)\n"
"	-m	set the alternate media type (default: ISO_Latin_1)\n"
;

void
main(
	int    argc,
	char **argv
)
{
	static const char *here = "main()";
	int   delay   = 0;
	int   printid = 0;
	int   daemon  = 0;
	char *file    = 0;

	XtToolkitInitialize();
	myContext = XtCreateApplicationContext();
	//
	// This display may get closed, and another opened, inside
	// CoEditor::_init(), if e.g. our parent is on a different screen
	//
	pArgc = &argc;
	globalArgv = argv;
	myDpy = XtOpenDisplay( myContext, 0, 0, "CoEd", 0, 0, &argc, argv );

	int c;
	while ((c = getopt( argc, argv, "p01w:t:m:" )) != -1) {
		switch (c) {
		    case 'p':
			printid = 1;
			break;
		    case '0':
			daemon = 1;
			break;
		    case '1':
			maxBuffers = 1;
			break;
		    case 'w':
			delay = atoi( optarg );
			break;
		    case 't':
			timeOutFactor = atoi( optarg );
			break;
		    case 'm':
			myAltMediaType = optarg;
			break;
		    default:
			fputs( usage, stderr );
			exit( 1 );
		}
	}
	if (optind < argc) {
		file = argv[ optind ];
	}
	while (delay > 0) {
		sleep( 1 );
		delay--;
	}

	int myTtFd;
	char *myProcID = ttdt_open( &myTtFd, ToolName, "SunSoft", "%I", 1 );
	Tt_status status = tt_ptr_error( myProcID );
	if (status != TT_OK) {
		fprintf( stderr, "ttdt_open(): %s\n",
			 tt_status_message(status) );
		myProcID = 0;
	} else {
		if (printid) {
			printf( "%s\n", myProcID );
		}

		XtAppAddInput( myContext, myTtFd, (XtPointer)XtInputReadMask,
			       tttk_Xt_input_handler, myProcID );

		ttmedia_ptype_declare( "Sun_CoEd", 0, CoEditor::loadISOLatin1_,
				       (void *)&myTopWidget, 1 );

		// Process the message that started us, if any
		tttk_Xt_input_handler( 0, 0, 0 );
		if (abortCode != 0) {
			// Error in message that caused us to start.
			exit( abortCode );
		}
	}

	if (CoEditor::numEditors == 0) {
		// started by hand, not by ToolTalk
		if (file == 0) {
			if (! daemon) {
				new CoEditor( &myTopWidget );
			}
		} else {
			new CoEditor( &myTopWidget, file );
		}
	}
	//
	// If sessPats is unset, then we have no joined the desktop
	// session yet.  So join it.
	//
	if (sessPats == 0) {
		Widget session_shell = CoEditor::editors[0]->shell;
		if (maxBuffers > 1) {
			//
			// In multi-window mode, no single window is the
			// distinguished window.
			//
			session_shell = myTopWidget;
		}
		sessPats = ttdt_session_join( 0, CoEditor::contractCB_,
				session_shell, CoEditor::editors[0], 1 );
	}

	while (daemon || (CoEditor::numEditors > 0)) {
		XEvent event;
		XtAppNextEvent( myContext, &event );
		XtDispatchEvent( &event );
	}
	ttdt_close( 0, 0, 1 );
}
