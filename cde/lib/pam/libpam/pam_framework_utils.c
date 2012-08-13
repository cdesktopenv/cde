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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: pam_framework_utils.c /main/8 1996/11/20 11:07:39 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident	"@(#)pam_framework_utils.c 1.37	95/12/20 SMI"	/*	*/

#include <syslog.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utmpx.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libintl.h>
#include <synch.h>
#include <shadow.h>
#include <locale.h>
#include <stdio.h>
#include <nl_types.h>
#include <X11/Xthreads.h>

#include <X11/Xos.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include "pam_impl.h"
#include "pam_loc.h"

static void __pam_msg_destroy(void *);
static char * __pam_thread_backup(char *);
static int __pam_input_output(pam_handle_t *, int, int, char [][], void *,
	struct pam_response **);


/*
 * __pam_free_resp():
 *	free storage for responses used in the call back "pam_conv" functions
 */

void
__pam_free_resp(int num_msg, struct pam_response *resp)
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
 * __pam_display_msg():
 *	display message by calling the call back functions
 *	provided by the application through "pam_conv" structure
 */

int
__pam_display_msg(pamh, msg_style, num_msg, messages, conv_apdp)
	pam_handle_t *pamh;
	int msg_style;
	int num_msg;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	void *conv_apdp;
{
	struct pam_response	*ret_respp = NULL;

	return (__pam_input_output(pamh, msg_style, num_msg, messages,
		conv_apdp, &ret_respp));
}

int
__pam_get_input(pamh, msg_style, num_msg, messages, conv_apdp, ret_respp)
	pam_handle_t *pamh;
	int msg_style;
	int num_msg;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	void *conv_apdp;
	struct pam_response	**ret_respp;
{
	return (__pam_input_output(pamh, msg_style, num_msg, messages,
		conv_apdp, ret_respp));
}

static int
__pam_input_output(pamh, msg_style, num_msg, messages, conv_apdp, ret_respp)
	pam_handle_t *pamh;
	int msg_style;
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
	struct pam_conv		*pam_convp;

	if ((retcode = pam_get_item(pamh, PAM_CONV, (void **)&pam_convp))
							!= PAM_SUCCESS) {
		return (retcode);
	}

	if (pam_convp == NULL)
		return (PAM_SYSTEM_ERR);

	i = 0;
	k = num_msg;

	msg = (struct pam_message *)calloc(num_msg,
				sizeof (struct pam_message));
	if (msg == NULL) {
		return (PAM_BUF_ERR);
	}
	m = msg;

	while (k--) {
		/*
		 * fill out the message structure to display prompt message
		 */
		m->msg_style = msg_style;
		m->msg = messages[i];
		m++;
		i++;
	}

	/*
	 * Call conv function to display the prompt.
	 */
	retcode = (pam_convp->conv)(num_msg, &msg, ret_respp, conv_apdp);
	return (retcode);
}

/*
 * __pam_get_authtok()
 *	retrieves a password of length "len" from the pam handle
 *	(pam_get_item) or from the input stream (pam_get_input).
 *
 * This function allocates memory for the new authtok.
 * Applications calling this function are responsible for
 * freeing this memory.
 *
 * If "source" is
 *	PAM_HANDLE
 * and "type" is:
 *	PAM_AUTHTOK - password is taken from pam handle (PAM_AUTHTOK)
 *	PAM_OLDAUTHTOK - password is taken from pam handle (PAM_OLDAUTHTOK)
 *
 * If "source" is
 *	PAM_PROMPT
 * and "type" is:
 *	0:		Prompt for new passwd, do not even attempt
 *			to store it in the pam handle.
 *	PAM_AUTHTOK:	Prompt for new passwd, store in pam handle as
 *			PAM_AUTHTOK item if this value is not already set.
 *	PAM_OLDAUTHTOK:	Prompt for new passwd, store in pam handle as
 *			PAM_OLDAUTHTOK item if this value is not
 *			already set.
 */
