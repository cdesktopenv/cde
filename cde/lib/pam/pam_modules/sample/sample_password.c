/* $XConsortium: sample_password.c /main/2 1996/05/09 04:30:43 drk $ */
/*
 * Copyright (c) 1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)sample_password.c 1.10     96/01/15 SMI"

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int
pam_sm_chauthtok(
	pam_handle_t	*pamh,
	int		flags,
	int		argc,
	const char	**argv)

{
	return (PAM_SUCCESS);
}
