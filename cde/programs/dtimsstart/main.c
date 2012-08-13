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
/* $XConsortium: main.c /main/10 1996/10/28 12:41:03 cde-hp $ */

/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef	lint
static char	*RCS_ID = "@(#)dtimsstart $Revision: /main/10 $ $Date: 1996/10/28 12:41:03 $";
#endif

char	*ProgramRevision = "dtimsstart $Revision: /main/10 $";

#define	_EXTERN_DEFINE_
#include	"xims.h"
#undef	_EXTERN_DEFINE_

#include	<signal.h>
#include	<locale.h>

#ifndef	NLS
#define catgets(i, sn,mn,s) (s)
#else
#define NL_SETN 1	/* set number */
#include <nl_types.h>
static nl_catd	catd = (nl_catd) -1;
#endif	/* NLS */

extern char *find_system_locale_name(char *);

    /* local functions */
static int	IsNoError(/* error */);
static int	EnvNeeded(/* error */);
static int	ErrMsgDisabled(/* error */);
static int	ximsShowImsList(/*  */);
static int	ximsShowCurrentIms(/*  */);
static int	prepare_remote(/* win_st */);
static int	ximsRemoteConf(/*  */);
static int	ximsSetMode(/*  */);
static void	show_select_mode(/* mode */);
static int	set_locale_env(/* locale */);
static void	init_command(/* progname */);
static bool	optname_match(/* name, str, minlen */);
static int	parse_options(/* argc, argv */);
static void	usage(/* force */);

static int	exitSignalNumber = 0;
static bool	do_usage = False;


main (argc, argv)
    int		argc;
    char	**argv;
{
    int		ret = NoError;
    int		ret2;

    init_command(argv[0]);

    ret = parse_options(argc, argv);

    ret2 = set_locale_env(Opt.LocaleName);	/* set LANG, LC_ALL, msg cat */
	/* get_user_environ() does check ErrNoLocale */

    if (do_usage) {	/* '-help' option */
	usage(True);
	Exit(NoError);
    }
    if (ret != NoError)
	Exit(ret);

    if (OpMode == MODE_START && getenv(ENV_NO_DTIMSSTART))
	Exit(ErrDisabled);

    if ((ret = set_cmd_env()) != NoError) {
	    /* get_user_environ()) & read_cmd_conf()) */
	Exit(ret);
    }

#ifdef	DEBUG
    if (DebugLvl > 2)	{ pr_CmdConf(); }
    if (DebugLvl > 1)	{ pr_CmdOpt(), pr_OpModeFlag(); pr_UserEnv(); }
#endif

    /* set signal handler */
    if (signal(SIGINT, SIG_IGN) != SIG_IGN)	signal(SIGINT, sigExit);
    signal(SIGTERM, sigExit);
    signal(SIGHUP, sigExit);
    signal(SIGQUIT, sigExit);

	/* create $HOME/.xims if needed */
    if (OpMode == MODE_START
	|| (OpMode == MODE_MODE && Opt.SelectMode != SEL_MODE_QUERY)) {
	if ((ret = create_xims_dir()) != NoError) {
	    OpErrCode = ret;
	    OpState = State_Init_Err;
	    ximsMain();		/* abort with error msg */
	}
    }

    OpState = State_Init_Done;

    switch (OpMode) {
	case MODE_LISTNAME:
	case MODE_LIST:		ret = ximsShowImsList(); break;
	case MODE_CURRENT:	ret = ximsShowCurrentIms(); break;
	case MODE_MODE:		ret = ximsSetMode(); break;
	case MODE_REMCONF:	ret = ximsRemoteConf(); break;

	case MODE_START:
#ifndef	DEBUG2
# if defined(__osf__) || defined(CSRG_BASED)
				setsid();
# else
				setpgrp();
# endif
#endif
				ximsMain();	/* never returns */
    }

    Exit(ret);
}


void	sigExit(sig)
    int	sig;
{
    DPR(("sigExit:  %s (%d)\n", sig_name(sig), sig));
    signal(sig, SIG_IGN);
    exitSignalNumber = sig;
    Exit(ErrSignaled);
}

void	Exit(err_code)
    int	err_code;
{
    int		exit_code = IsNoError(err_code) ? 0 : 1;

    DPR(("Exit(%s[%d]): exit(%d)\n",
				error_name(err_code), err_code, exit_code));

    if (Verbose > 0 && err_code != LastErrMsg) {
	if (exit_code != 0 && !ErrMsgDisabled(err_code)) {
	    UseMsgWindow = False;		/* disable msg window */
	    put_xims_errmsg(err_code, 0, 0, 0);
	}
    }

    if (OpFlag & FLAG_NOTIFY)
	NotifyErrCode(exit_code == 0 ? NoError : err_code);

    /*
     * if the reason we're exiting is because of a syntax
     * error, set the exit code to 2.
     */
    if (ErrSyntax == err_code)
	exit_code = 2;

    exit(exit_code);
}

int	NotifyErrCode(err_code)
    int	err_code;
{
    char	buf[BUFSIZ], *bp, val[20];
    static bool	notify_done = False;

    if (/* !(OpFlag & FLAG_NOTIFY) || */ notify_done)
	return False;
    notify_done = True;

    buf[0] = 0; bp = buf;
    bp = strcpyx(bp, STR_ERRORCODE);
    *bp++ = '=';
    sprintf(val, "%ld", (long)err_code);
    bp = strcpyx(bp, val);
    *bp++ = '\n'; *bp = 0;

    if (ErrFilePathNeeded(err_code)) {
	bp = strcpyx(bp, STR_ERRORPATH);
	*bp++ = '=';
	if (err_code == ErrImsAborted)
	    bp = strcpyx(bp, Opt.LogPath);
	else
	    bp = strcpyx(bp, errFilePath);
	*bp++ = '\n'; *bp = 0;
    }

    if (OpMode == MODE_REMCONF) {
	send_dtims_msg(WIN_ST_REMOTE_CONF, err_code);

    } else if (OpMode == MODE_START && (OpFlag & FLAG_REMOTERUN)) {
	send_dtims_msg(WIN_ST_REMOTE_RUN, err_code);

    } else {
	fflush(stdout); fflush(stderr);
	(void) fwrite((void *)buf, (size_t)(bp - buf), (size_t)1, stdout);
	fflush(stdout);
    }

    return True;
}


