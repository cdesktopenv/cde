/* $XConsortium: Invoke.h /main/4 1996/10/01 16:09:57 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef INVOKE_H
#define INVOKE_H

#include <sys/types.h>
#include <stdio.h>

class Invoke
{
 
 public:

  int status;

  Invoke(const char *command,
	 char **std_out = NULL,
	 char **std_err = NULL,
	 uid_t uid = (uid_t)-1); // To run the command as another, set uid >= 0
};

#endif // INVOKE_H
