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
/* $XConsortium: pam_appl.h /main/5 1996/05/09 04:24:28 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef _PAM_APPL_H
#define	_PAM_APPL_H

#pragma ident	"@(#)pam_appl.h	1.45	96/02/15 SMI"	/* PAM 2.6 */

#include <pwd.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* Generic PAM errors */
#define	PAM_SUCCESS		0	/* Normal function return */
#define	PAM_OPEN_ERR		1	/* Dlopen failure */
#define	PAM_SYMBOL_ERR		2	/* Symbol not found */
#define	PAM_SERVICE_ERR		3	/* Error in underlying service module */
#define	PAM_SYSTEM_ERR		4	/* System error */
#define	PAM_BUF_ERR		5	/* Memory buffer error */
#define	PAM_CONV_ERR		6	/* Conversation failure */
#define	PAM_PERM_DENIED		7	/* Permission denied */

/* Errors returned by pam_authenticate, pam_acct_mgmt(), and pam_setcred() */
#define	PAM_MAXTRIES		8	/* Maximum number of tries exceeded */
#define	PAM_AUTH_ERR		9	/* Authentication failure */
#define	PAM_NEW_AUTHTOK_REQD	10	/* Get new auth token from the user */
#define	PAM_AUTHTOKEN_REQD	PAM_NEW_AUTHTOK_REQD /* backward compatible */
#define	PAM_CRED_INSUFFICIENT	11	/* can not access auth data b/c */
					/* of insufficient credentials  */
#define	PAM_AUTHINFO_UNAVAIL	12	/* Can not retrieve auth information */
#define	PAM_USER_UNKNOWN	13	/* No account present for user */

/* Errors returned by pam_setcred() */
#define	PAM_CRED_UNAVAIL	14	/* can not retrieve user credentials */
#define	PAM_CRED_EXPIRED	15	/* user credentials expired */
#define	PAM_CRED_ERR		16	/* failure setting user credentials */

/* Errors returned by pam_acct_mgmt() */
#define	PAM_ACCT_EXPIRED	17	/* user account has expired */
#define	PAM_AUTHTOK_EXPIRED 	18	/* Password expired and no longer */
					/* usable */

/* Errors returned by pam_open/close_session() */
#define	PAM_SESSION_ERR		19	/* can not make/remove entry for */
					/* specified session */

/* Errors returned by pam_chauthtok() */
#define	PAM_AUTHTOK_ERR		  20	/* Authentication token */
					/*   manipulation error */
#define	PAM_AUTHTOK_RECOVERY_ERR  21	/* Old authentication token */
					/*   cannot be recovered */
#define	PAM_AUTHTOK_LOCK_BUSY	  22	/* Authentication token */
					/*   lock busy */
#define	PAM_AUTHTOK_DISABLE_AGING 23	/* Authentication token aging */
					/*   is disabled */

/* Errors returned by pam_get_data */
#define	PAM_NO_MODULE_DATA	24	/* module data not found */

/* Errors returned by modules */
#define	PAM_IGNORE		25	/* ignore module */

#define	PAM_ABORT		26	/* General PAM failure */
#define	PAM_TRY_AGAIN		27	/* Unable to update password */
					/* Try again another time */

/*
 * XXX: Make sure that PAM_TOTAL_ERRNUM = 28 in pam_impl.h
 */

/*
 * structure pam_message is used to pass prompt, error message,
 * or any text information from scheme to application/user.
 */

struct pam_message {
	int msg_style;		/* Msg_style - see below */
	char *msg; 		/* Message string */
};

/*
 * msg_style defines the interaction style between the
 * scheme and the application.
 */
#define	PAM_PROMPT_ECHO_OFF	1	/* Echo off when getting response */
#define	PAM_PROMPT_ECHO_ON	2 	/* Echo on when getting response */
#define	PAM_ERROR_MSG		3	/* Error message */
#define	PAM_TEXT_INFO		4	/* Textual information */

/* max # of authentication token attributes */
#define	PAM_MAX_NUM_ATTR	10

/* max size (in chars) of an authentication token attribute */
#define	PAM_MAX_ATTR_SIZE	80

/*
 * max # of messages passed to the application through the
 * conversation function call
 */
#define	PAM_MAX_NUM_MSG	32

/*
 * max size (in chars) of each messages passed to the application
 * through the conversation function call
 */
#define	PAM_MAX_MSG_SIZE	512

/*
 * max size (in chars) of each response passed from the application
 * through the conversation function call
 */
#define	PAM_MAX_RESP_SIZE	512

/*
 * structure pam_response is used by the scheme to get the user's
 * response back from the application/user.
 */

struct pam_response {
	char *resp;		/* Response string */
	int resp_retcode;	/* Return code - for future use */
};

/*
 * structure pam_conv is used by authentication applications for passing
 * call back function pointers and application data pointers to the scheme
 */
struct pam_conv {
	int (*conv)(int, struct pam_message **,
	    struct pam_response **, void *);
	void *appdata_ptr;		/* Application data ptr */
};

