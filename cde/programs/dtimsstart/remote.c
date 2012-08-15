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
/* @(#)$TOG: remote.c /main/9 1998/04/06 13:36:26 mgreess $ */

#include	"xims.h"

#include	<signal.h>
#include	<sys/wait.h>
#include	<netdb.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<sys/socket.h>
#include	<sys/errno.h>
#include	<setjmp.h>
#include	<fcntl.h>
#include	<limits.h>
#include	<X11/Xproto.h>		/* for X_ChangeHosts */
#include	<X11/Xatom.h>		/* for XA_STRING */

#if !defined(linux) && !defined(__FreeBSD__)
extern char	*sys_errlist[];
#endif

static char	*conf_msg_id = STR_CONFDATA;

#define	CONF_MSG_ID_LEN		strlen(conf_msg_id)
#define	CONF_MSG_DATASIZE_LEN	8
#define	CONF_MSG_HEADER_LEN	(CONF_MSG_ID_LEN + CONF_MSG_DATASIZE_LEN)
#define	CONF_MSG_SIZE_MAX	(8*1024 - 64)	/* < PIPE_BUF (PIPSIZ) */

static int	mk_remote_conf(/* list, locale, ims_name, status, confbuf, conflen */);
static char	*mk_ims_ent(/* bp, idx, ent */);
static int	parse_ims_list(/* ptr, list */);
static int	parse_remote_conf(/* listp, locale, confbuf, conflen */);
static int	prepare_action(/* act_typ, av, ac */);
static int	read_property(/* prop, type, format, remove, datapp, lenp */);

#if	0	/* README */

Atom:
    _DTIMSSTART_MAIN  _DTIMSSTART_STATUS  _DTIMSSTART_DATA

Property:
    _DTIMSSTART_STATUS (format: 32)	_DTIMSSTART_DATA (format: 8)

ClientMessage:
    message_type:	_DTIMSSTART_STATUS
    format:		32
    data.l[0]:		_REMOTE_CONF or _REMOTE_RUN
    data.l[1]:		ErrorCode
    data.l[2-4]:	0 (unused)

Status:
    _NONE  _INIT  _REMOTE_CONF  _REMOTE_RUN

Actions:
    DtImsGetRemoteConf  DtImsRunRemoteIms

Procedure:
    
  <initialization>
    * own _MAIN property
    * set _STATUS property to _INIT
    * set _DATA property to _INIT

  <get remote configuratuon (DtImsGetRemoteConf)>
    * clear _DATA property
    * change _STATUS property to _REMOTE_CONF
    * set additional command line options to _DATA property
    * invoke DtImsGetRemoteConf action

      [on remote dtimsstart]
	- collect data
	- check _STATUS property whether its value is _REMOTE_CONF
	- set collected data to _DATA property of owner of _MAIN (Replace)
	- send ClientMessage of ErrorCode to owner of _MAIN
	- exit

    * receive ClientMessage from remote dtimsstart (or action finished)
    * change _STATUS property to _INIT
    * read data from _DATA property (Delete)
    * parse ErrCode and data (conf data or ErrorPath)
    * update selection window

  <run IMS on remote host (DtImsRunRemoteIms)>
    * change _STATUS property to _REMOTE_RUN
    * set additional command line options to _DATA property
    * invoke DtImsRunRemoteIms action
    * popdown selection window (if sucessfully invoked)

      [on remote dtimsstart]
	- check _STATUS property whether its value is _REMOTE_RUN
	- invoke IMS and wait its preparation
	- set ErrorPath to _DATA property of owner of _MAIN (Replace)
	- send ClientMessage of ErrorCode to owner of _MAIN
	- exit

    * receive ClientMessage from remote dtimsstart (or action finished)
    * change _STATUS property to _INIT
    * parse ErrCode and data (ErrorPath)

  <termination>
    * disown _MAIN property
    * exit

#endif	/* README */


#ifdef NEED_STRCASECMP
/*
 * In case strcasecmp is not provided by the system here is one
 * which does the trick.
 */
static int
strcasecmp(register const char *s1,
	   register const char *s2)
{
    register int c1, c2;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}
#endif

