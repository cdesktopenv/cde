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
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

/*
 * $TOG: QueryTermView.C /main/13 1998/04/17 11:39:35 mgreess $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#define C_TOC_Element
#define L_Basic

#define C_QueryTermView
#define C_QueryGroupView
#define C_QueryTerm
#define C_QueryEditor
#define L_Query

#define C_WindowSystem
#define L_Other

#include "Prelude.h"

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

#include "UAS_OQLParser.hh"

#include <ctype.h>
#include <WWL/WXmMenu.h>
#include <WWL/WXmPushButton.h>

#include <stdlib.h>
#if defined(SVR4) || defined(SYSV) || defined(linux) || defined(CSRG_BASED)
#include <limits.h>
#endif

#define CLASS QueryTermView

#include "../Agents/create_macros.hh"

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

QueryTermView::QueryTermView (QueryTerm *query_term, QueryGroupView *parent,
			      QueryTermView *previous, QueryTermView *next)
: f_query_term (query_term), f_form (*parent), 
#ifdef UseQSearch
  f_pws_form((Widget) NULL), f_weight_field(NULL), f_weight_label((Widget)True),
#else
  f_pws_form ((Widget) NULL), f_proximity_label ((Widget) False),
#endif
  f_parent (parent), f_previous (previous), f_next (next),
  f_pw_button((Widget) NULL)
{
    f_group_view = NULL;

  // Link terms into the list. 
  if (previous != NULL)
    f_previous->f_next = this;
  else
    parent->f_term_view_list = this;
  if (next != NULL)
    f_next->f_previous = this;

  query_editor().increment_null_terms();

  // Duh. 
  create_ui();
#ifndef UseQSearch
  // Watch the previous term for changes to the connective so this one
  // can muck with the sensitivity of the proximity field.
  if (previous)
    {
      Observe (previous->query_term(), QueryTerm::CONNECTIVE_CHANGED,
	       &QueryTermView::prev_connective_changed);
    }
#endif
}

  
// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

QueryTermView::~QueryTermView()
{
  // Unlink from list
  if (f_previous != NULL)
    f_previous->f_next = f_next;
  else
    f_parent->f_term_view_list = f_next;
  if (f_next != NULL)
    f_next->f_previous = f_previous;

  // Notify editor if NULL term deleted.
  if (*(f_query_term->term_string()) == '\0')
    query_editor().decrement_null_terms();

  // Do form detachments here!! 

  // If term is a group object, delete it.
  if (term_type() == TYPE_GROUP)
    delete f_group_view;
}


// /////////////////////////////////////////////////////////////////
// create_ui - create the user interface
// /////////////////////////////////////////////////////////////////

void
QueryTermView::create_ui()
{
  Arg args[1];
  int n;

  XmStringLocalized mtfstring;

  register_actions();

  // f_form is used to hold the parent until the form is created. 
  ASSN  (WXmForm,         f_form,           f_form,       "qterm_view"      );
  // What is this widget used for? - 6/28/94 kamiya
  ASSN  (WXmToggleButton, f_select_toggle,  f_form,       "select"          );
  DECLC (WXmPulldownMenu, prefix_menu,      f_form,       "prefix_menu"     );

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, (Widget) prefix_menu); n++;
  f_prefix = WXmOptionMenu    (f_form, (char*)"prefix", WAutoManage, args, n);

  DECLM (WXmPushButtonGadget, contains,     prefix_menu,  "contains"        );
  DECLM (WXmPushButtonGadget, not_contains, prefix_menu,  "not_contains"    );

  mtfstring = CATGETS(Set_AgentLabel, 232, "Contains");
  XtVaSetValues(contains, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 233, "Doesn\'t Contain");
  XtVaSetValues(not_contains, XmNlabelString, (XmString)mtfstring, NULL);

  unsigned int caps = f_query_term->avail_caps();
  WXmPushButtonGadget starts     = (Widget)NULL;
  WXmPushButtonGadget not_starts = (Widget)NULL;
  if (caps & (0x01 << UAS_OQLParser::OQL_COMPLETION)) {
    ASSNM (WXmPushButtonGadget, starts,     prefix_menu,  "starts_with"     );
    ASSNM (WXmPushButtonGadget, not_starts, prefix_menu,  "not_starts_with" );

    mtfstring = CATGETS(Set_AgentLabel, 234, "Starts With");
    XtVaSetValues(starts, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring = CATGETS(Set_AgentLabel, 235, "Doesn\'t Start With");
    XtVaSetValues(not_starts, XmNlabelString, (XmString)mtfstring, NULL);
  }
  DECLC (WXmPulldownMenu, connect_menu,     f_form,       "connect_menu"    );

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, (Widget) connect_menu); n++;
  f_connective = WXmOptionMenu (f_form, (char*)"connective", WAutoManage, args, n);

  DECLM (WXmPushButtonGadget,   none,       connect_menu, " "               );
  DECLM (WXmPushButtonGadget,   orgad,      connect_menu, "or"              );
  WXmPushButtonGadget xorgad = (Widget)NULL;
  if (caps & (0x01 << UAS_OQLParser::OQL_XOR)) {
    ASSNM (WXmPushButtonGadget,   xorgad,        connect_menu, "xor"           );

    mtfstring = CATGETS(Set_AgentLabel, 237, "Xor");
    XtVaSetValues(xorgad, XmNlabelString, (XmString)mtfstring, NULL);
  }
  DECLM (WXmPushButtonGadget,   andgad,     connect_menu, "and"             );
  WXmPushButtonGadget near   = (Widget)NULL;
  WXmPushButtonGadget before = (Widget)NULL;
  if (caps & (0x01 << UAS_OQLParser::OQL_NEAR)) {
    ASSNM (WXmPushButtonGadget,   near,       connect_menu, "near"          );

    mtfstring = CATGETS(Set_AgentLabel, 239, "Near");
    XtVaSetValues(near, XmNlabelString, (XmString)mtfstring, NULL);
  }
  if (caps & (0x01 << UAS_OQLParser::OQL_BEFORE)) {
    ASSNM (WXmPushButtonGadget,   before,     connect_menu, "before"        );

    mtfstring = CATGETS(Set_AgentLabel, 240, "Before");
    XtVaSetValues(before, XmNlabelString, (XmString)mtfstring, NULL);
  }
  ASSNM (WXmArrowButton,        f_pw_button,f_form,       "pw_button"     );
  f_term_field = (WXmTextField *)(Widget)
    WXmTextField (f_form, "term_text", WAutoManage);
//  ASSNM (WXmTextField,    f_term_field,    f_form,       "term_text");

  mtfstring = CATGETS(Set_AgentLabel, 236, "Or");
  XtVaSetValues(orgad, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 238, "And");
  XtVaSetValues(andgad, XmNlabelString, (XmString)mtfstring, NULL);

  // Callbacks!
#ifdef UseQSearch
  ON_ACTIVATE  (f_pw_button,pw_activate);
  ON_ACTIVATE_D (contains, set_prefix, QueryTerm::PFX_CONTAIN);
  ON_ACTIVATE_D (not_contains, set_prefix, QueryTerm::PFX_NOT_CONTAIN);
  ON_ACTIVATE_D (orgad, set_connective, C_OR);
  ON_ACTIVATE_D (xorgad, set_connective, C_XOR);
  ON_ACTIVATE_D (andgad, set_connective, C_AND);
  SET_CALLBACK (*f_term_field,ValueChanged,term_changed);
  SET_CALLBACK (*f_term_field,ModifyVerify,verify_term);
#else
  // What is this widget used for? - 6/28/94 kamiya
  SET_CALLBACK (f_select_toggle,ValueChanged,select_toggle);
  if ((caps & (0x01 << UAS_OQLParser::OQL_WEIGHT)) ||
      (caps & (0x01 << UAS_OQLParser::OQL_NEAR_WITHIN))||
      (caps & (0x01 << UAS_OQLParser::OQL_BEFORE_WITHIN)))
    ON_ACTIVATE  (f_pw_button,pw_activate);
  else
    //f_pw_button.SetSensitive(False);
    f_pw_button.MappedWhenManaged(False);
  ON_ACTIVATE_D (contains, set_prefix, QueryTerm::PFX_CONTAIN);
  ON_ACTIVATE_D (not_contains, set_prefix, QueryTerm::PFX_NOT_CONTAIN);
  if (caps & (0x01 << UAS_OQLParser::OQL_COMPLETION)) {
    ON_ACTIVATE_D (starts, set_prefix, QueryTerm::PFX_COMPLETE);
    ON_ACTIVATE_D (not_starts, set_prefix, QueryTerm::PFX_NOT_COMPLETE);
  }
  ON_ACTIVATE_D (orgad, set_connective, C_OR);
  if (xorgad)
    ON_ACTIVATE_D (xorgad, set_connective, C_XOR);
  ON_ACTIVATE_D (andgad, set_connective, C_AND);
  if (caps & (0x01 << UAS_OQLParser::OQL_NEAR))
    ON_ACTIVATE_D (near, set_connective, C_NEAR);
  if (caps & (0x01 << UAS_OQLParser::OQL_BEFORE))
    ON_ACTIVATE_D (before, set_connective, C_BEFORE);
  SET_CALLBACK (*f_term_field,ValueChanged,term_changed);
  SET_CALLBACK (*f_term_field,ModifyVerify,verify_term);
#endif

  // Menu attachments. 
  f_prefix.SubMenuId (prefix_menu);
  f_prefix.OptionLabelGadget().Unmanage();
  f_connective.SubMenuId (connect_menu);
  f_connective.OptionLabelGadget().Unmanage();

  if (XtIsRealized (f_form.Parent()))
    f_form.Realize();  // Force geometry computations. 
  f_form.Manage();

  // Set up form attachments.
  if (f_previous != NULL)
    {
      ON_DEBUG(puts ("setting up previous attachments"));
      f_form.
	Set (WArgList (XmNtopAttachment, XmATTACH_WIDGET,
		       XmNtopWidget, (Widget) f_previous->f_form, NULL,
		       XmNtopOffset, 4, NULL));
    }
  else
    {
      ON_DEBUG(puts ("setting top to form"));
      f_form.Set (WArgList (XmNtopAttachment, XmATTACH_FORM,
			    XmNtopOffset, 1, NULL));
    }
  // Might want to set these when created to avoid weird visuals. 
  if (f_next != NULL)
    {
      ON_DEBUG(puts ("setting up next attachments"));
      f_next->f_form.Set (WArgList (XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, (Widget) f_form,
				    NULL));
    }
  else
    {
      ON_DEBUG(puts ("not setting up next form attachments"));
      //  f_form.BottomAttachment (XmATTACH_FORM);
    }

  traverse_here();

/*  XtUnmanageChild (XtParent(f_form));
    XtManageChild (XtParent(f_form)); */
}


