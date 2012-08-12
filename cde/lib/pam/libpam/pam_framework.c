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
/* $XConsortium: pam_framework.c /main/6 1996/11/01 10:12:32 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */



#include <syslog.h>

#ifdef sun
#ident  "@(#)pam_framework.c 1.74     96/02/15 SMI"
#include <dlfcn.h>
#endif

#ifdef   hpV4
#include <dl.h>
#endif



#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include "pam_impl.h"
#include "pam_loc.h"

#include <X11/Xos.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#ifdef sun
#define	dlopen 	_dlopen
#define	dlclose _dlclose
#define	dlsym 	_dlsym
#define	dlerror _dlerror
#endif

/*	PAM debugging	*/
#define	PAM_DEBUG	"/etc/pam_debug"


static int	pam_debug = 0;

/* functions to dynamically load modules */
static int		load_modules(pam_handle_t *, int, char *);
#ifdef sun
static void 		*open_module(char *);
static int		load_function(void *, char *, int (**func)());
#endif
#ifdef hpV4
static shl_t		open_module(char *);
static int		load_function(shl_t, char *, int (**func)());
#endif

/* functions to read and store the pam.conf configuration file */
static int		open_pam_conf(struct pam_fh **);
static void		close_pam_conf(struct pam_fh *);
static int		read_pam_conf(pam_handle_t *);
static int 		get_pam_conf_entry(struct pam_fh *, pamtab **);
static char		*read_next_token(char **);
static char		*nextline(struct pam_fh *);

/* functions to clean up and free memory */
static void		clean_up(pam_handle_t *);
static void		free_pamconf(pamtab *);
static void		free_pam_conf_info(pam_handle_t *);

/*
 *			pam_XXXXX routines
 *
 *	These are the entry points to the authentication switch
 */

/*
 * pam_start		- initiate an authentication transaction and
 *			  set parameter values to be used during the
 *			  transaction
 */

int
pam_start(
	const char *service,
	const char *user,
	const struct pam_conv	*pam_conv,
	pam_handle_t **pamh)
{
	struct	stat	statbuf;
	int	err;

	/*  turn on PAM debug if "magic" file exists  */

	if (stat(PAM_DEBUG, &statbuf) == 0) {
		pam_debug = 1;
		openlog("PAM", LOG_CONS|LOG_NDELAY, LOG_AUTH);
	}

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_start(%s %s)",
			service, (user)?user:"no-user");

	*pamh = (struct pam_handle *) calloc(1, sizeof (struct pam_handle));
	if (*pamh == NULL)
		return (PAM_BUF_ERR);

	if ((err = pam_set_item(*pamh, PAM_SERVICE, (void *)service))
		    != PAM_SUCCESS) {
		clean_up(*pamh);
		*pamh = NULL;
		return (err);
	}

	if ((err = pam_set_item(*pamh, PAM_USER, (void *) user))
			!= PAM_SUCCESS) {
		clean_up(*pamh);
		*pamh = NULL;
		return (err);
	}

	if ((err = pam_set_item(*pamh, PAM_CONV, (void *) pam_conv))
	    != PAM_SUCCESS) {
		clean_up(*pamh);
		*pamh = NULL;
		return (err);
	}

	/* read all the entries from pam.conf */
	if ((err = read_pam_conf (*pamh)) != PAM_SUCCESS) {
		clean_up (*pamh);
		*pamh = NULL;
		return (err);
	}

	return (PAM_SUCCESS);
}

/*
 * pam_end - terminate an authentication transaction
 */

int
pam_end(pam_handle_t *pamh, int pam_status)
{
	struct pam_module_data *psd, *p;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_end(): status = %s",
			pam_strerror(pamh, pam_status));

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	/* call the cleanup routines for module specific data */

	psd = pamh->ssd;
	while (psd) {
		if (psd->cleanup) {
			psd->cleanup(pamh, psd->data, pam_status);
		}
		p = psd;
		psd = p->next;
		free(p->module_data_name);
		free(p);
	}
	pamh->ssd = NULL;

	clean_up(pamh);

	/*  end syslog reporting  */

	if (pam_debug)
		closelog();

	return (PAM_SUCCESS);
}

/*
 * pam_set_item		- set the value of a parameter that can be
 *			  retrieved via a call to pam_get_item()
 */

int
pam_set_item(
	pam_handle_t 	*pamh,
	int 		item_type,
	const void 	*item)
{
	struct pam_item *pip;
	int	size;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_set_item(%d)", item_type);

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);
	/*
	 * Check that item_type is within valid range
	 */

	if (item_type <= 0 || item_type >= PAM_MAX_ITEMS)
		return (PAM_SYMBOL_ERR);

	pip = &(pamh->ps_item[item_type]);

	switch (item_type) {
		case PAM_AUTHTOK:
		case PAM_OLDAUTHTOK:
			if (pip->pi_addr != NULL)
				memset(pip->pi_addr, 0, pip->pi_size);
		case PAM_SERVICE:
		case PAM_USER:
		case PAM_TTY:
		case PAM_RHOST:
		case PAM_RUSER:
		case PAM_USER_PROMPT:
			if (pip->pi_addr != NULL) {
				free(pip->pi_addr);
			}

			if (item == NULL) {
				pip->pi_addr = NULL;
				pip->pi_size = 0;
			} else {
				pip->pi_addr = strdup((char *)item);
				if (pip->pi_addr == NULL) {
					pip->pi_size = 0;
					return (PAM_BUF_ERR);
				}
				pip->pi_size = strlen(pip->pi_addr);
			}
			break;
		case PAM_CONV:
			if (pip->pi_addr != NULL)
				free(pip->pi_addr);
			size = sizeof (struct pam_conv);
			if ((pip->pi_addr = (void *) calloc(1, size)) == NULL)
				return (PAM_BUF_ERR);
			if (item != NULL)
				(void) memcpy(pip->pi_addr, item,
						(unsigned int) size);
			else
				memset(pip->pi_addr, 0, size);
			pip->pi_size = size;
			break;
		default:
			return (PAM_SYMBOL_ERR);
	}

	return (PAM_SUCCESS);
}

/*
 * pam_get_item		- read the value of a parameter specified in
 *			  the call to pam_set_item()
 */

