/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
