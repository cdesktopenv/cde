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
/* @(#)$XConsortium: file.c /main/7 1996/05/07 13:59:24 drk $ */

#include	"xims.h"
#include	<fcntl.h>
#include	<sys/stat.h>

    /* local func */
static void put_selection_entry(FILE *fp, int ent_type, bool is_valid, char *val, char *val2);
static void	put_ims_conf(/* fp, sel */);
static void	put_select_mode(/* fp, select_mode */);
static void	put_selection_header(/* fp */);
static void	put_selection_sep(/* fp */);
static int	user_selection_fname(/* buf, buf_len, dpy_specific */);

static char	*SelectFileFormat = NULL;
static bool	cmdconf_initialized = False;


int	create_xims_dir(void)
{
    int		ret = NoError;
    char	path[MAXPATHLEN];

    /* if (!userEnv.homedir || !Conf.userImsDir)	return ErrNoHome; */
	   /*	get_user_environ() & read_cmd_conf() must be successfully */
	   /*	executed prior to calling this function */

	/* check directory */
    expand_string("%S", path, MAXPATHLEN, 0);
    if (!make_user_dir(path)) {
	setErrFile(path);
	expand_string("%U", path, MAXPATHLEN, 0);
	if (!make_user_dir(path))
	    return ErrDirCreate;
	expand_string("%S", path, MAXPATHLEN, 0);
	if (!make_user_dir(path))
	    return ErrDirCreate;
    }
#if	0
    expand_string("%T", path, MAXPATHLEN, 0);
    if (!make_user_dir(path)) {
	/* setErrFile(path); return ErrDirCreate; */	/* ignore */
    }
#endif

    if (OpMode == MODE_START) {
	ret = init_log_file(Opt.LogPath, True);
	if (ret != NoError && Opt.LogPath)	/* try default log file */
	    ret = init_log_file(NULL, True);
	if (ret != NoError)			/* try alternate log file */
	    ret = init_log_file(ALT_LOGPATH, True);
	DPR(("Opt.LogPath='%s'\n", Opt.LogPath));
    }

    return ret;
}

int	init_log_file(char *org_path, int check_size)
{
    char	path[MAXPATHLEN];
    bool	log_exists = False;
    int		fd;
    struct stat	stb;

	/* check log file */	/* if not writable, ims execution will abort */
    if (!org_path)	org_path = DEFAULT_LOGPATH;
    expand_string(org_path, path, MAXPATHLEN, 0);
    log_exists = stat(path, &stb) == 0;	/* access(path, F_OK) == 0; */

    if (log_exists) {
	if (access(path, W_OK) == -1
			|| stat(path, &stb) == -1 || !S_ISREG(stb.st_mode))
	    goto _err;

	    /* rename log file if its size is larger than MAX_LOGSIZE (20 KB) */
	if (check_size && stb.st_size > MAX_LOGSIZE) {
	    char	save_path[MAXPATHLEN], *bp;
	    bp = strcpyx(save_path, path); strcpyx(bp, ".old");
	    if (rename(path, save_path) == 0) {
	    	log_exists = False;
		DPR(("init_log_file(): %s renamed.\n", path));
	    } else {
		DPR(("init_log_file(): rename(to: %s) failed.\n", save_path));
	    }
	}
    }
    if (!log_exists) {
	if ((fd = creat(path, 0666)) == -1)
	    goto _err;
	(void) close(fd);
    }

    Opt.LogPath = NEWSTR(path);
    return NoError;

_err:
    if (Verbose > 1)	perror(path);
    setErrFile(path);
    return ErrFileCreate;
}


int	set_errorlog(char *path)
{
    int	fd;

    /* if (!path)	return; */
    (void) make_user_dir(dirname(path));
    fd = open(Opt.LogPath, O_WRONLY|O_CREAT|O_APPEND, 0666);
    if (fd >= 0) {
	if (dup2(fd, 2) >= 0) {
	    /* LogFp = stderr; */
	    return NoError;
	}
	close(fd);
    }

    if (Verbose > 1)	perror(path);
    setErrFile(path);
    return ErrFileCreate;
}