static int	IsNoError(error)
    ximsError	error;
{
    switch (error) {
	case NoError: case ErrIsNone: case ErrNotRun:
	case ErrImsWaiting: case ErrImsWaitDone:
	case ErrImsConnecting: case ErrImsConnectDone:
	case ErrNoImsEntry: case ErrNoLocaleConf:
	case ErrNoSelectionFile: case ErrNoSelection: case ErrSaveSelection:
	case ErrDisabled:
		return True;
    }
    return False;
}

static int	EnvNeeded(error)
    ximsError	error;
{
    switch (error) {
	case NoError: case ErrIsNone: case ErrNotRun:
	case ErrImsWaiting: case ErrImsWaitDone:
	case ErrImsRunning: case ErrImsTimeout:
	/* case ErrImsExecution: case ErrImsAborted: */
	case ErrOpenResource:	/* case ErrSignaled: */
		return True;
    }
    return False;
}

static int	ErrMsgDisabled(error)
    ximsError	error;
{
    switch (error) {
	case NoError: case ErrIsNone: case ErrNotRun:
	case ErrImsWaiting: case ErrImsWaitDone:
	case ErrImsConnecting: case ErrImsConnectDone:
		return True;
	case ErrSignaled:
		return (Verbose <= 2);
	case ErrInvState: case ErrInternal:
		return (Verbose <= 3);
    }
    return False;
}

int	ErrFilePathNeeded(error)
    ximsError	error;
{
    switch (error) {
	case ErrFileOpen: case ErrFileCreate: case ErrDirCreate:
	case ErrMissEntry: case ErrNoExecutable: case ErrImsAborted:
	case ErrOpenResource:
		return True;
    }
    return False;
}

int	InWaitingState()
{
    switch (OpState) {
	case State_Init_Err:
    	case State_Finish_Defered:
	case State_Mode_Done:
	    return True;
    }
    return False;
}

void	ximsMain()
{
    static int	call_cnt = 0;

    call_cnt++;

    DPR(("\nximsMain(call_cnt=%d): OpState=%s  OpErrCode=%s[%d]\n",
		call_cnt, StateName(), error_name(OpErrCode), OpErrCode));

#ifdef	DEBUG
    pr_brk("ximsMain");
#endif

    if (OpFlag & FLAG_REMOTERUN) {
	int ret;
	if ((ret = prepare_remote(WIN_ST_REMOTE_RUN)) != NoError) {
	    Exit(ret);
	}
    }

    switch (OpState) {
	case State_Init_Done:		if (Verbose > 1)
					    put_xims_log("Started.", 0, 0, 0);
					ximsSelect();

	case State_Select_Done:
	case State_Select_Canceled:	ximsStart();

	case State_Start_Done:		ximsWait();

	case State_Init_Err:
	case State_Select_Err:
	case State_Start_Err:
	case State_Wait_Err:
			if (!WaitingDialogReply && !IsNoError(OpErrCode)) {
			    LastErrMsg = OpErrCode;
			    put_xims_errmsg(OpErrCode, 0, 0, 0);
			}

	case State_Wait_Done:
					ximsFinish();

	case State_Finish:
			if (WaitingDialogReply) {
			    DPR(("ximsMain(): enter xevent_loop()\n"));
			    xevent_loop();	/* never returns */
			    WaitingDialogReply = False;	/* if failed */
			}

	case State_Finish_Defered:
			if (OpState == State_Finish_Defered) {
			    WaitingDialogReply = False;
			    ximsFinish();
			}

	case State_Finish_Done:		break;
	case State_Finish_Err:		break;

	case State_Mode_Done:
	case State_Mode_Canceled:	break;

	default:
					OpErrCode = ErrInvState; break;
    }

    Exit(OpErrCode);
}


void	ximsFinish()
{
    OpStateVal	oldOpState = OpState;

    DPR(("ximsFinish(): OpState=%s  OpErrCode=%s[%d]\n",
			StateName(), error_name(OpErrCode), OpErrCode));

    OpState = State_Finish;

    if (oldOpState != State_Finish_Defered) {

	if (!(OpFlag & FLAG_NORESOURCE) && isXsession())
	    restore_resources();

	if (WaitingDialogReply) {
	    DPR2(("ximsFinish(): OpState => State_Finish_Defered\n"));;
	    OpState = State_Finish_Defered;
	    return;
	}
    }

    if (OpMode == MODE_START && (OpFlag & FLAG_ENV)) {
	if (userSel.name && EnvNeeded(OpErrCode)) {
	    OutEnv	outEnv;

	    CLR(&outEnv, OutEnv);
	    if (make_new_environ(&outEnv, &userSel) == NoError) {
		put_new_environ(&outEnv);
		clear_OutEnv(&outEnv);
	    }
	} else {
	    DPR(("ximsFinish(): Not put env (pri->status=%s[%d])\n",
		    error_name(userSel.renv->status), userSel.renv->status));
	}
    }

    /* if (IsNoError(OpErrCode))	OpErrCode = NoError; */
    OpState = State_Finish_Done;
    return;
}


