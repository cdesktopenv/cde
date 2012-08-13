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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$TOG: TermPrimSetPty.c /main/2 1998/04/03 17:11:24 mgreess $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include "TermPrimOSDepI.h"
#include "TermPrimDebug.h"
#include <fcntl.h>
#ifdef  ALPHA_ARCHITECTURE
#include <sys/ioctl.h>
#include <sys/ttydev.h>
#endif /* ALPHA_ARCHITECTURE */
#include <termios.h>
#ifdef	USE_PTYS
#ifdef	HP_ARCHITECTURE
#include <sys/ptyio.h>
#endif	/* HP_ARCHITECTURE */
#endif	/* USE_PTYS */
#if	defined(HP_ARCHITECTURE) && !(OSMAJORVERSION > 9)
#include <bsdtty.h>
#endif	/* defined(HP_ARCHITECTURE) && !(OSMAJORVERSION > 9) */

#if defined (USE_SETCSMAP)
#include <langinfo.h>
#include <sys/param.h>
#endif   /* (USE_SETCSMAP) */

#ifdef	USE_SRIOCSREDIR
#include <sys/strredir.h>
#include <sys/stat.h>
#endif	/* USE_SRIOCSREDIR */

#ifdef	USE_TIOCCONS
#ifdef  IBM_ARCHITECTURE
#include <sys/ioctl.h>
#endif  /* IBM_ARCHITECTURE */
#include <sys/stat.h>
#endif	/* USE_TIOCCONS */

#ifdef  LINUX_ARCHITECTURE
#include <sys/ioctl.h>
#endif /* LINUX_ARCHITECTURE */

#ifdef	USE_STREAMS
#include <sys/types.h>
#include <stropts.h>
#include <sys/conf.h>
#endif	/* USE_STREAMS */

#if defined (USE_CSWIDTH)
#include <sys/ioctl.h>
#include <sys/eucioctl.h>
#endif   /* (USE_CSWIDTH) */

#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <Xm/Xm.h>

#define XTTYMODE_intr    0
#define XTTYMODE_quit    1
#define XTTYMODE_erase   2
#define XTTYMODE_kill    3
#define XTTYMODE_eof     4
#define XTTYMODE_eol     5
#define XTTYMODE_swtch   6
#define XTTYMODE_start   7
#define XTTYMODE_stop    8
#define XTTYMODE_brk     9
#define XTTYMODE_susp   10
#define XTTYMODE_dsusp  11
#define XTTYMODE_rprnt  12
#define XTTYMODE_flush  13
#define XTTYMODE_weras  14
#define XTTYMODE_lnext  15
#define NXTTYMODES      16

#if defined(CSRG_BASED)
#define TAB3	0x00000000
#define NLDLY	0x00000000
#define CRDLY	0x00000000
#define TABDLY	0x00000000
#define BSDLY	0x00000000
#define VTDLY	0x00000000
#define FFDLY	0x00000000
#define CBAUD	0x00000000
#endif
typedef struct _ttyMode
{
    char    *name;
    int      len;
    int      set;
    char     value;
}
ttyMode;

ttyMode _DtTermPrimTtyModeList[] = 
{
    { "intr" , 4, 0, '\0' }, /* tchars.t_intrc ; VINTR   */
    { "quit" , 4, 0, '\0' }, /* tchars.t_quitc ; VQUIT   */
    { "erase", 5, 0, '\0' }, /* sgttyb.sg_erase ; VERASE */
    { "kill" , 4, 0, '\0' }, /* sgttyb.sg_kill ; VKILL   */
    { "eof"  , 3, 0, '\0' }, /* tchars.t_eofc ; VEOF     */
    { "eol"  , 3, 0, '\0' }, /* VEOL                     */
    { "swtch", 5, 0, '\0' }, /* VSWTCH                   */
    { "start", 5, 0, '\0' }, /* tchars.t_startc          */
    { "stop" , 4, 0, '\0' }, /* tchars.t_stopc           */
    { "brk"  , 3, 0, '\0' }, /* tchars.t_brkc            */
    { "susp" , 4, 0, '\0' }, /* ltchars.t_suspc          */
    { "dsusp", 5, 0, '\0' }, /* ltchars.t_dsuspc         */
    { "rprnt", 5, 0, '\0' }, /* ltchars.t_rprntc         */
    { "flush", 5, 0, '\0' }, /* ltchars.t_flushc         */
    { "weras", 5, 0, '\0' }, /* ltchars.t_werasc         */
    { "lnext", 5, 0, '\0' }, /* ltchars.t_lnextc         */
    {    NULL, 0, 0, '\0' }, /* NULL terminate the array */
};

