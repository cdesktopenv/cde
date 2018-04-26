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
/* $XConsortium: unix_update_authtok_nisplus.c /main/5 1996/05/09 04:36:35 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident	"@(#)unix_update_authtok_nisplus.c 1.55	96/02/02 SMI"

#include "unix_headers.h"

#ifdef PAM_NISPLUS
static int		update_attr(pam_handle_t *, char *, char **, char *,
				int, char **, char **,
				struct passwd *, int, int,
				nis_result *, int);
static int		talk_to_npd(pam_handle_t *, char *, char **, char *,
				char *, char *, char *,
				struct passwd *, int, int,
				char **, char **,
				nis_result *, int, int);
static char		*reencrypt_secret(char *, char *, char *);
static nis_error	revert2oldpasswd(char *, nis_result *);

int
update_authtok_nisplus(
	pam_handle_t *pamh,
	char *domain,
	char *field,
	char *data[],			/* Depending on field: it can store */
					/* encrypted new passwd or new */
					/* attributes */
	char *old,			/* old passwd: clear version */
	char *oldrpc,			/* old rpc passwd: clear version */
	char *new,			/* new passwd: clear version */
	int  opwcmd,			/* old passwd cmd: nispasswd */
	struct passwd *nisplus_pwd,
	char *curcryptsecret,
	int privileged,
	nis_result *passwd_res,
	nis_result *cred_res,
	int debug,
	int nowarn)
{
	char tmpcryptsecret[HEXKEYBYTES+KEYCHECKSUMSIZE+1];
	char *newcryptsecret = NULL;
	entry_col	ecol[8];
	nis_object	*eobj;
	nis_result	*mres;
	char		mname[NIS_MAXNAMELEN];
	nis_name	pwd_domain;
	nis_error 	niserr = 0;
	struct spwd	sp;
	char		shadow[80];
	int		rc = 0;
	char 		messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	int		failover = FALSE;
	char		*gecos = NULL, *shell = NULL;
	char		*prognamep;
	char		*usrname;

	if ((rc = pam_get_item(pamh, PAM_SERVICE, (void **)&prognamep))
							!= PAM_SUCCESS ||
	    (rc = pam_get_item(pamh, PAM_USER, (void **)&usrname))
							!= PAM_SUCCESS)
		return (rc);

	/*
	 * Passwd is setuid program. We want the real user to send out
	 * any nis+ requests. The correct identity should have been set
	 * in ck_perm() when checking privilege.
	 */
	if (debug) {
		syslog(LOG_DEBUG,
			"the effective uid while updating NIS+ passwd is %d",
			geteuid());
	}

	if (opwcmd == FALSE) {
		/*
		 * Attempt to let NIS+ NPD do the password update.
		 * If the passwd entry is not present (in passwd_res)
		 *	try NPD for the local domain.
		 * If the passwd entry is present call NPD in the
		 *	domain the passwd entry resides in.
		 *	NPD wants only the domainname so strip off
		 *	the org_dir portion of the passwd directory.
		 */
		if (passwd_res == NULL || passwd_res->status != NIS_SUCCESS) {
			/*
			 * CAVEAT:
			 * Should never get here; ck_perm() should fail.
			 *
			 * It is a waste of time to try NPD for some values
			 * of passwd_res->status; additional checks advised
			 * if ever it is possible to get here.
			 */
			rc = talk_to_npd(pamh, field, data, domain, usrname,
				old, new, nisplus_pwd, failover, privileged,
				&shell, &gecos, passwd_res, debug, nowarn);
		} else {
			pwd_domain = NIS_RES_OBJECT(passwd_res)->zo_domain;
			if (strcmp(nis_leaf_of(pwd_domain), "org_dir") == 0) {
				pwd_domain = nis_domain_of(
					NIS_RES_OBJECT(passwd_res)->zo_domain);
			}

			rc = talk_to_npd(pamh, field, data, pwd_domain, usrname,
				old, new, nisplus_pwd, failover, privileged,
				&shell, &gecos, passwd_res, debug, nowarn);
		}

		if (rc == PAM_SUCCESS || rc == PAM_NISPLUS_PARTIAL_SUCCESS) {
			sprintf(messages[0], PAM_MSG(pamh, 108,
				"NIS+ password information changed for %s"),
				usrname);
			(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
				1, messages, NULL);

			if (rc == PAM_SUCCESS) {
				sprintf(messages[0], PAM_MSG(pamh, 109,
				"NIS+ credential information changed for %s"),
					usrname);
				(void) __pam_display_msg(
					pamh, PAM_TEXT_INFO, 1,
					messages, NULL);
			}
			return (PAM_SUCCESS);
		}
		/* failover to use old protocol */
		if (debug)
			syslog(LOG_DEBUG,
			    "Failed to use new passwd update protocol");

		/*
		 * There are two reasons we will get here:
		 * 1. passwd, shell, gecos update failed (true failover)
		 * 2. we are updating passwd attrs other than the above
		 *    three attrs. In this case, rc is equal to PAM_PERM_DENIED
		 *    (i.e. attrs not supported by new protocol)
		 */
		failover = TRUE;
	}

	if (strcmp(field, "passwd") == 0) {
		/*
		 * Old style nisplus update
		 *
		 * Obtain the old aging information. And modify, if need be,
		 * on top. At least the lstchg field needs to be changed.
		 */
		/* old protocol requires user credential info */
		if (cred_res == NULL || cred_res->status != NIS_SUCCESS) {
			syslog(LOG_ERR, "%s%s: %s", prognamep, NISPLUS_MSG,
				"Failover: user credential is required.");
			return (PAM_AUTHTOK_RECOVERY_ERR);
		}

		nisplus_populate_age(NIS_RES_OBJECT(passwd_res), &sp);

		(void) memcpy(tmpcryptsecret, curcryptsecret,
		    HEXKEYBYTES + KEYCHECKSUMSIZE + 1);

		/* same user check? */
		if ((!privileged) && (newcryptsecret = reencrypt_secret
		    (tmpcryptsecret, oldrpc, new)) == NULL) {
			sprintf(messages[0], " ");
			sprintf(messages[1], PAM_MSG(pamh, 110,
				"Unable to reencrypt NIS+ credentials for %s;"),
				usrname);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				2, messages, NULL);
			return (PAM_AUTHTOK_RECOVERY_ERR);
		}

		/* update passwd at server */
		(void) memset((char *)ecol, 0, sizeof (ecol));
		ecol[1].ec_value.ec_value_val = *data;
		ecol[1].ec_value.ec_value_len = strlen(*data) + 1;
		ecol[1].ec_flags = EN_CRYPT|EN_MODIFIED;

		/* update last change field */
		sp.sp_lstchg = DAY_NOW;
		if (sp.sp_max == 0) {
			/* passwd was forced to changed: turn off aging */
			sp.sp_max = -1;
			sp.sp_min = -1;
		}

		/* prepare shadow column */
		if (sp.sp_expire == -1) {
			sprintf(shadow, "%ld:%ld:%ld:%ld:%ld::%lu",
				sp.sp_lstchg,
				sp.sp_min,
				sp.sp_max,
				sp.sp_warn,
				sp.sp_inact,
				sp.sp_flag);
		} else {
			sprintf(shadow, "%ld:%ld:%ld:%ld:%ld:%ld:%lu",
				sp.sp_lstchg,
				sp.sp_min,
				sp.sp_max,
				sp.sp_warn,
				sp.sp_inact,
				sp.sp_expire,
				sp.sp_flag);
		}
		ecol[7].ec_value.ec_value_val = shadow;
		ecol[7].ec_value.ec_value_len = strlen(shadow) + 1;
		ecol[7].ec_flags = EN_CRYPT|EN_MODIFIED;

		/*
		 * build entry based on the one we got back from the server
		 */
		eobj = nis_clone_object(NIS_RES_OBJECT(passwd_res), NULL);
		if (eobj == NULL) {
			syslog(LOG_ERR, "%s%s: %s", prognamep, NISPLUS_MSG,
				"clone object failed");
			return (PAM_AUTHTOK_RECOVERY_ERR);
		}
		eobj->EN_data.en_cols.en_cols_val = ecol;
		eobj->EN_data.en_cols.en_cols_len = 8;

		/* strlen("[name=],.") + null + "." = 11 */
		if ((strlen(usrname) +
		    strlen(NIS_RES_OBJECT(passwd_res)->zo_name) +
		    strlen(NIS_RES_OBJECT(passwd_res)->zo_domain) + 11) >
			(size_t) NIS_MAXNAMELEN) {
			syslog(LOG_ERR, "%s%s: %s", prognamep, NISPLUS_MSG,
				"NIS+ name too long");
			return (PAM_BUF_ERR);
		}
		sprintf(mname, "[name=%s],%s.%s", usrname,
		    NIS_RES_OBJECT(passwd_res)->zo_name,
		    NIS_RES_OBJECT(passwd_res)->zo_domain);
		if (mname[strlen(mname) - 1] != '.')
			(void) strcat(mname, ".");
		mres = nis_modify_entry(mname, eobj, 0);

		/*
		 * It is possible that we have permission to modify the
		 * encrypted password but not the shadow column in the
		 * NIS+ table. In this case, we should try updating only
		 * the password field and not the aging stuff (lstchg).
		 * With the current NIS+ passwd table format, this would
		 * be the case most of the times.
		 */
		if (mres->status == NIS_PERMISSION) {
			ecol[7].ec_flags = 0;
			mres = nis_modify_entry(mname, eobj, 0);
			if (mres->status != NIS_SUCCESS) {
				sprintf(messages[0], PAM_MSG(pamh, 111,
				"%s%s: Password information update failed"),
					prognamep, NISPLUS_MSG);
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);

				(void) nis_freeresult(mres);
				return (PAM_AUTHTOK_RECOVERY_ERR);
			}
		}
		/* set column stuff to NULL so that we can free eobj */
		eobj->EN_data.en_cols.en_cols_val = NULL;
		eobj->EN_data.en_cols.en_cols_len = 0;
		(void) nis_destroy_object(eobj);
		(void) nis_freeresult(mres);

		sprintf(messages[0], PAM_MSG(pamh, 112,
			"NIS+ password information changed for %s"),
			usrname);
		(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
			1, messages, NULL);

		if (privileged) {
			sprintf(messages[0], " ");
			sprintf(messages[1], PAM_MSG(pamh, 113,
	"The NIS+ credential information for %s will not be changed."),
			usrname);
			sprintf(messages[2], PAM_MSG(pamh, 114,
	"User %s must do the following to update his/her"), usrname);
			sprintf(messages[3], PAM_MSG(pamh, 115,
	"credential information:"));
			sprintf(messages[4], PAM_MSG(pamh, 116,
	"Use NEW passwd for login and OLD passwd for keylogin."));
			sprintf(messages[5], PAM_MSG(pamh, 117,
	"Use \"chkey -p\" to reencrypt the credentials with the"));
			sprintf(messages[6], PAM_MSG(pamh, 118,
	"new login passwd."));
			sprintf(messages[7], PAM_MSG(pamh, 119,
	"The user must keylogin explicitly after their next login."),
				usrname);
			sprintf(messages[8], " ");
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				9, messages, NULL);

			return (PAM_SUCCESS);
		}

		/* update cred at server */
		(void) memset((char *)ecol, 0, sizeof (ecol));
		ecol[4].ec_value.ec_value_val = newcryptsecret;
		ecol[4].ec_value.ec_value_len = strlen(newcryptsecret) + 1;
		ecol[4].ec_flags = EN_CRYPT|EN_MODIFIED;
		eobj = nis_clone_object(NIS_RES_OBJECT(cred_res), NULL);
		if (eobj == NULL) {
			syslog(LOG_ERR, "%s%s: %s", prognamep, NISPLUS_MSG,
				"clone object failed");
			return (PAM_AUTHTOK_RECOVERY_ERR);
		}
		eobj->EN_data.en_cols.en_cols_val = ecol;
		eobj->EN_data.en_cols.en_cols_len = 5;

		/*
		 * Now, if one were stupid enough to run nispasswd as/for root
		 * on some machine, it would have looked up and modified
		 * the password entry for "root" in passwd.org_dir. Now,
		 * should we really apply this new password to the cred
		 * entry for "<machinename>.<domainname>" ?
		 *
		 * POLICY: NO. We have no way of identifying a root user in
		 * NIS+ passwd table for each root@machinename. We do not
		 * allow the one password for [name=root], passwd.org_dir
		 * to apply to all "<machinename>.<domainname>" principals.
		 * If somebody let a root entry in passwd table, it probably
		 * has modify permissions for a distinguished NIS+ principal
		 * which we let be associated only with NIS+ principal
		 * root.<domainname>. Does this make any sense ?
		 */

		/* strlen("[cname=.,auth_type=DES],.") + null + "." = 26 */
		if ((strlen(ENTRY_VAL(NIS_RES_OBJECT(cred_res), 0)) +
		    strlen(NIS_RES_OBJECT(cred_res)->zo_name) +
		    strlen(NIS_RES_OBJECT(cred_res)->zo_domain) + 26) >
			(size_t) NIS_MAXNAMELEN) {
			syslog(LOG_ERR, "%s%s: %s", prognamep, NISPLUS_MSG,
				"NIS+ name too long");
			return (PAM_BUF_ERR);
		}
		sprintf(mname, "[cname=%s,auth_type=DES],%s.%s",
		    ENTRY_VAL(NIS_RES_OBJECT(cred_res), 0),
		    NIS_RES_OBJECT(cred_res)->zo_name,
		    NIS_RES_OBJECT(cred_res)->zo_domain);
		if (mname[strlen(mname) - 1] != '.')
			(void) strcat(mname, ".");
		mres = nis_modify_entry(mname, eobj, 0);
		if (mres->status != NIS_SUCCESS) {

			/* attempt to revert back to the old passwd */
			niserr = revert2oldpasswd(usrname, passwd_res);

			if (niserr != NIS_SUCCESS) {
				sprintf(messages[0], "");
				sprintf(messages[1], PAM_MSG(pamh, 120,
		"WARNING: Could not reencrypt NIS+ credentials for %s;"),
				    usrname);
				sprintf(messages[2], PAM_MSG(pamh, 121,
		"login and keylogin passwords differ."));
				sprintf(messages[3], PAM_MSG(pamh, 122,
		"Use NEW passwd for login and OLD passwd for keylogin."));
				sprintf(messages[4], PAM_MSG(pamh, 117,
		"Use \"chkey -p\" to reencrypt the credentials with the"));
				sprintf(messages[5], PAM_MSG(pamh, 118,
		"new login passwd."));
				sprintf(messages[6], PAM_MSG(pamh, 123,
		"You must keylogin explicitly after your next login."));
				sprintf(messages[7], "");
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				    8, messages, NULL);

				return (PAM_AUTHTOK_RECOVERY_ERR);
			}

			sprintf(messages[0], PAM_MSG(pamh, 124,
"%s%s: couldn't change password for %s."),
				prognamep, NISPLUS_MSG, usrname);
			sprintf(messages[1], PAM_MSG(pamh, 125,
"Reason: failed to update the cred table with reencrypted credentials."));
			sprintf(messages[2], PAM_MSG(pamh, 126,
"Please notify your System Administrator."));
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				3, messages, NULL);

			(void) nis_freeresult(mres);
			return (PAM_AUTHTOK_RECOVERY_ERR);
		}
		/* set column stuff to NULL so that we can free eobj */
		eobj->EN_data.en_cols.en_cols_val = NULL;
		eobj->EN_data.en_cols.en_cols_len = 0;
		(void) nis_destroy_object(eobj);
		(void) nis_freeresult(mres);

		sprintf(messages[0], PAM_MSG(pamh, 109,
			"NIS+ credential information changed for %s"),
			usrname);
		(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
			1, messages, NULL);
	} else
		return (update_attr(pamh, field, data, usrname, 1, &shell,
			&gecos, nisplus_pwd, failover, privileged,
			passwd_res, nowarn));

	return (PAM_SUCCESS);
}