static int	ximsShowImsList()
{
    int		ret;
    int		i;
    int		host_type = HOST_LOCAL;
    char	*hostname;
    ImsList	*list = (ImsList *) 0;

    DPR(("ximsShowImsList()\n"));

    hostname = Opt.HostName;
    if (hostname) {
	host_type = check_hostname(hostname);
	userSel.hostname = NEWSTR(hostname);	/* for error msg */
    }

    switch (host_type) {
	case HOST_UNKNOWN:
	    ret = ErrUnknownHost;
	    break;

	case HOST_REMOTE:
	    ret = get_remote_conf(&list, hostname, NULL, NULL);
	    break;

	case HOST_LOCAL:
	    ret = get_ims_list(&list, NULL, True);
	    break;
    }

    if (ret != NoError) {
	DPR2(("ximsShowImsList: ret=%s[%d]\n", error_name(ret), ret));
	/* return ret; */
    } else if (Verbose < 1) {
	for (i = 0; i < list->num_ent; i++)
	    puts(list->elist[i]->name);
    } else {
	int	err, len;

	for (i = 0; i < list->num_ent; i++) {
	    /* If invoked from dtstyle with -listname put the "#" if this is 
	       the default one. */
	    if (Opt.Listname == 1)
		if (i == list->default_idx)
		    putchar('#');
	    err = list->elist[i]->status;
	    if (err)	putchar('(');
	    fputs(list->elist[i]->name, stdout);
	    if (err)	putchar(')');
	    /* If invoked from dtstyle with -listname put the label */
	    if (Opt.Listname == 1) {
		putchar(' ');
		fputs(list->elist[i]->label, stdout);
	    }

	    if (Verbose > 1) {
		char	*msg = "cannot execute";
		switch (err) {
		    case NoError:
			msg = "ok"; break;
		    case ErrNoExecutable:
			msg = "no executable file"; break;
		    case ErrNoImsConf:
			msg = "no configuration file"; break;
		    case ErrMissEntry:
			msg = "invalid configuration file"; break;
		}
		len = strlen(list->elist[i]->name) + (err ? 2 : 0);
		for ( ; len < 16; len++)	putchar(' ');
		putchar(' '); putchar('['); fputs(msg, stdout); putchar(']');
	    }
	    putchar('\n');
	}
    }
    fflush(stdout);

    if (list) {
	clear_ImsList(list); FREE(list);
    }

    return NoError;
}

static int	ximsShowCurrentIms()
{
    int		ret = NoError;
    FileSel	*fsel;

    DPR(("ximsShowCurrentIms()\n"));

    if (read_user_selection(&fsel, NULL) != NoError) {
	DPR2(("No selection file.\n"));
	return ErrNoSelectionFile;
    }

    if (fsel->name) {
	if (fsel->hostname)
	    printf("%s\t[on %s]\n", fsel->name, fsel->hostname);
	else
	    puts(fsel->name);
	fflush(stdout);
    } else {
	DPR2(("No IMS selected.\n"));
	ret = ErrNoSelection;
    }

    clear_FileSel(fsel); FREE(fsel);

    return ret;
}


static int	prepare_remote(win_st)
    int	win_st;
{
    int		ret;
    char	**av = NULL;
    int		ac = 0;
    int		cur_st;

    DPR(("prepare_remote()\n"));

#ifndef	DEBUG
    Verbose = DebugLvl = 0;	/* disable error messages */
#endif
    UseMsgWindow = False;

    if ((ret = init_window_env()) != NoError)
	return ret;

    if (winEnv.atom_owner == None)
	return ErrRemoteData;

    cur_st = get_window_status();
    if (cur_st != win_st) {
	DPR(("prepare_remote(): invalid status '%d', should be '%d'\n",
						cur_st, win_st));
	return ErrRemoteData;
    }

    ret = get_window_data(&ac, &av);	/* additional options */
    if (ret != NoError) {
	DPR(("prepare_remote(): get_window_data (%s[%d])\n",
			    error_name(ret), ret));
    }

    if (ret == NoError) {
	ret = parse_options(ac, av);
	if (ret != NoError) {
	    DPR(("secondary parse_options(): failed (%s[%d])\n",
				error_name(ret), ret));
	}
    }

    return NoError;
}

static int	ximsRemoteConf()
{
    int		ret;
    char	*locale;

    DPR(("ximsRemoteConf()\n"));

    if ((ret = prepare_remote(WIN_ST_REMOTE_CONF)) != NoError)
	return ret;

    locale = userEnv.real_locale ? userEnv.real_locale : userEnv.locale;

    ret = put_remote_conf(locale, Opt.ImsName);

    return ret;
}


static int	ximsSetMode()
{
    int		ret = NoError;
    int		cur_mode = SEL_MODE_NONE;
    ImsList	imsList;

    DPR(("ximsSetMode(op=%d)\n", Opt.SelectMode));

    OpState = State_Mode;

    if ((ret = read_localeconf(&imsList, NULL)) != NoError) {
	DPR(("ximsSetMode(%s) locale '%s' isn't supported\n", userEnv.locale));
	/* return ErrNoLocaleConf; */
    }

    cur_mode = get_select_mode();
    if (cur_mode != SEL_MODE_AUTO && cur_mode != SEL_MODE_NOAUTO)
	cur_mode = imsList.def_selmode;

    switch (Opt.SelectMode) {
	case SEL_MODE_NONE:
	case SEL_MODE_QUERY:	show_select_mode(cur_mode);
				break;
	case SEL_MODE_WIN:	start_mode_window(cur_mode);
					/* never returned */
				break;
#ifdef	SelectMode_ONCE
	case SEL_MODE_ONCE:
#endif	/* SelectMode_ONCE */
	case SEL_MODE_AUTO:
	case SEL_MODE_NOAUTO:	ret = set_select_mode(cur_mode, Opt.SelectMode);
				break;
    }

    OpErrCode = ret;
    OpState = State_Mode_Done;
    return ret;
}

