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
/* $TOG: vgcallback.c /main/16 1998/11/02 18:34:55 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        vgcallback.c
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Callback routines Dtgreet application.
 **
 **                These routines handle the callbacks from the widgets.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/


/***************************************************************************
 *
 *  Includes
 *
 ***************************************************************************/

#include	<stdio.h>
#include	<unistd.h>
#include	<setjmp.h>
#include	<signal.h>
#include	<sys/signal.h>
#include	<sys/param.h>

#include	<Xm/Xm.h>
#include	<Xm/MessageB.h>
#include	<Xm/TextF.h>
#include	<Xm/TextFP.h>
#include	<Xm/PushBG.h>
#include	<Xm/ToggleBG.h>
#include    <Dt/IconFile.h>
#include    <Dt/Icon.h>
#include	<pwd.h>


#ifdef AUDIT
#    include <sys/audit.h>
#endif

/* necessary for bzero */
#ifdef SVR4
#include        <X11/Xfuncs.h>
#endif

#include	"vg.h"
#include	"vgmsg.h"


/***************************************************************************
 *
 *  External declarations
 *
 ***************************************************************************/

extern  LogoInfo	logoInfo; /* information about the logo		   */



/***************************************************************************
 *
 *  Procedure declarations
 *
 ***************************************************************************/

static void CenterForm( Widget w1, Widget w2);
static void PingLost( void ) ;
static SIGVAL PingBlocked( int arg ) ;
static void ProcessTraversal( Widget w, int direction) ;
static void _DtShowDialog(DialogType dtype, XmString msg);
static void TellRequester(char * buf, size_t nbytes);

#  ifdef BLS
static	void PromptSensitivityLevel(void); /* prompt for B1 Sen. Level	   */
	int  VerifySensitivityLevel(void); /* verify B1 Sensitivity Level  */
#  endif

static int session_selected = False;
static Widget default_dt = NULL;


/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/

       Widget	focusWidget = NULL;

char	*userName = "\0";
struct passwd *user_p;


#ifdef BLS
static  int	normalPasswordWidget = True;
	char	*sensitivityLevel = NULL;
#endif

#ifndef SVR4
	long	groups[NGROUPS];
#endif

#ifdef SIA
  
#include <alloca.h>

SiaFormInfo siaFormInfo;


XmString multiline_xmstring(char *text)
{

    char *start, *end;
    Boolean done;
    XmString string = NULL;
    XmString tmp_string;
    XmString separator = NULL;
    char *buffer;

    if (!text) return (NULL);


    buffer = alloca(strlen((const char *)text) + 1);

    start = text;
    done = FALSE;


    while ( ! done)           /* loop thu local copy */
      {                               /* looking for \n */
      end = start;

      while ((*end != '\0') && (*end != '\n'))  end++;

      if (*end == '\0')
          done = TRUE;                /* we are at the end */

      /* Don't convert empty string unless it's an initial newline. */
      if ((start != end) || (start == text))
          {
          strncpy(buffer, start, end - start);
          buffer[end - start] = '\0';
          if (!string)
              string = XmStringCreate(buffer, XmFONTLIST_DEFAULT_TAG);
          else
              {
              tmp_string = XmStringCreate(buffer, XmFONTLIST_DEFAULT_TAG);
              string = XmStringConcat(string, tmp_string);
              XmStringFree(tmp_string);
              }
          }

      /* Make a separator if this isn't the last segment. */
      if (!done) 
          {
          if (!separator)
              separator = XmStringSeparatorCreate();
          string = XmStringConcat(string, separator);
          start = ++end;              /* start at next char */
          }
      }

    if (separator)
      XmStringFree(separator);

    return (string);
}

#endif /* SIA */


/***************************************************************************
 *
 *  CenterForm
 *
 *  Utility function to center one form horizontally within another.
 ***************************************************************************/

static void 
CenterForm( Widget w1, Widget w2 )
{

    Dimension	width;
    int		i, width1, width2;
   
    XtSetArg(argt[0], XmNwidth,  &width);
    XtGetValues(w1, argt, 1);
    width1 = (int)width;    

    XtSetArg(argt[0], XmNwidth,  &width);
    XtGetValues(w2, argt, 1);
    width2 = (int)width;
    
    i = 0;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_FORM		); i++;
    XtSetArg(argt[i], XmNleftOffset,		(width1 - width2) / 2	); i++;
    XtSetValues(w2,  argt, i);
}



 
/***************************************************************************
 *
 *  CleanupAndExit
 *
 *  close things down gracefully and exit
 ***************************************************************************/

void 
CleanupAndExit( Widget w, int exit_code )
{
    int i;
    Boolean  toggleon;		/* status of session toggle buttons     */
    
#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered CleanupAndExit ...");
#endif /* VG_TRACE */

    if (w != NULL)
      XtDestroyWidget(w);
    
    /*
     *  if user is logging in, set type of session desired. No more than
     *  one session type can be selected at a time (if any) ...
     */

    if ( exit_code == NOTIFY_OK ) {
	XtSetArg(argt[0], XmNset,	&toggleon		);

	XtGetValues(options_failsafe, argt, 1);
	if ( toggleon ) exit_code = NOTIFY_FAILSAFE;

	XtGetValues(options_dtlite, argt, 1);
	if ( toggleon ) exit_code = NOTIFY_DTLITE;


	XtGetValues(options_dt, argt, 1);
	if ( toggleon ) exit_code = NOTIFY_DT;


	if(options_last_dt != NULL) {
	XtGetValues(options_last_dt, argt, 1);
	if ( toggleon ) exit_code = NOTIFY_LAST_DT;
	}

        for(i = 0; i < appInfo.altDts ; ++i) {
	  if(alt_dts[i] != NULL) {
           XtGetValues(alt_dts[i], argt, 1);
	      if ( toggleon ) exit_code = NOTIFY_ALT_DTS + i + 1; /* alt desktops start  at 1 */ 
	   }	

	}

    }

    if (!session_selected)
	exit_code = NOTIFY_OK;

    CloseCatalog();

    ChangeBell("on");
    UnsecureDisplay();
    XSync (dpyinfo.dpy, 0);

    XtCloseDisplay(dpyinfo.dpy);
    exit (exit_code);
}


/***************************************************************************
 *
 *  ClearDtlabel
 *
 ***************************************************************************/

static void
ClearDtlabel(void)
{
    int i;
    XmString xms;

#ifdef VG_TRACE
    vg_TRACE_EXECUTION("ClearDtlabel:  entered ...");
#endif /* VG_TRACE */

    i = 0;
    xms = XmStringCreateLocalized("       ");
    XtSetArg(argt[i], XmNlabelString, xms); i++;
    XtSetValues(dt_label, argt, i);
    XmStringFree(xms);
}


