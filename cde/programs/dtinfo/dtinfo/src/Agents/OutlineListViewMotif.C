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
/*    Copyright (c) 1994 FUJITSU LIMITED      */
/*    All Rights Reserved                     */

/*
 * $TOG: OutlineListViewMotif.C /main/15 1997/06/18 17:31:56 samborn $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * withuot the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#include "UAS.hh"

#define C_WindowSystem
#define L_Other

#define C_OutlineList
#define C_OutlineElement
#define C_TOC_Element
#define L_Basic

#define C_OutlineListView
#define C_LibraryAgent
#define L_Agents

#define C_LibraryMgr
#define C_MessageMgr
#define L_Managers

#define USES_OLIAS_FONT

#include "Prelude.h"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

#include <WWL/WComposite.h>
#include <WWL/WXmScrollBar.h>

#include <iostream>
using namespace std;
#include <unistd.h>
#include <sys/param.h>

#ifdef DEBUG
#define DEBUGF(X) printf X
#else
#define DEBUGF(X)
#endif

#if 0
#  define RCS_DEBUG(statement) cerr << statement << endl
#else
#  define RCS_DEBUG(statement) 
#endif

extern "C" { void _Xm_dump_external(XmString); }

#ifdef NotDefined
static XmString
XmStringCreateComponent (XmStringComponentType tag, void *data, u_int length);
#endif

enum { XmSTRING_COMPONENT_POINTER = XmSTRING_COMPONENT_USER_BEGIN };


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

OutlineListView::OutlineListView (const WComposite &parent, const char *name,
				  bool automanage,
				  bool enable_activate)
: WXmList ((Widget) NULL), f_list (NULL), f_max_level (LEVEL_UNLIMITED),
  f_current_selection (NULL), f_selected_item_count(0),
  f_tracking_position (0),
  f_tracking_element (NULL),
  f_library_agent (NULL)
{
  static int serial_number = 1;

  // Assign a unique serial number to this outline list. 
  f_serial_number = serial_number++;

  // Non-scrolled list is useless so make it a scrolled list. 
  widget = XmCreateScrolledList (parent, (char *) name, NULL, 0);


  // if dtinfo_font is defined then get the fontlist for the widget 
  // and append thte dtinfo_font

  if (window_system().dtinfo_font()) {
      XmFontList tempFontList, defaultList;

      // get the current font list and make a copy
      XtVaGetValues(widget, XmNfontList, &tempFontList, NULL);
      defaultList = XmFontListCopy(tempFontList);

      // append the dtinfo font. XmFontListAppendEntry deallocates the original font list after
      // extracting the required information. 

      defaultList = XmFontListAppendEntry(defaultList, window_system().dtinfo_font());

      // set the new new font list back
      XtVaSetValues(widget, XmNfontList, defaultList, NULL);

  }

  // Save this pointer in user data of widget so that actions can
  // recover. 
  UserData (this);

#if 0
  printf (">>> *** shadow %d, margin %d, highlight %d\n",
	  ShadowThickness(), ListMarginWidth(), HighlightThickness());
#endif
  f_margin = ShadowThickness() + ListMarginWidth() + HighlightThickness();
  f_selection_policy = SelectionPolicy();

  register_actions();

  SetSingleSelectionCallback (this, (WWL_FUN) &OutlineListView::select);
  SetBrowseSelectionCallback (this, (WWL_FUN) &OutlineListView::select);
  SetExtendedSelectionCallback (this, (WWL_FUN) &OutlineListView::select);
  SetMultipleSelectionCallback (this, (WWL_FUN) &OutlineListView::select);

  SetConvertCallback (this, (WWL_FUN) &OutlineListView::printConvertCallback);

  if (enable_activate)
    {
      SetDefaultActionCallback (this, (WWL_FUN) &OutlineListView::activate);
      f_tracking_possible = 1;
    }
  else
    {
      f_tracking_possible = 0;
    }
  
  if (automanage)
    Manage();
}

static Dimension
icon_width(Widget w)
{
  XmFontList defaultList;
  XtVaGetValues(w, XmNfontList, &defaultList, NULL);

  // Setup parameters for calculating click position.
  char string[2];
  if (window_system().nofonts())
    string[0] = ' ';
  else
    string[0] = OLIAS_PLACEHOLDER_ICON;
  string[1] = '\0';
  XmString thing = XmStringCreate (string, (char*)OLIAS_FONT);

  Dimension rval = XmStringWidth(defaultList, thing);
  XmStringFree (thing);

  return rval;
}

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

OutlineListView::~OutlineListView()
{
#ifdef BOGUS
  if (f_list != NULL)
    f_data_handle = library_mgr().library().get_data_handle();
#endif
}


// /////////////////////////////////////////////////////////////////
// xmstring - return the xm string given an outline element
// /////////////////////////////////////////////////////////////////

XmString
OutlineListView::xmstring (OutlineElement *oe, unsigned force, char icon)
{
  if (force || (oe->string_creator() != f_serial_number))
    {
      if (oe->xm_string() != NULL)
	XmStringFree ((XmString) oe->xm_string());
      oe->set_xm_string ((void *) create_xm_string (oe, f_base_level,
						    f_tracking_possible, icon));
      oe->string_creator (f_serial_number);
    }
  return ((XmString) oe->xm_string());
}


// /////////////////////////////////////////////////////////////////
// set_list - set the list
// /////////////////////////////////////////////////////////////////

void
OutlineListView::set_list (OutlineList *list, BitHandle handle)
{
  // NOTE: Eventually it should be possible to set the list to NULL.
  if (f_list != NULL && f_list != list )
    list->free_data_handle (f_data_handle);
  f_list = list;
  if (list != NULL) {
    if (handle != 0)
      f_data_handle = handle ;
    else
      f_data_handle = list->get_data_handle();
  }
  // Determine the base level.
  f_base_level = 0;
  if (list->length() > 0) {
      //  SWM -- need to change this so that when determining
      //  the base level, we check all of the roots...
      f_base_level = ((OutlineElement *)(*list)[0])->level();
  }
  ON_DEBUG(printf (">> OutlineList base level = %d\n", f_base_level));
  regen_list();
}


// /////////////////////////////////////////////////////////////////
// regen_list - regenerate list based on internal list
// /////////////////////////////////////////////////////////////////

static u_int g_table_index;

void
OutlineListView::regen_list()
{
  Xassert (f_list != NULL);
  // if list is null we should just empty the list. 

  u_int visible_count = f_list->count_expanded (f_data_handle);

  XmStringTable table = new XmString[visible_count];
  bool *selected_list = new bool[visible_count];

  g_table_index = 0;
  generate_table (f_list, table, selected_list, 0);

  WArgList args;
  Items (table, args);
  ItemCount (visible_count, args);
  Set (args);

  delete [] table;
  delete [] selected_list;


  // reset tracking position to let dtinfo determine
  // the new position in track_to()
  f_tracking_element = NULL;
  f_tracking_position = 0;
}

// /////////////////////////////////////////////////////////////////
// create_outline_string - create the XmString to display
// /////////////////////////////////////////////////////////////////

XmString
OutlineListView::create_xm_string (OutlineElement *oe, int base_level,
				   unsigned char track_on, char icon) 
{
  XmString string, tmp, next;

  unsigned char track = track_on ? 1 : 0; // normalize track_on into 1 or 0

  // First component is pointer to original element.  Motif 1.2
  // will ignore this component. 
#ifdef NotDefined
  string = XmStringCreateComponent (XmSTRING_COMPONENT_POINTER,
				    &oe, sizeof (oe));
#endif
  // Next component is expand/contract icon, if any.
  // 1 for tracker, 1 for arrow, 3 for icon and spacing, 1 for terminator
  char s[64];
  char *p = s ;
  assert ((oe->level() - base_level + 6) * sizeof(char) < 64);
  // Spaces before and including arrow, if any
  if (window_system().nofonts())
  {
    memset (s, ' ', oe->level() - base_level + 1 + track);
  }
  else
  {
    if (track)
      {
	if (icon)
	  {
	    s[0] = icon ;
	  }
	else
	  {
	    s[0] = OLIAS_PLACEHOLDER_ICON ;
	  }
	p++ ;
      }
    memset (p, OLIAS_PLACEHOLDER_ICON, oe->level() - base_level);

    unsigned int pos = oe->level() - base_level + track;

    // Only show the icon if this entry has children and
    // the max level is unlimited or max level is higher than this level. 
    
    if (oe->has_children() &&
	(f_max_level == LEVEL_UNLIMITED || f_max_level > oe->level())) {
      if (oe->is_expanded (f_data_handle))
	s[pos] = OLIAS_EXPANDED_ICON;
      else
	s[pos] = OLIAS_CONTRACTED_ICON;
    }
    else
	s[pos] = OLIAS_NO_CHILDREN_ICON;
  }
  // Slot for icon -- plug in icon based on element type.
  char entry_icon = '\0';
  
  if (oe->type() == TOC_Element::TOC_ElementClass)
    {
      UAS_ObjectType type = ((TOC_Element *) oe)->toc()->type();
      switch (type)
	{
	case UAS_LIBRARY:
	  entry_icon = OLIAS_INFOLIB_ICON;
	  break;
	case UAS_BOOKCASE:
	  entry_icon = OLIAS_INFOBASE_ICON;
	  break;
	case UAS_BOOK:
	  entry_icon = OLIAS_BOOK_ICON;
	  break;
	default:
	  entry_icon = '\0';
	}
    }
  
  if (entry_icon != '\0' && !window_system().nofonts())
    {
      s[oe->level() - base_level + 1 + track] = OLIAS_SPACE04;
      s[oe->level() - base_level + 2 + track] = entry_icon;
      s[oe->level() - base_level + 3 + track] = OLIAS_SPACE04;
      s[oe->level() - base_level + 4 + track] = '\0';
    }
  else if (window_system().nofonts())
    {
      s[oe->level() - base_level + 1 + track] = ' ';
      s[oe->level() - base_level + 2 + track] = '\0';
    }
  else
    s[oe->level() - base_level + 1 + track] = '\0';
  
  next = XmStringCreate (s, (char*)OLIAS_FONT);
  
#ifdef NotDefined
  // Concat the parts 
  tmp = string;
  string = XmStringConcat (tmp, next);
  XmStringFree (tmp);
  XmStringFree (next);
#else
  string = next ;
#endif
  
  // Final component is text of item
  next = XmStringCreateLocalized ((String) oe->display_as());
  tmp = string;
  string = XmStringConcat (tmp, next);
  XmStringFree (tmp);
  XmStringFree (next);
  //  _Xm_dump_external (string);
  
  return (string);
}


// /////////////////////////////////////////////////////////////////
// set_icon - set the icon in the xm string (the quick way)
// /////////////////////////////////////////////////////////////////

// Warning: This routine depends heavily upon the format of the XmString
// generated above.  Any changes in that string WILL break this routine.

void
OutlineListView::set_icon (OutlineElement *oe)
{
  xmstring (oe, 1);		// force creation of string
}


// /////////////////////////////////////////////////////////////////
// generate_table - generate XmStringTable starting at list
// /////////////////////////////////////////////////////////////////

// NOTE: Can probably (and should) change g_element to f_element 
static OutlineElement *g_element;

void
OutlineListView::generate_table (OutlineList *list, XmStringTable &table,
				 bool *selected_list, u_int level, unsigned force)
{
  u_int i;

#if 0
  printf ("In generate_table, list = %p, level = %d\n", list, level);
#endif
  for (i = 0; i < list->length(); i++)
    {
      g_element = ((OutlineElement *)(*list)[i]);
      // NOTE: temporary for now - calc level in outline list code - DJB
      // The level can be stored in the list, instead of the element.
      // Store pointer to list in each element to back tracing. 
      
      // Level is initialized on first display.
      // NOTE: This is a temporary hack.  15:59 01/13/93 DJB 
      if (g_element->string_creator() == 0)
	g_element->level (level);
      
      xmstring (g_element, force); // create the string

      selected_list[g_table_index] = g_element->is_selected (f_data_handle);
      table[g_table_index++] = (XmString) g_element->xm_string();
      
      //  g_element->print();
      if ((g_element->has_children()) &&
	  g_element->is_expanded (f_data_handle))
	generate_table (g_element->children(), table, selected_list, level+1, force);
    }
}


#ifdef NotDefined
// /////////////////////////////////////////////////////////////////
// XmStringCreateComponent
// /////////////////////////////////////////////////////////////////

static XmString
XmStringCreateComponent (XmStringComponentType tag,
			 void *data, u_int length)
{
  // NOTE: Possible improvement is to handle greater lengths, but this
  // is easiest for now.  DJB 8-8-92
  assert (length <= 124);
  
  // Length = ASN header + Component header + data 
  unsigned char total_length = 2 + length;
  
  XmString string = (XmString) XtMalloc (total_length + 4);
  unsigned char *p = (unsigned char*) string;
  
  // Write the ASN.1 header (values from XmString.c)
  *p++ = 0xdf;
  *p++ = 0x80;
  *p++ = 0x06;
  *p++ = total_length;
  
  // Write the component header
  *p++ = tag;
  *p++ = length;
  
  memcpy (p, data, length);
#if 0
  printf ("data = %p\n", data);
  
  int i;
  printf ("XmStringComponent = ");
  for (i = 0; i < total_length + 4; i++)
    printf ("%02x ", string[i]);
  puts ("");
#endif
  return (string);
}
#endif


// /////////////////////////////////////////////////////////////////
// y_to_outline_element
// /////////////////////////////////////////////////////////////////

OutlineElement *
OutlineListView::y_to_outline_element (Position y)
{
  OutlineElement *oe;
  // NOTE: if having problems with list expansion items, check here 
  
  // NOTE: Motif 1.2 bug alert: YToPos returns zero based, instead of 1 based
  // as documented. 
  
  // NOTE: seems to be fixed returning 1 based 
  
  f_item_pos = YToPos (y) - 1;	
  
#if 0
  printf ("Click in item %d, item count = %d\n", f_item_pos, ItemCount());
#endif
  // Another Motif 1.2 bug.  YToPos should return 0 for bogus position,
  // but it doesn't check the range. 
  if ((int) f_item_pos < 0 || (int) f_item_pos >= ItemCount())
    return (NULL);
  
  oe = item_at (f_item_pos);
  
#if 1
  ON_DEBUG(printf ("Found element %p (%s) at pos %d, level %d\n",
		   oe, oe->display_as(), f_item_pos, oe->level()));
#endif
  
  f_item_pos++;		// Adjust for Motif bug here. 
  return (oe);
}


// /////////////////////////////////////////////////////////////////
// icon_extent
// /////////////////////////////////////////////////////////////////

inline Position
OutlineListView::icon_extent(OutlineElement *oe) const
{
  ON_DEBUG (printf ("horiz sbar value = %d\n",
		    WXmScrollBar(HorizontalScrollBar()).Value()));
  ON_DEBUG (printf ("  extent = %d\n",
		    f_margin - WXmScrollBar(HorizontalScrollBar()).Value() +
		    icon_width(*this) * (oe->level() - f_base_level + 1 + f_tracking_possible)));
  
  return (f_margin - WXmScrollBar(HorizontalScrollBar()).Value() +
	  icon_width(*this) * (oe->level() - f_base_level + 1 + f_tracking_possible));
}


// /////////////////////////////////////////////////////////////////
// select_start - mouse button pressed (action proc)
// /////////////////////////////////////////////////////////////////

void
OutlineListView::_select_start (Widget w, XEvent *event,
				String *params, Cardinal *num_params)
{
  WXmPrimitive W (w);
  OutlineListView *lv = (OutlineListView *) W.UserData();
  if (lv == NULL)
    return;
  lv->select_start (w, event, params, num_params);
}

void
OutlineListView::select_start (Widget w, XEvent *event,
			       String *params, Cardinal *num_params)
{
  if (event->type != ButtonPress)
    return;
  
  ON_DEBUG(printf ("Select start at (%d, %d)\n",
		   event->xbutton.x, event->xbutton.y));
  
  f_outline_element = y_to_outline_element (event->xbutton.y);
  
#ifdef DEBUG
  if (f_outline_element)
    {
      printf (">>> *** shadow %d, margin %d, highlight %d\n",
	      ShadowThickness(), ListMarginWidth(), HighlightThickness());
      
      printf ("margin = %d, icon width = %d, level = %d\n",
	      f_margin, icon_width(*this), f_outline_element->level());
    }
  else
    printf ("No element found at this location\n");
#endif
  // See if item valid and icon was clicked in.
  ON_DEBUG (printf ("X pos = %d\n", event->xbutton.x));
  if (f_outline_element && event->xbutton.x < icon_extent (f_outline_element)
      && f_outline_element->has_children())
    {
      ON_DEBUG (puts ("Clicked in an icon #1"));
      return;
    }
  
  // Set variable to signal that icon was not clicked on.  This is
  // checked in the select_end method. 
  f_outline_element = NULL;
  
  ON_DEBUG (puts ("Passing through start"));
  XtCallActionProc (w, "ListBeginSelect", event, params, *num_params);
}


// /////////////////////////////////////////////////////////////////
// select_end - mouse button released (action_proc)
// /////////////////////////////////////////////////////////////////

void
OutlineListView::_select_end (Widget w, XEvent *event, String *params,
			      Cardinal *num_params)
{
  WXmPrimitive W (w);
  OutlineListView *lv = (OutlineListView *) W.UserData();
  if (lv == NULL)
    return;
  lv->select_end (w, event, params, num_params);
}

void
OutlineListView::select_end (Widget w, XEvent *event,
			     String *params, Cardinal *num_params)
{
  if (event->type != ButtonRelease)
    return;
  
  ON_DEBUG (printf ("Select end at (%d, %d)\n",
		    event->xbutton.x, event->xbutton.y));
  
  // See if selection ended on the same item and in range.
  if (f_outline_element != NULL &&
      (y_to_outline_element (event->xbutton.y) == f_outline_element) &&
      f_outline_element && event->xbutton.x < icon_extent (f_outline_element)
      && f_outline_element->has_children() &&
      (f_max_level == LEVEL_UNLIMITED ||
       f_max_level > f_outline_element->level()))
    {
      ON_DEBUG (puts ("Clicked in an icon #2"));
      unsigned char current_policy = f_selection_policy;
      u_int subcount;
      
      // Remove previous tracking, if any.
      // Must happen before any expand/contract takes place
      // or either the position will be wrong, or item hidden. 
      if (f_tracking_position > 0)
	{
	  if (window_system().nofonts())
	    set_track_icon (f_tracking_element, f_tracking_position, ' ');
	  else
	    set_track_icon (f_tracking_element, f_tracking_position,
			    OLIAS_PLACEHOLDER_ICON);
	  f_tracking_position = 0;
	}
      
      if (f_outline_element->is_expanded (f_data_handle))
	{
	  /* -------- Toggle current state to contracted -------- */
	  f_outline_element->set_contracted (f_data_handle);
	  subcount =
	    f_outline_element->children()->count_expanded (f_data_handle);
	  DeleteItemsPos (subcount, f_item_pos + 1);
	}
      else
	{
	  /* -------- Toggle current state to expanded -------- */
	  // Turn on the wait cursor if database access to occur.
	  bool wait_state = FALSE;
	  if (!f_outline_element->children_cached())
	    {
	      window_system().set_wait_state (WS_ON);
	      wait_state = TRUE;
	    }
	  
	  f_outline_element->set_expanded (f_data_handle);
	  subcount =
	    f_outline_element->children()->count_expanded (f_data_handle);
	  XmStringTable table = new XmString[subcount];
	  bool *selected_list = new bool[subcount];
	  g_table_index = 0;
	  generate_table (f_outline_element->children(),
			  table, selected_list,
			  f_outline_element->level() + 1);
	  AddItemsUnselected (table, subcount, f_item_pos + 1);
	  // Must be in multiple select mode to add selected items 
	  if (f_selection_policy != XmMULTIPLE_SELECT)
	    {
	      SelectionPolicy (XmMULTIPLE_SELECT);
	      current_policy = XmMULTIPLE_SELECT;
	    }
	  // Bogus Motif should have a routine to select multiple items. 
	  while (subcount > 0)
	    if (selected_list[--subcount])
	      SelectPos (f_item_pos + subcount + 1, False);
	  delete table;
	  delete selected_list;
	  
	  // Turn the wait cursor off if it was on. 
	  if (wait_state)
	    window_system().set_wait_state (WS_OFF);
	}
      
      // Update the icon for new mode 
      set_icon (f_outline_element);
      
      // Tell the list about the change
      XmString item = (XmString) f_outline_element->xm_string();
      bool selected = PosSelected (f_item_pos);
      ReplaceItemsPosUnselected (&item, 1, f_item_pos);
      
      // YAMB (Yet Another Motif Bug): Cannot call ReplaceItemsPos
      // because if the item matches another item in the list that
      // happens to be selected, the item replaced will be selected. 
      // So, do selection here, if needed. 
      if (selected)
	{
	  // Bugs Galore!!  Motif will automatically (gee thanks)
	  // deselect all other selections on a select call, even
	  // if this list is in extended select mode.  Work around
	  // by switching to multiple select mode which allows it.
	  if (current_policy != XmMULTIPLE_SELECT)
	    {
	      SelectionPolicy (XmMULTIPLE_SELECT);
	      current_policy = XmMULTIPLE_SELECT;
	    }
	  
	  // Select it, do not call the select callback. 
	  SelectPos (f_item_pos, False);
	}
      
      // Update the tracking if activated.
      // Must happen after the expand/contract so that the track
      // location is correctly computed.
      bool scroll = (f_tracking_element == f_outline_element);
      if (f_tracking_possible && f_library_agent)
	f_library_agent->track (scroll);
      
      // Switch the policy back, if it had been changed. 
      if (f_selection_policy != current_policy)
	SelectionPolicy (f_selection_policy);
    }
  // f_outline_element can only be NULL if the click down wasn't in the icon. 
  else if (f_outline_element == NULL)
    {
      ON_DEBUG (("Passing through end"));
      XtCallActionProc (w, "ListEndSelect", event, params, *num_params);
    }