int	read_cmd_conf(void)
{
    char	buf[MAXPATHLEN];
    CmdConf	*conf = &Conf;
    char	*conf_dir, *path;
    char	*p, *lp, *valp;
    int		line_num, num_alias;
    DtEnv	*dt;
    RemoteEnv	*remote;
    FILE	*fp;
# ifdef	old_hpux
    VueEnv	*vue;
    LocaleAlias	*tmp_alias[MAXIMSENT], *ap;
# endif	/* old_hpux */

    conf_dir = DTIMS_CONFDIR;
    if (!(path = Opt.ConfPath) || !*path) {
	if (!(path = getenv("DTIMS_STARTCONF")) || !*path) {
	    if ((p = getenv("DTIMS_CONFDIR")) && *p)
		conf_dir = p;
	    snprintf(path = buf, sizeof(buf), "%s/%s", conf_dir, DTIMS_CONFFILE);
	}
    }
    DPR3(("read_cmd_conf(): path=%s\n", path));

    if ((fp = fopen(path, "r")) == NULL) {
	DPR3(("\tcannot open '%s'\n", path));
#ifdef	DEBUG2
	DPR(("cannot open '%s' -- continue with default config\n", path));
	goto _default;
#else
	setErrFile(path);
	return ErrFileOpen;
#endif
    }

    remote = conf->remote = ALLOC(1, RemoteEnv);
    remote->disabled = remote->useRemsh = False;
    remote->timeout = REMOTE_TIMEOUT;
    remote->passEnv = NULL;

    num_alias = 0;
    start_tag_line(path);
    while ((line_num = read_tag_line(fp, &lp, &valp)) > 0) {
	if (!valp) {
	    DPR3(("\t[line=%d] no value for '%s'\n", line_num, lp));
	    continue;
	}

	if (strncmp(lp, "Ims", 3) == 0) {
	    p = lp + 3;
	    if (strncmp(p, "ConfigDir", 4) == 0) {
		RENEWSTR(conf->imsConfDir, valp);
	    } else if (strncmp(p, "AppDefDir", 4) == 0) {
		RENEWSTR(conf->imsAppDir, valp);
	    } else if (strncmp(p, "LogDir", 4) == 0) {
		RENEWSTR(conf->imsLogDir, valp);
	    } else if (strncmp(p, "LogFile", 4) == 0) {
		RENEWSTR(conf->imsLogFile, valp);
	    } else if (strncmp(p, "DirName", 3) == 0) {
		RENEWSTR(conf->imsDir, valp);
	    } else
		goto _inv;
	} else if (strncmp(lp, "User", 4) == 0) {
	    p = lp + 4;
	    if (strncmp(p, "ImsDir", 4) == 0) {
		RENEWSTR(conf->userImsDir, valp);
	    } else if (strncmp(p, "TmpDir", 4) == 0) {
		RENEWSTR(conf->userTmpDir, valp);
	    } else if (strncmp(p, "AltTmpDir", 3) == 0) {
		RENEWSTR(conf->userAltDir, valp);
	    } else
		goto _inv;
	} else if (strncmp(lp, "Dt", 2) == 0) {
	    p = lp + 2;
	    if (!conf->dt)	dt = conf->dt = ALLOC(1, DtEnv);
	    if (strncmp(p, "ConfigDir", 4) == 0) {
		RENEWSTR(dt->confDir, valp);
	    } else if (strncmp(p, "UserDir", 5) == 0) {
		RENEWSTR(dt->userDir, valp);
	    } else if (strncmp(p, "ResourcePath", 4) == 0) {
		RENEWSTR(dt->resPath, valp);
	    } else
		goto _inv;
	} else if (strncmp(lp, "ImXmod.", 7) == 0) {
	    p = lp + 7;
	    if (strncmp(p, "XIM", 3) == 0) {
		RENEWSTR(conf->xmod[Proto_XIM], valp);
	    } else if (strncmp(p, "Ximp", 4) == 0) {
		RENEWSTR(conf->xmod[Proto_Ximp], valp);
	    } else if (strncmp(p, "Xsi", 3) == 0) {
		RENEWSTR(conf->xmod[Proto_Xsi], valp);
# ifdef	old_hpux
	    } else if (strncmp(p, "Xhp", 3) == 0) {
		RENEWSTR(conf->xmod[Proto_Xhp], valp);
# endif	/* old_hpux */
	    } else if (strncmp(p, "None", 3) == 0) {
		RENEWSTR(conf->xmod[Proto_None], valp);
	    } else
		goto _inv;
	} else if (strncmp(lp, "ImAtom.", 7) == 0) {
	    p = lp + 7;
	    if (strncmp(p, "XIM", 3) == 0) {
		RENEWSTR(conf->atom[Proto_XIM], valp);
	    } else if (strncmp(p, "Ximp", 4) == 0) {
		RENEWSTR(conf->atom[Proto_Ximp], valp);
	    } else if (strncmp(p, "Xsi", 3) == 0) {
		RENEWSTR(conf->atom[Proto_Xsi], valp);
# ifdef	old_hpux
	    } else if (strncmp(p, "Xhp", 3) == 0) {
		RENEWSTR(conf->atom[Proto_Xhp], valp);
# endif	/* old_hpux */
	    } else if (strncmp(p, "None", 3) == 0) {
		RENEWSTR(conf->atom[Proto_None], valp);
	    } else
		goto _inv;
	} else if (strncmp(lp, "Action.", 7) == 0) {
	    p = lp + 7;
	    if (strncmp(p, "GetRemoteConf", 8) == 0) {
		RENEWSTR(conf->action[ACT_GETREMCONF], valp);
	    } else if (strncmp(p, "RunRemoteIMs", 8) == 0) {
		RENEWSTR(conf->action[ACT_RUNREMIMS], valp);
	    } else
		goto _inv;
	} else if (strncmp(lp, "Remote", 6) == 0) {
	    p = lp + 6;
	    if (strncmp(p, "Disabled", 3) == 0) {
		remote->disabled = str_to_bool(valp, False);
	    } else if (strncmp(p, "UseRemsh", 4) == 0) {
		remote->useRemsh = str_to_bool(valp, False);
	    } else if (strncmp(p, "Timeout", 3) == 0) {
		int	n = 0;
		if (str_to_int(valp, &n) && n >= 0)
		    remote->timeout = n;
	    } else if (strncmp(p, "Environment", 3) == 0) {
		RENEWSTR(remote->passEnv, valp);
	    } else
		goto _inv;
# ifdef	old_hpux
	} else if (strncmp(lp, "Vue", 3) == 0) {
	    p = lp + 3;
	    if (!conf->vue)	vue = conf->vue = ALLOC(1, VueEnv);
	    if (strncmp(p, "ConfigDir", 4) == 0) {
		RENEWSTR(vue->confDir, valp);
	    } else if (strncmp(p, "UserDir", 5) == 0) {
		RENEWSTR(vue->userDir, valp);
	    } else if (strncmp(p, "UseLiteFile", 5) == 0) {
		RENEWSTR(vue->uselite, valp);
	    } else if (strncmp(p, "LiteResourcePath", 5) == 0) {
		RENEWSTR(vue->litePath, valp);
	    } else if (strncmp(p, "ResourcePath", 5) == 0) {
		RENEWSTR(vue->resPath, valp);
	    } else
		goto _inv;
	} else if (strncmp(lp, "LocaleAlias", 6) == 0) {
	    ap = ALLOC(1, LocaleAlias);
	    p = valp; cut_field(valp);
	    ap->name = NEWSTR(p);
	    ap->aliases = NEWSTR(valp);
	    tmp_alias[num_alias++] = ap;
	} else if (strncmp(lp, "XhpLocales", 3) == 0) {
	    int	idx = 0;
	    p = strrchr(lp, '.');
	    if (!p)	continue;	/* invalid */
	    switch (p[1]) {
		case 'J':	idx = XHP_JPN; break;
		case 'K':	idx = XHP_KOR; break;
		case 'C':	idx = XHP_CHS; break;
		case 'T':	idx = XHP_CHT; break;
		default:		goto _inv;
	    }
	    if (!conf->xhp)
		conf->xhp = ALLOC(XHP_LANG_NUM, XhpLocale);
	    else if (conf->xhp[idx].locales) {
		FREE(conf->xhp[idx].locales);
	    }
	    conf->xhp[idx].type = p[1];
	    conf->xhp[idx].locales = NEWSTR(valp);
# endif	/* old_hpux */
	} else {
	_inv:
	    DPR(("\t[line=%d] invalid entry '%s'\n", line_num, lp));
	}
    }
    fclose(fp);

# ifdef	old_hpux
    if (num_alias > 0) {
	conf->alias = ALLOC(num_alias + 1, LocaleAlias *);
	memcpy((void *)conf->alias, (void *)tmp_alias,
				num_alias*sizeof(LocaleAlias *));
	conf->alias[num_alias] = (LocaleAlias *)0;
    }
# endif	/* old_hpux */

    /* if (remote->disabled)	FREE(remote->passEnv); */

_default:
	/* set default value unless set */
    if (!conf->imsConfDir)	conf->imsConfDir = NEWSTR(conf_dir);
    if (!conf->imsAppDir)	conf->imsAppDir = NEWSTR(DTIMS_APPDIR);
    if (!conf->imsLogDir)	conf->imsLogDir = NEWSTR(DTIMS_LOGDIR);
    if (!conf->imsLogFile)	conf->imsLogFile = NEWSTR(DTIMS_LOGFILE);
    if (!conf->imsDir)		conf->imsDir = NEWSTR(DTIMS_IMSDIR);
    if (!conf->userImsDir)	conf->userImsDir = NEWSTR(DTIMS_USRIMSDIR);
    if (!conf->userTmpDir)	conf->userTmpDir = NEWSTR(DTIMS_USRTMPDIR);
    if (!conf->userAltDir)	conf->userAltDir = NEWSTR(DTIMS_USRALTDIR);
    if (!conf->dt)		conf->dt = ALLOC(1, DtEnv);
    if (!conf->dt->confDir)	conf->dt->confDir = NEWSTR(DT_CONFDIR);
    if (!conf->dt->userDir)	conf->dt->userDir = NEWSTR(DT_USERDIR);
# ifdef	old_hpux
    if (!conf->vue)		conf->vue = ALLOC(1, VueEnv);
    if (!conf->vue->confDir)	conf->vue->confDir = NEWSTR(VUE_CONFDIR);
    if (!conf->vue->userDir)	conf->vue->userDir = NEWSTR(VUE_USERDIR);
# endif	/* old_hpux */
    if (!conf->xmod[Proto_XIM])	conf->xmod[Proto_XIM] = NEWSTR(IM_XMOD_XIM);
    if (!conf->xmod[Proto_Ximp]) conf->xmod[Proto_Ximp] = NEWSTR(IM_XMOD_XIMP);
    if (!conf->xmod[Proto_Xsi])	conf->xmod[Proto_Xsi] = NEWSTR(IM_XMOD_XSI);
    if (!conf->atom[Proto_XIM])	conf->atom[Proto_XIM] = NEWSTR(IM_ATOM_XIM);
    if (!conf->atom[Proto_Ximp]) conf->atom[Proto_Ximp] = NEWSTR(IM_ATOM_XIMP);
    if (!conf->atom[Proto_Xsi])	conf->atom[Proto_Xsi] = NEWSTR(IM_ATOM_XSI);
	/* default value of {xmod,atom}[Proto_None/Xhp] is NULL */
    if (!conf->action[ACT_GETREMCONF])
		conf->action[ACT_GETREMCONF] = NEWSTR(NAME_ACT_GETREMCONF);
    if (!conf->action[ACT_RUNREMIMS])
		conf->action[ACT_RUNREMIMS] = NEWSTR(NAME_ACT_RUNREMIMS);

    cmdconf_initialized = True;
    return NoError;
}

