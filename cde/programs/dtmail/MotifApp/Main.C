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
/* $XConsortium: Main.C /main/5 1996/10/17 17:00:51 drk $ */
/*
 *+SNOTICE
 *
 *      $XConsortium: Main.C /main/5 1996/10/17 17:00:51 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//         This example code is from the book:
//
//           Object-Oriented Programming with C++ and OSF/Motif
//         by
//           Douglas Young
//           Prentice Hall, 1992
//           ISBN 0-13-630252-1	
//
//         Copyright 1991 by Prentice Hall
//         All Rights Reserved
//
//  Permission to use, copy, modify, and distribute this software for 
//  any purpose except publication and without fee is hereby granted, provided 
//  that the above copyright notice appear in all copies of the software.
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// Main.C: Generic main program used by all applications
//////////////////////////////////////////////////////////
#include "Application.h"
#include <assert.h>

// We can implement main() in the library because the 
// framework completely encapsulates all Xt boilerplate 
// and all central flow of control. 


#ifdef _AIX
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
static struct sigaction action;

void _signal( int Sig )
{
    if ( getenv( "_ILS_DEBUG_" ) ) {
        printf( "Got signal[%d]. Generate full core...\n", Sig );
	sigaction( SIGIOT, NULL, &action );
	action.sa_flags |= SA_FULLDUMP;
	action.sa_flags &= ~SA_PARTDUMP;
	sigaction( SIGIOT, &action, NULL );
	abort();
    }
}
#endif /* _AIX */


int main ( int argc, char **argv )
{

#ifdef _AIX
    //
    // With Defect 174851, a lot of codes are changed to make dtmail i18n'ze.
    // Especially, dtmail allocates the memory dynamically to convert the mail
    // body. Therefore, I believe no defects there, but it will cause the
    // potential coredump. If coredump will happen, to get the FULLCORE image,
    // please set // _ILS_DEBUG_ environment variable on, like
    //     export _ILS_DEBUG_=:
    //     dtmail
    // and do the same operation as that of when coredump will have occurred.
    // If the signal will be received, dtmail generates full core dump image.
    // Note: This core file size will be big. So be aware of "ulimit" of sh.
    //
    if ( getenv( "_ILS_DEBUG_" ) )
        (void)signal( SIGILL|SIGIOT|SIGKILL|SIGBUS|SIGSEGV, _signal );
#endif /* _AIX */

    // Make sure the programmer has remembered to 
    // instantiate an Application object
    
    assert ( theApplication != NULL ); 
    
    // Init Intrinsics, build all windows, and enter event loop
    
    theApplication->initialize ( &argc, argv );
    
    theApplication->handleEvents();
    return(0);
}