static int
parseTtyModes
(
    char    *modeString,
    ttyMode *modeList
)
{
    ttyMode    *pMode;
    int         c, i;
    int         modeCount = 0;

    /*
    ** Search to the end of the.
    */

    while (1) {
        /*
        ** Skip white space,  if this is the end of the list,
        ** return.
        */
        while (*modeString && isascii(*modeString) && isspace(*modeString))
        {
             modeString++;
        }

        if (!*modeString) 
        {
	    DebugF('p', 2, fprintf(stderr,
		    ">>parseTtyModes() hit end of mode string, return=%d\n",
		    modeCount));
            return(modeCount);
        }

        /*
        ** Otherwise, see if 'modeString' is in the list of mode names.
        */
        for (pMode = modeList; pMode->name; pMode++)
        {
	    DebugF('p', 2, fprintf(stderr,
		    ">>parseTtyModes() comparing %.*s to %s\n",
		    pMode->len, modeString, pMode->name));
            if (strncmp(modeString, pMode->name, pMode->len) == 0)
            {
		DebugF('p', 2, fprintf(stderr, ">>parseTtyModes() match!\n"));
                break;
            }
        }
        if (!pMode->name) 
        {
	    DebugF('p', 2, fprintf(stderr, ">>parseTtyModes() no match\n"));
            return(-1);
        }

        /*
        ** Now look for a value for the setting.
        ** (Skip white space, return an error if no value.)
        */
        modeString += pMode->len;
        while (*modeString && isascii(*modeString) && isspace(*modeString))
        {
            modeString++;
        }
        if (!*modeString)
        {
	    DebugF('p', 2, fprintf(stderr,
		    ">>parseTtyModes() missing value\n"));
            return(-1);
        }
        /*
        ** Make sure we handle control characters correctly.
        */
        if (*modeString == '^')
        {
            modeString++;
            /*
            ** keep control bits
            */
            c = ((*modeString == '?') ? 0177 : *modeString & 31);
        } 
        else
        {
            c = *modeString;
        }
        /*
        ** Set the values, and get go back for more.
        */
        pMode->value = c;
        pMode->set   = 1;
        modeCount++;
        modeString++;
    }
    /* return(modeCount); */
}

#if defined (USE_CSWIDTH)
#define FIND_NUMBER(x, cp) \
{ \
    x = 0; \
    while (*cp && isdigit(*cp)) \
    { \
        x *= 10; \
	x += (*cp - '0'); \
	cp++; \
    } \
}

/*
** parse the cswidth string
** it should be in the form: X1[[:Y1][,X2[:Y2][,X3[:Y3]]]]
*/
static void
parseCSWidth
(
    char *cp,
    eucioc_t *wp
)
{
    int x;
    int i;
    
    /*
    ** set all cs widths to 0
    */
    wp->eucw[1] = wp->eucw[2] = wp->eucw[3] = 0;
    wp->scrw[1] = wp->scrw[2] = wp->scrw[3] = 0;
    if (!cp)
    {
	return;
    }

    DebugF('p', 4, fprintf(stderr,
	    ">>parseCSWidth(): csWidthString %s\n", cp));

    for (i = 1; i <= 3; i++)
    {
	/*
	** read Xn
	*/
	FIND_NUMBER(x, cp);
	wp->eucw[i] = x;
	if (!*cp)
	{
	    wp->scrw[i] = wp->eucw[i];
	    return;
	}

	/*
	** Yn might exist
	*/
	if (*cp == ':')
	{
	    cp++;
	    FIND_NUMBER(x, cp);
	    wp->scrw[i] = x;
	    if (!*cp)
	    {
		return;
	    }
	}
	else
	{
	    wp->scrw[i] = wp->eucw[i];
	}
	cp++;
    }
    DebugF('p', 4, fprintf(stderr,"parseCSWidth():"));
    DebugF('p', 4, fprintf(stderr,
	    "    eucw[1] : %d, scrw[1] : %d\n", wp->eucw[1], wp->scrw[1]));
    DebugF('p', 4, fprintf(stderr,
	    "    eucw[2] : %d, scrw[2] : %d\n", wp->eucw[2], wp->scrw[2]));
    DebugF('p', 4, fprintf(stderr,
	    "    eucw[3] : %d, scrw[3] : %d\n", wp->eucw[3], wp->scrw[3]));
}
#endif   /* (USE_CSWIDTH) */

