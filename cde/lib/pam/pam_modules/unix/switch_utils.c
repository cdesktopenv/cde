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
/* $XConsortium: switch_utils.c /main/5 1996/05/09 04:32:20 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)switch_utils.c 1.31     96/02/02 SMI"

#include "unix_headers.h"

/*
 * XXX
 * This file relies on the fact that you are using NSS_SWITCH.
 * If PAM_NIS or PAM_NISPLUS is defined, then the
 * include files for nss_switch are included.
 * Otherwise, the repository is assumed to be FILES,
 * and NSS_SWITCH is not used.
 */

static void	pr_config();

/*
 * The following is similar to getpwnam() except that it specifies
 * where to get the information. This is modeled after getpwnam_r().
 */
#ifdef PAM_NIS
static void
nss_nis_passwd(p)
	nss_db_params_t	*p;
{
	p->name = NSS_DBNAM_PASSWD;
	p->flags |= NSS_USE_DEFAULT_CONFIG;
	p->default_config = "nis";
}

static void
nss_nis_shadow(p)
	nss_db_params_t	*p;
{
	p->name = NSS_DBNAM_SHADOW;
	p->config_name    = NSS_DBNAM_PASSWD;	/* Use config for "passwd" */
	p->flags |= NSS_USE_DEFAULT_CONFIG;
	p->default_config = "nis";
}
#endif /* PAM_NIS */

#ifdef PAM_NISPLUS
static void
nss_nisplus_passwd(p)
	nss_db_params_t	*p;
{
	p->name = NSS_DBNAM_PASSWD;
	p->flags |= NSS_USE_DEFAULT_CONFIG;
	p->default_config = "nisplus";
}

static void
nss_nisplus_shadow(p)
	nss_db_params_t	*p;
{
	p->name = NSS_DBNAM_SHADOW;
	p->config_name    = NSS_DBNAM_PASSWD;	/* Use config for "passwd" */
	p->flags |= NSS_USE_DEFAULT_CONFIG;
	p->default_config = "nisplus";
}
#endif /* PAM_NISPLUS */

static char *
gettok(nextpp)
	char	**nextpp;
{
	char	*p = *nextpp;
	char	*q = p;
	char	c;

	if (p == 0) {
		return (0);
	}
	while ((c = *q) != '\0' && c != ':') {
		q++;
	}
	if (c == '\0') {
		*nextpp = 0;
	} else {
		*q++ = '\0';
		*nextpp = q;
	}
	return (p);
}

#if (PAM_NIS || PAM_NISPLUS)

/*
 * Return values: 0 = success, 1 = parse error, 2 = erange ...
 * The structure pointer passed in is a structure in the caller's space
 * wherein the field pointers would be set to areas in the buffer if
 * need be. instring and buffer should be separate areas.
 */
