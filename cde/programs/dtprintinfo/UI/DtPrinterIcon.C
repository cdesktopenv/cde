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
/* $TOG: DtPrinterIcon.C /main/5 1998/07/24 16:13:14 mgreess $ */
#include "DtPrinterIcon.h"
#include "Button.h"
#include "DtMainW.h"
#include "Prompt.h"
#include "DtPrtProps.h"
#include "DtSetModList.h"
#include "DtFindD.h"
#include "DtDetailsLabel.h"
#include "Dt/Action.h"

#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h> // This is for the getuid function
#include <stdlib.h> // This is for the getenv function
#include <sys/param.h>
#include <pwd.h>

#include "dtprintinfomsg.h"

const char *PROPS_PRINTER_ID = "PrinterPropsDE";
const char *PRINTER_ID = "PrinterDE";
const char *STATUS_FLAG = "Flag";
const char *PRINTER_ICON_FILE = "Fpprnt";
const char *PRINTERS_PERSONAL_DIR = ".dt/.Printers";

char DtPrinterIcon::homeDir[300] = "";

DtPrinterIcon::DtPrinterIcon(DtMainW *mainW, AnyUI *parent, Queue *que, 
			     PrinterApplicationMode _app_mode)
	: IconObj((char *) que->ObjectClassName(), parent,
		  GetPrinterLabel(que->Name(), _app_mode),
		  GetPrinterIcon(que->Name(), _app_mode))
{
   app_mode = _app_mode;
   queue = que;
   status = NULL;
   dnd = NULL;
#ifdef aix
   _print_device_up = NULL;
#endif

   // Return if initializing printers
   if (app_mode == INITIALIZE_PRINTERS)
      return;

   char *buf = new char[300];
   struct stat statbuff;
   if (*homeDir == '\0')
    {
      struct passwd * pwInfo;
      char *home = getenv("HOME");
      if (home == NULL || strlen(home) == 0)
       {
         pwInfo = getpwuid(getuid());
         home = pwInfo->pw_dir;
       }
      strcpy(homeDir, home);
      sprintf(buf, "%s/%s", homeDir, PRINTERS_PERSONAL_DIR);
      if (stat(buf, &statbuff) < 0)
       {
         sprintf(buf, "mkdir -p %s/%s", homeDir, PRINTERS_PERSONAL_DIR);
         system(buf);
       }
      sprintf(buf, "%s/.dt/types", homeDir);
      if (stat(buf, &statbuff) < 0)
       {
         sprintf(buf, "mkdir -p %s/.dt/types", homeDir);
         system(buf);
       }
      DtDbReloadNotify(&DtPrinterIcon::ReloadNotifyCB, this);
    }
   sprintf(buf, "%s/%s/%s_Print", homeDir, PRINTERS_PERSONAL_DIR,
	   queue->Name());
   if (stat(buf, &statbuff) < 0)
    {
      int fd = creat(buf, 0755);
      close(fd);
    }
   else if (statbuff.st_mode != 0755)
      chmod(buf, 0755);
   mainw = mainW;
   props = NULL;
   container = NULL;
   waitForChildren = false;
   if (app_mode == PRINT_MANAGER)
      expand = new Button(this, queue->DisplayName(), PUSH_BUTTON, OpenCloseCB,
		          NULL, NULL, NULL, NULL, "Dtplus");
   else
      expand = NULL;

   if (app_mode != CONFIG_PRINTERS)
    {
#ifdef aix
      n_devices = queue->NumberDevices();
      _print_device_up = new boolean[n_devices];
      int i;
      for (i = 0; i < n_devices; i++)
         _print_device_up[i] = true;
#else
      queue->ReadAttributes();
      _print_device_up = true;
#endif
      _print_queue_up = true;
      flag = new IconObj((char *)STATUS_FLAG, this, NULL, "DtFlag");
      flag->Visible(false);
      details_label = new DtDetailsLabel(this);
      _previous_show_only_my_jobs = mainw->setPrefD->ShowOnlyMyJobs() ?
				    false : true;
      ShowDetailsLabel(IconView(), false);
    }
   else
    {
      flag = NULL;
      details_label = NULL;
    }
   dnd = new DtDND(this, DndCB);
   mainw->RegisterPopup(this);
   ApplicationData = NULL;
   if (app_mode == SINGLE_PRINTER)
    {
      mainw->IconName((char *)Name());
      char *iconFile = GetPrinterIcon(que->Name(), _app_mode);
      char *s = new char [strlen(iconFile) + 6];
      if (depth == 1)
         sprintf(s, "%s.l.bm", iconFile);
      else
         sprintf(s, "%s.l.pm", iconFile);
      mainw->IconFile(s);
      if (!mainw->IconFile())
       {
	 *(s + strlen(iconFile) + 1) = 'm';
         mainw->IconFile(s);
       }
      delete [] s;
    }
   delete [] buf;
}

