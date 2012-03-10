// $XConsortium: NLS.hh /main/2 1996/05/09 20:05:27 drk $
/*
 *+SNOTICE
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

#ifndef _NLS_HH
#define _NLS_HH

char *catgets_cache2(nl_catd catd, int set, int num, char *dflt);

#ifdef hpV4
#  define GETMSG(catd, set_num, msg_num, def_str) catgets_cache2(catd, set_num, msg_num, def_str)
#else
#  define GETMSG(catd, set_num, msg_num, def_str) catgets(catd, set_num, msg_num, def_str)
#endif

#endif