static void	show_select_mode(mode)
    int		mode;
{
    char	*valp;

    printf("%s: \t", (catgets(catd,NL_SETN,20, "SelectMode")));

    switch (mode) {
	default:
#ifdef	DEBUG
	case SEL_MODE_NONE:	valp = "<NOT DEFINED>"; /* break; */
#endif
	case SEL_MODE_NOAUTO:
	    valp = (catgets(catd,NL_SETN,21, "ask_at_login"));
	    break;
	case SEL_MODE_AUTO:
	    valp = (catgets(catd,NL_SETN,22, "resume_current_input_method"));
	    break;
#ifdef	SelectMode_ONCE
	case SEL_MODE_ONCE:
	    valp = "auto-selection if once selected";
	    break;
#endif	/* SelectMode_ONCE */
    }
    puts(valp); fflush(stdout);

    return;
}


static int	set_locale_env(locale)
    char	*locale;
{
    char	*env_name, *env_value, *bp, *vp, buf[BUFSIZ], buf2[BUFSIZ];
    static char	*last_lang_env[2] = { NULL, NULL };

    buf[0] = buf2[0] = 0;
    env_name = "LANG";
    env_value = getenv(env_name);
    if (env_value)
	strcpy(buf, env_value);
    if (locale && *locale) {
	if (!*buf || strcmp(locale, buf)) {
	    bp = strcpyx(buf, env_name); *bp++ = '=';
	    strcpyx(bp, locale);
	    putenv(bp = NEWSTR(buf));
	    FREE(last_lang_env[0]);
	    last_lang_env[0] = bp;
	}
    } else if (*buf)
	locale = buf;
    else
	return ErrNoLocale;

    env_name = "LC_ALL";
    env_value = getenv(env_name);
    if (env_value)
	strcpy(buf2, env_value);
    if (!*buf2 || strcmp(locale, buf2)) {
	bp = strcpyx(buf2, env_name); *bp++ = '=';
	strcpyx(bp, locale);
	putenv(bp = NEWSTR(buf2));
	FREE(last_lang_env[1]);
	last_lang_env[1] = bp;
    }

    setlocale(LC_ALL, locale);

    /* set XFILESEARCHPATH */
    vp = getenv(ENV_XFILESEARCHPATH);
    bp = strcpyx(buf, ENV_XFILESEARCHPATH);
    bp = strcpyx(bp, "=");
    if (vp) {
	bp = strcpyx(bp, vp);
	bp = strcpyx(bp, ":");
    }
    bp = strcpyx(bp, ENV_XFILESEARCHPATH_STRING);
    putenv(NEWSTR(buf));

    /* set NLSPATH */
    vp = getenv(ENV_NLSPATH);
    bp = strcpyx(buf, ENV_NLSPATH);
    bp = strcpyx(bp, "=");
    if (vp) {
	bp = strcpyx(bp, vp);
	bp = strcpyx(bp, ":");
    }
    bp = strcpyx(bp, ENV_NLSPATH_STRING);
    putenv(NEWSTR(buf));

#ifdef	NLS
    if (catd != (nl_catd) -1)	(void) catclose(catd);
    catd = (nl_catd) -1;	(void) catclose(catd);

    if (Verbose > 0) {
	catd = catopen(DTIMS_PROGNAME, 0);
    }
#endif	/* NLS */

    return NoError;
}

static void	init_command(progname)
    char	*progname;
{
	/* globals */
    ProgramName = progname;
    if (progname = strrchr(progname, '/'))
	ProgramName = progname + 1;
#ifdef	unused
    if (strstr(ProgramName, "mode"))		ProgramType = MODE_MODE;
    else if (strstr(ProgramName, "list"))	ProgramType = MODE_LIST;
    else if (strstr(ProgramName, "current"))	ProgramType = MODE_CURRENT;
    else if (strstr(ProgramName, "conf"))	ProgramType = MODE_REMCONF;
    else
#endif
	ProgramType = MODE_START;

    OpState = State_None;
    OpMode = ProgramType;
    OpFlag = FLAG_DEFAULT;
    OpErrCode = NoError;
    LogFp = stderr;
    Wargc = 0;
    Wargv = NULL;
    WaitingDialogReply = False;
    WaitingActionDone = False;
    UseMsgWindow = True;
    IsRemote = False;
    LastErrMsg = NoError;
    Verbose = 1;
    DebugLvl = 0;

    ximsErrArgs[0] = ximsErrArgs[1] = ximsErrArgs[2] = (void *) 0;
    errFilePath[0] = 0;
    errFuncName = NULL;

    CLR(&Conf, CmdConf);
    CLR(&Opt, CmdOpt);
    CLR(&winEnv, WinEnv);
    CLR(&userEnv, UserEnv);
    CLR(&userSel, UserSelection);
    localList = (ImsList *) 0;

    return;
}


static bool	optname_match(name, str, minlen)
    char	*name, *str;
    int		minlen;
{
    int		nlen, slen;

    if (strcmp(name, str) == 0)		return True;
    nlen = strlen(name);
    slen = strlen(str);
    if (slen >= nlen || slen < minlen)	return False;
    if (strncmp(name, str, slen) == 0)	return True;

    return False;
}

