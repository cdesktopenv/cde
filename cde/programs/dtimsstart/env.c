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
/* @(#)$XConsortium: env.c /main/9 1996/10/30 05:57:27 pascale $ */

#include <pwd.h>
#include "xims.h"


typedef	enum {
    P_Shell,	/* posix shell */
    K_Shell,	/* Korn shell */
    C_Shell	/* C shell */
} ShellType;


    /* local func */
static int	shell_type(/* shell */);

int	set_cmd_env(void)
{
    int		ret = NoError;
    char	**aliases = (char **)0;
    char	*p;
    UserEnv	*uenv = &userEnv;

    if ((ret = get_user_environ()) != NoError)
	return ret;

    if ((ret = read_cmd_conf()) != NoError)
	return ret;

# ifdef	old_hpux
    p = get_real_locale(uenv->locale, &aliases);
    if (p) {
	uenv->real_locale = NEWSTR(p);
	uenv->locale_aliases = aliases;
    }
# endif	/* old_hpux */

    expand_cmd_conf();

    return ret;
}


int	get_user_environ(void)
{
    int		ret = NoError;
    char	buf[BUFSIZ], *p;
    int		n;
    UserEnv	*uenv = &userEnv;

    ret = NoError;

    CLR(&userEnv, UserEnv);

    if ((p = getenv("LANG")) && *p)
	uenv->locale = NEWSTR(p);
    else
	return ErrNoLocale;

    /* find the CDE generic locale name */
    if (find_CDE_locale_name() != NoError)
	return ErrNoCDELocale;

    if ((p = getenv("HOME")) && *p)
	uenv->homedir = NEWSTR(p);
    else {
	if (OpMode != MODE_LIST && OpMode != MODE_REMCONF) {
#ifdef	ALLOW_NOHOME
	    p = "/var/tmp";
	    uenv->homedir = NEWSTR(p);
#else
	    ret = ErrNoHome;
#endif	/* ALLOW_NOHOME */
	}
    }

    if ((p = Opt.DisplayName) && *p) {
	strcpy(buf, "DISPLAY=");
	strcat(buf, p);
	putenv(XtNewString(buf));
    } else
	p = getenv("DISPLAY");
    if (p && *p)
	uenv->displayname = NEWSTR(p);
    else {
	if (OpMode == MODE_START /* || OpMode == MODE_CURRENT */
	    || (OpMode == MODE_MODE && (OpFlag & USE_WINDOW_MASK)))
	    return ErrNoDisplay;
    }

    if (p = getenv("XMODIFIERS"))
	uenv->xmodifiers = NEWSTR(p);
    else
	uenv->xmodifiers = NULL;

    gethostname(buf, BUFSIZ);
    uenv->hostname = NEWSTR(buf);

    if (!(p = getlogin()))
      {
	struct passwd *pw;
	pw = getpwuid(getuid());
	p = pw->pw_name;
      }
    
    uenv->username = NEWSTR(p);

    n = 0;
    if (p = std_dpy_str(uenv->displayname, &n))
	uenv->displaydir = p;
    else
	uenv->displaydir = NEWSTR(uenv->displayname);
    uenv->screen_num = n;

    return ret;
}