void _DtTermPrimPtySendBreak(int pty, int msec)
{
#if	defined(USE_TIOCBREAK)
    (void) ioctl(pty, TIOCBREAK, 0);
#elif	defined(USE_TCSBRK)
    (void) ioctl(pty, TCSBRK, 0);
#elif	defined(USE_TCSENDBREAK)
    (void) tcsendbreak(pty, 0);
#else	/* none specified... */
    There is no RS232 break code specified for this architecture.  See
    TermPrimOSDepI.h for a list of #defines...
#endif	/* rs232 break definition... */
}

void _DtTermPrimPtySetWindowSize(int pty, short pixelWidth, short pixelHeight,
	short characterRows, short characterColumns)
{
    struct winsize ws;

    ws.ws_row = characterRows;
    ws.ws_col = characterColumns;
    ws.ws_xpixel = pixelWidth;
    ws.ws_ypixel = pixelHeight;
    (void) ioctl(pty, TIOCSWINSZ, &ws);
}

static struct termios refTio;
static int refValid = 0;

#if defined (USE_CSWIDTH)
/*
** default width settings for ldterm
*/
static eucioc_t refWp;
#endif   /* (USE_CSWIDTH) */

void
_DtTermPrimPtyGetDefaultModes()
{
    int tty = -1;
    int refTty = -1;

#if defined (USE_CSWIDTH)
    struct strioctl i_str;
#endif   /* (USE_CSWIDTH) */

    _DtTermProcessLock();
    if (!refValid) {
	/* see if we can get a reference tty to get our base reference from...
	 */
	if ((tty = open("/dev/tty", O_RDONLY, 0)) >= 0) {
	    if (!tcgetattr(tty, &refTio)) {
		/* we got a valid reference tty... */
		DebugF('p', 3, fprintf(stderr,
			">>_DtTermPrimPtyGetDefaultModes() valid reference \"/dev/tty\"\n"));
		refTty = tty;
		refValid = 1;
	    }
	}

	if (!refValid) {
	    for (refTty = 0; refTty < 3; refTty++) {
		if (!tcgetattr(refTty, &refTio)) {
		    DebugF('p', 3, fprintf(stderr,
			    ">>_DtTermPrimPtyGetDefaultModes() valid reference \fd %d\n", refTty));
		    refValid = 1;
		    break;
		}
	    }
	}

#if defined (USE_CSWIDTH)
	if (refValid && (MB_CUR_MAX > 1))
	{
	    /*
	    ** we are in a wide character locale, get the current
	    ** width settings...
	    */
	    i_str.ic_cmd    = EUC_WGET;
	    i_str.ic_timout = 0;
	    i_str.ic_len    = sizeof(struct eucioc);
	    i_str.ic_dp     = (char *)&refWp;
	    (void)ioctl(refTty, I_STR, &i_str);
	}
#endif   /* (USE_CSWIDTH) */

	/* all done...
	 */
	/* close off the "/dev/tty" fd... */
	if (tty >= 0) {
	    (void) close(tty);
	}
    }
    _DtTermProcessUnlock();
}