/***************************************************************************
 *
 *  ClearDtlabel
 *
 ***************************************************************************/

static void
ClearDtButtons(void)
{
    int i;

    /*
     *  Clear Dt toggles...
     */
    XtSetArg(argt[0], XmNset, False);

    for (i=0; i<appInfo.altDts; ++i)
      if (alt_dts[i] != NULL)
        XtSetValues(alt_dts[i], argt, 1);

    if (options_failsafe) XtSetValues(options_failsafe, argt, 1);
    if (options_dtlite) XtSetValues(options_dtlite, argt, 1);
    if (options_dt) XtSetValues(options_dt, argt, 1);
    if (options_last_dt) XtSetValues(options_last_dt, argt, 1);
}



 
/***************************************************************************
 *
 *  RespondClearCB
 *
 *  clear name/password text fields
 ***************************************************************************/

void 
RespondClearCB( Widget w, XtPointer client, XtPointer call )
{
  char buf[REQUEST_LIM_MAXLEN];
  ResponseClear *r = (ResponseClear *)buf;

#ifdef VG_TRACE
  vg_TRACE_EXECUTION("main:  entered RespondClearCB ...");
#endif /* VG_TRACE */

  ClearDtlabel();
  ClearDtButtons();
  SetDefaultDt(NULL);

  r->hdr.opcode = REQUEST_OP_CLEAR;
  r->hdr.reserved = 0;
  r->hdr.length = sizeof(*r);

  TellRequester(buf, (size_t) r->hdr.length);
}


 
/***************************************************************************
 *
 *  CopyrightCB
 *
 *  move the highlight back to login or password fields AFTER the copyright
 *  dialog is unposted.
 *
 ***************************************************************************/

void 
CopyrightCB( Widget w, XtPointer client_data, XtPointer call_data )
{

#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered CopyrightCB ...");
#endif /* VG_TRACE */

    if ( focusWidget != NULL)
	ProcessTraversal(focusWidget, XmTRAVERSE_CURRENT);

}



/***************************************************************************
 *
 *  EditPasswdCB
 *
 *  implement no-echo and no-cursor motion of the password
 ***************************************************************************/

void 
EditPasswdCB(Widget w, XtPointer client, XtPointer call_data)
{
    LoginTextPtr	textdata;
    XmTextVerifyPtr	cbs = (XmTextVerifyPtr) call_data;
    int			i;
    static char		buffer[MAXPATHLEN];
    register char	*s, *t;

#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered EditPasswdCB ...");
    vg_TRACE_EXECUTION(
	    "currInsert=%d newInsert=%d startPos=%d endPos=%d\n",
	    cbs->currInsert,cbs->newInsert,cbs->startPos, cbs->endPos);
    if (cbs->text->ptr) vg_TRACE_EXECUTION("text->ptr=%s\n", cbs->text->ptr);
    vg_TRACE_EXECUTION("noechobuf=%s\n", textdata->noechobuf);
#endif /* VG_TRACE */

    textdata = GetLoginTextPtr(w);
    if (NULL == textdata || textdata->bEcho) return;
    if (cbs->reason == XmCR_MOVING_INSERT_CURSOR) return;

    for (i=0, s=buffer, t=textdata->noechobuf;
	 (*t && i<cbs->startPos);
	 i++, s++, t++)
      *s = *t;

    if (cbs->text->ptr)
    {
        strcpy(s, cbs->text->ptr);
	s += cbs->text->length;
    }
    else
      *s = '\0';

    if (strlen(textdata->noechobuf) >= cbs->endPos)
    {
        t = textdata->noechobuf+cbs->endPos;
	if (strlen(t))
          strcpy(s, t);
    }

    strcpy(textdata->noechobuf, buffer);

    if (cbs->text->ptr)
      for (i=0, s=cbs->text->ptr; i<cbs->text->length; i++, s++)
        *s = '*';

#ifdef VG_TRACE
    vg_TRACE_EXECUTION("textdata->noechobuf=%s\n", textdata->noechobuf);
#endif
}


/***************************************************************************
 *
 *  FakeFocusIn
 *
 *  simulate a FocusIn event to the login_shell in order to turn on
 *  traversal. There is a bug in the Toolkit that is normally masked by 
 *  the window manager. Since we have no window manager, we need to simulate
 *  the FocusIn event it sends to the application.
 *
 *  Also force the initial focus to the login_text widget.
 ***************************************************************************/

void
FakeFocusIn( Widget focus_widget, XtPointer client_data, XEvent *eventprm,
             Boolean *continue_to_dispatch )
{
   XEvent event;
   XEvent * eventPtr = &event;

#ifdef VG_TRACE
   vg_TRACE_EXECUTION("main:  entered FakeFocusIn ...");
#endif /* VG_TRACE */

   /*
    * set the input focus to the login text widget...
    */
    
   XSetInputFocus( XtDisplay(focus_widget),
   		   XtWindow(focus_widget),
		   RevertToNone,
		   CurrentTime);

   /*
    *  create a synthetic focus-in event. 
    *
    *  Note: The above call to XSetInputFocus() was not originally included
    *        in this routine. A bug fix to Motif made it necessary to add
    *        the call. The synthetic focus-in event is probably now
    *        unnecessary but is left in for caution's sake. (12/08/92)
    */
    
   /* focus_widget = login_shell; */
   
   eventPtr->type = FocusIn;
   eventPtr->xfocus.serial = LastKnownRequestProcessed(XtDisplay(focus_widget));
   eventPtr->xfocus.send_event = True;
   eventPtr->xfocus.display = XtDisplay(focus_widget);
   eventPtr->xfocus.window = XtWindow(focus_widget);
   eventPtr->xfocus.mode = NotifyNormal;
   eventPtr->xfocus.detail = NotifyAncestor;

   XtDispatchEvent (eventPtr);

   ProcessTraversal(focus_widget, XmTRAVERSE_CURRENT);
   
   XtRemoveEventHandler(focus_widget, ExposureMask, FALSE,
   		   FakeFocusIn, NULL);
}

/***************************************************************************
 *
 *  LayoutCB
 *
 *  do final layout adjustments right before windows are mapped. This is
 *  necessary because the size of managers (Forms, etc) is not known until
 *  their window has been created (XtRealize). We want to make adjustments
 *  before the windows become visible
 *
 *  1. squeeze dialog width to fit on screen.
 *  2. increase dialog height if widgets overlap.
 *  3. center the main matte horizontally and vertically
 *  4. position the pushbuttons
 *  5. position the copyright
 *
 ***************************************************************************/