// /////////////////////////////////////////////////////////////////
// select_toggle - select/deselect the term
// /////////////////////////////////////////////////////////////////

void
QueryTermView::select_toggle (WCallback *wcb)
{
  XmToggleButtonCallbackStruct *tbcs =
    (XmToggleButtonCallbackStruct *) wcb->CallData();

  // notify someone about selection state...
  invert_colors();
}


// /////////////////////////////////////////////////////////////////
// _select - beep if insensitve, call normal select action otherwise
// /////////////////////////////////////////////////////////////////

void
QueryTermView::_select (Widget w, XEvent *event,
			String *params, Cardinal *num_params)
{
  WXmPrimitive W (w);
  // If we end changing the selection, call the real proc.
  if (W.Sensitive())
    XtCallActionProc (w, "Select", event, params, *num_params);
  else
    // beep
    ;
}


// /////////////////////////////////////////////////////////////////
// register_actions
// /////////////////////////////////////////////////////////////////

void
QueryTermView::register_actions()
{
  static bool registered = FALSE;
  if (registered)
    return;

  static XtActionsRec actions_list[] =
  {
    { (char*)"TermSelect", QueryTermView::_select },
  };

  XtAppAddActions (window_system().app_context(),
		   actions_list, XtNumber (actions_list));
}