DtPrinterIcon::~DtPrinterIcon()
{
   delete dnd;
#ifdef aix
   delete _print_device_up;
#endif
   delete status;
}

char *DtPrinterIcon::Description()
{
   static char buf[200];

   sprintf(buf, "%s_Print", queue->Name());
   char *desc = DtActionDescription(buf);
   return (desc ? desc : (char *)"");
}

void DtPrinterIcon::PrintQueueUp(boolean _flag)
{
   if (app_mode == CONFIG_PRINTERS)
      return;
   if (props)
      status[0]->Value(_flag ? MESSAGE(UpL) : MESSAGE(DownL));
   _print_queue_up = _flag;
   ShowFlag();
}

#ifdef aix

void DtPrinterIcon::PrintDeviceUp(boolean _flag, int index)
{
   if (app_mode == CONFIG_PRINTERS)
      return;
   if (index >= 0 && index < n_devices)
    {
      if (props)
         status[index + 1]->Value(_flag ? MESSAGE(UpL) : MESSAGE(DownL));
      _print_device_up[index] = _flag;
      ShowFlag();
    }
}

boolean DtPrinterIcon::PrintDeviceUp(int index)
{
   if (app_mode != CONFIG_PRINTERS)
    {
      if (index >= 0 && index < n_devices)
         return _print_device_up[index];
    }
   return false;
}

#else

void DtPrinterIcon::PrintDeviceUp(boolean _flag)
{
   if (app_mode == CONFIG_PRINTERS)
      return;
   if (props)
      status[1]->Value(_flag ? MESSAGE(UpL) : MESSAGE(DownL));
   _print_device_up = _flag;
   ShowFlag();
}

#endif

boolean DtPrinterIcon::SetName(char *_name)
{
   IconObj::SetName(_name);
   if (dnd)
      dnd->UpdateRects();
   return true;
}

boolean DtPrinterIcon::SetOpen(boolean _flag)
{
   if (app_mode != CONFIG_PRINTERS)
      IconObj::SetOpen(_flag);
   return true;
}

DtPrinterContainer *DtPrinterIcon::CreateContainer()
{
   if (!container)
    {
      container = new DtPrinterContainer((char *)Category(), this,
                                         queue->DisplayName());
      mainw->RegisterPopup(container);
      container->dnd = new DtDND(container, DndCB);
      container->ApplicationData = queue;
      ApplicationData = container;
    }
   return container;
}

