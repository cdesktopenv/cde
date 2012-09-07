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
/* (c) Copyright 1997 The Open Group */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 * Xdm - display manager daemon
 *
 * $TOG: session.c /main/21 1998/11/02 14:32:42 mgreess $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/*
 * session.c
 */

#ifdef _AIX
#ifdef _POWER
#include <stdio.h>
#include <sys/file.h>
#endif /* _POWER */
# include <usersec.h>
#endif /* _AIX */


# include "dm.h"
# include "vgmsg.h"
# include <signal.h>
# include <X11/Xatom.h>
# include <setjmp.h>
#if defined(__FreeBSD__)
# include <utmpx.h>
#else
# include <utmp.h>
#endif
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <limits.h>

#ifdef SIA
#include <sia.h>
#include <siad.h>
#include <X11/Intrinsic.h>
#endif

# include <X11/Xresource.h>

# include "vgproto.h"
# include "sysauth.h"


#ifdef sun
# include "solaris.h"
#endif

#ifdef BLS
# include <sys/security.h>
# include <prot.h>
#endif

#ifdef __KERBEROS
# include <krb.h>
#endif /*  __KERBEROS  */

#ifdef __PASSWD_ETC
#include "rgy_base.h"
#endif

#ifdef SIA

static  SIAENTITY *siaHandle = NULL;
static Boolean        dt_in_sia_ses_authent = False;

static struct sia_greeter_info {
    struct display *d;
    struct greet_info *greet;
    struct verify_info *verify;
    struct greet_state *state;
    int       status;
    } siaGreeterInfo;

static int SiaManageGreeter(
    int timeout,
    int rendition,
    unsigned char *title,
    int num_prompts,
    prompt_t *prompt);

static CopySiaInfo(SIAENTITY *siaHandle, struct greet_info *greet);

static void KillGreeter( void );

static int sia_greeter_pid;
static int sia_exit_proc_reg = FALSE;

#endif /* SIA */


#ifdef __apollo
  extern char *getenv();
#endif	

#define GREET_STATE_LOGIN             0
#define GREET_STATE_AUTHENTICATE      1
#define GREET_STATE_EXIT              2
#define GREET_STATE_EXPASSWORD        3
#define GREET_STATE_ERRORMESSAGE      4
#define GREET_STATE_LANG              5
#define GREET_STATE_BAD_HOSTNAME      6
#define GREET_STATE_ENTER             7
#define GREET_STATE_TERMINATEGREET    8
#define GREET_STATE_USERNAME          9
#define GREET_STATE_CHALLENGE        10
#define GREET_STATE_FORM             11

#ifndef DEF_SESSION
#define DEF_SESSION CDE_INSTALLATION_TOP "/bin/Xsession"
#endif

#define DOT             "."
#define DOTDOT          ".."

struct greet_state {
  int id;               /* state */
  int waitForResponse;  /* TRUE=wait for response from dtgreet */
  RequestHeader *request;      /* request buffer */
  ResponseHeader *response;    /* response buffer */
  int authenticated;    /* TRUE=user is authenticated */
  int vf;               /* last return code from Authenticate() */
  int loginReset;       /* reset flag for LOGIN state */
  char *msg;            /* message for VF_MESSAGE */
};

char *globalDisplayName;

/***************************************************************************
 *
 *  Local procedure declarations
 *
 ***************************************************************************/

static int  AbortClient( int pid) ;
static void DeleteXloginResources( struct display *d, Display *dpy) ;
       int  LoadXloginResources( struct display *d) ;
static int  ErrorHandler( Display *dpy, XErrorEvent *event) ;
static int  IOErrorHandler( Display *dpy) ;
static int  ManageGreeter( struct display *d, struct greet_info *greet,
                        struct verify_info *verify, struct greet_state *state) ;
static void RunGreeter( struct display *d, struct greet_info *greet,
                        struct verify_info *verify) ;
static void SessionExit( struct display *d, int status) ;
static void SessionPingFailed( struct display *d) ;
static int  StartClient(struct verify_info *verify, struct display *d,
                        int *pidp) ;
static SIGVAL catchAlrm( int arg ) ;
static SIGVAL catchHUP( int arg ) ;
static SIGVAL catchTerm( int arg ) ;
static SIGVAL waitAbort( int arg ) ;
static void SetupDisplay(struct display *d);

static void TellGreeter(RequestHeader *phdr);
static int AskGreeter(RequestHeader *preqhdr, char *b, int blen);
#if defined (DEBUG)
static void PrintResponse(ResponseHeader *phdr, int count);
#endif /* DEBUG */

#if defined (_AIX) && defined (_POWER)
static void release_aix_lic(void);
#endif

#if defined (_AIX) && !defined (_POWER)
static int session_execve(char *path, char *argv[], char *envp[]);
#else
#define session_execve(A,B,C) execve(A,B,C)
#endif 

#  ifdef __KERBEROS
      static void SetTicketFileName(uid_t uid);
#  endif /* __KERBEROS */

static void LoadAltDtsResources( struct display *d);
char * _ExpandLang(char *string, char *lang);



/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/

static int			clientPid;
static struct greet_info	greet;
static struct verify_info	verify;
static char			*defaultLanguage = NULL;

static jmp_buf	abortSession;

#ifdef BLS
  static char *sensitivityLevel;
#endif

#ifdef __KERBEROS
  static char krb_ticket_string[MAXPATHLEN];
#endif /* __KERBEROS */


XrmDatabase XresourceDB;
XrmDatabase XDB;


static SIGVAL
catchTerm( int arg )
{
    longjmp (abortSession, 1);
}

static jmp_buf	pingTime;

static SIGVAL
catchAlrm( int arg )
{
    longjmp (pingTime, 1);
}

static int
FileNameCompare (a, b)
#if defined(__STDC__)
	const void *a, *b;
#else
	char *a, *b;
#endif
{
    return strcoll (*(char **)a, *(char **)b);
}

static void
SessionPingFailed( struct display *d )
{
    if (clientPid > 1)
    {
    	AbortClient (clientPid);
    	source (&verify, d->reset);

#if defined (PAM) || defined(SUNAUTH) 
	{
    	    char* user = getEnv (verify.userEnviron, "USER");
	    char* ttyLine = d->gettyLine;
	    
#ifdef DEF_NETWORK_DEV
	    /* 
	     * If location is not local (remote XDMCP dtlogin) and 
	     * remote accouting is enabled (networkDev start with /dev/...)
	     * Set tty line name to match network device for accouting.  
	     * Unless the resource was specifically set, default is value 
	     * of DEF_NETWORK_DEV define (/dev/dtremote) 
	     */
	       
            if ( d->displayType.location != Local && 
		 networkDev && !strncmp(networkDev,"/dev/",5)) {
	        ttyLine = networkDev+5;
	    }
#endif 

#   ifdef PAM 
	    PamAccounting( verify.argv[0], d->name, d->utmpId, user, 
			   ttyLine, clientPid, ACCOUNTING, NULL);
#   else
	    solaris_accounting( verify.argv[0], d->name, d->utmpId, user, 
				ttyLine, clientPid, ACCOUNTING, NULL);
#   endif

#   ifdef sun
	    solaris_resetdevperm(ttyLine);
#   endif
	}
#endif

    }
    SessionExit (d, RESERVER_DISPLAY);
}


/*
 * We need our own error handlers because we can't be sure what exit code Xlib
 * will use, and our Xlib does exit(1) which matches REMANAGE_DISPLAY, which
 * can cause a race condition leaving the display wedged.  We need to use
 * RESERVER_DISPLAY for IO errors, to ensure that the manager waits for the
 * server to terminate.  For other X errors, we should give up.
 */

static int 
IOErrorHandler( Display *dpy )
{

    char *s = ((errno >= 0 && errno < sys_nerr) ? sys_errlist[errno]
						: "unknown error");

    LogError(ReadCatalog(
                       MC_LOG_SET,MC_LOG_FATAL_IO,MC_DEF_LOG_FATAL_IO),
	errno,s);
    exit(RESERVER_DISPLAY);
    return 0;
}

static int 
ErrorHandler( Display *dpy, XErrorEvent *event )
{
    LogError(ReadCatalog(MC_LOG_SET,MC_LOG_X_ERR,MC_DEF_LOG_X_ERR));
    if (XmuPrintDefaultErrorMessage (dpy, event, stderr) == 0) return 0;
    exit(UNMANAGE_DISPLAY);
}

    Display 		*dpy;

void
ManageSession( struct display *d )
{
    int			pid;
    Window		root;
   /* Display 		*dpy; */
#ifdef BYPASSLOGIN
    char	*BypassUsername;
    char	*BypassLogin();
#endif /* BYPASSLOGIN */
        

    Debug ("ManageSession():\n");

    /***********************************/
    /** remember the default language **/
    /***********************************/
    if (defaultLanguage == NULL) {
	if ( (d->language != NULL) && (strlen(d->language) > 0) ) {
	    defaultLanguage = strdup(d->language);
	} else {
	    defaultLanguage = "C";
	}
    }

#ifdef BYPASSLOGIN
    d->bypassLogin = 0;
    if ((BypassUsername = BypassLogin(d->name)) != NULL) {
	d->bypassLogin = 1;
	Debug("Login bypassed, running as %s\n",BypassUsername);
	greet.name = BypassUsername;
	if (!Verify (d, &greet, &verify)) {
	    Debug ("Login bypass verify failed!\n");
	    SessionExit (d, GREETLESS_FAILED);
	}
	Debug("Login bypass verify succeded!\n");
    } else
#endif /* BYPASSLOGIN */

    {
	int i;

	(void)XSetIOErrorHandler(IOErrorHandler);
	(void)XSetErrorHandler(ErrorHandler);
	SetTitle(d->name, (char *) 0);

	/*
	 * set root background to black...
	 */

	dpy = XOpenDisplay(d->name);
	for (i = ScreenCount(dpy) - 1; i >= 0; i--)
	{
	    Window	tmproot = RootWindow(dpy, i);

	    if (i == DefaultScreen(dpy))
	      root = tmproot;
	    
	    XSetWindowBackground(dpy, tmproot, BlackPixel(dpy, i));
	    XClearWindow(dpy, tmproot);
	}
	XFlush(dpy);

	/*
	** Step 5:  
	** Invoke Greet program, wait for completion.
	** If this routine returns, the user will have been
	** verified, otherwise the routine will exit inter-
	** nally with an appropriate exit code for the master
	** Dtlogin process.
	*/
     

	greet.name = greet.string = NULL;

	while (greet.name == NULL) {
	    SetHourGlassCursor(dpy, root);
	    LoadXloginResources (d);
	    SetupDisplay(d);
	    ApplyFontPathMods(d, dpy);
	    (void)XSetErrorHandler(ErrorHandler);
	    RunGreeter(d, &greet, &verify);

	    DeleteXloginResources (d, dpy);
	}
        XSetInputFocus(dpy, root, RevertToNone, CurrentTime);
	XCloseDisplay(dpy);
    }

#ifdef __KERBEROS
    /*
     *  Generate Kerberos ticket file name. Put in system and user
     *  environments...
     */

    if ( IsVerifyName(VN_KRB)) {
	SetTicketFileName(verify.uid);
	krb_set_tkt_string(krb_ticket_string);
	verify.systemEnviron = setEnv (verify.systemEnviron, 
				   "KRBTKFILE",
				   krb_ticket_string);

	verify.userEnviron   = setEnv (verify.userEnviron, 
				   "KRBTKFILE",
				   krb_ticket_string);
    }
#endif /* __KERBEROS */

   /* set LOCATION env var */
    if(d->displayType.location == Local)  {
        verify.systemEnviron = setEnv (verify.systemEnviron,
                                   LOCATION,
                                   "local");
        /* ITE is needed only for Local displays */
        /* set ITE env var */
        if(d->gettyLine)
           verify.systemEnviron = setEnv (verify.systemEnviron,
                                  "ITE",
                                  d->gettyLine);
    }
    else
        verify.systemEnviron = setEnv (verify.systemEnviron,
                                   LOCATION,
                                   "remote");

    {
        struct passwd *pwd;
        char   gid[25];
        sprintf(gid,"%ld",(long)getgid());
        /* set user group id (USER_GID) env var */
        verify.systemEnviron = setEnv (verify.systemEnviron,
                                   "USER_GID",
                                   gid);

        /* set root group id (ROOT_GID) env var */
        pwd = getpwnam("root");
        if(pwd)  {
           sprintf(gid,"%ld",(long)pwd->pw_gid);
           verify.systemEnviron = setEnv (verify.systemEnviron,
                                      "ROOT_GID",
                                      gid);
        }
    }

    /*
     * Run system-wide initialization file
     */
    if (source (&verify, d->startup) != 0)
    {
	Debug ("Startup program %s exited with non-zero status\n",
		d->startup);
	SessionExit (d, OBEYSESS_DISPLAY);
    }

#ifdef sun
    if ( solaris_setdevperm(d->gettyLine, verify.uid, verify.gid) == 0 ) {
	SessionExit (d, OBEYSESS_DISPLAY);
    }
#endif

    clientPid = 0;
    if (!setjmp (abortSession)) {
	signal (SIGTERM, catchTerm);
	/*
	 * Start the clients, changing uid/groups
	 *	   setting up environment and running the session
	 */
	if (StartClient (&verify, d, &clientPid)) {
	    Debug ("Client started\n");

	    /*
	     * We've changed dtlogin to pass HUP's down to the children
	     * so ignore any HUP's once the client has started.
	     */
	    signal(SIGHUP, SIG_IGN);

	    /*
	     * Wait for session to end,
	     */
	    for (;;) {
		if (d->pingInterval)
		{
		    if (!setjmp (pingTime))
		    {
			signal (SIGALRM, catchAlrm);
			alarm (d->pingInterval * 60);
			pid = wait ((waitType *) 0);
			alarm (0);
		    }
		    else
		    {
			alarm (0);
		    	if (!PingServer (d, (Display *) NULL))
			    SessionPingFailed (d);
		    }
		}
		else
		{
		    pid = wait ((waitType *) 0);
		}
		if (pid == clientPid)
		    break;
	    }

	    /*
	     * We've changed dtlogin to pass HUP's down to the children
	     * so ignore any HUP's once the client has started.
	     */
	    signal(SIGHUP, SIG_DFL);
	} else {
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_FAIL_START,MC_DEF_LOG_FAIL_START));
	}
    } else {
	/*
	 * when terminating the session, nuke
	 * the child and then run the reset script
	 */
	AbortClient (clientPid);
    }

    /*
     * on foreign displays without XDMCP, send a SIGTERM to the process
     * group of the session manager. This augments the "resetServer()"
     * routine and helps get all clients killed. It is possible for a client
     * to have a connection to the server, but not have a window.
     */

    if (d->displayType.location == Foreign   &&
        d->displayType.origin   != FromXDMCP	)
	AbortClient(clientPid);


