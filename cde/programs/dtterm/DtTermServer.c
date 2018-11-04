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
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include <sys/types.h>
#ifdef	TERMINAL_SERVER
#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermView.h"
#include "DtTermServer.h"
#include "TermPrimSetPty.h"
#ifdef	LOG_USAGE
#include "DtTermLogit.h"
#endif	/* LOG_USAGE */
#include <signal.h>
#include <errno.h>
#include <Dt/Service.h>

/* defines for types of service requests.  Upper case for the requestor
 * types, lower case for the server types...
 */
#define	SVC_SUCCESS	'S'
#define	SVC_FAIL	'F'
#define	SVC_NOTIFY	'N'

#define	SVC_REQUEST	'r'
#define	SVC_LOSE	'l'

#define	DTTERM_SVC_CLASS		"DTTERM"
#define	DTTERM_SVC_START_MSG		"DTTERM-START"
#define	DTTERM_SVC_TERMINATE_MSG	"DTTERM-TERMINATE"
#define	DTTERM_SVC_TERMINATION_MSG	"DTTERM-TERMINATION"

static XtIntervalId pingId = (XtIntervalId) 0;
int PingInterval = 5;

#ifdef	TIMEOUT
#define	STICKAROUND	15		/* 15 minutes... */
static XtIntervalId waitId = (XtIntervalId) 0;
#endif	/* TIMEOUT */

static Boolean ExitOnLastClose;

static char *serviceName = (char *) 0;
static DtSvcHandle serviceHandle;
static Boolean iAmTheServer = False;
static Boolean waitingForReply = False;
static Boolean waitedForReply = False;
static Widget refWidget;
char *ServerFailureMessage = (char *) 0;
int ServerFailureErrno = 0;
int InstanceCount = 0;

typedef struct _ServiceClientInfoRec {
    pid_t pid;
    Widget shellWidget;
    struct _ServiceClientInfoRec *prev;
    struct _ServiceClientInfoRec *next;
} ServiceClientInfoRec, *ServiceClientInfo;
    
static ServiceClientInfoRec serviceClientInfoHeadRec;
static ServiceClientInfo serviceClientInfoHead = &serviceClientInfoHeadRec;
static Boolean initFlag = False;

static void Initialize(
    Widget		  topLevel,
    int			  argc,
    char		**argv,
    char		 *serverId
);

static void clientMessageProc(
    DtSvcHandle	  service,
    Pointer		  clientData,
    String		 *messageFields,
    int			  numFields
);

static void serverRequestProc(
    DtSvcHandle	  service,
    DtSvcMsgContext	  replyContext,
    Pointer		  clientData,
    String		 *messageFields,
    int			  numFields
);

static void serverMessageProc(
    DtSvcHandle	  service,
    Pointer		  clientData,
    String		 *messageFields,
    int			  numFields
);

static void Ping(
    XtPointer		  clientData,
    XtIntervalId	 *id
);

#ifdef	TIMEOUT
static void TimeOut(
    XtPointer		  clientData,
    XtIntervalId	 *id
);
#endif	/* TIMEOUT */

static void CleanUp(
    int			  sig
);


static const int trapSignalList[] = {
    SIGINT,
    SIGQUIT,
    SIGTERM,
    SIGUSR1,
    SIGUSR2,
};

