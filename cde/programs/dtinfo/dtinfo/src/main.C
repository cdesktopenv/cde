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
/* $XConsortium: main.C /main/25 1996/11/22 11:02:05 rcs $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1994, 1995, 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

# include "UAS.hh"

#include <locale.h>
#ifdef SVR4
#ifndef USL
#include <libintl.h>
#endif
#endif

#if defined(sparc) && defined(MAP_ZERO)
#include <sys/mman.h>
#include <fcntl.h>
#endif

#ifdef __osf__
/* Suppress unaligned access message */
#include <sys/types.h>
#include <sys/sysinfo.h>
#endif /* __osf__ */

#define C_WindowSystem
#define L_Other

#define C_MessageMgr
#define C_InputMgrX
#define C_LibraryMgr
#define C_EnvMgr
#ifdef UseSessionMgmt
#define C_SessionMgr
#endif
#ifdef UseTooltalk
#define C_TtIpcMgr
#endif
#define L_Managers

#define C_UrlAgent
#define L_Agents

#include "Prelude.h"

#include "Managers/CatMgr.hh"

#include "Exceptions.hh"
#include "utility/mmdb_exception.h"

#ifdef MONITOR
extern "C" {
  extern void monitor(int);
  int quantify_clear_data(void);
}  
#endif
extern "C" {
  int quantify_clear_data(void);
}  


#ifdef UseSessionMgmt
// Background task to explicitly save the initial session state.
// Need this work proc to run last on startup.
//
Boolean
SetSession_wp( XtPointer /*fiddler_on_the_roof*/ )
{
  session().set() ;			// explicitly save the state
  return (Boolean)True ;		// must always return True
}

// Background task to check for and restore any prior session state
//
Boolean
RestoreSession_wp( XtPointer /*fiddler_on_the_roof*/ )
{
  session().restore() ;			// check for saved state & process
  return (Boolean)True ;		// must always return True
}
#endif


#ifdef UseTooltalk
// Background task to initialize with tooltalk.
// Do before initial session state saved.
//
Boolean
SetTooltalk_wp( XtPointer /*fiddler_on_the_roof*/ )
{
  if( !env().secondary() )
            tt_manager().establish_server();
  return (Boolean)True ;		// must always return True
}
#endif


// Background task to launch explicit section requests at startup
//
Boolean
DisplayNode_wp( XtPointer locator )
{
  UrlAgent::document( (char *)locator, 1 );	// arg2 = force new window
  delete [] (char *)locator;

  return (Boolean)True ;			// must always return True
}

// this series will window-stack multiple documents with the
// first in the list displayed last, and thus on top
//
void
DisplayDocList( UAS_List<UAS_String> docs )
{
    UAS_String  doc_locator, start_doc, end_doc ;
    char *document;
    char *temp;

#ifdef UseSessionMgmt
  if( docs.length() != 0 )
  {
    // the presumption for now is that any special session state
    // info will exist only if there were node display windows... 
    XtAppAddWorkProc( window_system().app_context(),
                      RestoreSession_wp, (char *)NULL ) ;
  }
#endif

  // set up to display the documents

  for (int i = 0; i < docs.length(); i++)
  {
    (*(docs[i])).split( '-', start_doc, end_doc ) ;
    doc_locator = start_doc.length() ? start_doc : end_doc ;

    // only print cares about a range of locators;
    // for display, just do the first locator if a range

    // allocate memory for a normal c string to pass
    // through to the work proc

    document = new char[doc_locator.length()];
    temp = (char *)doc_locator;
    strcpy(document, temp);

    XtAppAddWorkProc( window_system().app_context(),
                      DisplayNode_wp, document) ;
  }
}


// PrintNode_wp
//
// Background task to launch explicit section requests at startup
//
Boolean
PrintNode_wp( XtPointer locator )
{
  UrlAgent::print_document( (char *)locator);
  delete [] (char *)locator;
  return (Boolean)True ;                        // must always return True
}