void
LayoutCB( Widget w, XtPointer client_data, XtPointer call_data )
{
    int	i, j;
    Dimension	width, height;	/* size values returned by XtGetValues	   */
    Dimension	shadowThickness;/* size values returned by XtGetValues	   */
    Position	x, y;		/* position values returned by XtGetValues */
    
    int dpwidth, dpheight;	/* JET - display w/h set according to */
    int xorg, yorg;		/* xinerama usage */

    struct {			/* position, size of widgets (pixels)	   */
    int x, y;
    int	width;
    int height;
    }		mw, pw; /* matte, logo, drop shadow, login matte
    				       and greeting widgets		   */

    int		width1, width2; /* general width variable		   */
    int		height1;	/* general height variable		   */
    Position	x1, y1;		/* general position variables		   */
    int		offsety;	/* general offset variable		   */
    int		shadow_offsetx; /* offset for drop shadow (pixels)	   */
    int		shadow_offsety; /* offset for drop shadow (pixels)	   */
    int		spacing;	/* spacing between login & matte bottoms   */
    
    Widget	buttons[4];	/* pushbutton widgets			   */

    XtWidgetGeometry  geometry;	/* geometry of a widget			   */
    
    int		max_width;	/* maximum width  of a set of widgets	   */
    int		origin;		/* horizontal origin for button placement  */
    int		space;		/* total available space left between buttons */
    int         overlap;        /* possible widget overlap                 */

#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered LayoutCB ...");
#endif /* VG_TRACE */

#ifdef USE_XINERAMA
				/* get info on the prefered screen */
    if (!_DtXineramaGetScreen(dpyinfo.DtXineramaInfo, 
                              appInfo.xineramaPreferredScreen,
			      &dpwidth, &dpheight, &xorg, &yorg))
      {				/* no joy here either - setup for normal */
	dpwidth = dpyinfo.width;
	dpheight = dpyinfo.height;
	xorg = yorg = 0;
      }
#else  /* no Xinerama */
    dpwidth = dpyinfo.width;
    dpheight = dpyinfo.height;
    xorg = yorg = 0;
#endif    

    /*
     * - squeeze dialog to fit onto screen (if necessary)
     */
    i = 0;
    XtSetArg(argt[i], XmNwidth,                 &width                  ); i++;
    XtGetValues(matte, argt, i);
    mw.width  = ToPixel(matte, XmHORIZONTAL, (int)width  );
#define HMARGIN 4 /* min sum horizontal margin of matte */
    if (mw.width+HMARGIN > dpwidth)
    {
      int delta = mw.width + HMARGIN - dpwidth;
     /*
      * Matte width greater than screen so shrink matteFrame
      * and matte1 width to compensate.
      */
      i=0;
      XtSetArg(argt[i], XmNwidth,       &width          ); i++;
      XtGetValues(matteFrame, argt, i);

      width1 = ToPixel(matteFrame, XmHORIZONTAL, (int)width  );
      width1 -= delta;
      width1 = FromPixel(matteFrame, XmHORIZONTAL, width1 );

      i=0;
      XtSetArg(argt[i], XmNwidth,       width1          ); i++;
      XtSetValues(matteFrame, argt, i);

      width1 = dpwidth - HMARGIN;
      mw.width = FromPixel(matte, XmHORIZONTAL, width1 );

      i=0;
      XtSetArg(argt[i], XmNwidth,       mw.width        ); i++;
      XtSetValues(matte, argt, i);
    }

    /*
     * - Make sure the login widgets don't overlap.
     */
    if (login_form) {
	i = 0;
	XtSetArg(argt[i], XmNy,			&y			); i++;
	XtSetArg(argt[i], XmNheight, 		&height			); i++;
	XtGetValues(greeting, argt, i);

	i = 0;
	XtSetArg(argt[i], XmNy,			&y1			); i++;
	XtGetValues(login_form, argt, i);

	overlap = y + height - y1;

	if (overlap > -10) {
            i = 0;
            XtSetArg(argt[i], XmNbottomAttachment, XmATTACH_WIDGET); 	i++;
            XtSetArg(argt[i], XmNbottomWidget,	   login_form); 	i++;
            XtSetArg(argt[i], XmNbottomOffset,	   10);			i++;
            XtSetValues(greeting, argt, i);
	}
    }
 

    /*
     *  - center the main matte horizontally and vertically...
     */
     
    i = 0;
    XtSetArg(argt[i], XmNx,			&x			); i++;
    XtSetArg(argt[i], XmNy,			&y			); i++;
    XtSetArg(argt[i], XmNwidth,			&width			); i++;
    XtSetArg(argt[i], XmNheight,		&height			); i++;
    XtSetArg(argt[i], XmNshadowThickness,	&shadowThickness	); i++;
    XtGetValues(matte, argt, i);

    mw.width  = ToPixel(matte, XmHORIZONTAL, (int)width  );
    mw.height = ToPixel(matte, XmVERTICAL,   (int)height );

    mw.x = ( x > 0 ? ToPixel(matte, XmHORIZONTAL, (int) x)
	    : (dpwidth - mw.width)/2 );
    
    mw.y = ( y > 0 ? ToPixel(matte, XmVERTICAL, (int) y)
	    : (dpheight - mw.height)/2 );
 
    if ( mw.x < 0 ) mw.x = 0;
    if ( mw.y < 0 ) mw.y = 0;

    x1 = FromPixel(matte, XmHORIZONTAL, mw.x );
    y1 = FromPixel(matte, XmVERTICAL,   mw.y );

    x1 += xorg;			/* JET - adjust for xinerama */
    y1 += yorg;


    i = 0;
    XtSetArg(argt[i], XmNx,			x1			); i++;
    XtSetArg(argt[i], XmNy,			y1			); i++;
    XtSetValues(matte, argt, i);


    /*
     * space the buttons horizontally. Start at the center of the matte
     * and allow them to grow towards the edges...
     */

    i = 0;
    XtSetArg(argt[i], XmNwidth,		&width				); i++;
    XtGetValues(matte1, argt, i);

    max_width = width;

    i = 0;
    XtSetArg(argt[i], XmNwidth,		&width				); i++;
    XtGetValues(clear_button, argt, i);
    
    space = max_width - 4*width;
    spacing = space/4;
    
    if (spacing < 12) spacing = 12;

    i = 0;
    XtSetArg(argt[i], XmNleftAttachment, XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNleftOffset, spacing/2); i++;
    XtSetValues(ok_button,  argt, i);

    i = 0;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_WIDGET		); i++;
    XtSetArg(argt[i], XmNleftWidget, ok_button); i++;
    XtSetArg(argt[i], XmNleftOffset, spacing); i++;
    XtSetValues(clear_button,  argt, i);

    i = 0;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_WIDGET		); i++;
    XtSetArg(argt[i], XmNleftWidget, clear_button); i++;
    XtSetArg(argt[i], XmNleftOffset, spacing); i++;
    XtSetValues(options_button,  argt, i);
    
    i = 0;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_WIDGET		); i++;
    XtSetArg(argt[i], XmNleftWidget, options_button); i++;
    XtSetArg(argt[i], XmNleftOffset, spacing); i++;
    XtSetValues(help_button,  argt, i);
    
    /*
     *  - adjust the copyright vertically to align top with login_matte...
     */

    if (copyright_msg) {
	XtQueryGeometry(copyright_msg, NULL, &geometry);
    
	i = 0;
	XtSetArg(argt[i], XmNshadowThickness,	&width			); i++;
	XtGetValues(copyright_msg, argt, i);

	width1 = ToPixel(copyright_msg, XmHORIZONTAL, width);
	width1 = (dpwidth - (int) geometry.width - 2 * width1)/2;

	x1 = FromPixel(copyright_msg, XmHORIZONTAL, width1);
	y1 = FromPixel(copyright_msg, XmVERTICAL, mw.y);

	i = 0;
	XtSetArg(argt[i], XmNdefaultPosition,	False			); i++;
	XtSetArg(argt[i], XmNx,			x1			); i++;
	XtSetArg(argt[i], XmNy,			y1			); i++;
	XtSetValues(copyright_msg, argt, i);
    }
}




