/* $XConsortium: dtinfo_start.h /main/4 1996/09/14 14:52:13 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */
#ifndef _dtinfo_start_h_
#define _dtinfo_start_h_

/*
 * macro to get message catalog strings
 */
#ifndef NO_MESSAGE_CATALOG
# ifdef __ultrix
#  define _CLIENT_CAT_NAME "dtinfo_start.cat"
# else  /* __ultrix */
#  define _CLIENT_CAT_NAME "dtinfo_start"
# endif /* __ultrix */

extern char *_DtGetMessage (char *filename, int set_num, int message_num, char *message);

# define GETMESSAGE(set, number, string)\
    (_DtGetMessage(_CLIENT_CAT_NAME, set, number, string))
#else /* NO_MESSAGE_CATALOG */
# define GETMESSAGE(set, number, string)\
    string
#endif /* NO_MESSAGE_CATALOG */

#endif /* _dtinfo_start_h_ */