/*
 * The function uses the new protocol to update passwd attributes via
 * passwd daemon.
 */
static int
talk_to_npd(pam_handle_t *pamh, char *field, char **data, char *domain,
	char *user, char *oldpass, char *newpass,
	struct passwd *nisplus_pwd, int failover, int privileged,
	char **shell, char **gecos,
	nis_result *passwd_res, int debug, int nowarn)
{
	CLIENT		*clnt = NULL;
	char		*old_passwd = NULL;
	char		srv_pubkey[HEXKEYBYTES + 1];
	char		u_pubkey[HEXKEYBYTES + 1];
	char		u_seckey[HEXKEYBYTES + 1];
	des_block	deskey;
	unsigned long 	ident = 0, randval = 0;
	int 		error = 0, status, srv_keysize = HEXKEYBYTES + 1;
	int		retcode = PAM_SYSTEM_ERR;
	char		messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	nispasswd_error		*errlist = NULL;
	nispasswd_error		*p = NULL;

	if (user == NULL || domain == NULL || *domain == '\0') {
		retcode = PAM_AUTHTOK_RECOVERY_ERR;
		goto out;
	}

	if (debug)
		syslog(LOG_DEBUG, "domain=%s, user=%s", domain, user);

	/*
	 * Let's do a quick check whether the attrs are really of interest.
	 * We don't want to prompt for user passwd which is sure not to be
	 * used.
	 */
	retcode = update_attr(pamh, field, data, NULL, 0, NULL, NULL,
			nisplus_pwd, failover, privileged,
			passwd_res, nowarn);
	if (retcode != PAM_SUCCESS)
		goto out;

	if (oldpass == NULL) {
		/*
		 * This is possible from unix_set_authtokattr().
		 * Old passwd is required to change any attributes.
		 * This is imposed by new protocol to support users
		 * without credentials.
		 */

		retcode = __pam_get_authtok(pamh, PAM_PROMPT, 0, PASSWORD_LEN,
			PAM_MSG(pamh, 63, "Enter login(NIS+) password: "),
			&old_passwd);
		if (retcode != PAM_SUCCESS)
			goto out;
	} else {
		old_passwd = strdup(oldpass);
		if (old_passwd == NULL) {
			retcode = PAM_BUF_ERR;
			goto out;
		}
	}

	/* get gecos, shell and other */
	retcode = update_attr(pamh, field, data, user, 0, shell, gecos,
		nisplus_pwd, failover, privileged,
		passwd_res, nowarn);
	if (retcode != PAM_SUCCESS)
		goto out;

	if (npd_makeclnthandle(domain, &clnt, srv_pubkey, srv_keysize) ==
								FALSE) {
		syslog(LOG_ALERT,
		"Couldn't make a client handle to NIS+ password daemon");
		retcode = PAM_AUTHTOK_RECOVERY_ERR;
		goto out;
	}

/* again: doesn't need to generate a new pair of keys */
	/* generate a key-pair for this user */
	(void) __gen_dhkeys(u_pubkey, u_seckey, old_passwd);

	/*
	 * get the common des key from the servers' pubkey and
	 * the users secret key
	 */
	if (__get_cmnkey(srv_pubkey, u_seckey, &deskey) == FALSE) {
		syslog(LOG_ALERT, "Couldn't get a common DES key");
		retcode = PAM_AUTHTOK_RECOVERY_ERR;
		goto out;
	}
again:
	status = nispasswd_auth(user, domain, old_passwd, u_pubkey, &deskey,
		clnt, &ident, &randval, &error);
	if (status == NPD_FAILED) {
		switch (error) {
		case NPD_NOTMASTER:
			syslog(LOG_ALERT,
	"Password update daemon is not running with NIS+ master server");
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_SYSTEMERR:
			syslog(LOG_ALERT, "NIS+ system error");
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_IDENTINVALID:
			syslog(LOG_ALERT, "NIS+ identifier invalid");
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_PASSINVALID:
			syslog(LOG_ALERT, "NIS+ password invalid");
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_NOSUCHENTRY:
			syslog(LOG_ALERT, "No NIS+ password entry for %s",
				user);
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_NISERROR:
			syslog(LOG_ALERT, "NIS+ error");
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_CKGENFAILED:
			syslog(LOG_ALERT,
			    "Couldn't generate a common DES key");
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_NOPASSWD:
			syslog(LOG_ALERT, "No NIS+ password for %s", user);
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_NOTAGED:
			syslog(LOG_ALERT, "NIS+ passwd has not aged enough");
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		case NPD_NOSHDWINFO:
			syslog(LOG_ALERT, "No shadow password information");
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		default:
			syslog(LOG_ALERT, "NIS+ fatal error: %d", error);
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		}
	}
	if (status == NPD_TRYAGAIN) {
		/*
		 * call nispasswd_auth() again after getting another
		 * passwd. Note that ident is now non-zero.
		 */
		if (debug)
			syslog(LOG_DEBUG,
			    "status=tryagain; ident=%ld, randval=%ld",
			    ident, randval);

		if (old_passwd) {
			memset(old_passwd, 0, strlen(old_passwd));
			free(old_passwd);
		}

		/* wrong passwd: get auth token again */
		sprintf(messages[0], PAM_MSG(pamh, 127,
			"NIS+ Password incorrect: try again"));
		(void) __pam_display_msg(pamh, PAM_ERROR_MSG, 1,
		    messages, NULL);

		retcode = __pam_get_authtok(pamh, PAM_PROMPT, 0, PASSWORD_LEN,
			PAM_MSG(pamh, 63, "Enter login(NIS+) password: "),
			&old_passwd);
		if (retcode != PAM_SUCCESS)
			goto out;

		goto again;
	}
	if (status == NPD_SUCCESS) {
		/* send the new passwd & other changes */
		if (debug)
			syslog(LOG_DEBUG,
				"status=success; ident=%ld, randval=%ld",
				ident, randval);
		if (newpass == NULL) {
			/*
			 * This is possible from unix_set_authtokattr().
			 * Just use the same passwd so that we have a
			 * meaningful passwd field.
			 */
			newpass = old_passwd;
		}

		/* gecos and shell could be NULL if we just change passwd */
		status = nispasswd_pass(clnt, ident, randval, &deskey,
				newpass, *gecos, *shell, &error, &errlist);

		if (status == NPD_FAILED) {
			sprintf(messages[0], PAM_MSG(pamh, 128,
				"NIS+ password information update failed \
while talking to NIS+ passwd daemon"));
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
			if (debug)
				syslog(LOG_DEBUG, "error=%d", error);
			retcode = PAM_AUTHTOK_RECOVERY_ERR;
			goto out;
		}
		/*
		 * WHAT SHOULD BE DONE FOR THE PARTIAL SUCCESS CASE ??
		 * I'll just print out some messages
		 */
		if (status == NPD_PARTIALSUCCESS) {
			syslog(LOG_ALERT,
			"Password information is partially updated.");
			for (p = errlist; p != NULL; p = p->next) {
				if (p->npd_field == NPD_GECOS) {
					sprintf(messages[0], PAM_MSG(pamh, 129,
	"GECOS information was not updated: check NIS+ permissions."));
					(void) __pam_display_msg(pamh,
						PAM_ERROR_MSG, 1,
						messages, NULL);
				} else if (p->npd_field == NPD_SHELL) {
					sprintf(messages[0], PAM_MSG(pamh, 130,
	"SHELL information was not updated: check NIS+ permissions."));
					(void) __pam_display_msg(pamh,
						PAM_ERROR_MSG, 1,
						messages, NULL);
				} else if (p->npd_field == NPD_SECRETKEY) {
					sprintf(messages[0], PAM_MSG(pamh, 131,
		"NIS+ Credential information was not updated."));
					(void) __pam_display_msg(pamh,
						PAM_ERROR_MSG, 1,
						messages, NULL);
				}
			}
			/* check for collision with PAM_* return code */
			(void) __npd_free_errlist(errlist);
			retcode = PAM_NISPLUS_PARTIAL_SUCCESS;
			goto out;
		}
		(void) __npd_free_errlist(errlist);
	}
	retcode = PAM_SUCCESS;
out:
	if (old_passwd) {
		memset(old_passwd, 0, strlen(old_passwd));
		free(old_passwd);
	}
	return (retcode);
}


