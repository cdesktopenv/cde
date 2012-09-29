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
/* @(#)$TOG: start.c /main/9 1998/04/06 13:36:49 mgreess $ */

#include	"xims.h"
#include	<signal.h>
#include	<sys/wait.h>
#include	<setjmp.h>
#include	<unistd.h>

#if (defined(USL) || defined(__uxp__) || defined(linux) || defined(CSRG_BASED)) && !defined(_NFILE)
#define _NFILE FOPEN_MAX
#endif

#if !defined(linux) && !defined(CSRG_BASED)
extern char	*sys_errlist[];
extern int	sys_nerr;
#endif

    /* local functions */
static int	check_ims_opt(/* ptr */);
static char	*find_session_resfile(/* res_type */);
static int	check_selection(/* sel */);
static int	build_run_env(/* sel */);
static int	run_ims(/* sel */);
static int	invoke_ims(/* sel */);
static void	on_sig_chld(/* sig */);
static bool	is_ims_running(/* renv, ims */);
static int	settle_ims(/* sel */);
static Window	property_owner(/* prop_atom, prop_str */);
# ifdef	old_hpux
static void	catch_alarm(/* sig */);
static int	try_connection(/* sel */);
static int	create_dummy_ic(/* xim */);
# endif	/* old_hpux */


void	ximsStart()
{
    int		ret;
    UserSelection	*sel = &userSel;
    OpStateVal	oldOpState = OpState;

    DPR(("ximsStart(): OpState=%s  OpErrCode=%s[%d]\n",
			StateName(), error_name(OpErrCode), OpErrCode));

    OpState = State_Start;

#ifdef	DEBUG
    if (DebugLvl > 1)	pr_UserSelection(sel);
#endif

    ret = NoError;
    if (oldOpState == State_Select_Err) {
	/* don't change OpErrCode */
	OpState = State_Start_Err;
	return;
    }
    if (oldOpState == State_Select_Canceled) {
	clear_UserSelection(sel);
	ret = ErrNoSelection;
    } else {					/* save selection */
	if (!(OpFlag & FLAG_NOSAVE) && (sel->flag & F_SELECT_CHANGED)) {
	    if (save_user_selection(sel, NULL) != NoError) {
		DPR(("save_user_selection(): failed\n"));
		put_xims_warnmsg(ErrSaveSelection, 0, 0, 0);
		/* ret = ErrSaveSelection; */
	    }
	}
    }

    if (ret != NoError) {
	OpErrCode = ret;
	OpState = State_Start_Err;
	return;
    }

    if ((ret = check_selection(sel)) != NoError) {
	if (ret == ErrIsNone || ret == ErrNotRun) {
	    build_run_env(sel);		/* for make_new_environ() */
	}
	OpErrCode = ret;
	OpState = State_Start_Done;
	return;
    }

    build_run_env(sel);

    if (useWINDOW())		/* initilaize Xt */
	init_window_env();

    ret = run_ims(sel);

    OpErrCode = ret;
    OpState = ret == NoError ? State_Start_Done : State_Start_Err;

    return;
}


void	ximsWait()
{
    OpStateVal	oldOpState = OpState;
    UserSelection	*sel = &userSel;
    struct timeval	interval;
    time_t	start_tm = 0;
    int		lapse;

    DPR(("ximsWait(): OpState=%s  OpErrCode=%s[%d]\n",
				StateName(), error_name(OpErrCode), OpErrCode));

    OpState = State_Wait;

    if (oldOpState == State_Start_Err) {
	/* don't change OpErrCode */
	OpState = State_Wait_Err;
	return;
    }

    if (!is_waiting() || (OpFlag & FLAG_NOWAIT)) {
	ximsWaitDone();
    }

    if (im_mod_available(sel->renv) != 1) {

	if (useWINDOW()) {
	    xt_start_waiting();	/* never returns unless failed */
	}

	    /* waiting */
	lapse = 0;
	interval.tv_sec = Opt.Interval / 1000;
	interval.tv_usec = (Opt.Interval % 1000) * 1000;
	start_tm = time((time_t) 0);

	while (is_waiting()) {
	    select(0, 0, 0, 0, &interval);		/* usleep */
	    lapse = (int) time((time_t) 0) - start_tm;

	    if (im_mod_available(sel->renv) != 0 || lapse >= Opt.Timeout) {
		DPR(("ximsWait(tmout=%d): wait done (%d sec.)\n",
							Opt.Timeout, lapse));
		break;
	    }
	}
    }

    ximsWaitDone();
}


