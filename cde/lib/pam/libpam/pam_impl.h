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
/* $XConsortium: pam_impl.h /main/4 1996/05/09 04:25:20 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef	_PAM_IMPL_H
#define	_PAM_IMPL_H

#pragma ident	"@(#)pam_impl.h	1.42	96/02/02 SMI"	/* PAM 2.6 */

#ifdef __cplusplus
extern "C" {
#endif

#include <shadow.h>

#define	PAMTXD		"SUNW_OST_SYSOSPAM"

#define	AUTH_LIB	"/usr/lib/libpam.a"
#define	PAM_CONFIG	"/etc/pam.conf"
#define	PAM_LIB_DIR	"/usr/lib/security/"

#define	PAM_AUTH_MODULE		0
#define	PAM_ACCOUNT_MODULE	1
#define	PAM_PASSWORD_MODULE	2
#define	PAM_SESSION_MODULE	3
#define	PAM_NUM_MODULE_TYPES	4

#define	PAM_REQUIRED	1	/* required flag in config file */
#define	PAM_OPTIONAL	2	/* optional flag in config file */
#define	PAM_SUFFICIENT	4	/* sufficient flag in config file */

/* XXX: Make sure this is correct in pam_appl.h */
#define	PAM_TOTAL_ERRNUM	28	/* total # PAM error numbers */

/* authentication module functions */
#define	PAM_SM_AUTHENTICATE	"pam_sm_authenticate"
#define	PAM_SM_SETCRED		"pam_sm_setcred"

/* session module functions */
#define	PAM_SM_OPEN_SESSION	"pam_sm_open_session"
#define	PAM_SM_CLOSE_SESSION	"pam_sm_close_session"

/* password module functions */
#define	PAM_SM_CHAUTHTOK		"pam_sm_chauthtok"

/* account module functions */
#define	PAM_SM_ACCT_MGMT		"pam_sm_acct_mgmt"

#define	PAM_MAX_ITEMS		64	/* Max number of items */

/* for modules when calling __pam_get_authtok() */
#define	PAM_PROMPT	1	/* prompt user for new password */
#define	PAM_HANDLE	2	/* get password from pam handle (item) */

/* utility function prototypes */
extern void
__pam_free_resp(
	int num_msg,
	struct pam_response *resp
);

extern int
__pam_display_msg(
	pam_handle_t *pamh,
	int msg_style,
	int num_msg,
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE],
	void *conv_apdp
);

extern int
__pam_get_input(
	pam_handle_t *pamh,
	int msg_style,
	int num_msg,
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE],
	void *conv_apdp,
	struct pam_response **ret_respp
);

extern int
__pam_get_authtok(
	pam_handle_t *pamh,
	int source,
	int type,
	int len,
	char *prompt,
	char **authtok
);

extern char *
__pam_get_i18n_msg(
	pam_handle_t *pamh,
	char *filename,
	int set,
	int n,
	char *string
);

/* file handle for pam.conf */
struct pam_fh {
	FILE    *fconfig;
	char    line[256];
};

/* items that can be set/retrieved thru pam_[sg]et_item() */
struct	pam_item {
	void	*pi_addr;	/* pointer to item */
	int	pi_size;	/* size of item */
};

/* module specific data stored in the pam handle */
struct pam_module_data {
	char *module_data_name;		/* unique module data name */
	void *data;			/* the module specific data */
	void (*cleanup)(pam_handle_t *pamh, void *data, int pam_status);
	struct pam_module_data *next;	/* pointer to next module data */
};

/* each entry from pam.conf is stored here (in the pam handle) */
typedef struct pamtab {
	char	*pam_service;	/* PAM service, e.g. login, rlogin */
	int	pam_type;	/* AUTH, ACCOUNT, PASSWORD, SESSION */
	int	pam_flag;	/* required, optional, sufficient */
	char	*module_path;	/* module library */
	int	module_argc;	/* module specific options */
	char	**module_argv;
	void	*function_ptr;	/* pointer to struct holding function ptrs */
	struct pamtab *next;
} pamtab;

/* the pam handle */
struct pam_handle {
	struct  pam_item ps_item[PAM_MAX_ITEMS];	/* array of PAM items */
	pamtab	*pam_conf_info[PAM_NUM_MODULE_TYPES];	/* pam.conf info */
	struct	pam_module_data *ssd;		/* module specific data */
};

/*
 * the function_ptr field in struct pamtab
 * will point to one of these modules
 */
struct auth_module {
	int			(*pam_sm_authenticate)(
					pam_handle_t *pamh,
					int flags,
					int argc,
					const char **argv);
	int			(*pam_sm_setcred)(
					pam_handle_t *pamh,
					int flags,
					int argc,
					const char **argv);
};

struct password_module {
	int			(*pam_sm_chauthtok)(
					pam_handle_t *pamh,
					int flags,
					int argc,
					const char **argv);
};

struct session_module {
	int			(*pam_sm_open_session)(
					pam_handle_t *pamh,
					int flags,
					int argc,
					const char **argv);
	int			(*pam_sm_close_session)(
					pam_handle_t *pamh,
					int flags,
					int argc,
					const char **argv);
};

struct account_module {
	int			(*pam_sm_acct_mgmt)(
					pam_handle_t *pamh,
					int flags,
					int argc,
					const char **argv);
};

#ifdef __cplusplus
}
#endif

#endif	/* _PAM_IMPL_H */