#ifdef __KERBEROS
    /*
     * remove ticket file...
     */

    if ( IsVerifyName(VN_KRB) ) {
	dest_tkt();
    }

#endif /* __KERBEROS */

	
    /*
     * run system-wide reset file
     */
    Debug ("Source reset program %s\n", d->reset);
    source (&verify, d->reset);

#if defined(PAM) || defined(SUNAUTH)
    {
	char* user = getEnv (verify.userEnviron, "USER");
	char* ttyLine = d->gettyLine;
	    
#   ifdef DEF_NETWORK_DEV
	    /* 
	     * If location is not local (remote XDMCP dtlogin) and 
	     * remote accouting is enabled (networkDev start with /dev/...)
	     * Set tty line name to match network device for accouting.  
	     * Unless the resource was specifically set, default is value 
	     * of DEF_NETWORK_DEV define (/dev/dtremote) 
	     */
	       
            if ( d->displayType.location != Local && 
		 networkDev && !strncmp(networkDev,"/dev/",5)) {
	        ttyLine = networkDev+5;
	    }
#   endif 

#   ifdef PAM
	PamAccounting( verify.argv[0], d->name, d->utmpId, user, 
		       ttyLine, clientPid, ACCOUNTING, NULL);
#   else
	solaris_accounting( verify.argv[0], d->name, d->utmpId, user, 
			    ttyLine, clientPid, ACCOUNTING, NULL);
#   endif

#   ifdef sun
	solaris_resetdevperm(ttyLine);
#   endif
    }
#endif

    SessionExit (d, OBEYSESS_DISPLAY);
}


int
LoadXloginResources( struct display *d )
{
    char	cmd[1024];
    char	*language="";
    char	*lang_key="";
    char	*authority="";
    char	*auth_key="";
    char        *resources = NULL;
    char	*p;
    char	tmpname[32];

    if (d->resources && d->resources[0]) { 
        resources = _ExpandLang(d->resources, d->language);
        if (access (resources, R_OK) != 0) {
            /** fallback to the C locale for resources **/
	    Debug("LoadXloginResources - cant access %s\n", resources);
            Debug("\t %s.  Falling back to C.\n", sys_errlist[errno]);
            free(resources);
            resources = _ExpandLang(d->resources, "C");
            if (access (resources, R_OK) != 0) {
                /** can't find a resource file, so bail **/
	        Debug("LoadXloginResources - cant access %s.\n", resources);
                Debug("\t %s.  Unable to find resource file.\n",
		      sys_errlist[errno]);
                free(resources);
                return(-1);
            }
        }

	if (d->authFile && strlen(d->authFile) > 0 ) {
		authority = d->authFile;
		auth_key = "XAUTHORITY=";
	}

	if (d->language && strlen(d->language) > 0 ) {
		language = strdup(d->language);
		lang_key = "-D";
	}

	/*
	 *  replace any "-" or "." in the language name with "_". The C
	 *  preprocessor used by xrdb does not accept "-" or "." in a name.
	 */
	 
	while ( (p = strchr(language, '-')) != NULL ) {
	    *p = '_';
	}
	 
	while ( (p = strchr(language, '.')) != NULL ) {
	    *p = '_';
	}
		    
	if ( strlen(language) > 0 )
	    free(language);


	Debug("LoadXloginResources - loading resource db from %s\n", resources);
	if((XresourceDB = XrmGetFileDatabase(resources)) == NULL)
          Debug("LoadXloginResources - Loading resource db from %s failed\n",
		resources);

	LoadAltDtsResources(d); 

        strcpy(tmpname,"/var/dt/dtlogin_XXXXXX");
        (void) mktemp(tmpname);

        XrmPutFileDatabase(XresourceDB, tmpname);

	sprintf (cmd, "%s%s %s -display %s -load %s",
			auth_key, authority, d->xrdb, d->name, tmpname);
	Debug ("Loading resource file: %s\n", cmd);

	system (cmd);  

	if (debugLevel <= 10)
	  if (unlink (tmpname) == -1)
	    Debug ("unlink() on %s failed\n", tmpname);
    }

    if (resources) free (resources);

    return (0);
}

/***************************************************************************
 *
 *  LoadAltDtsResources 
 *
 *
 *  set up alternate desktop resources..
 *
 ***************************************************************************/
 
static void
LoadAltDtsResources(struct display *d)
{
    DIR         *dirp;
    struct dirent *dp;
    char        dirname[2][MAXPATHLEN];
    char        res_file[MAXPATHLEN];
    char        *rmtype;        /* for XrmGetResource()                    */
    XrmValue    rmvalue;        /* for XrmGetResource()                    */
    char        buf[MAXPATHLEN];
    char        tempbuf[MAXPATHLEN];
    XrmDatabase defDb;
    XrmDatabase userDb;
    char        altdtres[MAXPATHLEN];
    char        Altdtres[MAXPATHLEN];
    int         i = 0;
    int		j = 0;
    char        *resources = NULL;
    int         file_count = 0;
    int         num_allocated = 0;
    char        **file_list = NULL;
    int         list_incr = 10;

    if ( XrmGetResource(XresourceDB,
                   "Dtlogin*altDts",  "Dtlogin*AltDts",
                   &rmtype, &rmvalue ) ) {
		strcpy(tempbuf,rmvalue.addr);
		i = atoi(tempbuf);
    }

    strcpy(dirname[0],CDE_INSTALLATION_TOP "/config/%L/Xresources.d/");
    strcpy(dirname[1],CDE_CONFIGURATION_TOP "/config/%L/Xresources.d/");

    for(j = 0; j < 2 ; ++j)
    {
        resources = _ExpandLang(dirname[j], d->language);
        if (access (resources, R_OK) != 0)
	{
            Debug("LoadAltDtsResources- cant access %s.\n", resources);
            Debug("\t %s.  Falling back to C.\n", sys_errlist[errno]);

            if (resources)
	    {
	        free (resources);
	        resources = NULL;
	    }

            resources = _ExpandLang(dirname[j], "C");
            if (access (resources, R_OK) != 0)
	    {
                Debug("LoadAltDtsResources- cant access %s.\n", resources);
                Debug("\t %s.\n", sys_errlist[errno]);
	    }
	    else
              strcpy(dirname[j], resources);
	}
	else {
            strcpy(dirname[j],resources);
            Debug("LoadAltDtsResources- found resource dir %s\n", dirname[j]);
	}

        if (resources)
	{
	    free (resources);
	    resources = NULL;
	}
    }


    /*
     * Create a list of the alt DT files
     *
     * NOTE - an assumption made here is that files in /etc/dt
     * should take precedence over files in /usr/dt.  This precedence
     * is maintained during the sort becase /etc/dt will come before
     * /usr/dt
     */

    for(j = 0; j < 2 ; ++j) {

        if((dirp = opendir(dirname[j])) != NULL) {

            while((dp = readdir(dirp)) != NULL) {

                if ((strcmp(dp->d_name, DOT)    != 0) &&
                    (strcmp(dp->d_name, DOTDOT) != 0)) {

                    sprintf (res_file, "%s%s", dirname[j],dp->d_name);
                    if ((access (res_file, R_OK)) != 0)
		    {
                        Debug("LoadAltDtsResources- cant access %s.\n",
			      resources);
                        Debug("\t %s.\n", sys_errlist[errno]);
                        continue;
		    }

                    if (file_count == 0) {
                        file_list = malloc (list_incr * sizeof(char **));
                        num_allocated += list_incr;
                    }
                    if (file_count + 1 > num_allocated) {
                        num_allocated += list_incr;
                        file_list = realloc (file_list,
                                             num_allocated * sizeof(char **));
                    }
                    file_list[file_count] = strdup (res_file);
                    file_count++;
                }
            }
            closedir(dirp);
        }
    }

    if (file_count > 0)
	qsort (file_list, file_count, sizeof (char *), FileNameCompare);

    for (j = 0; j < file_count ; j++) {

	userDb = XrmGetFileDatabase(file_list[j]);
	XrmMergeDatabases(userDb,&XresourceDB);
 
	if ( XrmGetResource(XresourceDB, "Dtlogin*altDtsIncrement",
			"Dtlogin*AltDtsIncrement", &rmtype, &rmvalue ) ) {

	    /* 
	     * remove the trailing spaces 
	     */
	    if(strchr(rmvalue.addr,' '))
	        strcpy(tempbuf, strtok(rmvalue.addr," "));
	    else
		strcpy(tempbuf, rmvalue.addr);

	    if ((strcmp(tempbuf, "True") == 0) || 
                (strcmp(tempbuf, "TRUE") == 0))  {
 
	        if ( XrmGetResource(XresourceDB,
				"Dtlogin*altDtKey",  "Dtlogin*AltDtKey",
		       		&rmtype, &rmvalue ) ) {
		     ++i;
		     sprintf(altdtres,"Dtlogin*altDtKey%d",i);
		     XrmPutStringResource(&XresourceDB, altdtres, rmvalue.addr);
                 }
		 else
		    continue;
 
		 if ( XrmGetResource(XresourceDB,
				"Dtlogin*altDtName",  "Dtlogin*AltDtName",
				&rmtype, &rmvalue ) ) {
		     sprintf(altdtres,"Dtlogin*altDtName%d",i);
		     XrmPutStringResource(&XresourceDB, altdtres, rmvalue.addr);
		 }
		 if ( XrmGetResource(XresourceDB,
				"Dtlogin*altDtStart",  "Dtlogin*AltDtStart",
				&rmtype, &rmvalue ) ) {
		     sprintf(altdtres,"Dtlogin*altDtStart%d",i);
		     XrmPutStringResource(&XresourceDB, altdtres, rmvalue.addr);
		 }
		 if ( XrmGetResource(XresourceDB,
				"Dtlogin*altDtLogo",  "Dtlogin*AltDtLogo",
				&rmtype, &rmvalue ) ) {
		     sprintf(altdtres,"Dtlogin*altDtLogo%d",i);
		     XrmPutStringResource(&XresourceDB, altdtres, rmvalue.addr);
		 }
            }
        }
    }

    sprintf(tempbuf,"%d",i);
    XrmPutStringResource(&XresourceDB, "Dtlogin*altDts", tempbuf); 

    if (file_count > 0) {
	for (i = 0; i < file_count; i++) {
	    Debug ("Loading resource file: %s\n", file_list[i]);
	    free (file_list[i]);
	}
	free (file_list);
    }
}