/*ARGSUSED*/
static void
Initialize(
    Widget		  topLevel,
    int			  argc,
    char		**argv,
    char		 *serverId
)
{
    char		  hostname[BUFSIZ];
    int			  i;
    pid_t		  pid;
    struct sigaction	  sa;

    if (initFlag) {
	/* already initialized... */
	return;
    }

    refWidget = topLevel;

#ifdef	NOTDEF
    /* build a service name.  The service name needs to be application,
     * host, and username specific.  Since we are talking through the
     * display connection, it will already be display specific.
     * The format of the service name will be "DTTERM-hostname-uid"
     * (i.e., "DTTERM-hpcvxds.cv.hp.com-201")...
     */
    char *buffer = (char*) malloc(BUFSIZ);
    if (gethostname(hostname, sizeof(hostname))) {
	(void) strcpy(hostname, "unknown");
    }
    (void) sprintf(buffer, "%s-%s-%ld", DTTERM_SVC_CLASS, hostname, (long)getuid());
    serviceName = XtMalloc(strlen(buffer) + 1);
    (void) strcpy(serviceName, buffer);
    free(buffer);
#endif	/* NOTDEF */

    /* we will use serverId as the service name... */
    serviceName = XtMalloc(strlen(serverId) + 1);
    (void) strcpy(serviceName, serverId);

    /* get a handle... */
    serviceHandle = _DtSvcNewHandle(serviceName, refWidget);

    /* register with the server... */
    if (DT_SVC_SUCCESS == _DtSvcRegister(
	    serviceHandle,
	    False,
	    serverRequestProc,
	    (XtPointer) SVC_REQUEST,
	    serverMessageProc,
	    (XtPointer) SVC_LOSE)) {

	/* We are the new server.  We need to do several things:
	 *
	 *  -fork ourself off.  The server needs to run as a child of
	 *   our application so that it can stay around when our session
	 *   is done.
	 *
	 *  -dissassociate ourself from our parent.
	 *
	 *  -have the child re-exec ourself.  This will allow the child
	 *   to request a session and talk to us as any other normal
	 *   requestor process.
	 */
	/* if the 'n' flag is set, we will not daemonize ourself (i.e.,
	 * fork off and run as a child of the current process)...
	 */
	if (!isDebugSet('n')) {
	    for (i = 0; (i < 10) && ((pid = fork()) < 0); i++) {
		/* if we are out of process slots, then let's sleep
		 * a bit and try again...
		 */
		if (errno != EAGAIN) {
		    break;
		}

		/* give it a chance to clear up... */
		(void) sleep((unsigned long) 2);
	    }
	} else {
	    pid = 0;
	}

	if (pid < 0) {
	    /* can't do much of anything and we haven't done much of
	     * anything.  Let's just error out...
	     */
	    (void) perror("fork()");
	    (void) exit(1);
	} else if (pid > 0) {
	    /* parent.  Let's clean up, restart, and let the new process
	     * try again...
	     */
	    /* close the server connection... */
	    (void) close(ConnectionNumber(XtDisplay(refWidget)));
	    (void) execvp(argv[0], argv);
	    (void) perror(argv[0]);
	    (void) _exit(1);
	}

	/* child server process...
	 */
	/* set the iAmTheServer flag to True.  This flag will remain True
	 * until we lose ownership of the service...
	 */
	iAmTheServer = True;

	/* set up signal handlers so that we can clean up nicely... */
	(void) sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = CleanUp;

	for (i = 0; i < (sizeof(trapSignalList) / sizeof(trapSignalList[0]));
		i++) {
	    (void) sigaction(trapSignalList[i], &sa, (struct sigaction *) 0);
	}

	/* register for service.  This will allow us to deal with the
	 * case where we lose ownership of the service.  We will then
	 * be able to listen to termination requests from our client
	 * applications and shut down the sessions when requested to
	 * do so...
	 */
	(void) _DtSvcNotifyGroupRegister(
		serviceHandle,
		serverMessageProc,
		(XtPointer) SVC_NOTIFY);

	/* get our initial tty modes before we go and create a new
	 * session id...
	 */
	(void) _DtTermPrimPtyGetDefaultModes();

	/* new session group... */
	(void) setsid();
    } else {

	/* we are not the server.  We need to register for messages.  Then
	 * we are done and the service world is now for us...
	 */
	(void) _DtSvcNotifyGroupRegister(
		serviceHandle,
		clientMessageProc,
		(XtPointer) SVC_NOTIFY);
    }

    /* install a ping timeout... */
    if (PingInterval > 0) {
	pingId = XtAppAddTimeOut(XtWidgetToApplicationContext(topLevel),
		1000 * 60 * PingInterval, Ping, (XtPointer) topLevel);
    }

    /* make sure we are never called again... */
    initFlag = True;
}


static void
NiceCleanUp(
    XtPointer		  clientData,
    XtIntervalId	 *id
)
{
    ServiceClientInfo	  serviceClientInfo;
    char		  buffer[BUFSIZ];
    String		  args[20];
    int			  argcnt;

    /* find the serviceClientInfoRec for this widget... */
    for (serviceClientInfo = serviceClientInfoHead->next; serviceClientInfo;
	    serviceClientInfo = serviceClientInfo->next) {

	/* notify each client that the session (is being) terminated... */
	argcnt = 0;
	(void) sprintf(buffer, "%ld", (long)serviceClientInfo->pid);
	args[argcnt] = buffer; argcnt++;
	(void) _DtSvcNotifySend(
		serviceHandle,
		DTTERM_SVC_TERMINATION_MSG,
		args,
		argcnt);
	(void) XSync(XtDisplay(refWidget), 0);
    }

    /* we can now exit... */
    (void) exit(1);
}