int	expand_string(char *in_str, char *out_str, int out_len, ImsConf *ims)
{
    char	*p, *q, *ep;
    char        str[20];
    int		len = 0;
    UserEnv	*uenv = &userEnv;
    CmdConf	*conf = &Conf;

    p = in_str; q = out_str;
    if (*p == '~' && p[1] == '/') {
	q = strcpyx(q, uenv->homedir);
	*q++ = '/';
	p += 2;
	out_len -= q - out_str;
    }
    while (*p && out_len > 0) {
	ep = NULL;
	if (*p == '%') {
	    switch(p[1]) {

		case 'I':	ep = conf->imsConfDir; break;
		case 'R':	ep = conf->imsAppDir; break;
		case 'G':	ep = conf->imsLogDir; break;
		case 'g':	ep = conf->imsLogFile; break;
		case 'b':	ep = conf->imsDir; break;
		case 'S':	ep = conf->userImsDir; break;
		case 'T':	ep = conf->userTmpDir; break;
		case 'A':	ep = conf->userAltDir; break;
		case 'C':	ep = conf->dt->confDir; break;
		case 'U':	ep = conf->dt->userDir; break;
# ifdef	old_hpux
		case 'V':	ep = conf->vue->confDir; break;
		case 'X':	ep = conf->vue->userDir; break;
# endif	/* old_hpux */

		case 'L':	if (ep = uenv->real_locale)	break;
		case 'l':	ep = uenv->locale; break;
		case 'H':	ep = uenv->homedir; break;
		case 'u':	ep = uenv->username; break;
		case 'h':	ep = uenv->hostname; break;
		case 'D':	ep = uenv->displayname; break;
		case 'd':	ep = uenv->displaydir; break;

		case 'N':	if (ims && (ep = ims->servername2)) break;
		case 'n':	if (ims) ep = ims->servername; break;
		case 'c':	if (ims) ep = ims->classname; break;
		case 's':	sprintf(str, "%ld", (long) uenv->screen_num); 
		                if (str[0] != '\0')  ep = str;
		                break;
		case 'r':	ep = userSel.hostname; break;

		case '%':	p++;
		default:	DPR2(("expand_string: '%%%c' unknown\n"));
				ep = 0; break;
	    }
	    if (ep) {
		if ((out_len -= (int) strlen(ep)) <= 0)	break;
		q = strcpyx(q, ep);
		p += 2;
		continue;
	    }
	}
	if (--out_len <= 0)	break;
	*q++ = *p++;
    }

    *q = 0;
    len = q - out_str;

    DPR3(("expand_string(\"%s\"):\t\"%s\"\n", in_str, out_str));

    if (out_len <= 0) {
	DPR(("expand_string(): buffer overflow (len=%d)\n", len));
    }

    return len;
}


static int	shell_type(char *shell)
{
    char	*p;
    int		len;

    if (!shell || !*shell) {
	shell = getenv("SHELL");
	if (!shell || !*shell)	return P_Shell;
    }

    if (strchr(p = shell, '/')) {
	for (len = strlen(p); len > 1 && p[len - 1] == '/'; len--) ;
	shell[len] = 0;
	if (p = strrchr(shell, '/'))	shell = p + 1;
    }
    if (strstr(shell, "ksh") != NULL)
	return K_Shell;
    else if (strstr(shell, "csh") != NULL)
	return C_Shell;

    return P_Shell;
}


int	make_new_environ(OutEnv *oenv, UserSelection *sel)
{
    ImsConf	*ims;
    EnvEnt	*ep, *ep2;
    int		num, i;
    int		proto;
    char	*p, **pp;
    char	buf[BUFSIZ], *bp;
    char	*xmod, *xinput;
    char	**setp, **unsetp;
    bool	xmod_done, xinput_done;
    RunEnv	*renv;

    /* if (!oenv)	return ErrInternal; */
    if (!sel)	sel = &userSel;

    CLR(oenv, OutEnv);
    renv = sel->renv;
    ims = sel->ent->ims;
    xmod = ENV_XMODIFIERS;
    xmod_done = False;
# ifdef	old_hpux
    xinput = xhp_xinput_name(NULL);
    xinput_done = (xinput) ? False : True;
# else
    xinput = NULL;
    xinput_done = True;
# endif	/* old_hpux */
    proto = renv ? renv->proto : default_protocol(ims);

    setp = unsetp = 0;
    if (ims->env_set)	setp = parse_strlist(ims->env_set, ' ');
    if (ims->env_unset)	unsetp = parse_strlist(ims->env_unset, ' ');

	/* set: ims->env_set, XMODIFIERS & X?INPUT */
    num = 0;
    if (setp)	for (pp = setp; *pp; pp++, num++) ;
    ep = oenv->set = ALLOC(num + 2 + 1, EnvEnt);

    for (i = 0; i < num && (p = setp[i]); i++)
	if (strcmp(p, xmod) && (!xinput || strcmp(p, xinput))) {
	    ep->name = NEWSTR(p);
	    ep++;
	}
    if (renv && renv->im_mod) {
	ep->name = NEWSTR(xmod);
	bp = strcpyx(bp = buf, ENV_MOD_IM);	/* "@im=" */
	bp = strcpyx(bp, renv->im_mod);
	ep->value = NEWSTR(buf);
	ep++;
	xmod_done = True;
    }
# ifdef	old_hpux
    if (!xinput_done && (proto == Proto_Xhp)) {
#ifdef	DEBUG
	if (!ims->servername) {
	    DPR(("make_new_environ(): '%s' servername not defined\n",
							sel->name));
	}
#endif
	ep->name = NEWSTR(xinput);
	ep->value = NEWSTR(ims->servername);
	ep++;
	xinput_done = True;
    }
# endif	/* old_hpux */
    if (ep == oenv->set) {
	FREE(oenv->set); oenv->set = (EnvEnt *)0;
    } else
	ep->name = NULL;

	/* unset: XMODIFIERS & X?INPUT, ims->env_unset */
    num = 0;
    if (unsetp)	for (pp = unsetp; *pp; pp++, num++) ;
    ep = oenv->unset = ALLOC(num + 2 + 1, EnvEnt);

    if (!xmod_done) {
	ep->name = NEWSTR(xmod);
	ep++;
	xmod_done = True;
    }
# ifdef	old_hpux
    if (!xinput_done) {
	ep->name = NEWSTR(xinput);
	ep++;
	xinput_done = True;
    }
# endif	/* old_hpux */
    for (i = 0; i < num && (p = unsetp[i]); i++) {
	if (strcmp(p, xmod) == 0 || (xinput && (strcmp(p, xinput) == 0)))
	    continue;
	if (oenv->set) {
	    for (ep2 = oenv->set; ep2->name; ep2++)
		if (strcmp(p, ep2->name) == 0) {
		    p = NULL;
		    break;
		}
	}
	if (p) {
	    ep->name = NEWSTR(p);
	    ep++;
	}
    }
    if (ep == oenv->unset) {
	FREE(oenv->unset); oenv->unset = (EnvEnt *)0;
    } else
	ep->name = NULL;

    FREE_LIST(setp);
    FREE_LIST(unsetp);
    FREE(xinput);

    return NoError;
}

