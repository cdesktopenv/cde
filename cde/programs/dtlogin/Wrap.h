/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*
 * @DEC_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log$
 * Revision 1.1.2.2  1995/04/21  13:05:12  Peter_Derr
 * 	dtlogin auth key fixes from deltacde
 * 	[1995/04/14  18:03:37  Peter_Derr]
 *
 * 	Copy for use by dtlogin.
 * 	[1995/04/10  16:52:11  Peter_Derr]
 *
 * Revision 1.1.3.2  1994/07/08  21:01:55  Peter_Derr
 * 	Hide names for silly export rule.
 * 	[1994/05/18  17:57:54  Peter_Derr]
 * 
 * $EndLog$
 */
/* $XConsortium: Wrap.h /main/2 1996/01/29 18:00:15 mgreess $ */
/*
 * header file for compatibility with something useful
 */

typedef unsigned char auth_cblock[8];	/* block size */

typedef struct auth_ks_struct { auth_cblock _; } auth_wrapper_schedule[16];

extern void _XdmcpWrapperToOddParity();

#ifdef SILLYEXPORTRULE
#define _XdmcpAuthSetup _xX_a1
#define _XdmcpAuthDoIt _xX_b2
#endif
