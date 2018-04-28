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
/* $TOG: DtPrtProps.C /main/4 1998/07/24 16:14:02 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtPrtProps.h"
#include "DtFindSet.h"
#include "DtMainW.h"
#include "DtPrinterIcon.h"
#include "Button.h"
#include "Container.h"
#include "Group.h"
#include "IconObj.h"
#include "LabelObj.h"
#include "Sep.h"

#include "Dt/Action.h"

#include "dtprintinfomsg.h"

#include <stdio.h>
#include <unistd.h> // This is for the getpid function
#include <stdlib.h> // This is for the getenv function

DtPrtProps::DtPrtProps(DtMainW *parent, char *name, DtPrinterIcon *_printer)
	: Dialog(parent, name)
{
   mainw = parent;
   printer = _printer;
   _iconFile = STRDUP(printer->IconFile());
   findSetD = NULL;
   _has_been_posted = false;

   int i, captionWidth = 0, width, columns = 0;
   int n_attributes = printer->QueueObj()->NumAttributes();
   Attribute **attributes = printer->QueueObj()->Attributes();
   if ((width = StringWidth(MESSAGE(IconLabelL))) > captionWidth)
      captionWidth = width;
   if ((width = StringWidth(MESSAGE(IconGroupTitleL))) > captionWidth)
      captionWidth = width;
   if ((width = StringWidth(MESSAGE(DescriptionL))) > captionWidth)
      captionWidth = width;
   if ((width = strlen(printer->Name())) > columns)
      columns = width;
   for (i = 0; i < n_attributes; i++)
    {
      if ((width = StringWidth(attributes[i]->DisplayName)) > captionWidth)
	 captionWidth = width;
      if ((width = strlen(attributes[i]->DisplayValue)) > columns)
	 columns = width;
    }

   icon_prompt = new Prompt(this, MESSAGE(IconLabelL), true, STRING_PROMPT,
		            (char *)printer->Name(), NULL,
		            NULL, true, columns, 1, captionWidth + 8);
   icon_prompt->AttachLeft();
   icon_prompt->AttachRight();
   icon_prompt->AttachTop(5);
		       
   Container *form = new Container(this, "form", FORM);
   icon_group = new Group(form, NULL, FORM_BOX);
   LabelObj *label = new LabelObj(form, MESSAGE(IconGroupTitleL),
				  RIGHT_JUSTIFIED);
   label->AttachTop();
   label->AttachBottom();
   label->AttachLeft();
   label->Width(captionWidth + 8);
   icon_group->AttachTop();
   icon_group->AttachBottom();
   icon_group->AttachRight();
   icon_group->AttachLeft(label);

   // Creation of Large Icon
   largeIcon = new IconObj(icon_group, NULL, printer->IconFile());
   largeIcon->IconView(VERY_LARGE_ICON);
   largeIcon->AttachLeft(10);
   largeIcon->AttachTop(10);
   largeIcon->AttachBottom(10);

   // Creation of Medium Icon
   mediumIcon = new IconObj(icon_group, NULL, printer->IconFile());
   mediumIcon->IconView(MEDIUM_ICON);
   mediumIcon->AttachLeft(largeIcon, 10);
   mediumIcon->AttachBottom(10);

   // Creation of Small Icon
   smallIcon = new IconObj(icon_group, NULL, printer->IconFile());
   smallIcon->IconView(SMALL_ICON);
   smallIcon->AttachLeft(mediumIcon, 10);
   smallIcon->AttachBottom(10);

   // Creation of Find Set Button
   find_set = new Button(icon_group, MESSAGE(FindSetL), PUSH_BUTTON, FindSetCB,
			 this);
    
   find_set->AttachRight(10);
   find_set->AttachLeft(smallIcon, 10);
   find_set->AttachBottom(10);

   form->AttachLeft();
   form->AttachRight();
   form->AttachTop(icon_prompt, 5);

   Container *rc = new Container(this, "rc", VERTICAL_ROW_COLUMN);
   for (i = 0; i < n_attributes; i++)
    {
      new Prompt(rc, attributes[i]->DisplayName, false, STRING_PROMPT,
		 attributes[i]->DisplayValue, NULL,
		 NULL, true, columns, 1, captionWidth + 8);
      if (mainw->PrinterAppMode() != CONFIG_PRINTERS)
       {
	 char *value;
	 if (i == 0)
	    value = _printer->PrintQueueUp() ? MESSAGE(UpL) : MESSAGE(DownL);
	 else
#ifdef aix
	    value = _printer->PrintDeviceUp(i - 1) ? MESSAGE(UpL) :
						     MESSAGE(DownL);
#else
	    value = _printer->PrintDeviceUp() ? MESSAGE(UpL) : MESSAGE(DownL);
#endif
         new Prompt(rc, MESSAGE(PrinterStatusL), false, STRING_PROMPT,
		    value, NULL, NULL, true, columns, 1, captionWidth + 8);
       }
    }

   rc->AttachRight();
   rc->AttachLeft();
   rc->AttachBottom();

   Sep *sep = new Sep(this);
   sep->AttachRight();
   sep->AttachLeft();
   sep->AttachBottom(rc, 5);

   description = new Prompt(this, MESSAGE(DescriptionL), false,
			    MULTI_LINE_STRING_PROMPT, printer->Description(),
			    NULL, NULL, true, columns, 3, captionWidth + 8);
   description->AttachLeft();
   description->AttachRight();
   description->AttachTop(form, 5);
   description->AttachBottom(sep, 5);

   ok = new Button(this, MESSAGE(OKL), PUSH_BUTTON, OkCB, this);
   cancel = new Button(this, MESSAGE(CancelL), PUSH_BUTTON, CancelCB, this);
   help = new Button(this, MESSAGE(HelpL), PUSH_BUTTON, HelpCB, this);
   DefaultButton(ok);
   CancelButton(cancel);
}

DtPrtProps::~DtPrtProps()
{
   delete _iconFile;
}

boolean DtPrtProps::SetVisiblity(boolean flag)
{
   Dialog::SetVisiblity(flag);
   if (_has_been_posted == false)
    {
      _has_been_posted = true;
      find_set->Height(find_set->Height() + 6);
    }
   else if (flag)
      Reset();
   return true;
}

void DtPrtProps::Reset()
{
   if (strcmp(printer->IconFile(), _iconFile))
      SetActionIcons(printer->IconFile());
   icon_prompt->Value((char *)printer->Name());
   description->Value(printer->Description());
}

void DtPrtProps::Apply()
{
   char *buf;
   int buf_len;
   char *iconName = (char *)IconLabel();
   char *iconFile = (char *)IconFileName();

   if (!iconName || *iconName == '\0')
      iconName = (char *)printer->Name();
   if (!iconFile || *iconFile == '\0')
      iconFile = (char *)PRINTER_ICON_FILE;

   if (!strcmp(iconName, printer->Name()) &&
       !strcmp(iconFile, printer->IconFile()))
    {
      // return since nothing changed
      return;
    }

   char *save_msg = strdup(mainw->status_line->Name());
   mainw->WorkingCursor(true);
   mainw->status_line->Name(MESSAGE(UpdatingActionsL));
   Refresh();
   buf_len = strlen(printer->QueueObj()->Name()) +
             (2 * strlen(printer->HomeDir())) + 50;
   if (buf_len < 200)
      buf_len = 200;
   buf = new char [buf_len];
   FILE *fp_src, *fp_dest;
   char *filename = printer->CreateActionFile();
   sprintf(buf, "%s.%ld", filename, (long)getpid());
   if (fp_dest = fopen(buf, "w"))
    {
      if (fp_src = fopen(filename, "r"))
       {
         while (fgets(buf, buf_len, fp_src))
          {
            if (strstr(buf, "LABEL"))
               fprintf(fp_dest, "        LABEL                   %s\n",
                       iconName);
            else if (strstr(buf, "ICON"))
               fprintf(fp_dest, "        ICON                    %s\n",
                       iconFile);
            else
               fprintf(fp_dest, "%s", buf);
          }
         fclose(fp_src);
       }
    }
   fclose(fp_dest);
   sprintf(buf, "%s.%ld", filename, (long)getpid());
   rename(buf, filename);
   DtActionInvoke(((AnyUI *)mainw->Parent())->BaseWidget(), "ReloadActions",
                  NULL, 0, NULL, NULL, NULL, True, NULL, NULL);
   delete [] buf;
   mainw->WorkingCursor(false);
   mainw->status_line->Name(save_msg);
}

void DtPrtProps::CloseCB()
{
   Reset();
   Visible(false);
}

void DtPrtProps::OkCB(void *data)
{
   DtPrtProps *obj = (DtPrtProps *) data;
   
   obj->Apply();
   obj->Visible(false);
}

void DtPrtProps::ApplyCB(void *data)
{
   DtPrtProps *obj = (DtPrtProps *) data;
   obj->Apply();
}

void DtPrtProps::CancelCB(void *data)
{
   DtPrtProps *obj = (DtPrtProps *) data;
   
   obj->Reset();
   obj->Visible(false);
}

void DtPrtProps::ResetCB(void *data)
{
   DtPrtProps *obj = (DtPrtProps *) data;
   
   obj->Reset();
}

void DtPrtProps::HelpCB(void *data)
{
   DtPrtProps *obj = (DtPrtProps *) data;
   obj->HandleHelpRequest();
}

boolean DtPrtProps::HandleHelpRequest()
{
   mainw->DisplayHelp((char *)PROPS_PRINTER_ID);
   return true;
}

void DtPrtProps::SetActionIcons(const char *icon)
{
   delete _iconFile;
   _iconFile = STRDUP(icon);
   largeIcon->IconFile((char*)icon);
   mediumIcon->IconFile((char*)icon);
   smallIcon->IconFile((char*)icon);
}

void DtPrtProps::CallerCB(BaseUI *caller, char *iconFile)
{
   DtPrtProps *obj = (DtPrtProps *)caller;
   delete obj->_iconFile;
   obj->_iconFile = STRDUP(iconFile);
   obj->smallIcon->IconFile(iconFile);
   obj->mediumIcon->IconFile(iconFile);
   obj->largeIcon->IconFile(iconFile);
}

void DtPrtProps::FindSetCB(void *data)
{
   DtPrtProps *obj = (DtPrtProps *)data;
   if (!obj->mainw->findSetD)
    {
      char *name = new char [strlen(obj->mainw->Name()) +
			     strlen(MESSAGE(FindSetTitleL))];
      sprintf(name, MESSAGE(FindSetTitleL), obj->mainw->Name());
      obj->mainw->findSetD = new DtFindSet(obj->mainw, name,
					   &DtPrtProps::CallerCB);
      delete [] name;
    }
   obj->mainw->findSetD->Caller(obj);
   obj->mainw->findSetD->Visible(true);
}