int
pam_get_item(
	const pam_handle_t	*pamh,
	int			item_type,
	void			**item)
{
	struct pam_item *pip;

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	if (item_type <= 0 || item_type >= PAM_MAX_ITEMS)
		return (PAM_SYMBOL_ERR);

	pip = (struct pam_item *)&(pamh->ps_item[item_type]);

	*item = pip->pi_addr;

	return (PAM_SUCCESS);
}


/*
 * Get the value of PAM_USER. If not set, then use the convenience function
 * to prompt for the user. Use prompt if specified, else use PAM_USER_PROMPT
 * if it is set, else use default.
 */

int
pam_get_user(
	pam_handle_t *pamh,		/* PAM handle */
	char **user, 			/* User Name */
	const char *prompt_override)	/* Prompt */
{
	int	status;
	char	*prompt = NULL;

	struct pam_response *ret_resp = (struct pam_response *)0;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	if ((status = pam_get_item(pamh, PAM_USER, (void **)user))
							!= PAM_SUCCESS) {
		return (status);
	}

	/* if the user is set, return it */

	if (*user != NULL && *user[0] != '\0') {
		return (PAM_SUCCESS);
	}

	/*
	 * if the module is requesting a special prompt, use it.
	 * else use PAM_USER_PROMPT.
	 */

	if (prompt_override != NULL) {
		prompt = (char *)prompt_override;
	} else {
		status = pam_get_item(pamh, PAM_USER_PROMPT, (void**)&prompt);
		if (status != PAM_SUCCESS) {
			return (status);
		}
	}

	/* if the prompt is not set, use default */

	if (prompt == NULL || prompt[0] == '\0') {
		prompt = PAM_MSG(pamh, 32, "Please enter user name: ");
	}

	/* prompt for the user */

	strncpy(messages[0], prompt, sizeof (messages[0]));

	for (;;) {
		status = __pam_get_input(pamh, PAM_PROMPT_ECHO_ON, 1,
			messages, NULL, &ret_resp);

		if (status != PAM_SUCCESS) {
			return (status);
		}

		if (ret_resp->resp && ret_resp->resp[0] != '\0') {
			break;
		}
	}

	/* set PAM_USER */

	status = pam_set_item(pamh, PAM_USER, ret_resp->resp);
	__pam_free_resp(1, ret_resp);
	if (status != PAM_SUCCESS) {
		return (status);
	}

	/*
	 * finally, get PAM_USER. We have to call pam_get_item to get
	 * the value of user because pam_set_item mallocs the memory.
	 */

	status = pam_get_item(pamh, PAM_USER, (void**)user);
	return (status);
}

/*
 * Set module specific data
 */
pam_set_data(
	pam_handle_t *pamh,		/* PAM handle */
	const char *module_data_name,	/* unique module data name */
	const void *data,		/* the module specific data */
	void (*cleanup)(pam_handle_t *pamh, void *data, int pam_end_status)
)
{
	struct pam_module_data *psd;

	if (pamh == NULL || module_data_name == NULL)
		return (PAM_SYSTEM_ERR);

	/* check if module data already exists */

	for (psd = pamh->ssd; psd; psd = psd->next) {
		/*
		 * do not free any data because it is not known
		 * if the data was originally malloc'd.
		 */
		if (strcmp(psd->module_data_name, module_data_name) == 0) {
			psd->data = (void *)data;
			psd->cleanup = cleanup;
			return (PAM_SUCCESS);
		}
	}

	psd = malloc(sizeof (struct pam_module_data));
	if (psd == NULL)
		return (PAM_BUF_ERR);

	psd->module_data_name = strdup(module_data_name);
	if (psd->module_data_name == NULL) {
		free(psd);
		return (PAM_BUF_ERR);
	}

	psd->data = (void *)data;
	psd->cleanup = cleanup;
	psd->next = pamh->ssd;
	pamh->ssd = psd;
	return (PAM_SUCCESS);
}

/*
 * get module specific data
 */

int
pam_get_data(
	const pam_handle_t *pamh,
	const char *module_data_name,
	void **data
)
{
	struct pam_module_data *psd;

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	for (psd = pamh->ssd; psd; psd = psd->next) {
		if (strcmp(psd->module_data_name, module_data_name) == 0) {
			*data = psd->data;
			return (PAM_SUCCESS);
		}
	}

	return (PAM_NO_MODULE_DATA);
}

/*
 * PAM error strings
 *
 * XXX: Make sure these match the errors in pam_appl.h !!!!!!!
 */

static char *pam_error_strings [PAM_TOTAL_ERRNUM] = {
/* PAM_SUCCESS */	"Success",
/* PAM_OPEN_ERR */	"Dlopen failure",
/* PAM_SYMBOL_ERR */	"Symbol not found",
/* PAM_SERVICE_ERR */	"Error in underlying service module",
/* PAM_SYSTEM_ERR */	"System error",
/* PAM_BUF_ERR */	"Memory buffer error",
/* PAM_CONV_ERR */	"Conversation failure",
/* PAM_PERM_DENIED */	"Permission denied",
/* PAM_MAXTRIES */	"Maximum number of attempts exceeded",
/* PAM_AUTH_ERR */	"Authentication failed",
/* PAM_AUTHTOKEN_REQD */	"Get new authentication token",
/* PAM_CRED_INSUFFICIENT */	"Insufficient credentials",
/* PAM_AUTHINFO_UNAVAIL */	"Can not retrieve authentication info",
/* PAM_USER_UNKNOWN */		"No account present for user",
/* PAM_CRED_UNAVAIL */		"Can not retrieve user credentials",
/* PAM_CRED_EXPIRED */		"User credentials have expired",
/* PAM_CRED_ERR */		"Failure setting user credentials",
/* PAM_ACCT_EXPIRED */		"User account has expired",
/* PAM_AUTHTOK_EXPIRED */	"User password has expired",
/* PAM_SESSION_ERR */		"Can not make/remove entry for session",
/* PAM_AUTHTOK_ERR */		"Authentication token manipulation error",
/* PAM_AUTHTOK_RECOVERY_ERR */	"Authentication token can not be recovered",
/* PAM_AUTHTOK_LOCK_BUSY */	"Authentication token lock busy",
/* PAM_AUTHTOK_DISABLE_AGING */	"Authentication token aging disabled",
/* PAM_NO_MODULE_DATA */	"Module specific data not found",
/* PAM_IGNORE */		"Ignore module",
/* PAM_ABORT */			"General PAM failure ",
/* PAM_TRY_AGAIN */		"Password update failed - Try again "
};