int	put_remote_conf(locale, ims_name)
    char	*locale, *ims_name;
{
    int		ret;
    int		msg_status = NoError;
    int		conflen;
    char	confbuf[CONF_MSG_SIZE_MAX];
    ImsList	*list = (ImsList *) 0;

    DPR(("put_remote_conf(locale=%s, ims=%s)\n", locale, ims_name));

    ret = get_ims_list(&list, NULL, True);
    msg_status = ret;

    ret = mk_remote_conf(list, locale, ims_name, msg_status, confbuf, &conflen);
    if (list) {
	clear_ImsList(list);
	FREE(list);
    }

    ret = set_remote_confdata(confbuf, conflen);

    send_dtims_msg(WIN_ST_REMOTE_CONF, ret);

#if	0
    NotifyErrCode(NoError);
    (void) fwrite((void *)confbuf, (size_t)conflen, (size_t)1, stdout);
    fflush(stdout);
#endif

    return ret;
}

int	get_remote_conf(listp, hostname, locale, ims_name)
    ImsList	**listp;
    char	*hostname, *locale, *ims_name;
{
    int		ret = NoError;
    int		conflen = 0;
    char	*confbuf = 0;
    char	*opts[16];
    int		n, num_opts;
    char        *CDE_locale = NULL;

    if (!locale)
	locale = userEnv.real_locale ? userEnv.real_locale : userEnv.locale;

    CDE_locale = userEnv.CDE_locale;

    if (CDE_locale)
	DPR2(("get_remote_conf(%s, %s, %s)\n",
			hostname, CDE_locale, ims_name ? ims_name : "<all>"));
    else
	DPR2(("get_remote_conf(%s, %s, %s)\n",
			hostname, locale, ims_name ? ims_name : "<all>"));

    n = 0;
    /* Try to first use the CDE locale, else fallback to the locale. */
    if (CDE_locale) {
	opts[n++] = "-CDE_locale";
        opts[n++] = CDE_locale;
    }
    else {
	if (locale) { 
	    opts[n++] = "-locale"; 
	    opts[n++] = locale; 
	}
    }

    if (ims_name) { 
	opts[n++] = "-ims"; 
	opts[n++] = ims_name; 
    }
#ifdef	DEBUG
    if (DebugLvl >= 1) {
	int i;
	for (i = 0; i < DebugLvl; i++)	opts[n++] = "-debug";
    }
#endif
    opts[n] = NULL;
    num_opts = n;

    ret = prepare_action(ACT_GETREMCONF, opts, num_opts);
    if (ret != NoError) return ret;

    ret = invoke_action(Conf.action[ACT_GETREMCONF], hostname);
    change_window_status(WIN_ST_INIT);

    if (ret != NoError)	return ret;

    ret = read_remote_confdata(&confbuf, &conflen);
    if (ret != NoError) return ret;

    if (ret == NoError) {
	ret = parse_remote_conf(listp, locale, confbuf, conflen);
	FREE(confbuf);
	if (ims_name && ret == ErrRemoteNoIms)
	    ret = ErrRemoteMissIms;
    }

    return ret;
}


#define	PUT_DATA(nm, val)	*bp++ = ' ', bp = strcpyx(bp, (nm)), \
			*bp++ = '=', bp = strcpyx(bp, (val)), *bp++ = '\n'