char *DtPrinterIcon::CreateActionFile()
{
   static char filename[MAXPATHLEN + 1];
   FILE *fp;
   struct stat statbuff;
   boolean create_file;

   char *buf = new char[MAXPATHLEN + 1];
   char *lang = getenv("LANG");
   if (!(lang && *lang))
      lang = "C";

   if (app_mode == INITIALIZE_PRINTERS || app_mode == CONFIG_PRINTERS)
    {
      snprintf(filename, MAXPATHLEN, "/etc/dt/appconfig/types/%s", lang);
      if (stat(filename, &statbuff) < 0)
       {
         snprintf(buf, MAXPATHLEN, "/bin/mkdir -p %s", filename);
         system(buf);
       }
      snprintf(filename, MAXPATHLEN, "/etc/dt/appconfig/types/%s/%s.dt", lang,
	      queue->Name());
      if (stat(filename, &statbuff) < 0 || statbuff.st_size == 0)
         create_file = true;
      else
         create_file = false;
    }
   else
    {
      snprintf(buf, MAXPATHLEN, "%s/.dt/types/%s.dt", homeDir, queue->Name());
      if (stat(buf, &statbuff) < 0 || statbuff.st_size == 0)
       {
         snprintf(buf, MAXPATHLEN, "/etc/dt/appconfig/types/%s/%s.dt", lang, queue->Name());
         if (stat(buf, &statbuff) >= 0 && statbuff.st_size > 0)
	  {
            snprintf(buf, MAXPATHLEN, "/bin/cp /etc/dt/appconfig/types/%s/%s.dt %s/.dt/types",
		    lang, queue->Name(), homeDir);
	    system(buf);
	    create_file = false;
	  }
	 else
            create_file = true;
       }
      else
         create_file = false;
      snprintf(filename, MAXPATHLEN, "%s/.dt/types/%s.dt", homeDir, queue->Name());
    }
   if (create_file)
    {
      if (fp = fopen(filename, "w"))
       {
         time_t secs;
         time(&secs);
         fprintf(fp, "################################################################\n\n");
         fprintf(fp, "#   Actions and Datatypes for Printer \"%s\"\n\n", queue->Name());
         fprintf(fp, "#   Common Desktop Environment 1.0\n\n");
         fprintf(fp, "#   This file was created by the \"dtprintinfo\" program.\n\n");
         strftime(buf, 300, "%c", localtime(&secs));
         fprintf(fp, "#   Date of creation: %s\n\n", buf);
         fprintf(fp, "################################################################\n\n");
         fprintf(fp, "ACTION %s_Print\n", queue->Name());
         fprintf(fp, "{\n");
         fprintf(fp, "        ARG_TYPE                *\n");
         fprintf(fp, "        LABEL                   %s\n", queue->Name());
         fprintf(fp, "        ICON                    %s\n", PRINTER_ICON_FILE);
         fprintf(fp, "        TYPE                    COMMAND\n");
         fprintf(fp, "        WINDOW_TYPE             NO_STDIO\n");
         fprintf(fp, "        EXEC_STRING             env LPDEST=%s \\\n", queue->Name());
         fprintf(fp, "				/usr/dt/bin/dtaction Print %%(File)Arg_1%%\n");
	 sprintf(buf, MESSAGE(DefaultDescriptionL), queue->Name());
         fprintf(fp, "        DESCRIPTION             %s\n", buf);
         fprintf(fp, "}\n\n");
   
         fprintf(fp, "ACTION %s_Print\n", queue->Name());
         fprintf(fp, "{\n");
         fprintf(fp, "        ARG_COUNT               0\n");
         fprintf(fp, "        TYPE                    COMMAND\n");
         fprintf(fp, "        WINDOW_TYPE             NO_STDIO\n");
         fprintf(fp, "        EXEC_STRING             /usr/dt/bin/dtaction Dtprintinfo %s\n", queue->Name());
         fprintf(fp, "}\n");
         fflush(fp);
         fclose(fp);
         chmod(filename, 0644);
       }
    }
   delete [] buf;
   return filename;
}

