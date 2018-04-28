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
/* $TOG: IconObj.C /main/4 1998/07/24 16:15:38 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "IconObj.h"

#include "Icon.h"
#include "WorkArea.h"
#include <stdlib.h>
#include <stdio.h>

// #define LargeIconJustification XmALIGNMENT_CENTER
#define LargeIconJustification XmALIGNMENT_BEGINNING

IconObj::IconObj(MotifUI *parent, char *name, char *iconFile, char *details,
		 char *topString, char *bottomString, IconFields fields)
	: MotifUI(parent, name, NULL)
{
   CreateIconObj(parent, name, NULL, iconFile, details, topString,
		 bottomString, fields);
}

IconObj::IconObj(char *category, MotifUI *parent, char *name, char *iconFile,
                 char *details, char *topString, char *bottomString,
		 IconFields fields)
	: MotifUI(parent, name, category)
{
   CreateIconObj(parent, name, category, iconFile, details, topString,
		 bottomString, fields);
}

IconObj::~IconObj()
{
   delete _topString;
   delete _bottomString;
   delete _details;
   delete _iconFile;
   delete _stateIconName;
}

void IconObj::CreateIconObj(MotifUI *parent, char *name, char * /*category*/,
                            char *iconFile, char *details, char *topString,
			    char *bottomString, IconFields _fields)
{
   Pixmap pixmap;
   Pixmap mask;
   Pixel bg;
   Widget p, super_node;
   XmString xm_string, xm_topString, xm_bottomString;
   char *s;
   int shrinkOutline = false;
   int pixmapPlacement;
   int alignment;
   int isOpened;
   GuiIconFields gui_fields;

   if (fields = _fields)
    {
      int i;
      gui_fields = new GuiIconFieldsStruct;
      gui_fields->free_data = True;
      gui_fields->name_width = fields->name_width;
      gui_fields->field_spacing = fields->field_spacing;
      gui_fields->n_fields = fields->n_fields;
      gui_fields->selected = new Boolean[fields->n_fields];
      gui_fields->draw_fields = new Boolean[fields->n_fields];
      gui_fields->active = new Boolean[fields->n_fields];
      gui_fields->widths = new Dimension[fields->n_fields];
      gui_fields->alignments = new unsigned char[fields->n_fields];
      gui_fields->fields = new XmString[fields->n_fields];

      unsigned char alignment;
      for (i = 0; i < fields->n_fields; i++)
       {
	 gui_fields->widths[i] = fields->fields_widths[i];
	 switch (fields->alignments[i])
	 {
	 case LEFT_JUSTIFIED: alignment = XmALIGNMENT_BEGINNING; break;
	 case CENTERED: alignment = XmALIGNMENT_CENTER; break;
	 case RIGHT_JUSTIFIED: alignment = XmALIGNMENT_END; break;
	 }
	 gui_fields->alignments[i] = alignment;
	 if (fields->active)
	    gui_fields->active[i] = (Boolean)fields->active[i];
	 else
	    gui_fields->active[i] = True;
	 if (fields->draw_fields)
	    gui_fields->draw_fields[i] = (Boolean)fields->draw_fields[i];
	 else
	    gui_fields->draw_fields[i] = True;
	 gui_fields->selected[i] = False;
	 gui_fields->fields[i] = StringCreate(fields->fields[i]);
       }
    }
   else
      gui_fields = NULL;

   // Get small and large pixmaps and masks
   char icon_type = 'p';
   if (depth == 1)
       icon_type = 'b';
   _iconFile = new char [strlen(iconFile) + 6];
   _topString = STRDUP(topString);
   _bottomString = STRDUP(bottomString);
   _details = STRDUP(details);

   if (display == NULL)
    {
      strcpy(_iconFile, iconFile);
      _stateIconName = NULL;
      return;
    }

   sprintf(_iconFile, "%s.t.%cm", iconFile, icon_type);
   GetPixmaps(parent->BaseWidget(), _iconFile, &_smallPixmap, &_smallMask);
   sprintf(_iconFile, "%s.m.%cm", iconFile, icon_type);
   GetPixmaps(parent->BaseWidget(), _iconFile, &_largePixmap, &_largeMask);
   strcpy(_iconFile, iconFile);

   s = name;

   BaseUI *par = Parent();
   if (par && par->UISubClass() == ICON_LIST ||
       par->UISubClass() == SCROLLED_ICON_LIST)
      isOpened = true;
   else
      isOpened = false;
   switch (IconView())
    {
     case NAME_ONLY: 
        pixmapPlacement = GuiPIXMAP_LEFT;
        alignment = XmALIGNMENT_BEGINNING;
	pixmap = mask = XmUNSPECIFIED_PIXMAP;
	break;
     case LARGE_ICON:
	if (ContainerView() == TREE ||
	    (Parent()->UIClass() == CONTAINER &&
	     Parent()->UISubClass() == SCROLLED_VERTICAL_ROW_COLUMN))
	 {
	   isOpened = true;
           pixmapPlacement = GuiPIXMAP_LEFT;
	 }
	else
           pixmapPlacement = GuiPIXMAP_TOP;
	if (isOpened)
           alignment = XmALIGNMENT_BEGINNING;
	else
	   alignment = LargeIconJustification;
        shrinkOutline = true;
	pixmap = _largePixmap;
	mask = _largeMask;
	break;
     case DETAILS:
        s = new char [STRLEN(name) + STRLEN(details) + 2];
	if (details)
           sprintf(s, "%s %s", name, details);
	else
           strcpy(s, name);
	// no break
     case SMALL_ICON:
        alignment = XmALIGNMENT_BEGINNING;
        pixmapPlacement = GuiPIXMAP_LEFT;
	pixmap = _smallPixmap;
	mask = _smallMask;
	break;
    }
   // Get Parent and colors
   p = parent->InnerWidget();
   if (!XtIsComposite(p))
      p = XtParent(p);
   XtVaGetValues(p, XmNbackground, &bg, NULL);

   // If p is a icon then set superNode to it, otherwise set superNode to NULL
   super_node = GuiIsIcon(parent->BaseWidget()) ? parent->BaseWidget() : NULL;
   xm_string = StringCreate(s);
   xm_topString = StringCreate(topString);
   xm_bottomString = StringCreate(bottomString);
   Pixel text_select_color = black;
   Pixel select_color = white;
   if (select_color == bg)
    {
      text_select_color = white;
      select_color = black;
    }

   _w = XtVaCreateManagedWidget(name, iconWidgetClass, p, GuiNiconMask, mask,
				XmNlabelPixmap, pixmap, GuiNisOpened, isOpened,
				XmNalignment, alignment,
				GuiNsuperNode, super_node,
				GuiNpixmapPlacement, pixmapPlacement, 
				GuiNshrinkOutline, shrinkOutline,
				GuiNselectColorPersistent, true,
				GuiNselectColor, select_color,
				GuiNtextSelectColor, text_select_color,
				XmNlabelString, xm_string, 
                                GuiNfields, gui_fields,
				XmNbackground, bg,
				GuiNtopLabelString, xm_topString,
				GuiNbottomLabelString, xm_bottomString,
				XmNuserData, this, NULL);

   StringFree(xm_string);
   StringFree(xm_topString);
   StringFree(xm_bottomString);
   // Delete s if style = details
   if (IconView() == DETAILS)
      delete s;
   _previous_style = IconView();
   XtAddCallback(_w, GuiNsingleClickCallback, &IconObj::SingleClickCB, 
      (XtPointer) this);
   XtAddCallback(_w, GuiNdoubleClickCallback, &IconObj::DoubleClickCB, 
      (XtPointer) this);
   InstallHelpCB();
   _stateIconName = NULL;
}