/***************************************************************************
 *
 *  MenuItemCB
 *
 *  callback for options menu items
 ***************************************************************************/

void 
MenuItemCB( Widget w, XtPointer client_data, XtPointer call_data )
{
	int i;
    	char     *logoFile;
    	char     *logoName;
	char 	*temp_p;
	char 	temp[MAXPATHLEN];

#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered MenuItemCB ...");
#endif /* VG_TRACE */

    session_selected = True;

    switch ( (long) client_data) {

    case OB_RESTART_SERVER:
	CleanupAndExit(NULL, NOTIFY_RESTART);
	break;

    case OB_NO_WINDOWS:
	CleanupAndExit(NULL, NOTIFY_NO_WINDOWS);
	break;

    case OB_COPYRIGHT:
	_DtShowDialog(copyright, NULL);
	break;

    case OB_ALT_DTS:
    case OB_FAILSAFE:
    case OB_DTLITE:
    case OB_DT: 
    case OB_LAST_DT:
	/*
	 * set the label on the dt_label widget..
	 */ 

	if(w != options_last_dt) {
          XtSetArg(argt[0], XmNlabelString, &xmstr);
          XtGetValues(w, argt, 1);

          XtSetArg(argt[0], XmNlabelString, xmstr);
          XtSetValues(dt_label, argt, 1);
	}
	else
	  ClearDtlabel();

        i = 0;
        XtSetArg(argt[i], XmNuserData,          &logoFile          ); i++;
        XtGetValues(w, argt, i);

        /* 
	 * remove trailing spaces 
	 */
	if(strchr(logoFile,' '))
            temp_p = strtok(logoFile," ");	
	else
	    temp_p = logoFile;
 
        logoName = _DtGetIconFileName(DefaultScreenOfDisplay(dpyinfo.dpy),
                          temp_p, NULL, NULL, 0);
 
        if (logoName == NULL) {
           LogError( 
	     ReadCatalog(MC_LOG_SET,MC_LOG_NO_LOGOBIT,MC_DEF_LOG_NO_LOGOBIT),
                             logoFile);
           logoFile = NULL;
        }
        i = 0;
        XtSetArg(argt[i], XmNimageName, logoName); i++;
        XtSetValues(logo_pixmap, argt, i);

	/*
	 *  Clear Dt toggles...
	 */
	ClearDtButtons();

	/* 
	 * set the selected toggle button...
	 */
        XtSetArg(argt[0], XmNset, True);
        XtSetValues(w, argt, 1);

        SetDefaultDt(w);

	/*
	 *  return focus to name/password widgets...
	 */
	 
	if ( focusWidget != NULL)
	    ProcessTraversal(focusWidget, XmTRAVERSE_CURRENT);

	break;
    }

}




/***************************************************************************
 *
 *  OptionsUnmapCB
 *
 *  callback when options menu unmaps
 ***************************************************************************/

void 
OptionsUnmapCB( Widget wd, XtPointer client_data, XtPointer call_data )
{

    int		i;
    Dimension	width, height;

    Widget	w;
    XEvent	event;
    
#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered OptionsUnmapCB ...");
#endif /* VG_TRACE */

    /*
     *  simulate an exposure event over the Options pushbutton to make sure
     *  the pushbutton elevates. (there is a bug in the toolkit where this
     *  doesn't always happen on some servers)...
     */

    w = options_button;
    
    i = 0;
    XtSetArg(argt[i], XmNwidth,			&width			); i++;
    XtSetArg(argt[i], XmNheight,		&height			); i++;
    XtGetValues(w, argt, i);

    event.type                = Expose;
    event.xexpose.serial      = LastKnownRequestProcessed(XtDisplay(w));
    event.xexpose.send_event  = True;
    event.xexpose.display     = XtDisplay(w);
    event.xexpose.window      = XtWindow(w);
    event.xexpose.x           = 0;
    event.xexpose.y           = 0;
    event.xexpose.width       = 1;   /* one pixel seems to be good enough, */
    event.xexpose.height      = 1;   /* but time will tell...		   */
#if 0
    event.xexpose.width       = ToPixel(matte, XmHORIZONTAL, (int)width  );
    event.xexpose.height      = ToPixel(matte, XmVERTICAL,   (int)height );
#endif
    event.xexpose.count       = 0;
   
    XtDispatchEvent (&event);
}


 
/***************************************************************************
 *
 *  PingServerCB
 *
 *  Ping the server occasionally with an Xsync to see if it is still there.
 *  We do this here rather than in dtlogin since dtgreet has the server
 *  grabbed.
 *
 ***************************************************************************/

static jmp_buf	pingTime;
static int	serverDead = FALSE;
static int	pingInterval = 0;	/* ping interval (sec.)		   */
static int	pingTimeout;		/* ping timeout (sec.)		   */

static void 
PingLost( void )
{
    serverDead = TRUE;
    longjmp (pingTime, 1);
}


static SIGVAL
PingBlocked( int arg )

{
    serverDead = TRUE;
    longjmp (pingTime, 1);
}