int	expand_cmd_conf(void)
{
    char	**pp[20 + NUM_ACTIONS], *p, buf[BUFSIZ];
    int		i, j, n;
    CmdConf	*conf = &Conf;

#define	CHK_ADD(p)	if (strchr(p, '%')) pp[n++] = &(p);

#ifdef	DEBUG
    if (DebugLvl > 1)	{ pr_CmdConf(); }
#endif

    n = 0;
    CHK_ADD(conf->imsConfDir);
    CHK_ADD(conf->imsAppDir);
    CHK_ADD(conf->imsLogDir);
    CHK_ADD(conf->imsLogFile);
    CHK_ADD(conf->imsDir);
    CHK_ADD(conf->userImsDir)
    CHK_ADD(conf->userTmpDir)
    CHK_ADD(conf->userAltDir)
    for (j = 0; j < NUM_ACTIONS; j++)	CHK_ADD(conf->action[j])
    if (conf->dt) {
	CHK_ADD(conf->dt->confDir)
	CHK_ADD(conf->dt->userDir)
	CHK_ADD(conf->dt->resPath)
    }
# ifdef	old_hpux
    if (conf->vue) {
	CHK_ADD(conf->vue->confDir)
	CHK_ADD(conf->vue->userDir)
	CHK_ADD(conf->vue->uselite)
	CHK_ADD(conf->vue->resPath)
	CHK_ADD(conf->vue->litePath)
    }
# endif	/* old_hpux */
	/* xmod[] & atom[] must not be expanded, since no ims selected */
	/* remote->* should be expanded at preparation of remote exec */ 

#undef	CHK_ADD

    for (i = j = 0; i < n; i++) {
	p = *(pp[i]);
	expand_string(*(pp[i]), buf, BUFSIZ, 0);
#ifdef	DEBUG
	if (strcmp(p, buf))	j++;
#endif
	FREE(p);
	*(pp[i]) = NEWSTR(buf);
    }

    DPR(("expand_cmd_conf(): %d / %d entries modified\n", j, n));

    return n;
}