static int
str2passwd(const char *instr, int lenstr, void *ent, char *buffer, int buflen)
{
	struct passwd	*passwd	= (struct passwd *)ent;
	char		*p, *next;
	int		black_magic;	/* "+" or "-" entry */

	if (lenstr + 1 > buflen) {
		return (NSS_STR_PARSE_ERANGE);
	}
	/*
	 * We copy the input string into the output buffer and
	 * operate on it in place.
	 */
	(void) memcpy(buffer, instr, lenstr);
	buffer[lenstr] = '\0';

	next = buffer;

	passwd->pw_name = p = gettok(&next);		/* username */
	if (*p == '\0') {
		/* Empty username;  not allowed */
		return (NSS_STR_PARSE_PARSE);
	}
	black_magic = (*p == '+' || *p == '-');
	if (black_magic) {
		passwd->pw_uid	= UID_NOBODY;
		passwd->pw_gid	= GID_NOBODY;
		/*
		 * pwconv tests pw_passwd and pw_age == NULL
		 */
		passwd->pw_passwd = "";
		passwd->pw_age	= "";
		/*
		 * the rest of the passwd entry is "optional"
		 */
		passwd->pw_comment = "";
		passwd->pw_gecos = "";
		passwd->pw_dir	= "";
		passwd->pw_shell = "";
	}

	passwd->pw_passwd = p = gettok(&next);		/* password */
	if (p == 0) {
		if (black_magic)
			return (NSS_STR_PARSE_SUCCESS);
		else
			return (NSS_STR_PARSE_PARSE);
	}
	for (; *p != '\0'; p++) {			/* age */
		if (*p == ',') {
			*p++ = '\0';
			break;
		}
	}
	passwd->pw_age = p;

	p = next;					/* uid */
	if (p == 0 || *p == '\0') {
		if (black_magic)
			return (NSS_STR_PARSE_SUCCESS);
		else
			return (NSS_STR_PARSE_PARSE);
	}
	if (!black_magic) {
		passwd->pw_uid = strtol(p, &next, 10);
		if (next == p) {
			/* uid field should be nonempty */
			return (NSS_STR_PARSE_PARSE);
		}
		/*
		 * The old code (in 2.0 thru 2.5) would check
		 * for the uid being negative, or being greater
		 * than 60001 (the rfs limit).  If it met either of
		 * these conditions, the uid was translated to 60001.
		 *
		 * Now we just check for negative uids; anything else
		 * is administrative policy
		 */
		if (passwd->pw_uid < 0)
			passwd->pw_uid = UID_NOBODY;
	}
	if (*next++ != ':') {
		if (black_magic)
			p = gettok(&next);
		else
			return (NSS_STR_PARSE_PARSE);
	}
	p = next;					/* gid */
	if (p == 0 || *p == '\0') {
		if (black_magic)
			return (NSS_STR_PARSE_SUCCESS);
		else
			return (NSS_STR_PARSE_PARSE);
	}
	if (!black_magic) {
		passwd->pw_gid = strtol(p, &next, 10);
		if (next == p) {
			/* gid field should be nonempty */
			return (NSS_STR_PARSE_PARSE);
		}
		/*
		 * gid should be non-negative; anything else
		 * is administrative policy.
		 */
		if (passwd->pw_gid < 0)
			passwd->pw_gid = GID_NOBODY;
	}
	if (*next++ != ':') {
		if (black_magic)
			p = gettok(&next);
		else
			return (NSS_STR_PARSE_PARSE);
	}

	passwd->pw_gecos = passwd->pw_comment = p = gettok(&next);
	if (p == 0) {
		if (black_magic)
			return (NSS_STR_PARSE_SUCCESS);
		else
			return (NSS_STR_PARSE_PARSE);
	}

	passwd->pw_dir = p = gettok(&next);
	if (p == 0) {
		if (black_magic)
			return (NSS_STR_PARSE_SUCCESS);
		else
			return (NSS_STR_PARSE_PARSE);
	}

	passwd->pw_shell = p = gettok(&next);
	if (p == 0) {
		if (black_magic)
			return (NSS_STR_PARSE_SUCCESS);
		else
			return (NSS_STR_PARSE_PARSE);
	}

	/* Better not be any more fields... */
	if (next == 0) {
		/* Successfully parsed and stored */
		return (NSS_STR_PARSE_SUCCESS);
	}
	return (NSS_STR_PARSE_PARSE);
}

typedef const char *constp;

static bool_t	/* 1 means success and more input, 0 means error or no more */
getfield(nextp, limit, uns, valp)
	constp		*nextp;
	constp		limit;
	int		uns;
	void		*valp;
{
	constp		p = *nextp;
	char		*endfield;
	char		numbuf[12];  /* Holds -2^31 and trailing \0 */
	int		len;

	if (p == 0 || p >= limit) {
		return (0);
	}
	if (*p == ':') {
		p++;
		*nextp = p;
		return (p < limit);
	}
	if ((len = limit - p) > sizeof (numbuf) - 1) {
		len = sizeof (numbuf) - 1;
	}
	/*
	 * We want to use strtol() and we have a readonly non-zero-terminated
	 *   string, so first we copy and terminate the interesting bit.
	 *   Ugh.  (It's convenient to terminate with a colon rather than \0).
	 */
	if ((endfield = memccpy(numbuf, p, ':', len)) == 0) {
		if (len != limit - p) {
			/* Error -- field is too big to be a legit number */
			return (0);
		}
		numbuf[len] = ':';
		p = limit;
	} else {
		p += (endfield - numbuf);
	}
	if (uns) {
		*((unsigned long *)valp) = strtoul(numbuf, &endfield, 10);
	} else {
		*((long *)valp) = strtol(numbuf, &endfield, 10);
	}
	if (*endfield != ':') {
		/* Error -- expected <integer><colon>, got something else */
		return (0);
	}
	*nextp = p;
	return (p < limit);
}