// /////////////////////////////////////////////////////////////////
// invert_colors - invert fg and bg to indicate selection
// /////////////////////////////////////////////////////////////////

#define SET_COLORS(W) \
  XtVaSetValues (W, \
		 XmNbackground, foreground, \
		 XmNforeground, background, \
		 XmNselectColor, select_color, \
		 XmNtopShadowColor, top_shadow, \
		 XmNbottomShadowColor, bottom_shadow, \
		 NULL);

// TODO: set colors for text fields seperately (for when bg is different)

void
QueryTermView::invert_colors()
{
  Colormap colormap;
  Pixel background, foreground, top_shadow, bottom_shadow, select_color;
  WidgetList children = f_form.Children();
  int i, num = f_form.NumChildren();

  XtVaGetValues (f_form,
		 XmNcolormap, &colormap,
		 XmNbackground, &background,
		 XmNforeground, &foreground,
		 NULL);

  XmGetColors (XtScreen (f_form), colormap, foreground,
	       NULL, &top_shadow, &bottom_shadow, &select_color);

  SET_COLORS (f_form);

  for (i = 0; i < num; i++)
    SET_COLORS (children[i]);

  if (f_weight_field != 0)
    {
      SET_COLORS (f_weight_field);
#ifndef UseQSearch
      SET_COLORS (f_proximity_field);
      SET_COLORS (f_scope_field);
#endif
    }
}