void DtPrinterIcon::DndCB(BaseUI *obj, char **value, int * /*len*/,
			  DNDProtocol dndProtocol)
{
   DtPrinterIcon *printer;
   if (obj->UIClass() == ICON)
      printer = (DtPrinterIcon *)obj;
   else
      printer = (DtPrinterIcon *)obj->Parent();
   DtActionArg ap[1];
   char *old_LPDEST = NULL;
   ap[0].argClass = DtACTION_FILE;

   char *buf = new char[100];
   switch (dndProtocol)
   {
   case FILENAME_TRANSFER: // Dropping an Object on a printer
      ap[0].u.file.name = *value;
      if (printer->PrintActionExists())
         sprintf(buf, "%s_Print", printer->queue->Name());
      else
       {
	 if (old_LPDEST = STRDUP(getenv("LPDEST")))
	  {
            sprintf(buf, "LPDEST=%s", printer->queue->Name());
	    putenv(buf);
	  }
         strcpy(buf, "Print");
       }

      DtActionInvoke(((AnyUI *)printer->mainw->Parent())->BaseWidget(), buf, ap,
		     1, NULL, NULL, NULL, True, NULL, NULL);
      if (old_LPDEST)
       {
	 sprintf(buf, "LPDEST=%s", old_LPDEST);
	 putenv(buf);
	 delete old_LPDEST;
       }
      break;
   case CONVERT_DATA: // Dragging a printer to an object
      if (printer->PrintActionExists())
       {
	 struct stat statbuff;
         *value = new char[strlen(homeDir) + strlen(PRINTERS_PERSONAL_DIR) +
			   strlen(printer->queue->Name()) + 10];
         sprintf(*value, "%s/%s/%s_Print", homeDir, PRINTERS_PERSONAL_DIR,
		 printer->queue->Name());
	 if (stat(*value, &statbuff) < 0)
	  {
	    int fd = creat(*value, 0755);
	    close(fd);
	  }
       }
      else
	 *value = NULL;
      break;
   case DROP_ON_ROOT:
      {
      char *x = *value;
      char *y = strchr(x, '\n');
      *y++ = '\0';
      char *filename = strchr(y, '\n');
      *filename++ = '\0';
      char *work_space = strchr(filename, '\n');
      if (work_space)
         *work_space++ = '\0';
      }
      break;
   case TEXT_TRANSFER:
      break;
   case BUFFER_TRANSFER:
      *value = new char[strlen(printer->queue->Name()) + 10];
      sprintf(*value, "%s_Print", printer->queue->Name());
      break;
   case CONVERT_DELETE:
      break;
   case ANIMATE:
      break;
   }
   delete [] buf;
}

boolean DtPrinterIcon::SetIcon(IconStyle style)
{
   IconObj::SetIcon(style);
   if (dnd)
      dnd->UpdateRects();
   ShowDetailsLabel(style, Visible());

   return true;
}

boolean DtPrinterIcon::HandleHelpRequest()
{
   mainw->DisplayHelp((char *)PRINTER_ID);
   return true;
}

void DtPrinterIcon::UpdateExpand()
{
   if (expand)
    {
      if (Open() == true)
         expand->IconFile("Dtminus");
      else
         expand->IconFile("Dtplus");
    }
   ShowDetailsLabel(IconView(), true);
}

void DtPrinterIcon::Update()
{
   char *cmd = new char[200];
   sprintf(cmd, GET_QUEUE_STATUS, queue->Name());
   Invoke *_thread = new Invoke(cmd);
   PrintQueueUp(_thread->status ? false : true);
   delete _thread;
#ifdef aix
   int i;
   for (i = 0; i < n_devices; i++)
    {
      sprintf(cmd, GET_DEVICE_STATUS, queue->Device(i));
      _thread = new Invoke(cmd);
      PrintDeviceUp(_thread->status ? false : true, i);
      delete _thread;
    }
#else
#ifdef sun
   if (queue->IsRemote())
      PrintDeviceUp(_print_queue_up);
   else
    {
      sprintf(cmd, GET_DEVICE_STATUS, queue->Name());
      _thread = new Invoke(cmd);
      PrintDeviceUp(_thread->status ? false : true);
      delete _thread;
    }
#else
   sprintf(cmd, GET_DEVICE_STATUS, queue->Name());
   _thread = new Invoke(cmd);
   PrintDeviceUp(_thread->status ? false : true);
   delete _thread;
#endif
#endif
   delete [] cmd;
}

