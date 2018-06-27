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
/* $XConsortium: unix_get_authtokattr.c /main/8 1996/11/21 20:00:50 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)unix_get_authtokattr.c 1.59     96/01/07 SMI"

#include	"unix_headers.h"

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#include <X11/Xos_r.h>

#ifdef PAM_NISPLUS
static void _np_nss_initf_shadow(nss_db_params_t *);
static void _np_setspent();
static void _np_endspent();
static struct spwd * _np_getspent_r(struct spwd *, char *, int);
static struct spwd * _np_getspent();
#endif

/*
 * __get_authtoken_attr():
 *	To get authentication token attribute values.
 *
 * 	This function calls ck_perm() first to check the caller's
 * 	permission.  If the check succeeds, it will read the
 * 	attribute/value pairs from the shadow password entry of
 *	the user specified by the authentication handle "pamh"
 * 	and store them into a character array and return.
 */

/*
 * XXX: We use our own version of the shadow passwd getent routine.
 * See below for details.  Compatible with version 2 of the name service
 * switch.  In the future, the name service switch implementation may
 * change and these functions and the Makefile may have to
 * be modified.
 */

int
__get_authtoken_attr(
	pam_handle_t		*pamh,
	char			***ga_getattr,
	int			repository,
	const char		*domain,
	int			argc,
	const char		**argv)
{
	int		k;
	char 			value[PAM_MAX_ATTR_SIZE];
	int 			retcode;
	long 			lstchg;
	char 			*usrname;
	char 			*prognamep;
	char			**get_attributes;
	struct passwd		*pwd;
	struct spwd		*shpwd;
	int			found = 0;
	struct spwd 		*psp;
	struct tm		*tmp;
	int			privileged = 0;
	char 			messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	int			debug = 0;
	int			nowarn = 0;
	int			i;
	void			*passwd_res;

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcmp(argv[i], "nowarn") == 0)
			nowarn = 1;
		else
			syslog(LOG_ERR, "illegal UNIX module option %s",
				argv[i]);
	}

	if (debug)
		syslog(LOG_DEBUG,
		    "__get_authtoken_attr(): repository=%s",
			repository_to_string(repository));

	if ((retcode = pam_get_item(pamh, PAM_SERVICE, (void **)&prognamep))
						!= PAM_SUCCESS ||
	    (retcode = pam_get_item(pamh, PAM_USER, (void **)&usrname))
						!= PAM_SUCCESS)
		return (retcode);

	if ((get_attributes = (char **)
		calloc(PAM_MAX_NUM_ATTR, sizeof (char *))) == NULL)
		return (PAM_BUF_ERR);

	/* repository must be specified in the command line. */
	if (repository == PAM_REP_DEFAULT) {
		sprintf(messages[0], PAM_MSG(pamh, 40,
	"You must specify repository when displaying passwd attributes"));
		(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
		return (PAM_AUTHTOK_ERR);
	}

	if (!IS_FILES(repository) && !IS_NIS(repository) &&
	    !IS_NISPLUS(repository)) {
		sprintf(messages[0], PAM_MSG(pamh, 41,
		    "%s: System error: repository out of range"), prognamep);
		(void) __pam_display_msg(pamh, PAM_ERROR_MSG, 1,
			messages, NULL);
		return (PAM_SYSTEM_ERR);
	}

#ifdef PAM_NISPLUS
	if (usrname == NULL || *usrname == NULL) {
		/* print nis+ table */
		/*
		 * Cat the table using our private _np_getspent()
		 */
		if (!IS_NISPLUS(repository)) {
			sprintf(messages[0], PAM_MSG(pamh, 42,
				"Unable to retrieve username."));
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
			return (PAM_AUTHTOK_RECOVERY_ERR);
		}

		(void) _np_setspent();
		while ((psp = _np_getspent()) != NULL) {
			found++;
			sprintf(value, "%s  ", psp->sp_namp);

			if (psp->sp_pwdp == NULL) {
				sprintf(messages[0],
					"%s NP  ", value);
			} else if ((int)strlen(psp->sp_pwdp) < NUMCP) {
				sprintf(messages[0],
					"%s LK  ", value);
			} else {
				sprintf(messages[0],
					"%s PS  ", value);
			}

			if (psp->sp_max != -1) {
			  	_Xgtimeparams gmtime_buf;
				strcpy(value, messages[0]);
				if (psp->sp_lstchg) {
					lstchg = psp->sp_lstchg * DAY;
					tmp = _XGmtime(&lstchg, gmtime_buf);
					sprintf(messages[0],
						"%s %.2d/%.2d/%.2d  ",
						value,
						(tmp->tm_mon + 1),
						tmp->tm_mday, tmp->tm_year);
				} else
					sprintf(messages[0],
					"%s 00/00/00  ",
					value);

				strcpy(value, messages[0]);
				if ((psp->sp_min >= 0) && (psp->sp_warn > 0)) {
					sprintf(messages[0],
						"%s %d  %d  %d ",
						value,
						psp->sp_min, psp->sp_max,
						psp->sp_warn);
				} else if (psp->sp_min >= 0) {
					sprintf(messages[0],
						"%s %d  %d  ", value,
						psp->sp_min, psp->sp_max);
				} else if (psp->sp_warn > 0) {
					sprintf(messages[0],
						"%s    %d  %d ", value,
						psp->sp_max, psp->sp_warn);
				} else {
					sprintf(messages[0],
						"%s    %d  ",
						value, psp->sp_max);
				}
			}
			(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
				1, messages, NULL);
		}
		(void) _np_endspent();

		/*
		 * If password table does not have any entries or is missing,
		 * return fatal error.
		 */
		if (found == 0) {
			sprintf(messages[0],
				PAM_MSG(pamh, 43,
	"%s: Unexpected failure. Password database unchanged."),
				prognamep);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
			return (PAM_SYSTEM_ERR);
		}
		return (PAM_SUCCESS);
	}
#endif /* PAM_NISPLUS */

	retcode = ck_perm(pamh, repository,
			(char *)domain, &pwd, &shpwd, &privileged,
			(void **)&passwd_res, getuid(), debug, nowarn);
	if (retcode != 0) {
		return (retcode);
	}

	k = 0;

	/* get attribute "AUTHTOK_STATUS" */
	if (shpwd->sp_pwdp == NULL)
		(void) strcpy(value, "NP  ");
	else if ((int)strlen(shpwd->sp_pwdp) < NUMCP)
		(void) strcpy(value, "LK  ");
	else
		(void) strcpy(value, "PS  ");
	setup_attr(get_attributes, k++, "AUTHTOK_STATUS=", value);


	if (shpwd->sp_max != -1) {
		/* get attribute "AUTHTOK_LASTCHANGE" */
		if (shpwd->sp_lstchg) {
			lstchg = shpwd->sp_lstchg * DAY;
			sprintf(value, "%d", lstchg);
		} else {
			sprintf(value, "%d", shpwd->sp_lstchg);
		}
		setup_attr(get_attributes, k++,
			"AUTHTOK_LASTCHANGE=", value);

		/* get attribute "AUTHTOK_MINAGE"		*/
		/* "AUTHTOK_MAXAGE", and "AUTHTOK_WARNDATE"	*/
		if ((shpwd->sp_min >= 0) && (shpwd->sp_warn > 0)) {
			sprintf(value, "%d", shpwd->sp_min);
			setup_attr(get_attributes, k++,
				"AUTHTOK_MINAGE=", value);
			sprintf(value, "%d", shpwd->sp_max);
			setup_attr(get_attributes, k++,
				"AUTHTOK_MAXAGE=", value);
			sprintf(value, "%d", shpwd->sp_warn);
			setup_attr(get_attributes, k++,
				"AUTHTOK_WARNDATE=", value);
		} else {
			if (shpwd->sp_min >= 0) {
				sprintf(value, "%d", shpwd->sp_min);
				setup_attr(get_attributes, k++,
					"AUTHTOK_MINAGE=", value);
				sprintf(value, "%d", shpwd->sp_max);
				setup_attr(get_attributes, k++,
					"AUTHTOK_MAXAGE=", value);
			} else {
				if (shpwd->sp_warn > 0) {
					sprintf(value, "%d", shpwd->sp_max);
					setup_attr(get_attributes, k++,
						"AUTHTOK_MAXAGE=", value);
					sprintf(value, "%d", shpwd->sp_warn);
					setup_attr(get_attributes, k++,
						"AUTHTOK_WARNDATE=", value);
				} else {
					sprintf(value, "%d", shpwd->sp_max);
					setup_attr(get_attributes, k++,
						"AUTHTOK_MAXAGE=", value);
				}
			}
		}
	}
	/* terminate with NULL */
	setup_attr(get_attributes, k, NULL, NULL);

	*ga_getattr = &get_attributes[0];

	free_passwd_structs(pwd, shpwd);
	return (PAM_SUCCESS);

}