#if 0
else
  ON_DEBUG(puts ("Click down in icon, but click up wasn't"));
#endif
}

void
OutlineListView::update_list(OutlineList *list, BitHandle handle)
{
  u_int visible_count = list->count_expanded (handle);
  
  XmStringTable table = new XmString[visible_count];
  bool *selected_list = new bool[visible_count];
  g_table_index = 0;
  generate_table (list, table, selected_list, 0, 1);

  WArgList args;
  Items (table, args);
  ItemCount (visible_count, args);
  Set (args);

  delete table;
  delete selected_list;
}

// /////////////////////////////////////////////////////////////////
// register_actions
// /////////////////////////////////////////////////////////////////

void
OutlineListView::register_actions()
{
  static bool registered = FALSE;
  if (registered)
    return;
  
  static XtActionsRec actions_list[] =
    {
      {(char*)"OutlineListBeginSelect",   OutlineListView::_select_start},
      {(char*)"OutlineListEndSelect",     OutlineListView::_select_end},
    };
  
  
  XtAppAddActions (AppContext(), actions_list, XtNumber (actions_list));
}

/**********************************************************************
OutputAnAtomName: Translates a target from its internal atom format into 
a human readable character string.
**********************************************************************/
static void
OutputAnAtomName(Widget w, Atom target)
{
    char  *AtomName = (char *)malloc(sizeof(char *) * 34);
    
    AtomName = XGetAtomName(XtDisplay(w), target);
    printf("\t%s\n", AtomName);
}