void	ximsWaitDone()
{
    int		ret;
    UserSelection	*sel = &userSel;

    DPR(("ximsWaitDone():\tOpState=%s  OpErrCode=%s[%d]\n",
				StateName(), error_name(OpErrCode), OpErrCode));

    set_sig_chld(False);

    ret = sel->renv->status;
    switch (ret) {
	case ErrImsWaiting:
		sel->status = ErrImsTimeout;
		put_xims_log("'%s' timed-out.", sel->name, 0, 0);
		break;

	case ErrImsWaitDone:
		sel->status = NoError;
# ifdef	old_hpux
		if ((OpFlag & FLAG_CONNECT)
				|| (sel->ent->ims->flags & F_TRY_CONNECT)) {
		    sel->status = try_connection(sel);
		}
# endif	/* old_hpux */
		break;

	case ErrImsConnecting:
	case ErrImsConnectDone:
		sel->status = NoError;
		break;

	case ErrImsAborted:
		/* put_xims_log("'%s' aborted.", sel->name, 0, 0); */
	case ErrImsExecution:
	default:
		sel->status = ret;
		break;
    }

    if (sel->status != NoError) {
	OpErrCode = sel->status;
	DPR(("ximsWaitDone(): OpErrCode=%s[%d]\n",
					error_name(OpErrCode), OpErrCode));
    }

    restore_resources();

    settle_ims(sel);		/* clear WM_COMMAND property */

    OpState = OpErrCode == NoError ? State_Wait_Done : State_Wait_Err;

    ximsMain();
}

int	is_waiting()
{
    return userSel.renv && userSel.renv->status == ErrImsWaiting;
}

void	set_sig_chld(enable)
    int		enable;
{
    DPR(("set_sig_chld(%s)\n", enable ? "Enabled" : "Disabled"));
    signal(SIGCHLD, enable ? on_sig_chld : SIG_IGN);
}

int	im_mod_available(renv)
    RunEnv	*renv;
{
    Window	owner;

    if (!renv) {
	if (!(renv = userSel.renv))
	    return -1;
    }
    if (renv->status != ErrImsWaiting)
	return -1;

#ifdef	DEBUG
    if (DebugLvl >= 1)	putc('.', LogFp), fflush(LogFp);
#endif

    owner = property_owner(&renv->prop_atom, renv->atom_name);

    if (owner != None) {

#ifdef	ReconfirmProperty
	{
	    int	wait_period = 2;	/* 2 sec. */

	    DPR(("im_mod_available(): [RECONFIRM] sleep(%d)\n", wait_period));
	    sleep(wait_period);
	    if (property_owner(&renv->prop_atom, renv->atom_name) == None) {
		DPR(("\t[RECONFIRM] owner lost\n"));
		return 0;
	    }
	}
#endif	/* ReconfirmProperty */

#ifdef	DEBUG
	if (DebugLvl >= 1)	putc('\n', LogFp), fflush(LogFp);
#endif
	DPR2(("check_im_mod(): wait done\n"));
	renv->status = ErrImsWaitDone;
	return 1;
    }

    return 0;
}


	/* ***** IMS options ***** */

int	mk_ims_option(ptr, sel)
    char	*ptr;
    UserSelection	*sel;
{
    char	*bp = ptr;
    FileSel	*fsel = sel->fsel;
    ImsConf	*ims = sel->ent->ims;

    *bp = 0;
    if (ims->flags & F_NO_OPTION)	/* not applicable */
	return 0;

    if (fsel) {
	if (fsel->com_opt	/* common option */
			&& check_ims_opt(fsel->com_opt)) {
	     /* bp = strcpyx(bp, fsel->com_opt); */
	    bp += expand_string(fsel->com_opt, bp, BUFSIZ, ims);
	    *bp++ = ' ';
	}
	if (fsel->opts) {		/* individual option */
	    ImsOpt	**op;
	    for (op = fsel->opts; *op; op++) {
		if (strcmp((*op)->ims_name, sel->name) == 0
			&& check_ims_opt((*op)->opt_str)) {
		    /* bp = strcpyx(bp, (*op)->opt_str); */
		    bp += expand_string((*op)->opt_str, bp, BUFSIZ, ims);
		    *bp++ = ' ';
		}
	    }
	}
    }
    if (Opt.ImsOption && check_ims_opt(Opt.ImsOption)) {
	/* bp = strcpyx(bp, Opt.ImsOption); */
	bp += expand_string(Opt.ImsOption, bp, BUFSIZ, ims);
    }
    if (sel->iconic > 0 && strstr(ptr, STR_ICONIC_OPT) == NULL) {
	bp = strcpyx(bp, STR_ICONIC_OPT);
    }
    *bp = 0;

    return (int) (bp - ptr);
}