void IconObj::StateIconFile(char *stateIconName)
{
   delete _stateIconName;
   _stateIconName = STRDUP(stateIconName);
   SetStateIconFile(IconView());
}

void IconObj::SetStateIconFile(IconStyle style)
{
   Pixmap _statePixmap = XmUNSPECIFIED_PIXMAP;
   Pixmap _stateMask = XmUNSPECIFIED_PIXMAP;
   if (_stateIconName && style != NAME_ONLY)
    {
      char icon_type = 'p';
      if (depth == 1)
          icon_type = 'b';
      char *tmp = new char[strlen(_stateIconName) + 6];
      if (style == LARGE_ICON)
         sprintf(tmp, "%s.m.%cm", _stateIconName, icon_type);
      else
         sprintf(tmp, "%s.t.%cm", _stateIconName, icon_type);
      GetPixmaps(_w, tmp, &_statePixmap, &_stateMask);
      delete [] tmp;
    }
   XtVaSetValues(_w, GuiNstatePixmap, _statePixmap,
		 GuiNstateIconMask, _stateMask, NULL);
}

void IconObj::StateIconGravity(StateGravity stateGravity)
{
   XtVaSetValues(_w, GuiNstateGravity, stateGravity, NULL);
   _state_gravity = stateGravity;
}

int IconObj::NumberFields()
{
   GuiIconFields gui_fields;
   XtVaGetValues(_w, GuiNfields, &gui_fields, NULL);
   if (gui_fields)
      return gui_fields->n_fields;
   else
      return 0;
}