static int	parse_options(argc, argv)
    int		argc;
    char	**argv;
{
    char	*opt;
    int		i, n;
    int		wac = 1;
    char	*wav[80];
    int		orgMode;
    static bool	first_time = True;

#define	SET_FLAG(f)	OpFlag |= (f)
#define	CLR_FLAG(f)	OpFlag &= ~(f)
#define	CHK_FLAG(f)	(OpFlag & (f))

    if (first_time) {
	argc--; argv++;

	    /* preset */
	Opt.Listname = 0;
	orgMode = OpMode;
	if (orgMode == MODE_START)	SET_FLAG(FLAG_WINDOW);
	Opt.SelectMode = SEL_MODE_NONE;
    }

    for (i = 0; i < argc; i++) {
	opt = argv[i];
	if (*opt++ != '-')	goto _inv_opt;

	    /* option for MODE */
	if (optname_match("mode", opt, 2)) {
	    if (orgMode != MODE_START && orgMode != MODE_MODE)
		goto _inv_opt;
	    OpMode = MODE_MODE;
#if	0	/* not implemented yet */
	} else if (optname_match("style", opt, 3)) {
	    if (orgMode != MODE_START && orgMode != MODE_MODE)
		goto _inv_opt;
	    OpMode = MODE_STYLE;
#endif
	} else if (optname_match("listname", opt, 5)) {
	    if (orgMode != MODE_START && orgMode != MODE_LISTNAME)
		goto _inv_opt;
	    OpMode = MODE_LISTNAME;
	    Opt.Listname = 1;
	} else if (optname_match("list", opt, 2)) {
	    if (orgMode != MODE_START && orgMode != MODE_LIST)
		goto _inv_opt;
	    OpMode = MODE_LIST;
	} else if (optname_match("current", opt, 2)) {
	    if (orgMode != MODE_START && orgMode != MODE_CURRENT)
		goto _inv_opt;
	    OpMode = MODE_CURRENT;
	} else if (optname_match("remoteconf", opt, 7)) {
	    if (orgMode != MODE_START && orgMode != MODE_REMCONF)
		goto _inv_opt;
	    OpMode = MODE_REMCONF;
	    SET_FLAG(FLAG_NOTIFY);

	    /* option for FLAG */
	} else if (optname_match("nosave", opt, 4)) {
	    SET_FLAG(FLAG_NOSAVE);
	} else if (optname_match("nostart", opt, 4)) {
	    SET_FLAG(FLAG_NOSTART);
	} else if (optname_match("noenv", opt, 4)) {
	    CLR_FLAG(FLAG_ENV);
	} else if (optname_match("envonly", opt, 4)) {
	    SET_FLAG(FLAG_NOSTART|FLAG_ENV);
	} else if (optname_match("env", opt, 2)) {
	    SET_FLAG(FLAG_ENV);
	} else if (optname_match("nowait", opt, 4)) {
	    SET_FLAG(FLAG_NOWAIT);
	} else if (optname_match("notimeout", opt, 5)) {
	    SET_FLAG(FLAG_NOTIMEOUT);
	} else if (optname_match("noresource", opt, 5)) {
	    SET_FLAG(FLAG_NORESOURCE);
	} else if (optname_match("noremote", opt, 5)) {
	    SET_FLAG(FLAG_NOREMOTE);
	} else if (optname_match("notify", opt, 5)) {
	    SET_FLAG(FLAG_NOTIFY);
	} else if (optname_match("connect", opt, 4)) {
	    SET_FLAG(FLAG_CONNECT);
	} else if (optname_match("dt", opt, 2)) {
	    SET_FLAG(FLAG_DT);
# ifdef	old_hpux
	    CLR_FLAG(FLAG_VUE);
	} else if (optname_match("vue", opt, 2)) {
	    SET_FLAG(FLAG_VUE);
	    CLR_FLAG(FLAG_DT);
# endif	/* old_hpux */
	} else if (optname_match("window", opt, 2)) {
	    SET_FLAG(FLAG_WINDOW);
	} else if (optname_match("nowindow", opt, 4)) {
	    CLR_FLAG(FLAG_WINDOW);
	} else if (optname_match("remoterun", opt, 7)) {
	    SET_FLAG(FLAG_REMOTERUN);
	    SET_FLAG(FLAG_NOTIFY);

	} else if (optname_match("auto", opt, 2)) {
	    Opt.SelectMode = SEL_MODE_AUTO;
	} else if (optname_match("noauto", opt, 3)) {
	    Opt.SelectMode = SEL_MODE_NOAUTO;
#ifdef	SelectMode_ONCE
	} else if (optname_match("once", opt, 3)) {
	    Opt.SelectMode = SEL_MODE_ONCE;
#endif	/* SelectMode_ONCE */

	    /* help, verbose & debug options */
	} else if (optname_match("help", opt, 1)) {
	    if (first_time)
		do_usage = True;
	} else if (optname_match("quiet", opt, 1)) {
	    Verbose = DebugLvl = 0;
	} else if (optname_match("verbose", opt, 1)) {
	    Verbose++;
	} else if (optname_match("debug", opt, 2)) {
	    Verbose++; DebugLvl++;

	} else if (i >= argc - 1 || !*(argv[i+1])) {
	    goto _inv_opt;	/* the rest options need an argument */

	    /* options with an argument */
	} else if (optname_match("imsoption", opt, 4)) {
	    i++;
	    Opt.ImsOption = argv[i];
	} else if (optname_match("ims", opt, 2)) {
	    i++;
	    Opt.ImsName = argv[i];
	} else if (optname_match("display", opt, 2)) {
	    i++;
	    Opt.DisplayName = argv[i];
	    wav[wac++] = argv[i - 1]; wav[wac++] = argv[i];	/* ??? */
	} else if (optname_match("locale", opt, 2)) {
	    i++;
	    Opt.LocaleName = argv[i];
	} else if (optname_match("CDE_locale", opt, 2)) {
	    i++;
	    Opt.LocaleName = find_system_locale_name(argv[i]);
	} else if (optname_match("hostname", opt, 2)) {
	    i++;
	    Opt.HostName = argv[i];
	} else if (optname_match("config", opt, 2)) {
	    i++;
	    Opt.ConfPath = argv[i];
	} else if (optname_match("file", opt, 2)) {
	    i++;
	    Opt.UserPath = argv[i];
	} else if (optname_match("log", opt, 2)) {
	    i++;
	    Opt.LogPath = argv[i];
	} else if (optname_match("resource", opt, 3)) {
	    i++;
	    Opt.ResourceFile = argv[i];
	} else if (optname_match("shell", opt, 2)) {
	    i++;
	    Opt.ShellName = argv[i];
	} else if (optname_match("timeout", opt, 3)) {
	    i++;
	    if (str_to_int(argv[i], &n) && n >= 0)
		Opt.Timeout = n;
	} else if (optname_match("interval", opt, 3)) {
	    i++;
	    if (str_to_int(argv[i], &n) && n >= 0)
		Opt.Interval = n;

	    /* Xt options */
	} else if (optname_match("font", opt, 2)
		|| optname_match("fn", opt, 2)
		|| optname_match("fg", opt, 2)
		|| optname_match("bg", opt, 2)
		|| optname_match("bd", opt, 2)
		|| optname_match("foreground", opt, 4)
		|| optname_match("backgroundg", opt, 4)
		|| optname_match("bordercolor", opt, 4)
		|| optname_match("geometry", opt, 2)
		|| optname_match("title", opt, 2)
		|| optname_match("xnlLanguage", opt, 3)
		/* || optname_match("iconic", opt, 4) */
		|| optname_match("xrm", opt, 2)) {
	    i++;
	    wav[wac++] = argv[i - 1]; wav[wac++] = argv[i];

	} else
	    goto _inv_opt;
    }
    first_time = False;

    if (do_usage)	return NoError;

    switch (OpMode) {		/* adjust OpFlag */
#if	0	/* noy implemented yet */
	case MODE_STYLE:
	    SET_FLAG(FLAG_DT | FLAG_WINDOW);
	    CLR_FLAG(FLAG_VUE);
	    break;
#endif

	case MODE_START:
	    if (Opt.ImsName) {
		SET_FLAG(FLAG_NOSAVE);
		/* CLR_FLAG(USE_WINDOW_MASK); */
		Opt.SelectMode = SEL_MODE_GIVEN;
	    }
	    if (Opt.LogPath) {	/* open log file */
		if (set_errorlog(Opt.LogPath) != NoError)
		    Opt.LogPath = NULL;
	    }
	    if (CHK_FLAG(FLAG_REMOTERUN)) {
		SET_FLAG(FLAG_NOTIFY);
		SET_FLAG(FLAG_NOSAVE);
		CLR_FLAG(USE_WINDOW_MASK);
	    }
	    break;

	case MODE_MODE:
	    if (CHK_FLAG(FLAG_WINDOW)) {
		Opt.SelectMode = SEL_MODE_WIN;
	    } else if (Opt.SelectMode == SEL_MODE_NONE) {
		Opt.SelectMode = SEL_MODE_QUERY;
		CLR_FLAG(USE_WINDOW_MASK);
	    }
	    break;

	case MODE_LISTNAME:
	    SET_FLAG(FLAG_WINDOW);
	    break;

	default:
	    CLR_FLAG(USE_WINDOW_MASK);
	    break;
    }

    if (CHK_FLAG(USE_WINDOW_MASK)) {
	    /* create argc & argv for window */
	Wargv = ALLOC(wac + 1 + 4, char *);	/* spare for '-xnlLanguage' */
	memcpy(Wargv, wav, sizeof(char *) * wac);
	Wargv[0] = ProgramName;
	Wargv[wac] = NULL;
	Wargc = wac;
    } else {
	UseMsgWindow = False;		/* disable msg window */
    }

#undef	CLR_FLAG
#undef	SET_FLAG
#undef	CHK_FLAG

    return NoError;

_inv_opt:
    DPR(("%s: invalid option '%s'\n", ProgramName,  argv[i]));
    setErrArg1(argv[i]);

    return ErrSyntax;
}


	/* ********  usage  ******** */

