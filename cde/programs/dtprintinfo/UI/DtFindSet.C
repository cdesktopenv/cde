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
/* $TOG: DtFindSet.C /main/5 1998/07/24 16:12:36 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtFindSet.h"
#include "DtPrtProps.h"
#include "DtMainW.h"
#include "Button.h"
#include "Container.h"
#include "IconObj.h"
#include "LabelObj.h"
#include "ComboBoxObj.h"
#include "HelpSystem.h"
#include "Invoke.h"

#include "dtprintinfomsg.h"

#include <stdio.h>
#include <pwd.h>
#include <unistd.h> // This is for the getuid function
#include <stdlib.h> // This is for the getenv function

const char *GET_DIRS = 
   "lang=${LANG:-C} ; "
   "for i in $(echo $XMICONSEARCHPATH | sed -e 's/:/ /g' -e s/%L/$lang/g) ; "
     "do "
        "dirname $i ; "
     "done | sort -u";

// Subclass IconObj in order to disable SetOpen method
class FindSetIcon : public IconObj
{
 public:

   FindSetIcon(AnyUI *parent, char *name, char *icon) :
      IconObj(parent, name, icon) { }
   boolean SetOpen(boolean) { return true; }
};

DtFindSet::DtFindSet(DtMainW *parent, char *name, CallerCallback _callback)
	: Dialog(parent, name, MODAL)
{
   mainw = parent;
   callback = _callback;
   last_position = -1;
   helpSystem = NULL;
   char *output;
   Invoke *_thread = new Invoke(GET_DIRS, &output);
   char *s, *s1 = output;
   s = s1;
   n_dirs = 0;
   while (s && (s = strchr(s1, '\n')))
    {
      n_dirs++;
      s1 = s + 1;
    }
   if (_thread->status || n_dirs == 0)
    {
      delete [] output;
      struct passwd * pwInfo;
      char *home = getenv("HOME");
      if (home == NULL || strlen(home) == 0)
       {
         pwInfo = getpwuid(getuid());
         home = pwInfo->pw_dir;
       }
      output = new char[strlen(home) + 80];
      n_dirs = 3;
      sprintf(output, "%s/.dt/icons\n"
                      "/usr/dt/appconfig/icons/C\n"
                      "/etc/dt/appconfig/icons/C\n", (home[1] ? home : "" ));
    }
   dirs = new char *[n_dirs];
   filenames = new FileNames [n_dirs];
   s = output;
   int i;
   for (i = 0; i < n_dirs; i++)
    {
      s1 = s;
      s = strchr(s, '\n');
      *s++ = '\0';
      dirs[i] = strdup(s1);
      filenames[i] = new FileNamesStruct;
      filenames[i]->icons = NULL;
      filenames[i]->read_it = true;
      filenames[i]->n_icons = 0;
    }
   delete [] output;
   delete _thread;

   comboBox = new ComboBoxObj(this, ComboBoxCB, MESSAGE(IconFoldersL), dirs,
			      n_dirs);

   icon_label = new LabelObj(this, MESSAGE(IconTitleL));
   icons = new Container(this, "icons", SCROLLED_WORK_AREA);
   icons->Height(250);
   empty = new LabelObj(icons, MESSAGE(EmptyL));
   empty->Visible(false);

   comboBox->AttachTop(5);
   comboBox->AttachLeft();
   comboBox->AttachRight();

   icon_label->AttachTop(comboBox, 10);
   icon_label->AttachLeft();
   icon_label->AttachRight();
   icons->AttachTop(icon_label);
   icons->AttachRight();
   icons->AttachLeft();
   icons->AttachBottom();

   ok = new Button(this, MESSAGE(OKL), PUSH_BUTTON, OkCB, this);
   cancel = new Button(this, MESSAGE(CancelL), PUSH_BUTTON, CancelCB, this);
   help = new Button(this, MESSAGE(HelpL), PUSH_BUTTON, HelpCB, this);

   DefaultButton(ok);
   CancelButton(cancel);
}

DtFindSet::~DtFindSet()
{
   int i;
   for (i = 0; i < n_dirs; i++)
    {
      delete dirs[i];
      delete filenames[i];
    }
   delete []dirs;
   delete []filenames;
}

void DtFindSet::InitComboBox(BaseUI *obj, void * /*data*/)
{
   DtFindSet *findSet = (DtFindSet *)obj;
   findSet->Refresh();
   ComboBoxCB(findSet->comboBox, findSet->dirs[0], 1);
}

boolean DtFindSet::SetVisiblity(boolean flag)
{
   if (last_position == -1)
      AddTimeOut(&DtFindSet::InitComboBox, NULL, 1000);
   Dialog::SetVisiblity(flag);
   return true;
}

void DtFindSet::Reset()
{
}