/******************
 *
 * Function Name:  _ExpandLang
 *
 * Description:
 *
 *      This function takes the string "string", searches for occurences of
 *      "%L" in the string and if found, the "%L" is substituted with
 *      the value of the $LANG environment variable.
 *
 *      If $LANG is not defined, the %L is replace with NULL.
 *
 * Note: 
 *
 *      _ExpandLang() is based on the DtSvc _DtExpandLang() static routine.
 *
 * Synopsis:
 *
 *      ret_string = _ExpandLang (string);
 *
 *      char *ret_string;       Returns NULL if "string" is NULL or it points
 *                              to the expanded string.
 *
 *      char *string;           The first part of the pathname.  Typically
 *                              the directory containing the item of interest.
 *
 * Note: The caller is responsible for free'ing the returned string.
 *
 ******************/


char *
_ExpandLang(
        char *string,
        char *lang )
{
   char         *tmp;
   char         *pch;
   char         *trail;
   int          n = 0;
   int          lang_len = 0;
   int          tmp_len;
   int          i = 0;

   if (string == NULL)
      return (NULL);

   /*
    * Count the number of expansions that will occur.
    */


   for (n = 0, pch = string ; pch != NULL ; ) {
      if ((pch = strchr (pch, '%')) != NULL) {
         n++;
         pch++;
      }
   }

   if (n == 0)
      return (strdup(string));

/*
 * We should really be calling setlocale to determine the "default"
 * locale but setlocale's return value is not standardized across
 * the various vendor platforms nor is it consistent within differnt
 * revs of individual OS's. (e.g. its changing between HP-UX 9.0 and
 * HP-UX 10.0).   The "right" call would be the following line:
 *
 *  if ((lang = getenv ("LANG")) || (lang = setlocale(LC_C_TYPE,NULL)))
 *
 * Here we hard code the default to "C" instead of leaving it NULL.
 */
   if (lang || (lang = getenv ("LANG")) || (lang = "C"))
      lang_len = strlen (lang);

   /*
    * Create the space needed.
    */
   tmp_len = strlen (string) + (n * lang_len) + n + 1;
   tmp = (char *) malloc (tmp_len);
   for (i = 0; i < tmp_len; tmp[i] = '\0', i++);

   pch = string;

   while (pch != NULL) {
      trail = pch;

      if ((pch = strchr (pch, '%')) != NULL) {

         pch++;

         if (pch == NULL) {
            (void) strncat (tmp, trail, ((pch - 1) - trail) + 1);
         }
         else if ((pch != NULL) && *pch == 'L') {
            if (lang_len == 0) {
               if (((pch - trail) >=2) && (*(pch-2) == '/'))
                  /*
                   * Remove the "/" as well as the "%L".
                   */
                  (void) strncat (tmp, trail, (pch - trail) - 2);
               else
                  (void) strncat (tmp, trail, (pch - trail) - 1);
            }
            else {
               /*
                * Remove the "%L" and then append the LANG.
                */
               (void) strncat (tmp, trail, (pch - trail) - 1);
               (void) strcat (tmp, lang);
            }
         }
         else {
            (void) strncat (tmp, trail, (pch - trail) + 1);
         }
         if (pch != NULL)
            pch++;
      }
      else {
         /*
          * A '%' was not found.
          */
         (void) strcat (tmp, trail);
      }
   }

   return (tmp);
}


static void
SetupDisplay (struct display *d)
{
    char **env = 0, **crt_systemEnviron;

    if (d->setup && d->setup[0] && (access(d->setup, R_OK ) == 0))
    {
        crt_systemEnviron = verify.systemEnviron;
        env = systemEnv (d, (char *) 0, (char *) 0);
        if (d->authFile && strlen(d->authFile) > 0 )
            env = setEnv( env, "XAUTHORITY", d->authFile );
        if(d->displayType.location == Local)  
            env = setEnv (env, LOCATION, "local");
        else
            env = setEnv (env, LOCATION, "remote");
        verify.systemEnviron = env;
        source (&verify, d->setup);
        verify.systemEnviron = crt_systemEnviron;
        freeEnv (env);
    }
}

/*ARGSUSED*/
static void 
DeleteXloginResources( struct display *d, Display *dpy )
{
    XDeleteProperty(dpy, RootWindow (dpy, 0), XA_RESOURCE_MANAGER);
}

#if  0 			  /* dead code: transferred to Dtgreet */

static jmp_buf syncJump;

static SIGVAL
syncTimeout ()
{
    longjmp (syncJump, 1);
}


SecureDisplay (d, dpy)
struct display	*d;
Display		*dpy;
{
    Debug ("SecureDisplay():\n");
    signal (SIGALRM, syncTimeout);
    if (setjmp (syncJump)) {
	LogError(ReadCatalog(MC_LOG_SET,MC_LOG_NO_SECDPY,MC_DEF_LOG_NO_SECDPY),
		   d->name);
	SessionExit (d, RESERVER_DISPLAY);
    }
    alarm ((unsigned) d->grabTimeout);
    Debug ("Before XGrabServer()\n");
    XGrabServer (dpy);
    if (XGrabKeyboard (dpy, DefaultRootWindow (dpy), True, GrabModeAsync,
		       GrabModeAsync, CurrentTime) != GrabSuccess)
    {
	alarm (0);
	signal (SIGALRM, SIG_DFL);
	LogError(ReadCatalog(MC_LOG_SET,MC_LOG_NO_SECKEY,MC_DEF_LOG_NO_SECKEY),
		  d->name);
	SessionExit (d, RESERVER_DISPLAY);
    }
    Debug ("XGrabKeyboard() succeeded\n");
    alarm (0);
    signal (SIGALRM, SIG_DFL);
    pseudoReset (dpy);
    if (!d->grabServer)
    {
	XUngrabServer (dpy);
	XSync (dpy, 0);
    }
    Debug ("Done secure %s\n", d->name);
}

UnsecureDisplay (d, dpy)
struct display	*d;
Display		*dpy;
{
    Debug ("Unsecure display %s\n", d->name);
    if (d->grabServer)
	XUngrabServer (dpy);
    XSync (dpy, 0);
}
#endif

#ifdef _AIX
#ifdef _POWER

static void
release_aix_lic(void)
{
 /*
  * Release AIX iFOR/LS license (if any)
  */
  int fd;
  struct pipeinfo {
    int request_type;
    pid_t login_pid;
  } release_me;
  
  release_me.request_type = -1;
  release_me.login_pid = getpid();
  if ((fd = open("/etc/security/monitord_pipe", O_RDWR, 0600)) >= 0)
  {
    write(fd, &release_me, sizeof(release_me));
    close(fd);
  }
  Debug("release message to monitord: %s\n", (fd >= 0) ? "OK" : "failed");
}

#endif /* _POWER */
#endif /* _AIX */


static void
SessionExit( struct display *d, int status )
{

#ifdef _AIX
#ifdef _POWER
    release_aix_lic();
#endif /* _POWER */
#endif /* _AIX */

    /* make sure the server gets reset after the session is over */
    if (d->serverPid >= 2) {
        Debug("Reseting server:  pid %d signal %d\n",
	      d->serverPid, d->resetSignal);

        if (d->terminateServer == 0 && d->resetSignal)
	    kill (d->serverPid, d->resetSignal);
    }
    else
	ResetServer (d);

    Debug("Exiting Session with status:  %d\n", status);
    exit (status);
}