#ifdef	DEBUG
typedef struct {
    char	*name;
    char	*desc;
} OptDesc;

#if	0
static OptDesc	opts_start[] = {
    { "-env",	"print modified environment variables" },
    { "-shell <name>",	"override $SHELL" },
    { "-ims <name>",	"specifies input method name" },
    { "-imsopt <string>",	"specifies command options for input method server" },
    { "-host <name>",	"specifies host where input method server should be run" },
    { "-mode",	"change input method selection mode" },
    { "-list",	"show registered input method" },
    { "-help",	"show this message" },
    { NULL, NULL }
};

static OptDesc	opts_mode[] = {
    { "-auto",		"enable auto-selection mode" },
    { "-noauto",	"disable auto-selection mode" },
    { NULL, NULL }
};
#endif

static OptDesc	opts_internal[] = {
    { "-display <dpy>",	"override $DISPLAY" },
    { "-current",	"show selected input method" },
    { "-listname",      "show registered input method for dtstyle" },
    { "-remoteconf",	"print IMS configuration data for remote execution" },
    { "-remoterun",	"start IMS as remote execution" },
    { "-nosave",	"not save the selection" },
    { "-select",	"force select" },
    { "-noenv",		"disable printing environment variables" },
    { "-envonly",	"-noenv & -nostart" },
    { "-nostart",	"not start IMS" },
    { "-noresouce",	"not load session resource files" },
    { "-nowait", 	"immediately exits after IMS invocation" },
    { "-notimeout", 	"do not timeout" },
    { "-noremote", 	"disable remote execution" },
    { "-notify",	"print error code to stderr" },
# ifdef	old_hpux
    { "-vue",		"in VUE environment" },
    { "-connect", 	"enable try_connect" },
# endif	/* old_hpux */
#ifdef	SelectMode_ONCE
    { "-once",		"auto-selection once selected" },
#endif	/* SelectMode_ONCE */
    { "-dt"	,	"in CDE environment" },
    { "-window",	"use window environment" },
    { "-nowindow",	"do not use window environment" },
    { "-timeout  <num>",	   "specifies timeout period (sec.)" },
    { "-interval <num>",	   "specifies check interval (msec.)" },
    { "-locale  <locale>",	   "override $LANG" },
    { "-CDE_locale  <CDE_locale>", "CDE generic locale name" },
    { "-resource  <file>",	   "specifies resource file" },
    { "-config <file>",	"specifies configuration file" },
    { "-file <file>",	"specifies user selection file" },
    { "-log <file>",	"specifies log file" },
    { "-fn <fontlist>",	"specifies font" },
    { "-quiet",		"print no messages" },
    { "-verbose",	"print verbose messages" },
    { "-debug",		"print debug messages" },
    { "<Xt options>",	"standard X toolkit options" },
    { NULL, NULL }
};
#endif	/* DEBUG */