static void
CleanUp(
    int			  sig
)
{
    static Boolean	  firstTime = True;

    if (firstTime) {
	/* let's try to do this nicely and invoke our cleanup routine
	 * via the toolkit (i.e., outside of a signal handler)...
	 */
	(void) XtAppAddTimeOut(XtWidgetToApplicationContext(refWidget),
		    0, NiceCleanUp, (XtPointer) refWidget);
	firstTime = False;
    } else {
	/* we have received multiple attempts to kill ourself.  Just
	 * exit...
	 */
	(void) exit(1);
    }
}


/*ARGSUSED*/
static void
serverMessageProc(
    DtSvcHandle	  service,
    Pointer		  clientData,
    String		 *messageFields,
    int			  numFields
)
{
    switch ((int) clientData) {
    case SVC_LOSE:
	/* we lost control of the service... */
	iAmTheServer = False;

	if (InstanceCount <= 0) {
	    /* no reason to stay around... */
	    (void) _DtSvcDestroyHandle(serviceHandle);
	    (void) exit(0);
	}
	break;
    }
}


/*ARGSUSED*/
static void
serverRequestProc(
    DtSvcHandle	  service,
    DtSvcMsgContext	  replyContext,
    Pointer		  clientData,
    String		 *messageFields,
    int			  numFields
)
{
    Widget		  shellWidget;
    ServiceClientInfo	  serviceClientInfo;
    char		  buffer[BUFSIZ];
    String		  reply[20];
    pid_t		  pid = -1;
    Arg			  arglist[20];
    int			  i1;
    int			  i2;
    int			  argcnt = 0;
    char		**commandToExecute = (char **) 0;

    switch ((int) clientData) {
    case SVC_REQUEST:
	if (!strcmp(messageFields[0], DTTERM_SVC_START_MSG)) {
	    /* create our shell widget... */
	    argcnt = 0;
	    (void) XtSetArg(arglist[argcnt], XmNallowShellResize, True);
		    argcnt++;
	    shellWidget = XtAppCreateShell((char *) 0, "Dtterm",
		    applicationShellWidgetClass, XtDisplay((Widget) refWidget),
		    arglist, argcnt);

	    /* parse off messageFields and build the dttermview arglist... */
	    argcnt = 0;

	    for (i2 = 1; i2 < numFields; i2++) {
		if (!strcmp(messageFields[i2], "-pid")) {
		    (void) i2++;
		    if (i2 < numFields) {
			pid = (pid_t) strtol(messageFields[i2], (char **) 0, 0);
		    }

		} else if (!strcmp(messageFields[i2], "-ls")) {
		    (void) XtSetArg(arglist[argcnt], XmNloginShell,
			    True); argcnt++;

		} else if (!strcmp(messageFields[i2], "+ls")) {
		    (void) XtSetArg(arglist[argcnt], XmNloginShell,
			    False); argcnt++;

		} else if (!strcmp(messageFields[i2], "-e")) {
		    (void) i2++;
		    if (i2 < numFields) {
			/* DKS: somewhere we will need to free this... */
			commandToExecute = (char **)
				XtMalloc((numFields - i2 + 1) *
				sizeof(char *));
			for (i1 = 0; i2 < numFields; i1++, i2++) {
			    commandToExecute[i1] = messageFields[i2];
			}
			/* null term commandToExecute... */
			commandToExecute[i1] = (char *) 0;
			(void) XtSetArg(arglist[argcnt], XmNsubprocessArgv,
			    commandToExecute); argcnt++;
		    }
		}
	    }

	    /* create the dtterm... */
	    (void) CreateInstance(shellWidget, "Dtterm",
		    arglist, argcnt);

	    /* create the ServiceClientInfoRec for this instance...
	     */
	    serviceClientInfo =
		    (ServiceClientInfo) XtMalloc(sizeof(ServiceClientInfoRec));
	    serviceClientInfo->pid = pid;
	    serviceClientInfo->shellWidget = shellWidget;
	    /* insert it at the head of the list... */
	    serviceClientInfo->next = serviceClientInfoHead->next;
	    serviceClientInfo->prev = serviceClientInfoHead;
	    if (serviceClientInfoHead->next) {
		serviceClientInfoHead->next->prev = serviceClientInfo;
	    }
	    serviceClientInfoHead->next = serviceClientInfo;

	    (void) XtRealizeWidget(shellWidget);

	    InstanceCount++;
	    /* since we now have active instances, we can remove our
	     * wait timeout...
	     */
#ifdef	TIMEOUT
	    if (waitId) {
		(void) XtRemoveTimeOut(waitId);
		waitId = (XtIntervalId) 0;
	    }
#endif	/* TIMEOUT */
	    
	    /* ack the reply... */
	    i2 = 0;
	    (void) sprintf(buffer, "0x%lx", shellWidget);
	    reply[i2] = buffer; i2++;
	    (void) _DtSvcRequestReply(
		    serviceHandle,
		    replyContext,
		    reply,
		    i2,
		    True);
	}
    }
}