#ifdef PAM_NISPLUS

/*
 * XXX Our private version of the switch frontend for getspent.  We want to
 * search just the nisplus sp file, so we want to bypass normal nsswitch.conf
 * based processing.  This implementation compatible with version 2 of the
 * name service switch.
 */

#define	NSS_NISPLUS_ONLY	"nisplus"

int str2spwd(const char *, int, void *, char *, int);

static DEFINE_NSS_DB_ROOT(db_root);
static DEFINE_NSS_GETENT(context);

static void
_np_nss_initf_shadow(p)
	nss_db_params_t	*p;
{
	p->name	= NSS_DBNAM_SHADOW;
	p->config_name    = NSS_DBNAM_PASSWD;	/* Use config for "passwd" */
	p->default_config = NSS_NISPLUS_ONLY;   /* Use nisplus only */
	p->flags = NSS_USE_DEFAULT_CONFIG;
}

static void
_np_setspent()
{
	nss_setent(&db_root, _np_nss_initf_shadow, &context);
}

static void
_np_endspent()
{
	nss_endent(&db_root, _np_nss_initf_shadow, &context);
	nss_delete(&db_root);
}

static struct spwd *
_np_getspent_r(result, buffer, buflen)
	struct spwd	*result;
	char		*buffer;
	int		buflen;
{
	nss_XbyY_args_t arg;
	char		*nam;

	/* In getXXent_r(), protect the unsuspecting caller from +/- entries */

	do {
		NSS_XbyY_INIT(&arg, result, buffer, buflen, str2spwd);
			/* No key to fill in */
		nss_getent(&db_root, _np_nss_initf_shadow, &context, &arg);
	} while (arg.returnval != 0 &&
			(nam = ((struct spwd *)arg.returnval)->sp_namp) != 0 &&
			(*nam == '+' || *nam == '-'));

	return (struct spwd *) NSS_XbyY_FINI(&arg);
}

static nss_XbyY_buf_t *buffer;

static struct spwd *
_np_getspent()
{
	nss_XbyY_buf_t	*b;

	b = NSS_XbyY_ALLOC(&buffer, sizeof (struct spwd), NSS_BUFLEN_SHADOW);

	return (b == 0 ? 0 : _np_getspent_r(b->result, b->buffer, b->buflen));
}

#endif /* PAM_NISPLUS */
