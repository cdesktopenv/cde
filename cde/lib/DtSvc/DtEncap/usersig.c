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
 * File:         usersig.c $XConsortium: usersig.c /main/4 1996/06/21 17:33:04 ageorge $
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#define __need_all_signals
#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */

#include <signal.h>
#include <stdint.h>

#include <bms/bms.h>
#include <bms/Symbolic.h>
#include <bms/usersig.h>
#include "DtSvcLock.h"

static XeSymTable Xe_sig_table = (XeSymTable) NULL;


/*-------------------------------------------------------------------------+*/
static void Xe_addsig(XeString name, int value)
/*-------------------------------------------------------------------------+*/
{
    XeSymbol sym = Xe_intern(Xe_sig_table, name);
    sym->value = (void *) (intptr_t) value;
}

/*-------------------------------------------------------------------------+*/
static void Xe_init_sig_table(void)
/*-------------------------------------------------------------------------+*/
{
#   define SIG_HASH_SIZE 32

    _DtSvcProcessLock();
    Xe_sig_table = Xe_new_symtab(SIG_HASH_SIZE);

    Xe_addsig((XeString)"SIGABRT", SIGABRT);          /* Add the XOPEN XPG3 signals */
    Xe_addsig((XeString)"SIGALRM", SIGALRM);
    Xe_addsig((XeString)"SIGFPE",  SIGFPE);
    Xe_addsig((XeString)"SIGHUP",  SIGHUP);
    Xe_addsig((XeString)"SIGILL",  SIGILL);
    Xe_addsig((XeString)"SIGINT",  SIGINT);
    Xe_addsig((XeString)"SIGKILL", SIGKILL);
    Xe_addsig((XeString)"SIGPIPE", SIGPIPE);
    Xe_addsig((XeString)"SIGQUIT", SIGQUIT);
    Xe_addsig((XeString)"SIGSEGV", SIGSEGV);
    Xe_addsig((XeString)"SIGTERM", SIGTERM);
    Xe_addsig((XeString)"SIGUSR1", SIGUSR1);
    Xe_addsig((XeString)"SIGUSR2", SIGUSR2);
    Xe_addsig((XeString)"SIGCHLD", SIGCHLD);
    Xe_addsig((XeString)"SIGCONT", SIGCONT);
    Xe_addsig((XeString)"SIGSTOP", SIGSTOP);
    Xe_addsig((XeString)"SIGTSTP", SIGTSTP);
    Xe_addsig((XeString)"SIGTTIN", SIGTTIN);
    Xe_addsig((XeString)"SIGTTOU", SIGTTOU);
    
    /* 0 is a valid XPG3 signal, but it doesn't have a name. */

    Xe_addsig((XeString)"ZERO", 0);
 
    /* Now add signals that may or may not be around on a given platform */    

#   ifdef SIGIO    
    Xe_addsig((XeString)"SIGIO", SIGIO);  		/*  hpux sun apollo */
#   else
#       ifdef SIGPOLL
           Xe_addsig((XeString)"SIGIO", SIGPOLL);  	/*  hpux sun apollo */
#       endif
#    endif

#   ifdef SIGPOLL    
    Xe_addsig((XeString)"SIGPOLL", SIGPOLL);  		/*  hpux sun apollo */
#   else
#       ifdef SIGIO
           Xe_addsig((XeString)"SIGPOLL", SIGIO);  	/*  hpux sun apollo */
#       endif
#   endif

#   ifdef SIGEMT    
    Xe_addsig((XeString)"SIGEMT", SIGEMT);  		/*  hpux sun apollo */
#   endif

#   ifdef SIGBUS    
    Xe_addsig((XeString)"SIGBUS", SIGBUS);  		/*  hpux sun apollo */
#   endif

#   ifdef SIGIOT    
    Xe_addsig((XeString)"SIGIOT", SIGIOT);  		/*  hpux sun apollo */
#   endif

#   ifdef SIGURG    
    Xe_addsig((XeString)"SIGURG", SIGURG);  		/*  hpux sun apollo */
#   endif

#   ifdef SIGSYS    
    Xe_addsig((XeString)"SIGSYS", SIGSYS);  		/*  hpux sun apollo */
#   endif

#   ifdef SIGTRAP    
    Xe_addsig((XeString)"SIGTRAP", SIGTRAP);  		/*  hpux sun apollo */
#   endif

#   ifdef SIGPROF    
    Xe_addsig((XeString)"SIGPROF", SIGPROF);  		/*  hpux sun apollo */
#   endif

#   ifdef SIGCLD    
    Xe_addsig((XeString)"SIGCLD", SIGCLD);  		/*  hpux sun apollo */
#   endif

#   ifdef SIGVTALRM    
    Xe_addsig((XeString)"SIGVTALRM", SIGVTALRM);  	/*  hpux sun apollo */
#   endif

#   ifdef SIGWINCH
          Xe_addsig((XeString)"SIGWINCH", SIGWINCH);
#   endif
    _DtSvcProcessUnlock();
}