void DtPrinterIcon::ShowFlag()
{
   if (app_mode == CONFIG_PRINTERS)
      return;

   if (Visible() == false || mainw->setPrefD->ShowStatusFlags() == false)
    {
      flag->Visible(false);
      return;
    }
   // Update problem flag's visiblity
#ifdef aix
   boolean show_it;
   if (_print_queue_up)
    {
      int i;
      show_it = false;
      for (i = 0; i < n_devices; i++)
         if (_print_device_up[i] == false)
	  {
	    show_it = true;
	    break;
	  }
    }
   else
      show_it = true;
   flag->Visible(show_it);
#else
   if (_print_queue_up && _print_device_up)
      flag->Visible(false);
   else
      flag->Visible(true);
#endif
}

void DtPrinterIcon::NotifyVisiblity(BaseUI *obj)
{
   if (app_mode == CONFIG_PRINTERS)
      return;

   if (obj == flag)
    {
      boolean show_it = Visible();
      if (show_it)
       {
         if (mainw->setPrefD->ShowStatusFlags() == false)
            show_it = false;
         else
	  {
#ifdef aix
            if (_print_queue_up)
             {
	       show_it = false;
               int i;
               for (i = 0; i < n_devices; i++)
                  if (_print_device_up[i] == false)
	           {
	             show_it = true;
	             break;
	           }
             }
	    else
	       show_it = true;
#else
            if (_print_queue_up && _print_device_up)
               show_it = false;
            else
               show_it = true;
#endif
	  }
       }
      if (flag->Visible() != show_it)
         flag->Visible(show_it);
    }
   else if (obj == details_label)
    {
      boolean show_it = Visible();
      if (show_it)
       {
         if (container && IconView() == DETAILS && Open())
	  {
            if (container->NumChildren() == 0 ||
                (container->Children()[0])->UIClass() == LABEL)
               show_it = false;
            else if (mainw->setPrefD->ShowDetailsLabel())
	     {
	       show_it = false;
	       int i;
	       BaseUI **children = container->Children();
	       for (i = 0; i < container->NumChildren(); i++)
	          if (children[i]->Visible())
	           {
	             show_it = true;
	             break;
	           }
	     }
	    else
               show_it = mainw->setPrefD->ShowDetailsLabel();
	  }
         else
            show_it = false;
       }
      if (details_label->Visible() != show_it)
         details_label->Visible(show_it);
    }
}

void DtPrinterIcon::ShowDetailsLabel(IconStyle style, boolean is_visible)
{
   if (app_mode == CONFIG_PRINTERS)
      return;

   if (_previous_show_only_my_jobs != mainw->setPrefD->ShowOnlyMyJobs())
    {
      _previous_show_only_my_jobs = mainw->setPrefD->ShowOnlyMyJobs();
      details_label->Update(_previous_show_only_my_jobs);
    }
   if (container && style == DETAILS && is_visible && Open())
    {
     if (container->NumChildren() == 0 ||
          (container->Children()[0])->UIClass() == LABEL)
         details_label->Visible(false);
      else if (mainw->setPrefD->ShowDetailsLabel())
       {
	 boolean show_it = false;
	 int i;
	 BaseUI **children = container->Children();
	 for (i = 0; i < container->NumChildren(); i++)
	    if (children[i]->Visible())
	     {
	       show_it = true;
	       break;
	     }
         details_label->Visible(show_it);
       }
      else
         details_label->Visible(false);
    }
   else
      details_label->Visible(false);
}

boolean DtPrinterIcon::SetVisiblity(boolean _flag)
{
   if (_flag)
    {
      IconObj::SetVisiblity(_flag);
      Update();
      if (container && Open())
         container->Visible(true);
      ShowDetailsLabel(IconView(), _flag);
    }     
   else
    {
      if (container)
       {
         container->Visible(false);
	 if (mainw->findD)
	  {
	    int i;
	    BaseUI **children = container->Children();
	    for (i = 0; i < container->NumChildren(); i++)
	       mainw->findD->DeleteJobFromList(children[i]);
	  }
       }
      IconObj::SetVisiblity(_flag);
    }

   if (dnd)
      dnd->UpdateActivity(_flag);
   return true;
}