static int	mk_remote_conf(list, locale, ims_name, status, confbuf, conflen)
    ImsList	*list;
    int		status;
    char	*locale, *ims_name, *confbuf;
    int		*conflen;
{
    int		num_ent;
    int		i, j;
    char	*bp;
    int		len;
    int		data_sz;
    char	sz_ptr[20];
    ImsEnt	*ent;
    char        var[20];

    DPR(("mk_remote_conf(locale=%s, ims=%s)\n", locale, ims_name));

#ifdef	DEBUG
    if (DebugLvl >= 2 && list)	pr_ImsList(list);
#endif

    bp = confbuf + CONF_MSG_HEADER_LEN;

    num_ent = 0;
    if (status == NoError) {
	for (i = 0; i < list->num_ent; i++) {
	    ent = list->elist[i];
	    if ((ims_name && strcmp(ent->name, ims_name))
		|| (ent->ims && (ent->ims->flags & F_NO_REMOTE)))
		ent->status = ErrRemoteIms;
	    else
		num_ent++;
	}
	if (num_ent <= 0)
	    status = ErrRemoteNoIms;
    }

    bp = strcpyx(bp, "ImsList: "); bp = strcpyx(bp, locale); *bp++ = '\n';
    sprintf(var, "%ld", (long) status);
    PUT_DATA("ST", var);

    if (num_ent > 0) {
	sprintf(var, "%ld", (long) num_ent);
	PUT_DATA("ne", var);
	if (list->elist[list->default_idx]->status != ErrRemoteIms) {
	    PUT_DATA("df", list->elist[list->default_idx]->name);
	}
	sprintf(var, "%ld", (long) (list->def_selmode));
	PUT_DATA("sm", var);

	for (i = j = 0; i < list->num_ent; i++)
	    if (list->elist[i]->status != ErrRemoteIms)
		bp = mk_ims_ent(bp, j++, list->elist[i]);
    }
    bp = strcpyx(bp, "END"); *bp++ = '\n';

    data_sz = bp - (confbuf + CONF_MSG_HEADER_LEN);

	/* header (conf_msg_id & data_sz) */
    bp = confbuf;
    memset((void *) bp, (int) ' ', (size_t) CONF_MSG_HEADER_LEN);
    memcpy((void *) bp, conf_msg_id, CONF_MSG_ID_LEN);
    sprintf(sz_ptr, "%ld", (long) data_sz);
    len = strlen(sz_ptr);
    bp = confbuf + CONF_MSG_HEADER_LEN - 1 - len;
    memcpy((void *) bp, (void *) sz_ptr, (size_t) len);
    confbuf[CONF_MSG_HEADER_LEN - 1] = '\n';

    *conflen = CONF_MSG_HEADER_LEN + data_sz;

    DPR2(("mk_remote_conf(): conflen=%d data_sz=%d\n confbuf=%s",
						*conflen, data_sz, confbuf));

    return NoError;
}

static char	*mk_ims_ent(bp, idx, ent)
    ImsEnt	*ent;
    int		idx;
    register char	*bp;
{
    ImsConf	*ims = ent->ims;
    char        val[20];

    sprintf(val, "%ld", (long) idx);
    bp = strcpyx(bp, "Ent-"); bp = strcpyx(bp, val); *bp++ = '\n';    
    PUT_DATA("nm", ent->name);
    sprintf(val, "%ld", (long) ent->status);
    PUT_DATA("st", val);
    sprintf(val, "%ld", (long) ims->flags);
    PUT_DATA("fg", val);
    sprintf(val, "%ld", (long) ims->protocols); 
    PUT_DATA("pr", val);
    if (ent->label)		{ PUT_DATA("lb", ent->label); }
    if (ims->timeout)		
      { 
	sprintf(val, "%ld", (long) ims->timeout);
	PUT_DATA("to", val); 
       }
    if (ims->interval)
      { 
	sprintf(val, "%ld", (long) ims->interval);
	PUT_DATA("it", val); 
      }
    if (ims->servername)	{ PUT_DATA("sn", ims->servername); }
    if (ims->servername2)	{ PUT_DATA("sN", ims->servername2); }
    if (ims->classname)		{ PUT_DATA("cn", ims->classname); }
    if (ims->property)		{ PUT_DATA("pp", ims->property); }
    if (ims->cmd_path)		{ PUT_DATA("cp", ims->cmd_path); }
    if (ims->cmd_param)		{ PUT_DATA("cr", ims->cmd_param); }
    if (ims->env_set)		{ PUT_DATA("es", ims->env_set); }
    if (ims->env_unset)		{ PUT_DATA("eu", ims->env_unset); }
    if (ims->env_pass)		{ PUT_DATA("ep", ims->env_pass); }

    return bp;
}

#undef	PUT_DATA