/*
 * PAM equivalent to strerror()
 */
char *
pam_strerror(pam_handle_t *pamh, int errnum)
{
	if (errnum < 0 || errnum >= PAM_TOTAL_ERRNUM)
	    return (PAM_MSG(pamh, 28, "Unknown error"));
	else
	    return (PAM_MSG(pamh, errnum, (char *) pam_error_strings[errnum]));
}

/*
 * pam_authenticate - authenticate a user
 */

int
pam_authenticate(
	pam_handle_t	*pamh,
	int	flags)
{
	int			error = PAM_AUTH_ERR;
	int			first_error = PAM_AUTH_ERR;
	int			first_required_error = PAM_AUTH_ERR;
	int			required_module_failed = 0;
	int			optional_module_failed = 0;
	int			success = 0;
	pamtab			*modulep;
	struct auth_module	*authp;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_authenticate()");

	if ((error = load_modules(pamh, PAM_AUTH_MODULE, PAM_SM_AUTHENTICATE))
						!= PAM_SUCCESS) {
		if (pam_debug)
			syslog(LOG_DEBUG,
			    "pam_authenticate: load_modules failed");
		return (error);
	}

	modulep = pamh->pam_conf_info[PAM_AUTH_MODULE];
	while (modulep) {
		authp = (struct auth_module *)(modulep->function_ptr);
		if (authp && authp->pam_sm_authenticate) {
			error = authp->pam_sm_authenticate(pamh, flags,
				    modulep->module_argc,
				    (const char **)modulep->module_argv);

			switch (error) {
			case PAM_IGNORE:
				/* do nothing */
				break;
			case PAM_SUCCESS:
				if ((modulep->pam_flag & PAM_SUFFICIENT) &&
				    !required_module_failed) {
					pam_set_item(pamh,
							PAM_AUTHTOK,
							NULL);
					return (PAM_SUCCESS);
				}
				success = 1;
				break;
			default:
				if (modulep->pam_flag & PAM_REQUIRED) {
					if (!required_module_failed)
						first_required_error = error;
					required_module_failed++;
				} else {
					if (!optional_module_failed)
						first_error = error;
					optional_module_failed++;
				}
				syslog(LOG_DEBUG,
					"pam_authenticate: error %s",
						pam_strerror(pamh, error));
				break;
			}
		}
		modulep = modulep->next;
	}

	/* this will memset the password memory to 0 */
	pam_set_item(pamh, PAM_AUTHTOK, NULL);

	if (required_module_failed)
		return (first_required_error);
	else if (success == 0)
		return (first_error);
	else
		return (PAM_SUCCESS);

}

/*
 * pam_setcred - modify or retrieve user credentials
 */
int
pam_setcred(
	pam_handle_t	*pamh,
	int	flags)
{
	int			error = PAM_CRED_ERR;
	int			first_error = PAM_CRED_ERR;
	int			first_required_error = PAM_CRED_ERR;
	int			required_module_failed = 0;
	int			optional_module_failed = 0;
	int			success = 0;
	pamtab			*modulep;
	struct auth_module	*authp;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_setcred()");

	if ((error = load_modules(pamh, PAM_AUTH_MODULE, PAM_SM_SETCRED))
						!= PAM_SUCCESS) {
		if (pam_debug)
			syslog(LOG_DEBUG, "pam_setcred: load_modules failed");
		return (error);
	}

	modulep = pamh->pam_conf_info[PAM_AUTH_MODULE];
	while (modulep) {
		authp = (struct auth_module *)(modulep->function_ptr);
		if (authp && authp->pam_sm_setcred) {
			error = authp->pam_sm_setcred(pamh, flags,
				modulep->module_argc,
				(const char **) modulep->module_argv);

			switch (error) {
			case PAM_IGNORE:
				/* do nothing */
				break;
			case PAM_SUCCESS:
				/*
				 * pam_setcred() should not look at the
				 * SUFFICIENT flag because it is only
				 * applicable to pam_authenticate().
				 */
				success = 1;
				break;
			default:
				if (modulep->pam_flag & PAM_REQUIRED) {
					if (!required_module_failed)
						first_required_error = error;
					required_module_failed++;
				} else {
					if (!optional_module_failed)
						first_error = error;
					optional_module_failed++;
				}
				syslog(LOG_DEBUG,
					"pam_setcred: error %s",
						pam_strerror(pamh, error));
				break;
			}
		}
		modulep = modulep->next;
	}

	if (required_module_failed)
		return (first_required_error);
	else if (success == 0)
		return (first_error);
	else
		return (PAM_SUCCESS);

}

/*
 * pam_acct_mgmt - check password aging, account expiration
 */

int
pam_acct_mgmt(
	pam_handle_t	*pamh,
	int 	flags)
{
	int			error = PAM_ACCT_EXPIRED;
	int			first_error = PAM_ACCT_EXPIRED;
	int			first_required_error = PAM_ACCT_EXPIRED;
	int			required_module_failed = 0;
	int			optional_module_failed = 0;
	int			success = 0;
	pamtab			*modulep;
	struct account_module	*accountp;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_acct_mgmt()");

	if ((error = load_modules(pamh, PAM_ACCOUNT_MODULE, PAM_SM_ACCT_MGMT))
						!= PAM_SUCCESS) {
		if (pam_debug)
			syslog(LOG_DEBUG,
			    "pam_acct_mgmt: load_modules failed");
		return (error);
	}

	modulep = pamh->pam_conf_info[PAM_ACCOUNT_MODULE];
	while (modulep) {
		accountp = (struct account_module *)(modulep->function_ptr);
		if (accountp && accountp->pam_sm_acct_mgmt) {
			error = accountp->pam_sm_acct_mgmt(pamh, flags,
			    modulep->module_argc,
			    (const char **) modulep->module_argv);

			switch (error) {
			case PAM_IGNORE:
				/* do nothing */
				break;
			case PAM_SUCCESS:
				if ((modulep->pam_flag & PAM_SUFFICIENT) &&
				    !required_module_failed)
					return (PAM_SUCCESS);
				success = 1;
				break;
			default:
				if (modulep->pam_flag & PAM_REQUIRED) {
					if (!required_module_failed)
						first_required_error = error;
					required_module_failed++;
				} else {
					if (!optional_module_failed)
						first_error = error;
					optional_module_failed++;
				}
				syslog(LOG_DEBUG,
					"pam_acct_mgmt: error %s",
					pam_strerror(pamh, error));
				break;
			}
		}
		modulep = modulep->next;
	}

	if (required_module_failed)
		return (first_required_error);
	else if (success == 0)
		return (first_error);
	else
		return (PAM_SUCCESS);

}

