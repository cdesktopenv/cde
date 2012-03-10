/* $XConsortium: sample_close_session.c /main/2 1996/05/09 04:30:08 drk $ */
/*
 * Copyright (c) 1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)sample_close_session.c 1.8     96/01/12 SMI"

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>

int
pam_sm_close_session(
	pam_handle_t *pamh,
	int	flags,
	int	argc,
	const char **argv)
{
	return (PAM_SUCCESS);
}