static int	parse_ims_list(ptr, list)
    char	*ptr;
    ImsList	*list;
{
    register char	*bp = ptr;
    char	*np, *vp;
    char	*def_name;
    int		i, num_ent;
    ImsEnt	*ent = 0;
    ImsConf	*ims;

    CLR(list, ImsList);
    list->default_idx = -1;
    list->def_selmode = SEL_MODE_NOAUTO;
    list->num_ent = 0;

    def_name = NULL;
    num_ent = 0;
    while (np = strchr(bp, '\n')) {
	if (np == bp) {
	    bp++;
	    continue;
	}
	*np = 0;
	if (strncmp(bp, "Ent-", 4) == 0) {
#ifdef	DEBUG
	    if (list->num_ent <= 0) {
		DPR(("parse_ims_list(): ImsEnt: list->num_ent=%d\n", list->num_ent));
	    }
#endif
	    if (!list->elist)
		list->elist = ALLOC(list->num_ent, ImsEnt *);
	    if (num_ent >= list->num_ent) {
		DPR(("parse_ims_list(): too many entry: '%s'\n", bp));
		break;
	    }
	    ent = list->elist[num_ent] = ALLOC(1, ImsEnt);
	    ims = ent->ims = ALLOC(1, ImsConf);
	    num_ent++;
	} else if (strncmp(bp, "END", 3) == 0) {
	    break;
	} else if (bp[0] == ' ' && bp[3] == '=') {
	    bp++; vp = bp + 3;
		/*list */
	    if (strncmp(bp, "ST", 2) == 0)	list->status = atoi(vp);
	    else if (strncmp(bp, "ne", 2) == 0)	list->num_ent = atoi(vp);
	    else if (strncmp(bp, "df", 2) == 0)	def_name = vp;
	    else if (strncmp(bp, "sm", 2) == 0)	list->def_selmode = atoi(vp);
#ifdef	DEBUG
	    else if (!ent) {
		DPR(("parse_ims_list(): ImsEnt: list->elist[%d]=%%x\n", num_ent, ent));
	    }
#endif
		/* ent */
	    else if (strncmp(bp, "nm", 2) == 0)	{ RENEWSTR(ent->name, vp); }
	    else if (strncmp(bp, "lb", 2) == 0)	{ RENEWSTR(ent->label, vp); }
	    else if (strncmp(bp, "st", 2) == 0)	{ ent->status = atoi(vp); }
	    else if (strncmp(bp, "fg", 2) == 0)	{ ims->flags = atoi(vp); }
	    else if (strncmp(bp, "pr", 2) == 0)	{ ims->protocols = atoi(vp); }
	    else if (strncmp(bp, "to", 2) == 0)	{ ims->timeout = atoi(vp); }
	    else if (strncmp(bp, "it", 2) == 0)	{ ims->interval = atoi(vp); }
	    else if (strncmp(bp, "sn", 2) == 0)	{ RENEWSTR(ims->servername, vp); }
	    else if (strncmp(bp, "sN", 2) == 0)	{ RENEWSTR(ims->servername2, vp); }
	    else if (strncmp(bp, "cn", 2) == 0)	{ RENEWSTR(ims->classname, vp); }
	    else if (strncmp(bp, "pp", 2) == 0)	{ RENEWSTR(ims->property, vp); }
	    else if (strncmp(bp, "cp", 2) == 0)	{ RENEWSTR(ims->cmd_path, vp); }
	    else if (strncmp(bp, "cr", 2) == 0)	{ RENEWSTR(ims->cmd_param, vp); }
	    else if (strncmp(bp, "es", 2) == 0)	{ RENEWSTR(ims->env_set, vp); }
	    else if (strncmp(bp, "eu", 2) == 0)	{ RENEWSTR(ims->env_unset, vp); }
	    else if (strncmp(bp, "ep", 2) == 0)	{ RENEWSTR(ims->env_pass, vp); }
	    else {
		DPR(("parse_ims_list(): invalid line '%s'\n", bp - 1));
	    }
	} else {
	    DPR(("parse_ims_list(): invalid line '%s'\n", bp));
	}
	bp = np + 1;
    }

#ifdef	DEBUG
    if (num_ent != list->num_ent) {
	DPR(("parse_ims_list(): num_ent(%d) != list->num_ent(%d)\n",
								num_ent, list->num_ent));
    }
#endif

    list->num_ent = num_ent;
    list->default_idx = -1;
    if (num_ent > 0) {
	for (i = 0; i < num_ent; i++) {		/* check indispensable entry */
	    ent = list->elist[i];
	    if (ent->status == NoError)
		ent->status = check_ims_conf(ent->ims, ent->name);
	}

	if (def_name) {				/* set default_idx */
	    for (i = 0; i < num_ent; i++)
		if (strcmp(list->elist[i]->name, def_name) == 0) {
		    list->default_idx = i;
		    break;
		}
	}
    }

    return list->status;
}