/* print modified environment variables */
int	put_new_environ(OutEnv *oenv)
{
    EnvEnt	*ep;
    char	tmpbuf[BUFSIZ], *bp, *vp;
    int		typ = shell_type(Opt.ShellName);
    int		len;

    /* if (!oenv)				return ErrInternal; */
#ifdef	DEBUG
    if (DebugLvl >= 2)
	pr_OutEnv(oenv);
#endif

    if (!oenv->set && !oenv->unset)	return NoError;
    tmpbuf[0] = 0;

    if (typ == C_Shell) {	/* C-Shell format */
	bp = strcpyx(tmpbuf, "set noglob;\n");
	if (oenv->set) {
	    for (ep = oenv->set; ep->name; ep++) {
		if (!(vp = ep->value) && (vp = strchr(ep->name, '=')))
		    *vp++ = '\0';
		sprintf(bp, "setenv %s '%s';\n", ep->name, vp);
		bp += strlen(bp);
	    }
	}
	if (oenv->unset) {
	    bp = strcpyx(bp, "unsetenv ");
	    for (ep = oenv->unset; ep->name; ep++) {
		*bp++= ' ';
		bp = strcpyx(bp, ep->name);
	    }
	}
	bp = strcpyx(bp, ";\nunset noglob;\n");
    } else {			 	/* B-Shell format */
	bp = tmpbuf;
	if (oenv->set) {
	    for (ep = oenv->set; ep->name; ep++) {
		if (!(vp = ep->value) && (vp = strchr(ep->name, '=')))
		    *vp++ = '\0';
		sprintf(bp, "%s='%s';\n", ep->name, vp);
		bp += strlen(bp);
	    }
	    bp = strcpyx(bp, "export ");
	    for (ep = oenv->set; ep->name; ep++) {
		*bp++= ' ';
		bp = strcpyx(bp, ep->name);
	    }
	    bp = strcpyx(bp, ";\n");
	}
	if (oenv->unset) {
	    bp = strcpyx(bp, "unset ");
	    for (ep = oenv->unset; ep->name; ep++) {
		*bp++= ' ';
		bp = strcpyx(bp, ep->name);
	    }
	    bp = strcpyx(bp, ";\n");
	}
    }
    len = bp - tmpbuf;

    DPR3(("put_new_environ(len=%d):\t%s\n", len, tmpbuf));

    if (len > 0)
	write(1, tmpbuf, len);

    return NoError;
}


