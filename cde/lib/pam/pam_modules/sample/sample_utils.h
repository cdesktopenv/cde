/* $XConsortium: sample_utils.h /main/2 1996/05/09 04:31:40 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef _TEST_UTILS_H
#define	_TEST_UTILS_H

#pragma ident	"@(#)sample_utils.h	1.7	96/02/02 SMI"	/* PAM 2.6 */

#ifdef __cplusplus
extern "C" {
#endif

void
free_msg(int num_msg, struct pam_message *msg);

void
free_resp(int num_msg, struct pam_response *resp);

int
display_errmsg(
	int (*conv_funp)(),
	int num_msg,
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE],
	void *conv_apdp
);

int
get_authtok(
	int (*conv_funp)(),
	int num_msg,
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE],
	void *conv_apdp,
	struct pam_response	**ret_respp
);

/*
 * PAM_MSG macro for return of internationalized text
 */

#define	PAM_MSG(pamh, number, string)\
	(char *) __pam_get_i18n_msg(pamh, "pam_unix", 3, number, string)

#ifdef __cplusplus
}
#endif

#endif /* _TEST_UTILS_H */