// /////////////////////////////////////////////////////////////////
// set_prefix - change the prefix for this term
// /////////////////////////////////////////////////////////////////

void
QueryTermView::set_prefix (WCallback *wcb)
{
  f_query_term->prefix ((size_t) wcb->ClientData());
#ifdef UseQSearch
  if (f_weight_field)
	if ( (int)wcb->ClientData() == QueryTerm::PFX_CONTAIN) {
	    f_weight_label.SetSensitive (True);
	    f_weight_field.SetSensitive (True);
	}
	else {
	    assert( (int)wcb->ClientData() == QueryTerm::PFX_NOT_CONTAIN );
	    f_weight_label.SetSensitive (False);
	    f_weight_field.SetSensitive (False);
	}
  else
	if ( (int)wcb->ClientData() == QueryTerm::PFX_CONTAIN)
	    f_weight_label = (Widget)True;
	else {
	    assert( (int)wcb->ClientData() == QueryTerm::PFX_NOT_CONTAIN );
	    f_weight_label = (Widget) False;
	}
#endif
  query_editor().query_changed();
}


// /////////////////////////////////////////////////////////////////
// set_connective - change the connective for this term
// /////////////////////////////////////////////////////////////////

void
QueryTermView::set_connective (WCallback *wcb)
{
  // When the connective changes, get rid of the "none" entry. 
  XtUnmanageChild (WComposite(f_connective.SubMenuId()).Children()[0]);
  // Create a new one to follow this if necessary.
  if (f_query_term->connective() == C_NONE)
    {
      // For now we create the term and the view here because
      // there's only on editor (view) which means we don't have
      // to resort to notifications.  08:03 12/15/92 DJB
      QueryTerm *term =
	new QueryTerm (f_parent->query_group(),
		       f_query_term, f_query_term->next());
      new QueryTermView (term, f_parent, this, f_next);
    }
  // Set it in the term. 
  f_query_term->connective ((size_t) wcb->ClientData());
  query_editor().query_changed();
}


// /////////////////////////////////////////////////////////////////
// pw_activate - activate the pw button
// /////////////////////////////////////////////////////////////////

void
QueryTermView::pw_activate (WCallback *)
{
  if (f_pws_form == 0)
    create_fields();

  // change attachments of some widgets - 6/28/94 kamiya
  if (f_pw_button.ArrowDirection() == XmARROW_RIGHT)
    {
      // hook the text field to the connective so we can move the toggle
#ifdef GONE
      f_term_field->Set
	(WArgList (XmNrightAttachment, XmATTACH_WIDGET,
		   XmNrightWidget, (Widget) f_connective,
		   NULL));
      f_pw_button.WObject::Set
	(WArgList (XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
		   XmNleftWidget, (Widget) f_prefix,
		   XmNtopAttachment, XmATTACH_WIDGET,
		   XmNtopWidget, (Widget) f_prefix,
		   XmNrightAttachment, XmATTACH_NONE,
		   XmNarrowDirection, XmARROW_DOWN,
		   XmNtopOffset, 9,
		   NULL));
#endif
      f_pw_button.WObject::Set
	(WArgList (XmNrightAttachment, XmATTACH_NONE,
		   XmNarrowDirection, XmARROW_DOWN,
		   NULL));
      f_pws_form.Manage();
      f_connective.Set (WArgList (XmNbottomAttachment, XmATTACH_FORM,
				  XmNtopAttachment, XmATTACH_NONE,
				  NULL));
    }
  else
    {
      f_connective.Set (WArgList (XmNbottomAttachment, XmATTACH_NONE,
				  XmNtopAttachment, XmATTACH_FORM,
				  NULL));
      f_pws_form.Unmanage();
      f_pw_button.WObject::Set
	(WArgList (XmNrightAttachment, XmATTACH_WIDGET,
		   XmNrightWidget, (Widget) f_connective,
		   XmNarrowDirection, XmARROW_RIGHT,
		   NULL));
#ifdef GONE
      f_pw_button.WObject::Set
	(WArgList (XmNleftAttachment, XmATTACH_NONE,
		   XmNtopAttachment, XmATTACH_FORM,
		   XmNrightAttachment, XmATTACH_WIDGET,
		   XmNrightWidget, (Widget) f_connective,
		   XmNtopOffset, 8,
		   XmNarrowDirection, XmARROW_RIGHT,
		   NULL));

      f_term_field->Set
	(WArgList (XmNrightAttachment, XmATTACH_WIDGET,
		   XmNrightWidget, (Widget) f_pw_button,
		   XmNwidth, 5,
		   NULL));
#endif

      /* Setting the term_field width to 5 here is essential to
	 work-around a Motif 1.2.2 Form bug.  When the PWS thing
	 is expanded, the Form thinks that the term_field's
	 preferred width should get bigger (WRONG).  Later when
	 the PWS is contracted, the Form tries to maintain this
	 "fake" preferred width and wants to increase its width
	 to do so.  By setting the width to 5 here, we reset the
	 preferred width and prevent the width increase request.
	 We cannot allow the Form to request a width increates,
	 because when it is denied, the Form bails on the height
	 calculations (yet another bug).  */
    }
}