int	set_remote_env(char *ptr, char *env_pass)
{
    char	*bp = ptr, *ep;
    char	**ls, **ls2, **pp, **pp2;
    bool	dup_ent;

    ls = ls2 = 0;
    if (Conf.remote->passEnv
		&& (ls = parse_strlist(Conf.remote->passEnv, ' '))) {
	for (pp = ls; *pp; pp++)
	    if (ep = getenv(*pp)) {
		*bp++ = ' '; bp = strcpyx(bp, *pp); *bp++ = '=';
		*bp++ = '"'; bp = strcpyx(bp, ep); *bp++ = '"';
	    }
    }
    if (env_pass && (ls2 = parse_strlist(env_pass, ' '))) {
	for (pp2 = ls2; *pp2; pp2++) {
	    dup_ent = False;
	    if (ls) {		/* eliminate duplicate entries */
		for (pp = ls; *pp; pp++)
		    if (strcmp(*pp, *pp2) == 0) {
			dup_ent = True;
			break;
		    }
	    }
	    if (!dup_ent && (ep = getenv(*pp2))) {
		*bp++ = ' '; bp = strcpyx(bp, *pp2); *bp++ = '=';
		*bp++ = '"'; bp = strcpyx(bp, ep); *bp++ = '"';
	    }
	}
	FREE_LIST(ls2);
	ls2 = 0;
    }
    if (ls)	FREE_LIST(ls);
    /* if (bp != ptr)	*bp++ = ' '; */
    *bp = 0;

    DPR2(("set_remote_env('%s' & '%s'):\n\t'%s'\n",
					Conf.remote->passEnv, env_pass, ptr));
    return bp - ptr;
}

# ifdef	old_hpux
char	*xhp_xinput_name(char *locale)
{
    char	*xinput_name = "X@INPUT";
    char	**pp, *p;
    int		i, len;
    XhpLocale	*xhp = Conf.xhp;
    char	**ls;
    char	typ = 0;

    if (!xhp)		return NULL;
    if (!locale)	locale = userEnv.locale;

    for (i = 0; !typ && i < XHP_LANG_NUM; i++) {
	if (!(ls = parse_strlist(xhp[i].locales, ' ')))	continue;
	for (pp = ls; *pp; pp++) {
	    if (locale[0] != (*pp)[0])	continue;
	    len = strlen(p = *pp);
	    if ((p[len - 1] == '*' && strncmp(locale, p, len - 1) == 0)
		|| strcmp(locale, p) == 0) {
		typ = xhp[i].type;
		break;
	    }
	}
	FREE_LIST(ls);
    }
    if (!typ) {
	DPR2(("xhp_xinput_name(%s): unknown locale\n", locale));
	return NULL;
    }

    xinput_name[1] = typ;
    return NEWSTR(xinput_name);
}


char	*get_real_locale(char *locale, char ***aliases)
{
    int		i;
    int		match_idx = -1;
    LocaleAlias	**alias = Conf.alias, *ap;
    char	**ls, **pp;
    char	*real_locale = NULL;

    if (!locale || !alias)	return NULL;

    for (i = 0; match_idx < 0 && (ap = alias[i]); i++) {
	if (strcmp(ap->name, locale) == 0) {
	    match_idx = i;
	    if (aliases)
		ls = parse_strlist(ap->aliases, ' ');
	    break;
	}
	if (ls = parse_strlist(ap->aliases, ' ')) {
	    for (pp = ls; *pp; pp++)
		if (strcmp(locale, *pp) == 0) {
		    match_idx = i;
		    break;
		}
	    FREE_LIST(ls);
	    ls = (char **)NULL;
	}
    }

    if (match_idx >= 0) {
	real_locale = alias[match_idx]->name;
	if (aliases) {
	    *aliases = ls;
	    ls = (char **)NULL;
	}
    } else {
	real_locale = NULL;
	if (aliases)	*aliases = (char **) NULL;
    }
    if (ls)	FREE_LIST(ls);

    DPR(("get_real_locale(%s): real_locale=%s  aliases=%s\n",
		locale, real_locale, aliases ? *aliases : NULL));

    return real_locale;
}

# endif	/* old_hpux */