void IconObj::Field(int index, char **string, int *width, boolean *visible, 
		    boolean *active)
{
   XmString s;
   Dimension wid;
   unsigned char alignment;
   Boolean draw_field;
   Boolean selected;
   Boolean _active;

   GuiIconGetField(_w, index, &s, &wid, &alignment, &draw_field, &selected, 
		   &_active);
   if (width)
      *width = (int)wid;
   if (string)
      *string = StringExtract(s);
   if (visible)
      *visible = draw_field ? true : false;
   if (active)
      *active = _active ? true : false;
}

void IconObj::Field(int index, char *string, int width, boolean visible, 
		    boolean active)
{
   XmString s;
   Dimension wid;
   unsigned char alignment;
   Boolean draw_field;
   Boolean selected;
   Boolean _active;

   GuiIconGetField(_w, index, &s, &wid, &alignment, &draw_field, &selected, 
		   &_active);
   draw_field = visible;
   _active = active;
   wid = (Dimension) width;
   s = StringCreate(string);
   GuiIconSetField(_w, index, s, wid, alignment, draw_field, selected, _active);
}

void IconObj::BottomString(char *bottomString)
{
   delete _bottomString;
   _bottomString = STRDUP(bottomString);
   XmString xm_string = StringCreate(bottomString);
   XtVaSetValues(_w, GuiNbottomLabelString, xm_string, NULL);
   StringFree(xm_string);
}

void IconObj::TopString(char *topString)
{
   delete _topString;
   _topString = STRDUP(topString);
   XmString xm_string = StringCreate(topString);
   XtVaSetValues(_w, GuiNtopLabelString, xm_string, NULL);
   StringFree(xm_string);
}

void IconObj::IconFile(char *iconFile)
{
   delete _iconFile;
   _iconFile = new char [strlen(iconFile) + 6];

   // Get small and large pixmaps and masks
   char icon_type = 'p';
   if (depth == 1)
       icon_type = 'b';
   _iconFile = new char [strlen(iconFile) + 6];
   sprintf(_iconFile, "%s.t.%cm", iconFile, icon_type);
   GetPixmaps(_w, _iconFile, &_smallPixmap, &_smallMask);
   sprintf(_iconFile, "%s.m.%cm", iconFile, icon_type);
   GetPixmaps(_w, _iconFile, &_largePixmap, &_largeMask);

   Pixmap vlargePixmap, vlargeMask;
   switch (IconView())
   {
   case VERY_LARGE_ICON:
      sprintf(_iconFile, "%s.l.%cm", iconFile, icon_type);
      GetPixmaps(_w, _iconFile, &vlargePixmap, &vlargeMask);
      XtVaSetValues(_w, XmNlabelPixmap, vlargePixmap, GuiNiconMask, vlargeMask,
		    NULL);
      break;
   case LARGE_ICON:
   case MEDIUM_ICON:
      XtVaSetValues(_w, XmNlabelPixmap, _largePixmap, GuiNiconMask, _largeMask,
		    NULL);
      break;
   case SMALL_ICON:
   case TINY_ICON:
      XtVaSetValues(_w, XmNlabelPixmap, _smallPixmap, GuiNiconMask, _smallMask,
		    NULL);
      break;
   }
   strcpy(_iconFile, iconFile);
}

void IconObj::SetDetail()
{
   if (!_details)
      return;

   char *name = new char [strlen(_name) + strlen(_details) + 3];

   sprintf(name, "%s %s", _name, _details);
   XmString xm_string = StringCreate(name);
   XtVaSetValues(_w, XmNlabelString, xm_string, NULL);
   StringFree(xm_string);
   delete [] name;
}

void IconObj::Details(char *details)
{
   delete _details;
   _details = STRDUP(details);
   if (IconView() == DETAILS)
       SetDetail();
}

boolean IconObj::SetName(char *_name)
{
   return MotifUI::SetName(_name);
}

boolean IconObj::SetOpen(boolean flag)
{
   int pixmapPlacement;
   int alignment;
   int isOpened;

   BaseUI *parent = Parent();
   if (ContainerView() == TREE)
      isOpened = flag;
   else if (parent && parent->UISubClass() == ICON_LIST ||
            parent->UISubClass() == SCROLLED_ICON_LIST)
      isOpened = true;
   else
      isOpened = flag;
   if (IconView() == LARGE_ICON)
    {
      if (ContainerView() == TREE ||
	  (parent->UIClass() == CONTAINER &&
	   parent->UISubClass() == SCROLLED_VERTICAL_ROW_COLUMN))
       {
         isOpened = true;
         pixmapPlacement = GuiPIXMAP_LEFT;
       }
      else
       {
         pixmapPlacement = GuiPIXMAP_TOP;
       }
      if (isOpened)
         alignment = XmALIGNMENT_BEGINNING;
      else
	 alignment = LargeIconJustification;
    }
   else
    {
      alignment = XmALIGNMENT_BEGINNING;
      pixmapPlacement = GuiPIXMAP_LEFT;
    }
   XtVaSetValues(_w, XmNalignment, alignment, GuiNisOpened, isOpened,
		 GuiNpixmapPlacement, pixmapPlacement, NULL);
   return true;
}