void DtFindSet::Apply()
{
   BaseUI **items;
   int n_items;
   icons->Selection(&n_items, &items);
   if (n_items && callback)
    {
      FindSetIcon *icon = (FindSetIcon *)items[0];
      char *iconfile;
      if (iconfile = strrchr(icon->IconFile(), '/'))
	 iconfile++;
      else
         iconfile = icon->IconFile();
      (*callback)(caller, iconfile);
    }
   delete []items;
}

void DtFindSet::CloseCB()
{
   Reset();
   Visible(false);
}

void DtFindSet::OkCB(void *data)
{
   DtFindSet *obj = (DtFindSet *) data;
   
   obj->Apply();
   obj->Visible(false);
}

void DtFindSet::ApplyCB(void *data)
{
   DtFindSet *obj = (DtFindSet *) data;
   obj->Apply();
}

void DtFindSet::CancelCB(void *data)
{
   DtFindSet *obj = (DtFindSet *) data;
   
   obj->Reset();
   obj->Visible(false);
}

void DtFindSet::ResetCB(void *data)
{
   DtFindSet *obj = (DtFindSet *) data;
   
   obj->Reset();
}

void DtFindSet::HelpCB(void *data)
{
   DtFindSet *obj = (DtFindSet *) data;
   obj->HandleHelpRequest();
}

boolean DtFindSet::HandleHelpRequest()
{
   char old_msg[200];

   snprintf(old_msg, sizeof(old_msg), "%s", mainw->status_line->Name());
   mainw->status_line->Name(MESSAGE(GettingHelpL));
   mainw->WorkingCursor(true);

   if (!helpSystem)
    {
      char *title = new char[120];

      sprintf(title, "%s - %s", mainw->Name(), MESSAGE(HelpL));

      helpSystem = new HelpSystem(this, title, "Printmgr", "FindSetDE",
				  QUICK_HELP);
      helpSystem->Visible(true);
      delete [] title;
    }
   else
    {
      helpSystem->Visible(true);
      helpSystem->Refresh();
      helpSystem->HelpVolume("Printmgr", "FindSetDE");
    }
   mainw->status_line->Name(old_msg);
   mainw->WorkingCursor(false);
   return true;
}

void DtFindSet::ComboBoxCB(ComboBoxObj *obj, char *dir, int position)
{
   DtFindSet *findSet = (DtFindSet *)obj->Parent();
   if (findSet->last_position == position)
      return;

   FileNames filenames;
   int i;
   char *buf = new char[400]; // This allows for a directory len of 240 bytes

   findSet->PointerShape(HOUR_GLASS_CURSOR);
   findSet->icons->BeginUpdate();
   findSet->icons->SelectAll(false);
   if (findSet->last_position > 0)
    {
      filenames = findSet->filenames[findSet->last_position - 1];
      FindSetIcon **icon = filenames->icons;
      for (i = 0; i < filenames->n_icons; i++, icon++)
         (*icon)->Visible(false);
    }
   filenames = findSet->filenames[position - 1];
   if (filenames->read_it)
    {
      sprintf(buf, "cd %s 2> /dev/null ; "
		   "/bin/ls -1 | grep '\\.l\\.pm$' | "
		   "sed 's/\\.l\\.pm//g' | "
		   "while read file ; do "
                     "if [ -f $file.m.pm -a -f $file.t.pm ] ; "
			"then echo $file ; fi ; "
                   "done", dir);

      char *output;
      Invoke *_thread = new Invoke(buf, &output);
      char *s, *s1 = output;
      s = s1;
      filenames->n_icons = 0;
      while (s && (s = strchr(s1, '\n')))
       {
         filenames->n_icons++;
         s1 = s + 1;
       }
      if (filenames->n_icons)
       {
         s = output;
         filenames->icons = new FindSetIcon*[filenames->n_icons];
         for (i = 0; i < filenames->n_icons; i++)
          {
            s1 = s;
            s = strchr(s, '\n');
            *s++ = '\0';
	    if (!(i % 10))
	     {
               sprintf(buf, MESSAGE(LoadingIconsL), i, filenames->n_icons);
               findSet->icons->UpdateMessage(buf);
	     }
	    sprintf(buf, "%s/%s", dir, s1);
            filenames->icons[i] = new FindSetIcon(findSet->icons, s1, buf);
	  }
       }
      delete _thread;
      delete output;
      filenames->read_it = false;
    }
   else
    {
      FindSetIcon **icon = filenames->icons;
      for (i = 0; i < filenames->n_icons; i++, icon++)
         (*icon)->Visible(true);
    }
   findSet->PointerShape(LEFT_SLANTED_ARROW_CURSOR);
   findSet->icons->EndUpdate();
   sprintf(buf, MESSAGE(IconTitleL), filenames->n_icons);
   findSet->icon_label->Name(buf);
   if (filenames->n_icons)
      findSet->empty->Visible(false);
   else
      findSet->empty->Visible(true);
   findSet->last_position = position;

   delete [] buf;
}