static int 
StartClient( struct verify_info *verify, struct display *d, int *pidp )
{
    char	**f, *home;
    char        currentdir[PATH_MAX+1];
    char	*failsafeArgv[20];
    char	*user;			/* users name 			   */
    char 	*lang, *font;		/* failsafe LANG and font	   */

    int	pid;
    int		failsafe = FALSE;	/* do we run the failsafe session? */
    int		password = FALSE;	/* do we run /bin/passwd?	   */
    int		i;
    FILE	*lastsession;
    char 	lastsessfile[MAXPATHLEN];

#ifdef BLS
   struct pr_passwd	*b1_pwd;
#endif

#ifdef __AFS
#define NOPAG 0xffffffff
    long	pagval, j;
    long	ngroups, groups[NGROUPS];
#endif /* __AFS */

    waitType  status;

    if (verify->argv) {
	Debug ("StartSession %s: ", verify->argv[0]);
	for (f = verify->argv; *f; f++) {
		Debug ("%s ", *f);
		if ( strcmp(*f, "failsafe") == 0) failsafe = TRUE;
		if ( strcmp(*f, "password") == 0) failsafe = password = TRUE;
	}
	Debug ("; ");
    }
    if (verify->userEnviron) {
	for (f = verify->userEnviron; *f; f++)
		Debug ("%s ", *f);
	Debug ("\n");
    }

    user = getEnv (verify->userEnviron, "USER");
    
    switch (pid = fork ()) {
    case 0:
#ifdef SIA
        /* Force a failsafe session if we can't touch the home directory 
         * SIA has already attempted to chdir to HOME, and the current dir 
         * will be set to / if it failed. We just check to see if the HOME
         * path is our current directory or not.
         */
        home = getEnv (verify->userEnviron, "HOME");
        getcwd(currentdir, PATH_MAX+1);
	Debug("Current directory is: %s\n", currentdir);
#if 0
	/*
	 * CDExc17917
	 * The following little check doesn't really work.  For example,
	 * here at the XC, NIS reports my home directory as
	 * "/site/guests/montyb" while getcwd comes up with
	 * "/net/nexus/site/guests/montyb".
	 */
        if (strcmp(home, currentdir)) {
            Debug("Can't access home directory, setting failsafe to TRUE\n");
            failsafe = TRUE;
            LogError (ReadCatalog(
                      MC_LOG_SET,MC_LOG_NO_HMDIR,MC_DEF_LOG_NO_HMDIR),
                      home, getEnv (verify->userEnviron, "USER"));
            verify->userEnviron = setEnv(verify->userEnviron, "HOME", "/");
        }
#endif
#endif /* SIA */

	CleanUpChild ();

	/*
	 *  do process accounting...
	 */
#if defined(PAM) || defined(SUNAUTH)
	{
	    char* ttyLine = d->gettyLine;
	    
#   ifdef DEF_NETWORK_DEV
	    /* 
	     * If location is not local (remote XDMCP dtlogin) and 
	     * remote accouting is enabled (networkDev start with /dev/...)
	     * Set tty line name to match network device for accouting.  
	     * Unless the resource was specifically set, default is value 
	     * of DEF_NETWORK_DEV define (/dev/dtremote) 
	     */
	       
            if ( d->displayType.location != Local && 
		 networkDev && !strncmp(networkDev,"/dev/",5)) {
	        ttyLine = networkDev+5;
	    }
#   endif 

#   ifdef PAM
	    PamAccounting(verify->argv[0], d->name, d->utmpId, user, 
			  ttyLine, getpid(), USER_PROCESS, NULL);
#   else
	    solaris_accounting(verify->argv[0], d->name, d->utmpId, user, 
			       ttyLine, getpid(), USER_PROCESS, NULL);
#   endif
	}
#endif

#if !defined(sun) && !defined(CSRG_BASED)
	Account(d, user, NULL, getpid(), USER_PROCESS, status);
#endif

#ifdef AIXV3
         /*
          * In _AIX _POWER, the PENV_NOEXEC flag was added. This tells
          * setpenv() to set up the user's process environment and return
          * without execing. This allows us to set up the process environment
          * and proceed to the execute() call as do the other platforms. 
          *
          * Unfortunately, for AIXV3, the PENV_NOEXEC does not exist, so
          * we have to pospone the setpenv() to the actual execute().
          */

         /*
          * These defines are the tag locations in userEnviron.
          * IMPORTANT: changes to the locations of these tags in verify.c
          * must be reflected here by adjusting SYS_ENV_TAG or USR_ENV_TAG.
          */
          #define SYS_ENV_TAG 0
          #define USR_ENV_TAG 3

        /*
         * Set the user's credentials: uid, gid, groups,
         * audit classes, user limits, and umask.
	 * RK 09.13.93
         */
        if (setpcred(user, NULL) == -1)
        {
            Debug("Can't set User's Credentials (user=%s)\n",user);
            return (0);
        }
#ifdef _POWER
        {
          char *usrTag, *sysTag;
          extern char **newenv;

         /*
          * Save pointers to tags. The setpenv() function clears the pointers
          * to the tags in userEnviron as a side-effect.
          */
          sysTag = verify->userEnviron[SYS_ENV_TAG];
          usrTag = verify->userEnviron[USR_ENV_TAG];

         /*
          * Set the users process environment. Store protected variables and
          * obtain updated user environment list. This call will initialize
          * global 'newenv'.
          */
          #define SESSION_PENV (PENV_INIT | PENV_ARGV | PENV_NOEXEC)
          if (setpenv(user, SESSION_PENV, verify->userEnviron, NULL) != 0)
          {
              Debug("Can't set process environment (user=%s)\n",user);
              return(0);
          }

         /*
          * Restore pointers to tags.
          */
          verify->userEnviron[SYS_ENV_TAG] = sysTag;
          verify->userEnviron[USR_ENV_TAG] = usrTag;

         /*
          * Free old userEnviron and replace with newenv from setpenv().
          */
          freeEnv(verify->userEnviron);
          verify->userEnviron = newenv;
        }
#endif /* _POWER */
        

#endif /* AIXV3 */



#if defined(PAM)
	if (PamSetCred( verify->argv[0], 
			     user, verify->uid, verify->gid) > 0 ) {
            Debug("Can't set User's Credentials (user=%s)\n",user);
	    return(0);
	} 
#endif

#ifdef SUNAUTH 
	if ( solaris_setcred(verify->argv[0], 
			     user, verify->uid, verify->gid) > 0 ) {
            Debug("Can't set User's Credentials (user=%s)\n",user);
	    return(0);
	}
#endif /* SUNAUTH */


#ifndef sun

#ifdef BLS
    /*
     *  HP BLS B1 session setup...
     *
     *   1. look up user's protected account information.
     *   2. set the session sensitivity/clearance levels 
     *   3. set the logical UID (LUID)
     */

    if ( ISSECURE ) {
	Debug("BLS - Setting user's clearance, security level and luid.\n");
	set_auth_parameters(1, verify->argv);
	init_security();

	verify->user_name = user;
	strncpy(verify->terminal,d->name,15);
	verify->terminal[15]='\0';
	verify->pwd = getpwnam(user);

	if ( verify->pwd == NULL || strlen(user) == 0 ) {
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_BLSACCT,MC_DEF_LOG_NO_BLSACCT));
	    exit (1);
	}
	verify->prpwd= b1_pwd = getprpwnam(user);
        verify->uid = b1_pwd->ufld.fd_uid;
    
	if ( b1_pwd == NULL || strlen(user) == 0 ) {
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_BLSPACCT,MC_DEF_LOG_NO_BLSPACCT));
	    exit (1);
	}

	/*
	 * This has already been done successfully by dtgreet
	 * but we need to get all the information again for the
	 * dtlogin process.
	 */
	if ( verify_user_seclevel(verify,sensitivityLevel) != 1 ) {
	    Debug("BLS - Could not verify sensitivity level.\n");
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_VFYLVL,MC_DEF_LOG_NO_VFYLVL));
            exit (1);
	}

	if ( change_to_user(verify) != 1 ) {
	    Debug("BLS - Could not change to user: %s.\n",verify->user_name);
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_BLSUSR,MC_DEF_LOG_NO_BLSUSR),
		verify->user_name);
            exit (1);
	}

	Debug("BLS - Session setup complete.\n");
    } else {
#endif /* BLS */
			
#ifdef __apollo

	/* 
	 * This should never fail since everything has been verified already.
	 * If it does it must mean registry strangeness, so exit, and try
	 * again...
	 */

	if (!DoLogin (user, greet.password, d->name)) exit (1);
	
	/*
	 * extract the SYSTYPE and ISP environment values and set into user's 
	 * environment. This is necessary since we do an execve below...
	 */
	 
	verify->userEnviron = setEnv(verify->userEnviron, "SYSTYPE",
				     getenv("SYSTYPE"));

	verify->userEnviron = setEnv(verify->userEnviron, "ISP",
				     getenv("ISP"));

#else /* ! __apollo */


#  ifdef __AFS
	if ( IsVerifyName(VN_AFS) ) {
	    pagval = get_pag_from_groups(verify->groups[0], verify->groups[1]);
	    Debug("AFS - get_pag_from_groups() returned pagval = %d\n", pagval);

	    initgroups(greet.name, verify->groups[2]);
	    ngroups = getgroups(NGROUPS, groups);
	    Debug("AFS - getgroups() returned ngroups = %d\n", ngroups);
	    for (i=0; i < ngroups; i++)
		Debug("AFS - groups[%d] = %d\n", i, groups[i]);

	    if ((pagval != NOPAG) &&
	        (get_pag_from_groups(groups[0], groups[1])) == NOPAG ) {
                /* we will have to shift grouplist to make room for pag */
	        if (ngroups+2 > NGROUPS)
		    ngroups=NGROUPS-2;
	        for (j=ngroups-1; j >= 0; j--) {
		    groups[j+2] = groups[j];
	        }
	        ngroups += 2;
	        get_groups_from_pag(pagval, &groups[0], &groups[1]);
	        if (setgroups(ngroups, groups) == -1) {
		    LogError(
			ReadCatalog(
			  MC_LOG_SET,MC_LOG_AFS_FAIL,MC_DEF_LOG_AFS_FAIL));
		    exit(1);
		}
	    }
	}
#  else /* ! __AFS */
/* If SIA is enabled, the initgroups and setgid calls are redundant
 * so skip them.
 */
#  ifndef SIA
#    ifdef NGROUPS

	/*
	 * if your system does not support "initgroups(3C)", use
	 * the "setgroups()" call instead...
	 */
	 
#      if (defined(__hpux) || defined(__osf__))
	initgroups(user, -1);
#      else
	setgroups (verify->ngroups, verify->groups);
#      endif

/* setpenv() will set gid for AIX */
#if !defined (_AIX)
	setgid (verify->groups[0]);
#endif

#    else  /* ! NGROUPS */

/* setpenv() will set gid for AIX */
#if !defined (_AIX)
	setgid (verify->gid);
#endif

#    endif /* NGROUPS */
#  endif /* !SIA */
#  endif /* __AFS */


#  ifdef AUDIT
	setaudid(verify->audid);
	setaudproc(verify->audflg);
#  endif

/* setpenv() will set uid for AIX */
#if !defined(_AIX)
	if (setuid(verify->uid) != 0) {
	    Debug(   "Setuid failed for user %s, errno = %d\n", user, errno);
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_FAIL_SETUID,MC_DEF_LOG_FAIL_SETUID),
		user,errno);
	    exit (1);
	}
#endif

#endif /* __apollo */

#ifdef BLS
    }  /* ends the else clause of if ( ISSECURE ) */
#endif /* BLS */

#endif /* ! sun */
		    
	/*
	 *  check home directory again...
	 */

#ifndef SIA
/* Don't need to do this if SIA is enabled, already been done.
 */
	home = getEnv (verify->userEnviron, "HOME");
	if (home) {
		if (chdir (home) == -1) {
			LogError (ReadCatalog(
				MC_LOG_SET,MC_LOG_NO_HMDIR,MC_DEF_LOG_NO_HMDIR),
				home, getEnv (verify->userEnviron, "USER"));
			chdir ("/");
			verify->userEnviron = setEnv(verify->userEnviron, 
						     "HOME", "/");
		}
		else if(!failsafe) {
			strcpy(lastsessfile,home); 			/* save user's last session */
			strcat(lastsessfile,LAST_SESSION_FILE);
                	if((lastsession = fopen(lastsessfile,"w")) == NULL)
                        	Debug("Unable to open file for writing: %s\n",lastsessfile);
                	else{
                   		fputs(verify->argv[0],lastsession);
                   		fclose(lastsession);
                	 }
		}
	}
#endif


	SetUserAuthorization (d, verify);

	/*
	 * clear password...
	 */
	if (greet.password)
	    bzero(greet.password, strlen(greet.password));

#ifdef	BLS
	/*
	 * Write login information to a file
	 * The file name should really be settable by some kind of resource
	 * but time is short so we hard-wire it to ".dtlogininfo".
	 */
	if ( ! writeLoginInfo( ".dtlogininfo" , verify ) )
		Debug("Unable to write \".dtlogininfo\"\n");
#  ifndef NDEBUG
	/* extra debugging */
	if(!dump_sec_debug_info(verify)) {
	    Debug("Something wrong with environment\n");
	    exit(1);
	}
