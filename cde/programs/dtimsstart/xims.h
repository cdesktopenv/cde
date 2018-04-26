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
/* $XConsortium: xims.h /main/6 1996/04/17 16:25:50 pascale $ */

/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef	_XIMS_H_
#define	_XIMS_H_	1

#include	<X11/X.h>
#include	<X11/Xlib.h>
#include	<X11/Intrinsic.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<sys/param.h>
#include	<sys/time.h>

#include	"ximspath.h"
#include	"ximserr.h"


    /* operation modes */
#define	MODE_START		0	/* default */
#define	MODE_MODE		1	/* -mode option */
#define	MODE_STYLE		2	/* -style option */
#define	MODE_LIST		3	/* -list option */	/* no window */
#define	MODE_CURRENT		4	/* -current option */	/* no window */
#define	MODE_REMCONF		5	/* -remoteconf */	/* no window */
#define	MODE_LISTNAME		6	/* -listname option */	

    /* operation flags */
#define	FLAG_ENV		(1<<0)	/* -env option */
#define	FLAG_NOSAVE		(1<<1)	/* -nosave option */
#define	FLAG_NOSTART		(1<<2)	/* -nostart option (DEBUG) */
#define	FLAG_NOWAIT		(1<<3)	/* -nowait option */
#define	FLAG_NOTIMEOUT		(1<<4)	/* -notimeout option */
#define	FLAG_NORESOURCE		(1<<5)	/* -noresource option */
#define	FLAG_NOREMOTE		(1<<6)	/* -noremote option */
#define	FLAG_NOTIFY		(1<<7)	/* -notify option */
#define	FLAG_CONNECT		(1<<8)	/* -connect option */
#define	FLAG_REMOTERUN		(1<<9)	/* -remoterun option */
#define	FLAG_WINDOW		(1<<10)	/* -window option */
#define	FLAG_DT			(1<<11)	/* -dt option */
# ifdef	old_hpux
#define	FLAG_VUE		(1<<12)	/* -vue option */
# endif	/* old_hpux */

#define	FLAG_DEFAULT		(FLAG_DT)
# ifdef	old_hpux
#define	USE_WINDOW_MASK		(FLAG_WINDOW|FLAG_VUE|FLAG_DT)
# else
#define	USE_WINDOW_MASK		(FLAG_WINDOW|FLAG_DT)
# endif	/* old_hpux */

#define	MAXIMSENT		32	/* max # of ims for each locale */

#define	MIN_INTERVAL		100	/* msec */
#define	DEFAULT_INTERVAL	1000	/* msec */
#define	MIN_TIMEOUT		1	/* sec */
#define	DEFAULT_TIMEOUT		180	/* sec */
#define	REMOTE_TIMEOUT		60	/* sec */

#define	TAG_END_CHAR		':'
#define	TAG_SEP_CHAR		'.'
#define	LIST_SEP_CHAR		','
#define	COMMENT_CHAR		'!'
#define	COMMENT_CHAR2		'#'
#define	COMMENT_CHARS		"!#"

#define	NAME_NONE		"none"
#define	NAME_LOCAL		"local"
#define	NAME_BUILTIN		"builtin"
#define	NAME_DEFAULT		"@"
#define	NAME_DEFAULT_CHAR	'@'
#define	DUMMY_SERVER_NAME	NAME_DEFAULT

#define	STR_PREFIX_CHAR		'@'
#define	STR_DEFAULTIMS		"DefaultIms"
#define	STR_SELECTMODE		"SelectMode"
#define	STR_IMSNAME		"ImsName"
#define	STR_HOSTNAME		"HostName"
#define	STR_ICONIC		"IconicMode"
#define	STR_IMSOPTION		"ImsOption"
#define	STR_ERRORCODE		"#ErrorCode"
#define	STR_ERRORPATH		"#ErrorPath"
#define	STR_CONFDATA		"#IMSSTART-CONFDATA-1.0"
#define	STR_ICONIC_OPT		"-iconic"

    /* protocol index */
#define	Proto_None		0
#define	Proto_XIM		1
#define	Proto_Ximp		2
#define	Proto_Xsi		3
# ifdef	old_hpux
#define	Proto_Xhp		4
#define	NUM_PROTOCOLS		5
# else
#define	NUM_PROTOCOLS		4
# endif	/* old_hpux */
    /* protocol flag */
#define	ProtoBit(i)		(1<<(i))

    /* window state */
#define	WIN_ST_NONE		0
#define	WIN_ST_INIT		1
#define	WIN_ST_REMOTE_CONF	2
#define	WIN_ST_REMOTE_RUN	3
#define	WIN_ST_ACTION_DONE	4

    /* ims server flag */
#define	F_NO_SERVER		(1<<0)
#define	F_NO_REMOTE		(1<<1)
#define	F_NO_OPTION		(1<<2)
#define	F_HAS_WINDOW		(1<<3)
#define	F_TRY_CONNECT		(1<<4)
#define	F_BUILTIN		(1<<8)

    /* user selection flag */