static int	check_ims_opt(ptr)
    char	*ptr;
{
	/* option string must not contain any of shell's metacaharacters */
    if (strpbrk(ptr, "`'\"#;&()|<>\n")) {
	put_xims_log("ims option ignored: %s", ptr, 0, 0);
	DPR(("\tshell's meta-char in option \"%s\" -- ignored\n", ptr));
	return False;
    }
    return True;
}


	/* ********  resource  ******** */

#define	RES_TYPE_DT		0
#define	RES_TYPE_VUE		1

static bool	resource_loaded = False;

int	load_resources()
{
    int		ret;
    int		empty;
    char	*sess_res, *res_file;

    if (resource_loaded) {
	DPR2(("load_resources: already done -- not loaded\n"));
	return False;
    }

    empty = !save_RM();
    if (!empty) {		/* load if RESOURCE_MANAGER is empty */
	DPR2(("load_resources: RESOURCE_MANGER is not empty -- not loaded\n"));
	return False;
    }

    sess_res = res_file = NULL;
    if (!(OpFlag & FLAG_NORESOURCE)) {
	if (isDT()) {
	    sess_res = find_session_resfile(RES_TYPE_DT);
	}
# ifdef	old_hpux
	else if (isVUE()) {
	    sess_res = find_session_resfile(RES_TYPE_VUE);
	}
# endif	/* old_hpux */
	if (sess_res && !is_readable(sess_res, False)) {
	    FREE(sess_res);
	    sess_res = NULL;
	}
    }

    if (Opt.ResourceFile && is_readable(Opt.ResourceFile, False))
	res_file = Opt.ResourceFile;

    DPR(("load_resources():\tsess='%s'  res='%s'\n", sess_res, res_file));

    if (!isDT() && !sess_res && !res_file)	return False;
    if ((ret = open_display()) != NoError)	return False;

    ret = merge_RM(sess_res, res_file);
    resource_loaded = True;

    if (sess_res)	FREE(sess_res);

    return ret;
}

int	restore_resources()
{
    if (!resource_loaded) {
	DPR2(("restore_resources: not loaded yet -- not restored\n"));
	return False;
    }
    resource_loaded = False;
    return restore_RM();
}

static char	*find_session_resfile(res_type)
    int		res_type;
{
    char	path[MAXPATHLEN];
    char	**ls = (char **) 0, **pp;
    char	*res = NULL;
    bool	found = False;

    if (res_type == RES_TYPE_DT) {	/* DT */
	res = Conf.dt ? (Conf.dt)->resPath : NULL;
    }
# ifdef	old_hpux
    else if (res_type == RES_TYPE_VUE && Conf.vue) {	/* VUE */
	VueEnv	*vue = Conf.vue;
	res = vue->resPath;
	if (vue->uselite) {
	    expand_string(vue->uselite, path, MAXPATHLEN, (ImsConf *)0);
	    if (access(path, R_OK) == 0)
		res = vue->litePath;
	}
    }
# endif	/* old_hpux */

    if (!res)	return NULL;
    if (ls = parse_strlist(res, ':')) {
	for (pp = ls; *pp; pp++) {
	    expand_string(*pp, path, MAXPATHLEN, (ImsConf *)0);
	    if (access(path, R_OK) == 0) {
		found = True;
		break;
	    }
	}
	FREE_LIST(ls);
    }
    /* DPR2(("find_session_resfile(): '%s'\n", path)); */

    if (found)
	return NEWSTR(path);
    return NULL;
}


	/* ***** local functions ***** */