int
__pam_get_authtok(pam_handle_t *pamh, int source, int type, int len,
	char *prompt, char **authtok)
{

	int error = PAM_SYSTEM_ERR;
	char *new_password = NULL;
	struct pam_response *ret_resp = (struct pam_response *)0;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	char *backup_prompt = PAM_MSG(pamh, 31, "password: ");

	if (len >= PAM_MAX_RESP_SIZE) {
		syslog(LOG_ERR,
			"__pam_get_authtok: requested passwd length too long");
		return (PAM_BUF_ERR);
	}

	if ((*authtok = (char *)calloc(len + 1, sizeof (char))) == NULL) {
		*authtok = NULL;
		return (PAM_BUF_ERR);
	}

	if (prompt == NULL)
		prompt = backup_prompt;

	switch (source) {
	case PAM_HANDLE:

		/* get password from pam handle item list */

		switch (type) {
		case PAM_AUTHTOK:
		case PAM_OLDAUTHTOK:

			if ((error = pam_get_item(pamh, type,
				(void **)&new_password)) != PAM_SUCCESS) {
				free(*authtok);
				*authtok = NULL;
				return (error);
			}

			if (new_password == NULL || new_password[0] == '\0') {
				free(*authtok);
				*authtok = NULL;
			} else {
				strncpy(*authtok, new_password, len);
				(*authtok)[len] = '\0';
			}
			break;
		default:
			syslog(LOG_ERR,
				"__pam_get_authtok() invalid type: %d", type);
			free(*authtok);
			*authtok = NULL;
			return (PAM_SYMBOL_ERR);
		}
		break;
	case PAM_PROMPT:

		/*
		 * Prompt for new password and save in pam handle item list
		 * if the that item is not already set.
		 */

		strncpy(messages[0], prompt, sizeof (messages[0]));
		if ((error = __pam_get_input(pamh, PAM_PROMPT_ECHO_OFF,
			1, messages, NULL, &ret_resp)) != PAM_SUCCESS) {
			free(*authtok);
			*authtok = NULL;
			return (error);
		}

		/* save the new password if this item was NULL */
		if (type) {
			pam_get_item(pamh, type, (void **)&new_password);
			if (new_password == NULL)
				pam_set_item(pamh, type, ret_resp->resp);
		}

		strncpy(*authtok, ret_resp->resp, len);
		(*authtok)[len] = '\0';
		memset(ret_resp->resp, 0, strlen(ret_resp->resp));
		__pam_free_resp(1, ret_resp);
		break;
	default:
		syslog(LOG_ERR,
			"__pam_get_authtok() invalid source: %d", source);
		free(*authtok);
		*authtok = NULL;
		return (PAM_SYMBOL_ERR);
	}

	return (PAM_SUCCESS);
}


#if !defined(NL_CAT_LOCALE)
#define	NL_CAT_LOCALE 0
#endif

typedef struct _pam_msg_data {
	nl_catd fd;
} _pam_msg_data;

static void
__pam_msg_cleanup(
	pam_handle_t *pamh,
	void *data,
	int pam_status)
{
	_pam_msg_data *msg_data = (_pam_msg_data *) data;

	catclose(msg_data->fd);
	free(msg_data);
}

static void
__pam_msg_destroy(void *tsd)
{
	if (tsd)
		free((char *)tsd);
}

static char *
__pam_thread_backup(char *msg)
{
	char *data_buffer = 0;
	static xmutex_rec thread_lock = XMUTEX_INITIALIZER;
	static xthread_key_t thread_key = 0;
	static char fallback_buff[PAM_MAX_MSG_SIZE];

	memset(fallback_buff, 0, sizeof (fallback_buff));
	if (thread_key == 0) {
		xmutex_lock(&thread_lock);
		if (thread_key == 0)
			xthread_key_create(&thread_key, __pam_msg_destroy);
		xmutex_unlock(&thread_lock);
		if (thread_key == 0) {
			strncat(fallback_buff, msg, PAM_MAX_MSG_SIZE-1);
			return (fallback_buff);
		}
	}
	xthread_get_specific(thread_key, (void **)&data_buffer);

	if (data_buffer == (char *)NULL) {
		if ((data_buffer = (char *)calloc(PAM_MAX_MSG_SIZE,
				sizeof (char))) == NULL) {

			/* what else can i do? */
			strncat(fallback_buff, msg, PAM_MAX_MSG_SIZE-1);
			return (fallback_buff);
		}
		xthread_set_specific(thread_key, (void *)data_buffer);
	}
	/*
	 * Memset the buffer because we might have stale data from
	 * a previous thr_setspecific() call.
	 */
	memset(data_buffer, 0, PAM_MAX_MSG_SIZE);
	strncat(data_buffer, msg, PAM_MAX_MSG_SIZE-1);
	return (data_buffer);
}