/*
 *  str2spwd() -- convert a string to a shadow passwd entry.  The parser is
 *	more liberal than the passwd or group parsers;  since it's legitimate
 *	for almost all the fields here to be blank, the parser lets one omit
 *	any number of blank fields at the end of the entry.  The acceptable
 *	forms for '+' and '-' entries are the same as those for normal entries.
 *  === Is this likely to do more harm than good?
 *
 * Return values: 0 = success, 1 = parse error, 2 = erange ...
 * The structure pointer passed in is a structure in the caller's space
 * wherein the field pointers would be set to areas in the buffer if
 * need be. instring and buffer should be separate areas.
 */
int
str2spwd(instr, lenstr, ent, buffer, buflen)
	const char	*instr;
	int		lenstr;
	void	*ent; /* really (struct spwd *) */
	char	*buffer;
	int	buflen;
{
	struct spwd	*shadow	= (struct spwd *)ent;
	const char	*p = instr, *limit;
	char		*bufp;
	int	lencopy, black_magic;

	limit = p + lenstr;
	if ((p = memchr(instr, ':', lenstr)) == 0 ||
		++p >= limit ||
		(p = memchr(p, ':', limit - p)) == 0) {
		lencopy = lenstr;
		p = 0;
	} else {
		lencopy = p - instr;
		p++;
	}
	if (lencopy + 1 > buflen) {
		return (NSS_STR_PARSE_ERANGE);
	}
	(void) memcpy(buffer, instr, lencopy);
	buffer[lencopy] = 0;

	black_magic = (*instr == '+' || *instr == '-');
	shadow->sp_namp = bufp = buffer;
	shadow->sp_pwdp	= 0;
	shadow->sp_lstchg = -1;
	shadow->sp_min	= -1;
	shadow->sp_max	= -1;
	shadow->sp_warn	= -1;
	shadow->sp_inact = -1;
	shadow->sp_expire = -1;
	shadow->sp_flag	= 0;

	if ((bufp = strchr(bufp, ':')) == 0) {
		if (black_magic)
			return (NSS_STR_PARSE_SUCCESS);
		else
			return (NSS_STR_PARSE_PARSE);
	}
	*bufp++ = '\0';

	shadow->sp_pwdp = bufp;
	if (instr == 0) {
		if ((bufp = strchr(bufp, ':')) == 0) {
			if (black_magic)
				return (NSS_STR_PARSE_SUCCESS);
			else
				return (NSS_STR_PARSE_PARSE);
		}
		*bufp++ = '\0';
		p = bufp;
	} /* else p was set when we copied name and passwd into the buffer */

	if (!getfield(&p, limit, 0, &shadow->sp_lstchg))
			return (NSS_STR_PARSE_SUCCESS);
	if (!getfield(&p, limit, 0, &shadow->sp_min))
			return (NSS_STR_PARSE_SUCCESS);
	if (!getfield(&p, limit, 0, &shadow->sp_max))
			return (NSS_STR_PARSE_SUCCESS);
	if (!getfield(&p, limit, 0, &shadow->sp_warn))
			return (NSS_STR_PARSE_SUCCESS);
	if (!getfield(&p, limit, 0, &shadow->sp_inact))
			return (NSS_STR_PARSE_SUCCESS);
	if (!getfield(&p, limit, 0, &shadow->sp_expire))
			return (NSS_STR_PARSE_SUCCESS);
	if (!getfield(&p, limit, 1, &shadow->sp_flag))
			return (NSS_STR_PARSE_SUCCESS);
	if (p != limit) {
		/* Syntax error -- garbage at end of line */
		return (NSS_STR_PARSE_PARSE);
	}
	return (NSS_STR_PARSE_SUCCESS);
}

static nss_XbyY_buf_t *buffer;
static DEFINE_NSS_DB_ROOT(db_root);

#define	GETBUF()	\
	NSS_XbyY_ALLOC(&buffer, sizeof (struct passwd), NSS_BUFLEN_PASSWD)

struct passwd *
getpwnam_from(name, rep)
	const char	*name;
	int		rep;
{
	nss_XbyY_buf_t  *b = GETBUF();
	nss_XbyY_args_t arg;

	if (b == 0)
		return (0);

	NSS_XbyY_INIT(&arg, b->result, b->buffer, b->buflen, str2passwd);
	arg.key.name = name;

	switch (rep) {
#ifdef PAM_NISPLUS
	case PAM_REP_NISPLUS:
		nss_search(&db_root, nss_nisplus_passwd, NSS_DBOP_PASSWD_BYNAME,
		    &arg);
		break;
#endif
#ifdef PAM_NIS
	case PAM_REP_NIS:
		nss_search(&db_root, nss_nis_passwd, NSS_DBOP_PASSWD_BYNAME,
		    &arg);
		break;
#endif
	default:
		return (NULL);
	}

	return (struct passwd *) NSS_XbyY_FINI(&arg);
}