#  endif  /* ! NDEBUG */
#endif	/* BLS */

	/*
	 *  exec session...
	 */
	if (verify->argv) {
	    if ( !failsafe) {
		Debug ("Executing session %s\n", verify->argv[0]);
		execute (verify->argv, verify->userEnviron);
		LogError(ReadCatalog(
			MC_LOG_SET,MC_LOG_SES_EXEFAIL,MC_DEF_LOG_SES_EXEFAIL),
			verify->argv[0]);
	    }
	} else {
		LogError(ReadCatalog(
			MC_LOG_SET,MC_LOG_NO_CMDARG,MC_DEF_LOG_NO_CMDARG));
	}
	
	


	/*
	 * specify a font for the multi-byte languages...
	 */
	    
	font = NULL;
	lang = getEnv (verify->userEnviron, "LANG");



	i = 0;
	if ( password ) {
#if defined(_AIX)
	    failsafeArgv[i++] = "/usr/bin/X11/aixterm";
#elif defined(sun)
	    failsafeArgv[i++] = "/usr/openwin/bin/xterm";
#elif defined (USL) || defined(__uxp__)
            failsafeArgv[i++] = "/usr/X/bin/xterm";
#elif defined(__hpux)
	    failsafeArgv[i++] = "/usr/bin/X11/hpterm";
#elif defined(CSRG_BASED)
	    failsafeArgv[i++] = "/usr/local/bin/xterm";
#else
	    failsafeArgv[i++] = "/usr/bin/X11/xterm";
#endif
	    failsafeArgv[i++] = "-geometry";
	    failsafeArgv[i++] = "80x10";
	    failsafeArgv[i++] = "-bg";
	    failsafeArgv[i++] = "white";
	    failsafeArgv[i++] = "-fg";
	    failsafeArgv[i++] = "black";
#ifdef _AIX
	    /* aixterm requires -lang option. */
	    failsafeArgv[i++] = "-lang";
	    failsafeArgv[i++] = lang;
#else /* _AIX */
	    failsafeArgv[i++] = "-fn";

	    if (font == NULL) font = "fixed";
	    failsafeArgv[i++] = font;
#endif /* _AIX */

	    failsafeArgv[i++] = "-e";
	    failsafeArgv[i++] = "/bin/passwd";
#if defined (__apollo) || defined(__PASSWD_ETC)
	    failsafeArgv[i++] = "-n";
#endif
	    failsafeArgv[i++] = getEnv (verify->userEnviron, "USER");
	}
	else {
	    failsafeArgv[i++] = d->failsafeClient;
#ifdef sun 
	    failsafeArgv[i++] = "-C";
#endif
	    failsafeArgv[i++] = "-ls";

	    if (font != NULL) {
		failsafeArgv[i++] = "-fn";
		failsafeArgv[i++] = font;
	    }
	}

	failsafeArgv[i] = 0;
	Debug ("Executing failsafe session\n", failsafeArgv[0]);
	execute (failsafeArgv, verify->userEnviron);
	exit (1);
    case -1:
	Debug ("StartSession(): fork failed\n");
	LogError(ReadCatalog(MC_LOG_SET,MC_LOG_NO_SESFORK,MC_DEF_LOG_NO_SESFORK),
		d->name);
	return 0;
    default:
	Debug ("StartSession(): fork succeeded, pid = %d\n", pid);
	*pidp = pid;

	return 1;
    }
}

static jmp_buf	tenaciousClient;

static SIGVAL
waitAbort( int arg )
{
	longjmp (tenaciousClient, 1);
}

#if defined(SYSV) || defined(SVR4)
# include	<ctype.h>
#define killpg(pgrp, sig) kill(-(pgrp), sig)
#endif /* SYSV */

static int 
AbortClient( int pid )
{
    int	sig = SIGTERM;
#ifdef __STDC__
    volatile int	i;
#else
    int	i;
#endif
    int	retId;
    for (i = 0; i < 4; i++) {
	if (killpg (pid, sig) == -1) {
	    switch (errno) {
	    case EPERM:
		LogError(ReadCatalog(
			MC_LOG_SET,MC_LOG_NO_KILLCL,MC_DEF_LOG_NO_KILLCL));
	    case EINVAL:
	    case ESRCH:
		return;
	    }
	}
	if (!setjmp (tenaciousClient)) {
	    (void) signal (SIGALRM, waitAbort);
	    (void) alarm ((unsigned) 10);
	    retId = wait ((waitType *) 0);
	    (void) alarm ((unsigned) 0);
	    (void) signal (SIGALRM, SIG_DFL);
	    if (retId == pid)
		break;
	} else
	    signal (SIGALRM, SIG_DFL);
	sig = SIGKILL;
    }
}

int 
source( struct verify_info *verify, char *file )
{
    char	*args[2];
    int		pid;
    waitType	result;

    if (file && file[0]) {
	Debug ("Source(): %s\n", file);
	switch (pid = fork ()) {
	case 0:
	    CleanUpChild ();
	    args[0] = file;
	    args[1] = NULL;
	    execute (args, verify->systemEnviron);
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_EXE,MC_DEF_LOG_NO_EXE),args[0]);
	    exit (1);
	case -1:
	    Debug ("Source(): fork failed\n");
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_FORK,MC_DEF_LOG_NO_FORK),file);
	    return 1;
	    break;
	default:
	    while (wait (&result) != pid)
		    ;
	    break;
	}
	return waitVal (result);
    }
    return 0;
}