/* the pam handle */
typedef struct pam_handle pam_handle_t;

/*
 * pam_start() is called to initiate an authentication exchange
 * with PAM.
 */
extern int
pam_start(
	const char *service_name,		/* Service Name */
	const char *user,			/* User Name */
	const struct pam_conv *pam_conv,	/* Conversation structure */
	pam_handle_t **pamh		/* Address to store handle */
);

/*
 * pam_end() is called to end an authentication exchange with PAM.
 */
extern int
pam_end(
	pam_handle_t *pamh,		/* handle from pam_start() */
	int status			/* the final status value that */
					/* gets passed to cleanup functions */
);

/*
 * pam_set_item is called to store an object in PAM handle.
 */
extern int
pam_set_item(
	pam_handle_t *pamh,		/* PAM handle */
	int item_type, 			/* Type of object - see below */
	const void *item		/* Address of place to put pointer */
					/*   to object */
);

/*
 * pam_get_item is called to retrieve an object from the static data area
 */
extern int
pam_get_item(
	const pam_handle_t *pamh, 	/* PAM handle */
	int item_type, 			/* Type of object - see below */
	void **	item			/* Address of place to put pointer */
					/*   to object */
);

/* Items supported by pam_[sg]et_item() calls */
#define	PAM_SERVICE	1		/* The program/service name */
#define	PAM_USER	2		/* The user name */
#define	PAM_TTY		3		/* The tty name */
#define	PAM_RHOST	4		/* The remote host name */
#define	PAM_CONV	5		/* The conversation structure */
#define	PAM_AUTHTOK	6		/* The authentication token */
#define	PAM_OLDAUTHTOK	7		/* Old authentication token */
#define	PAM_RUSER	8		/* The remote user name */
#define	PAM_USER_PROMPT	9		/* The user prompt */

/*
 * pam_get_user is called to retrieve the user name (PAM_USER). If PAM_USER
 * is not set then this call will prompt for the user name using the
 * conversation function. This function should only be used by modules, not
 * applications.
 */

extern int
pam_get_user(
	pam_handle_t *pamh,		/* PAM handle */
	char **user, 			/* User Name */
	const char *prompt		/* Prompt */
);

/*
 * pam_set_data is used to create module specific data, and
 * to optionally add a cleanup handler that gets called by pam_end.
 *
 */
extern int
pam_set_data(
	pam_handle_t *pamh,		/* PAM handle */
	const char *module_data_name,	/* unique module data name */
	const void *data,		/* the module specific data */
	void (*cleanup)(pam_handle_t *pamh, void *data, int pam_end_status)
);

/*
 * get module specific data set by pam_set_scheme_data.
 * returns PAM_NO_MODULE_DATA if specified module data was not found.
 */
extern int
pam_get_data(
	const pam_handle_t *pamh,
	const char *module_data_name,
	void **data
);

/*
 * PAM equivalent to strerror();
 */
extern char *
pam_strerror(
	pam_handle_t *pamh,	/* pam handle */
	int errnum		/* error number */
);

/* general flag for pam_* functions */
#define	PAM_SILENT	0x80000000

/*
 * pam_authenticate is called to authenticate the current user.
 */
extern int
pam_authenticate(
	pam_handle_t *pamh,
	int flags
);

/*
 * Flags for pam_authenticate
 */

#define	PAM_DISALLOW_NULL_AUTHTOK 2	/* The password must be non-null */

/*
 * pam_acct_mgmt is called to perform account management processing
 */
extern int
pam_acct_mgmt(
	pam_handle_t *pamh,
	int flags
);

/*
 * pam_open_session is called to note the initiation of new session in the
 * appropriate administrative data bases.
 */
extern int
pam_open_session(
	pam_handle_t *pamh,
	int flags
);

/*
 * pam_close_session records the termination of a session.
 */
extern int
pam_close_session(
	pam_handle_t	*pamh,
	int		flags
);

/* pam_setcred is called to set the credentials of the current user */
extern int
pam_setcred(
	pam_handle_t *pamh,
	int flags
);

/* flags for pam_setcred() */
#define	PAM_ESTABLISH_CRED	1	/* set scheme specific user id */
#define	PAM_DELETE_CRED		2	/* unset scheme specific user id */
#define	PAM_REINITIALIZE_CRED	4	/* reinitialize user credentials */
					/* (after a password has changed */
#define	PAM_REFRESH_CRED	8	/* extend lifetime of credentials */

/* pam_chauthtok is called to change authentication token */

extern int
pam_chauthtok(
	pam_handle_t	*pamh,
	int		flags
);

/*
 * Be careful - there are flags defined for pam_sm_chauthtok() in
 * pam_modules.h also.
 */
#define	PAM_CHANGE_EXPIRED_AUTHTOK	4 /* update expired passwords only */

#ifdef	__cplusplus
}
#endif

#endif /* _PAM_APPL_H */