int	read_imsconf(ImsConf *conf, char *ims_name, char *ims_fname)
{
    int		ret = NoError;
    char	path[MAXPATHLEN];
    char	*lp, *valp, *p;
    int		len, n, line_num;
    FILE	*fp;

    if (!ims_fname)	ims_fname = ims_name;
    len = expand_string("%I/", path, MAXPATHLEN, 0);
    strcpyx(path + len, ims_fname);

    DPR3(("read_imsconf(%s): path=%s\n", ims_name, path));

    CLR(conf, ImsConf);
    if ((fp = fopen(path, "r")) == NULL) {
	DPR3(("\tcannot open '%s'\n", path));
	return ErrNoImsConf;
    }

    start_tag_line(path);
    while ((line_num = read_tag_line(fp, &lp, &valp)) > 0) {
	if (!valp) {
	    DPR3(("\t[line=%d] no value for '%s'\n", line_num, lp));
	    continue;
	}
	/* valp = trim_line(valp); */

	/* TYP_NAME */
	if (strcmp(lp, "name") == 0)
	    ;	/* conf->name = NEWSTR(valp); */
	else if (strcmp(lp, "server_name") == 0) {
	    if (p = strchr(valp, ',')) {	/* contain secondary names */
		conf->servername2 = NEWSTR(valp);	/* save full name */
		*p = 0;
	    }
	    conf->servername = NEWSTR(valp);	/* save primary name only */
	} else if (strcmp(lp, "class_name") == 0)
	    conf->classname = NEWSTR(valp);
	else if (strcmp(lp, "property") == 0)
	    conf->property = NEWSTR(valp);
	/* TYP_PATH */
	else if (strcmp(lp, "cmd_path") == 0)
	    conf->cmd_path = NEWSTR(valp);
	/* TYP_STRING */
	else if (strcmp(lp, "cmd_param") == 0)
	    conf->cmd_param = NEWSTR(valp);

	/* TYP_NUMERIC */
	else if (strcmp(lp, "chk_timeout") == 0) {
	    if (str_to_int(valp, &n))
		conf->timeout = n;
	} else if (strcmp(lp, "chk_interval") == 0) {
	    if (str_to_int(valp, &n))	
		conf->interval = n;
	}

	/* TYP_BOOL */
#define	SET_FLAG(f)	\
	(str_to_bool(valp, False) ? (conf->flags |= (f)) : (conf->flags &= ~(f)))

	else if (strcmp(lp, "no_server") == 0)
	    SET_FLAG(F_NO_SERVER);
	else if (strcmp(lp, "no_remote") == 0)
	    SET_FLAG(F_NO_REMOTE);
	else if (strcmp(lp, "no_option") == 0)
	    SET_FLAG(F_NO_OPTION);
# ifdef	old_hpux
	else if (strcmp(lp, "try_connect") == 0)
	    SET_FLAG(F_TRY_CONNECT);
# endif	/* old_hpux */
	else if (strcmp(lp, "has_window") == 0)
	    SET_FLAG(F_HAS_WINDOW);
#undef	SET_FLAG

	/* TYP_NAMELIST */
	else if (strcmp(lp, "env_set") == 0)
	    conf->env_set = NEWSTR(valp);
	else if (strcmp(lp, "env_unset") == 0)
	    conf->env_unset = NEWSTR(valp);
	else if (strcmp(lp, "env_pass") == 0)
	    conf->env_pass = NEWSTR(valp);
	else if (strcmp(lp, "protocols") == 0)
	    conf->protocols = parse_protolist(valp);
	else {
	    DPR3(("\t[line=%d] invalid entry '%s'\n", line_num, lp));
	}
    }
    fclose(fp);

    ret = check_ims_conf(conf, ims_name);
    if (ret != NoError)
	setErrFile(path);

    return ret;
}