void DtPrinterIcon::DisplayProps()
{
   if (!props)
    {
      char *title = new char[200];
      sprintf(title, MESSAGE(PrinterPropsTitleL), mainw->Name());
      queue->ReadAttributes();
      props = new DtPrtProps(mainw, title, this);
      int n_matches = 0;
      props->FindByName(MESSAGE(PrinterStatusL), 0, &n_matches,
			(BaseUI***)&status);
      delete [] title;
    }
   props->Visible(true);
}

void DtPrinterIcon::ReloadNotifyCB(XtPointer data)
{
   DtPrinterIcon *printer = (DtPrinterIcon *)data;
   DtDbLoad();
   int i, n_siblings = printer->NumSiblings();
   BaseUI **siblings = printer->Siblings();

   for (i = 0; i < n_siblings; i++)
    {
      printer = (DtPrinterIcon *)siblings[i];
      char *iconName = GetPrinterLabel(printer->queue->Name(),
				       printer->app_mode);
      char *iconFile = GetPrinterIcon(printer->queue->Name(),
				      printer->app_mode);

      if (strcmp(iconName, printer->Name()))
       {
	 printer->Name(iconName);
         if (printer->PrinterAppMode() == SINGLE_PRINTER)
	    printer->mainw->IconName(iconName);
         if (printer->PrinterAppMode() == PRINT_MANAGER)
	  {
	    if (printer->mainw->findD && printer->mainw->findD->Visible())
	       printer->mainw->findD->UpdatePrinter(printer);
	  }
       }
      if (strcmp(iconFile, printer->IconFile()))
       {
	 printer->IconFile(iconFile);
         if (printer->PrinterAppMode() == SINGLE_PRINTER &&
	     iconFile && *iconFile)
	  {
	    char *s = new char [strlen(iconFile) + 6];
	    if (depth == 1)
	       sprintf(s, "%s.l.bm", iconFile);
	    else
	       sprintf(s, "%s.l.pm", iconFile);
	    printer->mainw->IconFile(s);
	    delete [] s;
	  }
       }
      if (printer->props && printer->props->Visible())
         printer->props->Reset();
    }
   if (printer->mainw->setModList && printer->mainw->setModList->Visible())
      printer->mainw->setModList->Reset();
}

void DtPrinterIcon::OpenCloseCB(void *data) 
{
   BaseUI *obj = ((BaseUI *)data)->Parent();
   if (obj->Open())
      obj->Open(false);
   else
      obj->Open(true);
}

boolean DtPrinterIcon::PrintActionExists()
{
   boolean b;
   char *buf = new char[60]; 
   sprintf(buf, "%s_Print", queue->Name());
   b =  (DtActionExists(buf) ? true : false);
   delete [] buf;
   return b;
}

char *DtPrinterIcon::GetPrinterIcon(const char *printer,
			            PrinterApplicationMode _app_mode)
{
   static char buf[200];

   sprintf(buf, "%s_Print", printer);
   if (_app_mode != INITIALIZE_PRINTERS && DtActionExists(buf))
    {
      char *iconfile = DtActionIcon(buf);
      if (iconfile)
         strcpy(buf, iconfile);
      else
         strcpy(buf, PRINTER_ICON_FILE);
    }
   else
      strcpy(buf, PRINTER_ICON_FILE);
   return buf;
}

char *DtPrinterIcon::GetPrinterLabel(const char *printer,
			             PrinterApplicationMode _app_mode)
{
   static char buf[200];

   sprintf(buf, "%s_Print", printer);
   if (_app_mode != INITIALIZE_PRINTERS && DtActionExists(buf))
    {
      char *label = DtActionLabel(buf);
      if (label)
         strcpy(buf, label);
      else
         strcpy(buf, printer);
    }
   else
      strcpy(buf, printer);
   return buf;
}
