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
/* $XConsortium: IconObj.h /main/3 1995/11/06 09:41:52 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef ICONOBJ_H
#define ICONOBJ_H

#include "MotifUI.h"

typedef struct
{
   int name_width;
   int n_fields;
   int field_spacing;
   char **fields;
   int *fields_widths;
   LabelType *alignments;
   boolean *draw_fields;
   boolean *selected;
   boolean *active;
} IconFieldsRec, *IconFields, **IconFieldsList;

typedef enum {
   NORTHWEST_GRAVITY, 
   NORTH_GRAVITY, 
   NORTHEAST_GRAVITY, 
   WEST_GRAVITY, 
   CENTER_GRAVITY, 
   EAST_GRAVITY, 
   SOUTHWEST_GRAVITY, 
   SOUTH_GRAVITY, 
   SOUTHEAST_GRAVITY
} StateGravity;

class IconObj : public MotifUI {

 private:
   
   static void SingleClickCB(Widget, XtPointer, XtPointer);
   static void DoubleClickCB(Widget, XtPointer, XtPointer);

   char *_details;
   char *_iconFile;
   char *_topString;
   char *_bottomString;
   IconStyle _previous_style;
   Pixmap _smallPixmap;
   Pixmap _smallMask;
   Pixmap _largePixmap;
   Pixmap _largeMask;
   IconFields fields;
   StateGravity _state_gravity;
   char *_stateIconName;

   void SetDetail();
   void SetStateIconFile(IconStyle);
   void CreateIconObj(MotifUI *, char *, char *, char *, char *, char *,
		      char *, IconFields);

 protected:
   
   // Override SetView and SetOpen for icon
   virtual boolean SetIcon(IconStyle); 
   virtual boolean SetOpen(boolean); 
   virtual boolean SetName(char *); 


 public:

   IconObj(MotifUI *parent,
           char *name,
	   char *icon,
	   char *details = NULL,
	   char *topString = NULL,
	   char *bottomString = NULL,
	   IconFields fields = NULL);

   IconObj(char *category,
	   MotifUI *parent,
           char *name,
	   char *icon,
	   char *details = NULL,
	   char *topString = NULL,
	   char *bottomString = NULL,
	   IconFields fields = NULL);

   ~IconObj();

   void Details(char *);                           // Set details
   char *Details() { return _details; }            // Access details
   void TopString(char *);                         // Set top string
   char *TopString() { return _topString; }        // Access top string
   void BottomString(char *);                      // Set bottom string
   char *BottomString() { return _bottomString; }  // Access bottom string
   void IconFile(char *);                          // Set top string
   char *IconFile() { return _iconFile; }          // Access top string
   void StateIconFile(char *);                     // Set top string
   char *StateIconFile() { return _stateIconName; }// Access top string
   void StateIconGravity(StateGravity);
   StateGravity StateIconGravity() { return _state_gravity; }
   void Field(int index, char *string, int width, boolean visible = true,
	      boolean active = true);
   void Field(int index, char **string, int *width, boolean *visible,
	      boolean *active);
   int NumberFields();

   const UI_Class UIClass()           { return ICON; }
   const char *const UIClassName()    { return "IconObj"; }

};

#endif /* ICONOBJ_H */