int 
execute(char **argv, char **environ )
{

    /*
     *  make stdout follow stderr to the log file...
     */

    dup2 (2,1);
    
    session_execve (argv[0], argv, environ);

    /*
     * In case this is a shell script which hasn't been made executable
     * (or this is a SYSV box), do a reasonable thing...
     */

#ifdef _AIX
    /* errno is EACCES if not executable */
    if (errno == ENOEXEC || errno == EACCES) {
#else /* _AIX */
    if (errno == ENOEXEC) {
#endif /* _AIX */
	char	program[1024], *e, *p, *optarg;
	FILE	*f;
	char	**newargv, **av;
	int	argc;

	/*
	 * emulate BSD kernel behaviour -- read
	 * the first line; check if it starts
	 * with "#!", in which case it uses
	 * the rest of the line as the name of
	 * program to run.  Else use "/bin/sh".
	 */
	f = fopen (argv[0], "r");
	if (!f)
	    return;
	if (fgets (program, sizeof (program) - 1, f) == NULL)
 	{
	    fclose (f);
	    return;
	}
	fclose (f);
	e = program + strlen (program) - 1;
	if (*e == '\n')
	    *e = '\0';
	if (!strncmp (program, "#!", 2)) {
	    p = program + 2;
	    while (*p && isspace (*p))
		++p;
	    optarg = p;
	    while (*optarg && !isspace (*optarg))
		++optarg;
	    if (*optarg) {
		*optarg = '\0';
		do
		    ++optarg;
		while (*optarg && isspace (*optarg));
	    } else
		optarg = 0;
	} else {
	    p = "/bin/sh";
	    optarg = 0;
	}
	Debug ("Shell script execution: %s (optarg %s)\n",
		p, optarg ? optarg : "(null)");
	for (av = argv, argc = 0; *av; av++, argc++)
		;
	newargv = (char **) malloc ((argc + (optarg ? 3 : 2)) * sizeof (char *));
	if (!newargv)
	    return -1;
	av = newargv;
	*av++ = p;
	if (optarg)
	    *av++ = optarg;
	while (*av++ = *argv++)
	    ;
	session_execve (newargv[0], newargv, environ);
    }
}




/*****************************************************************************
*    RunGreeter
*    
*    Invoke the Greeter process and wait for completion. If the user was 
*    successfully verified, return to the calling process. If the user
*    selected a restart or abort option, or there was an error invoking the
*    Greeter, exit this entire process with appropriate status.
*
*****************************************************************************/

#define MSGSIZE 512

extern int session_set;	
extern char *progName;	/* Global argv[0]; dtlogin name and path */

int         response[2], request[2];

/* Fixes problem with dtlogin signal handling */
static int greeterPid = 0;
static struct display *greeter_d = NULL;

static SIGVAL
catchHUP(int arg)
{
    Debug("Caught SIGHUP\n");
    if (greeterPid) 
    {
	Debug("Killing greeter process:  %d\n", greeterPid);
	kill(greeterPid, SIGHUP);
    }
    if (greeter_d)
      SessionExit(greeter_d, REMANAGE_DISPLAY);
    else
      exit(REMANAGE_DISPLAY);
}

static void 
RunGreeter( struct display *d, struct greet_info *greet,
            struct verify_info *verify )
{

    int       pid;
    waitType	status;
    
    int		rbytes;
    static char	msg[MSGSIZE];
    char	*p;
    char	**env;
    char	*path;
    struct greet_state state;
    int 	notify_dt;

#ifdef __PASSWD_ETC
#  ifndef U_NAMELEN
#    define U_NAMELEN	sizeof(rgy_$name_t)
#  endif

    int i;
    static char name_short[U_NAMELEN];
#endif

#ifdef SIA

    int argc = 1;
    char *argv[] = { "dtlogin", 0 };
    char *hostName = NULL;
    char *loginName = NULL;
    int siaStatus = -1;

    /*
     * Initialize SIA
     */
    
    if (d->serverPid == -1)
      hostName = d->name;

    siaStatus = sia_ses_init(&siaHandle, argc, argv, hostName, 
                              loginName, d->name, 1, NULL);
    if (siaStatus != SIASUCCESS)
      {
      Debug("sia_ses_init failure status %d\n", siaStatus);
      exit(1);
      }

#endif

    greeterPid = 0;
    if (!setjmp (abortSession)) {
	signal(SIGTERM, catchTerm);

	/*
	 * We've changed dtlogin to pass HUP's down to the children
	 * so ignore any HUP's once the client has started.
	 */
	greeter_d = d;
        signal(SIGHUP, catchHUP);

	/*
	 *  set up communication pipes...
	 */
	 
	pipe(response);
	pipe(request);
	rbytes = 0;


	switch (greeterPid = fork ()) {
	case 0:

	    /*
	     *  pass some information in the environment...
	     */
		   
	    env = 0;
	    sprintf(msg,"%d", d->grabServer);
	    env = setEnv(env, GRABSERVER,  msg);
	
	    sprintf(msg,"%d", d->grabTimeout);
	    env = setEnv(env, GRABTIMEOUT, msg);


	    if (timeZone && strlen(timeZone) > 0 )
		env = setEnv(env, "TZ", timeZone);

	    if (errorLogFile && errorLogFile[0])
		env = setEnv(env, ERRORLOG,  errorLogFile);

	    if (d->authFile)
		env = setEnv(env, "XAUTHORITY", d->authFile);

	    if (d->dtlite)
		env = setEnv(env, DTLITE,  "True");

	    if (d->session)  
		env = setEnv(env, SESSION,  d->session);

	    if(session_set)
		env = setEnv(env, SESSION_SET, "True");

            if (d->pmSearchPath)
                env = setEnv(env, "XMICONSEARCHPATH", d->pmSearchPath);

            if (d->bmSearchPath)
                env = setEnv(env, "XMICONBMSEARCHPATH", d->bmSearchPath);

#if defined (__KERBEROS) || defined (__AFS)
	    if (d->verifyName) {
		if ( (strcmp(d->verifyName, VN_AFS) == 0) ||
		     (strcmp(d->verifyName, VN_KRB) == 0)    ) {
		     
		    env = setEnv(env, VERIFYNAME, d->verifyName );
		}
		else {
		    LogError(ReadCatalog(
			MC_LOG_SET,MC_LOG_IMPROP_AUTH,MC_DEF_LOG_IMPROP_AUTH),
			d->verifyName);
		    d->verifyName = NULL;
		}
	    }
#endif
	    		
            if((path = getenv("NLSPATH")) != NULL)
                env = setEnv(env, "NLSPATH", path);
#ifdef __hp_osf
	    env = setEnv(env, "NLSPATH", "/usr/lib/nls/msg/%L/%N.cat");
#endif

	    
	    /*
	     *  ping remote displays...
	     *
	     */

	    if (d->displayType.location == Local) {
                GettyRunning(d); /* refresh gettyState */
		if (d->gettyState != DM_GETTY_USER)
		    env = setEnv(env, LOCATION, "local");
	    }
	    else {
		sprintf(msg,"%d", d->pingInterval);
		env = setEnv(env, PINGINTERVAL,  msg);
	
		sprintf(msg,"%d", d->pingTimeout);
		env = setEnv(env, PINGTIMEOUT, msg);
	    }


	    if ( d->langList && strlen(d->langList) > 0 )
		env = setEnv(env, LANGLIST,  d->langList);
#if !defined (ENABLE_DYNAMIC_LANGLIST)
	    else if (languageList && strlen(languageList) > 0 )
		env = setEnv(env, LANGLIST, languageList);
#endif /* ENABLE_DYNAMIC_LANGLIST */

            {
		char *language = NULL;

#if defined (ENABLE_DYNAMIC_LANGLIST)
                language = d->language;
#endif /* ENABLE_DYNAMIC_LANGLIST */

	        if ( d->language && strlen(d->language) > 0 )
		  env = setLang(d, env, language);
	    }

            if((path = getenv("XKEYSYMDB")) != NULL)
                env = setEnv(env, "XKEYSYMDB", path);

#ifdef sun
            if((path = getenv("OPENWINHOME")) != NULL)
                env = setEnv(env, "OPENWINHOME", path);
#endif

#ifdef __apollo
	    /*
	     *  set environment for Domain machines...
	     */
	    env = setEnv(env, "ENVIRONMENT", "bsd");
	    env = setEnv(env, "SYSTYPE", "bsd4.3");
#endif
		

	    Debug ("Greeter environment:\n");
	    printEnv(env);
	    Debug ("End of Greeter environment:\n");

	   /*
	    * Writing to file descriptor 1 goes to response pipe instead.
	    */
	    close(1);
	    dup(response[1]);
	    close(response[0]);
	    close(response[1]);

	   /*
	    * Reading from file descriptor 0 reads from request pipe instead.
	    */
	    close(0);
	    dup(request[0]);
	    close(request[0]);
	    close(request[1]);

 	    CleanUpChild ();

	    /*
	     * figure out path to dtgreet...
	     */

	    strcpy(msg, progName);
    
	    if ((p = (char *) strrchr(msg, '/')) == NULL)
		strcpy(msg,"./");
	    else
		*(++p) = NULL;

	    strcat(msg,"dtgreet");

	    execle(msg, "dtgreet", "-display", d->name, (char *)0, env);
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_DTGREET,MC_DEF_LOG_NO_DTGREET),
		 msg, d->name);
	    exit (NOTIFY_ABORT_DISPLAY);

	case -1:
	    Debug ("Fork of Greeter failed.\n");
	    LogError(ReadCatalog(
		MC_LOG_SET,MC_LOG_NO_FORKCG,MC_DEF_LOG_NO_FORKCG),d->name);
            close(request[0]);
            close(request[1]);
            close(response[0]);
            close(response[1]);
	    exit (UNMANAGE_DISPLAY);

	default:
	    Debug ("Greeter started\n");

	    close(response[1]); /* Close write end of response pipe */
	    close(request[0]); /* Close read end of request pipe */


            /*
             * Retrieve information from greeter and authenticate.
             */
            globalDisplayName = d->name;
            state.id = GREET_STATE_ENTER;
            state.waitForResponse = FALSE;
#ifdef SIA
           
            /*
             * atexit() registers this function to be called if exit() is
             * called.  This is needed because in enhanced security mode, SIA
             * may call exit() whn the user fails to enter or change a
             * password.
             */
            sia_greeter_pid = greeterPid;
            if (!sia_exit_proc_reg)
                {
                atexit(KillGreeter);
                sia_exit_proc_reg = TRUE;
                }
  
            siaGreeterInfo.d = d;
            siaGreeterInfo.greet = greet;
            siaGreeterInfo.verify = verify;
            siaGreeterInfo.state = &state;
            siaGreeterInfo.status = TRUE;
  
            siaStatus =  -1;
            while(siaStatus != SIASUCCESS)
                {
                while(siaStatus != SIASUCCESS && siaGreeterInfo.status)
                    {
                    Debug ("RunGreeter: before sia_ses_authent\n");
                    dt_in_sia_ses_authent = True;
                    siaStatus = sia_ses_authent(SiaManageGreeter, NULL, 
                                                siaHandle);
                    dt_in_sia_ses_authent = False;
                    Debug ("RunGreeter: after sia_ses_authent status = %d\n",
                                siaStatus);
                    if (siaStatus == SIAFAIL && siaGreeterInfo.status)
                        {
                        state.id = GREET_STATE_ERRORMESSAGE;
                        state.vf = VF_INVALID;
                        ManageGreeter(d, greet, verify, &state);
                        }
                    if (!siaGreeterInfo.status || siaStatus == SIASTOP)
                        break;
                    }
                if (!siaGreeterInfo.status || siaStatus == SIASTOP)
                    {
                    sia_ses_release(&siaHandle);
                    break;
                    }
  
                Debug("RunGreeter: before sia_ses_estab\n");
                siaStatus = sia_ses_estab(SiaManageGreeter, siaHandle);
                Debug ("RunGreeter: after sia_ses_estab status = %d\n",
                                siaStatus);
  
                if (!siaGreeterInfo.status)
                    break;
  
                if (siaStatus == SIASUCCESS)
                    {
                    Debug("RunGreeter: before sia_ses_launch\n");
                    siaStatus = sia_ses_launch(SiaManageGreeter, siaHandle);
                    Debug("RunGreeter: after sia_ses_launch status = %d\n",
                                siaStatus);
                    }
                if (!siaGreeterInfo.status)
                    break;
  
                if (siaStatus != SIASUCCESS)
                    {
                    Debug("RunGreeter: sia_ses_launch failure\n");
                    /* establish & launch failures do a release */
                    siaHandle = NULL;
                    siaStatus = sia_ses_init(&siaHandle, argc, argv, hostName, 
                                                loginName, d->name, 1, NULL);
                    if (siaStatus != SIASUCCESS)
                        {
                        Debug("sia_ses_init failure status %d\n", siaStatus);
                        exit(RESERVER_DISPLAY);
                        }
                    }
                }
            /*
             * sia_ses_launch() wil probably seteuid to that of the
             * user, but we don't want that now.
             */
            seteuid(0);
            /*
             * extract necessary info from SIA context struct
             */
            if (siaHandle)
                {
                if (siaStatus == SIASUCCESS)
                    CopySiaInfo(siaHandle, greet);
                sia_ses_release(&siaHandle);
                }
            state.id = GREET_STATE_TERMINATEGREET;
            if (siaGreeterInfo.status)
                {
                while (ManageGreeter(d, greet, verify, &state))
                  ;
                }
            sia_greeter_pid = 0;
#else
            while (ManageGreeter(d, greet, verify, &state))
              ;
#endif /* SIA */

	    /*
	     * Wait for Greeter to end...
	     */
	    for (;;) {
		pid = wait (&status);
		if (pid == greeterPid)
		    break;
	    }


	    /*
	     *  Greeter exited. Check return code...
	     */
	     
	    Debug("Greeter return status; exit = %d, signal = %d\n", 
	    	   waitCode(status), waitSig(status));


	    /*
	     * remove authorization file if used...
	     */
     
	    if (d->authorizations && d->authFile &&
	        waitVal(status) != NOTIFY_LANG_CHANGE
#ifdef BLS
		&& waitVal(status) != NOTIFY_BAD_SECLEVEL
#endif
	       ) {

/***
	    	Debug ("Done with authorization file %s, removing\n",
			d->authFile);
		(void) unlink (d->authFile);
***/
	    }


		if(waitVal(status) > NOTIFY_ALT_DTS) 
		      d->sessionType = waitVal(status);


	    switch (waitVal(status)) {
	    case NOTIFY_FAILSAFE:
	        greet->string = "failsafe";
		break;
	    case NOTIFY_PASSWD_EXPIRED:
	        greet->string = "password";
		break;
	    case NOTIFY_DTLITE:
	    case NOTIFY_DT:
	    case NOTIFY_OK:
	    case NOTIFY_LAST_DT:
	        d->sessionType = waitVal(status);
		break;
	    default:
	        break;
	    }

	  Debug("waitVal - status is  %d\n", waitVal(status));
	   if(waitVal(status) > NOTIFY_ALT_DTS)
		notify_dt = NOTIFY_ALT_DTS; /* It is alt desktops */
	    else
		notify_dt = waitVal(status);

	    switch (notify_dt) {
	    case NOTIFY_FAILSAFE:
	    case NOTIFY_PASSWD_EXPIRED:
	    case NOTIFY_DTLITE:
	    case NOTIFY_DT:
	    case NOTIFY_OK:
	    case NOTIFY_LAST_DT:
	    case NOTIFY_ALT_DTS:

	       if (NULL == greet->name) return;

               /*
                *  greet->name, greet->password set in ManageGreeter().
                */
		Debug("Greeter returned name '%s'\n", greet->name);

#ifdef __PASSWD_ETC
		greet->name_full = greet->name;
                           /* get just person name out of full SID */
                i = 0;
                while (i < sizeof(rgy_$name_t) 
		     && greet->name_full[i] != '.'
                     && greet->name_full[i] != '\0') {
                     name_short[i] = greet->name_full[i];
                     i++;
                }
                name_short[i] = '\0';
                greet->name = name_short;
#endif
		    
#ifdef __AFS
               /*
                * groups[] set in Authenticate().
                */
		if ( IsVerifyName(VN_AFS) ) {
		  verify->groups[0] = groups[0];
		  verify->groups[1] = groups[1];
		  Debug("Greeter returned groups[0] '%d'\n", verify->groups[0]);
		  Debug("Greeter returned groups[1] '%d'\n", verify->groups[1]);
		}
#endif
		    
#ifdef BLS
               /*
                * sensitivityLevel set in BLS_Verify()
                */
		greet->b1security = sensitivityLevel;
#endif

		Verify(d, greet, verify);
		return;

	    case NOTIFY_ABORT:
		Debug ("Greeter Xlib error or SIGTERM\n");
		SessionExit(d, OPENFAILED_DISPLAY);

	    case NOTIFY_RESTART:
	    	Debug ("Greeter requested RESTART_DISPLAY\n");
		SessionExit(d, RESERVER_DISPLAY);

	    case NOTIFY_ABORT_DISPLAY:
		Debug ("Greeter requested UNMANAGE_DISPLAY\n");
		SessionExit(d, UNMANAGE_DISPLAY);

	    case NOTIFY_NO_WINDOWS:
	        Debug ("Greeter requested NO_WINDOWS mode\n");
		if (d->serverPid >= 2)
		    /*
		     *  Don't do a SessionExit() here since that causes
		     *  the X-server to be reset. We know we are going to
		     *  terminate it anyway, so just go do that...
		     */
		    exit(SUSPEND_DISPLAY);
		else
		    return;

	    case NOTIFY_LANG_CHANGE:
		Debug ("Greeter requested LANG_CHANGE\n");

	       /*
		* copy requested language into display struct "d". Note,
		* this only happens in this child's copy of "d", not in
		* the master struct. When the user logs out, the
		* resource-specified language (if any) will reactivate.
		*/
		Debug("Greeter returned language '%s'\n", d->language);

		if (strcmp(d->language, "default") == 0) {
		    int len = strlen(defaultLanguage) + 1;
		    d->language = (d->language == NULL  ?
				   malloc(len) : realloc (d->language, len));
		    strcpy(d->language, defaultLanguage);
		}
		return;
#ifdef	BLS
	    case NOTIFY_BAD_SECLEVEL:
		return;
#endif
	    case waitCompose (SIGTERM,0,0):
		Debug ("Greeter exited on SIGTERM\n");
		SessionExit(d, OPENFAILED_DISPLAY);
		
	    default:
		Debug ("Greeter returned unknown status %d\n", 
			waitVal(status));
		SessionExit(d, REMANAGE_DISPLAY);
	    }
	}
        signal(SIGHUP, SIG_DFL);
    }
    else {
	AbortClient(greeterPid);
	SessionExit(d, UNMANAGE_DISPLAY);
    }
}