int 
PingServer( void )
{
    int	    (*oldError)();
    SIGVAL  (*oldSig)();
    int	    oldAlarm;

    oldError = XSetIOErrorHandler ((XIOErrorHandler)PingLost);
    oldAlarm = alarm (0);
    oldSig = signal (SIGALRM, PingBlocked);
    alarm (pingTimeout * 60);
    if (!setjmp (pingTime))
    {
	XSync (dpyinfo.dpy, 0);
    }
    else
    {
	if ( serverDead ) {
	    LogError(ReadCatalog(
                                 MC_LOG_SET,MC_LOG_DEADSRV,MC_DEF_LOG_DEADSRV),
		dpyinfo.name);
	    alarm (0);
	    signal (SIGALRM, SIG_DFL);
	    XSetIOErrorHandler (oldError);
	    return 0;
	}
    }
    alarm (0);
    signal (SIGALRM, oldSig);
    alarm (oldAlarm);
    XSetIOErrorHandler (oldError);
    return 1;
}


void 
PingServerCB( XtPointer call_data, XtIntervalId *id )
{

    char *t;
    

    /*
     *  get ping values from the environment...
     */

    if ( pingInterval == 0 ) {
	pingInterval  = ((t = (char *)getenv(PINGINTERVAL)) == NULL ? 0 : atoi(t));
	pingTimeout   = ((t = (char *)getenv(PINGTIMEOUT))  == NULL ? 0 : atoi(t));
    }

    
    /*
     *  ping the server. If successful, set a timer for the next ping,
     *  otherwise cleanup and exit...
     */

    if ( pingInterval != 0 ) {
	if (PingServer())
	    XtAddTimeOut((unsigned long) pingInterval * 60 * 1000, 
	    		  PingServerCB, NULL);
	else
	    exit(NOTIFY_RESTART);
    }
}


 
/***************************************************************************
 *
 *  PostMenuCB
 *
 *  post the option_button pop-up menu
 ***************************************************************************/

void 
PostMenuCB( Widget w, XtPointer client_data, XtPointer call_data )
{

    XmAnyCallbackStruct *p;

    p = (XmAnyCallbackStruct *) call_data;

#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered PostMenuCB ...");
#endif /* VG_TRACE */

    /*
     *  make options menus if they don't yet exist...
     */

    if (options_menu == NULL)
	MakeOptionsMenu();


    /*
     *  post menu...
     */

    if (p->reason == XmCR_ARM       	&&
	p->event->type == ButtonPress) {

	XmMenuPosition(options_menu, p->event);
	XtManageChild(options_menu);
    }
}




/***************************************************************************
 *
 *  ProcessTraversal
 *
 *  move the input focus
 ***************************************************************************/

static void 
ProcessTraversal( Widget w, int direction )
{
    int i;

#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered ProcessTraversal ...");
#endif /* VG_TRACE */

    i = XmProcessTraversal(w, direction);

#ifndef __hpux
    /*
     * Versions of Motif other than HP do not support the XmfocusCallback
     * on the TextField widget. We simulate it here by manually invoking the
     * callback routine...
     */

    TextFocusCB(w, NULL, NULL);
#endif    
}



 
/***************************************************************************
 *
 *  RefreshEH
 *
 *  cause the entire screen to refresh via exposure events
 ***************************************************************************/

void 
RefreshEH( Widget w, XtPointer client_data, XEvent *event,
             Boolean *continue_to_dispatch )
{

    Window	cover;
    
#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered RefreshEH ...");
#endif /* VG_TRACE */

    /*
     *  map/unmap a window that covers the entire screen. The resultant
     *  exposure events will refresh the screen. Note, the default
     *  background pixmap is NONE.
     */
     
    cover = XCreateWindow ( dpyinfo.dpy,		/* display	   */
    			    dpyinfo.root,		/* root window ID  */
    			    0,				/* x origin	   */
    			    0,				/* y origin	   */
    			    dpyinfo.width,		/* width	   */
    			    dpyinfo.height,		/* height	   */
			    0,				/* border width	   */
			    0,				/* depth	   */
			    InputOutput,		/* class	   */
			    CopyFromParent,		/* visual	   */
			    0,				/* value mask	   */
			    (XSetWindowAttributes *)NULL); /* attributes   */
    
    XMapWindow(dpyinfo.dpy, cover);
    XDestroyWindow(dpyinfo.dpy, cover);
    XFlush(dpyinfo.dpy);
}


/***************************************************************************
 *
 *  RequestCB
 *
 *  Accept a request from client
 ***************************************************************************/