/*
 * pam_open_session - begin session management
 */

int
pam_open_session(
	pam_handle_t    *pamh,
	int	flags)
{
	int			error = PAM_SESSION_ERR;
	int			first_error = PAM_SESSION_ERR;
	int			first_required_error = PAM_SESSION_ERR;
	int			required_module_failed = 0;
	int			optional_module_failed = 0;
	int			success = 0;
	pamtab			*modulep;
	struct session_module	*sessionp;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_open_session()");

	if ((error = load_modules(pamh, PAM_SESSION_MODULE,
				PAM_SM_OPEN_SESSION)) != PAM_SUCCESS) {
		if (pam_debug)
			syslog(LOG_DEBUG,
			    "pam_open_session: load_modules failed");
		return (error);
	}

	modulep = pamh->pam_conf_info[PAM_SESSION_MODULE];
	while (modulep) {
		sessionp = (struct session_module *)(modulep->function_ptr);
		if (sessionp && sessionp->pam_sm_open_session) {
			error = sessionp->pam_sm_open_session(pamh, flags,
				modulep->module_argc,
				(const char **) modulep->module_argv);

			switch (error) {
			case PAM_IGNORE:
				/* do nothing */
				break;
			case PAM_SUCCESS:
				if ((modulep->pam_flag & PAM_SUFFICIENT) &&
				    !required_module_failed)
					return (PAM_SUCCESS);
				success = 1;
				break;
			default:
				if (modulep->pam_flag & PAM_REQUIRED) {
					if (!required_module_failed)
						first_required_error = error;
					required_module_failed++;
				} else {
					if (!optional_module_failed)
						first_error = error;
					optional_module_failed++;
				}
				syslog(LOG_DEBUG,
					"pam_open_session: error %s",
						pam_strerror(pamh, error));
				break;
			}
		}
		modulep = modulep->next;
	}

	if (required_module_failed)
		return (first_required_error);
	else if (success == 0)
		return (first_error);
	else
		return (PAM_SUCCESS);

}

/*
 * pam_close_session - terminate session management
 */

int
pam_close_session(
	pam_handle_t	*pamh,
	int	flags)
{
	int			error = PAM_SESSION_ERR;
	int			first_error = PAM_SESSION_ERR;
	int			first_required_error = PAM_SESSION_ERR;
	int			required_module_failed = 0;
	int			optional_module_failed = 0;
	int			success = 0;
	pamtab			*modulep;
	struct session_module	*sessionp;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_close_session()");

	if ((error = load_modules(pamh, PAM_SESSION_MODULE,
				PAM_SM_CLOSE_SESSION)) != PAM_SUCCESS) {
		if (pam_debug)
			syslog(LOG_DEBUG,
			    "pam_close_session: load_modules failed");
		return (error);
	}

	modulep = pamh->pam_conf_info[PAM_SESSION_MODULE];
	while (modulep) {
		sessionp = (struct session_module *)(modulep->function_ptr);
		if (sessionp && sessionp->pam_sm_close_session) {
			error = sessionp->pam_sm_close_session(
				pamh, flags, modulep->module_argc,
				(const char **) modulep->module_argv);

			switch (error) {
			case PAM_IGNORE:
				/* do nothing */
				break;
			case PAM_SUCCESS:
				if ((modulep->pam_flag & PAM_SUFFICIENT) &&
				    !required_module_failed)
					return (PAM_SUCCESS);
				success = 1;
				break;
			default:
				if (modulep->pam_flag & PAM_REQUIRED) {
					if (!required_module_failed)
						first_required_error = error;
					required_module_failed++;
				} else {
					if (!optional_module_failed)
						first_error = error;
					optional_module_failed++;
				}
				syslog(LOG_DEBUG,
					"pam_close_session: error %s",
						pam_strerror(pamh, error));
				break;
			}
		}
		modulep = modulep->next;
	}

	if (required_module_failed)
		return (first_required_error);
	else if (success == 0)
		return (first_error);
	else
		return (PAM_SUCCESS);
}

/*
 * pam_chauthtok - change user authentication token
 */