// /////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////

void
OutlineListView::printConvertCallback(WCallback *wcb)
{
    XmConvertCallbackStruct *ccs = (XmConvertCallbackStruct *)wcb->CallData();
    Widget         w = wcb->GetWidget();
    unsigned int * selectedPositions;
    int            selectedItemCount;
    char           filepath[MAXPATHLEN];
    OutlineElement *oe = NULL;
    FILE *         fp;
    int            n, len, slen;
    
    Atom FILE_NAME = XInternAtom(XtDisplay(w), XmSFILE_NAME, False);
    Atom TARGETS = XInternAtom(XtDisplay(w), XmSTARGETS, False);
    Atom MOTIF_EXPORT_TARGETS = XInternAtom(XtDisplay(w), XmS_MOTIF_EXPORT_TARGETS, False);
    
    RCS_DEBUG("printConvertCallback: called.\n");
    
    if (ccs == NULL) {
	return;
    }
    
    RCS_DEBUG("\nNow in ConvertCallback.\n");
    RCS_DEBUG("\tSelection: ");
    OutputAnAtomName((Widget)w, ccs->selection);
    RCS_DEBUG("\tTarget: ");
    OutputAnAtomName((Widget)w, ccs->target); 
    
    /*
     *  XmeDragSource is going to call ConvertCallback and ask
     *  it to convert MOTIF_EXPORT_TARGETS. 
     */ 
    if ( (ccs->target == MOTIF_EXPORT_TARGETS) ||
	 (ccs->target == TARGETS))
    {
	
	/* 
	 *  this callback must support the FILE_NAME transfer
	 *  as this is the mechanism the Printer Icon on the 
	 *  front panel uses to transfer data.
	 */
	Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 1);
	if (targs == NULL) {
	    ccs->status = XmCONVERT_REFUSE;
	    printf("Refuse.\n");
	    return;
	}
	n = 0;
	targs[n] = FILE_NAME; n++;
	
	ccs->value = (XtPointer) targs;
	ccs->type = XA_ATOM;
	ccs->length = n;
	ccs->format = 32;
	ccs->status = XmCONVERT_DONE;  /* Yes, we converted the target. */
    }
    
    /* 
     *  As the drop site supports FILE_NAME as an import target, then
     *  the drop site will ask ConvertCallback to convert the 
     *  value to FILE_NAME format. 
     */ 
    else if (ccs->target == FILE_NAME) {
	
	/*
	 *   Get the text from the container
	 */
	XtVaGetValues(w,
		      XmNselectedPositions,    &selectedPositions,
		      XmNselectedItemCount,   &selectedItemCount,
		      NULL);

	f_selected_item_count = selectedItemCount;
	
	// This is commented out for now until i go and make this function private	
#if 0
	tmpfile = _DtActGenerateTmpFile(NULL, 
					"%s.itp", 
					( S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP), 
					&fp);
#else	
	len = MIN(strlen(getenv("HOME")), MAXPATHLEN - 1);
	*((char *) memcpy(filepath, getenv("HOME"), len) + len) = '\0';
	slen = len;
	len = MIN(strlen("/.dt/tmp/file.itp"), MAXPATHLEN - 1);
	*((char *) memcpy(filepath + slen,
			  "/.dt/tmp/file.itp", len) + len) = '\0';
#endif
	
	// print on on debug
	fprintf(stderr, "printConvertCallback: temp file = %s.\n", filepath);
	
	// open the temporary file for writing

	if ((fp = fopen(filepath, "w")) == NULL) {
	    fprintf(stderr, "Cannot open file %s.\n", filepath);
	}

	// write out each element in the list

	for (unsigned int i = 0; i < f_selected_item_count; i++)	{
	    oe = item_at(selectedPositions[i] - 1) ;
	    TOC_Element *te = (TOC_Element *)oe;

	    // write out the locator
	    if (fputs(((TOC_Element *)te)->toc()->locator(), fp) == EOF) {
		fprintf(stderr, "Cannot write file %s in current directory.\n", filepath);
		return;
	    }

	    // write out eol
	    if (fputs("\n", fp) == EOF) {
		fprintf(stderr, "Cannot write file %s in current directory.\n", filepath);
		return;
	    }
	    
	}

	// close the file
	fclose(fp);
	
	AppPrintData* p = window_system().GetAppPrintData();
	p->f_pshell_parent = w;
	 
	// Assign file name to XmConvertCallbackStruct. 
	
	ccs->value  = (XtPointer)filepath;
	ccs->type   = XA_STRING;
	ccs->length = strlen(filepath);
	ccs->format = 8;
	ccs->status = XmCONVERT_DONE;
    }
    
    else  {
	/* Unexpected target. */
	ccs->status = XmCONVERT_REFUSE;
    } 
}