void
_DtTermPrimPtyInit
(
    int   pty,
    char *modeString,
    char *csWidthString
)
{
    struct termios tio;

#if defined (USE_CSWIDTH)
    struct strioctl i_str;
    eucioc_t        wp;
#endif   /* (USE_CSWIDTH) */

#if defined (USE_SETCSMAP)
    /*
    ** set thing up so we can use setcsmap()
    ** for the time being, this is IBM specific
    */
    char path[MAXPATHLEN];
    int  oldStdin = -1;
#endif   /* (USE_SETCSMAP) */

#ifdef	NOTDEF
#ifdef	USE_STREAMS
    if (ioctl(pty, I_PUSH, "ptem") < 0) {
	(void) perror("I_PUSH ptem");
    }

    if (ioctl(pty, I_PUSH, "ldterm") < 0) {
	(void) perror("I_PUSH ldterm");
    }

    if (ioctl(pty, I_PUSH, "ttcompat") < 0) {
	(void) perror("I_PUSH ttcompat");
    }
#endif	/* USE_STREAMS */
#endif	/* NOTDEF */

    if (refValid) {
	/* we will start from the reference tty...
	 */
	/* we already got the termios structure.  No need to get again... */
	DebugF('p', 3, fprintf(stderr,
		">>_DtTermPrimPtyInit() using refTio\n"));
	tio = refTio;

#if defined (USE_CSWIDTH)
	/*
	** use the cs width information from the reference...
	*/
	wp  = refWp;
#endif   /* (USE_CSWIDTH) */
	
	/* DKS: are there any other terminal states we need to get?... */
    } else {
	/* let's set a reasonable default... */
	DebugF('p', 3, fprintf(stderr,
		">>_DtTermPrimPtyInit() generating default termio\n"));
	(void) memset(&tio, '\0', sizeof(tio));

	tio.c_iflag       = ICRNL | IXON   | IXOFF;
	tio.c_oflag       = OPOST | ONLCR  | TAB3;
	tio.c_cflag       = B9600 | CS8    | CREAD | PARENB | HUPCL;
	tio.c_lflag       = ISIG|ICANON|ECHO|ECHOE|ECHOK|IEXTEN|ECHOCTL|ECHOKE;
	/* DKS: this is termio specific.  Do we need it?...
	tio.c_line        = 0;
	*/
	tio.c_cc[VINTR]   = 0x7f;         /* DEL          */
	tio.c_cc[VQUIT]   = '\\' & 0x3f;  /* '^\'         */
	tio.c_cc[VERASE]  = '#';          /* '#'          */
	tio.c_cc[VKILL]   = '@';          /* '@'          */
	tio.c_cc[VEOF]    = 'D' & 0x3f;   /* '^D'         */
	tio.c_cc[VEOL]    = '@' & 0x3f;   /* '^@'         */
#ifdef VSWITCH
	tio.c_cc[VSWITCH] = '@' & 0x3f;   /* '^@'         */
#endif  /* VSWITCH */

#if defined (USE_CSWIDTH)
	/*
	** get the cs width information from the resource
	*/
	parseCSWidth(csWidthString, &wp);
#endif   /* (USE_CSWIDTH) */

    }

    /* now, let's clean up certain flags... */
    /* input: nl->nl, don't ignore cr, cr->nl
     *        turn on IXOFF pacing so that we can do paste without
     *        overflowing the buffer...
     */
    tio.c_iflag &= ~(INLCR | IGNCR);
    tio.c_iflag |=   ICRNL | IXOFF;

    /* output: cr->cr, nl is not return, no delays, nl->cr/nl
     */
    tio.c_oflag &= ~(OCRNL | ONLRET | NLDLY | CRDLY | TABDLY |
		     BSDLY | VTDLY  | FFDLY);
    tio.c_oflag |=   ONLCR;

    /* baud rate is 9600 (nice default), turn off clocal and turn on
     * hupcl so that the last close will SIGHUP processes running on
     * the tty...
     */
    tio.c_cflag &= ~(CBAUD | CLOCAL);
    tio.c_cflag |= B9600 | HUPCL;

    /* enable signals, canonical processing (erase, kill, etc), echo...
     */
    tio.c_lflag |= ISIG | ICANON | ECHO | IEXTEN | ECHOCTL | ECHOKE;

    /* reset EOL to the default value (ksh mucks this up sometimes)...
     */
    tio.c_cc[VEOL] = '@' & 0x3f;			/* '^@' */

    /* reset EOF to the default value (ksh and csh muck with this)... */
    tio.c_cc[VEOF] = 'D' & 0x3f;			/* '^D' */

    /* 
    ** Now its time to handle the ttyModes
    ** Decide if the user supplied a ttyModes resource, if so then
    ** parse it and if it was a legal mode string, pass the parse result
    */
#define TMODE(ind,var) if (_DtTermPrimTtyModeList[ind].set) var = _DtTermPrimTtyModeList[ind].value;

    _DtTermProcessLock();
    if (modeString)
    {
        if (parseTtyModes(modeString, _DtTermPrimTtyModeList) < 0)
        {
            /*
            ** NOTE: should we prepend the program name to this string?
            */
            fprintf(stderr, "Bad tty modes \"%s\"\n", modeString);
        }
        else
        {
            TMODE (XTTYMODE_intr,  tio.c_cc[VINTR]);
            TMODE (XTTYMODE_quit,  tio.c_cc[VQUIT]);
            TMODE (XTTYMODE_erase, tio.c_cc[VERASE]);
            TMODE (XTTYMODE_kill,  tio.c_cc[VKILL]);
            TMODE (XTTYMODE_eof,   tio.c_cc[VEOF]);
            TMODE (XTTYMODE_eol,   tio.c_cc[VEOL]);

#if	defined(HP_ARCHITECTURE)
            TMODE (XTTYMODE_swtch, tio.c_cc[VSWTCH]);
            TMODE (XTTYMODE_susp,  tio.c_cc[VSUSP]);
#if	OSMAJORVERSION > 9
	    /* HP-UX 10.0 supports the new, extended c_cc[] array...
	     */
            TMODE (XTTYMODE_start, tio.c_cc[VSTART]);
            TMODE (XTTYMODE_stop,  tio.c_cc[VSTOP]);
            TMODE (XTTYMODE_dsusp, tio.c_cc[VDSUSP]);
#ifdef	NOTDEF
	    /* the following two parameters are not supported by
	     * HP-UX 10.0.
	     */
            TMODE (XTTYMODE_rprnt, tio.c_cc[VREPRINT]);
            TMODE (XTTYMODE_flush, tio.c_cc[VDISCARD]);
#endif	/* NOTDEF */
            TMODE (XTTYMODE_weras, tio.c_cc[VWERASE]);
            TMODE (XTTYMODE_lnext, tio.c_cc[VLNEXT]);
#else	/* OSMAJORVERSION > 9 */
	    {
		/* With HP-UX 9.0 (and earlier) we need to set dsuspc
		 * via the ltchars array.  In addition, we have no support
		 * for rprnt, flush, weras, and lnext...
		 */
		struct ltchars ltc;

		if (!ioctl(pty, TIOCGLTC, &ltc)) {
		    TMODE (XTTYMODE_dsusp, ltc.t_dsuspc);
		    (void) ioctl(pty, TIOCSLTC, &ltc);
		}
	    }
#endif	/* OSMAJORVERSION > 9 */

#elif	defined(IBM_ARCHITECTURE)
            TMODE (XTTYMODE_start, tio.c_cc[VSTRT]);
            TMODE (XTTYMODE_stop,  tio.c_cc[VSTOP]);
            TMODE (XTTYMODE_susp,  tio.c_cc[VSUSP]);
            TMODE (XTTYMODE_dsusp, tio.c_cc[VDSUSP]);
            TMODE (XTTYMODE_rprnt, tio.c_cc[VREPRINT]);
            TMODE (XTTYMODE_flush, tio.c_cc[VDISCRD]);
            TMODE (XTTYMODE_weras, tio.c_cc[VWERSE]);
            TMODE (XTTYMODE_lnext, tio.c_cc[VLNEXT]);

#elif	defined(SUN_ARCHITECTURE)
            TMODE (XTTYMODE_swtch, tio.c_cc[VSWTCH]);
            TMODE (XTTYMODE_start, tio.c_cc[VSTART]);
            TMODE (XTTYMODE_stop,  tio.c_cc[VSTOP]);
            TMODE (XTTYMODE_susp,  tio.c_cc[VSUSP]);
            TMODE (XTTYMODE_dsusp, tio.c_cc[VDSUSP]);
            TMODE (XTTYMODE_rprnt, tio.c_cc[VREPRINT]);
            TMODE (XTTYMODE_flush, tio.c_cc[VDISCARD]);
            TMODE (XTTYMODE_weras, tio.c_cc[VWERASE]);
            TMODE (XTTYMODE_lnext, tio.c_cc[VLNEXT]);

#elif   defined(ALPHA_ARCHITECTURE)
	    TMODE (XTTYMODE_start, tio.c_cc[VSTART]);
	    TMODE (XTTYMODE_stop,  tio.c_cc[VSTOP]);
	    TMODE (XTTYMODE_susp,  tio.c_cc[VSUSP]);
	    TMODE (XTTYMODE_dsusp, tio.c_cc[VDSUSP]);
	    TMODE (XTTYMODE_rprnt, tio.c_cc[VREPRINT]);
	    TMODE (XTTYMODE_flush, tio.c_cc[VDISCARD]);
	    TMODE (XTTYMODE_weras, tio.c_cc[VWERASE]);
	    TMODE (XTTYMODE_lnext, tio.c_cc[VLNEXT]);
#endif
        }
#undef TMODE
    }
    _DtTermProcessUnlock();
    
    (void) tcsetattr(pty, TCSADRAIN, &tio);

#if defined (USE_CSWIDTH)
    if (MB_CUR_MAX > 1)
    {
	/*
	** we are in a wide character locale, set the cs
	** width settings...
	*/
	i_str.ic_cmd    = EUC_WSET;
	i_str.ic_timout = 0;
	i_str.ic_len    = sizeof(struct eucioc);
	i_str.ic_dp     = (char *)&wp;
	(void)ioctl(pty, I_STR, &i_str);
    }
#endif   /* (USE_CSWIDTH) */

#if defined (USE_SETCSMAP)
    /*
    ** NOTE:
    **     Setcsmap() only operates on STDIN, so we have to do some
    **     munging around to map the pty to STDIN in order to get
    **     the desired result.  This may seem wasteful, but it 
    **     makes it easier to encapsulate the OS dependencies in
    **     this function.
    */
    if (pty != 0)
    {
	oldStdin = fcntl(0, F_DUPFD, 1);
	(void) close(0);
	(void) dup(pty);
    }

    sprintf(path, "%s%s", CSMAP_DIR, nl_langinfo(CODESET));
    if(access(path, E_ACC|R_ACC) == 0)
    {
	setcsmap(path);
    }
	
    if (pty != 0)
    {
	(void) close(0);
	if (oldStdin >= 0)
	{
	    (void) dup(oldStdin);
	    (void) close(oldStdin);
	}
    }
#endif   /* (USE_SETCSMAP) */
}