static int	check_selection(sel)
    UserSelection	*sel;
{
    int		ret = NoError;

    if (!sel->name || !sel->list)
	ret = ErrNoSelection;
    else if (sel->ims_idx < 0 || sel->ims_idx >= sel->list->num_ent)
	ret = ErrNoSelection;
    else if (sel->status != NoError)
	ret = sel->status;
    else if (strcmp(sel->name, NAME_NONE) == 0)
	ret = sel->status = ErrIsNone;
    else if (sel->ent->ims->flags & F_NO_SERVER)
	ret = sel->status = ErrNotRun;
    else if (OpFlag & FLAG_NOSTART)
	ret = sel->status = ErrNotRun;

    return ret;
}

static int	build_run_env(sel)
    UserSelection	*sel;
{
    char	*p;
    int		proto;
    int		len;
    int		tmout, intv;
    char	*log_path;
    char	*bp;
    char	envbuf[BUFSIZ], optbuf[BUFSIZ];
    RunEnv	*renv;
    ImsConf	*ims = sel->ent->ims;
    char        *cmd_param = ims->cmd_param ? ims->cmd_param : "";

    renv = ALLOC(1, RunEnv);

    renv->is_remote = sel->host_type == HOST_REMOTE ? True : False;

	/* proto, im_mod  & atom */
    proto = renv->proto = default_protocol(ims);
    if (p = Conf.xmod[proto]) {
	char	buf[BUFSIZ];
	expand_string(p, buf, BUFSIZ, ims);
	renv->im_mod = NEWSTR(buf);
    }
    if (p = Conf.atom[proto]) {
	char	buf[BUFSIZ];
	expand_string(p, buf, BUFSIZ, ims);
	renv->atom_name = NEWSTR(buf);
    } else {		/* copy im_mod, instead */
	renv->atom_name = NEWSTR(renv->im_mod);
# ifdef	old_hpux
	if ((p = renv->atom_name) && strchr(p, '#')) {
	    while (p = strchr(p, '#'))		/* replace '#' with '@' */
		*p++ = '@';
	}
# endif	/* old_hpux */
    }

	/* others */
    renv->pid = (pid_t) 0;
    renv->status = NoError;
    renv->wait_status = 0;

    sel->renv = renv;

    if (sel->status != NoError)		/* ErrIsNone or ErrNotRun */
	return sel->status;

    if (renv->is_remote)
	return NoError;

	/* command line */
    optbuf[0] = envbuf[0] = 0;
    log_path = Opt.LogPath;

    bp = envbuf;
	/* set XMODIFIERS */
    bp = strcpyx(bp, "XMODIFIERS='");
    bp = strcpyx(bp, ENV_MOD_IM);
    bp = strcpyx(bp, renv->im_mod);
    bp = strcpyx(bp, "'");
    *bp = 0;
    /* Local, LANG & DISPLAY have been already set to *environ by putenv() */

	/* IMS options */
    mk_ims_option(optbuf, sel);

    /* len = sysconf(_SC_ARG_MAX) / 2; len = Max(len, BUFSIZ); */
    len = strlen(envbuf) + strlen(ims->cmd_path)
	    + strlen(cmd_param) + strlen(optbuf) + strlen(log_path) + 40;

    /* for local execution */
    renv->cmdbuf = ALLOC(len, char);
    sprintf(renv->cmdbuf, " %s; export XMODIFIERS; exec  %s %s %s >> %s 2>&1 ",
	envbuf, ims->cmd_path, cmd_param, optbuf, log_path);

	/* timeout & interval */
    if (Opt.Timeout > 0)	tmout = Opt.Timeout;
    else if (ims->timeout > 0)	tmout = ims->timeout;
    else			tmout = DEFAULT_TIMEOUT;
    if (Opt.Interval > 0)	intv = Opt.Interval;
    else if (ims->interval > 0)	intv = ims->interval;
    else			intv = DEFAULT_INTERVAL;
    tmout = Max(tmout,  MIN_TIMEOUT);
    intv = Max(intv, MIN_INTERVAL);
    if (intv/1000 > tmout)	intv = tmout * 1000;
    /* else if (intv/10 < tmout)	intv = tmout * 10; */
    Opt.Timeout = tmout;
    Opt.Interval = intv;

#ifdef	DEBUG
    if (DebugLvl >= 1)	pr_RunEnv(sel->renv);
    DPR(("build_run_env(): Timeout=%d (sec)  Interval=%d (msec)\n",
					tmout, intv));
#endif

    return NoError;
}