// /////////////////////////////////////////////////////////////////
// select - selection callback
// /////////////////////////////////////////////////////////////////

void
OutlineListView::select (WCallback *wcb)
{
  XmListCallbackStruct *lcs = (XmListCallbackStruct *) wcb->CallData();
  OutlineElement *oe = NULL;
  
  DEBUGF (("**** select: item count = %d, pos = %d, selected = %s\n",
	   lcs->selected_item_count, lcs->item_position,
	   (PosSelected (lcs->item_position) ? "true" : "false")));
  
  //  DEBUGF ((">>>> SELECTION TYPE <<< %d\n", lcs->selection_type));
  
  /* Fix for DTS #6303 -- Removed "if" statement.  Always deselect all. */
  deselect (f_list, DESELECT_ALL);
  
  if (f_selection_policy == XmMULTIPLE_SELECT ||
      f_selection_policy == XmEXTENDED_SELECT)
    {
      f_selected_item_count = lcs->selected_item_count;
#ifdef DEBUG
      printf ("selected # = %d\n", f_selected_item_count);
      printf ("         # = %d\n", SelectedItemCount());
#endif
      for (unsigned int i = 0; i < f_selected_item_count; i++)
	{
	  DEBUGF (("M-> extracting at %d\n", lcs->selected_item_positions[i]));
	  oe = item_at(lcs->selected_item_positions[i] - 1) ;
	  oe->set_selected (f_data_handle);
	}
      // Make the current selection the last item selected.  This is
      // (unfortunately) used by the LibraryAgent for the detach feature.
      // The reason is that the Document List didn't support these selection
      // modes until printing was added and could only have one selection.
      // If there's more than one selection it won't matter because the
      // Detach button will be insensitive. 
      f_current_selection = oe;
    }
  else
    {
      if (PosSelected (lcs->item_position))
	{
	  DEBUGF (("--> extracting pointer @ %d\n", lcs->item_position));
	  oe = item_at(lcs->item_position - 1) ;
	  oe->set_selected (f_data_handle);
	  // Save the selection:
	  f_current_selection = oe;
	  f_selected_item_count = 1;
	}
    }
  if (f_selected_item_count != 1) // 94/10/28 haya 
    f_item_pos = 0;		  // modify to change sensitive of detach
  else				  // button by cursol key.
    f_item_pos = *lcs->selected_item_positions; //
  
  notify (ENTRY_SELECTED);
  
  // DISCOURSE:  If the list is in extended selection mode it is possible
  // for the user to toggle in and out of add mode (cf. Motif Style Guide
  // 1.2 pg. 4-6).  Unfortunately there's no way for us to know if he is
  // in add mode, but that's ok for now, since there's a bug in Motif 1.2
  // which deselects the previous selection, even when add mode is on and
  // the selection policy is extended select. 
}