// PrintDocList will print a list of sections

void
PrintDocList( UAS_List<UAS_String> docs )
{
    UAS_String  doc_locator, start_doc, end_doc ;
    char *document;
    char *temp;
    
    for (int i = 0; i < docs.length(); i++)
    {
	(*(docs[i])).split( '-', start_doc, end_doc ) ;
	doc_locator = start_doc.length() ? start_doc : end_doc ;
	
	// only print cares about a range of locators;
	document = new char[doc_locator.length()];
	temp = (char *)doc_locator;
	strcpy(document, temp);
		
	// ... needs expansion of range here if present ...
	XtAppAddWorkProc( window_system().app_context(),
			  PrintNode_wp, document ) ;
    }
}

int
main(int argc, char **argv)
{
    INIT_EXCEPTIONS();
    
#if defined(sparc) && defined(MAP_ZERO)
    // to permit dtsearch to access address zero 
    mmap(NULL, 0x1000, PROT_READ, MAP_PRIVATE | MAP_FIXED,
	 open("/dev/zero", O_RDONLY), 0);
#endif
    
#ifdef MONITOR
    monitor(0);
#endif

#ifdef __osf__
/* Code to suppress unaligned access message. */
   unsigned long        op;
   int                  buffer[2];
   unsigned long        nbytes = 1;
   char*                arg = 0;
   unsigned long        flag = 0;

   int                  ssi_status;

   op = SSI_NVPAIRS;

   buffer[0] = SSIN_UACPROC;
   buffer[1] =  0x00000001;
#ifdef DEBUG_UAC
   buffer[1] |= 0x00000004;
#endif

   ssi_status = setsysinfo ( op, (caddr_t) buffer, nbytes, arg, flag );
#endif
    
    WindowSystem window_system (argc, argv);
    CatMgr msg_catalog_mgr;
    InputMgrX input_manager;
    if (env().init(argc, argv) < 0)
	exit(1);
    
    mtry
    {
	
	// don't set up for session management or tooltalk if
	// invoked for print only
	if (!window_system.videoShell()->print_only) {
#ifdef UseSessionMgmt
	    // set session state register to occur after startup has settled
	    XtAppAddWorkProc( window_system.app_context(),
			      SetSession_wp, (char *)NULL ) ;
#endif
#ifdef UseTooltalk
	    XtAppAddWorkProc( window_system.app_context(),
			      SetTooltalk_wp, (char *)NULL ) ;
#endif
	}
	
	// set up to process any explicit section display requests for
        // startup. If print only, print specified sections.
	UAS_List<UAS_String>env_sections( env().sections() );

	if (window_system.videoShell()->print_only) {
	    PrintDocList( env_sections );
	}
	else {
	    DisplayDocList( env_sections );
	}
	
	// request immediate loading of any/all infolibs specified
	UAS_List<UAS_String>env_infolibs( env().infolibs() );
	library_mgr().init( env_infolibs );
	
	window_system.run();
    }
    mcatch (Exception &, e)
    {
	char buffer[256];
	sprintf (buffer,
		 "Internal Error: Exception got away.\nFile: %s, Line: %d",
#ifdef C_API
		 e.file(), e.line());
#else
		 __FILE__, __LINE__);
#endif
	message_mgr().error_dialog (buffer);
	exit (1);
    }
    end_try;
    
    return (0);
}

#ifdef hpux

extern "C" {
void DosClose () { cerr << "DosClose\n"; }
void DosWrite () { cerr << "DosWrite\n"; }
void DosPeekNmPipe () { cerr << "DosPeekNmPipe\n"; }
void DosDisconnectNmPipe () { cerr << "DosDisconnectNmPipe\n"; }
void DosMakeNmPipe () { cerr << "DosMakeNmPipe\n"; }
void DosConnectNmPipe () { cerr << "DosConnectNmPipe\n"; }
void DosRead () { cerr << "DosRead\n"; }
}

#endif