int
pam_chauthtok(
	pam_handle_t		*pamh,
	int			flags)
{
	int			error = PAM_AUTHTOK_ERR;
	int			first_error = PAM_AUTHTOK_ERR;
	int			first_required_error = PAM_AUTHTOK_ERR;
	int			required_module_failed = 0;
	int			optional_module_failed = 0;
	int			success = 0;
	int			i;
	int			sm_flags;
	pamtab			*modulep;
	struct password_module	*passwdp;

	if (pam_debug)
		syslog(LOG_DEBUG, "pam_chauthtok()");

	/* do not let apps use PAM_PRELIM_CHECK or PAM_UPDATE_AUTHTOK */
	if (flags & PAM_PRELIM_CHECK || flags & PAM_UPDATE_AUTHTOK)
		return (PAM_SYMBOL_ERR);

	if ((error = load_modules(pamh, PAM_PASSWORD_MODULE, PAM_SM_CHAUTHTOK))
						!= PAM_SUCCESS) {
		if (pam_debug)
			syslog(LOG_DEBUG,
			    "pam_chauthtok: load_modules failed");
		return (error);
	}

	for (i = 1; i <= 2; i++) {
	    switch (i) {
	    case 1:
		/* first time thru loop do preliminary check */
		sm_flags = flags | PAM_PRELIM_CHECK;
		break;
	    case 2:
		/* 2nd time thru loop update passwords */
		success = 0;
		sm_flags = flags | PAM_UPDATE_AUTHTOK;
		break;
	    }

	    modulep = pamh->pam_conf_info[PAM_PASSWORD_MODULE];
	    while (modulep) {
		passwdp = (struct password_module *)(modulep->function_ptr);
		if (passwdp && passwdp->pam_sm_chauthtok) {
			error = passwdp->pam_sm_chauthtok(pamh, sm_flags,
				modulep->module_argc,
				(const char **) modulep->module_argv);

			switch (error) {
			case PAM_IGNORE:
				/* do nothing */
				break;
			case PAM_SUCCESS:
				if (modulep->pam_flag & PAM_SUFFICIENT) {
					if (!(flags & PAM_PRELIM_CHECK) &&
					    !required_module_failed) {
						/*
						 * If updating passwords, the
						 * module is sufficient, and
						 * no required modules have
						 * failed, then memset the
						 * passwd memory to 0 and return
						 */
						pam_set_item(pamh,
							PAM_AUTHTOK, NULL);
						pam_set_item(pamh,
							PAM_OLDAUTHTOK, NULL);
						return (PAM_SUCCESS);
					}
				}
				success = 1;
				break;
			default:
				if (modulep->pam_flag & PAM_REQUIRED) {
					if (!required_module_failed)
						first_required_error = error;
					required_module_failed++;
				} else {
					if (!optional_module_failed)
						first_error = error;
					optional_module_failed++;
				}
				syslog(LOG_DEBUG,
					"pam_chauthtok: error %s",
						pam_strerror(pamh, error));
				break;
			}
		}
		modulep = modulep->next;
	    } /* while */

	    /* this will memset the password memory to 0 */
	    pam_set_item(pamh, PAM_AUTHTOK, NULL);
	    pam_set_item(pamh, PAM_OLDAUTHTOK, NULL);

	    if (required_module_failed)
		return (first_required_error);
	    else if (success == 0)
		return (first_error);
	    else if (sm_flags & PAM_UPDATE_AUTHTOK) {
		/*
		 * Only return PAM_SUCCESS if this is the second
		 * time through the loop.
		 */
		return (PAM_SUCCESS);
	    }

	/*
	 * If we reach here, the prelim check succeeded.
	 * Go thru the loop again to update the passwords.
	 */

	} /* for */

	/* should never reach this point!!! */
}

/*
 * load_modules()
 * open_module()
 * load_function()
 *
 * Routines to load a requested module on demand
 */

/*
 * load_modules - load the requested module.
 *		  if the dlopen or dlsym fail, then
 *		  the module is ignored.
 */

static int
load_modules(pam_handle_t *pamh, int type, char *function_name)
{

#ifdef sun
	void *mh;
#endif

#ifdef hpV4
	shl_t mh;
#endif

	pamtab *pam_entry;
	struct auth_module *authp;
	struct account_module *accountp;
	struct session_module *sessionp;
	struct password_module *passwdp;
	int loading_functions = 0; /* are we currently loading functions? */

	if ((pam_entry = pamh->pam_conf_info[type]) == NULL) {
		syslog(LOG_ERR, "load_modules: no module present");
		return (PAM_SYSTEM_ERR);
	}

	while (pam_entry != NULL) {
		if (pam_debug)
			syslog(LOG_DEBUG, "load_modules: %s",
				pam_entry->module_path);

		switch (type) {
		case PAM_AUTH_MODULE:

			/* if the function has already been loaded, return */
			authp = pam_entry->function_ptr;
			if (!loading_functions &&
				(((strcmp(function_name, PAM_SM_AUTHENTICATE)
									== 0) &&
				authp && authp->pam_sm_authenticate) ||
				((strcmp(function_name, PAM_SM_SETCRED) == 0) &&
				authp && authp->pam_sm_setcred))) {
				return (PAM_SUCCESS);
			}

			/* function has not been loaded yet */
			loading_functions = 1;
			authp = (struct auth_module *)
				calloc(1, sizeof (struct auth_module));
			if (authp == NULL)
				return (PAM_BUF_ERR);

			/* if open_module fails, continue */
			if ((mh = open_module
				(pam_entry->module_path)) == NULL)
				syslog(LOG_ERR,
				"load_modules: can not open module %s",
				pam_entry->module_path);

			/* load the authentication function */
			if (strcmp(function_name, PAM_SM_AUTHENTICATE) == 0) {
				if (load_function(mh, PAM_SM_AUTHENTICATE,
				    &authp->pam_sm_authenticate) !=
								PAM_SUCCESS) {
					/* ignore if dlsym fails */
					free(authp);
					break;
				}

			/* load the setcred function */
			} else if (strcmp(function_name, PAM_SM_SETCRED) == 0) {
				if (load_function(mh, PAM_SM_SETCRED,
				    &authp->pam_sm_setcred) != PAM_SUCCESS) {
					/* ignore if dlsym fails */
					free(authp);
					break;
				}
			}
			pam_entry->function_ptr = authp;
			break;
		case PAM_ACCOUNT_MODULE:
			accountp = pam_entry->function_ptr;
			if (!loading_functions &&
			    (strcmp(function_name, PAM_SM_ACCT_MGMT) == 0) &&
			    accountp && accountp->pam_sm_acct_mgmt) {
				return (PAM_SUCCESS);
			}

			loading_functions = 1;
			accountp = (struct account_module *)
				calloc(1, sizeof (struct account_module));
			if (accountp == NULL)
				return (PAM_BUF_ERR);

			/* if open_module fails, continue */
			if ((mh = open_module
				(pam_entry->module_path)) == NULL)
				syslog(LOG_ERR,
				"load_modules: can not open module %s",
				pam_entry->module_path);

			if (load_function(mh, PAM_SM_ACCT_MGMT,
				&accountp->pam_sm_acct_mgmt)
				!= PAM_SUCCESS) {
				syslog(LOG_ERR,
				"load_modules: pam_sm_acct_mgmt() missing");
				free(accountp);
				break;
			}
			pam_entry->function_ptr = accountp;
			break;
		case PAM_SESSION_MODULE:
			sessionp = pam_entry->function_ptr;
			if (!loading_functions &&
			    (((strcmp(function_name, PAM_SM_OPEN_SESSION)
								== 0) &&
			    sessionp && sessionp->pam_sm_open_session) ||
			    ((strcmp(function_name, PAM_SM_CLOSE_SESSION)
								== 0) &&
			    sessionp && sessionp->pam_sm_close_session))) {
				return (PAM_SUCCESS);
			}

			loading_functions = 1;
			sessionp = (struct session_module *)
				calloc(1, sizeof (struct session_module));
			if (sessionp == NULL)
				return (PAM_BUF_ERR);

			/* if open_module fails, continue */
			if ((mh = open_module
				(pam_entry->module_path)) == NULL)
				syslog(LOG_ERR,
				"load_modules: can not open module %s",
				pam_entry->module_path);

			if ((strcmp(function_name, PAM_SM_OPEN_SESSION) == 0) &&
			    load_function(mh, PAM_SM_OPEN_SESSION,
				&sessionp->pam_sm_open_session)
				!= PAM_SUCCESS) {
				free(sessionp);
				break;
			} else if ((strcmp(function_name,
					PAM_SM_CLOSE_SESSION) == 0) &&
				    load_function(mh, PAM_SM_CLOSE_SESSION,
					&sessionp->pam_sm_close_session)
					!= PAM_SUCCESS) {
				free(sessionp);
				break;
			}
			pam_entry->function_ptr = sessionp;
			break;
		case PAM_PASSWORD_MODULE:
			passwdp = pam_entry->function_ptr;
			if (!loading_functions &&
			    (strcmp(function_name, PAM_SM_CHAUTHTOK) == 0) &&
			    passwdp && passwdp->pam_sm_chauthtok) {
				return (PAM_SUCCESS);
			}

			loading_functions = 1;
			passwdp = (struct password_module *)
				calloc(1, sizeof (struct password_module));
			if (passwdp == NULL)
				return (PAM_BUF_ERR);

			/* if open_module fails, continue */
			if ((mh = open_module
				(pam_entry->module_path)) == NULL)
				syslog(LOG_ERR,
				"load_modules: can not open module %s",
				pam_entry->module_path);

			if (load_function(mh, PAM_SM_CHAUTHTOK,
				&passwdp->pam_sm_chauthtok) != PAM_SUCCESS) {
				free(passwdp);
				break;
			}
			pam_entry->function_ptr = passwdp;
			break;
		default:
			if (pam_debug) {
				syslog(LOG_DEBUG,
				"load_modules: unsupported type %d", type);
			}
			break;
		}

		pam_entry = pam_entry->next;
	} /* while */

	return (PAM_SUCCESS);
}