// /////////////////////////////////////////////////////////////////
// deselect - turn off select bit in in-memory elements in list
// /////////////////////////////////////////////////////////////////

#define OUTLINE_ELEMENT(I) ((OutlineElement *)(*list)[I])

void
OutlineListView::deselect (OutlineList *list, deselect_mode_t mode)
{
  u_int i;
  
  for (i = 0; i < list->length(); i++)
    {
      OUTLINE_ELEMENT(i)->unset_selected (f_data_handle);
      if (OUTLINE_ELEMENT(i)->children_cached() &&
	  OUTLINE_ELEMENT(i)->has_children() &&
	  (mode == DESELECT_ALL ||
	   OUTLINE_ELEMENT(i)->is_expanded (f_data_handle)))
	deselect (OUTLINE_ELEMENT(i)->children(), mode);
    }
}


// /////////////////////////////////////////////////////////////////
// activate - activate callback
//
// Here's the deal: to get a consistent result for "activate" on any
// entry in the booklist, such expert activation will always** do at
// least an open of the object in the/a reading window. If the item
// also is the parent of a sub-hierarchy of more nodes, the first
// level beneath this object will be expanded [or contracted] in the
// booklist. No expansion will occur, of course, if it is the lowest
// leaf in the tree. A TOC (== a "Book" level) will display its TOC
// page per its OutlineElement type; all other entries will display
// the document or the top-most document in the sub-hierarchy. -jcb
//
// ** Only if an entry has children and is initially expanded,
//  double-click contracts it, but does Not [re-]display its topmost
//  document. The assumption is being made that user Contract of a
//  hierarchical level is more often than not an expression of lesser
//  interest in its content. In many cases, the topmost doc will have
//  been previously displayed by a double-click on the entry while
//  contracted, anyway.
//
// Also, the double-click action "selects" only the activated entry.
// /////////////////////////////////////////////////////////////////