void 
RequestCB(
        XtPointer client_data,
        int *source,
        XtInputId *id)
{
  char buf[512];
  int count;
  int remainder;
  RequestHeader *phdr = (RequestHeader *)buf;

#ifdef VG_TRACE
  vg_TRACE_EXECUTION("main:  entered RequestCB ...");
#endif /* VG_TRACE */

 /*
  * There's a request in the pipe. Read the header.
  */
  count = read(0, buf, sizeof(*phdr));
  if (count != sizeof(*phdr))
  {
    return;
  }

 /*
  * Calculate amount of data after header.
  */ 
  remainder =  phdr->length - sizeof(*phdr);
  if (remainder > 0)
  {
   /*
    * Read remainder of request.
    */
    count = read(0, buf+sizeof(*phdr), remainder);
  }

 /* 
  * Initiate response to request.
  */
  switch(phdr->opcode)
  {
    case REQUEST_OP_EXIT:
#ifdef VG_TRACE
      vg_TRACE_EXECUTION("main:  got REQUEST_OP_EXIT ...");
#endif /* VG_TRACE */
      RespondExitCB(NULL, NULL, NULL);
      break;

    case REQUEST_OP_MESSAGE:
      {
        RequestMessage *r = (RequestMessage *)phdr;
        XmString string;

#ifdef VG_TRACE
        vg_TRACE_EXECUTION("main:  got REQUEST_OP_MESSAGE ...");
#endif /* VG_TRACE */
        if (r->idMC)
        {
          FILE *fp;

         /*
          * Caller passed in MC_* message id.
          */
          if (r->idMC == MC_NO_LOGIN &&
              (fp = fopen(NO_LOGIN_FILE,"r")) != NULL)
          {
           /*
            * For MC_NO_LOGIN read message from file.
            */
            char buffer[256];
            int j;

            string = NULL;

            while (fgets(buffer, 256, fp) != NULL)
            {
              j = strlen(buffer);
              if ( buffer[j-1] == '\n' ) buffer[j-1] = '\0';

              string = XmStringConcat(xmstr,
                                      XmStringCreate(buffer,
                                      XmFONTLIST_DEFAULT_TAG));
              string = XmStringConcat(xmstr, XmStringSeparatorCreate());
            }

            fclose(fp);
          }
          else
          {
           /*
            * Read message from message catalog.
            */
            string = ReadCatalogXms(MC_ERROR_SET, r->idMC, buf+r->offMessage);
          }
        }
        else
        {
         /*
          * Generate message from provided string. 
          */
#ifdef SIA
          string = multiline_xmstring(buf+r->offMessage);
#else
          string = XmStringCreate(buf+r->offMessage,XmFONTLIST_DEFAULT_TAG);
#endif
        }
      
        _DtShowDialog(error, string);
 
        XmStringFree(string);
      }
      break;

    case REQUEST_OP_HOSTNAME:
#ifdef VG_TRACE
      vg_TRACE_EXECUTION("main:  got REQUEST_OP_HOSTNAME ...");
#endif /* VG_TRACE */
      _DtShowDialog(hostname, NULL);
      break;

    case REQUEST_OP_EXPASSWORD:
#ifdef VG_TRACE
      vg_TRACE_EXECUTION("main:  got REQUEST_OP_EXPASSWORD ...");
#endif /* VG_TRACE */
      _DtShowDialog(expassword, NULL);
      break;

    case REQUEST_OP_CHPASS:
#ifdef VG_TRACE
      vg_TRACE_EXECUTION("main:  got REQUEST_OP_CHPASS ...");
#endif /* VG_TRACE */
      break;

    case REQUEST_OP_CHALLENGE:
      {
        RequestChallenge *r = (RequestChallenge *)phdr;
        XmString string;
        int i;
        LoginTextPtr textdata;
        Boolean change;

#ifdef VG_TRACE
        vg_TRACE_EXECUTION("main:  got REQUEST_OP_CHALLENGE ...");
#endif /* VG_TRACE */
        textdata = GetLoginTextPtr(login_text);

        change = (textdata->bEcho != r->bEcho);

        XtUnmapWidget(textdata->text[textdata->bEcho]);

        textdata->bEcho = r->bEcho;
        textdata->noechobuf[0] = '\0';

        XtAddEventHandler(textdata->text[textdata->bEcho], ExposureMask, False,
                        FakeFocusIn, NULL);

        XtMapWidget(textdata->text[textdata->bEcho]);

        XtPopup(login_shell, XtGrabNone); 

	XGrabKeyboard (dpyinfo.dpy, XtWindow (textdata->text[textdata->bEcho]),
        False, GrabModeAsync, GrabModeAsync, CurrentTime); 

        XmTextFieldSetString(
          textdata->text[textdata->bEcho],
          r->offUserNameSeed ? buf+r->offUserNameSeed : "");

        XmTextFieldSetSelection (
          textdata->text[1],
          0, XmTextFieldGetLastPosition(textdata->text[1]),
          CurrentTime );

        if (r->idMC)
        {
         /*
          * Read message from message catalog.
          */
          string = ReadCatalogXms(MC_LABEL_SET, r->idMC, buf+r->offChallenge);
        }
        else
        {
         /*
          * Generate message from provided string.
          */
          string = XmStringCreate(buf+r->offChallenge,XmFONTLIST_DEFAULT_TAG);
        }

        i = 0;
        XtSetArg(argt[i], XmNlabelString,       string                   ); i++;
        XtSetValues(login_label, argt, i);

        XmStringFree(string); 

        if (change)
        {
           char buf[256];

           i = 0;
           if (textdata->bEcho)
           {
             XtSetArg(argt[i], XmNlabelString, textdata->onGreeting     ); i++;
           }
           else
           {
             sprintf(buf, textdata->offGreetingFormat,
                     textdata->offGreetingUname);
             string = XmStringCreate(buf, XmFONTLIST_DEFAULT_TAG);
             XtSetArg(argt[i], XmNlabelString, string    ); i++;
             free(textdata->offGreetingUname);
           }
           XtSetValues(greeting, argt, i);
	   /* set the dt_label with the session that is enabled */
  	   SetDtLabelAndIcon();

        }

        XtSetSensitive(ok_button, True);
        XtSetSensitive(clear_button, True);
        XtSetSensitive(options_button, True);
        XtSetSensitive(help_button, True);

	XUngrabKeyboard(dpyinfo.dpy,CurrentTime);
      }
      break;
#ifdef SIA
    case REQUEST_OP_FORM:
      {
      RequestForm *r = (RequestForm *)buf;
      int i;
      char *prompt_ptr;

#ifdef VG_TRACE
      vg_TRACE_EXECUTION("main:  got REQUEST_OP_FORM ...");
#endif /* VG_TRACE */
      siaFormInfo.num_prompts = r->num_prompts;
      siaFormInfo.rendition = r->rendition;
      siaFormInfo.title = XtMalloc(strlen(buf + r->offTitle));
      strcpy(siaFormInfo.title, buf + r->offTitle);

      prompt_ptr = buf + r->offPrompts;

      for (i=0; i < siaFormInfo.num_prompts; i++)
          {
          siaFormInfo.visible[i] = r->visible[i];
          siaFormInfo.prompts[i] = XtMalloc(strlen(prompt_ptr));
          siaFormInfo.answers[i] = NULL;
          strcpy(siaFormInfo.prompts[i], prompt_ptr);
          prompt_ptr += strlen(prompt_ptr) + 1;
          }

      /*
       * Create Widgets:
       *  Form
       *   Title
       *   prompt labels and answer text fields for each prompt
       *   OK button
       * Add callbacks as needed.  If not visible don't echo.
       * On OK callback, collect info and send it.  Destroy widgets.
       */
       SiaForm(&siaFormInfo);

      }
      break;
#endif /* SIA */
  }


#if 0
  {
   /*
    * Send immediate response to debug.
    */
    char outbuf[512];
    char *p;
    ResponseDebug *rdebug = (ResponseDebug *)outbuf;

    rdebug->hdr.opcode = REQUEST_OP_DEBUG;
    rdebug->hdr.reserved = 0;
    rdebug->offString = sizeof(*rdebug);
    p = ((char *)(rdebug)) + rdebug->offString;
    strcpy(p, "This is my debug string");
    rdebug->hdr.length = sizeof(*rdebug) + strlen(p) + 1;
    
    TellRequester(outbuf, (size_t) rdebug->hdr.length);
  } 
#endif
}

/***************************************************************************
 *
 *  RespondExitCB
 *
 *  Respond to an exit request from client
 ***************************************************************************/