#define	F_SELECT_NONE		0
#define	F_SELECT_FILE		(1<<0)
#define	F_SELECT_WINDOW		(1<<1)
#define	F_SELECT_CHANGED	(1<<4)

    /* select_mode */
#define	NUM_SEL_MODE		2
#define	SEL_MODE_NOAUTO		0
#define	SEL_MODE_AUTO		1
#define	SEL_MODE_ONCE		2
#define	SEL_MODE_NONE		-1
#define	SEL_MODE_GIVEN		-2
#define	SEL_MODE_QUERY		-3
#define	SEL_MODE_WIN		-4

    /* host type */
#define	HOST_LOCAL		0
#define	HOST_REMOTE		1
#define	HOST_UNKNOWN		2

# ifdef	old_hpux
    /* index for XhpLocale */
#define	XHP_JPN			0
#define	XHP_KOR			1
#define	XHP_CHS			2
#define	XHP_CHT			3
#define	XHP_LANG_NUM		4
# endif	/* old_hpux */

    /* message type for put_msg_win() */
#define	MSGTYP_INFO		0
#define	MSGTYP_WARN		1
#define	MSGTYP_FATAL		2
#define	MSGTYP_CONFIRM		3

    /* help message type for ximsHelp() */
#define	HELP_SELECTION		0
#define	HELP_MODE		1

    /* actions internally used */
#define	ACT_GETREMCONF		0
#define	ACT_RUNREMIMS		1
#define	NUM_ACTIONS		2
#define	ACTION_MASK		0xf0


typedef	short		bool;

    /* operation states */
typedef enum {
    State_None = 0,
    State_Init,
    State_Init_Err,
    State_Init_Done,
    State_Select,
    State_Select_Err,
    State_Select_Canceled,
    State_Select_Done,
    State_Start,
    State_Start_Err,
    State_Start_Done,
    State_Wait,
    State_Wait_Err,
    State_Wait_Done,
    State_Finish,
    State_Finish_Err,
    State_Finish_Defered,
    State_Finish_Done,
    State_Mode,
    State_Mode_Canceled,
    State_Mode_Done,

    State_Action_Invoked,
    State_Action_Waiting,
    State_Action_Done,
    State_Action_Err,

    State_Last
} OpStateVal;

    /* structure definitions */

typedef struct {
    char	*name;
    char	*value;
} EnvEnt;

typedef struct {
    char	*ims_name;
    char	*opt_str;
} ImsOpt;

typedef struct {
    EnvEnt	*set;
    EnvEnt	*unset;
} OutEnv;

# ifdef	old_hpux
typedef struct {
    char	*name;
    char	*aliases;
} LocaleAlias;

typedef struct {
    char	type;
    char	*locales;
} XhpLocale;

typedef struct {
    char	*confDir;	/* override /etc/vue/config */
    char	*userDir;	/* override .vue */
    char	*uselite;	/* $HOME/.vue/sessions/usevuelite */
    char	*resPath;	/* path of session resource (colon separated) */
    char	*litePath;
} VueEnv;
# endif	/* old_hpux */

typedef struct {
    char	*confDir;	/* override /usr/dt/config */
    char	*userDir;	/* override .dt */
    char	*resPath;	/* path of session resource (colon separated) */
} DtEnv;

typedef struct {
    bool	disabled;	/* disable remote execution */
    bool	useRemsh;	/* use remsh(1) instead of action */
    int		timeout;	/* timeout for remote execution */
    char	*passEnv;	/* env vars passed to remote IMS */
} RemoteEnv;

typedef struct {
    int		flags;
    int		protocols;
    int		timeout;
    int		interval;
    char	*servername;
    char	*servername2;
    char	*classname;
    char	*property;
    char	*cmd_path;
    char	*cmd_param;
    char	*env_set;
    char	*env_unset;
    char	*env_pass;
} ImsConf;

typedef struct {
    int		status;
    char	*name;
    char	*fname;
    char	*label;
    ImsConf	*ims;
} ImsEnt;

typedef struct {
    short	status;
    short	default_idx;
    short	def_selmode;
    short	num_ent;
    ImsEnt	**elist;
} ImsList;

typedef struct {
    int		status;
    int		wait_status;
    pid_t	pid;
    bool	is_remote;
    int		proto;
    char	*im_mod;
    char	*atom_name;
    char	*cmdbuf;
    Atom	prop_atom;
} RunEnv;

typedef struct {
    bool	dpy_specific;
    short	select_mode;
    short	iconic;
    char	*name;
    char	*hostname;
    char	*com_opt;
    ImsOpt	**opts;
    char	*real_fname;		/* actually read */
} FileSel;