static int	run_ims(sel)
    UserSelection	*sel;
{
    int		ret = NoError;

    if ((ret = open_display()) != NoError)
	return ret;

    if (is_ims_running(sel->renv, sel->ent->ims)) {
	sel->status = ErrImsRunning;
	DPR(("run_ims(): '%s' is already running\n", sel->name));
	return sel->status;
    }

    load_resources();

    if (sel->renv->is_remote) {
	/* ret = set_host_acss(sel->hostname); */

	ret = exec_remote_ims(sel);
	return ret;
    }

    ret = invoke_ims(sel);

    return ret;
}

static int	invoke_ims(sel)
    UserSelection	*sel;
{
    RunEnv	*renv = sel->renv;
    pid_t	pid;
    int		i;

    set_sig_chld(True);

    pid = fork();
    if (pid == (pid_t) -1) {
	put_xims_log("fork failed [%s]",
		(errno <= sys_nerr) ? sys_errlist[errno] : NULL, 0, 0);
#ifdef	DEBUG
	perror("fork");
#endif
	return renv->status = ErrImsExecution;
    }
    if (pid == (pid_t) 0) {	/* child */
	for (i = 0; i < _NFILE; i++)
	    (void) close(i);

#if defined(__osf__) || defined(CSRG_BASED)
	setsid();
#else
	setpgrp();
#endif
	execl(SH_PATH, "sh", "-c", renv->cmdbuf, NULL);

	put_xims_log("%s: exec failed [%s]", SH_PATH,
		(errno <= sys_nerr) ? sys_errlist[errno] : NULL, 0, 0);
	/* perror(SH_PATH); */
	sleep(1);
	_exit(1);
    }

    /* parent */
    renv->pid = pid;
    renv->wait_status = 0;
    renv->status = ErrImsWaiting;

    put_xims_log("'%s' started for %s", sel->name, userEnv.displayname, 0);

    DPR(("invoke_ims(%s): pid=%d\n", sel->name, pid));

    return NoError;
}

static void	on_sig_chld(sig)
    int sig;
{
    int		wait_status = 0;
    pid_t	pid;
    int		cause;
    RunEnv	*renv = userSel.renv;

    errno = 0;
    do {
#ifdef	_XPG4_EXTENDED
	pid = wait3(&wait_status, WNOHANG, (struct rusage *)NULL);
#else
	pid = waitpid((pid_t) -1, &wait_status, WNOHANG);
#endif	/* _XPG4_EXTENDED */
    } while (pid == -1 && errno == EINTR);

    DPR(("\ton_sig_chld(): pid=%d  errno=%d\n", pid, errno));

    if (pid == -1)
	return;

    signal(SIGCHLD, on_sig_chld);

    if (WIFEXITED(wait_status)) {
	cause = ErrImsAborted;
    } else if (WIFSIGNALED(wait_status)) {
	cause = ErrImsAborted;
    } else {	/* WIFSTOPPED(wait_status) */
	return;
    }

    if (renv->pid == pid) {
	if (renv->status == ErrImsWaiting || renv->status == ErrImsConnecting) {
	    renv->status = cause;
	    renv->wait_status = wait_status;
	    DPR(("on_sig_chld(): '%s' aborts (wait_status=%#x)\n",
					    userSel.name, wait_status));
	    put_xims_log("'%s' aborted.", userSel.name, 0, 0);
	    ximsWaitDone();
	}
#ifdef	DEBUG
	else {
	    DPR(("\tsig_chld: renv->state=%s is not ErrImsWaiting\n",
				    error_name(renv->status)));
	}
#endif
    }
#ifdef	DEBUG
    else {
	DPR(("\tsig_chld: pid=%d != renv->pid=%d\n", pid, renv->pid));
    }
#endif

    return;
}