#if	defined(USE_TIOCCONS)
#ifndef	CONSOLE_DEVICE
#define	CONSOLE_DEVICE	"/dev/console"
#endif	/* CONSOLE_DEVICE */
void _DtTermPrimPtyConsoleModeEnable(int pty)
{
    struct stat		  st;
    int			  one = 1;

    /* check to see if we are the owner of the device... */
    if (!stat(CONSOLE_DEVICE, &st)) {
	/* stat succeeded... */
	if (st.st_uid == getuid()) {
	    /* we are the owner, check the access... */
	    if (!access(CONSOLE_DEVICE, R_OK | W_OK)) {
		/* and we can read/write it... */
		/* we need to be setuid root... */
		(void) _DtTermPrimToggleSuidRoot(True);

		if (ioctl(pty, TIOCCONS, &one)) {
		    /* failure, errno was set... */
		    (void) perror(CONSOLE_DEVICE);
		    (void) fprintf(stderr,
			    "attempt to make tty the console failed\n");
		}
		/* we no longer need to be suid root... */
		(void) _DtTermPrimToggleSuidRoot(False);
	    } else {
		/* we can't read/write it... */
		(void) perror(CONSOLE_DEVICE);
		(void) fprintf(stderr, "-C console access denied\n");
	    }
	} else {
	    /* we are not the owner -- return an access owner... */
	    errno = EACCES;
	    (void) perror(CONSOLE_DEVICE);
	    (void) fprintf(stderr, "-C console access denied\n");
	}
    } else {
	/* we were unable to stat the file, errno is already set,
	 * failure...
	 */
	(void) perror(CONSOLE_DEVICE);
	(void) fprintf(stderr, "-C console access denied\n");
    }
}