int	check_ims_conf(ImsConf *ims, char *ims_name)
{
    int		ret = NoError;
    char	*missing_entry = NULL;

    if (strcmp(ims_name, NAME_NONE) == 0)	return NoError;

    if ((ims->cmd_path) && strcmp(ims->cmd_path, NAME_BUILTIN) == 0) {
	ims->flags |= (F_BUILTIN | F_NO_SERVER | F_NO_REMOTE | F_NO_OPTION);
	ims->flags &= ~F_TRY_CONNECT;
	FREE(ims->cmd_path);
    }

	/* check indispensable entry */
    if (!ims->cmd_path && !(ims->flags & F_BUILTIN)) {
	missing_entry = "cmd_path";
	DPR(("read_imsconf(%s): no '%s' entry\n", ims_name, missing_entry));
    }
    if (!ims->servername) {
	DPR(("read_imsconf(%s): no '%s' entry\n", ims_name, missing_entry));
	missing_entry = "servername";
    }
    if (!ims->protocols) {
	DPR(("read_imsconf(%s): no '%s' entry\n", ims_name, missing_entry));
	missing_entry = "protocols";
    }

    if (missing_entry) {
	setErrArg1(missing_entry);
	ret = ErrMissEntry;
    }

    return ret;
}

int	read_localeconf(ImsList *list, char *locale_name)
{
    char	path[MAXPATHLEN];
    char	*lp, *valp, *pp;
    char	*def_name;
    int		sel_mode;
    int		len, line_num, num_ent;
    ImsEnt	*ent, *etmp[MAXIMSENT];
    FILE	*fp;
    UserEnv	*uenv = &userEnv;

    len = expand_string("%I/", path, MAXPATHLEN, 0);
    if (!locale_name || !*locale_name) {
	locale_name = uenv->real_locale ? uenv->real_locale : uenv->locale;
    }
    strcpyx(path + len, locale_name);

    DPR3(("read_localeconf(%s): path=%s\n", locale_name, path));

    CLR(list, ImsList);
    list->status = NoError;
    list->default_idx = -1;
    list->def_selmode = SEL_MODE_NOAUTO;

    if ((fp = fopen(path, "r")) == NULL) {
	DPR3(("\tcannot open '%s'\n", path));
	list->status = ErrNoLocaleConf;
	return ErrNoLocaleConf;
    }

    def_name = NULL;
    sel_mode = SEL_MODE_NONE;
    num_ent = 0;
    start_tag_line(path);
    while ((line_num = read_tag_line(fp, &lp, &valp)) > 0) {

	if (lp[0] == STR_PREFIX_CHAR) {
	    if (!valp) {
		DPR3(("\t[line=%d] no value for '%s'\n", line_num, lp));
	    	continue;
	    }
	    if (strncmp(lp + 1, STR_DEFAULTIMS, 3) == 0) {
		RENEWSTR(def_name, valp);
	    } else if (strncmp(lp + 1, STR_SELECTMODE, 3) == 0) {
		int	m = SEL_MODE_NONE;
		if (str_to_int(valp, &m) && m >= 0)
		    sel_mode = m;
	    } else {
		DPR2(("\t[line=%d] invalid entry '%s'\n", line_num, lp));
	    }
	} else {	/* ims name entry */
	    if (num_ent >= MAXIMSENT) {
		DPR(("\ttoo many IMS defined: '%s' ignored\n", lp));
		break;
	    }
	    ent = ALLOC(1, ImsEnt);
	    if (pp = strchr(lp, STR_PREFIX_CHAR)) {
		*pp++ = 0;
		if (*pp)
		    ent->fname = NEWSTR(pp);
		else {
		    DPR(("\tempty file name for '%s' -- ignored\n", lp));
		}
	    }
	    ent->name = NEWSTR(lp);
	    if (valp)	ent->label = NEWSTR(valp);
	    etmp[num_ent++] = ent;
	}
    }
    fclose(fp);

    if (num_ent) {
	list->num_ent = num_ent;
	list->elist = ALLOC(num_ent, ImsEnt *);
	memcpy((void *)list->elist, (void *)etmp, num_ent*sizeof(ImsEnt *));

	list->def_selmode =
		(sel_mode != SEL_MODE_NONE) ? sel_mode : SEL_MODE_NOAUTO;

	if (def_name) {
	    int	i;
	    for (i = 0; i < list->num_ent; i++)
		if (strcmp(def_name, list->elist[i]->name) == 0) {
		    list->default_idx = i;
		    break;
	    }
	    FREE(def_name);
	}
    }

#ifdef	DEBUG
    if (DebugLvl > 2)	pr_ImsList(list);
#endif

    return list->status;
}