/*ARGSUSED*/
static void
clientMessageProc(
    DtSvcHandle	  service,
    Pointer		  clientData,
    String		 *messageFields,
    int			  numFields
)
{
    int			  i1;
    pid_t		  pid;
    char		  buffer[BUFSIZ];

    switch ((int) clientData) {
	case SVC_NOTIFY:
	    /* process the notify message... */
	    if (!strcmp(messageFields[0], DTTERM_SVC_TERMINATION_MSG)) {
		if (numFields >= 2) {
		    pid = (pid_t) strtol(messageFields[1], (char **) 0, 0);
		} else {
		    pid = -1;
		}

		if (pid == getpid()) {
		    /* our session terminated -- exit... */
		    (void) _DtSvcDestroyHandle(serviceHandle);
		    (void) exit(0);
		}
	    }
	    break;

	case SVC_FAIL:
	    /* turn on the waitingForReply flag... */
	    waitingForReply = False;
	    /* set the waitedForReply flag to True (i.e., failure)... */
	    waitedForReply = True;

	    /* get errno (if returned)... */
	    if (numFields >= 2) {
		errno = (int) strtol(messageFields[1], (char **) 0, 0);
	    }
	    /* build any failure message... */
	    *buffer = '\0';
	    for (i1 = 2; i1 < numFields; i1++) {
		if (*buffer) {
		    (void) strcat(buffer, " ");
		}
		(void) strcat(buffer, messageFields[i1]);
	    }
	    ServerFailureMessage =
		    XtRealloc(ServerFailureMessage,
		    strlen(buffer));
	    (void) strcpy(ServerFailureMessage, buffer);
	    break;

	case SVC_SUCCESS:
	    /* turn on the waitingForReply flag... */
	    waitingForReply = False;
	    /* set the waitedForReply flag to False (i.e., success)... */
	    waitedForReply = False;
	    break;
    }
}


Boolean
ServerStartSession(
    Widget		  topLevel,
    int			  argc,
    char		**argv,
    Boolean		  server,
    char		 *serverId,
    Boolean		  exitOnLastClose,
    Boolean		  block,
    Boolean		  loginShell,
    char		**commandToExec
)
{
    char		  buffer[BUFSIZ];
    String		  args[20];
    int			  argcnt;
    int			  i1;
    XEvent		  event;
    XtAppContext	  appContext;

    ExitOnLastClose = exitOnLastClose;

    (void) Initialize(topLevel, argc, argv, serverId);
    if (iAmTheServer) {
	/* we are the server.  We need to wait for our clients to make
	 * a request of us.
	 */

#ifdef	LOG_USAGE
	/* log our startup... */
	(void) LogStart(0, argc, argv);
#endif	/* LOG_USAGE */

	return(True);
    }

    /* if we go to this point and the -server option was specified, we
     * should go away.  Otherwise we should request service...
     */
    if (server) {
	/* all that was requested was creation of a server.  We can go
	 * away now...
	 */
	(void) exit(0);
    }

    /* make a request of the server to start a session...
     */
    /* need 2 for "-pid" and "<pid>"... */
    argcnt = 2;
    if (commandToExec && *commandToExec) {
	/* need one for "-e"... */
	(void) argcnt++;

	/* need one for each string in the command... */
	/*EMPTY*/
	for (i1 = 0; commandToExec[i1]; i1++) {
	    ;
	}
	argcnt += i1;
    }

    argcnt = 0;
    args[argcnt] = "-pid"; argcnt++;
    (void) sprintf(buffer, "%ld", (long)getpid());
    args[argcnt] = buffer; argcnt++;

    if (loginShell) {
	args[argcnt] = "-ls"; argcnt++;
    } else {
	args[argcnt] = "+ls"; argcnt++;
    }

    if (commandToExec && *commandToExec) {
	args[argcnt] = "-e"; argcnt++;
	for (i1 = 0; commandToExec[i1]; i1++) {
	    args[argcnt] = commandToExec[i1]; argcnt++;
	}
    }

    if (DT_SVC_FAIL == _DtSvcRequestSend(
	    serviceHandle,
	    DTTERM_SVC_START_MSG,
	    args,
	    argcnt,
	    clientMessageProc,
	    (XtPointer) SVC_SUCCESS,
	    clientMessageProc,
	    (XtPointer) SVC_FAIL)) {
	(void) fprintf(stderr, "request to server failed\n");
	return(True);
    }

    /* dispatch locally until we get back the results from our server... */
    appContext = XtWidgetToApplicationContext(topLevel);
    for (waitingForReply = True; waitingForReply; ) {
	(void) XtAppNextEvent(appContext, &event);
	(void) XtDispatchEvent(&event);
    }

    if (!block) {
	/* we succeeded, we can exit now... */
	(void) _DtSvcDestroyHandle(serviceHandle);
	(void) exit(0);
    }

    return(waitedForReply);
}

