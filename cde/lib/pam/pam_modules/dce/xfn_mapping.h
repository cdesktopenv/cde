/* $XConsortium: xfn_mapping.h /main/4 1996/05/09 04:28:15 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef _XFN_MAPPING_H
#define	_XFN_MAPPING_H

#ident  "@(#)xfn_mapping.h 1.6     95/09/19 SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#define	MAP_PASSLEN	32		/* XXX max mapped passwd length */
#define	MAP_HEXLEN	(2*MAP_PASSLEN)	/* XXX max passwd length in hex */
#define	DCE_XFN_PASS_ATTR	"onc_dce_passwd"

#define	XFN_MAP_DEBUG	0x1	/* enable debugging */

int
xfn_get_mapped_password(
	int flags,		/* XFN_MAP_DEBUG, etc */
	char *user,		/* xfn user */
	char *xfn_attr,		/* xfn attribute that holds the target pass */
	char *key,		/* key (password) encrypting target password */
	char *out,		/* un-encrypted target password */
	int max_out_len		/* buffer size of out */
);

int
xfn_update_mapped_password(
	int flags,		/* XFN_MAP_DEBUG, etc */
	char *user,		/* xfn user */
	char *xfn_attr,		/* xfn attribute that holds the target pass */
	char *key,		/* key (password) encrypting target password */
	char *password		/* un-encrypted target password */
);

#endif

#ifdef	__cplusplus
}
#endif