static bool	is_ims_running(renv, ims)
    RunEnv	*renv;
    ImsConf	*ims;
{
    char	*prop_str;
    Window	owner;
    Atom	*atomp;

    if (prop_str = ims->property) {
	atomp = (Atom *)0;
    } else {
	if (!(prop_str = renv->atom_name))
	    return False;
	atomp = &renv->prop_atom;
    }

    owner = property_owner(atomp, prop_str);

    DPR2(("is_ims_running(): prop='%s'[%d] owned by %#x\n",
				prop_str, atomp ? *atomp : -1, owner));

#ifdef	unused
	/* check primary server name for XIM */
    if (owner == None && !ims->property && renv->proto == Proto_XIM
	&& ims->server_name2 && strstr(renv->atom_name, ims->server_name2) {
	char	buf[BUFSIZ], *p;

	prop_str = strcpy(buf, renv->atom_name);
	if (p = strchr(prop_str, '=')) {
	    strcpy(p + 1, ims->servername);
	    atomp = (Atom *)0;
	    owner = property_owner(atomp, prop_str);
	    DPR2(("is_ims_running(): prop='%s'[%d] owned by %#x\n",
				prop_str, atomp ? *atomp : -1, owner));
	}
    }
#endif

    return owner == None ? False : True;
}


static int	settle_ims(sel)
    UserSelection	*sel;
{

    if (isXsession()) {
	char	*prop_str;
	Window	owner;
	Atom	*atomp;
	ImsConf	*ims = sel->ent->ims;

	owner = None;
	if (prop_str = ims->property) {
	    atomp = (Atom *)0;
	    owner = property_owner(atomp, prop_str);
	}
#if	0
	else if (sel->renv) {
	    if (prop_str = sel->renv->atom_name)
		owner = search_clear_cmd_property(prop_str);
	}
#endif

	if (owner)
	    clear_cmd_property(owner);
    }

    return NoError;
}

static Window	property_owner(prop_atom, prop_str)
    Atom	*prop_atom;
    char	*prop_str;
{
    Atom	property = prop_atom ? *prop_atom : None;

    if (property == None) {
	property = XInternAtom(Dpy, prop_str, True);
	if (property == None)
	    return None;
	if (prop_atom)
	    *prop_atom = property;
    }
    return XGetSelectionOwner(Dpy, property);
}


# ifdef	old_hpux
	/* ***** try_connection ***** */

#define	MAX_RETRY	5

static jmp_buf	jmp_env;
static Window	dmy_win = 0;	/* temporary window used for XCreateIC() */

static void	catch_alarm(sig)
    int	sig;
{
    signal(SIGALRM, SIG_IGN);
    alarm(0);
    longjmp(jmp_env, 1);
}

static int	try_connection(sel)
    UserSelection	*sel;
{
    RunEnv	*renv = sel->renv;
    ImsConf	*ims = sel->ent->ims;
    char	envbuf[BUFSIZ], *bp;
    XIM		xim;
    int		ic_ok, retry_cnt;
    static char	*saved_xmod = NULL;
#ifdef	DEBUG
    time_t	last_time;
#endif

    DPR(("try_connection(%s):\n", sel->name));

    if (sel->status != NoError || !renv->im_mod)
	return sel->status;

    renv->status = ErrImsConnecting;
    set_sig_chld(True);

	/* set XMODIFIERS */
    saved_xmod = NULL;
    bp = strcpyx(envbuf, ENV_MOD_IM);
    bp = strcpyx(bp, renv->im_mod);
    saved_xmod = XSetLocaleModifiers(envbuf);
    DPR(("\tXSetLocaleModifiers(%s)\n", envbuf));

    ic_ok = False;
    if (setjmp(jmp_env) == 0) {
	signal(SIGALRM, catch_alarm);
	alarm(Opt.Timeout);

	for (retry_cnt = 0; !ic_ok && retry_cnt <= MAX_RETRY; retry_cnt++) {
	    if (retry_cnt)	sleep(retry_cnt * retry_cnt);

#ifdef	DEBUG
	    last_time = time((time_t)0);
#endif
	    xim = XOpenIM(Dpy, (XrmDatabase)0, ims->servername, ims->classname);
	    if (xim) {
		DPR(("try_connection(%d): XOpenIM() OK [%d sec.]",
				retry_cnt, time((time_t)0) - last_time));
#ifdef	DEBUG
		last_time = time((time_t)0);
#endif
		ic_ok = create_dummy_ic(xim);
		DPR(("\tXCreateIC() %s [%d sec.]\n",
			ic_ok ? "OK" : "Failed", time((time_t)0) - last_time));
		XCloseIM(xim); xim = 0;
	    } else {
		DPR(("try_connection(%d): XOpenIM() failed.\n", retry_cnt));
		ic_ok = False;
	    }
	}
    } else {		/* long_jmp() by alarm [timeout] */
	alarm(0); signal(SIGALRM, SIG_IGN);
	DPR(("try_connection(): XOpenIM() & XCreateIC() timed-out.\n"));
	if (dmy_win) {
	    XDestroyWindow(Dpy, dmy_win); dmy_win = 0;
	}
	    /* neither XDestroyIC() nor XCloseIM() should be called */
	xim = 0;
	ic_ok = False;
    }
    alarm(0); signal(SIGALRM, SIG_IGN);

	/* restore XMODIFIERS */
    if (saved_xmod) {
	DPR2(("\tXSetLocaleModifiers(save='%s')\n", saved_xmod));
	XSetLocaleModifiers(saved_xmod);
    }

    set_sig_chld(False);
    renv->status = ErrImsConnectDone;

    return ic_ok ? NoError : ErrImsTimeout;	/* ErrImsConnect; */
}

static int	create_dummy_ic(xim)
    XIM		xim;
{
    int		scr;
    XFontSet	fset;
    XIMStyles	*im_styles;
    XIMStyle	style;
    XIC		ic;
    unsigned long	fg, bg;
    XRectangle	area;
    XVaNestedList	status_attr;

    scr = DefaultScreen(Dpy);
    fg = BlackPixel(Dpy, scr);
    bg = WhitePixel(Dpy, scr);
    dmy_win = XCreateSimpleWindow(Dpy, RootWindow(Dpy, scr),
							0, 0, 1, 1, 0, bg, fg);

	/* search (PreeditNothing | StatusNothing [or StatusArea]) style */
    ic = 0;
    style = (XIMStyle) 0;
    im_styles = (XIMStyles *) 0;
    if (XGetIMValues(xim, XNQueryInputStyle, &im_styles, NULL)) {
	DPR(("create_dummy_ic(): XGetIMValues(XNQueryInutStyle) failed.\n"));
	goto _err;
    }
    if (!im_styles || !im_styles->count_styles) {
	DPR(("create_dummy_ic(): No input styles supported on IMS.\n"));
	if (im_styles)	XFree(im_styles);
	goto _err;
    }
    if ((int)im_styles->count_styles > 0) {
	int	i, alt;
	for (i = 0, alt = -1; i < (int)im_styles->count_styles; i++)
	    if (im_styles->supported_styles[i] & XIMPreeditNothing) {
		if (im_styles->supported_styles[i] & XIMStatusNothing) {
		    style = im_styles->supported_styles[i];
		    break;
		} else if (im_styles->supported_styles[i] & XIMStatusArea) {
		    alt = i;
		}
	    }
	if (!style && alt >= 0)	style = im_styles->supported_styles[alt];
	XFree(im_styles);
    }
    if (!style) {
	DPR(("create_dummy_ic(): 'PreeditNothing' styles not supported.\n"));
	goto _err;
	/* style = XIMPreeditNothing | XIMStatusNothing; */
    }

    fset = 0;
    status_attr = (XVaNestedList) 0;
    if (style & XIMStatusArea) {
	area.x = area.y = 0; area.width = area.height = 1;
	status_attr = XVaCreateNestedList(NULL,
			XNArea, &area,
			XNForeground, fg,
			XNBackground, bg,
			XNFontSet, fset,
			NULL );
    }

    ic = XCreateIC(xim,	XNInputStyle, style,
			XNClientWindow, dmy_win,
			XNStatusAttributes, status_attr,
			XNFocusWindow, dmy_win,
			NULL );

    /* if (fset)	XFreeFontSet(Dpy, fset); */
    if (ic)	XDestroyIC(ic);

_err:
    if (dmy_win)	XDestroyWindow(Dpy, dmy_win);
    dmy_win = 0;

    return ic ? True : False;
}

# endif	/* old_hpux */