void 
RespondExitCB(
        Widget w,
        XtPointer client,
        XtPointer call)
{
  char buf[REQUEST_LIM_MAXLEN];
  ResponseExit *r = (ResponseExit *)buf;

#ifdef VG_TRACE
  vg_TRACE_EXECUTION("main:  entered RespondExitCB ...");
#endif /* VG_TRACE */

  r->hdr.opcode = REQUEST_OP_EXIT;
  r->hdr.reserved = 0;
  r->hdr.length = sizeof(*r);

  TellRequester(buf, (size_t) r->hdr.length);

  CleanupAndExit(NULL, NOTIFY_OK);
}
/***************************************************************************
 *
 *  RespondLangCB
 *
 *  Respond to a lang item selection from user
 ***************************************************************************/
int amChooser = 0;
int orig_argc;
char **orig_argv;

void
RespondLangCB( Widget w, XtPointer client, XtPointer call)
{
  XtSetArg(argt[0], XmNset, True);
  XtSetValues(w, argt, 1);

#ifdef VG_TRACE
  vg_TRACE_EXECUTION("main:  entered RespondLangCB ...");
#endif /* VG_TRACE */

  if (amChooser) {
	/** this is probably not the ideal way to do this **/
	/** but it does work ok.                          **/
	/** notice the related code in chooser.c at the   **/
	/** beginning of main.                            **/
	char buff[128];
	if (XmToggleButtonGadgetGetState(w)) {
	    strcpy(buff, "LANG="); 
	    strcat(buff, client);
	    putenv(buff);
	    execv(orig_argv[0], orig_argv);
	}
  } else {
    char buf[REQUEST_LIM_MAXLEN];
    ResponseLang *r = (ResponseLang *)buf;
    char *p;
  
    r->hdr.opcode = REQUEST_OP_LANG;
    r->hdr.reserved = 0;
    r->offLang = sizeof(*r);
    p = ((char *)(r)) + r->offLang;
    strcpy(p, XmToggleButtonGadgetGetState(w) ? client : "default");
    r->hdr.length = sizeof(*r) + strlen(p) + 1;

    TellRequester(buf, (size_t) r->hdr.length);
 
    CleanupAndExit(NULL, NOTIFY_LANG_CHANGE);
  }
}

/***************************************************************************
 *
 *  RespondChallengeCB
 *
 *  Respond to a challenge request from client
 ***************************************************************************/

void 
RespondChallengeCB(
        Widget w,
        XtPointer client,
        XtPointer call)
{
  char buf[REQUEST_LIM_MAXLEN];
  ResponseChallenge *r = (ResponseChallenge *)buf;
  char *value;
  char *p;
  LoginTextPtr textdata;

#ifdef VG_TRACE
  vg_TRACE_EXECUTION("main:  entered RespondChallengeCB ...");
#endif /* VG_TRACE */

  XtSetSensitive(ok_button, False);
  XtSetSensitive(clear_button, False);
  XtSetSensitive(options_button, False);
  XtSetSensitive(help_button, False);

  textdata = GetLoginTextPtr(login_text);

 /*
  * Get username and password. Username is obtained from widget
  * while password is stored in global buffer.
  */
  if (textdata->bEcho)
  {
    value = XmTextFieldGetString(login_text);
    textdata->offGreetingUname = strdup(value);
    userName = strdup(value);
    if (strlen(textdata->offGreetingUname) > (size_t) 16)
    {
      textdata->offGreetingUname[16] = '\0';
      userName[16] = '\0';
    }
  }
  else
  {
    value = textdata->noechobuf;
  }

  r->hdr.opcode = REQUEST_OP_CHALLENGE;
  r->hdr.reserved = 0;
  r->offResponse = sizeof(*r);
  p = buf + r->offResponse;
  strcpy(p, value);
  r->hdr.length = r->offResponse + strlen(p) + 1; 

  if (textdata->bEcho)
  {
    XtFree(value);
    XmTextFieldSetString(login_text, "");
  }
  else
  {
   /*
    * Clean password memory to foil snoopers.
    */
    bzero(textdata->noechobuf, strlen(textdata->noechobuf));
  }

  TellRequester(buf, (size_t) r->hdr.length);
}

/***************************************************************************
 *
 *  RespondDialogCB
 *
 *  Respond to a request that displayed a dialog
 ***************************************************************************/

void 
RespondDialogCB(
        Widget w,
        XtPointer client,
        XtPointer call_data)
{
  char buf[REQUEST_LIM_MAXLEN];
  XmAnyCallbackStruct *reason = (XmAnyCallbackStruct *)call_data;

#ifdef VG_TRACE
  vg_TRACE_EXECUTION("main:  entered RespondDialogCB ...");
#endif /* VG_TRACE */

  if (w == error_message)
  {
  ResponseMessage *r= (ResponseMessage *)buf;

  r->hdr.opcode = REQUEST_OP_MESSAGE;
  r->hdr.reserved = 0;
  r->hdr.length = sizeof(*r);

  TellRequester(buf, (size_t) r->hdr.length);
  } 
  else if (w == passwd_message)
  {
    if (reason->reason == XmCR_OK)
    {
      CleanupAndExit(w, NOTIFY_PASSWD_EXPIRED);
    } 
    else
    {
  ResponseExpassword *r= (ResponseExpassword *)buf;

  r->hdr.opcode = REQUEST_OP_EXPASSWORD;
  r->hdr.reserved = 0;
  r->hdr.length = sizeof(*r);

  TellRequester(buf, (size_t) r->hdr.length);
    }
  }
  else if (w == hostname_message)
  {
    if (reason->reason == XmCR_OK)
    {
      CleanupAndExit(w, NOTIFY_OK);
    } 
    else
    {
      CleanupAndExit(w, NOTIFY_NO_WINDOWS);
    }
  }
}

/***************************************************************************
 *
 *  SetDefaultDt
 *
 *  save the default dt widget.
 **************************************************************************/
void
SetDefaultDt(Widget w)
{
    default_dt = w;
}

/***************************************************************************
 *
 *  SetDtLabelAndIcon
 *
 *  label to display in the dt_label widget and 
 *  the logo to display in logo_pixmap 
 **************************************************************************/