#ifdef UseQSearch
void QueryTermView::weight_modified (WCallback *wcb)
{
    CALL_DATA (XmTextVerifyCallbackStruct, tvp);
    if (tvp->text) {
	int length = tvp->text->length;
	for (char *p = tvp->text->ptr; p < tvp->text->ptr + length; p++) {
	    if (*p < '0' || *p > '9')
		break;
	}
	if (p < tvp->text->ptr + length) // non number chars found
	    tvp->doit = False;
    }
}
#endif

// /////////////////////////////////////////////////////////////////
// create_fields
// /////////////////////////////////////////////////////////////////

void
QueryTermView::create_fields()
{
  XmStringLocalized mtfstring;

  // Before proximity label and field are created, the default
  // sensitivity is stored in the label object.  11:53 01/14/93 DJB 
  // this shouldn't cause a 64 bit conversion problem - rCs

  size_t proximity_sensitive = (size_t) (Widget) f_proximity_label;
  
  ASSN  (WXmForm,         f_pws_form,        f_form,       "pws_form"        );
  ASSNM (WXmLabelGadget,  f_proximity_label, f_pws_form,   "proximity_label" );
  DECLM (WXmLabelGadget,  weight_label,      f_pws_form,   "weight_label"    );
  DECL  (WXmLabelGadget,  scope_label,       f_pws_form,   "scope_label"     );
  ASSNM (WXmTextField,    f_proximity_field, f_pws_form,   "proximity_field" );
  ASSNM (WXmTextField,    f_weight_field,    f_pws_form,   "weight_field"    );
  ASSN  (WXmTextField,    f_scope_field,     f_pws_form,   "scope_field"     );

  mtfstring = CATGETS(Set_AgentLabel, 241, "Weight");
  XtVaSetValues(weight_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 242, "Proximity");
  XtVaSetValues(f_proximity_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 243, "Scope");
  XtVaSetValues(scope_label, XmNlabelString, (XmString)mtfstring, NULL);

  SET_CALLBACK (f_proximity_field,ValueChanged,proximity_changed);
  SET_CALLBACK (f_weight_field,ValueChanged,weight_changed);
  SET_CALLBACK (f_proximity_field,ModifyVerify,verify_numeric);
  SET_CALLBACK (f_weight_field,ModifyVerify,verify_numeric);

  if (proximity_sensitive)
    {
      f_proximity_label.SetSensitive (True);
      f_proximity_field.SetSensitive (True);
    }

  f_pws_form.Realize();  // Force geometry computations. 
}


// /////////////////////////////////////////////////////////////////
// verify_term
// /////////////////////////////////////////////////////////////////

void
QueryTermView::verify_term (WCallback *wcb)
{
  XmTextVerifyPtr text = (XmTextVerifyPtr) wcb->CallData();

  // Make sure all spacing characters are a space (' ').
  // This strips newlines, linefeeds, and other undesirables. 
  // (Fix for DTS-8457).
  char *s = text->text->ptr;

#ifdef UseWideChars	
  if (s != NULL) {
    while (*s)
      {
	int	size;
	size = mblen(s, MB_LEN_MAX);
	if (size == 1) {
	  if (isspace(*s))
	    *s = ' ';
	  s++;
	} else {
	  s += size;
	}
      }
  }
#else
  if (s != NULL)
    while (*s)
      {
	if (isspace(*s))
	  *s = ' ' ;
	s++;
      }
#endif
}