/*
 *
 * Function: __pam_get_i18n_msg
 *
 *
 * Parameters:
 *
 *	int	set -	The message catalog set number.
 *
 *	int	  n -  	The message number.
 *
 *	char	 *s -	The default message if the message is not
 *			retrieved from a message catalog.
 *
 * Returns: the string for set 'set' and number 'n'.
 *
 */

char *
__pam_get_i18n_msg(
	pam_handle_t *pamh,
	char *filename,
	int set,
	int n,
	char *s)
{
	char *msg;
	char *output_msg;
	nl_catd nlmsg_fd;

	/*
	 * If pam handle was supplied,
	 * look for stored message file descriptor.
	 */

	if (pamh != NULL) {
	    _pam_msg_data *msg_data;
	    int status = pam_get_data(pamh, filename, (void**) &msg_data);

	    if (status == PAM_SUCCESS) {
		return (catgets(msg_data->fd, set, n, s));
	    }

	    if (status == PAM_NO_MODULE_DATA) {
		/*
		 * No message file descriptor found, make and store one.
		 */
		nlmsg_fd = catopen(filename, NL_CAT_LOCALE);
		msg = catgets(nlmsg_fd, set, n, s);

		if ((msg_data = (_pam_msg_data *)
		    calloc(1, sizeof (_pam_msg_data))) == NULL) {
			output_msg = __pam_thread_backup(msg);
			catclose(nlmsg_fd);
			return (output_msg);
		}

		msg_data->fd = nlmsg_fd;
		pam_set_data(pamh, filename, msg_data, __pam_msg_cleanup);

		return (msg);
	    }
	}

	/* NULL pamh */

	nlmsg_fd = catopen(filename, NL_CAT_LOCALE);
	msg = catgets(nlmsg_fd, set, n, s);
	output_msg = __pam_thread_backup(msg);
	catclose(nlmsg_fd);
	return (output_msg);

}


#ifdef PAM_MAYBE_WILL_BE_USED_LATER
extern int pam_debug;

/* Errors returned by __setutmp_mgmt/__reset_utmp_mgmt() */
#define	__NOENTRY		27	/* No entry found */
#define	__ENTRYFAIL		28	/* Couldn't make/remove the entry */

/* Errors returned by __setproc_cred() */
#define	__BAD_GID		29	/* Invalid Group ID */
#define	__INITGROUP_FAIL	30	/* Initialization of group IDs failed */
#define	__BAD_UID		31	/* Invaid User ID */
#define	__SETGROUP_FAIL		32	/* Set of group IDs failed */

#define	INIT_PROC_PID	1
#define	PAMTXD		"SUNW_OST_SYSOSPAM"
#define	SCPYN(a, b)	(void) strncpy(a, b, sizeof (a))

/* utility function to do UTMP/WTMP management */
int
__setutmp_mgmt(
	char *user,	/* user */
	char *ttyn,	/* ttyn */
	char *rhost,	/* remote hostname */
	int flags,	/* Flags - see below */
	int type,	/* type of utmp/wtmp entry */
	char id[]	/* 4 byte id field for utmp */
);

/* Flags for the flags field */

#define	__UPDATE_ENTRY  1	/* Update an existing entry */
#define	__NOLOG		2	/* Don't log the new session */
#define	__LOGIN		4	/* login type entry (sigh...) */

/*
 * __reset_utmp_mgmt is a utility function which terminates UTMP/WTMP mgmt
 */
int
__reset_utmp_mgmt(
	char **user,	/* user */
	char **ttyn,	/* tty name */
	char **rhost,	/* remote host */
	int flags,	/* flags - see below */
	int status,	/* logout process status */
	char id[]	/* logout ut_id (/etc/inittab id) */
);

/* flags for the flags field */
#define	__NOOP	8	/* No utmp action desired */

/* __setproc_cred is a utility function to set process credentials */
int
__setproc_cred(
	char *user,		/* user */
	int flags,		/* flags - see below */
	uid_t uid,		/* User ID to set for this process */
	gid_t gid,		/* Group ID */
	int ngroups,		/* Number of groups */
	gid_t *grouplist	/* Group list */
);