/*-------------------------------------------------------------------------+*/
int XeNameToSignal(XeString name)
/*-------------------------------------------------------------------------+*/

/* Return SIG_NOT_IN_TABLE if signal name is not in hash table */

{
    XeSymbol sym;

    _DtSvcProcessLock();
    if (!Xe_sig_table) Xe_init_sig_table();

    sym = Xe_lookup(Xe_sig_table, name);

    _DtSvcProcessUnlock();
    return (sym) ? (intptr_t) sym->value : XE_SIG_NOT_IN_TABLE;
}

/*-------------------------------------------------------------------------+*/
XeString XeSignalToName(int sig)
/*-------------------------------------------------------------------------+*/
{
   switch (sig) 
   {
    /* These are XOPEN XPG3 signals */       
    /* ---------------------------- */
    case SIGABRT: 	return (XeString)"SIGABRT"; /* "SIGIOT" is an alias */
    case SIGALRM:	return (XeString)"SIGALRM";
    case SIGFPE:	return (XeString)"SIGFPE";
    case SIGHUP:	return (XeString)"SIGHUP";
    case SIGILL:	return (XeString)"SIGILL";
    case SIGINT:	return (XeString)"SIGINT";
    case SIGKILL:	return (XeString)"SIGKILL";
    case SIGPIPE:	return (XeString)"SIGPIPE";
    case SIGQUIT:	return (XeString)"SIGQUIT";
    case SIGSEGV:	return (XeString)"SIGSEGV";
    case SIGTERM:	return (XeString)"SIGTERM";
    case SIGUSR1:	return (XeString)"SIGUSR1";
    case SIGUSR2:	return (XeString)"SIGUSR2";
    case SIGCHLD:	return (XeString)"SIGCHLD"; /* "SIGCLD" is an alias */
    case SIGCONT:	return (XeString)"SIGCONT";
    case SIGSTOP:	return (XeString)"SIGSTOP";
    case SIGTSTP:	return (XeString)"SIGTSTP";
    case SIGTTIN:	return (XeString)"SIGTTIN";
    case SIGTTOU:	return (XeString)"SIGTTOU";

    /* 0 is a valid signal under XPG3, but it doesn't have a signal name */

    case 0:             return (XeString)"ZERO";


    /* NOTE: we have ifdef's for cases that are duplicated by other values   */
    /*       included but contain no code.  This is so when you compare this */
    /*       code to the stuff in Xe_init_sig_table() above, you see a one   */
    /*       to one correspondance.					     */
     
#   ifdef SIGIO    		 		/*  hpux sun apollo */
	                                        /*  (SIGPOLL, SIGTINT are aliases) */
    case SIGIO:		return (XeString)"SIGIO"; 
#   endif

#   ifdef SIGPOLL    				/*  hpux sun apollo */
/*  ----> Duplicated by SIGIO above
    case SIGPOLL:	return (XeString)"SIGPOLL";  	
*/
#   endif


#   ifdef SIGEMT    				/*  hpux sun apollo */
    case SIGEMT:	return (XeString)"SIGEMT";  	
#   endif

#   ifdef SIGBUS    				/*  hpux sun apollo */
    case SIGBUS:	return (XeString)"SIGBUS";  	
#   endif

#   ifdef SIGIOT    				/*  hpux sun apollo */
/*  ----> Duplicated by SIGABRT above

    case SIGIOT:	return (XeString)"SIGIOT";  	
*/
#   endif

#   ifdef SIGURG    				/*  hpux sun apollo */
    case SIGURG:	return (XeString)"SIGURG";  	
#   endif

#   ifdef SIGSYS    				/*  hpux sun apollo */
    case SIGSYS:	return (XeString)"SIGSYS";  	
#   endif

#   ifdef SIGTRAP    				/*  hpux sun apollo */
    case SIGTRAP:	return (XeString)"SIGTRAP";  	
#   endif

#   ifdef SIGPROF    				/*  hpux sun apollo */
    case SIGPROF:	return (XeString)"SIGPROF";  	
#   endif

#   ifdef SIGCLD    				/*  hpux sun apollo */
/*  ----> Duplicated by SIGCHLD above
    case SIGCLD:	return (XeString)"SIGCLD";  	
*/
#   endif

#   ifdef SIGVTALRM    				/*  hpux sun apollo */
    case SIGVTALRM:	return (XeString)"SIGVTALRM";  	
#   endif

#   ifdef SIGWINCH    				/*  sun apollo */
    case SIGWINCH:	return (XeString)"SIGWINCH";  	
#   endif


    default:		return XeString_NULL;
    }
}