static nss_XbyY_buf_t *spbuf;
static DEFINE_NSS_DB_ROOT(spdb_root);

#define	GETSPBUF()	\
	NSS_XbyY_ALLOC(&spbuf, sizeof (struct spwd), NSS_BUFLEN_SHADOW)

struct spwd *
getspnam_from(name, rep)
	const char	*name;
	int		rep;
{
	nss_XbyY_buf_t  *b = GETSPBUF();
	nss_XbyY_args_t arg;

	if (b == 0)
		return (0);

	NSS_XbyY_INIT(&arg, b->result, b->buffer, b->buflen, str2spwd);
	arg.key.name = name;
	switch (rep) {
#ifdef PAM_NISPLUS
	case PAM_REP_NISPLUS:
		nss_search(&spdb_root, nss_nisplus_shadow,
		    NSS_DBOP_SHADOW_BYNAME, &arg);
		break;
#endif
#ifdef PAM_NIS
	case PAM_REP_NIS:
		nss_search(&spdb_root, nss_nis_shadow,
		    NSS_DBOP_SHADOW_BYNAME, &arg);
		break;
#endif
	default:
		return (NULL);
	}
	return (struct spwd *) NSS_XbyY_FINI(&arg);
}
#endif /* (PAM_NIS || PAM_NISPLUS) */

static void
pr_config(pamh)
	pam_handle_t *pamh;
{
	char 		messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	sprintf(messages[0], PAM_MSG(pamh, 1,
	"Supported configurations for passwd management are as follows:"));
	sprintf(messages[1], PAM_MSG(pamh, 2, "    passwd: files"));
	sprintf(messages[2], PAM_MSG(pamh, 3, "    passwd: files nis"));
	sprintf(messages[3], PAM_MSG(pamh, 4, "    passwd: files nisplus"));
	sprintf(messages[4], PAM_MSG(pamh, 5, "    passwd: compat"));
	sprintf(messages[5], PAM_MSG(pamh, 6, "    passwd: compat AND"));
	sprintf(messages[6], PAM_MSG(pamh, 7, "    passwd_compat: nisplus"));
	sprintf(messages[7],
	PAM_MSG(pamh, 8, "Please check your /etc/nsswitch.conf file"));

	/* display the above 8 messages */
	(void) __pam_display_msg(pamh, PAM_ERROR_MSG, 8, messages, NULL);
}

/*
 * get name services (or repositories) of passwd.
 * o_rep: the specified respository in command line. If no repository is
 *	specified in the command line, o_rep is	equal to PAM_REP_DEFAULT.
 * return value: new repositories
 *	1. In the case of PAM_REP_DEFAULT, new repositories are from nsswitch
 *	   file (as long as it represents a valid and supported configuration).
 *	2. In the case of specified repository, it should be present as one
 *	   of the valid services (or repositories) in nsswitch file.
 *	   A warning is printed if this happens. Operation is continued.
 */