boolean IconObj::SetIcon(IconStyle style)
{
   Pixmap pixmap, mask;
   int shrinkOutline = false;
   int pixmapPlacement;
   int alignment;
   int isOpened;

   BaseUI *parent = Parent();
   if (ContainerView() == TREE)
      isOpened = true;
   else if (parent && parent->UISubClass() == ICON_LIST ||
            parent->UISubClass() == SCROLLED_ICON_LIST)
      isOpened = true;
   else
      isOpened = Open();
   switch (style)
    {
     case NAME_ONLY: 
        pixmapPlacement = GuiPIXMAP_LEFT;
        alignment = XmALIGNMENT_BEGINNING;
	if (_name)
	 {
	   pixmap = XmUNSPECIFIED_PIXMAP;
	   mask = XmUNSPECIFIED_PIXMAP;
	 }
	else
	 {
	   pixmap = _smallPixmap;
	   mask = _smallMask;
	 }
	break;
     case VERY_LARGE_ICON:
     case LARGE_ICON:
     case MEDIUM_ICON:
	if (ContainerView() == TREE ||
	    (parent->UIClass() == CONTAINER &&
	     parent->UISubClass() == SCROLLED_VERTICAL_ROW_COLUMN))
	 {
	   isOpened = true;
           pixmapPlacement = GuiPIXMAP_LEFT;
	 }
	else
           pixmapPlacement = GuiPIXMAP_TOP;
	if (isOpened)
           alignment = XmALIGNMENT_BEGINNING;
	else
	   alignment = LargeIconJustification;
        shrinkOutline = true;
        if (style == VERY_LARGE_ICON)
	 {
            if (depth == 1)
               strcat(_iconFile, ".l.bm");
	    else
               strcat(_iconFile, ".l.pm");
            GetPixmaps(_w, _iconFile, &pixmap, &mask);
	    _iconFile[strlen(_iconFile) - 5] = '\0';
	 }
	else
	 {
	   pixmap = _largePixmap;
	   mask = _largeMask;
	 }
	break;
     case DETAILS:
	SetDetail();
	// no break
     case SMALL_ICON:
     case TINY_ICON:
        alignment = XmALIGNMENT_BEGINNING;
        pixmapPlacement = GuiPIXMAP_LEFT;
	pixmap = _smallPixmap;
	mask = _smallMask;
	break;
    }
   if (_previous_style == DETAILS)
    {
       XmString xm_string = StringCreate(_name);
       XtVaSetValues(_w, XmNlabelPixmap, pixmap, GuiNiconMask, mask,
                     XmNlabelString, xm_string, XmNalignment, alignment,
		     GuiNpixmapPlacement, pixmapPlacement, 
		     GuiNisOpened, isOpened,
                     GuiNshrinkOutline, shrinkOutline, NULL);
       StringFree(xm_string);
    }
   else
      XtVaSetValues(_w, XmNlabelPixmap, pixmap, GuiNiconMask, mask,
		    GuiNpixmapPlacement, pixmapPlacement,
		    XmNalignment, alignment, GuiNisOpened, isOpened,
                    GuiNshrinkOutline, shrinkOutline, NULL);

   _previous_style = style;
   if (_stateIconName)
      SetStateIconFile(style);

   return true;
}


void IconObj::SingleClickCB(Widget, 
		            XtPointer client_data, 
			    XtPointer)
{
   IconObj *obj = (IconObj *) client_data;

   if (!obj->Name())
      return;
   if (obj->Selected())
      obj->Selected(false);
   else
      obj->Selected(true);
}

void IconObj::DoubleClickCB(Widget, 
		            XtPointer client_data, 
			    XtPointer)
{
   IconObj *obj = (IconObj *) client_data;

   if (obj->Name())
    {
      if (obj->Selected())
         obj->Selected(false);
      else
         obj->Selected(true);
      XmUpdateDisplay(obj->BaseWidget());
    }

   if (obj->Open())
      obj->Open(false);
   else
      obj->Open(true);
}