int	read_user_selection(FileSel **fselp, char *locale_name)
{
    char	path[MAXPATHLEN];
    int		ret;
    int		dpy_specific;
    FILE	*fp;
    FileSel	*fsel;

    dpy_specific = user_selection_fname(path, MAXPATHLEN, -1);

    DPR3(("read_user_selection(): path=%s\n", path));

    if ((fp = fopen(path, "r")) == NULL) {
	DPR3(("\tcannot open '%s'\n", path));
	/* *fselp = (FileSel *) 0; */
	return ErrNoSelectionFile;
    }

    fsel = ALLOC(1, FileSel);

    start_tag_line(path);
    ret = read_selection_file(fsel, fp);
    fclose(fp);

    fsel->dpy_specific = dpy_specific;
    fsel->real_fname = NEWSTR(locale_name);
    *fselp = fsel;

    return NoError;
}

int	read_selection_file(FileSel *fsel, FILE *fp)
{
    char	*lp, *valp, *vp, *p;
    int		i, nopts, line_num;
    int		select_mode, iconic;
    char	*imsname, *hostname, *com_opt;
    ImsOpt	*opts[MAXIMSENT], *op;

    select_mode = SEL_MODE_NONE;
    imsname = hostname = com_opt = NULL;
    iconic = -1;
    nopts = 0;
    opts[0] = (ImsOpt *)NULL;

    while ((line_num = read_tag_line(fp, &lp, &valp)) > 0) {
	if (!valp) {
	    DPR3(("\t[line=%d] no value for '%s'\n", line_num, lp));
	    continue;
	}
	if (lp[0] != STR_PREFIX_CHAR) {
	    DPR3(("\t[line=%d] invalid name '%s'\n", line_num, lp));
	    continue;
	}
	if (strncmp(lp + 1, STR_SELECTMODE, 3) == 0) {
	    if (str_to_int(valp, &i) && i >= 0)
		select_mode = i;
	} else if (strncmp(lp + 1, STR_IMSNAME, 4) == 0) {
	    vp = valp; cut_field(valp);
	    RENEWSTR(imsname, vp);
	} else if (strncmp(lp + 1, STR_HOSTNAME, 4) == 0) {
	    vp = valp; cut_field(valp);
	    FREE(hostname);
	    if (strcmp(vp, NAME_LOCAL))
		hostname = NEWSTR(vp);
	} else if (strncmp(lp + 1, STR_ICONIC, 3) == 0) {
	    if (*valp)
		iconic = str_to_bool(valp, False);
	} else if (strncmp(lp + 1, STR_IMSOPTION, 4) == 0) {
	    if (p = strchr(lp + 1, STR_PREFIX_CHAR)) {	/* indiv. opt */
		if (nopts >= MAXIMSENT) {
		    DPR(("\t[line=%d] too many options - '%s' ignored\n",
								line_num, lp));
		    continue;
		}
		if (!*(++p)) {
		    DPR(("\t[line=%d] no ims name - '%s' ignored\n",
								line_num, lp));
		    continue;
		}
		for (op = 0, i = 0; i < nopts; i++)
		    if (strcmp(p, opts[i]->ims_name) == 0) {
			op = opts[i];
			FREE(op->ims_name);
			FREE(op->opt_str);
			break;
		    }
		if (!op) {
		    op = ALLOC(1, ImsOpt);
		    opts[nopts++] = op;
		}
		op->ims_name = NEWSTR(p);
		op->opt_str = NEWSTR(valp);
	    } else {					/* common opt */
		RENEWSTR(com_opt, valp);
	    }
	} else {
	    DPR3(("\t[line=%d] unknown name '%s'\n", line_num, lp));
	}
    }

    fsel->name = imsname;
    fsel->hostname = hostname;
    fsel->com_opt = com_opt;
    fsel->select_mode = select_mode;
    fsel->iconic = iconic;

    if (nopts > 0) {
	fsel->opts = ALLOC(nopts + 1, ImsOpt *);
	COPY(fsel->opts, opts, nopts, ImsOpt *);
	fsel->opts[nopts] = (ImsOpt *)0;
    }

    return NoError;
}

int	save_user_selection(UserSelection *sel, char *locale_name)
{
    char	path[MAXPATHLEN];
    int		dpy_specific;
    FILE	*fp;
    FileSel	*fsel = sel->fsel;

    dpy_specific = user_selection_fname(path, MAXPATHLEN, -1);

    if ((fp = fopen(path, "w")) == NULL) {
	DPR(("\tcannot create '%s'\n", path));
	setErrFile(path);
	return ErrFileCreate;
    }

    put_selection_header(fp);
    if (fsel)
	put_select_mode(fp, fsel->select_mode);
    put_ims_conf(fp, sel);
    put_selection_sep(fp);
    fclose(fp);

    DPR3(("save_user_selection(): '%s' saved on '%s'\n", sel->name, path));

    return NoError;
}

#define	_SELECTMODE	0
#define	_IMSNAME	1
#define	_HOSTNAME	2
#define	_ICONIC		3
#define	_IMSOPTION	4
#define	_IMSOPT2	5