/*****************************************************************************
* ManageGreeter
 
  State transitions
 
  enter -> ENTER: 
    This is the entry into greeter state processing. Allocate and initialize
    state structure.

  ENTER -> LOGIN:
    Display the login screen. Upon display, the login screen can be 'reset'. If
    reset is true, the username and password fields are cleared and the focus
    is set to the username field. If reset is false, the username and password
    field is untouched and the focus is set to the password field.

  LOGIN -> AUTHENTICATE:
    Authenticate the username entered on login screen. 

  AUTHENTICATE -> TERMINATEGREET:
    User passed authentication so terminate the greeter.

  AUTHENTICATE -> EXPASSWORD: 
    User passed authentication, but the their password has expired.
    Display old password message. This message allows the user to 
    change their password by starting a getty and running passwd(1).  

  AUTHENTICATE -> BAD_HOSTNAME:
    User passed authentication, but the their hostname is empty.
    Display a dialog that allows the user to run a getty to fix the
    problem, or start the desktop anyway.

  AUTHENTICATE -> ERRORMESSAGE:
    User failed authentication, so display error message. 

  AUTHENTICATE -> LOGIN
    User failed authentication, but did not enter a password. Instead
    of displaying an error message, redisplay the login screen with
    the focus set to the password field. If the user authenticates again
    without the password field set, display an error. This allows a user
    to type the ubiquitous "username<ENTER>password<ENTER>" sequence.

  EXIT -> exit
    Free state structure and return false to stop state transitions.

  ERRORMESSAGE -> LOGIN
    Display error message base on return code from last authentication
    attempt. Redisplay login screen with reset set to true.

  (state) -> LANG -> (state)
    User has chosen a new language. Transition to LANG state to save off
    the new language, and transition back to original state. 
  
*****************************************************************************/

#define SETMC(M, ID) M.id = MC_##ID; M.def = MC_DEF_##ID

static int
ManageGreeter( struct display *d, struct greet_info *greet,
            struct verify_info *verify, struct greet_state *state )
{
  struct {
    int id;
    char *def;
  } msg;

  if (state->waitForResponse)
  {
    if (!AskGreeter(NULL, (char *)state->response, REQUEST_LIM_MAXLEN))
    {
     /*
      * Dtgreet has terminated. 
      */
      state->id = GREET_STATE_EXIT;
      state->waitForResponse = FALSE;
      return(TRUE);
    }

    if (state->request->opcode != state->response->opcode)
    {
     /*
      * An unrequested event arrived. See if it's one we
      * are expecting.
      */
      switch(state->response->opcode)
      {
        case REQUEST_OP_LANG:
          {
           /*
            * User has changed language. Recursively handle this state
            * and return to current state.
            */
            struct greet_state lang_state;

            lang_state = *state;
            lang_state.id = GREET_STATE_LANG;
            lang_state.waitForResponse = FALSE;
            ManageGreeter(d, greet, verify, &lang_state);
            Debug("Response opcode REQUEST_OP_LANG\n");
            return(TRUE);
          }
          break;

        case REQUEST_OP_CLEAR:
          {
           /*
            * User has requested the screen be cleared.
            */
            state->id = GREET_STATE_USERNAME;
            state->waitForResponse = TRUE;
            Debug("Response opcode REQUEST_OP_CLEAR\n");
          }
          break;
    
        default:
          Debug("Response opcode UNEXPECTED RESPONSE!\n");
#ifndef SIA
          return(TRUE);
#endif
          break;
      }
    }
    else
    {
     /*
      * Got the response we were expecting.
      */
      state->waitForResponse = FALSE;
    }
  }

  switch(state->id)
  {
    case GREET_STATE_ENTER:
      {
       /*
        * Enter - initialize state
        */
        Debug("GREET_STATE_ENTER\n");

        state->request = (RequestHeader *)malloc(REQUEST_LIM_MAXLEN);
        state->response= (ResponseHeader *)malloc(REQUEST_LIM_MAXLEN);
        state->authenticated = FALSE;
        state->msg = NULL;

        state->id = GREET_STATE_USERNAME;
      }
      break;

    case GREET_STATE_USERNAME:
      {
       /*
        * Get user name
        */
        RequestChallenge *r;

        Debug("GREET_STATE_USERNAME\n");

        Authenticate(d, NULL, NULL, NULL);

        SETMC(msg, LOGIN_LABEL);

        r = (RequestChallenge *)state->request;
        r->hdr.opcode = REQUEST_OP_CHALLENGE;
        r->hdr.reserved = 0;
        r->bEcho = TRUE;
        r->idMC = msg.id;
        r->hdr.length = sizeof(*r);

        r->offChallenge = sizeof(*r);
        strcpy(((char *)r) + r->offChallenge, msg.def);
        r->hdr.length += strlen(msg.def) + 1;

        if (greet->name)
        {
          r->offUserNameSeed = r->hdr.length;
          strcpy(((char *)r) + r->offUserNameSeed, greet->name);
          r->hdr.length += strlen(greet->name) + 1;
          Debug("Greet name:  %s\n", greet->name);
        }
        else
        {
          r->offUserNameSeed = 0;
        }

        if (greet->name)
        {
          free(greet->name); greet->name = NULL;
        }
        if (greet->password)
        {
          free(greet->password); greet->password = NULL;
        }

        TellGreeter((RequestHeader *)r);
        state->waitForResponse = TRUE;

        state->id = GREET_STATE_AUTHENTICATE;
      }
      break;

    case GREET_STATE_CHALLENGE:
      {
       /*
        * Get user name
        */
        RequestChallenge *r;

        Debug("GREET_STATE_CHALLENGE\n");

        if (greet->password)
        {
          free(greet->password); greet->password = NULL;
        }

        SETMC(msg, PASSWD_LABEL);

        r = (RequestChallenge *)state->request;
        r->hdr.opcode = REQUEST_OP_CHALLENGE;
        r->hdr.reserved = 0;
        r->bEcho = FALSE;
        r->idMC = msg.id;
        r->offUserNameSeed = 0;
        r->offChallenge = sizeof(*r);
        strcpy(((char *)r) + r->offChallenge, msg.def);
        r->hdr.length = sizeof(*r) + strlen(msg.def) + 1;

        TellGreeter((RequestHeader *)r);
        state->waitForResponse = TRUE;

        state->id = GREET_STATE_AUTHENTICATE;
      }
      break;

    case GREET_STATE_AUTHENTICATE:
      {
       /*
        * Attempt to authenticate.
        */
        ResponseChallenge *r;
   
        Debug("GREET_STATE_AUTHENTICATE\n"); 

        r = (ResponseChallenge *)state->response;
     
        if (greet->name == NULL)
        {
          greet->name = strdup(((char *)r) + r->offResponse);
          if (strlen(greet->name) == 0)
          {
            state->id = GREET_STATE_USERNAME;
            break;
          }
        }
        else 
        { 
          greet->password = strdup(((char *)r) + r->offResponse);
        }

        if (state->msg)
        {
          free(state->msg);
          state->msg = NULL;
        }

       /*
        * Attempt to authenticate user. 'username' should be a 
        * non-empty string. 'password' may be an empty string.
        */
        state->vf = Authenticate(d, greet->name, greet->password, &state->msg);

        if (state->vf == VF_OK || 
            state->vf == VF_PASSWD_AGED || 
            state->vf == VF_BAD_HOSTNAME)
        {
          state->authenticated = TRUE;
        }

       /*
        * General transitions.
        */
        switch (state->vf)
        {
          case VF_OK:           state->id = GREET_STATE_TERMINATEGREET; break; 
          case VF_PASSWD_AGED:  state->id = GREET_STATE_EXPASSWORD; break; 
          case VF_BAD_HOSTNAME: state->id = GREET_STATE_BAD_HOSTNAME; break; 
          case VF_CHALLENGE:    state->id = GREET_STATE_CHALLENGE; break; 
          default:              state->id = GREET_STATE_ERRORMESSAGE; break; 
        }
      }
      break;

    case GREET_STATE_EXIT: 
      {
       /*
        * Free resources and leave.
        */
        Debug("GREET_STATE_EXIT\n");

        if (state->msg)
        {
          free(state->msg);
        }
 
        if (!state->authenticated)
        {
          if (greet->name)
          {
            free(greet->name); greet->name = NULL;
          }
          if (greet->password)
          {
            free(greet->password); greet->password = NULL;
          }
        }

        free(state->request);
        free(state->response);
        return(FALSE);
      }
      break;

    case GREET_STATE_ERRORMESSAGE:
      {
       /* 
        * Display error message.
        */
        RequestMessage *r;

        Debug("GREET_STATE_ERRORMESSAGE\n");

        r = (RequestMessage *)state->request;

        switch(state->vf)
        {
          case VF_INVALID:       SETMC(msg, LOGIN); break;
          case VF_HOME:          SETMC(msg, HOME); break;
          case VF_MAX_USERS:     SETMC(msg, MAX_USERS); break;
          case VF_BAD_UID:       SETMC(msg, BAD_UID); break;
          case VF_BAD_GID:       SETMC(msg, BAD_GID); break;
          case VF_BAD_AID:       SETMC(msg, BAD_AID); break;
          case VF_BAD_AFLAG:     SETMC(msg, BAD_AFLAG); break;
          case VF_NO_LOGIN:      SETMC(msg, NO_LOGIN); break;
#ifdef BLS
          case VF_BAD_SEN_LEVEL: SETMC(msg, BAD_SEN_LEVEL); break;
#endif
          case VF_MESSAGE: msg.id=0; msg.def=state->msg; break;
          default: msg.id=0; msg.def=""; break;
        }

        r->hdr.opcode = REQUEST_OP_MESSAGE;
        r->hdr.reserved = 0;
        r->idMC = msg.id;
        r->offMessage = sizeof(*r);
        strcpy(((char *)r) + r->offMessage, msg.def);
        r->hdr.length = sizeof(*r) + strlen(msg.def) + 1;

        TellGreeter((RequestHeader *)r);
        state->waitForResponse = TRUE;

        state->id = GREET_STATE_USERNAME;
      }
      break;

    case GREET_STATE_LANG:
      {
       /*
        * User selected new language.
        */
        ResponseLang *r;
        char *lang;
        int len;

        Debug("GREET_STATE_LANG\n");

        r = (ResponseLang *)state->response;
        lang = ((char *)r) + r->offLang;
        len = strlen(lang) + 1;

        d->language = (d->language == NULL ?
                       malloc(len) : realloc(d->language, len));
        strcpy(d->language, lang);
        Debug("Language returned:  %s\n", d->language);
      }
      break;

    case GREET_STATE_TERMINATEGREET:
      {
       /*
        * Terminate dtgreet.
        */
        RequestExit *r;

        Debug("GREET_STATE_TERMINATEGREET\n");

        r = (RequestExit *)state->request;

        r->hdr.opcode = REQUEST_OP_EXIT;
        r->hdr.reserved = 0;
        r->hdr.length = sizeof(*r);

        TellGreeter((RequestHeader *)r);
        state->waitForResponse = TRUE;

        state->id = GREET_STATE_EXIT;
      }
      break;

    case GREET_STATE_EXPASSWORD:
      {
       /*
        * Display password expired message.
        */
        RequestExpassword *r;

        Debug("GREET_STATE_EXPASSWORD\n");

        r = (RequestExpassword *)state->request;

        r->hdr.opcode = REQUEST_OP_EXPASSWORD;
        r->hdr.reserved = 0;
        r->hdr.length = sizeof(*r);

        TellGreeter((RequestHeader *)r);
        state->waitForResponse = TRUE;

        state->id = GREET_STATE_USERNAME;
      }
      break;

    case GREET_STATE_BAD_HOSTNAME:
      {
       /*
        * Display password expired message.
        */
        RequestHostname *r;

        Debug("GREET_STATE_BAD_HOSTNAME\n");

        r = (RequestHostname *)state->request;

        r->hdr.opcode = REQUEST_OP_HOSTNAME;
        r->hdr.reserved = 0;
        r->hdr.length = sizeof(*r);

        TellGreeter((RequestHeader *)r);
        state->waitForResponse = TRUE;

        state->id = GREET_STATE_USERNAME;
      }
      break;

#ifdef SIA
    case GREET_STATE_FORM:
      {
       /*
        * Get arbitrary number of answers.
        */

        Debug("GREET_STATE_FORM\n");
 
        AskGreeter(state->request, (char *)state->response, REQUEST_LIM_MAXLEN);
 
        state->waitForResponse = FALSE;
        state->id = GREET_STATE_USERNAME;
      }
      break;
#endif /* SIA */
  }