/*
 * open_module		- Open the module first checking for
 *			  propers modes and ownerships on the file.
 */

#ifdef sun
static void *
open_module(char *module_so)
{
#endif
#ifdef hpV4
static shl_t
open_module(char *module_so)
{
#endif
	struct stat	stb;
	char		*errmsg;
#ifdef sun
	void		*lfd;
#endif /* sun */
#ifdef hpV4
	shl_t		lfd;
#endif /* hpV4 */

	/*
	 * Stat the file so we can check modes and ownerships
	 */
	if (stat(module_so, &stb) < 0) {
		syslog(LOG_ALERT, "open_module: stat(%s) failed: %s",
				module_so, strerror(errno));
		return (NULL);
	}

	/*
	 * Check the ownership of the file
	 */
	if (stb.st_uid != (uid_t)0) {
		syslog(LOG_ALERT,
			"open_module: Owner of the module %s is not root",
			module_so);
		return (NULL);
	}

	/*
	 * Check the modes on the file
	 */
	if (stb.st_mode&S_IWGRP) {
		syslog(LOG_ALERT,
			"open_module: module %s writable by group",
			module_so);
		return (NULL);
	}
	if (stb.st_mode&S_IWOTH) {
		syslog(LOG_ALERT,
			"open_module: module %s writable by world", module_so);
		return (NULL);
	}

	/*
	 * Perform the dlopen()
	 */
#ifdef sun
	lfd = (void *) dlopen(module_so, RTLD_LAZY);
#endif /* sun */

#ifdef hpV4
	lfd = shl_load(module_so, BIND_DEFERRED, 0L);
#endif /* hpV4 */

	if (lfd == NULL) {
		if (pam_debug) {
			errmsg = (char *) strerror(errno);
			syslog(LOG_DEBUG, "open_module: %s failed: %s",
				module_so, errmsg);
		}
		return (NULL);
	}

	return (lfd);

}

/*
 * load_function - call dlsym() to resolve the function address
 */
#ifdef sun
static int
load_function(void *lfd, char *name, int (**func)())
{
#endif
#ifdef hpV4
static int
load_function(shl_t lfd, char *name, int (**func)())
{
#endif
	char *errmsg = NULL;

#ifdef hpV4
void *proc_addr = NULL;
int stat;

#endif

	if (lfd == NULL)
		return (PAM_SYMBOL_ERR);

	/*
	 * The APIs for opening the shared objects are palatform dependent
	 * and hence the ifdef platform
	 */
#ifdef sun
	*func = (int (*)())dlsym(lfd, name);
	if (*func == NULL) {
		if (pam_debug) {
			errmsg = (char *) dlerror();
			syslog(LOG_DEBUG,
			"dlsym failed %s: error %s",
			name, errmsg != NULL ? errmsg : "");
		}
		return (PAM_SYMBOL_ERR);
	}
#endif

#ifdef hpV4

	stat = shl_findsym(&lfd, name, TYPE_PROCEDURE, proc_addr);

	*func = (int (*)())proc_addr;

	if (stat) {
		if (pam_debug) {
			strerror_r(errno, errmsg, MAX_ERRMESSAGE_LENGTH);
			syslog(LOG_DEBUG, "shl_findsym failed %s: error %s",
				name, errmsg != NULL ? errmsg : "");
		}
		return (PAM_SYMBOL_ERR);
	}
#endif
	if (pam_debug) {
		syslog(LOG_DEBUG,
			"load_function: successful load of %s", name);
	}
	return (PAM_SUCCESS);
}

/*
 * open_pam_conf()
 * close_pam_conf()
 * read_pam_conf()
 * get_pam_conf_entry()
 * read_next_token()
 * nextline()
 *
 * Routines to read the pam.conf configuration file
 */

/*
 * open_pam_conf - open the pam.conf config file
 */

static int
open_pam_conf(struct pam_fh **pam_fh)
{
	struct stat	stb;

	/*
	 * Stat the file so we can check modes and ownerships
	 */
	if (stat(PAM_CONFIG, &stb) < 0) {
		syslog(LOG_ALERT, "open_pam_conf: stat(%s) failed: %s",
			PAM_CONFIG, strerror(errno));
		return (0);
	}

	/*
	 * Check the ownership of the file
	 */
	if (stb.st_uid != (uid_t)0) {
		syslog(LOG_ALERT,
		    "open_pam_conf: Owner of %s is not root", PAM_CONFIG);
		return (0);
	}

	/*
	 * Check the modes on the file
	 */
	if (stb.st_mode&S_IWGRP) {
		syslog(LOG_ALERT,
		    "open_pam_conf: %s writable by group", PAM_CONFIG);
		return (0);
	}
	if (stb.st_mode&S_IWOTH) {
		syslog(LOG_ALERT,
			"open_pam_conf: %s writable by world", PAM_CONFIG);
		return (0);
	}

	*pam_fh = calloc(1, sizeof (struct pam_fh));
	if (*pam_fh == NULL)
		return (0);

	(*pam_fh)->fconfig = fopen(PAM_CONFIG, "r");
	return ((*pam_fh)->fconfig != NULL);
}

/*
 * close_pam_conf - close pam.conf
 */

static void
close_pam_conf(struct pam_fh *pam_fh)
{

	if (pam_fh->fconfig == NULL) {
		free(pam_fh);
		return;
	}
	fclose(pam_fh->fconfig);
	free(pam_fh);
}

/*
 * read_pam_conf - read in each entry in pam.conf and store info
 *		   under the pam handle.
 */

static int
read_pam_conf(pam_handle_t *pamh)
{

	struct pam_fh *pam_fh = NULL;
	pamtab *pamentp = NULL;
	pamtab *pament_traverse = NULL;
	pamtab *new_head = NULL;
	char *service;
	int other_needed[PAM_NUM_MODULE_TYPES] = {1, 1, 1, 1};
	int error;

	if ((error = pam_get_item(pamh, PAM_SERVICE, (void **)&service))
							!= PAM_SUCCESS)
		goto out;

	if (open_pam_conf(&pam_fh) == 0) {
		error = PAM_SYSTEM_ERR;
		goto out;
	}

	while (get_pam_conf_entry(pam_fh, &pamentp)) {
		/*
		 * Look for the current service.
		 * If we find it, purge all "other" services
		 * that we have stored.  Keep loading "other"
		 * services if we still haven't found an entry
		 * for the current service.
		 */
		if (other_needed[pamentp->pam_type] == 1 &&
		    !strcasecmp(pamentp->pam_service, service)) {

			/*
			 * We found a service match.  Purge the "other"
			 * entries we have stored.
			 */
			new_head = pamh->pam_conf_info[pamentp->pam_type];
			pament_traverse = new_head;
			while (new_head != NULL &&
				!strcasecmp(new_head->pam_service, "other")) {
				new_head = new_head->next;
				free_pamconf(pament_traverse);
				pament_traverse = new_head;
			}
			pamh->pam_conf_info[pamentp->pam_type] = new_head;

			/* "other" entries no longer needed for this type */
			other_needed[pamentp->pam_type] = 0;
		}

		/* add service */
		if ((other_needed[pamentp->pam_type] == 1 &&
		    !strcasecmp(pamentp->pam_service, "other")) ||
		    (other_needed[pamentp->pam_type] == 0 &&
		    !strcasecmp(pamentp->pam_service, service))) {

			pamentp->next = NULL;
			if (pamh->pam_conf_info[pamentp->pam_type] == NULL) {
				pamh->pam_conf_info[pamentp->pam_type] =
									pamentp;
			} else {
				pament_traverse =
					pamh->pam_conf_info[pamentp->pam_type];
				while (pament_traverse->next != NULL)
					pament_traverse = pament_traverse->next;
				pament_traverse->next = pamentp;
			}
		} else
			free_pamconf(pamentp);

		pamentp = NULL;
	}

	(void) close_pam_conf(pam_fh);
	error = PAM_SUCCESS;
out:
	return (error);
}

/*
 * get_pam_conf_entry - get a pam.conf entry
 *	return 0: no more entries or error
 *	return non 0: good entry
 *	all bad entries in pam.conf are ignored.
 */

static int
get_pam_conf_entry(struct pam_fh *pam_fh, pamtab **pam)
{
	char		*cp, *arg;
	int		argc;
	char		*tmp, *tmp_free;
	int		i;

	while (1) {
		if ((*pam = (pamtab *)calloc(1, sizeof (pamtab))) == NULL)
			return (0);

		cp = nextline(pam_fh);
		if (cp == (char *) 0)
			goto mal_formed;

		arg = read_next_token(&cp);
		if (arg == NULL) {
			syslog(LOG_CRIT,
			"illegal entry in pam.conf: missing field: %s", cp);
			free_pamconf (*pam);
			continue;
		}

		(*pam)->pam_service = strdup(arg);
		if ((*pam)->pam_service == NULL) {
			syslog(LOG_ERR, "strdup: out of memory");
			goto mal_formed;
		}

		if (cp == (char *) 0) {
			syslog(LOG_CRIT,
			"illegal entry in pam.conf: missing field: %s", cp);
			free_pamconf (*pam);
			continue;
		}

		arg = read_next_token(&cp);
		if (cp == (char *) 0) {
			syslog(LOG_CRIT,
			"illegal entry in pam.conf: missing field: %s", cp);
			free_pamconf (*pam);
			continue;
		}
		if (strcasecmp(arg, "auth") == 0)
			(*pam)->pam_type = PAM_AUTH_MODULE;
		else if (strcasecmp(arg, "account") == 0)
			(*pam)->pam_type = PAM_ACCOUNT_MODULE;
		else if (strcasecmp(arg, "session") == 0)
			(*pam)->pam_type = PAM_SESSION_MODULE;
		else if (strcasecmp(arg, "password") == 0)
			(*pam)->pam_type = PAM_PASSWORD_MODULE;
		else {
			/* error */
			syslog(LOG_CRIT, "invalid module type: %s", arg);
			free_pamconf (*pam);
			continue;
		}

		/* pam level flag */
		arg = read_next_token(&cp);
		if (cp == (char *) 0) {
			syslog(LOG_CRIT,
			"illegal entry in pam.conf: missing field: %s", cp);
			free_pamconf (*pam);
			continue;
		}
		if (strcasecmp(arg, "required") == 0)
			(*pam)->pam_flag = PAM_REQUIRED;
		else if (strcasecmp(arg, "optional") == 0)
			(*pam)->pam_flag = PAM_OPTIONAL;
		else if (strcasecmp(arg, "sufficient") == 0)
			(*pam)->pam_flag = PAM_SUFFICIENT;
		else {
			/* error */
			syslog(LOG_CRIT, "invalid flag: %s", arg);
			free_pamconf (*pam);
			continue;
		}

		arg = read_next_token(&cp);
		if (cp == (char *) 0) {
			syslog(LOG_CRIT,
			"illegal entry in pam.conf: missing field: %s", cp);
			free_pamconf (*pam);
			continue;
		}
		/*
		 * If module path does not start with "/", then
		 * prepend PAM_LIB_DIR (/usr/lib/security/).
		 */
		if (arg[0] != '/') {
			if (((*pam)->module_path = (char *)
				calloc(strlen(PAM_LIB_DIR) + strlen(arg) + 1,
					sizeof (char))) == NULL) {
				syslog(LOG_ERR, "strdup: out of memory");
				goto mal_formed;
			}
			sprintf((*pam)->module_path, "%s%s",
				PAM_LIB_DIR, arg);
		} else {
			(*pam)->module_path = strdup(arg);
			if ((*pam)->module_path == NULL) {
				syslog(LOG_ERR, "strdup: out of memory");
				goto mal_formed;
			}
		}

		/* count the number of options first */
		argc = 0;
		if ((tmp = strdup(cp)) == NULL) {
			syslog(LOG_ERR, "strdup: out of memory");
			goto mal_formed;
		}
		tmp_free = tmp;
		for (arg = read_next_token(&tmp); arg;
					arg = read_next_token(&tmp))
			argc++;
		free(tmp_free);

		/* allocate pointer array */
		if (argc > 0) {
			(*pam)->module_argv = (char **)
					calloc(argc+1, sizeof (char *));
			if ((*pam)->module_argv == NULL) {
				syslog(LOG_ERR, "calloc: out of memory");
				goto mal_formed;
			}
			i = 0;
			for (arg = read_next_token(&cp); arg;
				arg = read_next_token(&cp)) {
				(*pam)->module_argv[i] = strdup(arg);
				if ((*pam)->module_argv[i] == NULL) {
					syslog(LOG_ERR, "strdup failed");
					goto mal_formed;
				}
				i++;
			}
			(*pam)->module_argv[argc] = NULL;
		}
		(*pam)->module_argc = argc;
		break;
	} /* while */

	return (1);

mal_formed:

	free_pamconf(*pam);
	return (0);
}


/*
 * read_next_token - skip tab and space characters and return the next token
 */

static char *
read_next_token(char **cpp)
{
	register char *cp = *cpp;
	char *start;

	while (*cp == ' ' || *cp == '\t')
		cp++;
	if (*cp == '\0') {
		*cpp = (char *)0;
		return ((char *)0);
	}
	start = cp;
	while (*cp && *cp != ' ' && *cp != '\t')
		cp++;
	if (*cp != '\0')
		*cp++ = '\0';
	*cpp = cp;
	return (start);
}

/*
 * nextline - skip all blank lines and comments
 */

static char *
nextline(struct pam_fh *pam_fh)
{
	char	*cp;
	char	*ll;

	/*
	 * Skip the blank line, comment line
	 */
	while ((ll = fgets(pam_fh->line, sizeof (pam_fh->line),
		pam_fh->fconfig)) && (*(pam_fh->line) == '\n' ||
		*(pam_fh->line) == '#'));

	if (ll == NULL)
		return ((char *)0);
	cp = strchr(pam_fh->line, '\n');
	if (cp)
		*cp = '\0';
	return (pam_fh->line);
}

/*
 * clean_up()
 * free_pamconf()
 * free_pam_conf_info()
 *
 * Routines to free allocated storage
 */

/*
 * clean_up -  free allocated storage in the pam handle
 */

static void
clean_up(pam_handle_t *pamh)
{
	int i;

	if (pamh) {
		free_pam_conf_info(pamh);
		for (i = 0; i < PAM_MAX_ITEMS; i++) {
			if (pamh->ps_item[i].pi_addr != NULL)
				free(pamh->ps_item[i].pi_addr);
		}
		free(pamh);
	}
}

/*
 * free_pamconf - free memory used to store pam.conf entry
 */

static void
free_pamconf(pamtab *cp)
{
	int i;

	if (cp) {
		if (cp->pam_service)
			free(cp->pam_service);
		if (cp->module_path)
			free(cp->module_path);
		for (i = 0; i < cp->module_argc; i++) {
			if (cp->module_argv[i])
				free(cp->module_argv[i]);
		}
		if (cp->module_argc > 0)
			free(cp->module_argv);
		if (cp->function_ptr)
			free(cp->function_ptr);

		free(cp);
	}
}

/*
 * free_pam_conf_info - free memory used to store all pam.conf info
 *			under the pam handle
 */

static void
free_pam_conf_info(pam_handle_t *pamh)
{

	pamtab *pamentp;
	pamtab *pament_trail;
	int i;

	for (i = 0; i < PAM_NUM_MODULE_TYPES; i++) {
		pamentp = pamh->pam_conf_info[i];
		pament_trail = pamentp;
		while (pamentp) {
			pamentp = pamentp->next;
			free_pamconf(pament_trail);
			pament_trail = pamentp;
		}
	}

}