#elif	defined(USE_SRIOCSREDIR)
#ifndef	CONSOLE_DEVICE
#define	CONSOLE_DEVICE	"/dev/console"
#endif	/* CONSOLE_DEVICE */
void _DtTermPrimPtyConsoleModeEnable(int pty)
{
    struct stat		  st;
    int			  consoleFd;

    /* check to see if we are the owner of the device... */
    if (!stat(CONSOLE_DEVICE, &st)) {
	/* stat succeeded... */
	if (st.st_uid == getuid()) {
	    /* we are the owner, open the file... */
	    if ((consoleFd = open("/dev/console", O_RDWR | O_NOCTTY)) >= 0) {
		if (ioctl(consoleFd, SRIOCSREDIR, pty) == -1) {
		(void) perror(CONSOLE_DEVICE);
		    (void) fprintf(stderr,
			    "attempt to make tty the console failed\n");
		}
		(void) close(consoleFd);
	    } else {
		/* we can't open it for reading and writing... */
		(void) perror(CONSOLE_DEVICE);
		(void) fprintf(stderr, "-C console access denied\n");
	    }
	} else {
	    /* we are not the owner -- return an access owner... */
	    errno = EACCES;
	    (void) perror(CONSOLE_DEVICE);
	    (void) fprintf(stderr, "-C console access denied\n");
	}
    } else {
	/* we were unable to stat the file, errno is already set,
	 * failure...
	 */
	(void) perror(CONSOLE_DEVICE);
	(void) fprintf(stderr, "-C console access denied\n");
    }
}


#else
void _DtTermPrimPtyConsoleModeEnable(int pty)
{
    (void) fprintf(stderr,
	    "-C console access not supported on this architecture\n");
}
#endif