// /////////////////////////////////////////////////////////////////
// term_changed
// /////////////////////////////////////////////////////////////////

void
QueryTermView::term_changed()
{
  char old_term = *(f_query_term->term_string());
  char *new_term = f_term_field->GetString();
  f_query_term->term_string (new_term);

  if (old_term == '\0')
    query_editor().decrement_null_terms();
  else if (*new_term == '\0')
    query_editor().increment_null_terms();

  query_editor().query_changed();
}

#ifndef UseQSearch
// /////////////////////////////////////////////////////////////////
// proximity_changed
// /////////////////////////////////////////////////////////////////

void
QueryTermView::proximity_changed()
{
  f_query_term->proximity (f_proximity_field.GetString());
  query_editor().query_changed();
}
#endif

// /////////////////////////////////////////////////////////////////
// weight_changed
// /////////////////////////////////////////////////////////////////

void
QueryTermView::weight_changed()
{
#ifdef UseQSearch
  static int nest_count = 0;
  nest_count++;
  if (nest_count == 1) {
	int cursor_pos;
	char* weight_string = f_weight_field.GetString();
	if (*weight_string) {
	    int w = atoi(weight_string);
	    if (w > 100) {
		w = 100;
		cursor_pos = 3;
	    }
	    else if (w < 0)
		cursor_pos = w = 0;
	    else
		cursor_pos = f_weight_field.GetInsertionPosition();
	    sprintf(weight_string, "%d", w);
	    f_weight_field.SetString(weight_string);
	    f_weight_field.SetInsertionPosition(cursor_pos);
	}
	XtFree(weight_string);
	nest_count--;
  }
  else {
	nest_count--;
	return;
  }
#endif
  f_query_term->weight (f_weight_field.GetString());
  query_editor().query_changed();
}


// /////////////////////////////////////////////////////////////////
// set_pws_label - set pws label based on p, w & s values.
// /////////////////////////////////////////////////////////////////

#ifdef UNUSED
void
QueryTermView::set_pws_label()
{
  XmString s, t1, t2;

  // NOTE: Get rid of English letters here!
  if (*f_query_term->weight() == NULL)
    t1 = XmStringCreate ("W", "norm");
  else
    t1 = XmStringCreate ("W", "bold");

  if (*f_query_term->proximity() == NULL)
    t2 = XmStringCreate ("P", "norm");
  else
    t2 = XmStringCreate ("P", "bold");

  s = XmStringConcat (t1, t2);
  XmStringFree (t1);
  XmStringFree (t2);
  t1 = s;

#ifdef SCOPE
  if (*f_query_term->scope() == NULL)
    t2 = XmStringCreate ("S", "norm");
  else
    t2 = XmStringCreate ("S", "bold");

  s = XmStringConcat (t1, t2);
  XmStringFree (t1);
  XmStringFree (t2);
#endif

  f_pws_toggle.LabelString (WXmString (s, WXmString::NO_COPY));
  XmStringFree (s);
}
#endif

#ifndef UseQSearch
// /////////////////////////////////////////////////////////////////
// prev_connective_changed
// /////////////////////////////////////////////////////////////////

void
QueryTermView::prev_connective_changed()
{
  // To save space the sensitivity state of the proximity field is stored
  // in the WWL proximity label when the label hasn't been created yet.

  switch (f_previous->query_term()->connective())
    {
      case C_NEAR:
      case C_BEFORE:
        if (f_pws_form != 0)
	  {
	    f_proximity_label.SetSensitive (True);
	    f_proximity_field.SetSensitive (True);	  }
	else
	  f_proximity_label = (Widget) True;
	break;

      default:
	if (f_pws_form != 0)
	  {
	    f_proximity_label.SetSensitive (False);
	    f_proximity_field.SetSensitive (False);
	  }
	else
	  f_proximity_label = (Widget) False;
	break;
      }
}
#endif

// /////////////////////////////////////////////////////////////////
// verify_numeric - only allow field input of numbers
// /////////////////////////////////////////////////////////////////

void
QueryTermView::verify_numeric (WCallback *wcb)
{
  CALL_DATA (XmTextVerifyCallbackStruct,tvp);

  if (tvp->text != NULL)
    {
      int length = tvp->text->length;
      char *p = tvp->text->ptr;
      for (; length > 0; p++, length--)
	if (!isdigit (*p))
	  {
	    tvp->doit = False;
	    return;
	  }
    }
}