static int
update_attr(pam_handle_t *pamh, char *field, char **data, char *usrname,
	int opwcmd, char **sh_p, char **gecos_p,
	struct passwd *nisplus_pwd, int failover, int privileged,
	nis_result *passwd_res, int nowarn)
{
	entry_col	ecol[8];
	nis_object	*eobj;
	nis_result	*mres;
	char		mname[NIS_MAXNAMELEN];
	struct spwd	sp;		/* new attr values in here */
	char		*value;
	int		maxdate;
	int		mindate;
	int		warndate;
	static char	lkstring[] = "*LK*"; /* ??? in header */
	int		flag = 0;	/* any change in shadow column */
	char		**data_p = data;
	char		shadow[80];
	char		*newhome;
	char		*newgecos;
	char		*newsh;
	char 		messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	if (strcmp(field, "attr") == 0) {
		/*
		 * Obtain the old aging information. And modify, if need be,
		 * on top.
		 */
		if (opwcmd)
			nisplus_populate_age(NIS_RES_OBJECT(passwd_res), &sp);

		(void) memset((char *)ecol, 0, sizeof (ecol));
		while (*data != NULL) {
			/* AUTHTOK_DEL: not applicable */

			/* check attribute: AUTHTOK_LK */
			if ((value = attr_match("AUTHTOK_LK", *data))
								!= NULL) {
				/* new update protocol doesn't support this */
				if (opwcmd == FALSE)
					return (PAM_PERM_DENIED);

				if (strcmp(value, "1") == 0) {
					/* lock password */
					ecol[1].ec_value.ec_value_val =
					    &lkstring[0];
					ecol[1].ec_value.ec_value_len =
					    strlen(&lkstring[0]) + 1;
					ecol[1].ec_flags = EN_CRYPT|EN_MODIFIED;

					if (!(attr_find
					    ("AUTHTOK_EXP", data_p))) {
						sp.sp_lstchg = DAY_NOW;
						flag = 1;
					}
				}
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_EXP */
			if ((value = attr_match("AUTHTOK_EXP", *data))
			    != NULL) {
				/* new update protocol doesn't support this */
				if (opwcmd == FALSE)
					return (PAM_PERM_DENIED);

				if (strcmp(value, "1") == 0) {
					/* expire password */
					sp.sp_lstchg = (long) 0;
					flag = 1;
				}
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_MAXAGE */
			if ((value = attr_match("AUTHTOK_MAXAGE", *data))
			    != NULL) {
				/* new update protocol doesn't support this */
				if (opwcmd == FALSE)
					return (PAM_PERM_DENIED);

				/* set max field */
				maxdate = (int)atol(value);
				if (!(attr_find
					("AUTHTOK_MINAGE", data_p)) &&
					sp.sp_min == -1)
					sp.sp_min = 0;
				if (maxdate == -1) {	/* turn off aging */
					sp.sp_min = -1;
					sp.sp_warn = -1;
				} else if (sp.sp_max == -1)
					sp.sp_lstchg = DAY_NOW;

				sp.sp_max = maxdate;
				flag = 1;
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_MINAGE */
			if ((value = attr_match("AUTHTOK_MINAGE", *data))
			    != NULL) {
				/* new update protocol doesn't support this */
				if (opwcmd == FALSE)
					return (PAM_PERM_DENIED);

				/* set min field */
				mindate = (int)atol(value);
				if (!(attr_find
					("AUTHTOK_MAXAGE", data_p)) &&
				    sp.sp_max == -1 && mindate != -1)
					return (PAM_AUTHTOK_ERR);
				sp.sp_min = mindate;
				flag = 1;
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_WARNDATE */
			if ((value = attr_match("AUTHTOK_WARNDATE", *data))
			    != NULL) {
				/* new update protocol doesn't support this */
				if (opwcmd == FALSE)
					return (PAM_PERM_DENIED);

				/* set warn field */
				warndate = (int)atol(value);
				if (sp.sp_max == -1 && warndate != -1)
					return (PAM_AUTHTOK_ERR);
				sp.sp_warn = warndate;
				flag = 1;
				data++;
				continue;
			}

			if ((value = attr_match("AUTHTOK_SHELL", *data))
			    != NULL) {
				/* see if quick check */
				if (usrname == NULL)
					return (PAM_SUCCESS);

				if (nisplus_pwd == NULL && opwcmd) {
				    if (!nowarn) {
					sprintf(messages[0], PAM_MSG(pamh, 132,
					    "No NIS+ record"));
					(void) __pam_display_msg(pamh,
						PAM_ERROR_MSG, 1,
						messages, NULL);
				    }
				    return (PAM_AUTHTOK_RECOVERY_ERR);
				}

				/*
				 * If failover, we already got the shell info
				 * in "shell". Don't ask again.
				 */
				if (failover)
					newsh = *sh_p;
				else
					newsh = getloginshell(pamh,
					    nisplus_pwd->pw_shell,
					    privileged, nowarn);

				/* if NULL, shell unchanged */
				if (newsh == NULL)
					return (PAM_SUCCESS);

				if (opwcmd || failover) {
					ecol[6].ec_value.ec_value_val = newsh;
					ecol[6].ec_value.ec_value_len =
					    strlen(newsh) + 1;
					ecol[6].ec_flags = EN_MODIFIED;
				} else
					*sh_p = newsh;
				data++;
				continue;
			}

			if ((value = attr_match("AUTHTOK_HOMEDIR", *data))
			    != NULL) {
				/* new update protocol doesn't support this */
				if (opwcmd == FALSE)
					return (PAM_PERM_DENIED);

				/* home directory */
				if (nisplus_pwd == NULL) {
				    if (!nowarn) {
					sprintf(messages[0], PAM_MSG(pamh, 132,
					    "No NIS+ record"));
					(void) __pam_display_msg(pamh,
						PAM_ERROR_MSG, 1,
						messages, NULL);
				    }
				    return (PAM_AUTHTOK_RECOVERY_ERR);
				}
				newhome = gethomedir(pamh, nisplus_pwd->pw_dir,
						nowarn);
				/* if NULL, homedir unchanged */
				if (newhome == NULL)
					return (PAM_SUCCESS);
				ecol[5].ec_value.ec_value_val = newhome;
				ecol[5].ec_value.ec_value_len =
				    strlen(newhome) + 1;
				ecol[5].ec_flags = EN_MODIFIED;
				data++;
				continue;
			}

			if ((value = attr_match("AUTHTOK_GECOS", *data))
			    != NULL) {
				/* see if quick check */
				if (usrname == NULL)
					return (PAM_SUCCESS);

				/* finger information */
				if (nisplus_pwd == NULL && opwcmd) {
				    if (!nowarn) {
					sprintf(messages[0], PAM_MSG(pamh, 132,
					    "No NIS+ record"));
					(void) __pam_display_msg(pamh,
						PAM_ERROR_MSG, 1,
						messages, NULL);
				    }
				    return (PAM_AUTHTOK_RECOVERY_ERR);
				}
				if (failover) {
					newgecos = *gecos_p;
				} else {
					newgecos = getfingerinfo(pamh,
						nisplus_pwd->pw_gecos, nowarn);
				}

				/* if NULL, gecos unchanged */
				if (newgecos == NULL)
					return (PAM_SUCCESS);

				if (opwcmd || failover) {
					ecol[4].ec_value.ec_value_val =
					    newgecos;
					ecol[4].ec_value.ec_value_len =
					    strlen(newgecos) + 1;
					ecol[4].ec_flags = EN_MODIFIED;
				} else
					*gecos_p = newgecos;
				data++;
				continue;
			}
		} /* while */

		if (usrname == NULL)
			return (PAM_SUCCESS);

		if (flag && opwcmd) {
			/* prepare shadow column */
			if (sp.sp_expire == -1) {
				sprintf(shadow, "%ld:%ld:%ld:%ld:%ld::%lu",
				    sp.sp_lstchg,
				    sp.sp_min,
				    sp.sp_max,
				    sp.sp_warn,
				    sp.sp_inact,
				    sp.sp_flag);
			} else {
				sprintf(shadow, "%ld:%ld:%ld:%ld:%ld:%ld:%lu",
				    sp.sp_lstchg,
				    sp.sp_min,
				    sp.sp_max,
				    sp.sp_warn,
				    sp.sp_inact,
				    sp.sp_expire,
				    sp.sp_flag);
			}
			ecol[7].ec_value.ec_value_val = shadow;
			ecol[7].ec_value.ec_value_len = strlen(shadow) + 1;
			ecol[7].ec_flags = EN_CRYPT|EN_MODIFIED;
		}

		if (opwcmd || failover) {
			eobj = nis_clone_object(NIS_RES_OBJECT(passwd_res),
			    NULL);
			if (eobj == NULL) {
				syslog(LOG_ERR, "NIS+ clone object failed");
				return (PAM_AUTHTOK_RECOVERY_ERR);
			}
			eobj->EN_data.en_cols.en_cols_val = ecol;
			eobj->EN_data.en_cols.en_cols_len = 8;

			/* strlen("[name=],.") + null + "." = 17 */
			if ((strlen(usrname) +
			    strlen(NIS_RES_OBJECT(passwd_res)->zo_name) +
			    strlen(NIS_RES_OBJECT(passwd_res)->zo_domain) +
			    17) > (size_t) NIS_MAXNAMELEN) {
				syslog(LOG_ERR, "NIS+ name too long");
				return (PAM_BUF_ERR);
			}
			sprintf(mname, "[name=%s],%s.%s", usrname,
				NIS_RES_OBJECT(passwd_res)->zo_name,
				NIS_RES_OBJECT(passwd_res)->zo_domain);
			if (mname[strlen(mname) - 1] != '.')
				(void) strcat(mname, ".");
			mres = nis_modify_entry(mname, eobj, 0);
			if (mres->status != NIS_SUCCESS) {
				sprintf(messages[0], PAM_MSG(pamh, 133,
		"NIS+ password information update failed (update_attr)"));
				(void) __pam_display_msg(pamh,
					PAM_ERROR_MSG, 1, messages, NULL);

				return (PAM_AUTHTOK_RECOVERY_ERR);
			}

			sprintf(messages[0], PAM_MSG(pamh, 112,
				"NIS+ password information changed for %s"),
				usrname);
			(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
				1, messages, NULL);
		}
	}
	return (PAM_SUCCESS);
}

/*
 * Return reencrypted secret key.
 * The first two if statements should always succeed as these tests
 * are also carried out in getnewpasswd().
 */
static char *
reencrypt_secret(char *oldsecret, char *oldpass, char *newpass)
{
	static char crypt[HEXKEYBYTES + KEYCHECKSUMSIZE + 1];

	if (xdecrypt(oldsecret, oldpass) == 0)
		return (NULL); /* cbc_crypt failed */

	if (memcmp(oldsecret, &(oldsecret[HEXKEYBYTES]), KEYCHECKSUMSIZE) != 0)
		return (NULL); /* didn't really decrypt */

	(void) memcpy(crypt, oldsecret, HEXKEYBYTES);
	(void) memcpy(crypt + HEXKEYBYTES, oldsecret, KEYCHECKSUMSIZE);
	crypt[HEXKEYBYTES + KEYCHECKSUMSIZE] = 0;

	if (xencrypt(crypt, newpass) == 0)
		return (NULL); /* cbc_crypt encryption failed */

	return (crypt);
}


/*
 * Revert back to the old passwd
 */
static nis_error
revert2oldpasswd(char *usrname, nis_result *passwd_res)
{
	entry_col ecol[8];
	nis_object *eobj;
	nis_result *mres;
	char mname[NIS_MAXNAMELEN];

	/*
	 * clear column data
	 */
	(void) memset((char *) ecol, 0, sizeof (ecol));

	/*
	 * passwd (col 1)
	 */
	ecol[1].ec_value.ec_value_val =
		ENTRY_VAL(NIS_RES_OBJECT(passwd_res), 1);
	ecol[1].ec_value.ec_value_len =
		ENTRY_LEN(NIS_RES_OBJECT(passwd_res), 1);
	ecol[1].ec_flags = EN_CRYPT|EN_MODIFIED;

	/*
	 * build entry based on the global "passwd_res"
	 */
	eobj = nis_clone_object(NIS_RES_OBJECT(passwd_res), NULL);
	if (eobj == NULL)
		return (NIS_SYSTEMERROR);
	eobj->EN_data.en_cols.en_cols_val = ecol;
	eobj->EN_data.en_cols.en_cols_len = 8;

	sprintf(mname, "[name=%s],%s.%s", usrname,
		NIS_RES_OBJECT(passwd_res)->zo_name,
		NIS_RES_OBJECT(passwd_res)->zo_domain);

	mres = nis_modify_entry(mname, eobj, 0);
	return (mres->status);
}

#endif /* PAM_NISPLUS */