void
ServerInstanceTerminated(
    Widget		  w
)
{
    ServiceClientInfo	  serviceClientInfo;
    char		  buffer[BUFSIZ];
    String		  args[20];
    int			  argcnt;

    /* find the serviceClientInfoRec for this widget... */
    for (serviceClientInfo = serviceClientInfoHead->next; serviceClientInfo;
	    serviceClientInfo = serviceClientInfo->next) {
	if (serviceClientInfo->shellWidget == w) {
	    break;
	}
    }

    if (serviceClientInfo && (serviceClientInfo->shellWidget == w)) {
	/* notify the client that the session terminated... */
	argcnt = 0;
	(void) sprintf(buffer, "%ld", (long)serviceClientInfo->pid);
	args[argcnt] = buffer; argcnt++;
	(void) _DtSvcNotifySend(
		serviceHandle,
		DTTERM_SVC_TERMINATION_MSG,
		args,
		argcnt);

	/* free up the serviceClientInfoRec... */
	serviceClientInfo->prev->next = serviceClientInfo->next;
	if (serviceClientInfo->next) {
	    serviceClientInfo->next->prev = serviceClientInfo->prev;
	}
	(void) XtFree((char *) serviceClientInfo);
    }

    if ((--InstanceCount <= 0) && (!iAmTheServer)) {
	(void) _DtSvcDestroyHandle(serviceHandle);
	(void) exit(0);
    } else
#ifdef	TIMEOUT
    if ((InstanceCount <= 0) && !waitId) {
	/* set up a wait timeout and stick around for a while before
	 * we exit...
	 */
	waitId = XtAppAddTimeOut(XtWidgetToApplicationContext(refWidget),
		1000 * 60 * STICKAROUND, TimeOut, (XtPointer) refWidget);
    }
#endif	/* TIMEOUT */

    if ((InstanceCount <= 0) && ExitOnLastClose) {
	(void) exit(0);
    }
}

#ifdef	TIMEOUT
static void
TimeOut(
    XtPointer		  clientData,
    XtIntervalId	 *id
)
{
    /* if we have no instances active, go away... */
    if (InstanceCount <= 0) {
	(void) exit(0);
    }

    /* otherwise, clear the waitId... */
    if (*id == waitId) {
	waitId = (XtIntervalId) 0;
    }
}
#endif	/* TIMEOUT */

static void
Ping(
    XtPointer		  clientData,
    XtIntervalId	 *id
)
{
    Widget		  w = (Widget) clientData;
    Window		  root;
    Window		  child;
    int			  rootX;
    int			  rootY;
    int			  winX;
    int			  winY;
    unsigned int	  mask;

    if (*id != pingId) {
	return;
    }

    /* cause a round trip to the server... */
    (void) XQueryPointer(XtDisplay(w), XtWindow(w), &root, &child,
	    &rootX, &rootY, &winX, &winY, &mask);

    /* reset the timeout... */
    if (PingInterval > 0) {
	pingId = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
		1000 * 60 * PingInterval, Ping, clientData);
    }
}
#else
/* dummy variable to get pass compilation phase */
static char *foo;
#endif	/* TERMINAL_SERVER */
