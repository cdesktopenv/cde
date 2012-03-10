/*
 * $XConsortium: debug.h /main/5 1996/07/18 14:57:44 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */



#ifndef _debug_h
#define _debug_h 1

#ifdef DEBUG

#if !defined ( __STDC__) && !defined (hpux) && !defined(__osf__)
#define debug(s, x)  s <<  "x" << " = " << (x) << "\n"
#else
#define debug(s, x)  s << #x << " = " << (x) << "\n"
#endif
#define MESSAGE(s, x)  s << x << "\n"

#else /* DEBUG */

#define debug(s,x)
#define MESSAGE(s,x)
#endif /* DEBUG */

#ifndef C_API
#define STDERR_MESSAGE(x) cerr << x << "\n";
#else
#define STDERR_MESSAGE(x) fprintf(stderr, "%s\n",  x);
#endif


#endif