typedef struct {
    int		flag;		/* F_SELECT_* */
    int		status;		/* error code */
    int		ims_idx;	/* index of selected ims to list->elist[] */
    int		host_type;	/* host type: HOST_{LOCAL|REMOTE|UNKNOWN} */
    int		iconic;		/* iconic mode: */
    char	*name;		/* selected ims */
    char	*hostname;	/* current host */
    ImsList	*list;		/* current list */
    ImsEnt	*ent;		/* selected ims ent */
    FileSel	*fsel;		/* original selection on file */
    RunEnv	*renv;		/* run env used on start.c & env.c */
} UserSelection;

typedef struct {
    char	*hostname;
    char	*username;
    char	*locale;
    char	*real_locale;
    char        *CDE_locale;
    char	**locale_aliases;
    char	*homedir;
    char	*displayname;
    char	*displaydir;
    char	*xmodifiers;
    short	screen_num;
} UserEnv;

typedef struct {
    char	*DisplayName;	/* override $DISPLAY */
    char	*LocaleName;	/* override $LANG */
    char	*ConfPath;	/* override $CONF/start.conf */
    char	*UserPath;	/* override $HOME/.xims/<locale> */
    char	*LogPath;	/* override $HOME/.xims/ximslog */
    char	*ResourceFile;	/* resource file for IMS */
    char	*ImsName;	/* IMS name */
    char	*ImsOption;	/* IMS option */
    char	*HostName;	/* remote host name */
    char	*ShellName;	/* shell name */
    int		Timeout;	/* wait timeout (sec) */
    int		Interval;	/* check interval (msec) */
    int		SelectMode;	/* SEL_MODE_* */
    int         Listname;       /* Used only from dtstyle */
} CmdOpt;

typedef struct {
    char	*imsConfDir;	/* [%I] /usr/dt/config/ims/ */
    char	*imsAppDir;	/* [%R] /usr/dt/app-defaults/ */
    char	*imsLogDir;	/* [%G] /var/dt/ */
    char	*imsLogFile;	/* [%g] imslog */
    char	*imsDir;	/* [%b] ims */
    char	*userImsDir;	/* [%S] $HOME/.dt/ims/ */
    char	*userTmpDir;	/* [%T] $HOME/.dt/tmp/ */
    char	*userAltDir;	/* [%A] alt tmp /var/tmp/ */
    char	*xmod[NUM_PROTOCOLS];	/* format of XMODIFIERS */
    char	*atom[NUM_PROTOCOLS];	/* atom name to be examined */
    char	*action[NUM_ACTIONS];	/* actions internally used */
    DtEnv	*dt;
    RemoteEnv	*remote;
# ifdef	old_hpux
    VueEnv	*vue;
    XhpLocale	*xhp;
    LocaleAlias	**alias;
# endif	/* old_hpux */
} CmdConf;

typedef struct {
    int		status;
    Display	*Dpy;
    Widget	TopW;
    Window	atom_owner;
    Atom	atom_main;
    Atom	atom_status;
    Atom	atom_data;
    XtAppContext	appC;
} WinEnv;

    /* Global Variables */

extern char		*ProgramRevision;	/* on main.c */
extern Display		*Dpy;		/* display (on win.c) */
extern Widget		TopW;		/* toplevel-shell (on win.c) */
extern XtAppContext	appC;

# ifdef	_EXTERN_DEFINE_
#define	Extern
# else
#define	Extern	extern
# endif	/* _EXTERN_DEFINE_ */

Extern OpStateVal	OpState;
Extern int		OpMode;		/* one of MODE_* */
Extern int		OpFlag;		/* bitOR of FLAG_* */
Extern CmdOpt		Opt;		/* command line options */
Extern CmdConf		Conf;
Extern WinEnv		winEnv;
Extern UserEnv		userEnv;
Extern UserSelection	userSel;
Extern ImsList		*localList;

Extern char		*ProgramName;	/* invoked program name */
Extern int		ProgramType;	/* same as OpMode */
Extern int		Verbose;	/* verbose flag */
Extern int		DebugLvl;	/* debug flag */
Extern FILE		*LogFp;		/* log file */
Extern int		Wargc;		/* argc for XtAppInitialize */
Extern char		**Wargv;	/* argv for XtAppInitialize */
Extern bool		WaitingDialogReply;
Extern bool		WaitingActionDone;
Extern bool		UseMsgWindow;
Extern bool		IsRemote;
Extern int		LastErrMsg;

#undef	Extern

# ifdef	old_hpux
#define	isXsession()	((OpFlag & (FLAG_VUE|FLAG_DT)) ? True : False)
#define	isVUE()		((OpFlag & FLAG_VUE) ? True : False)
# else
#define	isXsession()	((OpFlag & FLAG_DT) ? True : False)
#define	isVUE()		(False)
# endif	/* old_hpux */
#define	isDT()		((OpFlag & FLAG_DT) ? True : False)
#define	useWINDOW()	\
		(((OpFlag & USE_WINDOW_MASK) || window_env_ok()) ? True : False)

#define	RemoteOn()	\
	(((OpFlag & FLAG_NOREMOTE) || Conf.remote->disabled) ? False : True)

#include	"ximsfunc.h"

#endif	/* _XIMS_H_ */