int
get_ns(pamh, o_rep, debug, nowarn)
	pam_handle_t *pamh;
	int		o_rep;
	int debug;
	int nowarn;
{
#if (PAM_NIS || PAM_NISPLUS)
	struct __nsw_switchconfig *conf = NULL;
	struct __nsw_switchconfig *confcomp = NULL;
	enum __nsw_parse_err pserr;
	struct __nsw_lookup *lkp;
	struct __nsw_lookup *lkp2;
	int	rep = 0;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	/* yppasswd/nispasswd doesn't care about nsswitch file */
	if (IS_OPWCMD(o_rep))
		return (o_rep);

	conf = __nsw_getconfig("passwd", &pserr);
	if (conf == NULL) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 9,
				"Can't find name service for passwd"));
			__pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}

		if (IS_NISPLUS(o_rep)) {
		    if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 10,
				"You may not use nisplus repository"));
			__pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		    }
		    return (-1);
		} else if (o_rep != PAM_REP_DEFAULT) {
			/*
			 * The user specified a repository:
			 * Allow the user to try to change the passwd
			 * even though the specified repository is
			 * not listed in nsswitch.conf
			 */
			return (o_rep);
		} else {
			/*
			 * The user did not specify a repository:
			 * Allow the user to try to change the passwd
			 * in the default repositories (files and nis)
			 * even though we can not find the name service
			 * switch entry.
			 */
			rep = PAM_REP_FILES | PAM_REP_NIS;
			return (rep);	/* default */
		}
	}

	if (debug)
		syslog(LOG_DEBUG, "number of services is %d",
			conf->num_lookups);
	lkp = conf->lookups;

	/*
	 * XXX: Currently we do now support more than 2 services
	 */
	if (conf->num_lookups > 2) {
		pr_config(pamh);
		return (-1);
	} else if (conf->num_lookups == 1) {
		/* files or compat */
		if (strcmp(lkp->service_name, "files") == 0) {
			rep |= PAM_REP_FILES;
			if (o_rep == PAM_REP_NIS || o_rep == PAM_REP_NISPLUS) {
			    if (!nowarn) {
				sprintf(messages[0],
				    PAM_MSG(pamh, 11,
	"Your specified repository is not defined in the nsswitch file!"));
				__pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			    }
			    return (o_rep);
			}
			return (rep);
		} else if (strcmp(lkp->service_name, "compat") == 0) {
			/* get passwd_compat */
			confcomp = __nsw_getconfig("passwd_compat", &pserr);
			if (confcomp == NULL) {
				rep = PAM_REP_FILES | PAM_REP_NIS;
				if (o_rep == PAM_REP_NISPLUS) {
				    if (!nowarn) {
					sprintf(messages[0],
						PAM_MSG(pamh, 11,
	"Your specified repository is not defined in the nsswitch file!"));
					__pam_display_msg(pamh, PAM_ERROR_MSG,
						1, messages, NULL);
				    }
				    return (o_rep);
				} else if (o_rep != PAM_REP_DEFAULT)
					return (o_rep);
				else
					return (rep);
			} else {
				/* check the service: nisplus? */
				if (strcmp(confcomp->lookups->service_name,
				    "nisplus") == 0) {
					rep = PAM_REP_FILES | PAM_REP_NISPLUS;
					if (o_rep == PAM_REP_NIS) {
					    if (!nowarn) {
						sprintf(messages[0],
						PAM_MSG(pamh, 11,
	"Your specified repository is not defined in the nsswitch file!"));
						__pam_display_msg(pamh,
							PAM_ERROR_MSG,
							1, messages, NULL);
					    }
					    return (o_rep);
					} else if (o_rep != PAM_REP_DEFAULT)
						return (o_rep);
					else
						return (rep);
				} else {
					/* passwd_compat must be nisplus?? */
					return (-1);
				}
			}
		} else {
			pr_config(pamh);
			return (-1);
		}
	} else  { /* two services */
		lkp = conf->lookups;
		lkp2 = lkp->next;
		if (strcmp(lkp->service_name, "files") == 0) {
			/* files nis, or files nisplus */
			rep |= PAM_REP_FILES;
			/* continue */
		} else {
			pr_config(pamh);
			return (-1);
		}
		if (strcmp(lkp2->service_name, "nis") == 0) {
			rep |= PAM_REP_NIS;
			if (o_rep == PAM_REP_NISPLUS) {
			    if (!nowarn) {
				sprintf(messages[0],
				    PAM_MSG(pamh, 11,
	"Your specified repository is not defined in the nsswitch file!"));
				__pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			    }
			    return (o_rep);
			} else if (o_rep != PAM_REP_DEFAULT)
				return (o_rep);
			else
				return (rep);
		} else if (strcmp(lkp2->service_name, "nisplus") == 0) {
			rep |= PAM_REP_NISPLUS;
			if (o_rep == PAM_REP_NIS) {
			    if (!nowarn) {
				sprintf(messages[0],
				    PAM_MSG(pamh, 11,
	"Your specified repository is not defined in the nsswitch file!"));
				__pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			    }
			    return (o_rep);
			} else if (o_rep != PAM_REP_DEFAULT)
				return (o_rep);
			else
				return (rep);
		} else {
			pr_config(pamh);
			return (-1);
		}
	}
#else
	return (PAM_REP_FILES);
#endif /* (PAM_NIS || PAM_NISPLUS) */
}