static int	parse_remote_conf(listp, locale, confbuf, conflen)
    ImsList	**listp;
    char	*locale, *confbuf;
    int		conflen;
{
    int		ret = NoError;
    char 	*bp = confbuf;
    ImsList	*list;
    int		data_sz = 0;

    DPR(("parse_remote_conf(%s)\n", locale));

    if (conflen < (int) CONF_MSG_HEADER_LEN		/* check header */
		|| strncmp(confbuf, conf_msg_id, CONF_MSG_ID_LEN)) {
	    ret = ErrNoImsstart;
    } else {
	confbuf[CONF_MSG_HEADER_LEN - 1] = 0;	/* <= '\n' */
	bp = confbuf + CONF_MSG_ID_LEN;
	while (*bp == ' ')	bp++;
	if (!str_to_int(bp, &data_sz) || data_sz < 0) {
	    ret = ErrNoImsstart;
	} else if (conflen < data_sz + (int) CONF_MSG_HEADER_LEN) {
	    DPR(("\tconflen(%d) != data_sz(%d) + HDR\n", conflen, data_sz));
	    data_sz =  conflen - CONF_MSG_HEADER_LEN;
	    ret = ErrNoImsstart;
	}
    }

#ifdef	DEBUG
    if (ret != NoError && conflen > (int) CONF_MSG_HEADER_LEN) {
	if (!confbuf[CONF_MSG_HEADER_LEN - 1])
	    confbuf[CONF_MSG_HEADER_LEN - 1] = '@';
	confbuf[conflen] = 0;
	DPR(("\tinvalid header[len=%d]: %s\n", conflen, confbuf));
    }
#endif
    if (ret != NoError)	return ErrRemoteAction;

    bp = confbuf + CONF_MSG_HEADER_LEN;
    bp[data_sz] = 0;
    if (strncmp(bp, "ImsList:", 8)
    /*		|| strncmp(bp + 9, locale, strlen(locale)) */
		|| !(bp = strchr(bp, '\n'))) {
	return ErrRemoteAction;
    }
    /* confbuf[conflen] = 0; */

    list = ALLOC(1, ImsList);
    ret = parse_ims_list(bp, list);

    if (ret != NoError || list->num_ent == 0) {
	clear_ImsList(list);
	FREE(list);
	list = (ImsList *) 0;
	ret = ErrRemoteNoIms;
    }
    *listp = list;

    return ret;
}


int	exec_remote_ims(sel)
    UserSelection	*sel;
{
    int		ret = NoError;
    int		n, num_opts, binc;
    char	*bp, *np;
    char	envbuf[BUFSIZ];
    char	tmpbuf[BUFSIZ];
    char	*opts[32];
    char	**av;
    int		ac;
    char	*ims_name = sel->name;
    ImsConf	*ims = sel->ent->ims;
    char        val[20];

    DPR(("exec_remote_ims(): '%s' on %s\n", ims_name, sel->hostname));

	/* build options */
    n = 0;
    bp = tmpbuf; tmpbuf[0] = 0;

    opts[n++] = "-ims"; opts[n++] = ims_name;
    opts[n++] = "-notify";
    opts[n++] = "-nosave";
    opts[n++] = "-nowindow";

#if	0
    binc = expand_string(bp, "%L", BUFSIZ, 0);
    opts[n++] = "-locale";
    opts[n++] = bp; bp += binc + 1;
    binc = expand_string(bp, "%d.%s", BUFSIZ, 0);
    opts[n++] = "-display";
    opts[n++] = bp; bp += binc = 1;
#endif

#ifdef	DEBUG
    if (DebugLvl >= 1) {
	int i;
	for (i = 0; i < DebugLvl; i++)	opts[n++] = "-debug";
    }
#endif

	/* options */
    if (OpFlag & FLAG_NOWAIT)		opts[n++] = "-nowait";
    if (OpFlag & FLAG_NOTIMEOUT)	opts[n++] = "-notimeout";
    if (OpFlag & FLAG_CONNECT)		opts[n++] = "-connect";
    if (Opt.Timeout > 0) {
	sprintf(val, "%ld", (long)Opt.Timeout);
	np = strcpyx(bp, val);
	opts[n++] = "-timeout";
	opts[n++] = bp; bp = np + 1;
    }
    if (Opt.Interval > 0) {
	sprintf(val, "%ld", (long)Opt.Interval);
	np = strcpyx(bp, val);
	opts[n++] = "-interval";
	opts[n++] = bp; bp = np + 1;
    }
    if (mk_ims_option(bp, sel)) {
	sprintf(val, "%ld", (long)Opt.Interval);
	np = strcpyx(bp, val);
	opts[n++] = "-imsopt";
	opts[n++] = bp; bp = np + 1;
    }
    bp = NULL;
    opts[n] = NULL;
    num_opts = n;

	/* env variables */
    set_remote_env(envbuf, ims->env_pass);

    ret = prepare_action(ACT_RUNREMIMS, opts, num_opts);
    if (ret != NoError) return ret;

    ret = invoke_action(Conf.action[ACT_RUNREMIMS], sel->hostname);
    change_window_status(WIN_ST_INIT);

    if (ret != NoError)	return ret;

    if (ret == NoError) {
	ac = 0; av = NULL;
	ret = get_window_data(&ac, &av);
	ret = NoError;

	if (ret != NoError) return ret;
    }

    if (ret == NoError) {
	put_xims_log("'%s' started for %s on %s.",
				sel->name, userEnv.displayname, sel->hostname);
    }

    DPR2(("exec_remote_ims(): ret=%s[%d]\n", error_name(ret), ret));

    return ret;
}