/* flags indicates specific set credential actions */

#define	__INITGROUPS	0x00000001	/* Request to initgroups() */
#define	__SETGROUPS	0x00000002	/* Request to setgroups() */
#define	__SETEGID	0x00000004	/* Set effective gid only */
#define	__SETGID	0x00000008	/* Set real gid */
#define	__SETEUID	0x00000010	/* Set effective uid only */
#define	__SETUID	0x00000020	/* Set real uid */
#define	__SETEID	(__SETEGID|__SETEUID)	/* Set effective ids only */
#define	__SETRID	(__SETGID|__SETUID)	/* Set real ids */

/*
 * __setutmp_mgmt - A utility function used to do UTMP/WTMP management.
 *		    This function is NOT meant to be part of the official
 *		    PAM API, and only serves as a convenience function.
 *
 *	"user" is the current username.
 *	"ttyn" is the tty name.
 *	"rhost" is the remote hostname.
 *	The following flags may be set in the "flags" field:
 *
 *	__UPDATE_ENTRY		No new entry will be created if utmp
 *				entry not found - return __NOENTRY
 *	__NOLOG		Generate a wtmp/wtmpx entry only
 *	__LOGIN		Caller is a login application - update
 *				utmp entry accordingly
 *
 *	"type" is used to indicate the type of utmp/wtmp entry written
 *		(see also utmp.h and utmpx.h)
 *	"id is an optional application-defined 4 byte array that represents
 *	the /sbin/inittab id (created by the process that puts an entry in
 *	utmp).
 *
 *	Upon successful completion, PAM_SUCCESS is returned.
 *	Error values may include:
 *
 *	__NOENTRY	An entry for the specified process was not found
 *	__ENTRYFAIL	Could not make/remove entry for specified process
 */

int
__setutmp_mgmt(
	char 	*user,
	char	*ttyn,
	char	*rhost,
	int	flags,
	int	type,
	char	id[])
{
	int			tmplen;
	struct utmpx    	*u = (struct utmpx *)0;
	struct utmpx		utmp;
	char			*ttyntail;
	int			err = PAM_SUCCESS;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_open_session(%d)\n", type);

	(void) memset((void *)&utmp, 0, sizeof (utmp));

	(void) time(&utmp.ut_tv.tv_sec);
	utmp.ut_pid = getpid();
	if (rhost != NULL && rhost[0] != '\0') {
		(void) strcpy(utmp.ut_host, rhost);
		tmplen = strlen(rhost) + 1;
		if (tmplen < sizeof (utmp.ut_host))
			utmp.ut_syslen = tmplen;
		else
			utmp.ut_syslen = sizeof (utmp.ut_host);
	} else {
		(void) memset(utmp.ut_host, 0, sizeof (utmp.ut_host));
		utmp.ut_syslen = 0;
	}

	(void) strcpy(utmp.ut_user, user);
	/*
	 * Copy in the name of the tty minus the "/dev/" if a /dev/ is
	 * in the path name.
	 */

	if (!(flags&__LOGIN))
		SCPYN(utmp.ut_line, ttyn);

	ttyntail = ttyn;

	utmp.ut_type = type;

	if (id != NULL)
		(void) memcpy(utmp.ut_id, id, sizeof (utmp.ut_id));

	if ((flags & __NOLOG) == __NOLOG) {
		updwtmpx(WTMPX_FILE, &utmp);
	} else {
		/*
		 * Go through each entry one by one, looking only at INIT,
		 * LOGIN or USER Processes.  Use the entry found if flags == 0
		 * and the line name matches, or if the process ID matches if
		 * the UPDATE_ENTRY flag is set.  The UPDATE_ENTRY flag is
		 * mainly for login which normally only wants to update an
		 * entry if the pid fields matches.
		 */

		if (flags & __LOGIN) {
		    while ((u = getutxent()) != NULL) {
			if ((u->ut_type == INIT_PROCESS ||
			    u->ut_type == LOGIN_PROCESS ||
			    u->ut_type == USER_PROCESS) &&
			    ((flags == __LOGIN && ttyn != NULL &&
					strncmp(u->ut_line, ttyntail,
						sizeof (u->ut_line)) == 0) ||
			    u->ut_pid == utmp.ut_pid)) {
				if (ttyn)
					SCPYN(utmp.ut_line,
					    (ttyn + sizeof ("/dev/") - 1));
				if (id == NULL) {
					(void) memcpy(utmp.ut_id, u->ut_id,
						    sizeof (utmp.ut_id));
				}
				(void) pututxline(&utmp);
				break;
			}
		    }	   /* end while */
		    endutxent();		/* Close utmp file */
		}

		if (u == (struct utmpx *)NULL) {
			/* audit_login_main11(); */
			if (flags & __UPDATE_ENTRY)
				err =  __NOENTRY;
			else
				(void) makeutx(&utmp);
		}
		else
			updwtmpx(WTMPX_FILE, &utmp);
	}
	return (err);
}

