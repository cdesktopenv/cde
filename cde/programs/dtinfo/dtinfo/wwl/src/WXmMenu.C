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
/* $XConsortium: WXmMenu.cc /main/3 1996/06/11 17:03:45 cde-hal $ */

#ifndef STREAMH
#include <sstream>
using namespace std;
#endif

#ifndef _WXmMenu_h
#include <WWL/WXmMenu.h>
#endif

// The class WXmPopupMenu encapsulates a standard Motif popup menu.

WXmPopupMenu::WXmPopupMenu (WComposite& father, char* name, ArgList args,
			    Cardinal card)
: WXmRowColumn (XmCreatePopupMenu (Widget(father), name, args, card))
{
#ifdef STUPID
   if (name != NULL) {
       WXmLabelGadget label(widget, "title");
       label.LabelString(name);
   }
#endif
}

WXmPopupMenu::WXmPopupMenu (WComposite& father, char* name, WArgList args)
: WXmRowColumn (XmCreatePopupMenu (Widget(father), name, args.Args(),
				   args.NumArgs()))
{
#ifdef STUPID
   if (name != NULL) {
       WXmLabelGadget label(widget, "title");
       label.LabelString(name);
   }
#endif
}

WXmPopupMenu::WXmPopupMenu (WComposite& father, char* name, int automanage,
			    WArgList args)
: WXmRowColumn (XmCreatePopupMenu (Widget(father), name, args.Args(),
				   args.NumArgs()))
{
   automanage = 1;
#ifdef STUPID
   if (name != NULL) {
       WXmLabelGadget label(widget, "title");
       label.LabelString(name);
   }
#endif
   Manage();
}

/*?
Adds a push button to the popup menu and associates a callback function
to this button if the \var{proc} is not NULL. An arglist can be passed
to initialize some resources.
?*/
WXmPushButtonGadget
WXmPopupMenu :: AddPushButton (char* name, XtCallbackProc proc, caddr_t closure, ArgList args, Cardinal card)
{
	WXmPushButtonGadget pb(widget, name, args, card);

	if (proc)
		pb.AddActivateCallback(proc, closure);

	return pb;
}


/*?
Adds a push button to the popup menu and associates a callback method
to this button. The method is called with the object \var{wwl}. An
arglist can be passed to initialize some resources.
?*/
WXmPushButtonGadget
WXmPopupMenu :: AddPushButton (char* name, WWL* wwl, WWL_FUN fun, ArgList args, Cardinal card)
{
	WXmPushButtonGadget pb(widget, name, args, card);

	pb.SetActivateCallback(wwl, fun);

	return pb;
}
  
/*?class WXmPulldownMenu

The class \typ{WXmPulldownMenu} encapsulates a standard Motif pulldown menu.
?*/


/*?
Adds a push button to the pulldown menu and associates a callback function
to this button if the \var{proc} is not NULL. An arglist can be passed
to initialize some resources.
?*/
WXmPushButtonGadget
WXmPulldownMenu :: AddPushButton (char* name, XtCallbackProc proc, caddr_t closure, ArgList args, Cardinal card)
{
	WXmPushButtonGadget pb(widget, name, args, card);

	if (proc)
		pb.AddActivateCallback(proc, closure);

	return pb;
}


/*?
Adds a push button to the pulldown menu and associates a callback method
to this button. The method is called with the object \var{wwl}. An
arglist can be passed to initialize some resources.
?*/
WXmPushButtonGadget
WXmPulldownMenu :: AddPushButton (char* name, WWL* wwl, WWL_FUN fun, ArgList args, Cardinal card)
{
	WXmPushButtonGadget pb(widget, name, args, card);

	pb.SetActivateCallback(wwl, fun);

	return pb;
}

/*?class WXmOptionMenu

The class \typ{WXmOptionMenu} encapsulates a standard Motif option menu.
?*/

WXmOptionMenu :: WXmOptionMenu(
				WComposite& father, 
				char* name)
: WXmRowColumn (XmCreateOptionMenu(Widget(father), name, NULL, 0))
{
   WXmPulldownMenu pd(father, name); // create the pulldown
   SubMenuId(pd);  // Set the subMenuId to the new pulldown
}


/*
WXmOptionMenu :: AddPulldownMenu (
WComposite&      parent,
char*		name )
{ // 15mar92 need to add pulldown to option menu
   WXmPulldownMenu pd(parent, name); // create the pulldown
   SubMenuId(pd);  // Set the subMenuId to the new pulldown
   return pd;
} // Add a pulldown to the option menu
*/

/*?
Adds a push button to the option menu and associates a callback function
to this button if the \var{proc} is not NULL. An arglist can be passed
to initialize some resources.
?*/
WXmPushButtonGadget
WXmOptionMenu :: AddPushButton (
char*		name,
XtCallbackProc	proc,
caddr_t		closure,
ArgList		args,
Cardinal	card)
{ // 15mar92 modified to add PushButton to pulldown of option menu
   WXmPushButtonGadget pb(SubMenuId(), name, args, card);

   if (proc) {
      pb.AddActivateCallback(proc, closure);
   }

   return pb;
}


/*?
Adds a push button to the option menu and associates a callback method
to this button. The method is called with the object \var{wwl}. An
arglist can be passed to initialize some resources.
?*/
WXmPushButtonGadget
WXmOptionMenu :: AddPushButton (
char*		name,
WWL*		wwl,
WWL_FUN		fun,
ArgList		args,
Cardinal	card)
{ // 15mar92 modified to add PushButton to pulldown of option menu

   WXmPushButtonGadget pb(SubMenuId(), name, args, card);

   pb.SetActivateCallback(wwl, fun);

   return pb;
}

/*?
Returns the cascade button gadget associated with the option menu.
?*/
WXmCascadeButtonGadget
WXmOptionMenu :: OptionButtonGadget ()
{
   WXmCascadeButtonGadget cb(XmOptionButtonGadget(widget));

   return cb;
}

/*?
Returns the label gadget associated with the option menu.
?*/
WXmLabelGadget
WXmOptionMenu :: OptionLabelGadget ()
{
   WXmLabelGadget cb(XmOptionLabelGadget(widget));

   return cb;
}

/*?class WXmMenuBar

This class encapsulates a standard menu bar.
?*/


/*?
Adds a cascade button to the menu bar. A mnemonic character can also
be added.
?*/
WXmCascadeButton
WXmMenuBar :: AddCascadeButton (char* name, char mnemonic)
{
	WXmCascadeButton cb(widget, name);
	if (mnemonic) {
		cb.Mnemonic(mnemonic);
	}
	return cb;
}

#ifdef DOC
/*?
Add a cascade menu to the menu bar.
?*/
void
WXmMenuBar :: AddCascadeMenu (char* name, char mnemonic, WXmPulldownMenu& menu)
{
}

#endif