static void	usage(force)
    int		force;
{
    char	*fmt = "\t%-20s%s\n";

    if (!force && Verbose <= 0)	return;

    fprintf(LogFp, (catgets(catd,NL_SETN,1, "usage:  %s  [options ..]")),
							    ProgramName);
    putc('\n', LogFp);

    if (OpMode != MODE_MODE) {
	fprintf(LogFp, fmt,
	    "-env", (catgets(catd,NL_SETN,2, "print modified environment variables")));
	fprintf(LogFp, fmt,
	    "-shell <name>", (catgets(catd,NL_SETN,3, "override $SHELL")));
	fprintf(LogFp, fmt,
	    "-ims <name>", (catgets(catd,NL_SETN,4, "specifies input method name")));
	fprintf(LogFp, fmt,
	    "-imsopt <string>", (catgets(catd,NL_SETN,5, "specifies command options for input method server")));
	fprintf(LogFp, fmt,
	    "-host <name>", (catgets(catd,NL_SETN,6, "specifies host where input method server should be run")));
	fprintf(LogFp, fmt,
	    "-mode", (catgets(catd,NL_SETN,7, "change input method selection mode")));
	fprintf(LogFp, fmt,
	    "-list", (catgets(catd,NL_SETN,8, "print registered input method")));
	fprintf(LogFp, fmt,
	    "-help", (catgets(catd,NL_SETN,9, "show this message")));
    } else {
	fprintf(LogFp, fmt,
	    "-auto", (catgets(catd,NL_SETN,23, "force 'resume_current_input_method' mode")));
	fprintf(LogFp, fmt,
	    "-noauto", (catgets(catd,NL_SETN,24, "force 'ask_at_login' mode")));
    }

#ifdef	DEBUG
    if (Verbose > 1) {
	OptDesc	*opts;
	fprintf(LogFp, "\n    <internal options>\n");
	for (opts = opts_internal; opts->name; opts++)
	    fprintf(LogFp, fmt, opts->name, opts->desc);
    }
#endif

}


	/* ********  error messages  ******** */