int	check_hostname(hostname)
    char	*hostname;
{
    int		host_type = HOST_UNKNOWN;
    char	*local = userEnv.hostname;
    struct hostent	*hp;
    unsigned long 	addr = 0L;
    static unsigned long 	local_addr = 0L;

    if (!hostname || !*hostname || strcasecmp(hostname, "local") == 0
			|| strcasecmp(hostname, userEnv.hostname) == 0) {
	host_type =  HOST_LOCAL;
    } else {		/* compare inet address */
	if (!local_addr) {
	    if ((hp = gethostbyname(local)) && hp->h_addrtype == AF_INET) {
		local_addr = *((unsigned long *) hp->h_addr_list[0]);
	    } else {
		DPR(("check_hostname(%s)\tgethostbyname() failed\n", local));
		host_type = HOST_REMOTE;
	    }
	}
	if (host_type == HOST_UNKNOWN) {
	    if ((hp = gethostbyname(hostname)) && hp->h_addrtype == AF_INET) {
		addr = *((unsigned long *) hp->h_addr_list[0]);
		if (addr == local_addr)
		    host_type = HOST_LOCAL;
		else
		    host_type = HOST_REMOTE;
	    } else {
		DPR(("check_hostname(%s)\tunknown\n", hostname));
		host_type = HOST_UNKNOWN;
	    }
	}
    }

    DPR(("check_hostname(%s): [%s]  addr=%#x, local=%#x\n", hostname,
			host_type == HOST_LOCAL ? "LOCAL" :
			(host_type == HOST_REMOTE ? "REMOTE" : "UNKNOWN"),
			addr, local_addr));

    return host_type;
}



int	set_remote_confdata(confbuf, conflen)
    char *confbuf;
    int conflen;
{
    char *av[2];

    av[0] = confbuf; av[1] = NULL;

    return set_window_data(1, av);
}

int	read_remote_confdata(confbuf, conflen)
    char **confbuf;
    int *conflen;
{
    char **av = NULL;
    int ac = 0;
    int ret;

    ret = get_window_data(&ac, &av);
    /* if (ac != 1) {  FREE  av[i]; return ErrBabData; } */

    *confbuf = av[0];
    *conflen = strlen(av[0]);

    return NoError;
}


static int	prepare_action(act_typ, av, ac)
    int	act_typ;
    char	**av;
    int		ac;
{
    int ret;

    ret = init_window_env();
    if (ret != NoError)	return ret;

    switch (act_typ) {
	case ACT_GETREMCONF:
		change_window_status(WIN_ST_REMOTE_CONF);
		break;

	case ACT_RUNREMIMS:
		change_window_status(WIN_ST_REMOTE_RUN);
		break;

	default:	return ErrInternal;
    }

    ret = set_window_data(ac, av);

    return NoError;
}


int	get_window_status()
{
    long	*datap;
    int		len = 0;
    int		win_st;

    if (winEnv.atom_status == None || winEnv.atom_owner == None)
	return WIN_ST_NONE;

    if (winEnv.atom_owner == None)	return WIN_ST_NONE;
#if	0
    if (winEnv.atom_owner == XtWindow(winEnv.TopW))
	return winEnv.status;
#endif

    win_st = WIN_ST_NONE;
    if (read_property(winEnv.atom_status, XA_INTEGER, 32, False,
	    (char **)&datap, &len) == True && len > 0) {
	    win_st = datap[0];
	    FREE(datap);
    }
    return win_st;
}