/*
 * __reset_utmp_mgmt	A utility function used to terminate  UTMP/WTMP mgmt.
 *			This function is NOT meant to be part of the official
 *			PAM API, and only serves as a convenience function.
 *
 *	"user" is the current username.
 *	"ttyn" is the tty name.
 *	"rhost" is the remote hostname.
 *	The following flags may be set in the "flags" field:
 *
 *	__NOLOG		Write a wtmp/wtmpx entry only
 *	__NOOP			Ignore utmp/wtmp processing
 *
 *	"status" is the logout process status.
 *	"id is an optional application-defined 4 byte array that represents
 *	the /sbin/inittab id (created by the process that puts an entry in
 *	utmp).
 *
 *	Upon successful completion, PAM_SUCCESS is returned.
 *	Error values may include:
 *
 *	__NOENTRY	An entry for the specified process was not found
 *	__ENTRYFAIL	Could not make/remove entry for specified process
 */

int
__reset_utmp_mgmt(
	char	**user,
	char	**ttyn,
	char	**rhost,
	int	flags,
	int	status,
	char	id[])
{
	struct utmpx 		*up;
	struct utmpx  		ut;
	int			err = 0;
	int			pid;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_close_session()\n");

	/*
	 * do we want to do any utmp processing?
	 */
	if (flags & __NOOP) {
		return (PAM_SUCCESS);
	}

	pid = (int) getpid();

	if ((flags & __NOLOG) == __NOLOG) {	/* only write to wtmp files */
			/* clear utmpx entry */
		(void) memset((char *)&ut, 0, sizeof (ut));

		if (id != NULL)
			(void) memcpy(ut.ut_id, id, sizeof (ut.ut_id));

		if (*ttyn != NULL && **ttyn != '\0') {
			if (strstr(*ttyn, "/dev/") != NULL)
			(void) strncpy(ut.ut_line, (*ttyn+sizeof ("/dev/")-1),
							sizeof (ut.ut_line));
			else
				(void) strncpy(ut.ut_line, *ttyn,
							sizeof (ut.ut_line));
		}
		ut.ut_pid  = pid;
		ut.ut_type = DEAD_PROCESS;
		ut.ut_exit.e_termination = 0;
		ut.ut_exit.e_exit = 0;
		ut.ut_syslen = 1;
		(void) gettimeofday(&ut.ut_tv, NULL);
		updwtmpx(WTMPX_FILE, &ut);

		return (PAM_SUCCESS);
	} else {
		setutxent();
		while (up = getutxent()) {
			if (up->ut_pid == pid) {
				if (up->ut_type == DEAD_PROCESS) {
					/*
					 * Cleaned up elsewhere.
					 */
					endutxent();
					return (0);
				}
				if ((*user = (char *) strdup(up->ut_user))
							== NULL ||
				    (*ttyn = (char *) strdup(up->ut_line))
							== NULL ||
				    (*rhost = (char *) strdup(up->ut_host))
							== NULL ||)
					return (PAM_BUF_ERR);

				up->ut_type = DEAD_PROCESS;
				up->ut_exit.e_termination = status & 0xff;
				up->ut_exit.e_exit = (status >> 8) & 0xff;
				if (id != NULL)
					(void) memcpy(up->ut_id, id,
						sizeof (up->ut_id));
				(void) time(&up->ut_tv.tv_sec);

				/*
				 * For init (Process ID 1) we don't write to
				 * init's pipe, since we are init.
				 */
				if (getpid() == INIT_PROC_PID) {
					(void) pututxline(up);
					/*
					 * Now attempt to add to the end of the
					 * wtmp and wtmpx files.  Do not create
					 * if they don't already exist.
					 */
					updwtmpx("wtmpx", up);
				} else {
					if (modutx(up) == NULL) {
						syslog(LOG_INFO,
							    "\tmodutx failed");
						/*
						 * Since modutx failed we'll
						 * write out the new entry
						 * ourselves.
						 */
						(void) pututxline(up);
						updwtmpx("wtmpx", up);
					}
				}

				endutxent();

				return (PAM_SUCCESS);
			}
		}

		endutxent();
		return (__NOENTRY);
	}

}

