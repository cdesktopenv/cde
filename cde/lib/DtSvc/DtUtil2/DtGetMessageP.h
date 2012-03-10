/* $TOG: DtGetMessageP.h /main/5 1998/07/30 12:14:37 mgreess $
 *
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1995 Hewlett-Packard Company.
 * (c) Copyright 1995 International Business Machines Corp.
 * (c) Copyright 1995 Sun Microsystems, Inc.
 * (c) Copyright 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * DtGetMessage.h - Interfaces for the DtSvc library's private message 
 *    catalog APIs
 */

#ifndef _DtGetMessage_h
#define _DtGetMessage_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 * External declarations
 */
extern char *Dt11GetMessage (
        char 		*filename,
        int 		set,
        int 		number,
        char 		*string);

/*
 * Dt11GETMESSAGE macro
 */
#ifndef NO_MESSAGE_CATALOG
# define _MESSAGE_CAT_NAME       "dt"
# define Dt11GETMESSAGE(set, number, string)\
	Dt11GetMessage(_MESSAGE_CAT_NAME, set, number, string)
#else
# define Dt11GETMESSAGE(set, number, string)\
    	string
#endif

#ifdef __cplusplus
}
#endif

#endif /* _DtGetMessage_h */