char	*xims_errmsg(err_num, arg1, arg2, arg3)
    int		err_num;
    void	*arg1, *arg2, *arg3;
{
    char	*fmt = NULL, *bp;
    int		len;
    static char	msgbuf[BUFSIZ];

    switch (err_num) {
	case NoError:
		break;
	case ErrSyntax:		/* arg1: option string */
		fmt = (catgets(catd,NL_SETN,31, "invalid option '%s'"));
		break;
	case ErrNoHome:
		fmt = (catgets(catd,NL_SETN,32, "environment variable 'HOME' not defined"));
		break;
	case ErrNoLocale:
		fmt = (catgets(catd,NL_SETN,33, "environment variable 'LANG' not defined"));
		break;
	case ErrNoCDELocale:
		fmt = (catgets(catd,NL_SETN,34, "this locale is not supported by the desktop."));
		break;
	case ErrNoDisplay:
		fmt = (catgets(catd,NL_SETN,35, "environment variable 'DISPLAY' not defined"));
		break;

	case ErrFileOpen:	/* arg1: file name */
		fmt = (catgets(catd,NL_SETN,36, "cannot open file\n  [%s]"));
		setErrArg1(errFilePath);
		break;
	case ErrFileCreate:	/* arg1: file name */
		fmt = (catgets(catd,NL_SETN,37, "cannot create file\n  [%s]"));
		setErrArg1(errFilePath);
		break;
	case ErrDirCreate:	/* arg1: dir name */
		fmt = (catgets(catd,NL_SETN,38, "cannot create directory\n  [%s]"));
		setErrArg1(errFilePath);
		break;
	case ErrMissEntry:	/* arg1: entry name, arg2: file name */
		fmt = (catgets(catd,NL_SETN,39, "missing '%s' entry in configuration file\n  [%s]"));
		setErrArg2(errFilePath);
		break;

	case ErrAnotherProg:
		fmt = (catgets(catd,NL_SETN,40, "another '%s' is already running"));
		setErrArg1(ProgramName);
		break;
	case ErrNoSelectionFile:	/* arg1: locale name */
		fmt = (catgets(catd,NL_SETN,41, "no selection file for '%s'"));
		setErrArg1(userEnv.locale);
		break;
	case ErrSaveSelection:	/* arg1: file name */
		fmt = (catgets(catd,NL_SETN,42, "cannot create selection file\n  [%s]"));
		setErrArg1(errFilePath);
		break;
	case ErrNoSelection:	/* arg1: locale name */
		fmt = (catgets(catd,NL_SETN,43, "no ims selected for '%s'"));
		setErrArg1(userEnv.locale);
		break;
	case ErrNoLocaleConf:	/* arg1: file name */
		fmt = (catgets(catd,NL_SETN,44, "no locale configuration file for '%s'"));
		setErrArg1(userEnv.locale);
		break;
	case ErrNoImsEntry:	/* arg1: locale name */
		fmt = (catgets(catd,NL_SETN,45, "no ims configured for '%s'"));
		setErrArg1(userEnv.locale);
		break;
	case ErrNoImsConf:	/* arg1: ims name */
		fmt = (catgets(catd,NL_SETN,46, "no ims configuration file for '%s'"));
		setErrArg1(userSel.name);
		break;
	case ErrNotRegistered:	/* arg1: ims name */
		fmt = (catgets(catd,NL_SETN,47, "ims '%s' not registered"));
		setErrArg1(userSel.name);
		break;
	case ErrNoExecutable:	/* arg1: ims name, arg2: file name */
		fmt = (catgets(catd,NL_SETN,48, "no executable file for '%s'\n  [%s]"));
		setErrArg1(userSel.name);
		setErrArg2(userSel.ent->ims->cmd_path);
		break;

	case ErrImsRunning:	/* arg1: ims name */
		fmt = (catgets(catd,NL_SETN,49, "ims '%s' is already running"));
		setErrArg1(userSel.name);
		break;
	case ErrImsExecution:	/* arg1: ims name */
		fmt = (catgets(catd,NL_SETN,50, "cannot execute ims '%s'"));
		setErrArg1(userSel.name);
		break;
	case ErrImsAborted:	/* arg1: ims name, arg2: log file */
		fmt = (catgets(catd,NL_SETN,51, "ims '%s' aborted.  See log file.\n  [%s]"));
		setErrArg1(userSel.name);
		if (userSel.host_type == HOST_REMOTE)
		    setErrArg2(errFilePath);
		else
		    setErrArg2(Opt.LogPath);
		break;
	case ErrImsTimeout:	/* arg1: file name */
		fmt = (catgets(catd,NL_SETN,52, "ims '%s' is not available yet"));
		setErrArg1(userSel.name);
		break;

	case ErrUnknownHost:	/* arg1: host name */
		fmt = (catgets(catd,NL_SETN,53, "unknown host '%s'"));
		setErrArg1(userSel.hostname);
		break;
	case ErrRemoteAction:	/* arg1: action name */
		fmt = (catgets(catd,NL_SETN,54, "action '%s' failed"));
		setErrArg1(errFuncName);
		break;
	case ErrRemoteData:	/* arg1: host name */
		fmt = (catgets(catd,NL_SETN,55, "remote execution failed on '%s'"));
		setErrArg1(userSel.hostname);
		break;
	case ErrNoImsstart:	/* arg1: host name */
		fmt = (catgets(catd,NL_SETN,56, "remote functionality is not available on '%s'"));
		setErrArg1(userSel.hostname);
		break;
	case ErrRemoteNoIms:	/* arg1: host name */
		fmt = (catgets(catd,NL_SETN,57, "no ims registered on '%s'"));
		setErrArg1(userSel.hostname);
		break;
	case ErrRemoteMissIms:	/* arg1: ims name, arg2: host name */
		fmt = (catgets(catd,NL_SETN,58, "ims '%1$s' not registered on '%2$s'"));
		setErrArg1(userSel.name);
		setErrArg2(userSel.hostname);
		break;

	case ErrOpenDpy:	/* arg1: display name */
		fmt = (catgets(catd,NL_SETN,59, "cannot open display '%s'"));
		setErrArg1(userEnv.displayname);
		break;

#ifdef	DEBUG
	case ErrSignaled:	/* arg1: signal number */
		fmt = "terminated by signal (signal=%d)";
		setErrArg1(exitSignalNumber);
		break;
	case ErrDisabled:	/* arg1: program name, arg2: env name */
		fmt = "ximsstart is disabled ('%s' is set)";
		setErrArg1(ENV_NO_DTIMSSTART);
		break;
	case ErrRemoteIms:	/* arg1: ims name, arg2: host name */
		fmt = "cannot execute ims '%s' on '%s'";
		setErrArg1(userSel.name);
		setErrArg2(userSel.hostname);
		break;
	case ErrOpenResource:	/* arg1: file name */
		fmt = "cannot open resource file\n  [%s]";
		setErrArg1(Opt.ResourceFile);
		break;
	case ErrMemory:		/* arg1: function name */
		fmt = "cannot allocate memory in %s()";
		setErrArg1(errFuncName);
		break;

	case ErrIsNone:		/* arg1: ims name */
		fmt = "the selected ims is '%s'";
		setErrArg1(userSel.name);
		break;
	case ErrNotRun:		/* arg1: ims name */
		fmt = "the selected ims '%s' need not to run";
		setErrArg1(userSel.name);
		break;
	case ErrImsWaiting:	/* arg1: ims name */
		fmt = "internal error [ImsWaiting] (ims='%s')";
		setErrArg1(userSel.name);
		break;
	case ErrImsWaitDone:	/* arg1: ims name */
		fmt = "internal error [ImsWaitDone] (ims='%s')";
		setErrArg1(userSel.name);
		break;
	case ErrImsConnecting:	/* arg1: ims name */
		fmt = "internal error [ImsConnecting] (ims='%s')";
		setErrArg1(userSel.name);
		break;
	case ErrImsConnectDone:	/* arg1: ims name */
		fmt = "internal error [ImsConnectDone] (ims='%s')";
		setErrArg1(userSel.name);
		break;
	case ErrInvState:	/* arg1: OpStateVal */
		fmt = "internal error [invalid state: '%d']";
		setErrArg1(OpState);
		break;
	case ErrInternal:	/* arg1: function name */
		fmt = "internal error in %s()";
		setErrArg1(errFuncName);
		break;
#endif	/* DEBUG */
    }

    if (!fmt)	return NULL;

    bp = strcpyx(msgbuf, ProgramName); *bp++ = ':'; *bp++ = ' ';
    if (!arg1)	arg1 = ximsErrArgs[0];
    if (!arg2)	arg2 = ximsErrArgs[1];
    if (!arg3)	arg3 = ximsErrArgs[2];
    sprintf(bp, fmt, arg1, arg2, arg3);
    len = strlen(bp);
    bp[len++] = '\n'; bp[len] = '\0';

    return msgbuf;
}