void
OutlineListView::activate (WCallback *wcb)
{
  XmListCallbackStruct *lcs = (XmListCallbackStruct *) wcb->CallData();
  OutlineElement *oe;
  int	expanded = False;
  
  oe = item_at(lcs->item_position - 1) ;

#ifdef DEBUG
  if( oe == NULL ) printf( "OLV::activate: NULL outline element pointer\n" ) ;
#endif

  if ( oe->has_children() )
    {
      u_int subcount;

      // Remove previous tracking, if any.
      // Must happen before any expand/contract takes place
      // or either the position will be wrong, or item hidden.
      if (f_tracking_position > 0)
        {
          if (window_system().nofonts())
            set_track_icon (f_tracking_element, f_tracking_position, ' ');
          else
            set_track_icon (f_tracking_element, f_tracking_position,
                            OLIAS_PLACEHOLDER_ICON);
          f_tracking_position = 0;
        }

      if (oe->is_expanded (f_data_handle))
        {
          expanded = True;
#ifdef DEBUG
          printf( "OLV::activate: entry is Expanded\n" ) ;
#endif
          /* -------- Toggle current state to contracted -------- */
          oe->set_contracted (f_data_handle);
          subcount =
            oe->children()->count_expanded (f_data_handle);
          DeleteItemsPos (subcount, lcs->item_position + 1);
        }
      else
        {
#ifdef DEBUG
          printf( "OLV::activate: entry is Not expanded\n" ) ;
#endif
          /* -------- Toggle current state to expanded -------- */
          // Turn on the wait cursor if database access to occur.
          bool wait_state = FALSE;
          if (!oe->children_cached())
            {
              window_system().set_wait_state (WS_ON);
              wait_state = TRUE;
            }

          oe->set_expanded (f_data_handle);
          subcount =
            oe->children()->count_expanded (f_data_handle);
          XmStringTable table = new XmString[subcount];
          bool *sel_list = new bool[subcount];
          g_table_index = 0;
          generate_table (oe->children(),
                          table, sel_list,
                          oe->level() + 1);
          AddItemsUnselected (table, subcount, lcs->item_position + 1);
          delete table;
          delete sel_list;

          // Turn the wait cursor off if it was on.
          if (wait_state)
            window_system().set_wait_state (WS_OFF);
        }

      // Update the icon for new mode
      set_icon (oe);
      // Tell the list about the change
      XmString item = (XmString) oe->xm_string();
      ReplaceItemsPosUnselected (&item, 1, lcs->item_position);

      // Update the tracking if activated.
      // Must happen after the expand/contract so that the track
      // location is correctly computed.
      bool scroll = (f_tracking_element == oe);
      if (f_tracking_possible && f_library_agent)
        f_library_agent->track (scroll);
    }
  else
    {
      // seems to occur ok for has_children case, above...
      deselect (f_list, DESELECT_ALL);
    }

  // Select it as the only selection; do not call the select callback.
  // Need to reset all class pointers to elements or item pos, regardless
  // of traversal method paths in which used by class.
  SelectPos (lcs->item_position, True);
  oe->set_selected (f_data_handle);
  f_current_selection = oe;
  f_selected_item_count = 1;
  f_item_pos = lcs->item_position;

  f_outline_element = NULL;    // because it should be with this path


  ON_DEBUG(cout << ">>>>>> CALLING DISPLAY ON ELEMENT <<<<<<" << endl);

  if ((oe->type() == TOC_Element::TOC_ElementClass) && !expanded)
    {
      mtry {
          UAS_ObjectType type = ((TOC_Element *) oe)->toc()->type();
          switch (type)
          {
	    case UAS_LIBRARY:
	      {
	        UAS_List<UAS_Common> kids =
	              ((TOC_Element *)oe)->toc()->children();
	        if (   (kids != (const int)0)
	            && (kids[0] != (const int)0)
                    && (kids[0]->type() == UAS_BOOKCASE))
	        {
	          UAS_List<UAS_Common> bckids = kids[0]->children();
	          if (! (bckids[0] == (const int)0))
	          {
	            bckids[0]->retrieve();
	          }
	        }
	      }
	      break;

	    case UAS_BOOKCASE:
	      {
	        UAS_List<UAS_Common> kids =
	              ((TOC_Element *)oe)->toc()->children();
	        if (! (kids[0] == (const int)0))
	        {
	          kids[0]->retrieve();
	        }
	      }
	      break;

	    default:
	    case UAS_BOOK:
	      // let the OutlineElement "display" method handle it.
	      oe->display();
	      break;
          }
        }
      mcatch_any()
        {
          message_mgr().error_dialog( (char*)UAS_String(CATGETS(
                           Set_UrlAgent, 5, "Document not found." )) ) ;
        }
      end_try;
    }
  else if (oe->type() != TOC_Element::TOC_ElementClass)  oe->display();
}