int	change_window_status(status)
    int	status;
{
    if (winEnv.atom_status == None || winEnv.atom_owner == None)
	return ErrInternal;

    winEnv.status = status;

    (void)XChangeProperty(winEnv.Dpy, winEnv.atom_owner,
		winEnv.atom_status, XA_INTEGER,
		32, PropModeReplace, (unsigned char *)&status, 1);

    XSync(winEnv.Dpy, False);

    DPR(("change_window_status(): new status=%d\n", status));
    return NoError;
}

int	set_window_data(ac, av)
    int	ac;
    char **av;
{
    register int i;
    register int nbytes;
    register char *buf, *bp;

    if (winEnv.atom_data == None || winEnv.atom_owner == None)
	return ErrInternal;

#ifdef	DEBUG
    if (DebugLvl >= 1) {
    	int i;
	printf("set_window_data() av[%d] = { ", ac);
	for (i = 0; i < ac; i++)	
	    printf("\"%s\", ", av[i]);
	printf("}\n");
    }
#endif

    for (i = 0, nbytes = 1; i < ac; i++)
	nbytes += strlen(av[i]) + 1;
    if (bp = buf = XtMalloc(nbytes)) {	/* copy args into single buffer */
	for (i = 0; i < ac; i++) {
	    if (av[i]) {
		(void) strcpy(bp, av[i]);
		bp += strlen(av[i]) + 1;
	    } else
		*bp++ = '\0';
	}
	(void)XChangeProperty(winEnv.Dpy, winEnv.atom_owner,
			winEnv.atom_data, XA_STRING, 8,
			PropModeReplace, (unsigned char *)buf, nbytes);
	XSync(winEnv.Dpy, False);
	XtFree(buf);
    }
    DPR(("set_window_data(): len=%d data=\"%s\"\n", nbytes, buf));

    return NoError;
}


int	get_window_data(acp, avp)
    int	*acp;
    char ***avp;
{
    int ac;
    char *data;
    char **av;
    int len = 0;
    int	i, j;

    if (winEnv.atom_data == None || winEnv.atom_owner == None)
	return ErrInternal;

    if (read_property(winEnv.atom_data, XA_STRING, 8, True, &data, &len) != True) {
	*acp = 0;
	*avp = NULL;
	return ErrRemoteData;
    }

    ac = 0; av = NULL;
    if (len > 0) {
	for (i = 1; i < len - 1; i++)	if (data[i] == '\0')	ac++;
	av = (char **) ALLOC(ac + 1, char *);

	j = 0;
	if (ac == 1) {
	    av[j++] = data;
	} else {
	    av[j++] = NEWSTR(data);
	    for (i = 1; i < len - 1; i++)
		if (data[i] == '\0') {
		    av[j++] = NEWSTR(data + i + 1);
		}
	    FREE(data);
	}
	av[j] = NULL;
    }

#ifdef	DEBUG
    if (DebugLvl >= 2) {
	int i;
	printf("get_window_data() av[%d] = { ", ac);
	for (i = 0; i < ac; i++)	
	    printf("\"%s\", ", av[i]);
	printf("}\n");
    }
#endif

    *acp = ac;
    *avp = av;

    return NoError;
}


static int	read_property(prop, type, format, del_flag, datapp, lenp)
    Atom prop;
    Atom type;
    int format;
    int	del_flag;
    unsigned char **datapp;
    unsigned long *lenp;
{
    Atom realtype;
    int realformat;
    unsigned long bytesafter;

    *datapp = NULL;

    (void)XGetWindowProperty(winEnv.Dpy, winEnv.atom_owner,
			     prop, 0L, 1000000L, del_flag, type,
			     &realtype, &realformat, lenp,
			     &bytesafter, datapp);

    if (realtype == None) {	
	return False;
    } else if (realtype != type) {	/* wrong type */
	return False;
    } else if (realformat != format) {	/* wrong format */
	if (*datapp != NULL) XtFree((char *)*datapp);
	*datapp = NULL;
	return False;
    }
    return True;
}