static void put_selection_entry(FILE *fp, int ent_type, bool is_valid, char *val, char *val2)
{
    char	*name = NULL;

    switch (ent_type) {
	case _SELECTMODE:	name = STR_SELECTMODE; break;
	case _IMSNAME:		name = STR_IMSNAME; break;
	case _HOSTNAME:		name = STR_HOSTNAME; break;
	case _ICONIC:		name = STR_ICONIC; break;
	case _IMSOPT2:
	case _IMSOPTION:	name = STR_IMSOPTION; break;
	/* default:		is_valid = False; */
    }

    if (!is_valid)	putc(COMMENT_CHAR, fp);
    if (ent_type == _IMSOPT2)
	fprintf(fp, "%c%s%c%s%c\t%s\n", STR_PREFIX_CHAR, name,
				STR_PREFIX_CHAR, val2, TAG_END_CHAR, val);
    else
	fprintf(fp, "%c%s%c\t%s\n", STR_PREFIX_CHAR, name, TAG_END_CHAR, val);

}

static void	put_ims_conf(FILE *fp, UserSelection *sel)
{
    char	*valp, val[20];

    if ((valp = sel->name) && *valp)
	put_selection_entry(fp, _IMSNAME, True, valp, NULL);
    else {
	DPR(("put_ims_conf(): no ims name\n"));
	if (sel->fsel && (valp = sel->fsel->name) && *valp)
	    put_selection_entry(fp, _IMSNAME, False, valp, NULL);
    }
    if ((valp = sel->hostname) && *valp)
	put_selection_entry(fp, _HOSTNAME, True, valp, NULL);
    else if (sel->fsel && (valp = sel->fsel->hostname) && *valp)
	put_selection_entry(fp, _HOSTNAME, False, valp, NULL);

    if (sel->iconic != -1) {
	sprintf(val, "%ld", (long)sel->iconic);
	put_selection_entry(fp, _ICONIC, True, val, NULL);
    }

    if (sel->fsel && (valp = sel->fsel->com_opt) && *valp) {
	bool	opt_valid = True;
#if	0
	opt_valid = !sel->fsel->name || strcmp(sel->name, sel->fsel->name) == 0;
#endif
	put_selection_entry(fp, _IMSOPTION, opt_valid, valp, NULL);
    }

    if (sel->fsel && sel->fsel->opts) {
	ImsOpt	**op;
	for (op = sel->fsel->opts; *op; op++)
	    put_selection_entry(fp, _IMSOPT2, True,
					(*op)->opt_str, (*op)->ims_name);
    }
}

static void	put_select_mode(FILE *fp, int select_mode)
{
    char val[20];
    if (select_mode != SEL_MODE_NOAUTO && select_mode != SEL_MODE_AUTO
#ifdef	SelectMode_ONCE
	    && select_mode != SEL_MODE_ONCE
#endif	/* SelectMode_ONCE */
	)
	select_mode = SEL_MODE_NOAUTO;

    sprintf(val, "%ld", (long)select_mode);
    put_selection_entry(fp, _SELECTMODE, True, val, NULL);

}

static void	put_selection_header(FILE *fp)
{
    fprintf(fp, "%s %s\n", COMMENT_CHARS, ProgramRevision);
    if (SelectFileFormat)
	fprintf(fp, "%s%s\n", COMMENT_CHARS, SelectFileFormat);
}

static void	put_selection_sep(FILE *fp)
{
    fprintf(fp, "%s\n", COMMENT_CHARS);
}


int	get_select_mode(void)
{
    char	path[MAXPATHLEN];
    char	*lp, *valp;
    int		select_mode, dpy_specific;
    int		n, line_num;
    FILE	*fp;

    select_mode = SEL_MODE_NONE;
    dpy_specific = user_selection_fname(path, MAXPATHLEN, -1);

    DPR3(("get_select_mode(): path=%s\n", path));

    if ((fp = fopen(path, "r")) == NULL) {
	DPR3(("\tcannot open '%s'\n", path));
	return select_mode;
    }
    start_tag_line(path);

    while ((line_num = read_tag_line(fp, &lp, &valp)) > 0) {
	if (lp[0] == STR_PREFIX_CHAR
			&& strncmp(lp + 1, STR_SELECTMODE, 3) == 0) {
	    if (str_to_int(valp, &n))
		select_mode = n;
	}
    }
    fclose(fp);

    DPR3(("get_select_mode(%s): select_mode=%d\n",
				dpy_specific ? "dpy" : NULL, select_mode));

    return select_mode;
}