// /////////////////////////////////////////////////////////////////
// data_handle - return a new data handle
// /////////////////////////////////////////////////////////////////


BitHandle
OutlineListView::data_handle (BitHandle new_handle)
{
  f_data_handle = new_handle;
  // Need a variable since routine modifies it.
  u_int item_pos = 0;
  update_highlighting (f_list, item_pos);
  return new_handle;
}


// NOTE: is the  calling objects responsibility to make sure that expanded
// flag is correct with the current visual representation 

// /////////////////////////////////////////////////////////////////
// update_highlighting
// /////////////////////////////////////////////////////////////////


void
OutlineListView::update_highlighting_recursive (OutlineList *list,
						u_int &item_pos)
{
  for (unsigned int i = 0; i < list->length(); i++)
    {
      // Select it, do not call the select callback.  
      if (OUTLINE_ELEMENT(i)->is_selected (f_data_handle))
	{
	  if (!PosSelected(item_pos))
	    {
	      SelectPos (item_pos, False);
	    }
	} else {
	  DeselectPos(item_pos);
	}
      item_pos++;
      
      if (OUTLINE_ELEMENT(i)->is_expanded (f_data_handle) &&
	  OUTLINE_ELEMENT(i)->has_children())
	update_highlighting (OUTLINE_ELEMENT(i)->children(), item_pos);
    }
}