  return(TRUE);
}


static void
TellGreeter(
  RequestHeader *phdr)
{
  write(request[1], phdr, phdr->length);
}

static int
AskGreeter(
  RequestHeader *preqhdr,
  char *buf,
  int blen)
{
  int count;
  int remainder;
  ResponseHeader *phdr = (ResponseHeader *)buf;

  if (preqhdr) TellGreeter(preqhdr);

  phdr->opcode = REQUEST_OP_NONE;

  count = read(response[0], buf, sizeof(*phdr));

  if (count == sizeof(*phdr))
  {
   /*
    * Calculate amount of data after header.
    */
    remainder =  phdr->length - sizeof(*phdr);
    if (remainder > 0)
    {
     /*
      * Read remainder of response.
      */
      count += read(response[0], buf+sizeof(*phdr), remainder);
    }
  }

#if defined (DEBUG)
  if (debugLevel) PrintResponse(phdr, count);
#endif /* DEBUG */

  return(count);
}

#if defined (DEBUG)
static void
PrintResponse(
  ResponseHeader *phdr,
  int count)
{
  char *opstr = "UNKNOWN";

  if (!count)
  {
    Debug("opcode = (EOF)\n");
    return;
  }

  switch(phdr->opcode)
  {
    case REQUEST_OP_EXIT: opstr = "EXIT"; break;
    case REQUEST_OP_MESSAGE: opstr = "MESSAGE"; break;
    case REQUEST_OP_CHPASS: opstr = "CHPASS"; break;
    case REQUEST_OP_CHALLENGE: opstr = "CHALLENGE"; break;
    case REQUEST_OP_LANG: opstr = "LANG"; break;
    case REQUEST_OP_DEBUG: opstr = "DEBUG"; break;
  }

  Debug("opcode = %d (%s)\n", phdr->opcode, opstr);
  Debug("  reserved = %d\n", phdr->reserved);
  Debug("  length = %d\n", phdr->length);

  switch(phdr->opcode)
  {
    case REQUEST_OP_EXIT: break;
    case REQUEST_OP_LANG:
      Debug("  offLang=%d\n", ((ResponseLang *)phdr)->offLang);
      Debug("  lang='%s'\n",
        ((char *)phdr)+((ResponseLang *)phdr)->offLang);
      break;
    case REQUEST_OP_MESSAGE: break;
    case REQUEST_OP_CHPASS: break;
    case REQUEST_OP_CHALLENGE:
      Debug("  offResponse=%d\n", ((ResponseChallenge *)phdr)->offResponse);
      Debug("  response='%s'\n",
        ((char *)phdr)+((ResponseChallenge *)phdr)->offResponse);
      break;
    case REQUEST_OP_DEBUG:
      Debug("  offString=%d\n", ((ResponseDebug *)phdr)->offString);
      Debug("  string='%s'\n",
        ((char *)phdr)+((ResponseDebug *)phdr)->offString);
      break;
  }
}
#endif /* DEBUG */

#ifdef __KERBEROS

/***************************************************************************
 *
 *  SetTicketFileName
 *
 *  generate kerberos ticket file name. Name is returned in the static
 *  global variable "krb_ticket_string".
 *  
 ***************************************************************************/

static void 
SetTicketFileName(uid_t uid)
{


    char *env;
    char lhost[64], *p;


    /*
     *  generate ticket file pathname  (/tmp/tkt<uid>.<host>) ...
     */

    if (env = (char *)getenv("KRBTKFILE")) {
        (void) strncpy(krb_ticket_string, env, sizeof(krb_ticket_string)-1);
        krb_ticket_string[sizeof(krb_ticket_string)-1] = '\0';

    } else {
        if (gethostname(lhost, sizeof(lhost)) != -1) {
	    if (p = index(lhost, '.')) *p = '\0';
	    (void)sprintf(krb_ticket_string, "%s%ld.%s", TKT_ROOT, (long)uid, lhost);
        } else {
            /* 32 bits of signed integer will always fit in 11 characters
               (including the sign), so no need to worry about overflow */
	    (void) sprintf(krb_ticket_string, "%s%ld", TKT_ROOT, (long)uid);
	}
    }
}

#endif /* __KERBEROS */

#if defined (_AIX) && !defined (_POWER)

/***************************************************************************
 *
 *  session_execve
 *
 *  If this is an authenticated process (LOGNAME set), set user's 
 *  process environment by calling setpenv().
 *
 *  If this is not an authenticated process, just call execve()
 *  
 ***************************************************************************/

static int
session_execve(
  char *path,
  char *argv[],
  char *envp[])
{
  int rc;
  char *user = getEnv (envp, "LOGNAME");
 
  if (user == NULL)
  {
    rc = execve(path, argv, envp);
  }
  else
  {
    char *usrTag, *sysTag;

   /*
    * Save pointers to tags. The setpenv() function clears the pointers
    * to the tags in userEnviron as a side-effect.
    */
    sysTag = envp[SYS_ENV_TAG];
    usrTag = envp[USR_ENV_TAG];

   /*
    * Set the users process environment. This call execs arvg so it 
    * should not return. It it should return, restore the envp tags.
    */
    rc = setpenv(user, PENV_INIT | PENV_ARGV, envp, argv);

   /*
    * Restore pointers to tags.
    */
    envp[SYS_ENV_TAG] = sysTag;
    envp[USR_ENV_TAG] = usrTag;
  }

  return(rc);
}
#endif /* _AIX && !_POWER */

#ifdef SIA

/* collect the SIA parameters from a window system. */

static int SiaManageGreeter(
    int timeout,
    int rendition,
    unsigned char *title,
    int num_prompts,
    prompt_t *prompt)
{
    int i;
    struct {
      RequestMessage  greeter_message;
      char            msg_buffer[256];
      } greeter_msg_and_buffer;
    RequestForm *request_form;

    switch(rendition) 
      {
        case SIAMENUONE:
        case SIAMENUANY:
        case SIAONELINER:
        case SIAFORM:
          if (rendition == SIAFORM && dt_in_sia_ses_authent 
                                  && (num_prompts == 2))
              {
              /* Normal login, Password case */
              Debug ("SIAFORM Normal login, Password case\n");
              while (siaGreeterInfo.state->id != GREET_STATE_TERMINATEGREET
                    && siaGreeterInfo.state->id != GREET_STATE_EXIT
                    && (siaGreeterInfo.status = ManageGreeter(
                              siaGreeterInfo.d, siaGreeterInfo.greet, 
                              siaGreeterInfo.verify, siaGreeterInfo.state)))
                  ;

              if (!siaGreeterInfo.status 
                      || siaGreeterInfo.state->id == GREET_STATE_EXIT)
                  return(SIACOLABORT);

              strncpy((char *)prompt[0].result, siaGreeterInfo.greet->name, 
                      prompt[0].max_result_length);
              strncpy((char *)prompt[1].result,siaGreeterInfo.greet->password,
                      prompt[1].max_result_length);
              }
          else
              {
              char *res_ptr;
              char *pmpt_ptr;
              int req_form_size;
              ResponseForm *response_form;

              switch(rendition) 
                  {
                  case SIAMENUONE:
                      Debug("SIAMENUONE num_prompts = %d\n", num_prompts);
                      break;
                  case SIAMENUANY:
                      Debug("SIAMENUANY num_prompts = %d\n", num_prompts);
                      break;
                  case SIAONELINER:
                      Debug("SIAONELINER num_prompts = %d\n", num_prompts);
                      break;
                  case SIAFORM:
                      Debug("SIAFORM num_prompts = %d\n", num_prompts);
                      break;
                  }

              /* need to display form */

              req_form_size = sizeof(RequestForm) 
                                      + strlen((const char *)title) + 1;
              for (i=0; i<num_prompts; i++)
                  req_form_size += strlen((const char *)prompt[i].prompt) + 1;
              request_form = (RequestForm *) alloca(req_form_size);

              siaGreeterInfo.state->id = GREET_STATE_FORM;
              siaGreeterInfo.state->request = (RequestHeader *)request_form;
              /* siaGreeterInfo.state->vf = VF_MESSAGE; */

              request_form->hdr.opcode = REQUEST_OP_FORM;
              request_form->hdr.reserved = 0;
              request_form->hdr.length = req_form_size;
              request_form->num_prompts = num_prompts;
              request_form->rendition = rendition;
              request_form->offTitle = sizeof(RequestForm);
              request_form->offPrompts = sizeof(RequestForm) +
                                          strlen((const char *)title) + 1;
              strcpy((char *)request_form + request_form->offTitle, 
                      (const char *)title);

              pmpt_ptr = (char *)request_form + request_form->offPrompts;
              for (i=0; i<num_prompts; i++)
                  {
                  if (!prompt[i].prompt || prompt[i].prompt[0] == '\0')
                      *pmpt_ptr++ = '\0';
                  else
                      {
                      Debug("  prompt[%d]: %s\n", i, prompt[i].prompt);
                      strcpy(pmpt_ptr, (const char *)prompt[i].prompt);
                      pmpt_ptr += strlen((const char *)prompt[i].prompt);
                      }
                  request_form->visible[i] = 
                      (prompt[i].control_flags & SIARESINVIS) ? False : True;
                  }

              siaGreeterInfo.status = ManageGreeter(siaGreeterInfo.d, 
                                                  siaGreeterInfo.greet,
                                                  siaGreeterInfo.verify, 
                                                  siaGreeterInfo.state);

              response_form = (ResponseForm *)siaGreeterInfo.state->response;
              res_ptr = (char *)response_form + response_form->offAnswers;
              for (i = 0; i < response_form->num_answers; i++)
                  {
                  if (rendition == SIAMENUONE || rendition == SIAMENUANY)
                      {
                      if (res_ptr[0])
                          prompt[i].result = (unsigned char *)1;
                      else
                          prompt[i].result = NULL;
                      }
                  else
                      {
                      strcpy((char *)prompt[0].result, res_ptr);
                      }
                  res_ptr += strlen(res_ptr) + 1;
                  }
              if (!response_form->collect_status)
                  siaGreeterInfo.status = FALSE;
              }
            break;
        case SIAINFO:
        case SIAWARNING:

          Debug("SIAINFO or SIAWARNING %s\n", prompt[0].prompt);

          siaGreeterInfo.state->id = GREET_STATE_ERRORMESSAGE;
          siaGreeterInfo.state->request = (RequestHeader *) 
                              &greeter_msg_and_buffer.greeter_message;
          siaGreeterInfo.state->vf = VF_MESSAGE;
          siaGreeterInfo.state->msg = (char *)prompt[0].prompt;

          siaGreeterInfo.status = ManageGreeter(siaGreeterInfo.d, 
                                              siaGreeterInfo.greet,
                                              siaGreeterInfo.verify, 
                                              siaGreeterInfo.state);
            break;
        default:
            return(SIACOLABORT);
            break;
      }
    if (!siaGreeterInfo.status)
      return(SIACOLABORT);
    return(SIACOLSUCCESS);
}

static CopySiaInfo(SIAENTITY *siaHandle, struct greet_info *greet)
{

    greet->name = malloc(strlen(siaHandle->name) + 1);
    strcpy (greet->name, siaHandle->name);

    greet->password = malloc(strlen(siaHandle->password) + 1);
    strcpy (greet->password, siaHandle->password);
    

}

static void KillGreeter( void )
{
    if (sia_greeter_pid)
      AbortClient(sia_greeter_pid);
    sia_greeter_pid = 0;
}
#endif /* SIA */
