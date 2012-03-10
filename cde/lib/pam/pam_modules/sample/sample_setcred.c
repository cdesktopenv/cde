/* $XConsortium: sample_setcred.c /main/2 1996/05/09 04:31:02 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)sample_setcred.c 1.9     96/01/12 SMI"

#include <libintl.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#define	PAMTXD	"SUNW_OST_SYSOSPAM"

/*
 * pam_sm_setcred
 */
int
pam_sm_setcred(
	pam_handle_t *pamh,
	int   flags,
	int	argc,
	const char **argv)
{

	/*
	 * Set the credentials
	 */

	return (PAM_SUCCESS);
}