void
SetDtLabelAndIcon()
{
  static XmString	blanks = NULL;
  int			i;
  char           	*logoFile;
  char           	*logoName;
  char 			*temp_p;
   
#ifdef VG_TRACE
  vg_TRACE_EXECUTION("main:  entered SetDtLabelAndIcon ...");
#endif /* VG_TRACE */

  if (NULL == blanks)
    blanks = XmStringCreateLocalized("       ");

  /*
   *  if user is logging in, set dt_label desired. No more than
   *  one desktop  can be selected at a time ...
   */
  i = 0;
  XtSetArg(argt[0], XmNset, FALSE); i++;
 
  XtSetValues(options_failsafe, argt, i);
  XtSetValues(options_dtlite, argt, i);
  XtSetValues(options_dt, argt, i);
  if (options_last_dt != NULL)
    XtSetValues(options_last_dt, argt, i);
 
  for(i = 0; i<appInfo.altDts ; ++i)
    if(alt_dts[i] != NULL)
      XtSetValues(alt_dts[i], argt, i);

  if (NULL != default_dt) {
      i = 0;
      XtSetArg(argt[i], XmNset, True); i++;
      XtSetValues(default_dt, argt, i);

      if(default_dt == options_last_dt)
        xmstr = blanks;
      else
      {
         i = 0;
         XtSetArg(argt[i], XmNlabelString, &xmstr); i++;
         XtGetValues(default_dt, argt, i);  
      }
      
      i = 0;
      XtSetArg(argt[i], XmNuserData, &logoFile); i++;
      XtGetValues(default_dt, argt, i);
  }
  else {
      xmstr = blanks;
      logoFile = logoInfo.bitmapFile;
  }

#ifdef FIX_FOR_DEFECT_CDExc19307
  /*
   *  This code causes the Session toggle buttons to be reset
   *  to the default setting when the user commits a typing error.
   *  NOT very user friendly.
   *
   *  As for setting the defaults after the user is done entering
   *  the we could do the same thing by passing in an argument.  For
   *  now, I'm not convinced that it is required.
   */

  /*
   * to set the defaults  after the user is done			 
   * entering the login .. 
   */
   if (strcmp(userName,"\0") != 0)
     strcpy(userName,"\0");  
#endif

  i = 0;
  XtSetArg(argt[i], XmNlabelString, xmstr); i++;
  XtSetValues(dt_label, argt, i); 

  /* 
   * remove trailing spaces 
   */
  if(strchr(logoFile,' '))
     temp_p = strtok(logoFile," ");
  else
     temp_p = logoFile;
 
  logoName = _DtGetIconFileName(
			DefaultScreenOfDisplay(dpyinfo.dpy),
                        temp_p, NULL, NULL, 0);
 
  if (logoName == NULL) {
        LogError(
	ReadCatalog(MC_LOG_SET,MC_LOG_NO_LOGOBIT,MC_DEF_LOG_NO_LOGOBIT),
                             logoFile);
        logoFile = NULL;
  }

  i = 0;
  XtSetArg(argt[i], XmNimageName, logoName); i++;
  XtSetValues(logo_pixmap, argt, i);
}

/***************************************************************************
 *
 *  _DtShowDialog
 *
 *  display a dialog message box
 ***************************************************************************/

static void 
_DtShowDialog( DialogType dtype, XmString msg)
{
    Widget *w;
    
#ifdef VG_TRACE
    vg_TRACE_EXECUTION("main:  entered _DtShowDialog ...");
#endif /* VG_TRACE */

    switch (dtype) {
      case copyright:  w = &copyright_msg; break;
      case error:      w = &error_message; break;
      case expassword: w = &passwd_message; break;
      case help:       w = &help_message; break;
      case hostname:   w = &hostname_message; break;
      case help_chooser: w = &help_message; break;
    }

    if (*w == NULL)
    {
      MakeDialog(dtype);
    }
    
    if ( msg != NULL ) {
	XtSetArg(argt[0], XmNmessageString, msg	);
	XtSetValues(*w, argt, 1);
    }

    XtManageChild(*w);

    XSetInputFocus(XtDisplay(*w), XtWindow(*w), 
    		   RevertToPointerRoot, CurrentTime);
}    




/***************************************************************************
 *
 *  ShowDialogCB
 *
 *  callback to display a dialog message box
 ***************************************************************************/

void 
ShowDialogCB( Widget w, XtPointer dialog, XtPointer call_data )
{

    _DtShowDialog( (DialogType) dialog, NULL);
}




/***************************************************************************
 *
 *  TextFocusCB
 *
 *  set focusWidget global variable when focus changes
 ***************************************************************************/


void 
TextFocusCB( Widget w, XtPointer client_data, XtPointer call_data )
{
    if ( w == login_text )
	focusWidget = w;
}

/***************************************************************************
 *
 *  GetLoginTextPtr
 *
 *  Return login text widget instance data
 ***************************************************************************/

LoginTextPtr
GetLoginTextPtr( Widget w )
{
  LoginTextPtr textdata;
  int i;

  i = 0;
  XtSetArg(argt[i], XmNuserData,     &textdata              ); i++;
  XtGetValues(w, argt, i);

  return(textdata);
}


#ifdef BLS
/***************************************************************************
 *
 *  PromptSensitivityLevel
 *
 *  Prompt for B1 Sensitivity Level. The password widget set is reused for
 *  this purpose rather than creating another complete widget set. It already
 *  has most of the proper size and alignment specifications needed. Using
 *  the password set also allows the B1 code changes to be more localized.
 *
 ***************************************************************************/

static void 
PromptSensitivityLevel( void)
{

    Dimension	width;
    int		i, width1, width2;

    /*
     *  Use the password widget set to prompt for the B1 Sensitivity Level.
     *  Remember to put it back to normal if the user presses [Clear].
     */
     
    normalPasswordWidget = False;
    
    XtRemoveAllCallbacks(_text, XmNmodifyVerifyCallback);
    XmTextFieldSetString(_text,"");


    /*
     *  Change the label and resize the password form...
     */
     
    i = 0;
    XtSetArg(argt[i], XmNresizable,		True			); i++;
    XtSetArg(argt[i], XmNresizePolicy,		XmRESIZE_ANY		); i++;
    XtSetValues(_form, argt, i);

    i = 0;
    xmstr = ReadCatalogXms(MC_LABEL_SET, -1, "Sensitivity Level:");
    XtSetArg(argt[i], XmNrecomputeSize,		True			); i++;
    XtSetArg(argt[i], XmNlabelString,		xmstr			); i++;
    XtSetValues(_label, argt, i);

    XmStringFree(xmstr);
    
    /*
     *  Center the form horizontally in the login_matte...
     *
     */

    CenterForm(matte1, _form);
    
    ProcessTraversal(_text, XmTRAVERSE_CURRENT);

}

#endif /* BLS */


static void
TellRequester(char * buf, size_t nbytes)
{
#ifdef VG_TRACE
  vg_TRACE_EXECUTION("main:  entered TellRequester ...");
#endif /* VG_TRACE */
  if(-1 == write(1, buf, nbytes)) {
    perror(strerror(errno));
  }
}