/*
 * __setproc_cred - A utility function used to set the unix credentials of the
 *		    current process.  This function is NOT meant to be part of
 *		    the official PAM API, and only serves as a convenience
 *		    function.
 *
 *	"user" is the current username.
 *	The following flags may be set in the "flags" field:
 *
 *		__INITGROUPS	Initialize the supplementary group access list.
 *		__SETGROUPS 	Set the supplementary group access list.
 *		__SETEGID 	Set the effective group ID only.
 *		__SETGID 	Set the real and effective group IDs.
 *		__SETEUID 	Set the effective user ID only.
 *		__SETUID 	Set the real and effective user IDs.
 *		__SETEID 	Set the effective user and group IDs.
 *		__SETRID 	Set the real and effective user and group IDs.
 *
 *	"uid" and "gid" are the values of the user ID and group ID respectively.
 * 	"ngroups" is the number of supplementary groups.
 *	"grouplist" is a pointer to the list of supplementary groups.
 *
 *	Upon successful completion, PAM_SUCCESS is returned.
 *	Error values may include:
 *
 *		__BAD_GID		Invalid group ID
 *		__INITGROUP_FAIL	Initialization of group ID's failed
 *		__BAD_UID		Invalid user ID
 *		__SETGROUP_FAIL	Set of group ID's failed
 */

int
__setproc_cred(
	char	*user,
	int	flags,
	uid_t	uid,
	gid_t	gid,
	int	ngroups,
	gid_t	*grouplist)
{

	int	err = 0;
	char	messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	/*
	 * Set the credentials
	 */

	/* set the effective GID */
	if (flags & __SETEGID) {
		if (setegid(gid) == -1) {
			sprintf(messages[0], PAM_MSG(NULL, 29,
				"pam_sm_setcred: %s\n"), strerror(errno));
		return (__BAD_GID);
		}
	}

	/* set the real (and effective) GID */
	if (flags & __SETGID) {
		if (setgid(gid) == -1) {
			sprintf(messages[0], PAM_MSG(NULL, 30,
				"setproc_cred: %s\n"), strerror(errno));
			return (__BAD_GID);
		}
	}

	/*
	 * Initialize the supplementary group access list.
	 */
	if (!user)
		return (__INITGROUP_FAIL);
	if (flags & __INITGROUPS) {
		if (initgroups(user, gid) == -1) {
			sprintf(messages[0], PAM_MSG(NULL, 29,
				"pam_sm_setcred: %s\n"), strerror(errno));
			return (__INITGROUP_FAIL);
		}
	}

	/*
	 * Set the supplementary group access list.
	 */
	if (flags & __SETGROUPS) {
		if (setgroups(ngroups, (gid_t *)grouplist) == -1) {
			sprintf(messages[0], PAM_MSG(NULL, 29,
				"pam_sm_setcred: %s\n"), strerror(errno));
			return (__SETGROUP_FAIL);
		}
	}

	/*
	 * Set the user id
	 */

	/* set the effective UID */
	if (flags & __SETEUID) {
		if (seteuid(uid) == -1) {
			sprintf(messages[0], PAM_MSG(NULL, 29
				"pam_sm_setcred: %s\n"), strerror(errno));
			return (__BAD_UID);
		}
	}

	/* set the real (and effective) UID */
	if (flags & __SETUID) {
		if (setuid(uid) == -1) {
			sprintf(messages[0], PAM_MSG(NULL, 29,
				"pam_sm_setcred: %s\n"), strerror(errno));
			return (__BAD_UID);
		}
	}

	return (PAM_SUCCESS);

}

#endif
