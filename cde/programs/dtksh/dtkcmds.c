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
/* $TOG: dtkcmds.c /main/13 1997/07/14 18:16:01 samborn $ */
/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */

/* X includes */

#include	"name.h" 
#include	"shell.h" 
#include <signal.h>
#include <fcntl.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/StringDefs.h>
#include <Xm/XmStrDefs.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Dt/Service.h>
#include <Dt/Wsm.h>
#include <Dt/HourGlass.h>
#include <Dt/Help.h>
#include <Dt/Action.h>
#include <Dt/Dts.h>
#include <Dt/Print.h>
#include <Tt/tttk.h>
#include <Tt/tt_c.h>
#include "hash.h"
#include "stdio.h"
#define NO_AST
#include "dtksh.h"
#undef NO_AST
#include "xmksh.h"
#include "dtkcmds.h"
#include "xmcvt.h"
#include "widget.h"
#include "extra.h"
#include "xmwidgets.h"
#include "msgs.h"
#include <locale.h>

extern Namval_t *sh_assignok(Namval_t *np,int add);


static void PendingDestroy( 
                        Widget w,
                        wtab_t *wtab,
                        caddr_t callData) ;
static Boolean WtabDestroy( 
                        caddr_t callData) ;
static int _CreateWidget( 
                        Widget (*func)(),
                        int argc,
                        char *argv[]) ;
static int _DTKSH_XtDestroyWidget( 
                        Widget w) ;
static int do_single_widget_arg_func( 
                        int (*func)(),
                        int argc,
                        char **argv) ;
static void mainloopsighandler( 
                        int sig) ;
static int XtCallCallbacks_usage( 
                        char *arg0) ;
static void RegisterCmdStr( 
                        char type,
                        long id,
                        char *cmd) ;
static void RemoveCmdStr( 
                        char type,
                        long id) ;
static int do_RootWindowCmd( 
                        int (*func)(),
                        int argc,
                        char *argv[]) ;
static int cvtfontstruct( 
                        char *name,
                        XFontStruct **fn) ;
static int CatchNonFatalFontError( 
                        Display *display,
                        XErrorEvent *event) ;
static int cvtfont( 
                        Display *display,
                        char *name,
                        Font *fn) ;
static int cvtcolor( 
                        char *name,
                        Pixel *pix) ;
static int invokeXDrawFunction( 
                        int function,
                        int argc,
                        char *argv[]) ;
static int XtAddInputUsage( 
                        char *arg0) ;
static void DestroyInputRecord(
                        XtInputId id) ;
static int FindInputRecord(
                        XtInputId id) ;
static Boolean ProcessInput(
                        inputrec_t * inp,
                        int source,
                        XtInputId id,
                        Boolean eofFound) ;
static int VerifyString_usage( 
                        char *arg0) ;
static int XtSetSensitive_usage( 
                        char *arg0) ;
static int GetDisplayHandle(
                        int argc,
                        char **argv,
	                Widget (*func)());
static int RegisterTranslations( 
                        void (*func)(),
                        int argc,
                        char *argv[]) ;
static int LocateEHRecord( 
                        wtab_t *w,
                        char *ksh_cmd) ;
static int GetWorkspaceList( 
                        char *usageMsg,
                        Boolean getOccupied,
                        int argc,
                        char *argv[]) ;
static int DtTurnOnOrOffHourGlass( 
                        void (*func)(),
                        int argc,
                        char *argv[]) ;
static int WsmCommonProc(
                        int argc,
                        char *argv[],
	                void (*func)());
static int ttdt_SaveOrRevert(
                        Tt_status (*func)(),
                        int argc,
                        char *argv[] ) ;
static int message_DestroyOrReply( 
                        Tt_status (*func)(),
                        int argc,
                        char *argv[] );
static int message_FailOrReject( 
                        Tt_status (*func)(),
                        int argc,
                        char *argv[] );
static Tt_message TtFileCB(
                        Tt_message msg,
	                Tttk_op op,
	                char * pathName,
	                void * clientData,
	                int sameEuidEgid,
	                int sameProcId ) ;
static int tt_netfile_handler( 
                        int paramCount,
                        char * (*func)(),
                        char * usageMsg,
                        int argc,
                        char *argv[] ) ;
