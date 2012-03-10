/* $XConsortium: dce_session.c /main/4 1996/05/09 04:27:00 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)dce_session.c 1.3     96/01/10 SMI"

#include <libintl.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <dce/sec_login.h>
#include <dce/dce_error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <syslog.h>
#include <libintl.h>

#include "pam_impl.h"
#include "utils.h"

/*
 * pam_sm_open_session
 */
int
pam_sm_open_session(
	pam_handle_t *pamh,
	int   flags,
	int	argc,
	const char **argv)
{
	return (PAM_SUCCESS);
}

/*
 * pam_sm_close_session
 */
int
pam_sm_close_session(
	pam_handle_t *pamh,
	int   flags,
	int	argc,
	const char **argv)
{
	return (PAM_SUCCESS);
}
