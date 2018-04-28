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
/* $XConsortium: sample_utils.c /main/2 1996/05/09 04:31:21 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)sample_utils.c 1.8     96/01/12 SMI"

#include <security/pam_appl.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "sample_utils.h"

/* ******************************************************************** */
/*									*/
/* 		Utilities Functions					*/
/*									*/
/* ******************************************************************** */

/*
 * free_msg():
 *	free storage for messages used in the call back "pam_conv" functions
 */

void
free_msg(num_msg, msg)
	int num_msg;
	struct pam_message *msg;
{
	int 			i;
	struct pam_message 	*m;

	if (msg) {
		m = msg;
		for (i = 0; i < num_msg; i++, m++) {
			if (m->msg)
				free(m->msg);
		}
		free(msg);
	}
}

/*
 * free_resp():
 *	free storage for responses used in the call back "pam_conv" functions
 */

void
free_resp(num_msg, resp)
	int num_msg;
	struct pam_response *resp;
{
	int			i;
	struct pam_response	*r;

	if (resp) {
		r = resp;
		for (i = 0; i < num_msg; i++, r++) {
			if (r->resp)
				free(r->resp);
		}
		free(resp);
	}
}

/*
 * display_errmsg():
 *	display error message by calling the call back functions
 *	provided by the application through "pam_conv" structure
 */

int
display_errmsg(conv_funp, num_msg, messages, conv_apdp)
	int (*conv_funp)();
	int num_msg;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	void *conv_apdp;
{
	struct pam_message	*msg;
	struct pam_message	*m;
	struct pam_response	*resp;
	int			i;
	int			k;
	int			retcode;

	msg = (struct pam_message *)calloc(num_msg,
					sizeof (struct pam_message));
	if (msg == NULL) {
		return (PAM_CONV_ERR);
	}
	m = msg;

	i = 0;
	k = num_msg;
	resp = NULL;
	while (k--) {
		/*
		 * fill out the pam_message structure to display error message
		 */
		m->msg_style = PAM_ERROR_MSG;
		m->msg = (char *)malloc(PAM_MAX_MSG_SIZE);
		if (m->msg != NULL)
			(void) strcpy(m->msg, (const char *)messages[i]);
		else
			continue;
		m++;
		i++;
	}

	/*
	 * Call conv function to display the message,
	 * ignoring return value for now
	 */
	retcode = conv_funp(num_msg, &msg, &resp, conv_apdp);
	free_msg(num_msg, msg);
	free_resp(num_msg, resp);
	return (retcode);
}

/*
 * get_authtok():
 *	get authentication token by calling the call back functions
 *	provided by the application through "pam_conv" structure
 */

int
get_authtok(conv_funp, num_msg, messages, conv_apdp, ret_respp)
	int (*conv_funp)();
	int num_msg;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	void *conv_apdp;
	struct pam_response	**ret_respp;
{
	struct pam_message	*msg;
	struct pam_message	*m;
	int			i;
	int			k;
	int			retcode;

	i = 0;
	k = num_msg;

	msg = (struct pam_message *)calloc(num_msg,
						sizeof (struct pam_message));
	if (msg == NULL) {
		return (PAM_CONV_ERR);
	}
	m = msg;

	while (k--) {
		/*
		 * fill out the message structure to display error message
		 */
		m->msg_style = PAM_PROMPT_ECHO_OFF;
		m->msg = (char *)malloc(PAM_MAX_MSG_SIZE);
		if (m->msg != NULL)
			(void) strcpy(m->msg, (char *)messages[i]);
		else
			continue;
		m++;
		i++;
	}

	/*
	 * Call conv function to display the prompt,
	 * ignoring return value for now
	 */
	retcode = conv_funp(num_msg, &msg, ret_respp, conv_apdp);
	free_msg(num_msg, msg);
	return (retcode);
}