int	set_select_mode(int cur_mode, int new_mode)
{
    char	path[MAXPATHLEN];
    int		dpy_specific;
    FILE	*fp;

    switch (new_mode) {
	case SEL_MODE_NOAUTO:	break;
	case SEL_MODE_AUTO:	break;
#ifdef	SelectMode_ONCE
	case SEL_MODE_ONCE:	break;
#endif	/* SelectMode_ONCE */
	case SEL_MODE_NONE:
	default:		new_mode = SEL_MODE_NOAUTO; break;
    }

    dpy_specific = user_selection_fname(path, MAXPATHLEN, -1);

    DPR3(("set_selection_mode(): path=%s\n", path));

#if	0
    if (!make_user_dir(dirname(path))) {
	setErrFile(dirname(path));
	return ErrDirCreate;
    }
#endif

    if ((fp = fopen(path, "r+")) == NULL) {
	DPR3(("\tcannot open '%s'\n", path));
	if ((fp = fopen(path, "w")) == NULL) {
	    setErrFile(path);
	    return ErrFileCreate;
	}
	cur_mode = SEL_MODE_NONE;
    }

    if (cur_mode == SEL_MODE_NONE) {	/* append '@SelectMode' line */
	fseek(fp, 0, SEEK_END);
	put_select_mode(fp, new_mode);
	fclose(fp);
    } else {
	FILE	*new_fp;
	char	new_fname[MAXPATHLEN];
	char	line_buf[BUFSIZ];
	char	*lp, *valp;
	int	n, line_num, mode_line;

	sprintf(new_fname, "%s,tmp", path);
	if (!(new_fp = fopen(new_fname, "w"))) {
	    fclose(fp);
	    DPR(("set_select_mode(): cannot create %s\n", new_fname));
	    setErrFile(path);
	    return ErrFileCreate;
	}

	line_num = mode_line = 0;
	while (fgets(lp = line_buf, BUFSIZ, fp)) {
	    line_num++;
	    skip_white(lp);
	    if (*lp == STR_PREFIX_CHAR) {
		if (strncmp(lp + 1, STR_SELECTMODE, 3) == 0) {
		    if (mode_line)	continue;	/* ignore this line */
		    if (valp = strchr(lp, TAG_END_CHAR)) {
			valp++;
			skip_white(valp);
			if (str_to_int(valp, &n) && n == new_mode)
			    mode_line = -1;
		    }
		    put_select_mode(new_fp, new_mode);
		    mode_line = line_num;
		} else
		    fputs(line_buf, new_fp);
	    } else
		fputs(line_buf, new_fp);
	}
	if (!mode_line)
	    put_select_mode(new_fp, new_mode);

	fclose(new_fp);
	fclose(fp);
	if (mode_line == -1) {		/* not changed */
	    (void) unlink(new_fname);
	} else {
	    if (rename(new_fname, path) == -1) {
		setErrFile(path);
		return ErrFileCreate;
	    }
	}
	(void) unlink(new_fname);
    }

    DPR2(("set_select_mode(%s): new_mode=%d <- %d\n",
			dpy_specific ? "dpy" : NULL, new_mode, cur_mode));

    return NoError;
}

static int	user_selection_fname(char *buf, int buf_len, int dpy_specific)
{
    int		len;
    UserEnv	*uenv = &userEnv;
    static bool	real_done = False;

    if (Opt.UserPath) {
	strncpy(buf, Opt.UserPath, buf_len);
	buf[buf_len-1] = 0;
	dpy_specific = 0;
    } else {
	if (dpy_specific == -1) {
	    len = expand_string("%S/%d", buf, buf_len, 0);
	    dpy_specific = is_directory(buf, False) ? 1 : 0;
	}
	len = expand_string(dpy_specific ? "%S/%d/" : "%S/", buf, buf_len, 0);

# ifdef	old_hpux
	if (!real_done && uenv->real_locale) {
	    real_done = True;
	    strcpy(buf + len, uenv->real_locale);
	    if (!is_readable(buf, True)) {
		bool	rename_done = False;
		char	buf2[MAXPATHLEN], *bp;

		strncpy(buf2, buf, len);
		bp = buf2 + len; *bp = 0;
		if (strcmp(uenv->locale, uenv->real_locale)) {
		    strcpy(bp, uenv->locale);
		    if (is_readable(buf2, False)) {
			rename_done = rename(buf2, buf) == 0;
			DPR(("user_selection_fname(): rename(%s, %s) %s\n",
					uenv->locale, uenv->real_locale,
					rename_done ? "OK" : "Failed"));
		    }
		}

		if (uenv->locale_aliases) {
		    char	**ap;
		    for (ap = uenv->locale_aliases; *ap; ap++) {
			strcpy(bp, *ap);
			if (!is_readable(buf2, False))	continue;
			if (rename_done) {
			    (void) unlink(buf2);
			    DPR(("user_selection_fname(): unlink(%s) %s\n",
									*ap));
			} else {
			    rename_done = rename(buf2, buf) == 0;
			    DPR(("user_selection_fname(): rename(%s, %s) %s\n",
				*ap, uenv->real_locale,
						rename_done ? "OK" : "Failed"));
			}
		    }
		}
		/* real_done = rename_done; */
	    }
	}
# endif	/* old_hpux */

	/* Add the CDE-generic locale name */
	strcpy(buf + len, real_done ? uenv->real_locale : uenv->CDE_locale);
	buf[buf_len-1] = 0;
    }

    return dpy_specific;
}


int	parse_protolist(char *valp)
{
    int		proto_bits = 0;

    if (strstr(valp, "XIM"))	proto_bits |= ProtoBit(Proto_XIM);
    if (strstr(valp, "Ximp"))	proto_bits |= ProtoBit(Proto_Ximp);
    if (strstr(valp, "Xsi"))	proto_bits |= ProtoBit(Proto_Xsi);
# ifdef	old_hpux
    if (strstr(valp, "Xhp"))	proto_bits |= ProtoBit(Proto_Xhp);
# endif	/* old_hpux */
    if (strstr(valp, "None"))	proto_bits |= ProtoBit(Proto_None);
    return proto_bits;
}

int	default_protocol(ImsConf *conf)
{
    if (conf->protocols & ProtoBit(Proto_XIM))		return Proto_XIM;
    else if (conf->protocols & ProtoBit(Proto_Ximp))	return Proto_Ximp;
    else if (conf->protocols & ProtoBit(Proto_Xsi))	return Proto_Xsi;
# ifdef	old_hpux
    else if (conf->protocols & ProtoBit(Proto_Xhp))	return Proto_Xhp;
# endif	/* old_hpux */
    else						return Proto_None;
}