void
OutlineListView::update_highlighting (OutlineList *list, u_int &item_pos)
{
  
  if (item_pos == 0)
    {
      DeselectAllItems();
      item_pos = 1 ;
    }
  
  // Motif bug - see above.
  unsigned char current_policy = f_selection_policy;
  
  if (f_selection_policy != XmMULTIPLE_SELECT)
    {
      current_policy = f_selection_policy;
      f_selection_policy = XmMULTIPLE_SELECT;
      SelectionPolicy (f_selection_policy);
    }
  
  update_highlighting_recursive (list, item_pos);
  
  // Switch the policy back, if it was changed. 
  if (f_selection_policy != current_policy)
    {
      f_selection_policy = current_policy;
      SelectionPolicy (f_selection_policy);
    }
}


// /////////////////////////////////////////////////////////////////
// selected_item_list
// /////////////////////////////////////////////////////////////////

List *
OutlineListView::selected_item_list()
{
  return (f_list->selected_items (f_data_handle));
}


// /////////////////////////////////////////////////////////////////
// clear
// /////////////////////////////////////////////////////////////////

void
OutlineListView::clear()
{
  DeselectAllItems();
  deselect(f_list, DESELECT_ALL);
  f_selected_item_count = 0;
}

// /////////////////////////////////////////////////////////////////
// track_to - display tracking icon at specified position
// /////////////////////////////////////////////////////////////////

void
OutlineListView::track_to (OutlineElement *oe, u_int position, char icon)
{
  // Remove the old tracking icon, if any set. 
  if (f_tracking_position > 0) {
    if (window_system().nofonts())
      set_track_icon (f_tracking_element, f_tracking_position, ' ');
    else
      set_track_icon (f_tracking_element, f_tracking_position,
		      OLIAS_PLACEHOLDER_ICON);
  }
  
  // Set the new tracking icon. 
  set_track_icon (oe, position, icon);
  f_tracking_position = position;
  f_tracking_element = oe;
}


void
OutlineListView::untrack()
{
  if (f_tracking_position > 0) {
    if (window_system().nofonts())
      set_track_icon (f_tracking_element, f_tracking_position, ' ');
    else
      set_track_icon (f_tracking_element, f_tracking_position,
		      OLIAS_PLACEHOLDER_ICON);
  }
  
  f_tracking_position = 0;
}


// Warning: This routine depends heavily upon the format of the XmString
// generated earlier.  Any changes in that string WILL break this routine.

void
OutlineListView::set_track_icon (OutlineElement *oe, u_int position, char icon)
{
  xmstring (oe, 1, icon);

  // Tell the list about the change
  XmString item = (XmString) oe->xm_string();
  bool selected = PosSelected (position);
  ReplaceItemsPosUnselected (&item, 1, position);
  if (selected)
    {
      unsigned char current_policy = f_selection_policy;
      if (current_policy != XmMULTIPLE_SELECT)
	{
	  SelectionPolicy (XmMULTIPLE_SELECT);
	  current_policy = XmMULTIPLE_SELECT;
	}
      
      // Select it, do not call the select callback. 
      SelectPos (position, False);
      // Switch the policy back, if it had been changed. 
      if (f_selection_policy != current_policy)
	SelectionPolicy (f_selection_policy);
    }
}

// OutlineListView::item_at(position)
//
// return the OutlineElement that is visible at 'position' in 
// the list view 
//

OutlineElement *
OutlineListView::item_at(unsigned int position)
{
  OutlineList *lptr = f_list ;
  unsigned int count  = 0 ;
  unsigned int offset = 0 ;
  
  ON_DEBUG(cerr << "item_at: " << position << endl);
  
  // start at the beginning and descend the nested lists until we
  // reach the desired position, and return the element there
  
  OutlineElement *element = (OutlineElement *) (*lptr)[offset] ;
  while (count < position)
    {
      if (element->is_expanded(f_data_handle) && element->has_children())
	{
	  if (count + element->children()->count_expanded(f_data_handle) >= position)
	    {
	      ON_DEBUG(cerr << "descend: " << element->display_as() << endl);
	      lptr = element->children();
	      offset = 0 ;
	    }
	  else
	    {
	      count += element->children()->count_expanded(f_data_handle) ;
	      offset++ ;
	    }
	}
      else
	{
	  offset++ ;			// go to the next one
	}
      element = (OutlineElement*) (*lptr)[offset] ;
      count++ ;
    }
  ON_DEBUG(cerr << "item_at ==> " << element->display_as() << endl);
  return element ;
}