static Namval_t * CreateEmptyNameValuePair( 
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessIntValue( 
                        int value,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp,
                        char *format,
                        Namfun_t *fp_new) ;
static Namval_t * ProcessStringValue( 
                        char *value,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessBooleanIntValue( 
                        int value,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp,
                        Namfun_t *fp_new) ;
static Namval_t * ProcessTraversalDirection( 
                        XmTraversalDirection dir,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessSelectionType( 
                        char selType,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessIntTable( 
                        int *table,
                        int count,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessXmStringTable( 
                        XmString *table,
                        int count,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessWidgetHandle( 
                        Widget handle,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessXmStringValue( 
                        XmString xmstring,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessHyperType( 
                        int hyperType,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static void InitEventTables( void ) ;
static Namval_t * ProcessCallbackEvent( 
                        XEvent *event,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * _IntProcessCallbackReason( 
                        struct named_integer *table,
                        XmAnyCallbackStruct *cbData,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessCallbackReason( 
                        XmAnyCallbackStruct *cbData,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static Namval_t * ProcessHelpCallbackReason( 
                        XmAnyCallbackStruct *cbData,
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp) ;
static void _DtActionInvokeUsage( void ) ;
static void DtkReloadHandler( 
                        XtPointer clientData ) ;
static Namfun_t * CloneDiscipline(
                        Namdisc_t * discipline );
static void FreeDiscipline(
                        Namfun_t * discipline );
static Namdisc_t * CheckClassDisciplines(
                        WidgetClass class,
                        char *cbname) ;
void SetTextDoit(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);
void SetTextStartPos(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);
void SetTextEndPos(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);
void SetTextPtr(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);
void SetTextLen(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);
void SetTextFormat(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);
void SetTextWCSptr(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);
void SetTextWCSlen(
                        Namval_t *np,
                        char *name,
                        Namfun_t *fp);



#define CLEAR_AREA        0
#define CLEAR_WINDOW      1
#define DRAW_ARC          2
#define DRAW_IMAGE_STRING 3
#define DRAW_LINE         4
#define DRAW_LINES        5
#define DRAW_POINT        6
#define DRAW_POINTS       7
#define DRAW_RECTANGLE    8
#define DRAW_SEGMENTS     9
#define DRAW_STRING       10
#define FILL_ARC          11
#define FILL_POLYGON      12
#define FILL_RECTANGLE    13
#define COPY_AREA         14

#define WORKPROC_CMDS     0
#define TIMEOUT_CMDS      1


#define MAXARGS 4096
#define SLISTITEMSIZE	16

Widget Toplevel;
Boolean invalidFont;

/* List of all name/value pairs created during handling of a callback */
Namval_t *** npTable = NULL;
int npTableSize = 0;
int * npListSizes = NULL;
int nestingLevel = -1;

static Namdisc_t transDiscipline ={0, NULL, NULL, NULL, NULL, 
                                  (Namval_t *(*)())transCreateDisc, NULL, NULL};
static Namdisc_t ehDiscipline = {0, NULL, NULL, NULL, NULL, 
                                  (Namval_t *(*)())ehCreateDisc, NULL, NULL};
static Namdisc_t dftDiscipline = {0, NULL, NULL, NULL, NULL, 
                                  (Namval_t *(*)())dftCreateDisc, NULL, NULL};
static Namdisc_t nopDiscipline = {0, NULL, NULL, NULL, NULL, 
                                  (Namval_t *(*)())nopCreateDisc, NULL, NULL};

static Namdisc_t text_doit_disc = {0, (void (*)())SetTextDoit, NULL, NULL, NULL,
                                  NULL, NULL, NULL};
static Namdisc_t text_startpos_disc = {0, (void (*)())SetTextStartPos, NULL, 
                                  NULL, NULL,NULL, NULL, NULL};
static Namdisc_t text_endpos_disc = {0, (void (*)())SetTextEndPos, NULL, NULL, 
                                  NULL, NULL, NULL, NULL};
static Namdisc_t text_ptr_disc = {0, (void (*)())SetTextPtr, NULL, NULL, NULL,
                                  NULL, NULL, NULL};
static Namdisc_t text_len_disc = {0, (void (*)())SetTextLen, NULL, NULL, NULL,
                                  NULL, NULL, NULL};
static Namdisc_t text_format_disc = {0, (void (*)())SetTextFormat, NULL, NULL, 
                                  NULL, NULL, NULL, NULL};
static Namdisc_t text_wcsptr_disc = {0, (void (*)())SetTextWCSptr, NULL, NULL, 
                                  NULL, NULL, NULL, NULL};
static Namdisc_t text_wcslen_disc = {0, (void (*)())SetTextWCSlen, NULL, NULL, 
                                  NULL, NULL, NULL, NULL};

static const XtActionsRec Ksh_actions[] = {
	{ "ksh_eval",	Translation_ksh_eval }
};

static dtksh_client_data_t ** cbDataTable = NULL;
static int cbDataTableSize = 0;
static dtksh_event_handler_data_t ** ehDataTable = NULL;
static int ehDataTableSize = 0;

static char * str_XtRString = XtRString;

char str_s_eq_s[] = "%s=%s";
char str_s_eq[] = "%s=";
char str_nill[] = "";

typedef struct
{
   int state;
   Window icon;
} WmStateData;

typedef struct
{
   long id;
   char * cmd;
} CommandString;

CommandString * workProcCmds = NULL;
int workProcCmdsSize = 0;
CommandString * timeOutCmds = NULL;
int timeOutCmdsSize = 0;


typedef struct {
   char * ksh_cmd;
   Tt_pattern * patterns;
} Ttdt_file_cb_data;

Ttdt_file_cb_data ** fileCBList = 0;
int sizeFileCBList = 0;

typedef struct {
   char * fieldName;
   char * representation;
   Cardinal valueOffset;
   Cardinal valueSize;
} EventEntryTable;

typedef struct {
   char * eventType;
   EventEntryTable * table;
} XEventTable;


/* Keeps track of file input generated by XtAddInput() */
typedef struct {
   Boolean inUse;
   XtInputId id;
   inputrec_t * inp;
} InputRecord;

InputRecord * activeInputs = NULL;
int numActiveInputs = 0;



/*
 * When we are notified that a widget is being destroyed (through the
 * widget's destroy callback, it is not yet safe for us to remove all
 * internal knowledge of that widget, because the shell script may have
 * also added a destroy callback, which gets call AFTER ours.  Therefore,
 * all we can do is mark the widget as 'pending destroy', and add a 
 * workproc; the workproc will then take care of removing the widget.
 */

static Boolean workProcAdded = False;

static void
PendingDestroy(
        Widget w,
        wtab_t *wtab,
        caddr_t callData )
{
   wtab->mask |= DT_PENDING_DESTROY;
   if (!workProcAdded)
   {
      workProcAdded = True;
      XtAddWorkProc((XtWorkProc)WtabDestroy, NULL);
   }
}


static Boolean
WtabDestroy(
        caddr_t callData )
{
   int i;

   for (i = 0; i < NumW; i++)
   {
      if ((W[i]->type == TAB_WIDGET) && (W[i]->mask & DT_PENDING_DESTROY))
      {
         XtFree(W[i]->wname);
         XtFree(W[i]->widid);
	 if (W[i]->envar) {
                char *val = env_get(W[i]->envar);

                /*
                 * Blank out the environment variable holding the
                 * widget handle, but only if it still holds the
                 * handle!  This guards against the possibility that
                 * the user has re-used the same variable for another
                 * widget later.
                 */
                if (val && W[i]->widid && strcmp(val, W[i]->widid) == 0) {
                        env_blank(W[i]->envar);
                }
                XtFree(W[i]->envar);
         }
         W[i]->type = TAB_EMPTY;
         Wtab_free++;
      }
   }

   workProcAdded = False;
   return(True);
}

wtab_t *
set_up_w(
        Widget wid,
        wtab_t *parent,
        char *var,
        char *name,
        classtab_t *class )
{
	char widid[8];
	static wtab_t *w;

	get_new_wtab(&w, widid);
	if (var) {
		env_set_var(var, widid);
		w->envar = strdup(var);
	} else {
		w->envar = strdup("none");
	}
	w->type = TAB_WIDGET;
	w->wname = name ? strdup(name) : strdup(XtName(wid));
	w->wclass = class;
	w->parent = parent;
	w->widid = strdup(widid);
	w->w = wid;
	w->mask = 0;
	XtAddCallback(wid, XtNdestroyCallback, (XtCallbackProc)PendingDestroy, 
                      (caddr_t)w);
	return(w);
}

static short Needfree[MAXARGS];

void
parse_args(
        char *arg0,
        int argc,
        char **argv,
        wtab_t *w,
        wtab_t *parent,
        classtab_t *class,
        int *n,
        Arg *args,
	int * pargc,
	char ** pargv ,
	Boolean postponePixmaps )
{
	int i;
	char *colon, *resource, *val, *p;
	XtArgVal argval;
	int freeflag, len;
        char * errmsg;
	int conversionResult;

	if (pargc)
		(*pargc) = 0;
	*n = 0;
	for (i = 0; i < argc; i++) {
		if (i >= MAXARGS) {
			errmsg = strdup(GETMESSAGE(5,1, 
                                      "Too many resource parameters have been specified; skipping '%s'"));
			printerrf(arg0, errmsg,
                                 argv[*n], NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL);
                        free(errmsg);
			continue;
		}
		if ((colon = strchr(argv[i], ':')) == NULL)
                {
                        errmsg = strdup(GETMESSAGE(5,2, 
                                "Bad resource specification; should be of the form 'name:value' : %s"));
			printerrf(arg0, errmsg,
                             argv[i], NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			continue;
		}
		val = &colon[1];
		len = colon - argv[i];
		resource = XtMalloc(len + 1);
		strncpy(resource, argv[i], len);
		resource[len] = '\0';

		/*
		 * The following special check fixes a bug in Xt, where the
		 * string defined for the XmNwaitForWm resource does not
		 * follow the naming conventions, and is set to "waitforwm".
		 * In dtksh, users expect the naming conventions to be
		 * followed, and this breaks for this one resource.
		 */
		if (strcmp(resource, "waitForWm") == 0)
			strcpy(resource, XmNwaitForWm);

		if ((conversionResult = ConvertStringToType(arg0, w, parent, 
			class, resource, val, &argval, &freeflag, 
                        postponePixmaps)) == CONVERT_SUCCEEDED) 
		{
			XtSetArg(args[*n], resource, argval);

                        /* 
                         * The following is a memory leak, but it allows us to
                         * comply with what Xt has spec'ed as the required
			 * behavior of the geometry string (It is a bogus
			 * requirement!).  The Xt Shell widget does not make 
			 * a copy of the incoming 'geometry' string, but 
			 * instead, simply keeps a pointer to the string 
			 * passed-in by the application.  For dtksh, this is 
			 * a problem, because we would typically free up the 
			 * string right away.  This hack causes us to not free 
			 * up the string.
                         */
                        if (strcmp(resource, XmNgeometry) == 0)
			   Needfree[*n] = False;
                        else
			   Needfree[*n] = freeflag;

			(*n)++;
		}
		else if (conversionResult == CONVERT_POSTPONED)
		{
			/*
			 * Postpone processing this resource until after the
			 * the widget has been created, or, in the case of a
                         * pixmap resource, until any new colors have been set.
			 */
			if (pargc)
			{
				pargv[*pargc] = argv[i];
				(*pargc)++;
			}
			XtFree(resource);
		}
		else
		{
			XtFree(resource);
		}
	}
}

void
free_args(
        int n,
        Arg *args )
{
	int i;

	/*
	 * Free up argument pointers
	 */
	for (i = 0; i < n; i++) {
		XtFree(args[i].name);
		if (Needfree[i]) {
			XtFree((String)args[i].value);
		}
	}
}


int
do_XtInitialize(
        int argc,
        char *argv[] )
{
	int ret;
	char *dtkdb_hook;
        char * errmsg;
        int * lockedFds;

	if (Toplevel != NULL)
        {
		errmsg = strdup(GETMESSAGE(5,4, 
                            "The toolkit has already been initialized"));
		printerr(argv[0], errmsg, NULL);
                free(errmsg);
		return(1);
	}

	if (argc < 4) {
           errmsg =strdup(GETMESSAGE(5,5, 
                      "Usage: XtInitialize variable applicationName applicationClass [args ...]"));
	   printerr(str_nill, errmsg, NULL);
           free(errmsg);
	   return(1);
	}
        lockedFds = LockKshFileDescriptors();
	ret = toolkit_initialize(argc, argv);
        UnlockKshFileDescriptors(lockedFds);
	XtAddActions((XtActionList)Ksh_actions, XtNumber(Ksh_actions));
	if ((dtkdb_hook = env_get("DTKDB_HOOK")) != NULL) {
		ksh_eval(dtkdb_hook);
	}
	return(ret);
}

static int
_CreateWidget(
        Widget (*func)(),
        int argc,
        char *argv[] )
{
	Widget widget;
	classtab_t *class;
	char *arg0 = argv[0];
	wtab_t *w, *pw, *wtab;
	char *wname, *wclass, *parentid, *var;
	Arg	args[MAXARGS];
	int	i;
	int n;
        char * errmsg;
	int pargc;
	char ** pargv;

	if (argc < 5) {
                errmsg = strdup(GETMESSAGE(5,6, 
                        "Usage: %s variable name class parent [arg:val ...]"));
		printerrf(str_nill, errmsg,
                  argv[0], NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                free(errmsg);
		return(1);
	}
	var = argv[1];
	wname = argv[2];
	wclass = argv[3];
	parentid = argv[4];
	pw = str_to_wtab(argv[0], parentid);
	if (pw == NULL) {
		errmsg = strdup(GetSharedMsg(DT_NO_PARENT));
		printerr(argv[0], errmsg, NULL);
                free(errmsg);
		return(1);
	}
	argv += 5;
	argc -= 5;
	if ((class = str_to_class(arg0, wclass)) == NULL) {
		return(1);
	}
	pargc = 0;
	if (argc > 0) { 
		pargv = (char **)XtMalloc(sizeof(char *) * argc);
	}
	else {
		pargv = NULL;
	}
	n = 0;
	parse_args(arg0, argc, argv, NULL, pw, class, &n, args, &pargc, pargv,
                   True);
	widget = func(wname, class->class, pw->w, args, n);
	if (widget != NULL) {
		wtab = set_up_w(widget, pw, var, wname, class);

		/* Process any postponed resources */
		if (pargc > 0)
		{
			free_args(n, args);
			n = 0;
			parse_args(arg0, pargc, pargv, wtab, pw, class, &n, 
			           args, NULL, NULL, False);
			XtSetValues(widget, args, n);
		}
	} else {
                errmsg = strdup(GetSharedMsg(DT_WIDGET_CREATE_FAILED));
		printerrf(argv[0], errmsg, wname, 
                         NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                free(errmsg);
		env_blank(argv[1]);
	}
	free_args(n, args);
	XtFree((char *)pargv);

	return(0);
}

int
do_XtCreateApplicationShell(
        int argc,
        char *argv[] )
{
	Widget widget;
	classtab_t *class;
	char *arg0 = argv[0];
	wtab_t *w, *wtab;
	char *wname, *wclass, *var;
	Arg	args[MAXARGS];
	int	i;
	int n;
        char * errmsg;
	int pargc;
	char ** pargv;

	if (argc < 4) {
           errmsg=strdup(GETMESSAGE(5,7, 
                    "Usage: XtCreateApplicationShell variable name class [arg:val ...]"));
 	   printerr(str_nill, errmsg, NULL);
           free(errmsg);
	   return(1);
	}
	var = argv[1];
	wname = argv[2];
	wclass = argv[3];
	argv += 4;
	argc -= 4;
	if ((class = str_to_class(arg0, wclass)) == NULL) {
		return(1);
	}
	pargc = 0;
	if (argc > 0) {
		pargv = (char **)XtMalloc(sizeof(char *) * argc);
	}
	else {
		pargv = NULL;
	}
	n = 0;
	parse_args(arg0, argc, argv, NULL, NULL, class, &n, args, &pargc,
		   pargv, True);
	widget = XtCreateApplicationShell(wname, class->class, args, n);
	if (widget != NULL) {
		wtab = set_up_w(widget, NULL, var, wname, class);
	
		/* Process any postponed resources */
		if (pargc > 0)
		{
			free_args(n, args);
			n = 0;
			parse_args(arg0, pargc, pargv, wtab, NULL, class, 
				   &n, args, NULL, NULL, False);
			XtSetValues(widget, args, n);
		}
	} else {
                errmsg = strdup(GetSharedMsg(DT_WIDGET_CREATE_FAILED));
		printerrf(argv[0], errmsg, wname,
                         NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                free(errmsg);
		env_blank(argv[1]);
	}
	free_args(n, args);
	XtFree((char *)pargv);

	return(0);
}

int
do_XtCreatePopupShell(
        int argc,
        char *argv[] )
{
	return(_CreateWidget(XtCreatePopupShell, argc, argv));
}

int
do_XtCreateManagedWidget(
        int argc,
        char *argv[] )
{
	return(_CreateWidget(XtCreateManagedWidget, argc, argv));
}

int
do_XtCreateWidget(
        int argc,
        char *argv[] )
{
	return(_CreateWidget(XtCreateWidget, argc, argv));
}

int
do_XtPopup(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   XtGrabKind grab;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg=strdup(GETMESSAGE(5,8, 
              "Usage: XtPopup widget GrabNone|GrabNonexclusive|GrabExclusive"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }
   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);
   else 
   {
      grab = XtGrabNone;
      if (argc < 3 || strcmp(argv[2], "GrabNone") == 0) 
         grab = XtGrabNone;
      else if (strcmp(argv[2], "GrabNonexclusive") == 0) 
         grab = XtGrabNonexclusive;
      else if (strcmp(argv[2], "GrabExclusive") == 0) 
         grab = XtGrabExclusive;
      else 
      {
         errmsg=strdup(GETMESSAGE(5,9, 
                   "The grab type '%s' is not recognized; using 'GrabNone'"));
         printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                   NULL, NULL);
         free(errmsg);
      }

      XtPopup(w->w, grab);
   }
   return(0);
}

static int
_DTKSH_XtDestroyWidget(
        Widget w )
{
   XtDestroyWidget(w);
   return(1);
}

static int
do_single_widget_arg_func(
        int (*func)(),
        int argc,
        char **argv )
{
	wtab_t *w;
	int i;
        char * errmsg;

	if (argc < 2) {
                errmsg = strdup(GetSharedMsg(DT_USAGE_WIDGET));
		printerrf(str_nill, errmsg, argv[0], NULL, NULL,
                         NULL, NULL, NULL, NULL, NULL);
                free(errmsg);
		return(1);
	}
	for (i = 1; i < argc; i++) {
		w = str_to_wtab(argv[0], argv[i]);
		if (w != NULL) {
			func(w->w);
		}
	}
	return(0);
}

int
do_XtDestroyWidget(
        int argc,
        char *argv[] )
{
   return(do_single_widget_arg_func(_DTKSH_XtDestroyWidget, argc, argv));
}

int
do_single_widget_test_func(
        int (*func)(),
        int argc,
        char **argv )
{
	wtab_t *w;
	int i;
        char * errmsg;

	if (argc != 2) {
		errmsg=strdup(GetSharedMsg(DT_USAGE_WIDGET));
		printerrf(str_nill, errmsg, argv[0], NULL, NULL,
                          NULL, NULL, NULL, NULL, NULL);
                free(errmsg);
		return(1);
	}
	w = str_to_wtab(argv[0], argv[1]);
	if (w != NULL) {
		return(!func(w->w));
	}
	return(255);
}

int
do_XtIsSensitive(
        int argc,
        char *argv[] )
{
   return(do_single_widget_test_func((int(*)())XtIsSensitive, argc, argv));
}


/*
 * XtIsShell() is a macro, so we can't use do_single_widget_test_func().
 */
int
do_XtIsShell(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   int i;
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,10, "Usage: XtIsShell widget"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w != NULL) 
      return(!XtIsShell(w->w));
   return(255);
}

int
do_XtIsManaged(
        int argc,
        char *argv[] )
{
   return(do_single_widget_test_func((int(*)())XtIsManaged, argc, argv));
}

int
do_XtIsRealized(
        int argc,
        char *argv[] )
{
   return(do_single_widget_test_func((int(*)())XtIsRealized, argc, argv));
}

int
do_XtRealizeWidget(
        int argc,
        char *argv[] )
{
   return(do_single_widget_arg_func((int(*)())XtRealizeWidget, argc, argv));
}

int
do_XtUnrealizeWidget(
        int argc,
        char *argv[] )
{
   return(do_single_widget_arg_func((int(*)())XtUnrealizeWidget, argc, argv));
}

/*
 * XtMapWidget() is a macro, so can't use do_single_widget_arg_func()
 */

int
do_XtMapWidget(
        int argc,
        char *argv[] )
{
	wtab_t *w;
	int i;
        char * errmsg;

	if (argc < 2) {
		errmsg = strdup(GETMESSAGE(5,11, "Usage: XtMapWidget widget"));
		printerr(str_nill, errmsg, NULL);
                free(errmsg);
		return(1);
	}
	for (i = 1; i < argc; i++) {
		w = str_to_wtab(argv[0], argv[i]);
		if (w != NULL) {
			XtMapWidget(w->w);
		}
	}
	return(0);
}

int
do_XtUnmapWidget(
        int argc,
        char **argv )
{
	wtab_t *w;
	int i;
        char * errmsg;

	if (argc < 2) {
	   errmsg = strdup(GETMESSAGE(5,12, "Usage: XtUnmapWidget widget"));
	   printerr(str_nill, errmsg, NULL);
           free(errmsg);
	   return(1);
	}
	for (i = 1; i < argc; i++) {
		w = str_to_wtab(argv[0], argv[i]);
		if (w != NULL) {
			XtUnmapWidget(w->w);
		}
	}
	return(0);
}

int
do_XtPopdown(
        int argc,
        char **argv )
{
   return(do_single_widget_arg_func((int(*)())XtPopdown, argc, argv));
}

int
do_XtMainLoop(
        int argc,
        char **argv )
{
        /* 
         * Required to guarantee that all of the shell script's "echo"
         * requests have been taken care of, before we drop into the
         * the black hole called XtMainLoop.
         */
        fflush(stdout);

	XtMainLoop();
	return(1);
}


int
do_XtDisplay(
        int argc,
        char **argv )
{
   return(GetDisplayHandle(argc, argv, (Widget (*)())XtDisplay));
}


int
do_XtDisplayOfObject(
        int argc,
        char **argv )
{
   return(GetDisplayHandle(argc, argv, (Widget (*)())XtDisplayOfObject));
}


static int
GetDisplayHandle(
        int argc,
        char **argv,
	Widget (*func)())
{
   wtab_t *w;
   char *arg0 = argv[0];
   char * variable = argv[1];
   char buf[128];
   char * errmsg;

   if (argc != 3)
   {
      errmsg = strdup(GETMESSAGE(5,13, "Usage: %s variable widget"));
      printerrf(str_nill, errmsg, arg0, NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(arg0, argv[2]);
   if (w == NULL) 
      return(1);

   sprintf(buf, "0x%lx", (long)(*func)(w->w));
   alt_env_set_var(variable, buf);
   return(0);
}


int
do_XtNameToWidget(
        int argc,
        char *argv[] )
{
   char *arg0 = argv[0];
   wtab_t * w;
   char * variable = argv[1]; 
   Widget child;
   classtab_t *ctab;
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,82, 
            "Usage: XtNameToWidget variable referenceWidget names"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(arg0, argv[2]);
   if (w == NULL) {
        alt_env_set_var(variable, str_nill); 
	return(1);
   }

   child = XtNameToWidget(w->w, argv[3]);
 
   if (child == NULL)
   {
      alt_env_set_var(variable, str_nill); 
      return(1);
   }

   w = widget_to_wtab(child);
   if (w == NULL) {
        alt_env_set_var(variable, str_nill); 
	return(1);
   }
   /*
    * If the widget class has no resources registered, then this is
    * the first known instance of this widget class, so we need to
    * force the resource list to be loaded.  This can frequently
    * occur if a Motif convenience function is used, which creates
    * a 'hidden' parent.
    */
   ctab = w->wclass;
   if (ctab->res == NULL)
      (void)str_to_class(arg0, ctab->cname);

   alt_env_set_var(variable,  w->widid); 
   return(0);
}


int
do_XtScreen(
        int argc,
        char **argv )
{
   wtab_t *w;
   char *arg0 = argv[0];
   char * variable = argv[1];
   char buf[128];
   char * errmsg;

   if (argc != 3)
   {
      errmsg = strdup(GETMESSAGE(5,14, "Usage: XtScreen variable widget"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(arg0, argv[2]);
   if (w == NULL) 
      return(1);

   sprintf(buf, "0x%lx", (long)XtScreen(w->w));
   alt_env_set_var(variable, buf);
   return(0);
}


int
do_XtWindow(
        int argc,
        char **argv )
{
   wtab_t *w;
   char *arg0 = argv[0];
   char * variable = argv[1];
   char buf[128];
   char * errmsg;

   if (argc != 3)
   {
      errmsg = strdup(GETMESSAGE(5,15, "Usage: XtWindow variable widget"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(arg0, argv[2]);
   if (w == NULL) 
      return(1);

   sprintf(buf, "0x%lx", (long)XtWindow(w->w));
   alt_env_set_var(variable, buf);
   return(0);
}


static int
XtCallCallbacks_usage(
        char *arg0 )
{
   char * errmsg;

   errmsg = strdup(GETMESSAGE(5,16, 
                   "Usage: XtCallCallbacks widget callbackName"));
   printerr(str_nill, errmsg, NULL);
   free(errmsg);
   return(1);
}


int
do_XtCallCallbacks(
        int argc,
        char **argv )
{
   wtab_t *w;
   char *arg0 = argv[0];

   if (argc != 3)
      return(XtCallCallbacks_usage(arg0));

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);
   else
      XtCallCallbacks(w->w, argv[2], NULL);
   return(0);
}


int
do_XtHasCallbacks(
        int argc,
        char **argv )
{
   wtab_t *w;
   char *arg0 = argv[0];
   char * msg;
   char * variable = argv[1];
   XtCallbackStatus callbackStatus;
   XrmValue fval, tval;
   char * errmsg;

   if (argc != 4)
   {
      errmsg = strdup(GETMESSAGE(5,17, 
                   "Usage: XtHasCallbacks variable widget callbackName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[2]);
   if (w == NULL) 
      return(1);
   else
      callbackStatus = XtHasCallbacks(w->w, argv[3]);

   switch (callbackStatus)
   {
      case XtCallbackNoList:
      {
         msg = "CallbackNoList";
         break;
      }
      case XtCallbackHasNone:
      {
         msg = "CallbackHasNone";
         break;
      }
      case XtCallbackHasSome:
      {
         msg = "CallbackHasSome";
         break;
      }
   }
   alt_env_set_var(variable, msg);
   return(0);
}

int
do_XtAddCallback(
        int argc,
        char **argv )
{
   char * errmsg;

   if (argc != 4)
   {
      errmsg = strdup(GETMESSAGE(5,18, 
                  "Usage: XtAddCallback widget callbackName ksh-command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   return(AddOneCallback(argv[0], argv[1], argv[2], argv[3], NULL));
}


/*
 * This function is used to add both regular Xt Callbacks, and
 * Motif WMProtocol callback.
 */
int
AddOneCallback(
        char *cmd,
        char *widget,
        char *cbName,
        char *kshcmd,
        char *propAtomStr )
{
   wtab_t *w;
   dtksh_client_data_t *cdata;
   char * p;
   Atom propAtom;
   char * errmsg;

   w = str_to_wtab(cmd, widget);
   if (w == NULL) 
      return(1);

   if (propAtomStr)
   {
      propAtom = (Atom)strtoul(propAtomStr, &p, 0);
      if (p == propAtomStr)
      {
         errmsg = strdup(GetSharedMsg(DT_BAD_ATOM));
         printerrf(cmd, errmsg, propAtomStr, NULL, NULL, NULL, NULL, NULL,
                   NULL, NULL);
         free(errmsg);
         return(1);
      }
   }
   else
      propAtom = None;

   cdata = GetNewCBData(kshcmd, w, cbName, propAtom);

   if (strcmp(cmd, "XtAddCallback") == 0)
      XtAddCallback(w->w, cbName, (XtCallbackProc)stdCB, (XtPointer)cdata);
   else
   {
      XmAddWMProtocolCallback(w->w, propAtom, (XtCallbackProc)stdCB, 
                              (XtPointer)cdata);
   }

   return(0);
}

int
do_XtRemoveCallback(
        int argc,
        char **argv )
{
   char * errmsg;

   if (argc != 4)
   {
      errmsg = strdup(GETMESSAGE(5,19, 
                  "Usage: XtRemoveCallback widget callbackName ksh-command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   return(RemoveOneCallback (argv[0], argv[1], argv[2], argv[3], NULL, NULL));
}

/*
 * This function is used to delete both regular Xt Callbacks, and
 * Motif WMProtocol callback.
 */
int
RemoveOneCallback(
        char *cmd,
        char *widget,
        char *cbName,
        char *kshcmd,
        char *propAtomStr,
        char *handleStr )
{
   wtab_t *w;
   dtksh_client_data_t *cdata;
   int i;
   Atom propAtom;
   char * p;
   DtWsmCBContext handle;
   char * errmsg;

   w = str_to_wtab(cmd, widget);
   if (w == NULL) 
      return(1);

   if (propAtomStr)
   {
      propAtom = (Atom)strtoul(propAtomStr, &p, 0);
      if (p == propAtomStr)
      {
         errmsg = strdup(GetSharedMsg(DT_BAD_ATOM));
         printerrf(cmd, errmsg, propAtomStr, NULL, NULL, NULL, NULL, NULL,
                   NULL, NULL);
         free(errmsg);
         return(1);
      }
   }
   else
      propAtom = None;

   if (handleStr)
   {
      handle = (DtWsmCBContext)strtoul(handleStr, &p, 0);
      if (p == handleStr)
      {
         errmsg = strdup(GETMESSAGE(5,20, 
                      "The following is an invalid callback handle: %s"));
         printerrf(cmd, errmsg, handleStr, NULL, NULL, NULL, NULL, NULL,
                   NULL, NULL);
         free(errmsg);
         return(1);
      }
   }
   else
      handle = NULL;

   /* Locate the matching table entry */
   if ((i = LocateCBRecord (w, cbName, kshcmd, propAtom, handle)) >= 0)
   {
      cdata = cbDataTable[i];
      if (strcmp(cmd, "XtRemoveCallback") == 0)
         XtRemoveCallback(w->w, cbName, (XtCallbackProc)stdCB,(XtPointer)cdata);
      else if (strcmp(cmd, "XmRemoveWMProtocolCallback") == 0)
      {
         XmRemoveWMProtocolCallback(w->w, propAtom, (XtCallbackProc)stdCB, 
                                    (XtPointer)cdata);
      }
      else 
      {
         DtWsmRemoveWorkspaceCallback(handle);
      }

      if (--(cdata->refCount) <= 0)
      {
         XtFree(cdata->ksh_cmd);
         XtFree(cdata->cbname);
         XtFree((XtPointer)cdata);
         cbDataTable[i] = NULL;
         return(0);
      }

      return(0);
   }

   errmsg = strdup(GETMESSAGE(5,21, 
            "The specified callback is not registered"));
   printerr(cmd, errmsg, NULL);
   free(errmsg);
   return(1);
}

int
do_XtAddEventHandler(
        int argc,
        char **argv )
{
   wtab_t *w;
   char *arg0 = argv[0];
   dtksh_event_handler_data_t *ehdata;
   Boolean nonMaskable;
   EventMask eventMask;
   XrmValue fval, tval;
   char * errmsg;

   if (argc != 5)
   {
      errmsg=strdup(GETMESSAGE(5,22, 
               "Usage: XtAddEventHandler widget mask nonMaskable ksh-command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(arg0, argv[1]);
   if (w == NULL) 
      return(1);

   fval.addr = argv[3];
   fval.size = strlen(argv[3]);
   XtConvert(Toplevel, XtRString, &fval, XtRBoolean, &tval);

   if (tval.size != 0) 
      nonMaskable = *((Boolean *)(tval.addr));
   else
      return(1);

   fval.addr = argv[2];
   fval.size = strlen(argv[2]);
   XtConvert(Toplevel, XtRString, &fval, "EventMask", &tval);

   if (tval.size != 0) 
      eventMask = *((EventMask *)(tval.addr));
   else
      return(1);

   if ((eventMask == 0) && (nonMaskable == False))
      return(1);

   ehdata = GetNewEHData(argv[4], w, eventMask, nonMaskable);

   XtAddEventHandler(w->w, eventMask, nonMaskable, (XtEventHandler)stdEH, 
                     (XtPointer)ehdata);

   return(0);
}

int
do_XtRemoveEventHandler(
        int argc,
        char **argv )
{
   wtab_t *w;
   char *arg0 = argv[0];
   dtksh_event_handler_data_t *ehdata;
   int i;
   Boolean nonMaskable;
   EventMask eventMask;
   XrmValue fval, tval;
   char * errmsg;

   if (argc != 5)
   {
      errmsg =strdup(GETMESSAGE(5,23, 
            "Usage: XtRemoveEventHandler widget mask nonMaskable ksh-command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(arg0, argv[1]);
   if (w == NULL) 
      return(1);

   /* Locate the matching table entry */
   if ((i = LocateEHRecord (w, argv[4])) >= 0)
   {
      ehdata = ehDataTable[i];

      fval.addr = argv[3];
      fval.size = strlen(argv[3]);
      XtConvert(Toplevel, XtRString, &fval, XtRBoolean, &tval);

      if (tval.size != 0) 
      {
         nonMaskable = *((Boolean *)(tval.addr));

         /* See if non-maskable event processing has been turned off */
         if (nonMaskable)
            ehdata->nonMaskable = False;
      }
      else
         return(1);

      fval.addr = argv[2];
      fval.size = strlen(argv[2]);
      XtConvert(Toplevel, XtRString, &fval, "EventMask", &tval);

      if (tval.size != 0) 
      {
         eventMask = *((EventMask *)(tval.addr));

         /* Disable the specified set of events */
         ehdata->eventMask &= ~eventMask;
      }
      else
         return(1);

      XtRemoveEventHandler (w->w, eventMask, nonMaskable, (XtEventHandler)stdEH,
                            (XtPointer)ehdata);

      if ((ehdata->eventMask == 0) && (ehdata->nonMaskable == False))
      {
         /* It is now safe to remove this entry */
         XtFree(ehdata->ksh_cmd);
         XtFree((XtPointer)ehdata);
         ehDataTable[i] = NULL;
         return(0);
      }

      return(0);
   }

   errmsg = strdup(GETMESSAGE(5,24, 
                   "The specified event handler is not registered"));
   printerr(arg0, errmsg, NULL);
   free(errmsg);
   return(1);
}

int
do_XtGetValues(
        int argc,
        char **argv )
{
	int i, j;
	int n;
	char *arg0 = argv[0];
	char *val, *p, *str;
	Arg args[MAXARGS];
	char *envar[MAXARGS];
	wtab_t *w;
        char * errmsg;

	if (argc < 3) {
                errmsg = strdup(GETMESSAGE(5,25, 
                        "Usage: XtGetValues widget resource:variable ..."));
		printerr(str_nill, errmsg, NULL);
                free(errmsg);
		return(1);
	}
	w = str_to_wtab(argv[0], argv[1]);
	argv += 2;
	argc -= 2;
	if (w == NULL) {
		return(1);
	}
	/*
	 * Arguments are of the form:
	 *
	 *     resource:envar
	 */

	for (i = 0, n = 0; i < argc; i++) {
		if ((p = strchr(argv[i], ':')) == NULL) {
                        errmsg=strdup(GETMESSAGE(5,26, 
                                "The following resource parameter is incorrectly formed: %s"));
	 	   	printerrf(arg0, errmsg, argv[i],
                                  NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			continue;
		}
		*p = '\0';

                /*
                 * The following special check fixes a bug in Xt, where the
                 * string defined for the XmNwaitForWm resource does not
                 * follow the naming conventions, and is set to "waitforwm".
                 * In dtksh, users expect the naming conventions to be
                 * followed, and this breaks for this one resource.
                 */
                if (strcmp(argv[n], "waitForWm") == 0)
			args[n].name = strdup(XmNwaitForWm);
		else
			args[n].name = strdup(argv[n]);

		envar[n] = &p[1];
		*p = ':';
		args[n].value = (XtArgVal)stakalloc(256);
		n++;
	}
	XtGetValues(w->w, args, n);
	for (i = 0; i < n; i++) {
		if (ConvertTypeToString(arg0, w->wclass, w, w->parent, args[i].name, args[i].value, &str) != FAIL) {
			env_set_var(envar[i], str);
		}
                else
			env_blank(envar[i]);
		XtFree(args[i].name);
	}
	return(0);
}

int
do_XtSetValues(
        int argc,
        char **argv )
{
	int n;
	char *arg0 = argv[0];
	Arg args[MAXARGS];
	wtab_t *w;
        char * errmsg;
        int pargc;
        char ** pargv;

	if (argc < 3) {
		errmsg = strdup(GETMESSAGE(5,27, 
                            "Usage: XtSetValues widget arg:val ..."));
		printerr(str_nill, errmsg, NULL);
                free(errmsg);
		return(1);
	}
	w = str_to_wtab(argv[0], argv[1]);
	argv += 2;
	argc -= 2;
	if (w == NULL) {
		return(1);
	} else {
        	pargc = 0;
        	pargv = (char **)XtMalloc(sizeof(char *) * argc);
		n = 0;
		parse_args(arg0, argc, argv, w, w->parent, w->wclass, &n, args,
			   &pargc, pargv, True);
		if (n > 0) 
			XtSetValues(w->w, args, n);

               	/* Process any postponed resources */
               	if (pargc > 0)
               	{
                       	free_args(n, args);
                       	n = 0;
                       	parse_args(arg0, pargc, pargv, w, w->parent, 
				w->wclass, &n, args, NULL, NULL, False);
                       	XtSetValues(w->w, args, n);
               	}
		free_args(n, args);
		XtFree((char *)pargv);
	}
	return(0);
}


/*
 * When a timeout or work proc is added, the memory allocated for the
 * clientData (i.e. the command string) will be lost, unless we provide
 * a means of associating the string with the workproc/timeout id, and
 * then free up the memory when the workproc/timeout is removed.  The
 * following two functions implement such a mechanism.  This prevents
 * a memory leak from occurring.
 */

static void
RegisterCmdStr(
        char type,
        long id,
        char *cmd )
{
   CommandString **table;
   int * tableSize;

   if (type == WORKPROC_CMDS)
   {
      table = &workProcCmds;
      tableSize = &workProcCmdsSize;
   }
   else
   {
      table = &timeOutCmds;
      tableSize = &timeOutCmdsSize;
   }

   (*tableSize)++;
   *table = (CommandString *)XtRealloc((char *)*table,
                                       sizeof(CommandString) * (*tableSize));
   (*table)[(*tableSize)-1].id = id;
   (*table)[(*tableSize)-1].cmd = cmd;
}

static void
RemoveCmdStr(
        char type,
        long id )
{
   CommandString **table;
   int * tableSize;
   int i, j;

   if (type == WORKPROC_CMDS)
   {
      table = &workProcCmds;
      tableSize = &workProcCmdsSize;
   }
   else
   {
      table = &timeOutCmds;
      tableSize = &timeOutCmdsSize;
   }

   for (i = 0; i < (*tableSize); i++)
   {
      if (id == (*table)[i].id)
      {
         XtFree((*table)[i].cmd);
         (*tableSize)--;

         for (j = i; j < (*tableSize); j++)
            (*table)[j] = (*table)[j+1];
 
         *table = (CommandString *)XtRealloc((char *) (*table),
                                    sizeof(CommandString) * (*tableSize));
         break;
      }
   }
}


int
do_XtAddWorkProc(
        int argc,
        char *argv[] )
{
   char *variable;
   char *cmd;
   char buf[256];
   XtWorkProcId id;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,28, 
                      "Usage: XtAddWorkProc variable command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   variable = argv[1];
   cmd = strdup((char *)argv[2]);
   id = XtAddWorkProc((XtWorkProc)stdWorkProcCB, (XtPointer)cmd);
   RegisterCmdStr(WORKPROC_CMDS, (long)id, cmd);
   sprintf(buf, "0x%lx", (long)id);
   alt_env_set_var(variable, buf);
   return(0);
}

int
do_XtRemoveWorkProc(
        int argc,
        char *argv[] )
{
   XtWorkProcId id;
   char *p;
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,29, "Usage: XtRemoveWorkProc workProcId"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   id = strtoul(argv[1], &p, 16);
   if (p == argv[1]) 
   {
      errmsg=strdup(GETMESSAGE(5,30, 
                   "The workProcId parameter must be a hex number: %s"));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }
   XtRemoveWorkProc(id);
   RemoveCmdStr(WORKPROC_CMDS, (long)id);
   return(0);
}

int
do_XtAddTimeOut(
        int argc,
        char *argv[] )
{
   unsigned long milliseconds = 0;
   wtab_t *w;
   char *variable;
   char *cmd;
   char buf[256];
   XtIntervalId id;
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,31, 
               "Usage: XtAddTimeOut variable milliseconds command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   variable = argv[1];
   if ((milliseconds = atol(argv[2])) <= 0) 
   {
      errmsg = strdup(GETMESSAGE(5,32, 
                "The milliseconds parameter must be greater than zero"));
      printerr(argv[0], errmsg, NULL);
      free(errmsg);
      alt_env_set_var(variable, str_nill);
      return(1);
   }

   cmd = strdup((char *)argv[3]);
   id = XtAddTimeOut(milliseconds, (XtTimerCallbackProc)stdTimerCB, 
                     (XtPointer)cmd);
   RegisterCmdStr(TIMEOUT_CMDS, (long)id, cmd);
   sprintf(buf, "0x%lx", (long)id);
   alt_env_set_var(variable, buf);
   return(0);
}

int
do_XtRemoveTimeOut(
        int argc,
        char *argv[] )
{
   XtIntervalId id;
   char *p;
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,33, "Usage: XtRemoveTimeOut intervalId"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   id = strtoul(argv[1], &p, 16);
   if (p == argv[1]) {
      errmsg = strdup(GETMESSAGE(5,34, 
                   "The intervalId parameter must be a hex number: %s"));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }
   XtRemoveTimeOut(id);
   RemoveCmdStr(TIMEOUT_CMDS, (long)id);
   return(0);
}

int
do_XtUnmanageChildren(
        int argc,
        char *argv[] )
{
	return(do_managelist_func(argc, argv, (int (*)())XtUnmanageChildren));
}

int
do_XtManageChildren(
        int argc,
        char *argv[] )
{
	return(do_managelist_func(argc, argv, (int (*)())XtManageChildren));
}

int
do_managelist_func(
        int argc,
        char *argv[],
        int (*func)() )
{
	wtab_t *w;
	int i;
	Widget widgets[MAXARGS];
	Cardinal nwidgets;
        char * errmsg;

	if (argc < 2) {
		errmsg = strdup(GETMESSAGE(5,35, "Usage: %s widget ..."));
		printerrf(str_nill, errmsg, argv[0], NULL, NULL,
                          NULL, NULL, NULL, NULL, NULL);
                free(errmsg);
		return(1);
	}
	for (nwidgets = 0, i = 1; i < argc && nwidgets < MAXARGS; i++) {
		w = str_to_wtab(argv[0], argv[i]);
		if (w != NULL) {
			widgets[nwidgets++] = w->w;
		}
	}
	func(widgets, nwidgets);
	return(0);
}


int
do_XtIsSubclass(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   int i;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,120, 
               "Usage: XtIsSubclass widget class"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w != NULL) 
   {
      for (i = 0; C[i].cname; i++)
      {
         if (strcmp(argv[2], C[i].cname) == 0)
            return(!XtIsSubclass(w->w, C[i].class));
      }
   }
   errmsg = strdup(GETMESSAGE(5,121, 
               "%s is not a valid widget class name"));
   printerrf(str_nill, errmsg, argv[2], NULL, NULL,
             NULL, NULL, NULL, NULL, NULL);
   free(errmsg);
   return(255);
}


int
do_XtClass(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   int i;
   char * errmsg;
   WidgetClass class;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,122, 
               "Usage: XtClass variable widget"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[2]);

   if (w != NULL) 
   {
      class = XtClass(w->w);
      for (i = 0; C[i].cname; i++)
      {
         if (C[i].class == class)
         {
            alt_env_set_var(argv[1], C[i].cname);
            return(0);
         }
      }
   }

   alt_env_set_var(argv[1], str_nill);
   return(255);
}



#define PARSE_POINTLIST (-1)
#define PARSE_SEGMENTLIST (-2)
#define PARSE_AREA (-3)

GC Standard_GC;

int
create_standard_gc(
        Display *display,
        Window drawable )
{
   Standard_GC = XCreateGC(display, drawable, 0, NULL);
   return(0);
}

int
do_XBell(
        int argc,
        char *argv[] )
{
   int volume;
   Display * display;
   char * p;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg=strdup(GETMESSAGE(5,36, "Usage: XBell display volume"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   } 
   else
      volume = atoi(argv[2]);

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   if (volume < -100)
      volume = -100;
   else if (volume > 100)
      volume = 100;

   XBell(display, volume);
   return(0);
}

static int
do_RootWindowCmd(
        int (*func)(),
        int argc,
        char *argv[] )
{
   Screen * screen;
   char * p;
   char buf[128];
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,37, "Usage: %s variable screen"));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL, NULL, NULL, NULL, 
                NULL, NULL);
      free(errmsg);
      return(1);
   } 

   screen = (Screen *)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GETMESSAGE(5,38, "The screen parameter is invalid: %s"));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   sprintf(buf, "%ld", (long)(*func)(screen));
   alt_env_set_var(argv[1], buf);
   return(0);
}

int
do_XRootWindowOfScreen(
        int argc,
        char *argv[] )
{
   return(do_RootWindowCmd((int (*)())XRootWindowOfScreen, argc, argv));
}

int
do_XWidthOfScreen(
        int argc,
        char *argv[] )
{
   return(do_RootWindowCmd(XWidthOfScreen, argc, argv));
}

int
do_XHeightOfScreen(
        int argc,
        char *argv[] )
{
   return(do_RootWindowCmd(XHeightOfScreen, argc, argv));
}

int
do_XDefineCursor(
        int argc,
        char *argv[] )
{
   Cursor cursor;
   Display * display;
   Window window;
   char * p;
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,39, 
                      "Usage: XDefineCursor display window cursorId"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   } 

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   window = (Window)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_WINDOW));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   cursor = (Cursor)strtoul(argv[3], &p, 0);
   if (p == argv[3]) 
   {
      errmsg = strdup(GETMESSAGE(5,40, 
                      "The cursorId parameter is invalid: %s"));
      printerrf(argv[0], errmsg, argv[3], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   XDefineCursor(display, window, cursor);
   return(0);
}

int
do_XUndefineCursor(
        int argc,
        char *argv[] )
{
   Display * display;
   Window window;
   char * p;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,41, 
                      "Usage: XUndefineCursor display window"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   } 

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   window = (Window)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_WINDOW));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   XUndefineCursor(display, window);
   return(0);
}

int
do_XtRemoveAllCallbacks(
        int argc,
        char *argv[] )
{
	wtab_t *w;
	int i;
        char * errmsg;

	if (argc != 3) {
                errmsg = strdup(GETMESSAGE(5,42, 
                          "Usage: XtRemoveAllCallbacks widget callbackName"));
		printerr(str_nill, errmsg, NULL);
                free(errmsg);
		return(1);
	}
	w = str_to_wtab(argv[0], argv[1]);
	if (w != NULL) {
		XtRemoveAllCallbacks(w->w, argv[2]);
		return(0);
	} else
		return(1);
}

static int
cvtfontstruct(
        char *name,
        XFontStruct **fn )
{
        XrmValue fval, tval;

        fval.addr = name;
        fval.size = strlen(name);
        XtConvert(Toplevel, XtRString, &fval, XtRFontStruct, &tval);

        if (tval.size != 0) {
                *fn = ((XFontStruct **)(tval.addr))[0];
                return(SUCCESS);
        } else
                return(FAIL);
}

static int
CatchNonFatalFontError(
        Display *display,
        XErrorEvent *event )

{
   invalidFont = True;
}

static int
cvtfont(
        Display *display,
        char *name,
        Font *fn )
{
   int (*oldHandler)();

   invalidFont = False;
   oldHandler = XSetErrorHandler(CatchNonFatalFontError);
   *fn = XLoadFont(display, name);
   XSync(display, False);
   XSetErrorHandler(oldHandler);

   if (!invalidFont)
      return(SUCCESS);
   else
      return(FAIL);
}

static int
cvtcolor(
        char *name,
        Pixel *pix )
{
        XrmValue fval, tval;

        fval.addr = name;
        fval.size = strlen(name);
        XtConvert(Toplevel, XtRString, &fval, XtRPixel, &tval);

        if (tval.size != 0) {
                *pix = ((Pixel *)(tval.addr))[0];
                return(SUCCESS);
        } else
                return(FAIL);
}

int
do_XTextWidth(
        int argc,
        char *argv[] )
{
   XFontStruct *fn;
   char *s;
   char buf[128];
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,43, 
                      "Usage: XTextWidth variable fontName string"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }
   if (cvtfontstruct(argv[2], &fn) != SUCCESS)
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_FONT));
      printerrf(argv[0], errmsg, argv[2], NULL,
                NULL, NULL, NULL, NULL, NULL, NULL);
      free(errmsg);
      return(1);
   }
   s = argv[3];
   sprintf(buf, "%ld", (long)XTextWidth(fn, s, strlen(s)));
   alt_env_set_var(argv[1], buf);
   return(0);
}

#define MAXDRAWARGS 6
#define LINE_ARGS 1
#define POLYGON_ARGS 2

static int
invokeXDrawFunction(
        int function,
        int argc,
        char *argv[] )
{
   char * functionName = argv[0];
   Display * display;
   Window drawable;
   Window destination;
   int srcX, srcY;
   int destX, destY;
   unsigned int width, height;
   char *s;
   char *sp;
   int i;
   int mode, parse;
   int text = FALSE;
   int (*func)();
   int argtype = 0;
   int polymode;
   int coordmode;
   GC  gc = NULL;
   int p[MAXDRAWARGS];
   int returnVal = 0;
   Boolean unknownOption;
   Boolean userSpecifiedGC = False;
   char * errmsg;

   if (argc < 3)
   {
      errmsg = strdup(GETMESSAGE(5,44, 
                      "Usage: %s display drawable [args ...]"));
      printerrf(str_nill, errmsg, functionName, NULL, NULL, NULL, NULL, NULL, 
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &sp, 0);
   if (sp == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }
   drawable = (Window)strtoul(argv[2], &sp, 0);
   if (sp == argv[2]) 
   {
      errmsg = strdup(GETMESSAGE(5,45, 
                      "The drawable parameter is invalid: %s"));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   if (function == COPY_AREA) 
   {
      parse = 0;
      func = XCopyArea;

      destination = (Window)strtoul(argv[3], &sp, 0);
      if (sp == argv[3]) 
      {
         errmsg = strdup(GETMESSAGE(5,46, 
                         "The destination parameter is invalid: %s"));
         printerrf(argv[0], errmsg, argv[3], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
         free(errmsg);
         return(1);
      }
      srcX = atoi(argv[4]);
      srcY = atoi(argv[5]);
      width = atoi(argv[6]);
      height = atoi(argv[7]);
      destX = atoi(argv[8]);
      destY = atoi(argv[9]);
      argc -= 7;
      argv += 7;
   } 
   else if (function == DRAW_RECTANGLE) 
   {
      parse = 4;
      func = XDrawRectangle;
   } 
   else if (function == FILL_RECTANGLE) 
   {
      parse = 4;
      func = XFillRectangle;
   } 
   else if (function == FILL_POLYGON) 
   {
      parse = PARSE_POINTLIST;
      func = XFillPolygon;
      argtype = POLYGON_ARGS;
      polymode = Complex;
      coordmode = CoordModeOrigin;
      if (argc > 3)
      {
         while (argv[3][0] == '-') 
         {
            if (strcmp(argv[3], "-Complex") == 0) 
               polymode = Complex;
            else if (strcmp(argv[3], "-Convex") == 0) 
               polymode = Convex;
            else if (strcmp(argv[3], "-Nonconvex") == 0)
               polymode = Nonconvex;
            else if (strcmp(argv[3], "-CoordModeOrigin") == 0) 
               coordmode = CoordModeOrigin;
            else if (strcmp(argv[3], "-CoordModePrevious") == 0) 
               coordmode = CoordModePrevious;
            else
               break;

            argc--;
            argv++;
         }
      }
   } 
   else if (function == DRAW_LINE) 
   {
      parse = 4;
      func = XDrawLine;
   } 
   else if (function == DRAW_SEGMENTS) 
   {
      parse = PARSE_SEGMENTLIST;
      func = XDrawSegments;
   } 
   else if (function == DRAW_LINES) 
   {
      parse = PARSE_POINTLIST;
      func = XDrawLines;
      argtype = LINE_ARGS;
      coordmode = CoordModeOrigin;
      if (argc > 3)
      {
         while (argv[3][0] == '-') 
         {
            if (strcmp(argv[3], "-CoordModeOrigin") == 0) 
               coordmode = CoordModeOrigin;
            else if (strcmp(argv[3], "-CoordModePrevious") == 0) 
               coordmode = CoordModePrevious;
            else
               break;

            argc--;
            argv++;
         }
      }
   } 
   else if (function == DRAW_STRING) 
   {
      parse = 2;
      text = TRUE;
      func = XDrawString;
   } 
   else if (function == DRAW_IMAGE_STRING) 
   {
      parse = 2;
      text = TRUE;
      func = XDrawImageString;
   } 
   else if (function == DRAW_ARC) 
   {
      parse = 6;
      func = XDrawArc;
   } 
   else if (function == FILL_ARC) 
   {
      parse = 6;
      func = XFillArc;
   } 
   else if (function == DRAW_POINT) 
   {
      parse = 2;
      func = XDrawPoint;
   } 
   else if (function == DRAW_POINTS) 
   {
      parse = PARSE_POINTLIST;
      func = XDrawPoints;
      argtype = LINE_ARGS;
      coordmode = CoordModeOrigin;
      if (argc > 3)
      {
         while (argv[3][0] == '-') 
         {
            if (strcmp(argv[3], "-CoordModeOrigin") == 0) 
               coordmode = CoordModeOrigin;
            else if (strcmp(argv[3], "-CoordModePrevious") == 0)
               coordmode = CoordModePrevious;
            else
               break;

            argc--;
            argv++;
         }
      }
   } 
   else if (function == CLEAR_WINDOW) 
   {
      parse = 0;
      func = XClearWindow;
   } 
   else if (function == CLEAR_AREA) 
   {
      parse = PARSE_AREA;
      func = XClearArea;
   }

   if (Standard_GC == NULL)
      create_standard_gc(display, drawable);

   while (argc > 4 && argv[3][0] == '-') 
   {
      if (gc == NULL)
         gc = XCreateGC(display, drawable, 0, NULL);

      if (strcmp(argv[3], "-gc") == 0) 
      {
         XFreeGC(display, gc);
         gc = (GC) atol(argv[4]);
         userSpecifiedGC = True;
      } 
      else if (strcmp(argv[3], "-foreground") == 0) 
      {
         Pixel pix;

         if (cvtcolor(argv[4], &pix) == SUCCESS)
            XSetForeground(display, gc, pix);
      } 
      else if (strcmp(argv[3], "-background") == 0) 
      {
         Pixel pix;

         if (cvtcolor(argv[4], &pix) == SUCCESS)
            XSetBackground(display, gc, pix);
      } 
      else if (strcmp(argv[3], "-font") == 0) 
      {
         Font fn;

         if (cvtfont(display, argv[4], &fn) == SUCCESS)
            XSetFont(display, gc, fn);
         else
         {
            errmsg = strdup(GetSharedMsg(DT_BAD_FONT));
            printerrf(functionName, errmsg, argv[4], NULL, NULL, NULL, 
                      NULL, NULL, NULL, NULL);
            free(errmsg);
            returnVal = 1;
         }
      } 
      else if (strcmp(argv[3], "-line_width") == 0) 
      {
         XGCValues v;

         v.line_width = atoi(argv[4]);
         XChangeGC(display, gc, GCLineWidth, &v);
      } 
      else if (strcmp(argv[3], "-function") == 0) 
      {
         XGCValues v;
         long f;

         unknownOption = False;

         if (strcmp(argv[4], "xor") == 0) 
            f = GXxor;
         else if (strcmp(argv[4], "or") == 0)
            f = GXor;
         else if (strcmp(argv[4], "clear") == 0)
            f = GXclear;
         else if (strcmp(argv[4], "and") == 0)
            f = GXand;
         else if (strcmp(argv[4], "copy") == 0)
            f = GXcopy;
         else if (strcmp(argv[4], "noop") == 0)
            f = GXnoop;
         else if (strcmp(argv[4], "nor") == 0)
            f = GXnor;
         else if (strcmp(argv[4], "nand") == 0)
            f = GXnand;
         else if (strcmp(argv[4], "set") == 0)
            f = GXset;
         else if (strcmp(argv[4], "invert") == 0)
            f = GXinvert;
         else if (strcmp(argv[4], "equiv") == 0)
            f = GXequiv;
         else if (strcmp(argv[4], "andReverse") == 0)
            f = GXandReverse;
         else if (strcmp(argv[4], "orReverse") == 0)
            f = GXorReverse;
         else if (strcmp(argv[4], "copyInverted") == 0)
            f = GXcopyInverted;
         else 
         {
            errmsg = strdup(GETMESSAGE(5,47, 
                      "Unrecognized graphics function name: %s"));
            printerrf(functionName, errmsg, argv[4],
                      NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            free(errmsg);
            returnVal = 1;
            unknownOption = True;
         }

         if (!unknownOption)
         {
            v.function = f;
            XChangeGC(display, gc, GCFunction, &v);
         }
      } 
      else if (strcmp(argv[3], "-line_style") == 0) 
      {
         XGCValues v;
         long f;

         unknownOption = False;

         if (strcmp(argv[4], "LineSolid") == 0)
            f = LineSolid;
         else if (strcmp(argv[4], "LineDoubleDash") == 0)
            f = LineDoubleDash;
         else if (strcmp(argv[4], "LineOnOffDash") == 0)
            f = LineOnOffDash;
         else {
            errmsg = strdup(GETMESSAGE(5,48, "Unrecognized line style: %s"));
            printerrf(functionName, errmsg,
                       argv[4], NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            free(errmsg);
            returnVal = 1;
            unknownOption = True;
         }

         if (!unknownOption)
         {
            v.line_style = f;
            XChangeGC(display, gc, GCLineStyle, &v);
         }
      } 
      else 
      {
         errmsg = strdup(GETMESSAGE(5,49, "Unrecognized drawing option: %s"));
         printerrf(functionName, errmsg, argv[3], NULL, NULL,
                   NULL, NULL, NULL, NULL, NULL);
         free(errmsg);
         returnVal = 1;
      }

      argv += 2;
      argc -= 2;
   }

   if (gc == NULL)
      gc = Standard_GC;

   argc -= 3;
   argv += 3;
   if (parse == PARSE_POINTLIST) 
   {
      XPoint *points = (XPoint *)malloc(sizeof(XPoint )*(argc/2+1));
      int npoints = 0;

      for (i = 0; i < argc-1; i += 2, npoints++) 
      {
         points[npoints].x = atoi(argv[i]);
         points[npoints].y = atoi(argv[i+1]);
      }

      switch (argtype) 
      {
         case POLYGON_ARGS:
         {
            (*func)(display, drawable, gc, points, argc/2, polymode, coordmode);
            break;
         }

         case LINE_ARGS:
         {
            (*func)(display, drawable, gc, points, argc/2, coordmode);
            break;
         }
      }

      free(points);
      argc -= 2*npoints;
      argv += 2*npoints;
   }
   else if (parse == PARSE_SEGMENTLIST) 
   {
      XSegment *segments;
      int nsegments = 0;

      segments = (XSegment *)malloc(sizeof(XSegment )*(argc/4+1));

      for (i = 0; i < argc-1; i += 4, nsegments++) 
      {
         segments[nsegments].x1 = atoi(argv[i]);
         segments[nsegments].y1 = atoi(argv[i+1]);
         segments[nsegments].x2 = atoi(argv[i+2]);
         segments[nsegments].y2 = atoi(argv[i+3]);
      }

      (*func)(display, drawable, gc, segments, argc/4);
      free(segments);
      argc -= 4*nsegments;
      argv += 4*nsegments;
   } 
   else if (parse == PARSE_AREA) 
   {
      Boolean exposures = False;
      XrmValue fval, tval;

      for (i = 0; i < 4 && argc > 0; i++) 
      {
         p[i] = atoi(argv[0]);
         argc --;
         argv ++;
      }

      if (argc > 0)
      {
         fval.addr = argv[0];
         fval.size = strlen(argv[0]);
         XtConvert(Toplevel, XtRString, &fval, XtRBoolean, &tval);

         if (tval.size != 0) 
            exposures = *((Boolean *)(tval.addr));

         argc --;
         argv ++;
      }

      (*func)(display, drawable, p[0], p[1], p[2], p[3], exposures);
   }
   else 
   {
      while (argc >= parse) 
      {
         for (i = 0; i < parse && i < argc; i++) 
            p[i] = atoi(argv[i]);

         if (text) 
         {
            (*func)(display, drawable, gc, 
                    p[0], p[1], argv[i], strlen(argv[i]));
            argc--;
            argv++;
         } 
         else if (func == XClearWindow)
            (*func)(display, drawable);
         else if (func == XCopyArea)
         {
            (*func)(display, drawable, destination, gc,
                    srcX, srcY, width, height, destX, destY);
         }
         else
         {
            (*func)(display, drawable, gc, 
                    p[0], p[1], p[2], p[3], p[4], p[5]);
         }

         argc -= parse;
         argv += parse;
         if (parse == 0)
            break;
      }
   }

   if ((gc != Standard_GC) && !userSpecifiedGC)
      XFreeGC(display, gc);

   if (argc != 0) 
   {
      errmsg = strdup(GETMESSAGE(5,50, 
                  "There were left over points which were ignored"));
      printerr(functionName, errmsg, NULL);
      free(errmsg);
      returnVal = 1;
   }

   return(returnVal);

}
#undef LINE_ARGS 
#undef POLYGON_ARGS

int
do_XDrawArc(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_ARC, argc, argv);
}

int
do_XDrawImageString(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_IMAGE_STRING, argc, argv);
}

int
do_XDrawLine(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_LINE, argc, argv);
}

int
do_XDrawLines(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_LINES, argc, argv);
}

int
do_XDrawPoint(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_POINT, argc, argv);
}

int
do_XDrawPoints(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_POINTS, argc, argv);
}

int
do_XDrawRectangle(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_RECTANGLE, argc, argv);
}

int
do_XCopyArea(
        int argc,
        char *argv[] )
{
   char * errmsg;

   if (argc < 10)
   {
     errmsg = strdup(GETMESSAGE(5,51, 
              "Usage: XCopyArea display source dest sourceX sourceY width height destX destY [args ...]"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   invokeXDrawFunction(COPY_AREA, argc, argv);
}

int
do_XDrawSegments(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_SEGMENTS, argc, argv);
}

int
do_XDrawString(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(DRAW_STRING, argc, argv);
}

int
do_XFillArc(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(FILL_ARC, argc, argv);
}

int
do_XFillPolygon(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(FILL_POLYGON, argc, argv);
}

int
do_XFillRectangle(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(FILL_RECTANGLE, argc, argv);
}

int
do_XClearArea(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(CLEAR_AREA, argc, argv);
}

int
do_XClearWindow(
        int argc,
        char *argv[] )
{
   invokeXDrawFunction(CLEAR_WINDOW, argc, argv);
}

int
ConvertTypeToString(
        char *arg0,
        classtab_t *class,
        wtab_t *w,
        wtab_t *parent,
        char *resource,
        XtArgVal val,
        char **ret )
{
	char *from_type;
	XtResourceList res;
	XrmValue    fr_val, to_val;
	char *nam;
        char * errmsg;

        if ((nam = hashget((Hash_table_t*)class->res, resource)) == NULL) {
		/* If we didn't find it in this widget's class record,
		 * see if the parent is a constraint widget class, and
		 * if so then see if we can find the class there.
		 */
		if (parent == NULL || parent->wclass == NULL ||
	 	  parent->wclass->con == NULL ||
                  (nam = hashget((Hash_table_t*)parent->wclass->con, resource))
                          == NULL)
                {
			errmsg = strdup(GetSharedMsg(DT_UNDEF_RESOURCE));
			printerrf(arg0, errmsg, 
				(char *)(class->cname), resource, NULL, NULL,
                                NULL, NULL, NULL, NULL);
                        free(errmsg);
			return(FAIL);
		}
	}
        res = (XtResourceList)nam;

	/*
	 * unfortunately, we have to have a special case for String
	 * type resources, since their size may vary.
	 */
	if (strcmp(res->resource_type, str_XtRString) == 0) {
		*ret = ((String *)val)[0];
		return(0);
	}
	fr_val.size = res->resource_size;
	fr_val.addr = (caddr_t)val;
	to_val.size = 0;
	to_val.addr = NULL;

	XtConvert(
	    w ? w->w : Toplevel,
	    res->resource_type,	/* from type */
	    &fr_val,	/* from value */
	    str_XtRString,	/* to type */
	    &to_val	/* the converted value */
	);
        if ((to_val.addr) || (strcmp(res->resource_type, XmRXmString) == 0)) {
		*ret = to_val.addr;
	} else {
	    errmsg=strdup(GETMESSAGE(5,52, 
                   "Unable to convert resource type '%s' to 'String'"));
	    printerrf(arg0, errmsg,
                      res->resource_type, NULL, NULL, NULL, NULL, NULL,
                      NULL, NULL);
            free(errmsg);
	    return(FAIL);
	}
	return(SUCCESS);
}

wtab_t *DTKSHConversionWidget;
classtab_t *DTKSHConversionClass;
char *DTKSHConversionResource;

int
ConvertStringToType(
        char *arg0,
        wtab_t *w,
        wtab_t *parent,
        classtab_t *class,
        char *resource,
        char *val,
        XtArgVal *ret,
        int *freeit,
        Boolean postponePixmaps )
{
	char *to_type;
	XtResourceList res;
	XrmValue    fr_val, to_val;
	char *nam;
        char * errmsg;

	DTKSHConversionClass = class;	/* needed by callback converter */
	DTKSHConversionResource = resource;  /* needed by callback converter */
	DTKSHConversionWidget = w;	/* needed by callback converter */

        if ((nam = hashget((Hash_table_t*)class->res, resource)) == NULL) {
		/* If we didn't find it in this widget's class record,
		 * see if the parent is a constraint widget class, and
		 * if so then see if we can find the class there.
		 */
		if (parent == NULL || parent->wclass == NULL ||
			parent->wclass->con == NULL ||
                        (nam = hashget((Hash_table_t*)parent->wclass->con, 
                                        resource)) == NULL)
                {
			errmsg = strdup(GetSharedMsg(DT_UNDEF_RESOURCE));
			printerrf(arg0, errmsg,
				(char *)(class->cname), resource, NULL,
                                NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			return(CONVERT_FAILED);
		}
	}
        res = (XtResourceList)nam;

	/*
	 * Unfortunately, because String types can be variable in size,
	 * we have to handle this as a special case.
	 */
	if (strcmp(res->resource_type, str_XtRString) == 0) {
		*ret = (XtArgVal)strdup(val);
		*freeit = TRUE;
		return(CONVERT_SUCCEEDED);
	}

	fr_val.size = strlen(val) + 1;
	fr_val.addr = (caddr_t)val;
	to_val.size = 0;
	to_val.addr = NULL;

	/*
	 * Hook to allow us to postpone processing of certain classes of
	 * resources.  In particular, Dimension based resources can't be
         * converted until the widget exists, nor can gadget pixmap resources.
         * Any other pixmap resource needs to be postponed until after any
         * color changes have taken effect, otherwise the string to pixmap
         * converter uses the existing colors, instead of the new colors.
	 */
	if (toolkit_special_resource(arg0, res, w, parent, 
		class, resource, val, ret, 
		freeit, postponePixmaps)) {
		return(CONVERT_POSTPONED);
	}
	XtConvert(
	    w ? w->w : Toplevel,
	    str_XtRString,	/* from type */
	    &fr_val,	/* from value */
	    res->resource_type,	/* to type */
	    &to_val	/* the converted value */
	);
	if (to_val.size && to_val.addr) {
		switch(to_val.size) {
		case sizeof(char):
		    *ret = ((char *)to_val.addr)[0];
		    *freeit = FALSE;
		    break;
		case sizeof(short):
		    *ret = (XtArgVal)((short *)to_val.addr)[0];
		    *freeit = FALSE;
		    break;
                case sizeof(int):
		    *ret = (XtArgVal)((int *)to_val.addr)[0];
                    *freeit = FALSE;
                    break;
		default:
		    /*
		     * Deal with sizeof(long) != sizeof(int) here.
		     * Bit of a cheat but it's a simple change.
		     */
		    if (to_val.size == sizeof(long)) {
			*ret = (XtArgVal)((long *)to_val.addr)[0];
			*freeit = FALSE;
			break;
		    }

		    /*
		     * There is a possibility that some
		     * coverters will return malloc'ed space and this
		     * is really unnecessary and will leak memory.  About
		     * the only way to handle this is to handle such types as
		     * special cases.  Maybe we need a hash table that
		     * contains the names of types that need the malloc?
		     * The X specs should really have some mechanism for
		     * knowing when to free the results of a conversion.
		     */
		    *ret = (XtArgVal)XtMalloc(to_val.size);
		    memcpy((char *)ret, to_val.addr, to_val.size);
		    *freeit = TRUE;
		}
	} else {
	    errmsg=strdup(GETMESSAGE(5,53, 
                   "Unable to convert resource type 'String' to type '%s'"));
	    printerrf(arg0, errmsg,
                      res->resource_type, NULL, NULL, NULL, NULL, NULL,
                      NULL, NULL);
            free(errmsg);
	    return(CONVERT_FAILED);
	}
	return(CONVERT_SUCCEEDED);
}

static int
XtAddInputUsage(
        char *arg0 )
{
        char * errmsg;

	errmsg=strdup(GETMESSAGE(5,54, 
          "Usage: XtAddInput variable [-r] fileDescriptor kshCommand"));
	printerr(str_nill, errmsg, NULL);
        free(errmsg);
	return(1);
}

int
do_XtAddInput(
        int argc,
        char *argv[] )
{
   int i, j;
   int fd;
   char *arg0 = argv[0];
   char *variable;
   char *cmd;
   inputrec_t *inp;
   XtInputId id;
   char buf[256];
   char * errmsg;
   unsigned char modeFlags = LINE_INPUT_MODE;


   if (argc < 4)
      return(XtAddInputUsage(arg0));

   variable = argv[1];
   argv+=2;
   argc-=2;

   if (strcmp(argv[0], "-r") == 0)
   {
      /* Raw mode; the registered handler will take care of reading input */
      modeFlags = RAW_INPUT_MODE;
      argv++;
      argc--;
   }

   if (argc != 2)
      return(XtAddInputUsage(arg0));

   fd = atoi(argv[0]);
   argv++;
   argc--;

   inp = (inputrec_t *)XtMalloc(sizeof(inputrec_t));
   if (modeFlags & RAW_INPUT_MODE)
   {
      /* Raw mode; the registered handler will do all buffering */
      inp->lnbufsize = 0;
      inp->lnbuf = NULL;
   }
   else
   {
      inp->lnbufsize = LINESIZE;
      inp->lnbuf = XtMalloc(inp->lnbufsize);
   }
   inp->fd = fd;
   inp->flags = modeFlags;
   inp->lnend = 0;
   inp->cmd = strdup(argv[0]);
   inp->lastCharIsBackslash = False;
   inp->lineWasTouched = False;

   id = XtAddInput(fd, (XtPointer)XtInputReadMask, 
                   (XtInputCallbackProc)stdInputCB, (caddr_t)inp);

   /* 
    * Save a record of this input, so that we can destroy the buffer
    * information when the input handler is unregistered.
    */
   for (i = 0; i < numActiveInputs; i++)
   {
      if (activeInputs[i].inUse == False)
         break;
   }

   if ( i >= numActiveInputs)
   {
      /* Grow the array */
      numActiveInputs += 5;
      activeInputs = (InputRecord *)XtRealloc((char *)activeInputs,
                                      sizeof(InputRecord) * numActiveInputs);
      for (j = i; j < numActiveInputs; j++)
      {
         activeInputs[j].inUse = False;
         activeInputs[j].inp = NULL;
         activeInputs[j].id = 0;
      }
   }
   activeInputs[i].inUse = True;
   activeInputs[i].id = id;
   activeInputs[i].inp = inp;

   sprintf(buf, "0x%lx", (long)id);
   alt_env_set_var(variable, buf);
   return(0);
}

int
do_XtRemoveInput(
        int argc,
        char *argv[] )
{
   XtInputId id;
   char *p;
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,56, "Usage: XtRemoveInput inputId"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   id = strtoul(argv[1], &p, 16);
   if (p == argv[1]) 
   {
      errmsg = strdup(GETMESSAGE(5,57, 
                        "The inputId parameter must be a hex number: %s"));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL,
                NULL, NULL, NULL);
      free(errmsg);
      return(1);
   }

   DestroyInputRecord(id);
   return(0);
}


/*
 * This function will attempt to remove the indicated input source.  If,
 * however, the source is busy (i.e. the XtRemoveInput() request came
 * from within the input handler), then we will simply mark the input
 * source as 'pending destroy', and will allow stdInputCB to do the
 * actual removing when it is safe.
 */
static void
DestroyInputRecord(
        XtInputId id )
{
   inputrec_t * inp;
   int entryIndex;

   if ((entryIndex = FindInputRecord(id)) >= 0)
   {
      inp = activeInputs[entryIndex].inp;

      if (inp->flags & INPUT_SOURCE_BUSY)
         inp->flags |= INPUT_SOURCE_PENDING_DELETE;
      else
      {
         /* It's ok to delete the source now */
         XtRemoveInput(id);
         activeInputs[entryIndex].inUse = False;
         activeInputs[entryIndex].id = 0;
         activeInputs[entryIndex].inp = NULL;
         XtFree(inp->lnbuf);
         XtFree((char *)inp);
      }
   }
}


static int
FindInputRecord(
        XtInputId id )
{
   int i;

   for (i = 0; i < numActiveInputs; i++)
   {
      if ((activeInputs[i].inUse) && (activeInputs[i].id == id))
         return(i);
   }

   return(-1);
}


/*
 * This function will initialize some environment variables, and then
 * invoke the handler registered for this input source.  If will return
 * 'True' if the handler called XtRemoveInput on this source; if this
 * has happened, then the 'inp' structure MUST NOT be touched again,
 * since it will have been freed up.
 */
static Boolean
ProcessInput(
        inputrec_t * inp,
        int source,
        XtInputId id,
        Boolean eofFound )
{
   Namval_t * sourceVar = nv_search("INPUT_SOURCE", sh.var_tree, NV_ADD);
   Namval_t * idVar = nv_search("INPUT_ID", sh.var_tree, NV_ADD);
   Namval_t * eofVar = nv_search("INPUT_EOF", sh.var_tree, NV_ADD);
   Namval_t * lineVar = nv_search("INPUT_LINE", sh.var_tree, NV_ADD);
   char strBuf[25];

   /* Initialize the environment variables */
   sprintf(strBuf, "%d", source);
   nv_putval(sourceVar, strBuf, NV_RDONLY);
   sprintf(strBuf, "0x%lx", (long)id);
   nv_putval(idVar, strBuf, NV_RDONLY);
   sprintf(strBuf, "%s", (eofFound ? "true" : "false"));
   nv_putval(eofVar, strBuf, NV_RDONLY);
   if ((inp->flags & RAW_INPUT_MODE) || (inp->lnend == 0))
   {
      strBuf[0] = '\0';
      nv_putval(lineVar, strBuf, NV_RDONLY);
   }
   else
      nv_putval(lineVar, inp->lnbuf, NV_RDONLY);

   /* Invoke the registered handler */
   inp->flags |= INPUT_SOURCE_BUSY;
   ksh_eval(inp->cmd);
   inp->flags &= ~INPUT_SOURCE_BUSY;

   /* Clean up the environment variables */
   nv_newattr(sourceVar, 0, 0);
   nv_close(sourceVar);
   nv_newattr(idVar, 0, 0);
   nv_close(idVar);
   nv_newattr(eofVar, 0, 0);
   nv_close(eofVar);
   nv_newattr(lineVar, 0, 0);
   nv_close(lineVar);

   /* If the handler removed the input source, then process it now */
   if (inp->flags & INPUT_SOURCE_PENDING_DELETE)
   {
      DestroyInputRecord(id);
      return(True);
   }

   return(False);
}


void
Translation_ksh_eval(
        Widget w,
        XEvent *event,
        String *params,
        Cardinal *num_params )
{
   char buf[128];
   int i;
   Namval_t * np;
   Namval_t * np2;
   wtab_t *wtab = NULL;
   Namfun_t * clonedDisc;

   if (w != NULL) 
      wtab = widget_to_wtab(w);

   nestingLevel++;
   np2 = GetNameValuePair("TRANSLATION_WIDGET");
   nv_newattr(np2, 0, 0);
   nv_putval(np2, (wtab ? wtab->widid : "Unknown"), NV_RDONLY);
   nv_newattr(np2, NV_RDONLY, 0);

   np = GetNameValuePair("TRANSLATION_EVENT");
   nv_newattr(np, 0, 0);
   sprintf(buf, "0x%lx", (long)event);
   nv_putval(np, buf, NV_RDONLY);
   nv_newattr(np, NV_RDONLY, 0);
   clonedDisc = CloneDiscipline(&transDiscipline);
   nv_stack(np, clonedDisc);

   for (i = 0; i < *num_params; i++) 
      ksh_eval(params[i]);

   /* Remove the discipline for the hierarchical variables */
   nv_stack(np, NULL);
   FreeDiscipline(clonedDisc);

   /* Free up all of the name/value pairs we created */
   FreeNestedVariables();
   nestingLevel--;
}


void
RestorePriorEnvVarValues(
        Namval_t *np1,
        char *value1,
        Namval_t *np2,
        char *value2 )
{
   if (value1 && np1)
   {
      nv_newattr(np1, 0, 0);
      nv_putval(np1, value1, NV_RDONLY);
      nv_newattr(np1, NV_RDONLY, 0);
   }
   if (value2 && np2)
   {
      nv_newattr(np2, 0, 0);
      nv_putval(np2, value2, NV_RDONLY);
      nv_newattr(np2, NV_RDONLY, 0);
   }
}

/*
 * stdCB() is the central routine from which all callback
 * functions are dispatched (specified by clientData).  The
 * variables "CB_WIDGET" and "CB_CALL_DATA" will be placed in 
 * the environment to represent the CallBackWidget handle.  
 */

void
stdCB(
        void *widget,
        caddr_t clientData,
        caddr_t callData )
{
   char buf[128];
   dtksh_client_data_t *cdata = (dtksh_client_data_t *)clientData;
   Namval_t * np;
   Namval_t * np2;
   WidgetClass class;
   Namdisc_t * discipline = NULL;
   int i;
   char * oldCB_WIDGET_value = NULL;
   char * oldCB_CALL_DATA_value = NULL;
   char * ptr;
   Namfun_t * clonedDisc;

   /*
    * The wtab_t entry of the cdata need not be filled in since
    * it could have been set via direct resource setting at widget
    * creation time, and the converter for string to callback would
    * not have had access to this information (since the widget
    * was not created yet.
    * Thus, we set it here.  Note that this will happen at most
    * one time, since we are modifying the cdata structure.
    */
   if (cdata->w == NULL) 
      cdata->w = widget_to_wtab(widget);

   nestingLevel++;
   np2 = GetNameValuePair("CB_WIDGET");
   nv_newattr(np2, 0, 0);
   if (ptr = nv_getval(np2))
      oldCB_WIDGET_value = strdup(ptr);
   nv_putval(np2, (cdata->w ? cdata->w->widid : "Unknown"), NV_RDONLY);
   nv_newattr(np2, NV_RDONLY, 0);

   /* Certain callbacks don't pass structures as the calldata */
   if ((cdata->cbname) && 
       ((strcmp(cdata->cbname, XmNpopupCallback) == 0) ||
        (strcmp(cdata->cbname, XmNpopdownCallback) == 0)))
   {
      /* The calldata indicates the grab type */
      XtGrabKind * grabKind = (XtGrabKind *)callData;
      switch (*grabKind)
      {
         case XtGrabNonexclusive:
         {
            strcpy(buf, "GrabNonexclusive");
            break;
         }

         case XtGrabExclusive:
         {
            strcpy(buf, "GrabExclusive");
            break;
         }

         default:
         {
            strcpy(buf, "GrabNone");
            break;
         }
      }
   }
   else
      sprintf(buf, "0x%lx", (long)callData);

   np = GetNameValuePair("CB_CALL_DATA");
   nv_newattr(np, 0, 0);
   if (ptr = nv_getval(np))
      oldCB_CALL_DATA_value = strdup(ptr);
   nv_putval(np, buf, NV_RDONLY);
   nv_newattr(np, NV_RDONLY, 0);

   /* 
    * Add a discipline for hierarchical variables.
    * Need to add a different discipline, based on the callback type,
    * since the fields within the callback structure differ depending
    * upon the type of callback and the widget.  NOTE: the WMProtocol
    * callback will use the default discipline.
    */
   if (cdata->cbname)
   {
      if ((strcmp(cdata->cbname, XmNpopupCallback) == 0) ||
          (strcmp(cdata->cbname, XmNpopdownCallback) == 0) ||
          (strcmp(cdata->cbname, XmNdestroyCallback) == 0))
      {
         discipline = &nopDiscipline;
      }
      else if (strcmp(cdata->cbname, XmNhelpCallback) == 0)
         discipline = &dftDiscipline;
      else
      {
         class = XtClass(cdata->w->w);
         while (class)
         {
            if (discipline = CheckClassDisciplines(class, cdata->cbname))
               break;

            class = class->core_class.superclass;
         }
      }
   }

   /* 
    * If a discipline was found, then use it; otherwise, we MUST set up
    * a default discipline; otherwise, any hierarchical variables 
    * referenced by the user are not under our control, thus never getting
    * freed up, and then also preventing future disciplines from getting
    * called when they should have.
    */
   if (discipline)
      clonedDisc = CloneDiscipline(discipline);
   else
      clonedDisc = CloneDiscipline(&dftDiscipline);
   nv_stack(np, clonedDisc);

   ksh_eval((char *)cdata->ksh_cmd);

   /* We may be nested, so restore old CB_WIDGET & CB_CALL_DATA values */
   RestorePriorEnvVarValues(np2, oldCB_WIDGET_value, np, oldCB_CALL_DATA_value);
   XtFree(oldCB_WIDGET_value);
   XtFree(oldCB_CALL_DATA_value);

   /* Remove the discipline for the hierarchical variables */
   nv_stack(np, NULL);
   FreeDiscipline(clonedDisc);

   /* Free up all of the name/value pairs we created */
   FreeNestedVariables();
   nestingLevel--;

   return;
}

/*
 * This is the callback handler for the 'workspace changed' callback.
 */
void
stdWSCB(
        void *widget,
        Atom atom,
        caddr_t clientData )
{
   char buf[128];
   dtksh_client_data_t *cdata = (dtksh_client_data_t *)clientData;
   Namval_t * np;
   Namval_t * np2;
   int i;
   char * oldCB_WIDGET_value = NULL;
   char * oldCB_CALL_DATA_value = NULL;
   char * ptr;
   Namfun_t * clonedDisc;

   nestingLevel++;
   np2 = GetNameValuePair("CB_WIDGET");
   nv_newattr(np2, 0, 0);
   if (ptr = nv_getval(np2))
      oldCB_WIDGET_value = strdup(ptr);
   nv_putval(np2, cdata->w->widid, NV_RDONLY);
   nv_newattr(np2, NV_RDONLY, 0);

   np = GetNameValuePair("CB_CALL_DATA");
   nv_newattr(np, 0, 0);
   sprintf(buf, "0x%lx", (long)atom);
   if (ptr = nv_getval(np))
      oldCB_CALL_DATA_value = strdup(ptr);
   nv_putval(np, buf, NV_RDONLY);
   nv_newattr(np, NV_RDONLY, 0);
   clonedDisc = CloneDiscipline(&nopDiscipline);
   nv_stack(np, clonedDisc);

   ksh_eval((char *)cdata->ksh_cmd);

   /* We may be nested, so restore old CB_WIDGET & CB_CALL_DATA values */
   RestorePriorEnvVarValues(np2, oldCB_WIDGET_value, np, oldCB_CALL_DATA_value);
   XtFree(oldCB_WIDGET_value);
   XtFree(oldCB_CALL_DATA_value);

   /* Remove the discipline for the hierarchical variables */
   nv_stack(np, NULL);
   FreeDiscipline(clonedDisc);

   /* Free up all of the name/value pairs we created */
   FreeNestedVariables();
   nestingLevel--;
}

void
stdInputCB(
        inputrec_t *inp,
        int *source,
        XtInputId *id )
{
   char buf[LINESIZE];
   char cmdbuf[LINESIZE];
   int cmd;
   char *p;
   int i, n, j;
   char * errmsg;
   int len;

   /* If in 'raw' mode, then simply let the handler do all the work */
   if (inp->flags & RAW_INPUT_MODE)
   {
      ProcessInput(inp, *source, *id, False);
      return;
   }

   /* try to read some input from the fd */
   if ((n = read(inp->fd, buf, sizeof(buf)-1)) <= 0) 
   {
      /* EOF; notify handler, passing in any remaining buffered data */
      if ((inp->lnend > 0) || (inp->lineWasTouched))
      {
         /* Force one call with the data, and a 2nd with the EOF */
         inp->lnbuf[inp->lnend] = '\0';
         ProcessInput(inp, *source, *id, False);
      }
      inp->lastCharIsBackslash = False;
      inp->lineWasTouched = False;
      inp->lnbuf[0] = '\0';
      inp->lnend = 0;
      ProcessInput(inp, *source, *id, True);
      return;
   }

   /* 
    * Go through appending to current line, execute line if you 
    * get an unquoted newline.  Strip off the newline, so that
    * we are consistent with the ksh 'read' command, remove
    * escaped newlines, and do backslash processing.
    */
   for (i = 0; i < n; ) 
   {
#ifdef NLS16
      len = mblen(buf+i, MB_CUR_MAX);
#else
      len = 1;
#endif
      inp->lineWasTouched = True;

      if ((inp->lnend + len) >= (inp->lnbufsize-1))
      {
         /* Grow the input buffer */
         inp->lnbufsize += (n + LINESIZE + 5);
         inp->lnbuf = XtRealloc(inp->lnbuf, inp->lnbufsize);
      }

      /* Perform backslash processing */
      if ((len == 1) && (buf[i] == '\\') && (!inp->lastCharIsBackslash))
      {
         /* Skip this character; the next character will be treated specially */
         inp->lastCharIsBackslash = True;
         i++;
         continue;
      }

      /*
       * If the previous character has been a backslash, then the current
       * character gets placed into the buffer without any special
       * processing; the exception is the newline character, which gets
       * dumped.
       */
      if ((len == 1) && (buf[i] == '\n'))
      {
         /* 
          * If the newline is escaped, then drop it, and continue.
          * Otherwise, process the line.
          */
         i++;
         if (inp->lastCharIsBackslash)
         {
            inp->lastCharIsBackslash = False;
            continue;
         }

         inp->lnbuf[inp->lnend] = '\0';
         if (ProcessInput(inp, *source, *id, False))
         {
            /* The handler called XtRemoveInput() on this source; abort */
            return;
         }
         inp->lnend = 0;
         inp->lineWasTouched = False;
      }
      else
      {
         /* Simply copy the next character into the buffer */
         inp->lastCharIsBackslash = False;
         for (j = 0; j < len; j++)
            inp->lnbuf[inp->lnend++] = buf[i++];
      }
   }
}

int
stdWorkProcCB(
        char *clientData )
{
   int retcode;
   int i;

   retcode = ksh_eval((char *)clientData);
   if (retcode != 0)
   {
      /* This is tricky, because we do not have the workproc id */
      for (i = 0; i < workProcCmdsSize; i++)
      {
         if (clientData == workProcCmds[i].cmd)
         {
            RemoveCmdStr(WORKPROC_CMDS, (long)workProcCmds[i].id);
            break;
         }
      }
   }
   return(retcode);
}

void
stdTimerCB(
        char *clientData,
        long *id )
{
	ksh_eval((char *)clientData);
        RemoveCmdStr(TIMEOUT_CMDS, (long)*id);
	return;
}


int
do_XFlush(
        int argc,
        char *argv[] )
{
   char *p;
   Display * display;
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,59, "Usage: XFlush display"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   XFlush(display);
   return(0);
}

int
do_XSync(
        int argc,
        char *argv[] )
{
   Boolean discard;
   XrmValue fval, tval;
   char *p;
   Display * display;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,60, "Usage: XSync display discard"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   fval.addr = argv[2];
   fval.size = strlen(argv[2]);
   XtConvert(Toplevel, XtRString, &fval, XtRBoolean, &tval);

   if (tval.size != 0) 
      discard = *((Boolean *)(tval.addr));
   else
      return(1);

   XSync(display, discard);
   return(0);
}

int
do_XRaiseWindow(
        int argc,
        char *argv[] )
{
   Boolean discard;
   XrmValue fval, tval;
   wtab_t *w;
   char *p;
   Display * display;
   Window window;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,61, "Usage: XRaiseWindow display window"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }
   window = (Window)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_WINDOW));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   XRaiseWindow(display, window);
   return(0);
}

static int
XtSetSensitive_usage(
        char *arg0 )
{
   char * errmsg;

   errmsg = strdup(GETMESSAGE(5,62, "Usage: %s widget [True|False]"));
   printerrf(str_nill, errmsg, arg0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   free(errmsg);
   return(1);
}

int
do_XtSetSensitive(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   Boolean boolean;
   XrmValue fval, tval;

   if (argc != 3)
      return(XtSetSensitive_usage(argv[0]));

   fval.addr = argv[2];
   fval.size = strlen(argv[2]);
   XtConvert(Toplevel, XtRString, &fval, XtRBoolean, &tval);

   if (tval.size != 0) 
      boolean = *((Boolean *)(tval.addr));
   else
      return(1);

   w = str_to_wtab(argv[0], argv[1]);
   if (w != NULL) 
      XtSetSensitive(w->w, boolean);
   else 
      return(1);
   return(0);
}

static int
RegisterTranslations(
        void (*func)(),
        int argc,
        char *argv[] )
{
   wtab_t *w;
   XtTranslations translationTable;
   XrmValue fval, tval;
   char * errmsg;

   if (argc != 3)
   {
      errmsg = strdup(GETMESSAGE(5,63, "Usage: %s widget translations"));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL,
               NULL, NULL, NULL, NULL, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   fval.addr = argv[2];
   fval.size = strlen(argv[2]);
   XtConvert(Toplevel, XtRString, &fval, XtRTranslationTable, &tval);

   if (tval.size != 0) 
      translationTable = *((XtTranslations *)(tval.addr));
   else
      return(1);

   (*func)(w->w, translationTable);
   return(0);
}

int
do_XtOverrideTranslations(
        int argc,
        char **argv )
{
   return(RegisterTranslations(XtOverrideTranslations, argc, argv));
}

int
do_XtAugmentTranslations(
        int argc,
        char **argv )
{
   return(RegisterTranslations(XtAugmentTranslations, argc, argv));
}

int
do_XtUninstallTranslations(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   XtTranslations * translationTable;
   XrmValue fval, tval;
   char * errmsg;

   if (argc != 2)
   {
      errmsg = strdup(GETMESSAGE(5,64, 
                      "Usage: XtUninstallTranslations widget"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   XtUninstallTranslations(w->w);
   return(0);
}

int
do_XtParent(
        int argc,
        char **argv )
{
   char *arg0 = argv[0];
   char * wname;
   wtab_t *wtab;
   classtab_t *ctab;
   char buf[128];
   char * errmsg;
   
   if (argc != 3 ) {
	errmsg = strdup(GETMESSAGE(5,65, "Usage: XtParent variable widget"));
	printerr(str_nill, errmsg, NULL);
        free(errmsg);
	return(1);
   }
   wname = argv[2];
   wtab = str_to_wtab(arg0, wname);
   if (wtab == NULL) {
	return(1);
   }

   if (wtab->parent == NULL) {
	   wtab = widget_to_wtab(XtParent(wtab->w));
	   if (wtab == NULL)
	     return(1);
	   /*
	    * If the widget class has no resources registered, then this is
	    * the first known instance of this widget class, so we need to
	    * force the resource list to be loaded.  This can frequently
	    * occur if a Motif convenience function is used, which creates
	    * a 'hidden' parent.
	    */
	   ctab = wtab->wclass;
	   if (ctab->res == NULL)
	      (void)str_to_class(arg0, ctab->cname);
   } else
	wtab = wtab->parent;

   sprintf(buf, "%s", wtab->widid);
   alt_env_set_var(argv[1], buf);
   return(0);
}

int
do_XtLastTimestampProcessed(
        int argc,
        char **argv )
{
   char *arg0 = argv[0];
   Display * display;
   char * p;
   char buf[128];
   char * errmsg;
   
   if (argc != 3 ) {
	errmsg = strdup(GETMESSAGE(5,66, 
                 "Usage: XtLastTimestampProcessed variable display"));
	printerr(str_nill, errmsg, NULL);
        free(errmsg);
	return(1);
   }

   display = (Display *)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   sprintf(buf, "%ld", (long)XtLastTimestampProcessed(display));
   alt_env_set_var(argv[1], buf);
   return(0);
}

/**********************************************/

/*
 * The following two functions work for both standard Xt callbacks
 * (which are identified by a callback name) and WM protocol callbacks
 * (which are identified by a property atom).  That is why both of
 * these parameters are passed in.  It is usually the case that only
 * one of the 'cbname' and 'propAtom' parameters are used; the unused
 * one should be set to NULL (cbname) or None (propAtom).
 */

dtksh_client_data_t *
GetNewCBData(
        char *ksh_cmd,
        wtab_t *w,
        char *cbname,
        Atom propAtom )
{
   dtksh_client_data_t * cdata;
   int i;
   int j;

   /* Can we reuse an existing entry? */
   if ((i = LocateCBRecord (w, cbname, ksh_cmd, propAtom, NULL)) >= 0)
   {
      cdata = cbDataTable[i];
      cdata->refCount++;
      return(cdata);
   }

   /* Look for an open slot */
   for (i = 0; i < cbDataTableSize; i++)
   {
      if (cbDataTable[i] == NULL)
         break;
   }

   if (i >= cbDataTableSize)
   {
      /* Need to enlarge the table */
      cbDataTableSize += 10;
      cbDataTable = (dtksh_client_data_t **)
                     XtRealloc((XtPointer)cbDataTable,
                     sizeof(dtksh_client_data_t *) * cbDataTableSize);
      for (j = i; j < cbDataTableSize; j++)
         cbDataTable[j] = NULL;
   }

   cdata = (dtksh_client_data_t *)XtMalloc(sizeof(dtksh_client_data_t));
   if (ksh_cmd)
      cdata->ksh_cmd = strdup(ksh_cmd);
   else
      cdata->ksh_cmd = NULL;
   cdata->w = w;
   if (cbname)
      cdata->cbname = strdup(cbname);
   else
      cdata->cbname = NULL;
   cdata->propAtom = propAtom;
   cdata->handle = NULL;
   cdata->refCount = 1;

   cbDataTable[i] = cdata;
   return(cdata);
}

int
LocateCBRecord(
        wtab_t *w,
        char *cbname,
        char *ksh_cmd,
        Atom propAtom,
        DtWsmCBContext handle )
{
   int i;

   /* Locate the matching table entry */
   for (i = 0; i < cbDataTableSize; i++)
   {
      if (cbDataTable[i])
      {
         if ((((cbname == NULL) && (cbDataTable[i]->cbname == NULL)) ||
             (((cbname != NULL) && (cbDataTable[i]->cbname != NULL)) &&
             (strcmp(cbDataTable[i]->cbname, cbname) == 0))) &&
             (cbDataTable[i]->w == w) &&
             (strcmp(cbDataTable[i]->ksh_cmd, ksh_cmd) == 0) &&
             (cbDataTable[i]->propAtom == propAtom) &&
             (cbDataTable[i]->handle == (XtPointer)handle))
         {
            return(i);
         }
      }
   }

   return(-1);
}

/**********************************************/

void
stdEH(
        void *widget,
        caddr_t clientData,
        XEvent *event,
        Boolean *continueToDispatch )
{
   char buf[128];
   dtksh_event_handler_data_t *ehdata;
   int i;
   Namval_t * np;
   Namval_t * np2;
   Namfun_t * clonedDisc;

   ehdata = (dtksh_event_handler_data_t *)clientData;

   nestingLevel++;
   np2 = GetNameValuePair("EH_WIDGET");
   nv_newattr(np2, 0, 0);
   nv_putval(np2, ehdata->w->widid, NV_RDONLY);
   nv_newattr(np2, NV_RDONLY, 0);

   np = GetNameValuePair("EH_EVENT");
   nv_newattr(np, 0, 0);
   sprintf(buf, "0x%lx", (long)event);
   nv_putval(np, buf, NV_RDONLY);
   clonedDisc = CloneDiscipline(&ehDiscipline);
   nv_stack(np, clonedDisc);

   ksh_eval((char *)ehdata->ksh_cmd);

   /* Remove the discipline for the hierarchical variables */
   nv_stack(np, NULL);
   FreeDiscipline(clonedDisc);

   /* Free up all of the name/value pairs we created */
   FreeNestedVariables();
   nestingLevel--;
}

/*
 * For a given widget, if the ksh-cmd is the same as one already
 * registered for this widget, then we will merge them into a
 * single event handler (by merging the event masks), as is done
 * by Xt anyways.
 */

dtksh_event_handler_data_t *
GetNewEHData(
        char *ksh_cmd,
        wtab_t *w,
        EventMask eventMask,
        Boolean nonMaskable )
{
   dtksh_event_handler_data_t * ehdata;
   int i;
   int j;

   /* Can we merge with an existing entry? */
   if ((i = LocateEHRecord (w, ksh_cmd)) >= 0)
   {
      ehdata = ehDataTable[i];
      if (nonMaskable)
         ehdata->nonMaskable = True;
      ehdata->eventMask |= eventMask;
      return(ehdata);
   }

   /* Look for an open slot */
   for (i = 0; i < ehDataTableSize; i++)
   {
      if (ehDataTable[i] == NULL)
         break;
   }

   if (i >= ehDataTableSize)
   {
      /* Need to enlarge the table */
      ehDataTableSize += 10;
      ehDataTable = (dtksh_event_handler_data_t **)
                     XtRealloc((XtPointer)ehDataTable,
                     sizeof(dtksh_event_handler_data_t *) * ehDataTableSize);
      for (j = i; j < ehDataTableSize; j++)
         ehDataTable[j] = NULL;
   }

   ehdata = (dtksh_event_handler_data_t *)
            XtMalloc(sizeof(dtksh_event_handler_data_t));
   if (ksh_cmd)
      ehdata->ksh_cmd = strdup(ksh_cmd);
   else
      ehdata->ksh_cmd = NULL;
   ehdata->w = w;
   ehdata->eventMask = eventMask;
   ehdata->nonMaskable = nonMaskable;

   ehDataTable[i] = ehdata;
   return(ehdata);
}


static int
LocateEHRecord(
        wtab_t *w,
        char *ksh_cmd )
{
   int i;

   /* Locate the matching event handler table entry */
   for (i = 0; i < ehDataTableSize; i++)
   {
      if (ehDataTable[i])
      {
         if ((ehDataTable[i]->w == w) &&
             (strcmp(ehDataTable[i]->ksh_cmd, ksh_cmd) == 0))
         {
            return(i);
         }
      }
   }

   return(-1);
}

int
do_DtSessionRestorePath(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   char * path;
   Boolean status;
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,67, 
               "Usage: DtSessionRestorePath widget pathVariable saveFile"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   status = DtSessionRestorePath(w->w, &path, argv[3]);
   if (status)
      alt_env_set_var(argv[2], path);
   else
      alt_env_set_var(argv[2], str_nill);

   return (!status);
}

int
do_DtSessionSavePath(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   char * path;
   char * file;
   Boolean status;
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,68, 
               "Usage: DtSessionSavePath widget pathVariable fileVariable"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   status = DtSessionSavePath(w->w, &path, &file);
   if (status)
   {
      env_set_var(argv[2], path);
      env_set_var(argv[3], file);
   }
   else
   {
      env_blank(argv[2]);
      env_blank(argv[3]);
   }

   return (!status);
}

int
do_DtShellIsIconified(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   Boolean status;
   Atom actual_type;
   int  actual_format;
   unsigned long nitems;
   unsigned long leftover;
   WmStateData * wm_state;
   Atom wmStateAtom;
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,69, "Usage: DtShellIsIconified widget"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   wmStateAtom = XmInternAtom (XtDisplay(w->w), "WM_STATE", False);

   /*  Getting the WM_STATE property to see if iconified or not */
   XGetWindowProperty(XtDisplay(w->w), XtWindow (w->w),
                       wmStateAtom, 0L, (long) LINESIZE, False,
                       wmStateAtom, &actual_type, &actual_format,
                       &nitems, &leftover, (unsigned char **) &wm_state);

   return (wm_state->state != IconicState);
}

int
do_DtSetStartupCommand(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   Atom commandAtom;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,70, 
                       "Usage: DtSetStartupCommand widget command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   commandAtom = XA_WM_COMMAND;

   XChangeProperty(XtDisplay(w->w), XtWindow(w->w), commandAtom,
                   XA_STRING, 8, PropModeReplace,
                   (unsigned char *)argv[2], strlen(argv[2])+1);
   XSync(XtDisplay(w->w), False);

   return(0);
}

/* This only works if the widget is not yet realized */
int
do_DtSetIconifyHint(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   Boolean state;
   XrmValue fval, tval;
   XWMHints   *wmhints;
   Arg args[5];
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,71, 
                      "Usage: DtSetIconifyHint widget boolean"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   fval.addr = argv[2];
   fval.size = strlen(argv[2]);
   XtConvert(Toplevel, XtRString, &fval, XtRBoolean, &tval);

   if (tval.size != 0) 
      state = *((Boolean *)(tval.addr));
   else
      return(1);

   if (state)
   {
     /* add the iconify hint to the current shell */
     XtSetArg(args[0], XmNinitialState, IconicState);
     XtSetValues(w->w, args, 1);
   }
   else
   {
     /* Remove the iconify hint from the current shell */
     wmhints = XGetWMHints(XtDisplay(w->w), XtWindow(w->w));
     wmhints->flags |= IconWindowHint;
     wmhints->initial_state = NormalState;
     XSetWMHints(XtDisplay(w->w), XtWindow(w->w), wmhints);
   }

   return(0);
}


int
do_DtWsmAddWorkspaceFunctions(
        int argc,
        char *argv[] )
{
   return(WsmCommonProc(argc, argv, (void (*)())DtWsmAddWorkspaceFunctions));
}


int
do_DtWsmRemoveWorkspaceFunctions(
        int argc,
        char *argv[] )
{
   return(WsmCommonProc(argc, argv, (void (*)())DtWsmRemoveWorkspaceFunctions));
}


static int
WsmCommonProc(
        int argc,
        char *argv[],
	void (*func)())
{
   wtab_t *w;
   Display * display;
   Window window;
   char * p;
   char buf[256];
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GetSharedMsg(DT_USAGE_DISPLAY_WINDOW));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL, NULL, NULL,
                NULL, NULL, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }
   window = (Window)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_WINDOW));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   (*func)(display, window);
   return (0);
}


int
do_DtWsmGetCurrentWorkspace(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   Display * display;
   Window rootWindow;
   char * p;
   Atom atom;
   char buf[256];
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GetSharedMsg(DT_USAGE_DISPLAY_ROOT_VAR));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      env_blank(argv[3]);
      return(1);
   }
   rootWindow = (Window)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GETMESSAGE(5,73, 
                      "The rootWindow parameter is invalid: %s"));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      env_blank(argv[3]);
      return(1);
   }

   DtWsmGetCurrentWorkspace(display, rootWindow, &atom);
   sprintf(buf, "%ld", (long)atom);
   env_set_var(argv[3], buf);
   return (0);
}


int
do_DtWsmSetCurrentWorkspace(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   char * p;
   Atom atom;
   Status result;
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,74, 
                      "Usage: DtWsmSetCurrentWorkspace widget atom"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   atom = (Atom)strtoul(argv[2], &p, 0);
   if (p == argv[2])
   {
      errmsg = strdup(GETMESSAGE(5,75, "The workspace atom is invalid: %s"));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   result = DtWsmSetCurrentWorkspace(w->w, atom);
   if (result == DT_SVC_SUCCESS)
      return (0);
   else
      return (1);
}


static int
GetWorkspaceList(
        char *usageMsg,
        Boolean getOccupied,
        int argc,
        char *argv[] )
{
   wtab_t *w;
   char * p;
   Display * display;
   Window root;
   unsigned long numWS;
   Atom * wsList;
   char * buf;
   char atom[128];
   int result;
   int i;
   char * errmsg;

   if (argc != 4) 
   {
      printerrf(str_nill, usageMsg, argv[0], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      env_blank(argv[3]);
      return(1);
   }
   root = (Window)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_WINDOW));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      env_blank(argv[3]);
      return(1);
   }

   if (getOccupied)
      result = DtWsmGetWorkspacesOccupied(display, root, &wsList, &numWS);
   else
      result = DtWsmGetWorkspaceList(display, root, &wsList, (int *)&numWS);

   if (result == Success)
   {
      buf = XtMalloc(1);
      buf[0] = '\0';

      for (i = 0; i < numWS; i++)
      {
         sprintf(atom, "%ld", (long)wsList[i]);
         buf = XtRealloc(buf, strlen(buf) + strlen(atom) + 2);
         if (i != 0)
            strcat(buf, ",");
         strcat(buf, atom);
      }

      env_set_var(argv[3], buf);
      XtFree(buf);
      XFree (wsList);
      return (0);
   }
   else
   {
      env_blank(argv[3]);
      return (1);
   }
}


int
do_DtWsmGetWorkspaceList(
        int argc,
        char *argv[] )
{
   char * errmsg;
   int retVal;

   errmsg = strdup(GetSharedMsg(DT_USAGE_DISPLAY_ROOT_VAR));
   retVal = GetWorkspaceList(errmsg, False, argc, argv);
   free(errmsg);
   return(retVal);
}


int
do_DtWsmGetWorkspacesOccupied(
        int argc,
        char *argv[] )
{
   char * errmsg;
   int retVal;

   errmsg = strdup(GetSharedMsg(DT_USAGE_DISPLAY_WINDOW_VAR));
   retVal = GetWorkspaceList(errmsg, True, argc, argv);
   free(errmsg);
   return(retVal);
}


int
do_DtWsmSetWorkspacesOccupied(
        int argc,
        char *argv[] )
{
   char * p;
   Display * display;
   Window window;
   unsigned long numWS;
   Atom * wsList;
   char * buf;
   int i;
   char * nextAtom;
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,78, 
             "Usage: DtWsmSetWorkspacesOccupied display window workspaceList"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }
   window = (Window)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_WINDOW));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   numWS = 0;
   p = argv[3];

   /* Strip leading spaces */
   while (*p == ' ')
      p++;

   nextAtom = strtok(p, ",");

   wsList = (Atom *)XtMalloc(1);
   wsList[0] = '\0';
   while (nextAtom)
   {
      if (strlen(nextAtom) > 0)
      {
         wsList = (Atom *)XtRealloc((char *)wsList, sizeof(Atom) * (numWS + 1));
         wsList[numWS] = atol(nextAtom);
         numWS++;
         nextAtom = strtok(NULL, ",");
      }
   }

   DtWsmSetWorkspacesOccupied(display, window, wsList, numWS);
   XtFree ((char *)wsList);

   return (0);
}


int
do_DtWsmGetCurrentBackdropWindow(
        int argc,
        char *argv[] )
{
   char * p;
   Display * display;
   Window rootWindow, returnedWin;
   char * errmsg;

   if (argc != 4) 
   {
      errmsg = strdup(GetSharedMsg(DT_USAGE_DISPLAY_ROOT_VAR));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[1], &p, 0);
   if (p == argv[1]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      env_blank(argv[3]);
      return(1);
   }
   rootWindow = (Window)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_WINDOW));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      env_blank(argv[3]);
      return(1);
   }

   returnedWin = DtWsmGetCurrentBackdropWindow(display, rootWindow);

   if (returnedWin != None)
   {
      char buf[128];

      sprintf(buf, "%d", (int)returnedWin);

      env_set_var(argv[3], buf);
      return (0);
   }
   else
   {
      env_blank(argv[3]);
      return (1);
   }
}


int
do_DtWsmOccupyAllWorkspaces(
        int argc,
        char *argv[] )
{
   return(WsmCommonProc(argc, argv, (void (*)())DtWsmOccupyAllWorkspaces));
}


int
do__DtGetHourGlassCursor(
        int argc,
        char *argv[] )
{
   char * p;
   Display * display;
   Cursor cursor;
   char buf[128];
   char * errmsg;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,79, 
                      "Usage: _DtGetHourGlassCursor variable display"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   display = (Display *)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_DISPLAY));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      alt_env_set_var(argv[1], str_nill);
      return(1);
   }

   cursor = _DtGetHourGlassCursor(display);
   sprintf(buf, "%d", cursor);
   alt_env_set_var(argv[1], buf);

   return (0);
}


static int
DtTurnOnOrOffHourGlass(
        void (*func)(),
        int argc,
        char *argv[] )
{
   char * p;
   Cursor cursor;
   char buf[128];
   wtab_t *w;
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GetSharedMsg(DT_USAGE_WIDGET));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL, NULL, NULL,
                NULL, NULL, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == NULL) 
      return(1);

   (*func)(w->w);
   return (0);
}


int
do__DtTurnOnHourGlass(
        int argc,
        char *argv[] )
{
  return(DtTurnOnOrOffHourGlass(_DtTurnOnHourGlass, argc, argv));
}


int
do__DtTurnOffHourGlass(
        int argc,
        char *argv[] )
{
   return(DtTurnOnOrOffHourGlass(_DtTurnOffHourGlass, argc, argv));
}

int
do_DtWsmAddCurrentWorkspaceCallback(
        int argc,
        char **argv )
{
   wtab_t *w;
   dtksh_client_data_t *cdata;
   char * p;
   Atom propAtom;
   DtWsmCBContext handle;
   char buf[128];
   char * errmsg;

   if (argc != 4)
   {
      errmsg = strdup(GETMESSAGE(5,80, 
        "Usage: DtWsmAddCurrentWorkspaceCallback variable widget ksh-command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   w = str_to_wtab(argv[0], argv[2]);
   if (w == NULL) 
   {
      alt_env_set_var(argv[1], str_nill);
      return(1);
   }

   /* Always force a new entry */
   cdata = GetNewCBData(NULL, NULL, NULL, None);
   cdata->w = w;
   cdata->ksh_cmd = strdup(argv[3]);

   handle = DtWsmAddCurrentWorkspaceCallback(w->w, 
		(DtWsmWsChangeProc)stdWSCB, (XtPointer)cdata);
   cdata->handle = (XtPointer)handle;

   sprintf(buf, "%ld", (long)handle);
   alt_env_set_var(argv[1], buf);

   return(0);
}

int
do_DtWsmRemoveWorkspaceCallback(
        int argc,
        char **argv )
{
   char * errmsg, *p;
   dtksh_client_data_t *cdata = (dtksh_client_data_t *)NULL;
   DtWsmCBContext handle;
   int i;

   if (argc != 2)
   {
      errmsg = strdup(GETMESSAGE(5,81, 
       "Usage: DtWsmRemoveWorkspaceCallback handle"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   handle = (DtWsmCBContext)strtoul(argv[1], &p, 0);
   if (p != argv[1])
   {
       for(i = 0; i < cbDataTableSize; i++)
       {
           if(cbDataTable[i] != (dtksh_client_data_t *)NULL)
           {
	       if(cbDataTable[i]->handle == handle)
	       {
	           cdata = cbDataTable[i];
	           break;
	       }
	    }
       }
   }

   if(cdata == (dtksh_client_data_t *)NULL)
   {
      errmsg = strdup(GETMESSAGE(5,20, 
                   "The following is an invalid callback handle: %s"));
      printerrf(argv[0], errmsg, argv[1], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   return(RemoveOneCallback (argv[0], cdata->w->widid, NULL, cdata->ksh_cmd,
			     NULL, argv[1]));
}


/*****************************************************************************/
/*****************************************************************************/


int
do_DtDbLoad(
        int argc,
        char *argv[] )
{
   char * errmsg;

   if (argc != 1) 
   {
      errmsg = strdup(GETMESSAGE(5,83, "Usage: DtDbLoad"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   DtDbLoad();
   return (0);
}


/* Only a single handler is allowed */
static char * reloadNotifyCommand = NULL;


/*
 * This is our internal 'ReloadNotify' callback; it simply interprets
 * the ksh command specified by the shell script.
 */
static void
DtkReloadHandler( 
	XtPointer clientData )
{
   ksh_eval((char *)reloadNotifyCommand);
}


/*
 * This command registers a ksh-command string, which will be executed
 * whenever a 'ReloadNotify' message is received.  Subsequent calls to
 * this command will simply replace the previous ksh-command witht the
 * new one.
 */
int
do_DtDbReloadNotify(
        int argc,
        char *argv[] )
{
   char * errmsg;
   static Boolean firstTime = True;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,84, "Usage: DtDbReloadNotify ksh-command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (firstTime)
   {
      firstTime = False;
      DtDbReloadNotify((DtDbReloadCallbackProc)DtkReloadHandler, NULL);
   }

   XtFree(reloadNotifyCommand);
   reloadNotifyCommand = strdup(argv[1]);
   return (0);
}


/*
 * This command is a boolean command, which returns 'True' if the
 * specified name correlates to a defined action.
 */
int
do_DtActionExists(
        int argc,
        char *argv[] )
{
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,85, "Usage: DtActionExists actionName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (DtActionExists(argv[1]))
      return(0);
   return(255);
}


/*
 * This command returns the label associated with an action.  If the
 * action is not defined, or if there is no label, then an empty string
 * is returned.
 */
int
do_DtActionLabel(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * label;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,86, 
                      "Usage: DtActionLabel variable actionName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (label = DtActionLabel(argv[2]))
   {
      alt_env_set_var(argv[1], label);
      XtFree(label);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


/*
 * This command returns the description associated with an action.  If the
 * action is not defined, or if there is no description, then an empty string
 * is returned.
 */
int
do_DtActionDescription(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * description;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,87, 
                      "Usage: DtActionDescription variable actionName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (description = DtActionDescription(argv[2]))
   {
      alt_env_set_var(argv[1], description);
      XtFree(description);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


static void
_DtActionInvokeUsage( void )
{
   char * errmsg;

   errmsg = strdup(GETMESSAGE(5,88, 
        "Usage: DtActionInvoke widget actionName termParms execHost contextDir useIndicator ksh-command [\"FILE\" fileName] ..."));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
}


/*
 * This command provides the shell script with the mechanism for requesting
 * that an action be invoked.  It accepts a variable list of arguments,
 * which can currently only be of type 'ARG_FILE'.
 */
int
do_DtActionInvoke(
        int argc,
        char *argv[] )
{
   wtab_t *w;
   char * termParms = (char *)NULL;
   char * execHost = (char *)NULL;
   char * contextDir = (char *)NULL;
   Boolean useIndicator;
   int aac;
   DtActionArg *aap = (DtActionArg *)NULL;
   XrmValue fval, tval;
   int i;
   int idx;
   int * lockedFds;

   /*
    * Either there must be no file args (so argc == 8), or if there are
    * file args, then there must be 2 components for each argument.
    * This check must change when/if we support arguments other than files.
    */
   if ((argc < 8) || (((argc - 8) % 2) != 0))
   {
      _DtActionInvokeUsage();
      return(1);
   }

   w = str_to_wtab(argv[0], argv[1]);
   if (w == (wtab_t *)NULL) 
      return(1);

   /* Get true/false value for useIndicator */
   fval.addr = argv[6];
   fval.size = strlen(argv[6]);
   XtConvert(Toplevel, XtRString, &fval, XtRBoolean, &tval);

   if (tval.size != 0) 
      useIndicator = *((Boolean *)(tval.addr));
   else
      return(1);

   if (argv[3] && (strlen(argv[3]) > 0))
      termParms = argv[3];

   if (argv[4] && (strlen(argv[4]) > 0))
      execHost = argv[4];

   if (argv[5] && (strlen(argv[5]) > 0))
      contextDir = argv[5];

   /*
    * Parse the optional file arguments
    * This will have to change when/if we support arguments other than files.
    */
   if ((aac = (argc - 8) / 2) > 0)
   {
      aap = (DtActionArg *)XtMalloc(sizeof(DtActionArg) * aac);
      for (i = 8; i < argc; i+=2 )
      {
	if(strcmp(argv[i], "FILE") != 0)
        {
           _DtActionInvokeUsage();
	   XtFree((char *)aap);
           return(1);
        }

        idx = (i - 8) / 2;
        aap[idx].argClass = DtACTION_FILE;
        aap[idx].u.file.name = argv[i + 1];
      }
   }

   /* Force fd above the range reserved by ksh for the user (0 - 9) */
   lockedFds = LockKshFileDescriptors();
   /*
    * Force callback to NULL until we have code in place to support it.
    */
   DtActionInvoke(w->w, argv[2], aap, aac, termParms, execHost, 
                  contextDir, useIndicator, NULL,
		  (XtPointer)NULL);
   UnlockKshFileDescriptors(lockedFds);

   XtFree((char *)aap);

   return (0);
}


/*****************************************************************************/
/*****************************************************************************/

int
do_DtDtsLoadDataTypes(
        int argc,
        char *argv[] )
{
   char * errmsg;

   if (argc != 1) 
   {
      errmsg = strdup(GETMESSAGE(5,89, "Usage: DtDtsLoadDataTypes"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   DtDtsLoadDataTypes();
   return (0);
}


int
do_DtDtsFileToDataType(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * datatype;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,90, 
                      "Usage: DtDtsFileToDataType variable fileName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (datatype = DtDtsFileToDataType(argv[2]))
   {
      alt_env_set_var(argv[1], datatype);
      DtDtsFreeDataType(datatype);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


int
do_DtDtsFileToAttributeValue(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * attribute;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,91,
              "Usage: DtDtsFileToAttributeValue variable fileName attrName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (attribute = DtDtsFileToAttributeValue(argv[2], argv[3]))
   {
      alt_env_set_var(argv[1], attribute);
      DtDtsFreeAttributeValue(attribute);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


int
do_DtDtsFileToAttributeList(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * attributeList;
   DtDtsAttribute ** attributes;
   int i;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,92,
              "Usage: DtDtsFileToAttributeList variable fileName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (attributes = DtDtsFileToAttributeList(argv[2]))
   {
      attributeList = XtMalloc(1);
      attributeList[0] = '\0';
      for (i = 0; attributes[i]; i++)
      {
         attributeList = XtRealloc(attributeList, 
                                   strlen(attributeList) + 
                                   strlen(attributes[i]->name) + 3);
         if (i != 0)
            strcat(attributeList, " ");
         strcat(attributeList, attributes[i]->name);
      }
      
      alt_env_set_var(argv[1], attributeList);
      DtDtsFreeAttributeList(attributes);
      XtFree(attributeList);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


int
do_DtDtsDataTypeToAttributeValue(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * attribute;
   char * optName;

   if ((argc != 4) && (argc != 5))
   {
      errmsg = strdup(GETMESSAGE(5,93,
   "Usage: DtDtsDataTypeToAttributeValue variable dataType attrName optName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if ((argc == 4) || (strlen(argv[4]) == 0))
      optName = NULL;
   else
      optName = argv[4];

   if (attribute = DtDtsDataTypeToAttributeValue(argv[2], argv[3], optName))
   {
      alt_env_set_var(argv[1], attribute);
      DtDtsFreeAttributeValue(attribute);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


int
do_DtDtsDataTypeToAttributeList(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * attributeList;
   DtDtsAttribute ** attributes;
   int i;
   char * optName;

   if ((argc != 3) && (argc != 4))
   {
      errmsg = strdup(GETMESSAGE(5,94,
        "Usage: DtDtsDataTypeToAttributeList variable dataType optName"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if ((argc == 3) || (strlen(argv[3]) == 0))
      optName = NULL;
   else
      optName = argv[3];

   if (attributes = DtDtsDataTypeToAttributeList(argv[2], optName))
   {
      attributeList = XtMalloc(1);
      attributeList[0] = '\0';
      for (i = 0; attributes[i]; i++)
      {
         attributeList = XtRealloc(attributeList, 
                                   strlen(attributeList) + 
                                   strlen(attributes[i]->name) + 3);
         if (i != 0)
            strcat(attributeList, " ");
         strcat(attributeList, attributes[i]->name);
      }
      
      alt_env_set_var(argv[1], attributeList);
      DtDtsFreeAttributeList(attributes);
      XtFree(attributeList);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


int
do_DtDtsFindAttribute(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * dataTypeList;
   char ** dataTypes;
   int i;

   if (argc != 4)
   {
      errmsg = strdup(GETMESSAGE(5,95,
                   "Usage: DtDtsFindAttribute variable name value"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (dataTypes = DtDtsFindAttribute(argv[2], argv[3]))
   {
      dataTypeList = XtMalloc(1);
      dataTypeList[0] = '\0';
      for (i = 0; dataTypes[i]; i++)
      {
         dataTypeList = XtRealloc(dataTypeList, 
                                   strlen(dataTypeList) + 
                                   strlen(dataTypes[i]) + 3);
         if (i != 0)
            strcat(dataTypeList, " ");
         strcat(dataTypeList, dataTypes[i]);
      }
      
      alt_env_set_var(argv[1], dataTypeList);
      DtDtsFreeDataTypeNames(dataTypes);
      XtFree(dataTypeList);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


int
do_DtDtsDataTypeNames(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * dataTypeList;
   char ** dataTypes;
   int i;

   if (argc != 2)
   {
      errmsg = strdup(GETMESSAGE(5,96, "Usage: DtDtsDataTypeNames variable"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (dataTypes = DtDtsDataTypeNames())
   {
      dataTypeList = XtMalloc(1);
      dataTypeList[0] = '\0';
      for (i = 0; dataTypes[i]; i++)
      {
         dataTypeList = XtRealloc(dataTypeList, 
                                   strlen(dataTypeList) + 
                                   strlen(dataTypes[i]) + 3);
         if (i != 0)
            strcat(dataTypeList, " ");
         strcat(dataTypeList, dataTypes[i]);
      }
      
      alt_env_set_var(argv[1], dataTypeList);
      DtDtsFreeDataTypeNames(dataTypes);
      XtFree(dataTypeList);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


int
do_DtDtsSetDataType(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * savedDataType;
   Boolean override;

   if (argc != 5)
   {
      errmsg = strdup(GETMESSAGE(5,97, 
       "Usage: DtDtsSetDataType variable fileName dataType override"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   /* Since XtInitialize may not have been called, can't use XtConvert */
   if (DtCompareISOLatin1(argv[4], "true"))
      override = True;
   else if (DtCompareISOLatin1(argv[4], "false"))
      override = False;
   else
      return(1);

   if (savedDataType = DtDtsSetDataType(argv[2], argv[3], override))
   {
      alt_env_set_var(argv[1], savedDataType);
      DtDtsFreeDataType(savedDataType);
   }
   else
      alt_env_set_var(argv[1], str_nill);
   return (0);
}


int
do_DtDtsDataTypeIsAction(
        int argc,
        char *argv[] )
{
   char * errmsg;

   if (argc != 2) 
   {
      errmsg = strdup(GETMESSAGE(5,98, 
                      "Usage: DtDtsDataTypeIsAction dataType"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   if (DtDtsDataTypeIsAction(argv[1]))
      return(0);
   return(255);
}


/*****************************************************************************/
/*****************************************************************************/

/*
 * This command will attempt to open a ToolTalk communications channel.
 */
int
do_ttdt_open(
        int argc,
        char *argv[] )
{
   char * errmsg;
   char * procId;
   Boolean sendStarted;
   XrmValue toVal;
   Cardinal nargs;
   char * statusString;
   char buf[25];
   int ttfd;
   int * lockedFds;
   Tt_status ttStatus;

   if (argc != 8) 
   {
      errmsg = strdup(GETMESSAGE(5,99, 
         "Usage: ttdt_open variable status variable2 toolname vendor version sendStarted"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   /*  Convert "true" or "false" to 1 or 0 */
   DtkshCvtStringToBool(argv[7], &toVal);

   if (toVal.size != 0) 
      sendStarted = *((Boolean *)(toVal.addr));
   else
      return(1);

   /* Force fd above the range reserved by ksh for the user (0 - 9) */
   lockedFds = LockKshFileDescriptors();
   procId = ttdt_open(&ttfd, argv[4], argv[5], argv[6], (int)sendStarted);
   UnlockKshFileDescriptors(lockedFds);

   /* Get the ttStatus and the asssociated string */
   ttStatus = tt_ptr_error(procId);
   DtkshCvtTtStatusToString(ttStatus, &toVal);

   if (toVal.size && toVal.addr)
      statusString = toVal.addr;
   else
      statusString = str_nill;
   env_set_var(argv[2], statusString);

   if (ttStatus == TT_OK)
      env_set_var(argv[1], procId);
   else
   {
      env_set_var(argv[1], str_nill);
      ttfd = -1;
   }

   sprintf(buf, "%d", ttfd);
   env_set_var(argv[3], buf);

   tt_free(procId);
   return (0);
}


/* 
 * This command is used to close a ToolTalk connection.
 */
int
do_ttdt_close(
        int argc,
        char *argv[] )
{
   char * errmsg;
   Boolean sendStopped;
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   char * procId;
   char * newProcId;

   if (argc != 5) 
   {
      errmsg = strdup(GETMESSAGE(5,103, 
         "Usage: ttdt_close status procId newProcId sendStopped"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[2]) == 0)
      procId = NULL;
   else
      procId = argv[2];

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[3]) == 0)
      newProcId = NULL;
   else
      newProcId = argv[3];

   DtkshCvtStringToBool(argv[4], &tval);

   if (tval.size != 0) 
      sendStopped = *((Boolean *)(tval.addr));
   else
      return(1);

   ttStatus = ttdt_close(procId, newProcId, (int)sendStopped);

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[1], statusString);

   return (0);
}


/*
 * This is the alternate input handler command for ToolTalk.  Shell scripts
 * will invoke it from their alternate input handlers, passing in all of
 * the required parameters.  This input handler will cause ToolTalk events
 * to be received and dispatched.
 */
int
do_tttk_Xt_input_handler(
        int argc,
        char *argv[] )
{
   char * errmsg;
   int source;
   XtInputId fid;
   char * p;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,100, 
         "Usage: tttk_Xt_input_handler procId source id"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   source = strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg=strdup(GETMESSAGE(5,101, 
                   "The source parameter must be an integer: %s"));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   fid = strtoul(argv[3], &p, 0);
   if (p == argv[3]) 
   {
      errmsg = strdup(GETMESSAGE(5,102, 
             "The id parameter must be a hex number: %s")); 
      printerrf(argv[0], errmsg, argv[3], NULL, NULL, NULL, NULL, NULL, 
                NULL, NULL);
      free(errmsg);
      return(1);
   }

   tttk_Xt_input_handler(argv[1], &source, &fid);
   return (0);
}


int
do_ttdt_session_join(
        int argc,
        char *argv[] )
{
   char * errmsg;
   Boolean join;
   char * sessId;
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   char buf[25];
   Widget widget;
   wtab_t *w;
   Tt_pattern * patterns;

   if (argc != 6) 
   {
      errmsg = strdup(GETMESSAGE(5,104, 
       "Usage: ttdt_session_join variable status sessId shellWidgetHandle join"
        ));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   DtkshCvtStringToBool(argv[5], &tval);

   if (tval.size != 0) 
      join = *((Boolean *)(tval.addr));
   else
      return(1);

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[3]) == 0)
      sessId = NULL;
   else
      sessId = argv[3];

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[4]) == 0)
      widget = NULL;
   else
   {
      w = str_to_wtab(argv[0], argv[4]);
      if (w == NULL) 
         return(1);
      widget = w->w;
   }

   patterns = ttdt_session_join(sessId, NULL, widget, NULL, (int)join);

   ttStatus = tt_ptr_error(patterns);

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[2], statusString);

   if (ttStatus == TT_OK)
   {
      sprintf(buf, "%ld", (long)patterns);
      env_set_var(argv[1], buf);
   }
   else
      env_set_var(argv[1], str_nill);

   return (0);
}


int
do_ttdt_session_quit(
        int argc,
        char *argv[] )
{
   char * errmsg;
   Boolean quit;
   char * sessId;
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   Tt_pattern * patterns;
   char * p;

   if (argc != 5) 
   {
      errmsg = strdup(GETMESSAGE(5,105, 
          "Usage: ttdt_session_quit status sessId sessPatterns quit"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   DtkshCvtStringToBool(argv[4], &tval);

   if (tval.size != 0) 
      quit = *((Boolean *)(tval.addr));
   else
      return(1);

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[2]) == 0)
      sessId = NULL;
   else
      sessId = argv[2];

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[3]) == 0)
      patterns = NULL;
   else
   {
      patterns = (Tt_pattern *)strtoul(argv[3], &p, 0);
      if (p == argv[3]) 
      {
         errmsg=strdup(GETMESSAGE(5,106, 
                   "The sessPatterns parameter is invalid: %s"));
         printerrf(argv[0], errmsg, argv[3], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
         free(errmsg);
         return(1);
      }
   }

   ttStatus = ttdt_session_quit(sessId, patterns, (int)quit);

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[1], statusString);

   return (0);
}


int
do_ttdt_file_event(
        int argc,
        char *argv[] )
{
   char * errmsg;
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   Boolean send;
   Tt_pattern * patterns;
   Tttk_op op;
   char * p;

   if (argc != 5) 
   {
      errmsg = strdup(GETMESSAGE(5,107, 
          "Usage: ttdt_file_event status op patterns send"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   fval.addr = argv[2];
   fval.size = strlen(argv[2]);
   XtConvert(Toplevel, XtRString, &fval, "TtOp", &tval); /* XXX */

   if (tval.size != 0) 
      op = *((Tttk_op *)(tval.addr));
   else
      return(1);

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[3]) == 0)
      patterns = NULL;
   else
   {
      patterns = (Tt_pattern *)strtoul(argv[3], &p, 0);
      if (p == argv[3]) 
      {
         errmsg = strdup(GetSharedMsg(DT_BAD_PATTERN));
         printerrf(argv[0], errmsg, argv[3], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
         free(errmsg);
         return(1);
      }
   }

   DtkshCvtStringToBool(argv[4], &tval);

   if (tval.size != 0) 
      send = *((Boolean *)(tval.addr));
   else
      return(1);


   ttStatus = ttdt_file_event(NULL, op, patterns, (int)send);

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[1], statusString);

   return (0);
}


/*
 * This is the internal callback invoked by tooltalk, whenever a message
 * is received for a file the user joined (using ttdt_file_join).  We
 * need to set up some environment variables, and then interpret the
 * ksh-cmd supplied by the shell script.
 */

static Tt_message
TtFileCB(
        Tt_message msg,
	Tttk_op op,
	char * pathName,
	void * clientData,
	int sameEuidEgid,
	int sameProcId )
{
   Ttdt_file_cb_data *cdata = (Ttdt_file_cb_data *)clientData;
   int results;
   char strBuf[25];
   Namval_t * msgVar;
   Namval_t * opVar;
   Namval_t * pathVar;
   Namval_t * sameProcVar;
   Namval_t * sameEuidVar;
   XrmValue fval, tval;
   int ttmark = tt_mark();;

   /* Initialize the environment variables */
   msgVar = nv_search("DT_TT_MSG", sh.var_tree, NV_ADD);
   sprintf(strBuf, "%ld", (long)msg);
   nv_putval(msgVar, strBuf, NV_RDONLY);

   opVar = nv_search("DT_TT_OP", sh.var_tree, NV_ADD);
   fval.addr = (caddr_t)&op;
   fval.size = sizeof(op);
   XtConvert(Toplevel, "TtOp", &fval, XtRString, &tval); /* XXX?? */
   if (tval.size && tval.addr)
      nv_putval(opVar, (char *)tval.addr, NV_RDONLY);
   else
      nv_putval(opVar, str_nill, NV_RDONLY);

   pathVar = nv_search("DT_TT_PATHNAME", sh.var_tree, NV_ADD);
   nv_putval(pathVar, pathName, NV_RDONLY);

   sameProcVar = nv_search("DT_TT_SAME_PROCID", sh.var_tree, NV_ADD);
   if (sameProcId)
      nv_putval(sameProcVar, "True", NV_RDONLY);
   else
      nv_putval(sameProcVar, "False", NV_RDONLY);
   sameEuidVar = nv_search("DT_TT_SAME_EUID_EGID", sh.var_tree, NV_ADD);
   if (sameProcId)
      nv_putval(sameEuidVar, "True", NV_RDONLY);
   else
      nv_putval(sameEuidVar, "False", NV_RDONLY);

   /* Interpret the registered command */
   results = ksh_eval((char *)cdata->ksh_cmd);

   /* Clean up the environment variables */
   nv_newattr(msgVar, 0, 0);
   nv_close(msgVar);
   nv_newattr(opVar, 0, 0);
   nv_close(opVar);
   nv_newattr(pathVar, 0, 0);
   nv_close(pathVar);
   nv_newattr(sameProcVar, 0, 0);
   nv_close(sameProcVar);
   nv_newattr(sameEuidVar, 0, 0);
   nv_close(sameEuidVar);

   tt_release(ttmark);
   return((Tt_message)results);
}


int
do_ttdt_file_join(
        int argc,
        char *argv[] )
{
   char * errmsg;
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   Boolean join;
   Tt_scope scope;
   Tt_pattern * patterns;
   Ttdt_file_cb_data * cData;
   char buf[30];
   int i, j;

   if (argc != 7) 
   {
      errmsg = strdup(GETMESSAGE(5,115,
      "Usage: ttdt_file_join variable status pathName scope join ksh-command"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   DtkshCvtStringToBool(argv[5], &tval);

   if (tval.size != 0) 
      join = *((Boolean *)(tval.addr));
   else
      return(1);

   fval.addr = argv[4];
   fval.size = strlen(argv[4]);
   XtConvert(Toplevel, XtRString, &fval, "TtScope", &tval); /* XXX ??? */

   if (tval.size != 0) 
      scope = *((Tt_scope *)(tval.addr));
   else
      return(1);

   cData = (Ttdt_file_cb_data *)XtMalloc(sizeof(Ttdt_file_cb_data));

   patterns = ttdt_file_join(argv[3], scope, (int)join, TtFileCB, cData);
   ttStatus = tt_ptr_error(patterns);

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[2], statusString);

   /* 
    * If the request failed, then no callback was added, so we can free
    * up the client data; otherwise, we need to finish filling in the
    * client data, and then saving it in our storage array, so that we
    * can later free it when ttdt_file_quit() is called.
    */
   if (ttStatus == TT_OK)
   {
      sprintf(buf, "%ld", (long)patterns);
      env_set_var(argv[1], buf);
      cData->ksh_cmd = strdup(argv[6]);
      cData->patterns = patterns;

      /* Add clientData to our storage array */
      for (i = 0; i < sizeFileCBList; i++)
      {
         if (fileCBList[i] == NULL)
            break;
      }
      if (i >= sizeFileCBList)
      {
        /* Grow the array */
        sizeFileCBList += 10;
        fileCBList = (Ttdt_file_cb_data **)XtRealloc((char *)fileCBList,
                                sizeof(Ttdt_file_cb_data *) * sizeFileCBList);
        for (j = i; j < sizeFileCBList; j++)
           fileCBList[j] = NULL;
      }
      fileCBList[i] = cData;
   }
   else
   {
      XtFree((char *)cData);
      env_set_var(argv[1], str_nill);
   }

   return (0);
}


int
do_ttdt_file_quit(
        int argc,
        char *argv[] )
{
   char * errmsg;
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   Boolean quit;
   Tt_pattern * patterns;
   char * p;
   int i;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,108, 
          "Usage: ttdt_file_quit status patterns quit"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[2]) == 0)
      patterns = NULL;
   else
   {
      patterns = (Tt_pattern *)strtoul(argv[2], &p, 0);
      if (p == argv[2]) 
      {
         errmsg = strdup(GetSharedMsg(DT_BAD_PATTERN));
         printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
                NULL, NULL);
         free(errmsg);
         return(1);
      }
   }

   DtkshCvtStringToBool(argv[3], &tval);

   if (tval.size != 0) 
      quit = *((Boolean *)(tval.addr));
   else
      return(1);

   ttStatus = ttdt_file_quit(patterns, (int)quit);

   /* Remove this entry from our list of file callbacks */
   for (i = 0; i < sizeFileCBList; i++)
   {
      if (fileCBList[i] && (fileCBList[i]->patterns == patterns))
      {
         XtFree(fileCBList[i]->ksh_cmd);
         XtFree((char *)fileCBList[i]);
         fileCBList[i] = NULL;
         break;
      }
   }

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[1], statusString);

   return (0);
}


int
do_ttdt_Get_Modified(
        int argc,
        char *argv[] )
{
   char * errmsg;
   XrmValue fval, tval;
   Tt_scope scope;
   int timeout;
   char * p;

   if (argc != 4) 
   {
      errmsg = strdup(GETMESSAGE(5,109, 
          "Usage: ttdt_Get_Modified pathName scope timeout"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   fval.addr = argv[2];
   fval.size = strlen(argv[2]);
   XtConvert(Toplevel, XtRString, &fval, "TtScope", &tval); /* XXX ?? */

   if (tval.size != 0) 
      scope = *((Tt_scope *)(tval.addr));
   else
      return(1);

   timeout = strtoul(argv[3], &p, 0);
   if (p == argv[3]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_TIMEOUT));
      printerrf(argv[0], errmsg, argv[3], NULL, NULL, NULL, NULL, NULL,
             NULL, NULL);
      free(errmsg);
      return(1);
   }

   if (ttdt_Get_Modified(NULL, argv[1], scope, 
       XtWidgetToApplicationContext(Toplevel), timeout))
   {
      return(0);
   }
   return(255);
}


/*
 * Common function for ttdt_Save and ttdt_Revert commands.
 */
static int
ttdt_SaveOrRevert(
        Tt_status (*func)(),
        int argc,
        char *argv[] )
{
   char * errmsg;
   XrmValue fval, tval;
   Tt_scope scope;
   int timeout;
   char * p;
   Tt_status ttStatus;
   char * statusString;

   if (argc != 5) 
   {
      errmsg = strdup(GETMESSAGE(5,110, 
                      "Usage: %s status pathName scope timeout"));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL, NULL, NULL, NULL,
             NULL, NULL);
      free(errmsg);
      return(1);
   }

   fval.addr = argv[3];
   fval.size = strlen(argv[3]);
   XtConvert(Toplevel, XtRString, &fval, "TtScope", &tval); /* XXX ?? */

   if (tval.size != 0) 
      scope = *((Tt_scope *)(tval.addr));
   else
      return(1);

   timeout = strtoul(argv[4], &p, 0);
   if (p == argv[4]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_TIMEOUT));
      printerrf(argv[0], errmsg, argv[4], NULL, NULL, NULL, NULL, NULL,
             NULL, NULL);
      free(errmsg);
      return(1);
   }

   ttStatus = (*func)(NULL, argv[2], scope, 
                      XtWidgetToApplicationContext(Toplevel), timeout);

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[1], statusString);

   return(0);
}


int
do_ttdt_Save(
        int argc,
        char *argv[] )
{
   return(ttdt_SaveOrRevert(ttdt_Save, argc, argv ));
}


int
do_ttdt_Revert(
        int argc,
        char *argv[] )
{
   return(ttdt_SaveOrRevert(ttdt_Revert, argc, argv ));
}


int
do_tt_error_pointer(
        int argc,
        char *argv[] )
{
   char * errmsg;
   XrmValue fval, tval;
   Tt_status ttStatus;
   void * errPtr;
   char buf[25];

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,112,
          "Usage: tt_error_pointer variable ttStatus"));
      printerr(str_nill, errmsg, NULL);
      free(errmsg);
      return(1);
   }

   /* Map the string into a ttStatus */
   DtkshCvtStringToTtStatus(argv[2], &tval);

   if (tval.size != 0) 
      ttStatus = *((Tt_status *)(tval.addr));
   else
      return(1);

   errPtr = tt_error_pointer(ttStatus);
   sprintf(buf, "%ld", (long)errPtr);
   alt_env_set_var(argv[1], buf);

   return (0);
}


static int
message_DestroyOrReply( 
        Tt_status (*func)(),
        int argc,
        char *argv[] )
{
   char * errmsg;
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   Tt_message message;
   char * p;

   if (argc != 3) 
   {
      errmsg = strdup(GETMESSAGE(5,113, "Usage: %s status msg"));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL, NULL, NULL, NULL,
             NULL, NULL);
      free(errmsg);
      return(1);
   }

   message = (Tt_message)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_MESSAGE));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
             NULL, NULL);
      free(errmsg);
      return(1);
   }

   ttStatus = (*func)(message);

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[1], statusString);

   return (0);
}


int
do_tttk_message_destroy(
        int argc,
        char *argv[] )
{
   return(message_DestroyOrReply(tttk_message_destroy, argc, argv));
}


int
do_tt_message_reply(
        int argc,
        char *argv[] )
{
   return(message_DestroyOrReply(tt_message_reply, argc, argv));
}


static int
message_FailOrReject( 
        Tt_status (*func)(),
        int argc,
        char *argv[] )
{
   char * errmsg;
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   Tt_message message;
   char * msgStatusString;
   Boolean destroy;
   char * p;

   if (argc != 6) 
   {
      errmsg = strdup(GETMESSAGE(5,114,
                 "Usage: %s status msg msgStatus msgStatusString destroy"));
      printerrf(str_nill, errmsg, argv[0], NULL, NULL, NULL, NULL, NULL,
             NULL, NULL);
      free(errmsg);
      return(1);
   }

   message = (Tt_message)strtoul(argv[2], &p, 0);
   if (p == argv[2]) 
   {
      errmsg = strdup(GetSharedMsg(DT_BAD_MESSAGE));
      printerrf(argv[0], errmsg, argv[2], NULL, NULL, NULL, NULL, NULL,
             NULL, NULL);
      free(errmsg);
      return(1);
   }

   /* Map the string into a ttStatus */
   DtkshCvtStringToTtStatus(argv[3], &tval);

   if (tval.size != 0) 
      ttStatus = *((Tt_status *)(tval.addr));
   else
      return(1);

   /* Need to treat "" equal to a NULL pointer here */
   if (strlen(argv[4]) == 0)
      msgStatusString = NULL;
   else
      msgStatusString = argv[4];

   /* Convert the boolean value */
   DtkshCvtStringToBool(argv[5], &tval);

   if (tval.size != 0) 
      destroy = *((Boolean *)(tval.addr));
   else
      return(1);

   ttStatus = (*func)(message, ttStatus, msgStatusString, destroy);

   /* Map the ttStatus into a string */
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[1], statusString);

   return (0);
}


int
do_tttk_message_reject(
        int argc,
        char *argv[] )
{
   return(message_FailOrReject(tttk_message_reject, argc, argv));
}


int
do_tttk_message_fail(
        int argc,
        char *argv[] )
{
   return(message_FailOrReject(tttk_message_fail, argc, argv));
}


static int
tt_netfile_handler( 
        int paramCount,
        char * (*func)(),
        char * usageMsg,
        int argc,
        char *argv[] )
{
   XrmValue fval, tval;
   Tt_status ttStatus;
   char * statusString;
   char * convertedName;
   Cardinal cargc;
   XrmValue cargv[1];

   if (argc != paramCount) 
   {
      printerr(str_nill, usageMsg, NULL);
      return(1);
   }

   if (paramCount == 4)
      convertedName = (char *)(*func)(argv[3]);
   else
      convertedName = (char *)(*func)(argv[3], argv[4]);

   /* 
    * Map the ttStatus into a string.  Note that we can't call the XtConvert
    * function, since a shell script may not have called XtInitialize.
    */
   ttStatus = tt_ptr_error(convertedName);
   DtkshCvtTtStatusToString(ttStatus, &tval);

   if (ttStatus == TT_OK)
      env_set_var(argv[1], convertedName);
   else
      env_set_var(argv[1], str_nill);

   if (tval.size && tval.addr)
      statusString = tval.addr;
   else
      statusString = str_nill;
   env_set_var(argv[2], statusString);

   tt_free(convertedName);
   return (0);
}


int
do_tt_file_netfile(
        int argc,
        char *argv[] )
{
   char * usageMsg;
   int results;

   usageMsg = strdup(GETMESSAGE(5,116,
             "Usage: tt_file_netfile variable status filename"));
   results = tt_netfile_handler(4, tt_file_netfile, usageMsg, argc, argv);
   XtFree(usageMsg);
   return(results);
}


int
do_tt_netfile_file(
        int argc,
        char *argv[] )
{
   char * usageMsg;
   int results;

   usageMsg = strdup(GETMESSAGE(5,117,
             "Usage: tt_netfile_file variable status netfilename"));
   results = tt_netfile_handler(4, tt_netfile_file, usageMsg, argc, argv);
   XtFree(usageMsg);
   return(results);
}


int
do_tt_host_file_netfile(
        int argc,
        char *argv[] )
{
   char * usageMsg;
   int results;

   usageMsg = strdup(GETMESSAGE(5,118,
             "Usage: tt_host_file_netfile variable status host filename"));
   results = tt_netfile_handler(5, tt_host_file_netfile, usageMsg, argc, argv);
   XtFree(usageMsg);
   return(results);
}


int
do_tt_host_netfile_file(
        int argc,
        char *argv[] )
{
   char * usageMsg;
   int results;

   usageMsg = strdup(GETMESSAGE(5,119,
             "Usage: tt_host_netfile_file variable status host netfilename"));
   results = tt_netfile_handler(5, tt_host_netfile_file, usageMsg, argc, argv);
   XtFree(usageMsg);
   return(results);
}



/*****************************************************************************/
/*****************************************************************************/


/*
 * Starting with the class of the widget, check to see if it defines the
 * indicated callback; if not, then keep checking its superclasses.
 */
static Namdisc_t *
CheckClassDisciplines(
        WidgetClass class,
        char *cbname )
{
   int i = 0;
   int j = 0;

   while (C[i].cname)
   {
      if (C[i].class == class)
      {
         if (C[i].disciplines)
         {
            while(C[i].disciplines[j].callbackName)
            {
               if (strcmp(cbname, C[i].disciplines[j].callbackName) == 0)
                  return(C[i].disciplines[j].discipline);

               j++;
            }
         }
      }
      i++;
   }
   return(NULL);
}

/*****************************************************************************/
/*****************************************************************************/

/*
 * The following collection of functions deal with handling the dynamic
 * setting of an environment variable, when referenced by the shell script.
 * All of the environment variables are based off of the CB_CALL_DATA
 * environment variable, which is set before the shell script's callback
 * is invoked.  After the shell script's callback returns, any dynamically
 * created environment variable are removed; thus, the scope is only
 * within the context of the callback.
 *
 * If the shell script attempts to reference a subfield of the CB_CALL_DATA,
 * and if the parent has not yet been reference, then the 'name' passed to
 * the discipline function will contain all of the previously unreferenced
 * portions of the environment variable name.  As an example, if the shell
 * script referenced ${CB_CALL_DATA.EVENT.TYPE} , and the "EVENT" portion
 * has not yet been referenced, then the incoming name will be "EVENT.TYPE".
 * This is why all of the discipline functions below use "strtok()"; this
 * allows us to break up the name into each token, and to then initialize
 * the token.  Any unrecognized tokens are set to the string "".  In the
 * above example, a new name/value pair will be created for both the
 * "EVENT" and the "TYPE" portions, and the returned name/value pair will
 * be for the "TYPE" portion, since it was the terminal portion of the
 * reference.
 */


static struct named_integer CallbackReasons[] = {
	{ "CR_NONE",		XmCR_NONE },
	{ "CR_HELP",		XmCR_HELP },
	{ "CR_VALUE_CHANGED",	XmCR_VALUE_CHANGED },
	{ "CR_INCREMENT",	XmCR_INCREMENT },
	{ "CR_DECREMENT",	XmCR_DECREMENT },
	{ "CR_PAGE_INCREMENT",	XmCR_PAGE_INCREMENT },
	{ "CR_PAGE_DECREMENT",	XmCR_PAGE_DECREMENT },
	{ "CR_TO_TOP",		XmCR_TO_TOP },
	{ "CR_TO_BOTTOM",	XmCR_TO_BOTTOM },
	{ "CR_DRAG",		XmCR_DRAG },
	{ "CR_ACTIVATE",	XmCR_ACTIVATE },
	{ "CR_ARM",		XmCR_ARM },
	{ "CR_DISARM",		XmCR_DISARM },
	{ "CR_MAP",		XmCR_MAP },
	{ "CR_UNMAP",		XmCR_UNMAP },
	{ "CR_FOCUS",		XmCR_FOCUS },
	{ "CR_LOSING_FOCUS",	XmCR_LOSING_FOCUS },
	{ "CR_MODIFYING_TEXT_VALUE",	XmCR_MODIFYING_TEXT_VALUE },
	{ "CR_MOVING_INSERT_CURSOR",	XmCR_MOVING_INSERT_CURSOR },
	{ "CR_EXECUTE",		XmCR_EXECUTE },
	{ "CR_SINGLE_SELECT",	XmCR_SINGLE_SELECT },
	{ "CR_MULTIPLE_SELECT",	XmCR_MULTIPLE_SELECT },
	{ "CR_EXTENDED_SELECT",	XmCR_EXTENDED_SELECT },
	{ "CR_BROWSE_SELECT",	XmCR_BROWSE_SELECT },
	{ "CR_DEFAULT_ACTION",	XmCR_DEFAULT_ACTION },
	{ "CR_CLIPBOARD_DATA_REQUEST",	XmCR_CLIPBOARD_DATA_REQUEST },
	{ "CR_CLIPBOARD_DATA_DELETE",	XmCR_CLIPBOARD_DATA_DELETE },
	{ "CR_CASCADING",	XmCR_CASCADING },
	{ "CR_OK",		XmCR_OK },
	{ "CR_CANCEL",		XmCR_CANCEL },
	{ "CR_APPLY",		XmCR_APPLY },
	{ "CR_NO_MATCH",	XmCR_NO_MATCH },
	{ "CR_COMMAND_ENTERED",	XmCR_COMMAND_ENTERED },
	{ "CR_COMMAND_CHANGED",	XmCR_COMMAND_CHANGED },
	{ "CR_EXPOSE",		XmCR_EXPOSE },
	{ "CR_RESIZE",		XmCR_RESIZE },
	{ "CR_INPUT",		XmCR_INPUT },
	{ "CR_GAIN_PRIMARY",	XmCR_GAIN_PRIMARY },
	{ "CR_LOSE_PRIMARY",	XmCR_LOSE_PRIMARY },
	{ "CR_CREATE",		XmCR_CREATE },
	{ "CR_TEAR_OFF_ACTIVATE",	XmCR_TEAR_OFF_ACTIVATE },
	{ "CR_TEAR_OFF_DEACTIVATE",	XmCR_TEAR_OFF_DEACTIVATE },
	{ "CR_OBSCURED_TRAVERSAL",	XmCR_OBSCURED_TRAVERSAL },
	{ "CR_PROTOCOLS",	6666 },
	{ NULL,				0 },
};


static struct named_integer HelpCallbackReasons[] = {
	{ "HELP_CR_CLOSE",		DtCR_HELP_CLOSE },
	{ "HELP_CR_LINK_ACTIVATE",	DtCR_HELP_LINK_ACTIVATE },
	{ NULL,				0 },
};


/*
 * Create a new name/value pair (if necessary), and add it to the list of
 * name/value pairs which must be cleaned up when we are done.
 */
Namval_t *
GetNameValuePair(
        char *name )
{
   Namval_t * np2;
   Namval_t** list;
   int i;

   if (((np2 = nv_search(name, sh.var_tree, 0)) == NULL) ||
        (nestingLevel == 0))
   {
      /* Add to the list only the first time referenced */
      if (nestingLevel + 1 > npTableSize)
      {
         npTable = (Namval_t ***)XtRealloc((char *)npTable,
                                     sizeof(Namval_t **) * (nestingLevel+1));
         npListSizes = (int *)XtRealloc((char *)npListSizes,
                                     sizeof(int) * (nestingLevel+1));
         for (i = npTableSize; i < (nestingLevel + 1); i++)
         {
            npTable[i] = NULL;
            npListSizes[i] = 0;
         }
         npTableSize = nestingLevel + 1;
      }

      np2 = nv_search(name, sh.var_tree, NV_ADD);
      (npListSizes[nestingLevel])++;
      list = npTable[nestingLevel] = (Namval_t **)XtRealloc(
            (char *)npTable[nestingLevel], 
            sizeof(Namval_t *) * npListSizes[nestingLevel]);
      list[npListSizes[nestingLevel] - 1] = np2;
      /*
       * I _think_ this works OK, because I _think_ the subshell code will
       * automatically clean up the "extra" Namval_t it might create here.
       * As long as we clean up the original, I don't think we leak here.
       */
      if(sh.subshell)
         np2 = sh_assignok(np2, 1);
   }
   return(np2);
}


/*
 * Free only those environment variables created at this nesting level.
 */
void
FreeNestedVariables( void )
{
   Namval_t** list;
   int i;

   if ((nestingLevel < 0) || (nestingLevel >= npTableSize))
      return;

   list = npTable[nestingLevel];

   for (i = 0; i < npListSizes[nestingLevel]; i++)
   {
      nv_newattr(list[i], 0, 0); 
      nv_stack(list[i], NULL);
      nv_close(list[i]);
   }

   XtFree((char *)list);
   npTable[nestingLevel] = NULL;
   npListSizes[nestingLevel] = 0;
}


/* 
 * Create an empty name/value pair.  
 * THIS FUNCTION ASSUMES THAT THE CALLER HAS DONE THE INITIAL strtok()
 * CALL, SO THAT WE CAN DO THE REMAINING ONES, TO INITIALIZE ALL REMAINING
 * TOKENS.
 */
static Namval_t *
CreateEmptyNameValuePair(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];

   np2 = GetNameValuePair(name);
   buf[0] = '\0';
   nv_putval(np2, buf, NV_RDONLY);

   if (name = strtok(NULL, "."))
      return(CreateEmptyNameValuePair(np, name, fp));
   else
      return(np2);
}


static Namval_t *
ProcessIntValue(
        int value,
        Namval_t *np,
        char *name,
        Namfun_t *fp,
        char *format,
        Namfun_t *fp_new) 
{
   Namval_t * np2;
   char buf[128];

   np2 = GetNameValuePair(name);
   sprintf(buf, format, value);

   nv_stack(np2, NULL);
   nv_putval(np2, buf, NV_RDONLY);
   if (fp_new)
      nv_stack(np2, fp_new);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessStringValue(
        char *value,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;

   np2 = GetNameValuePair(name);
   if (value)
      nv_putval(np2, value, NV_RDONLY);
   else
      nv_putval(np2, str_nill, NV_RDONLY);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessBooleanIntValue(
        int value,
        Namval_t *np,
        char *name,
        Namfun_t *fp,
        Namfun_t *fp_new )
{
   Namval_t * np2;
   char buf[128];

   np2 = GetNameValuePair(name);
   if (value)
      strcpy(buf, "true");
   else
      strcpy(buf, "false");

   /* 
    * Any old disciplies MUST be cleared, before setting value.  If this
    * is not done, then excessive looping occurs, and the value will not
    * be correct, the next time you retrieve it.
    */
   nv_stack(np2, NULL);
   nv_putval(np2, buf, NV_RDONLY);
   if (fp_new)
      nv_stack(np2, fp_new);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessTraversalDirection(
        XmTraversalDirection dir,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   XrmValue f, t;
   char * value;

   np2 = GetNameValuePair(name);

   f.addr = (caddr_t)&dir;
   f.size = sizeof(XmTraversalDirection);
   t.addr = NULL;
   t.size = 0;
   XtConvert(Toplevel, "TraversalDirection", &f, XtRString, &t);

   if (t.size && t.addr)
      value = t.addr;
   else
      value = str_nill;

   strcpy(buf, value);
   nv_putval(np2, buf, NV_RDONLY);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessSelectionType(
        char selType,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   XrmValue f, t;
   char * value;
   int tmpSelType = (int)selType;

   np2 = GetNameValuePair(name);

   f.addr = (caddr_t)&tmpSelType;
   f.size = sizeof(int);
   t.addr = NULL;
   t.size = 0;
   XtConvert(Toplevel, "ListSelectionType", &f, XtRString, &t);

   if (t.size && t.addr)
      value = t.addr;
   else
      value = str_nill;

   strcpy(buf, value);
   nv_putval(np2, buf, NV_RDONLY);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessIntTable(
        int *table,
        int count,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char * buf;
   char buf2[25];
   int i;

   buf = XtMalloc(1);
   buf[0] = '\0';

   np2 = GetNameValuePair(name);
   if (count > 0)
   {
      for (i = 0; i < count; i++)
      {
         sprintf(buf2, "%d", table[i]);
         buf = XtRealloc(buf, strlen(buf) + strlen(buf2) + (i == 0 ? 1 : 2));
         if (i != 0)
            strcat(buf, ",");
         strcat(buf, buf2);
      }
   }

   nv_putval(np2, buf, NV_RDONLY);
   XtFree (buf);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessXmStringTable(
        XmString *table,
        int count,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char * buf;
   int i;

   np2 = GetNameValuePair(name);
   buf = _CvtXmStringTableToString(table, count);
   nv_putval(np2, buf, NV_RDONLY);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessWidgetHandle(
        Widget handle,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   wtab_t * w;

   np2 = GetNameValuePair(name);
   w = widget_to_wtab(handle);
   strcpy(buf, w ? w->widid : "Unknown");
   nv_putval(np2, buf, NV_RDONLY);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessXmStringValue(
        XmString xmstring,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   wtab_t * w;
   char * value;

   np2 = GetNameValuePair(name);

   if ((value = XmStringToString(xmstring)) == NULL)
      value = str_nill;

   nv_putval(np2, value, NV_RDONLY);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static Namval_t *
ProcessHyperType(
        int hyperType,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   wtab_t * w;
   XrmValue f, t;
   char * value;

   np2 = GetNameValuePair(name);

   f.addr = (caddr_t)&hyperType;
   f.size = sizeof(long);
   t.addr = NULL;
   t.size = 0;
   XtConvert(Toplevel, "HelpHyperType", &f, XtRString, &t);

   if (t.addr)
      value = t.addr;
   else
      value = str_nill;

   strcpy(buf, value);
   nv_putval(np2, buf, NV_RDONLY);

   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);

   return(np2);
}


static XEventTable eventTable[] = {
	{"XANY", NULL},
	{"XBUTTON", NULL},
	{"XEXPOSE", NULL},
	{"XNOEXPOSE", NULL},
	{"XGRAPHICSEXPOSE", NULL},
	{"XKEY", NULL},
	{"XMOTION", NULL},
	{NULL, NULL},
};

static EventEntryTable xanyTable[] = {
   {"TYPE", "XE_EventType", XtOffsetOf(XEvent, xany.type), sizeof(int)},
   {"SERIAL", "XE_IntValue", XtOffsetOf(XEvent, xany.serial), 
     sizeof(unsigned long)},
   {"SEND_EVENT", "X_Bool", XtOffsetOf(XEvent, xany.send_event),
     sizeof(Boolean)},
   {"DISPLAY", "XE_HexValue", XtOffsetOf(XEvent, xany.display),
     sizeof(Display *)},
   {"WINDOW", "XE_Window", XtOffsetOf(XEvent, xany.window),
     sizeof(Window)},
   {NULL, NULL, 0, 0},
};

static EventEntryTable xbuttonTable[] = {
   {"TYPE", "XE_EventType", XtOffsetOf(XEvent, xbutton.type), sizeof(int)},
   {"SERIAL", "XE_IntValue", XtOffsetOf(XEvent, xbutton.serial), 
     sizeof(unsigned long)},
   {"SEND_EVENT", "X_Bool", XtOffsetOf(XEvent, xbutton.send_event),
     sizeof(Bool)},
   {"DISPLAY", "XE_HexValue", XtOffsetOf(XEvent, xbutton.display),
     sizeof(Display *)},
   {"WINDOW", "XE_Window", XtOffsetOf(XEvent, xbutton.window),
     sizeof(Window)},
   {"ROOT", "XE_Window", XtOffsetOf(XEvent, xbutton.root),
     sizeof(Window)},
   {"SUBWINDOW", "XE_Window", XtOffsetOf(XEvent, xbutton.subwindow),
     sizeof(Window)},
   {"TIME", "XE_IntValue", XtOffsetOf(XEvent, xbutton.time),
     sizeof(unsigned int)},
   {"X", "XE_IntValue", XtOffsetOf(XEvent, xbutton.x),
     sizeof(int)},
   {"Y", "XE_IntValue", XtOffsetOf(XEvent, xbutton.y),
     sizeof(int)},
   {"X_ROOT", "XE_IntValue", XtOffsetOf(XEvent, xbutton.x_root),
     sizeof(int)},
   {"Y_ROOT", "XE_IntValue", XtOffsetOf(XEvent, xbutton.y_root),
     sizeof(int)},
   {"STATE", "XE_ModifierState", XtOffsetOf(XEvent, xbutton.state),
     sizeof(unsigned int)},
   {"BUTTON", "XE_Button", XtOffsetOf(XEvent, xbutton.button),
     sizeof(unsigned int)},
   {"SAME_SCREEN", "X_Bool", XtOffsetOf(XEvent, xbutton.same_screen),
     sizeof(Bool)},
   {NULL, NULL, 0, 0},
};

static EventEntryTable xexposeTable[] = {
   {"TYPE", "XE_EventType", XtOffsetOf(XEvent, xexpose.type), sizeof(int)},
   {"SERIAL", "XE_IntValue", XtOffsetOf(XEvent, xexpose.serial), 
     sizeof(unsigned long)},
   {"SEND_EVENT", "X_Bool", XtOffsetOf(XEvent, xexpose.send_event),
     sizeof(Bool)},
   {"DISPLAY", "XE_HexValue", XtOffsetOf(XEvent, xexpose.display),
     sizeof(Display *)},
   {"WINDOW", "XE_Window", XtOffsetOf(XEvent, xexpose.window),
     sizeof(Window)},
   {"X", "XE_IntValue", XtOffsetOf(XEvent, xexpose.x),
     sizeof(int)},
   {"Y", "XE_IntValue", XtOffsetOf(XEvent, xexpose.y),
     sizeof(int)},
   {"WIDTH", "XE_IntValue", XtOffsetOf(XEvent, xexpose.width),
     sizeof(int)},
   {"HEIGHT", "XE_IntValue", XtOffsetOf(XEvent, xexpose.height),
     sizeof(int)},
   {"COUNT", "XE_IntValue", XtOffsetOf(XEvent, xexpose.count),
     sizeof(int)},
   {NULL, NULL, 0, 0},
};

static EventEntryTable xnoExposeTable[] = {
   {"TYPE", "XE_EventType", XtOffsetOf(XEvent, xnoexpose.type), sizeof(int)},
   {"SERIAL", "XE_IntValue", XtOffsetOf(XEvent, xnoexpose.serial), 
     sizeof(unsigned long)},
   {"SEND_EVENT", "X_Bool", XtOffsetOf(XEvent, xnoexpose.send_event),
     sizeof(Bool)},
   {"DISPLAY", "XE_HexValue", XtOffsetOf(XEvent, xnoexpose.display),
     sizeof(Display *)},
   {"DRAWABLE", "XE_Window", XtOffsetOf(XEvent, xnoexpose.drawable),
     sizeof(Window)},
   {"MAJOR_CODE", "XE_IntValue", XtOffsetOf(XEvent, xnoexpose.major_code),
     sizeof(int)},
   {"MINOR_CODE", "XE_IntValue", XtOffsetOf(XEvent, xnoexpose.minor_code),
     sizeof(int)},
   {NULL, NULL, 0, 0},
};

static EventEntryTable xgraphicsExposeTable[] = {
   {"TYPE", "XE_EventType", 
     XtOffsetOf(XEvent, xgraphicsexpose.type), sizeof(int)},
   {"SERIAL", "XE_IntValue", XtOffsetOf(XEvent, xgraphicsexpose.serial), 
     sizeof(unsigned long)},
   {"SEND_EVENT", "X_Bool", XtOffsetOf(XEvent, xgraphicsexpose.send_event),
     sizeof(Bool)},
   {"DISPLAY", "XE_HexValue", XtOffsetOf(XEvent, xgraphicsexpose.display),
     sizeof(Display *)},
   {"DRAWABLE", "XE_Window", XtOffsetOf(XEvent, xgraphicsexpose.drawable),
     sizeof(Window)},
   {"X", "XE_IntValue", XtOffsetOf(XEvent, xgraphicsexpose.x),
     sizeof(int)},
   {"Y", "XE_IntValue", XtOffsetOf(XEvent, xgraphicsexpose.y),
     sizeof(int)},
   {"WIDTH", "XE_IntValue", XtOffsetOf(XEvent, xgraphicsexpose.width),
     sizeof(int)},
   {"HEIGHT", "XE_IntValue", XtOffsetOf(XEvent, xgraphicsexpose.height),
     sizeof(int)},
   {"COUNT", "XE_IntValue", XtOffsetOf(XEvent, xgraphicsexpose.count),
     sizeof(int)},
   {"MAJOR_CODE", "XE_IntValue", 
     XtOffsetOf(XEvent, xgraphicsexpose.major_code),
     sizeof(int)},
   {"MINOR_CODE", "XE_IntValue", XtOffsetOf(XEvent, xgraphicsexpose.minor_code),
     sizeof(int)},
   {NULL, NULL, 0, 0},
};

static EventEntryTable xkeyTable[] = {
   {"TYPE", "XE_EventType", XtOffsetOf(XEvent, xkey.type), sizeof(int)},
   {"SERIAL", "XE_IntValue", XtOffsetOf(XEvent, xkey.serial), 
     sizeof(unsigned long)},
   {"SEND_EVENT", "X_Bool", XtOffsetOf(XEvent, xkey.send_event),
     sizeof(Bool)},
   {"DISPLAY", "XE_HexValue", XtOffsetOf(XEvent, xkey.display),
     sizeof(Display *)},
   {"WINDOW", "XE_Window", XtOffsetOf(XEvent, xkey.window),
     sizeof(Window)},
   {"ROOT", "XE_Window", XtOffsetOf(XEvent, xkey.root),
     sizeof(Window)},
   {"SUBWINDOW", "XE_Window", XtOffsetOf(XEvent, xkey.subwindow),
     sizeof(Window)},
   {"TIME", "XE_IntValue", XtOffsetOf(XEvent, xkey.time),
     sizeof(unsigned int)},
   {"X", "XE_IntValue", XtOffsetOf(XEvent, xkey.x),
     sizeof(int)},
   {"Y", "XE_IntValue", XtOffsetOf(XEvent, xkey.y),
     sizeof(int)},
   {"X_ROOT", "XE_IntValue", XtOffsetOf(XEvent, xkey.x_root),
     sizeof(int)},
   {"Y_ROOT", "XE_IntValue", XtOffsetOf(XEvent, xkey.y_root),
     sizeof(int)},
   {"STATE", "XE_ModifierState", XtOffsetOf(XEvent, xkey.state),
     sizeof(unsigned int)},
   {"KEYCODE", "XE_IntValue", XtOffsetOf(XEvent, xkey.keycode),
     sizeof(unsigned int)},
   {"SAME_SCREEN", "X_Bool", XtOffsetOf(XEvent, xkey.same_screen),
     sizeof(Bool)},
   {NULL, NULL, 0, 0},
};

static EventEntryTable xmotionTable[] = {
   {"TYPE", "XE_EventType", XtOffsetOf(XEvent, xmotion.type), sizeof(int)},
   {"SERIAL", "XE_IntValue", XtOffsetOf(XEvent, xmotion.serial), 
     sizeof(unsigned long)},
   {"SEND_EVENT", "X_Bool", XtOffsetOf(XEvent, xmotion.send_event),
     sizeof(Bool)},
   {"DISPLAY", "XE_HexValue", XtOffsetOf(XEvent, xmotion.display),
     sizeof(Display *)},
   {"WINDOW", "XE_Window", XtOffsetOf(XEvent, xmotion.window),
     sizeof(Window)},
   {"ROOT", "XE_Window", XtOffsetOf(XEvent, xmotion.root),
     sizeof(Window)},
   {"SUBWINDOW", "XE_Window", XtOffsetOf(XEvent, xmotion.subwindow),
     sizeof(Window)},
   {"TIME", "XE_IntValue", XtOffsetOf(XEvent, xmotion.time),
     sizeof(unsigned int)},
   {"X", "XE_IntValue", XtOffsetOf(XEvent, xmotion.x),
     sizeof(int)},
   {"Y", "XE_IntValue", XtOffsetOf(XEvent, xmotion.y),
     sizeof(int)},
   {"X_ROOT", "XE_IntValue", XtOffsetOf(XEvent, xmotion.x_root),
     sizeof(int)},
   {"Y_ROOT", "XE_IntValue", XtOffsetOf(XEvent, xmotion.y_root),
     sizeof(int)},
   {"STATE", "XE_ModifierState", XtOffsetOf(XEvent, xmotion.state),
     sizeof(unsigned int)},
   {"IS_HINT", "XE_MotionHint", XtOffsetOf(XEvent, xmotion.is_hint),
     sizeof(char)},
   {"SAME_SCREEN", "X_Bool", XtOffsetOf(XEvent, xmotion.same_screen),
     sizeof(Bool)},
   {NULL, NULL, 0, 0},
};

/*
 * The order in which the structures are initialized IS important; they
 * must be done in the same order as they are defined in the eventTable
 * structure.
 */
static void
InitEventTables( void )
{
   int i = 0;

   eventTable[i++].table = xanyTable;
   eventTable[i++].table = xbuttonTable;
   eventTable[i++].table = xexposeTable;
   eventTable[i++].table = xnoExposeTable;
   eventTable[i++].table = xgraphicsExposeTable;
   eventTable[i++].table = xkeyTable;
   eventTable[i++].table = xmotionTable;
}


static Namval_t *
ProcessCallbackEvent(
        XEvent *event,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   static Boolean initialized = False;
   int i = 0;
   int j = 0;
   EventEntryTable * table;
   XrmValue fval, tval;
   char * ptr;

   if (!initialized)
   {
      InitEventTables();
      initialized = True;
   }

   np2 = GetNameValuePair(name);
   sprintf(buf, "0x%lx", (long)event);
   nv_putval(np2, buf, NV_RDONLY);

   if (name = strtok(NULL, "."))
   {
      if (event == NULL)
         np2 = CreateEmptyNameValuePair(np, name, fp);
      else
      {
         np2 = GetNameValuePair(name);
         if (strcmp(name, "TYPE") == 0)
         {
            fval.addr = (caddr_t)&(event->type);
            fval.size = sizeof(long);
            XtConvert(Toplevel, "XE_EventType", &fval, XtRString, &tval);
            if (tval.size != 0) 
               ptr = (char *)(tval.addr);
            else
               ptr = str_nill;
            nv_putval(np2, ptr, NV_RDONLY);
         }
         else
         {
            while (eventTable[i].eventType)
            {
               if (strcmp(eventTable[i].eventType, name) == 0)
               {
                  sprintf(buf, "0x%lx", (long)event);
                  nv_putval(np2, buf, NV_RDONLY);

                  if (name = strtok(NULL, "."))
                  {
                     np2 = GetNameValuePair(name);
                     table = eventTable[i].table;
                     while (table[j].fieldName)
                     {
                        if (strcmp(table[j].fieldName, name) == 0)
                        {
                           if (table[j].valueSize == sizeof(char))
                           {
                              fval.addr = (caddr_t) ((char *)
                                         ((char *)event+table[j].valueOffset));
                              fval.size = sizeof(char);
                           }
                           else if (table[j].valueSize == sizeof(short))
                           {
                              fval.addr = (caddr_t) ((short *)
                                         ((char *)event+table[j].valueOffset));
                              fval.size = sizeof(short);
                           }
                           else if (table[j].valueSize == sizeof(int))
			   {
			      fval.addr = (caddr_t) ((int *)
					  ((char *)event+table[j].valueOffset));
                              fval.size = sizeof(int);
			   }
                           else if (table[j].valueSize == sizeof(long))
                           {
                              fval.addr = (caddr_t) ((long *)
                                         ((char *)event+table[j].valueOffset));
                              fval.size = sizeof(long);
                           }
                           XtConvert(Toplevel, table[j].representation,
                                     &fval, XtRString, &tval);
                           if (tval.size != 0) 
                               ptr = (char *)(tval.addr);
                            else
                               ptr = str_nill;
                            nv_putval(np2, ptr, NV_RDONLY);
                           break;
                        }
                        j++;
                     }
                     if (table[j].fieldName == NULL)
                        nv_putval(np2, str_nill, NV_RDONLY);

                     break;
                  }
               }
               i++;
            }
            if (eventTable[i].eventType == NULL)
               nv_putval(np2, str_nill, NV_RDONLY);
         }

         if (name = strtok(NULL, "."))
            np2 = CreateEmptyNameValuePair(np, name, fp);
      }
   }
   return(np2);
}


static Namval_t *
_IntProcessCallbackReason(
        struct named_integer *table,
        XmAnyCallbackStruct *cbData,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];

   /*
    * We won't use an Xt converter here (even though we could), because
    * we want to be able to handle the case where the callback reason
    * is one we don't know about.
    */
   np2 = GetNameValuePair(name);
   while (table->name)
   {
      if (table->value == cbData->reason)
      {
         nv_putval(np2, table->name, NV_RDONLY);
         if (name = strtok(NULL, "."))
            np2 = CreateEmptyNameValuePair(np, name, fp);
         return(np2);
      }
      table++;
   }

   /* 
    * fdt: someday, allow for an expandable table, which can be 
    * added to by the shell script; useful when loading new widgets.
    */

   /* Unknown callback reason; simply return the integer value */
   sprintf(buf, "%d", cbData->reason);
   nv_putval(np2, buf, NV_RDONLY);
   if (name = strtok(NULL, "."))
      np2 = CreateEmptyNameValuePair(np, name, fp);
   return(np2);
}


static Namval_t *
ProcessCallbackReason(
        XmAnyCallbackStruct *cbData,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   return(_IntProcessCallbackReason(CallbackReasons, cbData, np, name, fp));
}


/*
 * This requires a separate handler, due to the fact that the help
 * callback reasons overlap the standard Motif callback reasons!!
 */
static Namval_t *
ProcessHelpCallbackReason(
        XmAnyCallbackStruct *cbData,
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   return(_IntProcessCallbackReason(HelpCallbackReasons, cbData, np, name, fp));
}


/*
 * Certain classes of callbacks do not return a structure as the calldata.
 * Examples are the destroyCallback, popupCallback, popdownCallback and
 * the workspace changed callback. Since the calldata is a value, and not
 * a structure, any references to subfields are invalid.
 */
Namval_t *
nopCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char * token;
   char * dupName = strdup(name);

   token = strtok(dupName, ".");

   np2 = CreateEmptyNameValuePair(np, token, fp);
   XtFree(dupName);
   return(np2);
}


/*
 * This function creates a new name/value pair (representing an environment
 * variable) when it is referenced.  If the name/value pair already exists,
 * then it is simply reused.  We keep track of all the name/value pairs
 * we create, so that they can be destroyed when callback processing has
 * completed.  This handles the XmAnyCallbackStruct.
 */
Namval_t *
dftCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   XmAnyCallbackStruct * cbData;
   char * cbDataAddrStr;
   char * p;
   char * token;
   char * dupName = strdup(name);

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmAnyCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}


/*
 * This is the discipline handler for an event handler.
 * It only knows how to reference the event structure.
 */
Namval_t *
ehCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   XEvent * event;
   char * eventAddrStr;
   char * p;
   char * token;
   char * tmpBuf;

   eventAddrStr = nv_getv(np, fp);
   event = (XEvent *)strtoul(eventAddrStr, &p, 0);
   tmpBuf = XtMalloc(strlen(name) + strlen("EH_EVENT") + 2);
   sprintf(tmpBuf, "%s.%s", "EH_EVENT", name);
   token = strtok(tmpBuf, ".");
   np2 = ProcessCallbackEvent(event, np, token, fp);
   XtFree(tmpBuf);
   return(np2);
}


/*
 * This is the discipline handler for the translation handler.
 * It only knows how to reference the event structure.
 */
Namval_t *
transCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   Namval_t * np2;
   char buf[128];
   XEvent * event;
   char * eventAddrStr;
   char * p;
   char * token;
   char * tmpBuf;

   eventAddrStr = nv_getv(np, fp);
   event = (XEvent *)strtoul(eventAddrStr, &p, 0);
   tmpBuf = XtMalloc(strlen(name) + strlen("TRANSLATION_EVENT") + 2);
   sprintf(tmpBuf, "%s.%s", "TRANSLATION_EVENT", name);
   token = strtok(tmpBuf, ".");
   np2 = ProcessCallbackEvent(event, np, token, fp);
   XtFree(tmpBuf);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the scale widget. 
 * This handles the XmScaleCallbackStruct.
 */
Namval_t *
scaleCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmScaleCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmScaleCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "VALUE") == 0)
      np2 = ProcessIntValue((int)cbData->value, np, token, fp, "%d", NULL);
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the arrow widget. 
 * This handles the XmArrowButtonCallbackStruct.
 */
Namval_t *
arrowCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmArrowButtonCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmArrowButtonCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "CLICK_COUNT") == 0)
   {
      if (cbData->reason == XmCR_ACTIVATE)
      {
         np2 = ProcessIntValue((int)cbData->click_count, np, token, fp, "%d", 
                               NULL);
      }
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the comboBox widget. 
 * This handles the XmComboBoxCallbackStruct.
 */
Namval_t *
comboCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmComboBoxCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmComboBoxCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData,
				  np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "ITEM_OR_TEXT") == 0)
      np2 = ProcessXmStringValue(cbData->item_or_text, np, token, fp);
   else if (strcmp(token, "ITEM_POSITION") == 0)
   {
      np2 = ProcessIntValue((int)cbData->item_position, np, token, fp, "%d",
                            NULL);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the command widget. 
 * This handles the XmCommandCallbackStruct.
 */
Namval_t *
cmdCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmCommandCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;
   int len;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmCommandCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "VALUE") == 0)
      np2 = ProcessXmStringValue(cbData->value, np, token, fp);
   else if (strcmp(token, "LENGTH") == 0)
   {
      if ((p = XmStringToString(cbData->value)) == NULL)
         len = 0;
      else
         len = strlen(p);
      np2 = ProcessIntValue((int)len, np, token, fp, "%d", NULL);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the drawingArea widget. 
 * This handles the XmDrawingAreaCallbackStruct.
 */
Namval_t *
dAreaCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmDrawingAreaCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmDrawingAreaCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "WINDOW") == 0)
      np2 = ProcessIntValue((int)cbData->window, np, token, fp, "0x%x", NULL);
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the drawnButton widget. 
 * This handles the XmDrawnButtonCallbackStruct.
 */
Namval_t *
dbtnCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmDrawnButtonCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmDrawnButtonCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "WINDOW") == 0)
      np2 = ProcessIntValue((int)cbData->window, np, token, fp, "0x%x", NULL);
   else if (strcmp(token, "CLICK_COUNT") == 0)
   {
      if (cbData->reason == XmCR_ACTIVATE)
      {
         np2 = ProcessIntValue((int)cbData->click_count, np, token, fp, "%d", 
                               NULL);
      }
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the file selection widget. 
 * This handles the XmFileSelectionBoxCallbackStruct.
 */
Namval_t *
fselCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmFileSelectionBoxCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;
   int len;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmFileSelectionBoxCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "VALUE") == 0)
      np2 = ProcessXmStringValue(cbData->value, np, token, fp);
   else if (strcmp(token, "LENGTH") == 0)
   {
      if ((p = XmStringToString(cbData->value)) == NULL)
         len = 0;
      else
         len = strlen(p);
      np2 = ProcessIntValue(len, np, token, fp, "%d", NULL);
   }
   else if (strcmp(token, "MASK") == 0)
      np2 = ProcessXmStringValue(cbData->mask, np, token, fp);
   else if (strcmp(token, "MASK_LENGTH") == 0)
   {
      if ((p = XmStringToString(cbData->mask)) == NULL)
         len = 0;
      else
         len = strlen(p);
      np2 = ProcessIntValue(len, np, token, fp, "%d", NULL);
   }
   else if (strcmp(token, "DIR") == 0)
      np2 = ProcessXmStringValue(cbData->dir, np, token, fp);
   else if (strcmp(token, "DIR_LENGTH") == 0)
   {
      if ((p = XmStringToString(cbData->dir)) == NULL)
         len = 0;
      else
         len = strlen(p);
      np2 = ProcessIntValue(len, np, token, fp, "%d", NULL);
   }
   else if (strcmp(token, "PATTERN") == 0)
      np2 = ProcessXmStringValue(cbData->pattern, np, token, fp);
   else if (strcmp(token, "PATTERN_LENGTH") == 0)
   {
      if ((p = XmStringToString(cbData->pattern)) == NULL)
         len = 0;
      else
         len = strlen(p);
      np2 = ProcessIntValue(len, np, token, fp, "%d", NULL);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the list widget. 
 * This handles the XmListCallbackStruct.
 */
Namval_t *
listCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmListCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;
   int len;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmListCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "ITEM") == 0)
      np2 = ProcessXmStringValue(cbData->item, np, token, fp);
   else if (strcmp(token, "ITEM_LENGTH") == 0)
   {
      if ((p = XmStringToString(cbData->item)) == NULL)
         len = 0;
      else
         len = strlen(p);
      np2 = ProcessIntValue(len, np, token, fp, "%d", NULL);
   }
   else if (strcmp(token, "ITEM_POSITION") == 0)
   {
      np2 = ProcessIntValue((int)cbData->item_position, np, token, fp, "%d", 
                            NULL);
   }
   else if (strcmp(token, "SELECTED_ITEMS") == 0)
   {
      if ((cbData->reason == XmCR_DEFAULT_ACTION) ||
          (cbData->reason == XmCR_MULTIPLE_SELECT) ||
          (cbData->reason == XmCR_EXTENDED_SELECT))
      {
         np2 = ProcessXmStringTable(cbData->selected_items,
                                     cbData->selected_item_count, 
                                     np, token, fp);
      }
      else
         np2 =  CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "SELECTED_ITEM_COUNT") == 0)
   {
      if ((cbData->reason == XmCR_DEFAULT_ACTION) ||
          (cbData->reason == XmCR_MULTIPLE_SELECT) ||
          (cbData->reason == XmCR_EXTENDED_SELECT))
      {
         np2 = ProcessIntValue((int)cbData->selected_item_count, np, token, 
                                fp, "%d", NULL);
      }
      else
         np2 =  CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "SELECTED_ITEM_POSITIONS") == 0)
   {
      if ((cbData->reason == XmCR_DEFAULT_ACTION) ||
          (cbData->reason == XmCR_MULTIPLE_SELECT) ||
          (cbData->reason == XmCR_EXTENDED_SELECT))
      {
         np2 = ProcessIntTable(cbData->selected_item_positions,
                                cbData->selected_item_count,
                                np, token, fp);
      }
      else
         np2 =  CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "SELECTION_TYPE") == 0)
   {
      if (cbData->reason == XmCR_EXTENDED_SELECT)
         np2 = ProcessSelectionType(cbData->selection_type, np, token, fp);
      else
         np2 =  CreateEmptyNameValuePair(np, token, fp);
   }
   else
      np2 =  CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the pushbutton widget. 
 * This handles the XmPushButtonCallbackStruct.
 */
Namval_t *
pbtnCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmPushButtonCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmPushButtonCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "CLICK_COUNT") == 0)
   {
      if (cbData->reason == XmCR_ACTIVATE)
      {
         np2 = ProcessIntValue((int)cbData->click_count, np, token, fp, "%d", 
                               NULL);
      }
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the rowcolumn widget. 
 * This handles the XmRowColumnCallbackStruct.
 */
Namval_t *
rcCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmRowColumnCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmRowColumnCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
   {
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, 
                                  fp);
   }
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if ((strcmp(token, "WIDGET") == 0) && 
            (cbData->reason == XmCR_ACTIVATE))
   {
      np2 = ProcessWidgetHandle(cbData->widget, np, token, fp);
   }
   else if ((strcmp(token, "DATA") == 0) && (cbData->reason == XmCR_ACTIVATE))
      np2 = ProcessIntValue((int)cbData->data, np, token, fp, "0x%x", NULL);
   else if ((strcmp(token, "CALLBACKSTRUCT") == 0) && 
            (cbData->reason == XmCR_ACTIVATE))
   {
      np2 = ProcessIntValue((int)cbData->callbackstruct, np, token,fp,"0x%x", 
                            NULL);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the scrollbar widget. 
 * This handles the XmScrollBarCallbackStruct.
 */
Namval_t *
sbarCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmScrollBarCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmScrollBarCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "VALUE") == 0)
      np2 = ProcessIntValue((int)cbData->value, np, token, fp, "%d", NULL);
   else if (strcmp(token, "PIXEL") == 0)
   {
      if ((cbData->reason == XmCR_TO_BOTTOM) || (cbData->reason == XmCR_TO_TOP))
         np2 = ProcessIntValue((int)cbData->pixel, np, token,fp,"%d", NULL);
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the scrolledwindow widget. 
 * This handles the XmTraverseObsuredCallbackStruct.
 */
Namval_t *
swinCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTraverseObscuredCallbackStruct * cbData;
   char * token;
   char * p;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmTraverseObscuredCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "TRAVERSAL_DESTINATION") == 0)
      np2 = ProcessWidgetHandle(cbData->traversal_destination, np, token, fp);
   else if (strcmp(token, "DIRECTION") == 0)
      np2 = ProcessTraversalDirection(cbData->direction, np, token, fp);
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the selection box widget. 
 * This handles the XmSelectionBoxCallbackStruct.
 */
Namval_t *
sboxCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmSelectionBoxCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;
   int len;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmSelectionBoxCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "VALUE") == 0)
      np2 = ProcessXmStringValue(cbData->value, np, token, fp);
   else if (strcmp(token, "LENGTH") == 0)
   {
      if ((p = XmStringToString(cbData->value)) == NULL)
         len = 0;
      else
         len = strlen(p);
      np2 = ProcessIntValue(len, np, token, fp, "%d", NULL);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the toggle widget. 
 * This handles the XmToggleButtonCallbackStruct.
 */
Namval_t *
tbtnCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmToggleButtonCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmToggleButtonCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "SET") == 0)
      np2 = ProcessBooleanIntValue((int)cbData->set, np, token, fp, NULL);
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is one ofthe 'create' discipline function for the text widget. 
 * This handles the XmTextVerifyCallbackStruct.  If a subfield can
 * be altered by a shell script (i.e. CB_CALL_DATA.TEXT.PTR), then
 * we must assign a discipline to the 'PTR' environment variable,
 * so that we will be notified when the assignment occurs, and thus
 * can update the real callback structure.
 */
Namval_t *
textCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTextVerifyCallbackStruct * cbData;
   char * p;
   char * token;
   XrmValue f, t;
   char * value;
   char buf[25];
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmTextVerifyCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
      np2 = ProcessCallbackReason((XmAnyCallbackStruct *)cbData, np, token, fp);
   else if (strcmp(token, "EVENT") == 0)
      np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
   else if (strcmp(token, "DOIT") == 0)
   {
      static Namfun_t * disc = NULL;

      if (disc == NULL)
        disc = CloneDiscipline(&text_doit_disc);

      np2 = ProcessBooleanIntValue((int)cbData->doit, np, token, fp, disc);
   }
   else if (strcmp(token, "CURRINSERT") == 0)
      np2 = ProcessIntValue((int)cbData->currInsert, np, token, fp, "%d", NULL);
   else if (strcmp(token, "NEWINSERT") == 0)
      np2 = ProcessIntValue((int)cbData->newInsert, np, token, fp, "%d", NULL);
   else if (strcmp(token, "STARTPOS") == 0)
   {
      static Namfun_t * disc = NULL;

      if (disc == NULL)
        disc = CloneDiscipline(&text_startpos_disc);

      if ((cbData->reason == XmCR_LOSING_FOCUS) ||
          (cbData->reason == XmCR_MODIFYING_TEXT_VALUE))
      {
         np2 = ProcessIntValue((int)cbData->startPos, np, token, fp,"%d", disc);
      }
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "ENDPOS") == 0)
   {
      static Namfun_t * disc = NULL;

      if (disc == NULL)
        disc = CloneDiscipline(&text_endpos_disc);

      if ((cbData->reason == XmCR_LOSING_FOCUS) ||
          (cbData->reason == XmCR_MODIFYING_TEXT_VALUE))
      {
         np2 = ProcessIntValue((int)cbData->endPos, np, token, fp, "%d", disc);
      }
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "TEXT") == 0)
   {
      if (cbData->reason == XmCR_MODIFYING_TEXT_VALUE)
      {
         np2 = GetNameValuePair(token);
         sprintf(buf, "0x%lx", (long)cbData->text);
         nv_putval(np2, buf, NV_RDONLY);

         /* Need to handle the substructure fields */
         if (token = strtok(NULL, "."))
         {
            np2 = GetNameValuePair(token);
            if (strcmp(token, "PTR") == 0)
            {
               static Namfun_t * disc = NULL;

               if (disc == NULL)
                 disc = CloneDiscipline(&text_ptr_disc);

               /* Any old disciplies MUST be cleared, before setting value */
               nv_stack(np2, NULL);
               if (cbData->text->ptr)
                  nv_putval(np2, cbData->text->ptr, NV_RDONLY);
               else
                  nv_putval(np2, str_nill, NV_RDONLY);
               nv_stack(np2, disc);
            }
            else if (strcmp(token, "LENGTH") == 0)
            {
               static Namfun_t * disc = NULL; 

               if (disc == NULL)
                 disc = CloneDiscipline(&text_len_disc);

               /* Any old disciplies MUST be cleared, before setting value */
               sprintf(buf, "%d", cbData->text->length);
               nv_stack(np2, NULL);
               nv_putval(np2, buf, NV_RDONLY);
               nv_stack(np2, disc);
            }
            else if (strcmp(token, "FORMAT") == 0)
            {
               static Namfun_t * disc = NULL;

               if (disc == NULL)
                 disc = CloneDiscipline(&text_format_disc);

               f.addr = (caddr_t)&(cbData->text->format);
               f.size = sizeof(XmTextFormat);
               t.addr = NULL;
               t.size = 0;
               XtConvert(Toplevel, "TextFormat", &f, XtRString, &t);
  
               /* Any old disciplies MUST be cleared, before setting value */
               if (t.size && t.addr)
                  value = t.addr;
               else
                  value = str_nill;
               nv_stack(np2, NULL);
               nv_putval(np2, value, NV_RDONLY);
               nv_stack(np2, disc);
            }
            else
               nv_putval(np2, str_nill, NV_RDONLY);

            /* No deeper nesting is supported */
            if (token = strtok(NULL, "."))
               np2 = CreateEmptyNameValuePair(np, token, fp);
         }
      }
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is one ofthe 'create' discipline function for the text widget. 
 * This handles the XmTextVerifyCallbackStructWcs.
 */
Namval_t *
textCreateDisc2(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTextVerifyCallbackStructWcs * cbData;
   char * p;
   char * token;
   Namval_t * np2;
   char * nameCopy = strdup(name);
   char * dupName = strdup(name);
   char buf[25];

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (XmTextVerifyCallbackStructWcs *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "TEXT") == 0)
   {
      np2 = GetNameValuePair(token);
      sprintf(buf, "0x%lx", (long)cbData->text);
      nv_putval(np2, buf, NV_RDONLY);

      /* Need to handle the substructure fields */
      if (token = strtok(NULL, "."))
      {
         np2 = GetNameValuePair(token);
         if (strcmp(token, "WCSPTR") == 0)
         {
            static Namfun_t * disc = NULL;

            if (disc == NULL)
              disc = CloneDiscipline(&text_wcsptr_disc);

            /* Any old disciplies MUST be cleared, before setting value */
            nv_stack(np2, NULL);
            if (cbData->text->wcsptr)
            {
               wchar_t * wcBuf;
               char * mbBuf;
               int mbBufSize;
               int count, i;

               /* 
                * It appears that the wchar string coming in is NOT NULL
                * terminated; we must make our own copy, before calling
                * wcstombs().
                */
               wcBuf = (wchar_t *)XtMalloc((cbData->text->length + 1) * 
                                           sizeof(wchar_t));
               for ( i = 0; i < cbData->text->length; i++)
                  wcBuf[i] = cbData->text->wcsptr[i];
               wcBuf[i] = 0;

               mbBufSize = ((cbData->text->length + 1) * sizeof(wchar_t));
               mbBuf = XtMalloc(mbBufSize);
               count = wcstombs(mbBuf, wcBuf, mbBufSize - sizeof(wchar_t));
               if (count >= 0)
               {
                  mbBuf[count] = '\0';
                  nv_putval(np2, mbBuf, NV_RDONLY);
               }
               else
                  nv_putval(np2, str_nill, NV_RDONLY);
               XtFree(mbBuf);
               XtFree((char *)wcBuf);
            }
            else
               nv_putval(np2, str_nill, NV_RDONLY);
            nv_stack(np2, disc);
         }
         else if (strcmp(token, "LENGTH") == 0)
         {
            static Namfun_t * disc = NULL; 

            if (disc == NULL)
              disc = CloneDiscipline(&text_wcslen_disc);

            /* Any old disciplies MUST be cleared, before setting value */
            sprintf(buf, "%d", cbData->text->length);
            nv_stack(np2, NULL);
            nv_putval(np2, buf, NV_RDONLY);
            nv_stack(np2, disc);
         }
         else
            nv_putval(np2, str_nill, NV_RDONLY);

         /* No deeper nesting is supported */
         if (token = strtok(NULL, "."))
            np2 = CreateEmptyNameValuePair(np, token, fp);
      }
   }
   else
      np2 = textCreateDisc(np, nameCopy, fp);

   XtFree(nameCopy);
   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'create' discipline function for the help widget. 
 * This handles the DtHelpDialogCallbackStruct.
 */
Namval_t *
helpCreateDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   DtHelpDialogCallbackStruct * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (DtHelpDialogCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   /* cbData can be NULL if invoked from XtCallCallbacks */
   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "REASON") == 0)
   {
      /*
       * Can't use the normal callback reason function, since the help
       * callback reasons are not unique!  They overlap with the normal
       * callback reasons.
       */
      np2 = ProcessHelpCallbackReason((XmAnyCallbackStruct *)cbData, np, 
                                      token, fp);
   }
   else if (strcmp(token, "EVENT") == 0)
   {
      if (cbData->event)
         np2 = ProcessCallbackEvent(cbData->event, np, token, fp);
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "LOCATIONID") == 0)
   {
      if (cbData->reason == DtCR_HELP_LINK_ACTIVATE)
         np2 = ProcessStringValue(cbData->locationId, np, token, fp);
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "HELPVOLUME") == 0)
   {
      if (cbData->reason == DtCR_HELP_LINK_ACTIVATE)
         np2 = ProcessStringValue(cbData->helpVolume, np, token, fp);
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "SPECIFICATION") == 0)
   {
      if (cbData->reason == DtCR_HELP_LINK_ACTIVATE)
         np2 = ProcessStringValue(cbData->specification, np, token, fp);
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else if (strcmp(token, "HYPERTYPE") == 0)
   {
      if (cbData->reason == DtCR_HELP_LINK_ACTIVATE)
         np2 = ProcessHyperType(cbData->hyperType, np, token, fp);
      else
         np2 = CreateEmptyNameValuePair(np, token, fp);
   }
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}

/* 
 * This is the 'DtPrintSetupProc' discipline function for the DtPrintSetupBox
 * widget. It handles the DtPrintSetupData struct with the exception of the
 * print_display and print_screen members.
 */
Namval_t *
dtPrintSetupProcDisc(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   DtPrintSetupData * cbData;
   char * p;
   char * token;
   char * dupName = strdup(name);
   Namval_t * np2;

   cbDataAddrStr = nv_getv(np, fp);
   cbData = (DtPrintSetupData *)strtoul(cbDataAddrStr, &p, 0);

   token = strtok(dupName, ".");

   if (cbData == NULL)
      np2 = CreateEmptyNameValuePair(np, token, fp);
   else if (strcmp(token, "PRINTER_NAME") == 0)
      np2 = ProcessStringValue(cbData->printer_name, np, token, fp);
   else if (strcmp(token, "DESTINATION") == 0)
      np2 = ProcessIntValue(cbData->destination, np, token, fp, "%d", NULL);
   else if (strcmp(token, "DEST_INFO") == 0)
      np2 = ProcessStringValue(cbData->dest_info, np, token, fp);
   else
      np2 = CreateEmptyNameValuePair(np, token, fp);

   XtFree(dupName);
   return(np2);
}


/****************************************************************************
 *
 * The following functions are used to create and free a copy of a
 * discipline structure.  We register a discipline when we want to
 * be notified that the shell script has referenced one of our special
 * environment variables.  This gives us the ability to dynamically
 * assign the value they will receive.  This is used during callback,
 * translation and event handling (i.e. CB_CALL_DATA, etc).  We need
 * to duplicate the discipline structure due to how ksh handles these
 * structures; it stores each successive one in a linked list.  Typically,
 * this is not a problem.  However, because callbacks can become nested
 * (From within one callback, the script does something which causes another
 * callback to fire), if the same discipline structure gets passed in twice,
 * when it gets added to the linked list, the original 'next' pointer gets
 * trashed; the typical result is that the next time one of the special
 * environment variables is referenced, dtksh goes into an infinite loop.
 * The solution appears to be to 'clone' the discipline, thus creating
 * a unique structure each time, and preventing the 'next' pointer from
 * getting trashed.
 *
 **************************************************************************/

static Namfun_t *
CloneDiscipline(
        Namdisc_t * discipline )
{
   Namfun_t * clonedDisc;

   clonedDisc = (Namfun_t *)XtMalloc(sizeof(Namfun_t));
   clonedDisc->disc = discipline;
   clonedDisc->next = NULL;
   return(clonedDisc);
}


static void
FreeDiscipline(
        Namfun_t * discipline )
{
   XtFree((char *)discipline);
}


void
SetTextDoit(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   XrmValue fval, tval;
   Boolean doit = True;
   char * cbDataAddrStr;
   XmTextVerifyCallbackStruct * cbData;
   char * p;
   Namval_t *cbDataNp;

   /* Convert from string to a boolean */
   fval.addr = name;
   fval.size = strlen(name);
   XtConvert(Toplevel, XtRString, &fval, XtRBoolean, &tval);

   if (tval.size != 0)
      doit = *((Boolean *)(tval.addr));

   if (cbDataNp = nv_open("CB_CALL_DATA", sh.var_tree, 0))
   {
      cbDataAddrStr = nv_getv(cbDataNp, NULL);
      cbData=(XmTextVerifyCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);
      cbData->doit = doit;
   }
}

void
SetTextStartPos(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTextVerifyCallbackStruct * cbData;
   char * p;
   Namval_t *cbDataNp;

   if (cbDataNp = nv_open("CB_CALL_DATA", sh.var_tree, 0))
   {
      cbDataAddrStr = nv_getv(cbDataNp, NULL);
      cbData=(XmTextVerifyCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);
      cbData->startPos = strtoul(name, &p, 0);
   }
}

void
SetTextEndPos(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTextVerifyCallbackStruct * cbData;
   char * p;
   Namval_t *cbDataNp;

   if (cbDataNp = nv_open("CB_CALL_DATA", sh.var_tree, 0))
   {
      cbDataAddrStr = nv_getv(cbDataNp, NULL);
      cbData=(XmTextVerifyCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);
      cbData->endPos = strtoul(name, &p, 0);
   }
}

void
SetTextPtr(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTextVerifyCallbackStruct * cbData;
   char * p;
   Namval_t *cbDataNp;
   static char * staticBuf = NULL;

   if (cbDataNp = nv_open("CB_CALL_DATA", sh.var_tree, 0))
   {
      cbDataAddrStr = nv_getv(cbDataNp, NULL);
      cbData=(XmTextVerifyCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);

      /* fdt
       * MOTIF BUG ALERT!!
       * Motif will not currently free the original buffer,
       * but will instead free our buffer.
       *
       * XtFree(staticBuf);
       */
      staticBuf = strdup(name);
      cbData->text->ptr = staticBuf;
   }
}

void
SetTextLen(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTextVerifyCallbackStruct * cbData;
   char * p;
   Namval_t *cbDataNp;

   if (cbDataNp = nv_open("CB_CALL_DATA", sh.var_tree, 0))
   {
      cbDataAddrStr = nv_getv(cbDataNp, NULL);
      cbData=(XmTextVerifyCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);
      cbData->text->length = strtoul(name, &p, 0);
   }
}

void
SetTextFormat(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   XrmValue fval, tval;
   Boolean doit = True;
   char * cbDataAddrStr;
   XmTextVerifyCallbackStruct * cbData;
   char * p;
   Namval_t *cbDataNp;

   /* Convert from string to format type */
   fval.addr = name;
   fval.size = strlen(name);
   XtConvert(Toplevel, XtRString, &fval, "TextFormat", &tval);

   if (tval.size && tval.addr &&
      (cbDataNp = nv_open("CB_CALL_DATA", sh.var_tree, 0)))
   {
      cbDataAddrStr = nv_getv(cbDataNp, NULL);
      cbData=(XmTextVerifyCallbackStruct *)strtoul(cbDataAddrStr, &p, 0);
      cbData->text->format = *((int *)(tval.addr));
   }
}

void
SetTextWCSptr(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTextVerifyCallbackStructWcs * cbData;
   char * p;
   Namval_t *cbDataNp;
   static wchar_t * wcBuf = NULL;
   int count;

   if (cbDataNp = nv_open("CB_CALL_DATA", sh.var_tree, 0))
   {
      cbDataAddrStr = nv_getv(cbDataNp, NULL);
      cbData=(XmTextVerifyCallbackStructWcs *)strtoul(cbDataAddrStr, &p, 0);

      /* fdt
       * MOTIF BUG ALERT!!
       * Motif will not currently free the original buffer,
       * but will instead free our buffer.
       *
       * XtFree(wcBuf);
       */
      wcBuf = (wchar_t *)XtMalloc((strlen(name) + 1) * sizeof(wchar_t));
      count = mbstowcs(wcBuf, name, strlen(name)+1);
      cbData->text->wcsptr = wcBuf;
   }
}

void
SetTextWCSlen(
        Namval_t *np,
        char *name,
        Namfun_t *fp )
{
   char * cbDataAddrStr;
   XmTextVerifyCallbackStructWcs * cbData;
   char * p;
   Namval_t *cbDataNp;

   if (cbDataNp = nv_open("CB_CALL_DATA", sh.var_tree, 0))
   {
      cbDataAddrStr = nv_getv(cbDataNp, NULL);
      cbData=(XmTextVerifyCallbackStructWcs *)strtoul(cbDataAddrStr, &p, 0);
      cbData->text->length = strtoul(name, &p, 0);
   }
}

/*
 * stdPrintSetupProc() is the central routine from which the DtPrintSetupBox
 * widget's DtPrintSetupProc resources are dispatched.
 * The variable "CB_WIDGET" will be placed in the environment to represent
 * the CallBackWidget handle.  Because DtPrintSetupProc doesn't work the
 * same way as a callback, we had to fudge this to work by storing the command
 * string inside the widget's wtab_t->info field.  This is pretty nasty and
 * non-generic, but there does not appear to be a good solution.
 *
 * The call_data is made available through the CD_CALL_DATA variable.
 *
 * If a new DtPrintSetupProc resource needs to be supported, the list of
 * hardcoded procs and associated information have to be updated, as follows:
 * - add a #define of the resource in dtksh.h
 * - add a new member in the ProcInfo_t to store that command in dtksh.h.
 * - add a new function called std<resource_name> which calls
 * stdPrintSetupProc().
 * - add a new case to set the command to the above function for the new
 * resource in DtkshCvtStringToPrintSetupProc().
 * - add a new case in the switch statement this function to account for
 * the new resource.
 *
 */

void
stdPrintSetupProc(
	int proctype,
	void  *widget,
	DtPrintSetupData *callData)
{
	wtab_t *w;
	ProcInfo_t *pinfo;
	char buf[128];
	Namval_t *np;
	Namval_t *np2;
	WidgetClass class;
	Namdisc_t *discipline = NULL;
	char *oldCB_WIDGET_value = NULL;
	char *oldCB_CALL_DATA_value = NULL;
	char *ptr;
	Namfun_t *clonedDisc;

	w = widget_to_wtab((Widget)widget);
	if (w == NULL)
		return;
	pinfo = (ProcInfo_t *) w->info;
	if (pinfo == NULL)
		return;

	nestingLevel++;
	np2 = GetNameValuePair("CB_WIDGET");
	nv_newattr(np2, 0, 0);
	if (ptr = nv_getval(np2))
		oldCB_WIDGET_value = strdup(ptr);
	nv_putval(np2, w->widid, NV_RDONLY);
	nv_newattr(np2, NV_RDONLY, 0);

	sprintf(buf, "0x%lx", (long)callData);

	np = GetNameValuePair("CB_CALL_DATA");
	nv_newattr(np, 0, 0);
	if (ptr = nv_getval(np))
		oldCB_CALL_DATA_value = strdup(ptr);
	nv_putval(np, buf, NV_RDONLY);
	nv_newattr(np, NV_RDONLY, 0);

	/* look up the discipline for DtPrintSetupProc */
	class = XtClass(w->w);
	while (class) {
		if (discipline = CheckClassDisciplines(class, DtRPrintSetupProc))
			break;
		class = class->core_class.superclass;
	}

   /* 
    * If a discipline was found, then use it; otherwise, we MUST set up
    * a default discipline; otherwise, any hierarchical variables 
    * referenced by the user are not under our control, thus never getting
    * freed up, and then also preventing future disciplines from getting
    * called when they should have.
    */
   if (discipline)
      clonedDisc = CloneDiscipline(discipline);
   else
      clonedDisc = CloneDiscipline(&dftDiscipline);
   nv_stack(np, clonedDisc);

	switch (proctype) {
	case PRINTER_INFO_PROC:
		ksh_eval(pinfo->printerInfoProcCommand);
		break;
	case SELECT_FILE_PROC:
		ksh_eval(pinfo->selectFileProcCommand);
		break;
	case SELECT_PRINTER_PROC:
		ksh_eval(pinfo->selectPrinterProcCommand);
		break;
	case SETUP_PROC:
		ksh_eval(pinfo->setupProcCommand);
		break;
	case VERIFY_PRINTER_PROC:
		ksh_eval(pinfo->verifyPrinterProcCommand);
		break;
	}

	/* We may be nested, so restore old CB_WIDGET & CB_CALL_DATA values */
	RestorePriorEnvVarValues(np2, oldCB_WIDGET_value, np, oldCB_CALL_DATA_value);
	XtFree(oldCB_WIDGET_value);
	XtFree(oldCB_CALL_DATA_value);

	/* Remove the discipline for the hierarchical variables */
	nv_stack(np, NULL);
	FreeDiscipline(clonedDisc);

	/* Free up all of the name/value pairs we created */
	FreeNestedVariables();
	nestingLevel--;

	return;
}

void
stdPrinterInfoProc(
	void  *widget,
    DtPrintSetupData *callData)
{
	stdPrintSetupProc(PRINTER_INFO_PROC, widget, callData);
	return;
}

void
stdSelectFileProc(
	void  *widget,
    DtPrintSetupData *callData)
{
	stdPrintSetupProc(SELECT_FILE_PROC, widget, callData);
	return;
}

void
stdSelectPrinterProc(
	void  *widget,
    DtPrintSetupData *psd)
{
	stdPrintSetupProc(SELECT_PRINTER_PROC, widget, psd);
	return;
}

void
stdSetupProc(
	void  *widget,
    DtPrintSetupData *callData)
{
	stdPrintSetupProc(SETUP_PROC, widget, callData);
	return;
}

void
stdVerifyPrinterProc(
	void  *widget,
    DtPrintSetupData *callData)
{
	stdPrintSetupProc(VERIFY_PRINTER_PROC, widget, callData);
	return;
}

