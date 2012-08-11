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
/* $TOG: DtApp.C /main/12 1998/08/25 12:58:57 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

// User Interface headers
#include "DtApp.h"
#include "DtMainW.h"
#include "DtSetModList.h"
#include "DtFindD.h"
#include "DtWorkArea.h"
#include "DtPrtJobIcon.h"
#include "DtPrinterIcon.h"
#include "DtPrtProps.h"
#include "Application.h"
#include "LabelObj.h"
#include "Button.h"
#include "Sep.h"

// Object headers
#include "PrintSubSys.h"
#include "ParseJobs.h"

// Command Execution header
#include "Invoke.h"
#include "Process.h"

// Message header
#include "dtprintinfomsg.h"

#include <stdlib.h> // This is for the getenv function
#include <unistd.h> // This is for the getuid function
#include <signal.h> // This is for the signal function
#include <sys/stat.h>
#include <pwd.h>

#ifdef aix
extern "C" { extern int seteuid(uid_t); }
#endif

// For Dialog Titles
#define TITLE(message, app_name) \
   char title[BUFSIZ]; \
   sprintf(title, message, app_name)

const char *PROPERTY = "Properties";
const char *HELP = "Help";
const char *FIND = "Find";
const char *EXIT = "Exit";
const char *HIDE = "Hide";
const char *RENAME = "Rename";
const char *OPEN = "Open";
const char *CLOSE = "Close";
const char *PRINTERS_DIR = ".dt/.Printers";

DtApp::DtApp(char *progname, int *argc, char **argv) :
      Application(progname, "Dtprintinfo", argc, argv)
{
   save_state = false;
   connect_timeout = 15;
   old_dbsearchpath = NULL;
   uid_t old_uid;
   single_printer = NULL;
   app_mode = SINGLE_PRINTER;
   char *app_name = MESSAGE(ApplicationName2L);
   if (!((lang = getenv("LANG")) && *lang))
      lang = "C";
   home = getenv("HOME");
   if (!home || *home == '\0')
    {
     struct passwd *pwInfo = getpwuid(getuid());
     home = strdup(pwInfo->pw_dir);
    }
   else
     home = strdup(home);
   printer_dir = new char[strlen(home) + strlen(PRINTERS_DIR) + 2];
   sprintf(printer_dir, "%s/%s", home, PRINTERS_DIR);
   int i;
   for (i = 0; i < *argc; i++)
    {
      if (!strcmp(argv[i], "-edit"))
       {
         app_name = MESSAGE(ApplicationName3L);
         app_mode = CONFIG_PRINTERS;
	 break;
       }
      else if (!strcmp(argv[i], "-all"))
       {
         app_name = MESSAGE(ApplicationName1L);
         app_mode = PRINT_MANAGER;
	 break;
       }
    }

   Process *procs = new Process();
   pid_t pid = getppid();
   char *c = procs->Command(pid);

   if (NULL != c && 0 == strncmp(c, "dtexec", 6))
    {
      if (app_mode == CONFIG_PRINTERS)
       {
         pid = procs->Parent(pid);
         c = procs->Command(pid);
         old_uid = procs->UID(pid);
	 if (!strncmp(c, "dtaction", 8))
	  {
            pid = procs->Parent(pid);
            c = procs->Command(pid);
            old_uid = procs->UID(pid);
            if (!strncmp(c, "dtexec", 6))
	     {
               pid = procs->Parent(pid);
               c = procs->Command(pid);
               old_uid = procs->UID(pid);
	     }
	  }
       }
      delete procs;
#ifdef DEAD_WOOD
      // This is a workaround to avoid a bug in dtsession or dtaction.
      // We should be fixing the problem there.
      //
      // Fork so that the dtaction grandparent exits.  If we don't the
      // session manager starts the dtaction process and our dtprintinfo
      // process, then dtaction process starts the dtprintinfo process and
      // we have two dtprintinfo processes running when the user logs in.

      // On Novell, we notice that the child is dying when the parent
      // exits.  Ignore the SIGHUP signal before fork() and life is good.
      signal(SIGHUP, SIG_IGN);

      if (fork() != 0)
         exit(0);
#endif
    }
   else
    {
      delete procs;
      old_uid = getuid();
    }

   if (app_mode == CONFIG_PRINTERS)
    {
      if (getuid() != 0)
       {
	 fprintf(stderr, MESSAGE(RootUserL), progname, "-edit");
	 fprintf(stderr, "\n");
	 exit(1);
       }
      char *lang = getenv("LANG");
      char *DBSearchPath = getenv("DTDATABASESEARCHPATH");
      old_dbsearchpath = STRDUP(DBSearchPath);
      char *buf;
      if (!(lang && *lang))
	 lang = "C";
      if (DBSearchPath && *DBSearchPath)
       {
         buf = new char[strlen(DBSearchPath) + strlen(lang) + 50];
         sprintf(buf, "DTDATABASESEARCHPATH=/etc/dt/appconfig/types/%s,%s",
		 lang, DBSearchPath);
       }
      else
       {
         buf = new char[strlen(DBSearchPath) + strlen(lang) + 50];
         sprintf(buf, "DTDATABASESEARCHPATH=/etc/dt/appconfig/types/%s", lang);
       }
      putenv(buf);
    }

   Name(app_name);

   Frequency = 30000;

   if (app_mode == CONFIG_PRINTERS)
      window = new DtMainW("Printers", this, app_name, SCROLLED_WORK_AREA,
			   SINGLE_SELECT, OpenClose, NULL, PreferenceCB, NULL,
		           MESSAGE(PrinterMenuL), MESSAGE(PrinterAcceleratorL),
			   app_mode);
   else
      window = new DtMainW("Printers", this, app_name, SCROLLED_ICON_LIST,
			   SINGLE_SELECT, OpenClose, NULL, PreferenceCB, NULL,
		           MESSAGE(PrinterMenuL), MESSAGE(PrinterAcceleratorL),
			   app_mode);
   window->Initialize();
   window->ApplicationData = NULL;
      
   if (app_mode == PRINT_MANAGER)
      window->DtAddAction(MESSAGE(FindChoiceL), (char *)window->Category(),
		          (char *)FIND, ActionCB, window,
		          MESSAGE(FindMnemonicL), MESSAGE(FindAcceleratorL));
   window->DtAddAction(MESSAGE(ExitChoiceL), (char *)window->Category(),
		       (char *)EXIT, ActionCB, window, MESSAGE(ExitMnemonicL), 
		       MESSAGE(ExitAcceleratorL));
   window->RegisterPopup(window->container);

   // Save print job's class name for registering GUI actions
   PrintJob *dummy_printjob;
   char *className;
   if (app_mode != CONFIG_PRINTERS)
    {
      dummy_printjob = new PrintJob(NULL, "_FOO", NULL, NULL, NULL, NULL, NULL);
      className = (char *) dummy_printjob->ObjectClassName();

      // Add Print Job Actions
      AddActions(dummy_printjob);
    }

   // Save queue's class name for registering GUI actions
   Queue *dummy_queue = new Queue(NULL, "_DUMMY_");
   char *className1 = (char *) dummy_queue->ObjectClassName();

   // Add GUI actions for queue
   if (app_mode == PRINT_MANAGER)
    {
      window->DtAddAction(MESSAGE(OpenChoiceL), className1, (char *)OPEN,
		          ActionCB, window, MESSAGE(OpenMnemonicL));
      window->DtAddAction(MESSAGE(CloseChoiceL), className1, (char *)CLOSE,
		          ActionCB, window, MESSAGE(CloseMnemonicL));
    }
   // Add Queue Actions
   if (getuid() == 0 && dummy_queue->NumActions())
    {
      window->DtAddSep(className1);
      AddActions(dummy_queue);
    }
   else
      delete dummy_queue;

   // Add GUI actions for queue
   if (app_mode != CONFIG_PRINTERS)
    {
      if (app_mode == PRINT_MANAGER)
       {
         window->DtAddSep(className1);
         window->DtAddAction(MESSAGE(HideChoiceL), className1, (char *)HIDE,
		             ActionCB, window, MESSAGE(HideMnemonicL));
       }
      //window->DtAddAction(MESSAGE(RenameChoiceL), className1, (char *)RENAME,
		            //ActionCB, window, MESSAGE(RenameMnemonicL));

      // Add common GUI actions for print jobs and queues
      window->DtAddSep(className);
      if (app_mode == PRINT_MANAGER)
         window->DtAddSep(className1);
      window->DtAddAction(MESSAGE(PropertiesChoiceL), className,
			  (char *)PROPERTY, ActionCB, window,
			  MESSAGE(PropertiesMnemonicL),
		          MESSAGE(PropertiesAcceleratorL),
		          "Ctrl<Key>osfBackSpace");
    }
   window->DtAddAction(MESSAGE(PropertiesChoiceL), className1, (char *)PROPERTY,
		       ActionCB, window, MESSAGE(PropertiesMnemonicL),
		       MESSAGE(PropertiesAcceleratorL),
		       "Ctrl<Key>osfBackSpace");
   if (app_mode == CONFIG_PRINTERS)
    {
      window->DtAddSep(className1);
      window->DtAddAction(MESSAGE(HelpChoiceL), className1, (char *)HELP,
		          ActionCB, window, MESSAGE(HelpMnemonicL), "F1");
    }
   else
    {
      window->DtAddSep(className);
      window->DtAddSep(className1);
      window->DtAddAction(MESSAGE(HelpChoiceL), className, (char *)HELP,
		          ActionCB, window, MESSAGE(HelpMnemonicL), "F1");
      window->DtAddAction(MESSAGE(HelpChoiceL), className1, (char *)HELP,
		          ActionCB, window, MESSAGE(HelpMnemonicL), "F1");
    }

   if (app_mode == PRINT_MANAGER)
    {
      Button *find = new Button(window->fileMenu, MESSAGE(FindChoiceL),
			        PUSH_BUTTON, FindCB, window,
			        MESSAGE(FindMnemonicL),
			        MESSAGE(FindAcceleratorL),
				MESSAGE(FindAcceleratorKeySeq)); 
      find->Order(0);
      Button *filter = new Button(window->viewMenu, MESSAGE(ModifyShowChoiceL),
			          PUSH_BUTTON, ModifyCB, window,
			          MESSAGE(ModifyShowMnemonicL));
    }
   // Add a time out to add queues, this way the UI is visible within secs
   ApplicationData = window;
   AddTimeOut(AddQueues, this, 0);
}

DtApp::~DtApp()
{
   free(home);
   delete [] printer_dir;
}

void DtApp::SaveYourSelf()
{
   if (app_mode == CONFIG_PRINTERS)
      return;

   save_state = true;

   char *value = window->setPrefD->ShowOnlyMyJobs() ? (char *)"1" : (char *)"0";
   Save("ShowOnlyMine", value);

   value = window->setPrefD->ShowStatusLine() ? (char *)"1" : (char *)"0";
   Save("ShowMessageLine", value);

   value = window->setPrefD->ShowDetailsLabel() ? (char *)"1" : (char *)"0";
   Save("ShowDetailsLabel", value);

   value = window->setPrefD->ShowStatusFlags() ? (char *)"1" : (char *)"0";
   Save("ShowProblemFlag", value);

   char interval[9];
   sprintf(interval, "%d", window->setPrefD->UpdateInterval());
   Save("UpdateInterval", interval);

   switch (window->container->IconView())
   {
   case DETAILS: value = "Details"; break;
   case NAME_ONLY: value = "NameOnly"; break;
   case SMALL_ICON: value = "SmallIcon"; break;
   default: value = "LargeIcon"; break;
   }
   Save("Representation", value);

   if (app_mode == PRINT_MANAGER)
    {
      int i;
      DtPrinterIcon **queues;
      char *attribute = (char*) malloc(100);
      queues = (DtPrinterIcon **) window->container->Children();
      for (i = 0; i < window->container->NumChildren(); i++)
       {
	 value = queues[i]->Visible() ? (char *)"1" : (char *)"0";
	 sprintf(attribute, "%s.Visible", queues[i]->QueueObj()->Name());
	 Save(attribute, value);
	 value = queues[i]->Open() ? (char *)"1" : (char *)"0";
	 sprintf(attribute, "%s.Open", queues[i]->QueueObj()->Name());
	 Save(attribute, value);
       }
      free(attribute);
    }
}

char *DtApp::SessionFile()
{
   if (app_mode == SINGLE_PRINTER)
    {
      if (single_printer)
         return (char *)single_printer->QueueObj()->Name();
      else
	 return "default";
    }
   else if (app_mode == PRINT_MANAGER)
      return "Printmgr";
   else
      return "Printcfg";
}

void DtApp::UpdateStatusLine()
{
   char *message;
   char *msg2;

   message = (char*) malloc(200);
   msg2 = (char*) malloc(100);

   if (app_mode == CONFIG_PRINTERS)
    {
      sprintf(message, MESSAGE(EditLangL), lang);
      window->status_line->Name(message);
    }
   else if (app_mode == SINGLE_PRINTER)
    {
      if (single_printer)
       {
         char *details = MESSAGE(UpL);

         if (single_printer->PrintQueueUp() == false)
            details = MESSAGE(DownL);
	 else
#ifdef aix
	  {
            Queue *queue = single_printer->QueueObj();
            int i;
            for (i = 0; i < queue->NumberDevices(); i++)
               if (single_printer->PrintDeviceUp(i) == false)
	        {
                  details = MESSAGE(DownL);
	          break;
	        }
	  }
#else
         if (single_printer->PrintDeviceUp() == false)
            details = MESSAGE(DownL);
#endif
         Container *rc = single_printer->JobContainer();
         if (window->setPrefD->ShowOnlyMyJobs() && rc)
          {
	    if (rc->FindByName(MESSAGE(EmptyL)))
               window->status_line->Name(details);
	    else
	     {
               sprintf(msg2, MESSAGE(JobsL), rc->NumChildren());
               sprintf(message, "%s  %s", details, msg2);
               window->status_line->Name(message);
	     }
          }
         else
            window->status_line->Name(details);
       }
      else
         window->status_line->Name(MESSAGE(NoDefaultPrinter1L));
    }
   else
    {
      int i, n_visible = 0;
      BaseUI ** children = window->container->Children();
      for (i = 0; i < window->container->NumChildren(); i++)
         if (children[i]->Visible() == false)
	    n_visible++;
      sprintf(msg2, MESSAGE(PrintersHiddenL), 
	      window->container->NumChildren(), n_visible);
      if (window->setPrefD->ShowOnlyMyJobs())
       {
         sprintf(message, "%s  %s", msg2, MESSAGE(ShowingMyJobsL));
         window->status_line->Name(message);
       }
      else
         window->status_line->Name(msg2);
    }

   free(message);
   free(msg2);
}

void DtApp::InitQueueDetails(BaseUI *obj, void *data)
{
   ((DtPrinterIcon *)obj)->Update();
   DtApp *app = (DtApp *)data;
   app = (DtApp *)app->Parent();

   if (app->save_state)
      return;

   int n_siblings = obj->NumSiblings();
   int i = obj->Order() + 1;
   if (i < n_siblings)
    {
      BaseUI **siblings = obj->Siblings();
      siblings[i]->AddTimeOut(InitQueueDetails, data, 3000);
    }
   if (app->app_mode == SINGLE_PRINTER)
      app->UpdateStatusLine();
}

void DtApp::RemoteStatusCB(BaseUI *obj, char *output, int)
{
   DtPrinterIcon *printer_icon = (DtPrinterIcon *)obj->Parent();
   Queue *queue = printer_icon->QueueObj();
   queue->ParseRemoteStatus(output);
   printer_icon->PrintDeviceUp(queue->RemoteUp());
   obj->EndUpdate();
   printer_icon->Open(true);
}

void DtApp::TurnOffHourGlass(BaseUI *obj, void *)
{
   DtMainW *window = (DtMainW *) obj;
   window->WorkingCursor(false);
}

void DtApp::ShowStatusDialog(DtPrinterIcon *printer_icon)
{
   Queue *queue = printer_icon->QueueObj();
   Attribute **attributes = queue->Attributes();
   char *message;
   int len = 0;

   if (printer_icon->PrintQueueUp() == false)
      len = strlen(MESSAGE(PrinterDownL)) + strlen(queue->Name());
#ifdef aix
   int i;
   for (i = 0; i < queue->NumberDevices(); i++)
      if (printer_icon->PrintDeviceUp(i) == false)
         len += strlen(MESSAGE(PrinterDownL)) +
		strlen(attributes[1 + i]->DisplayValue) + 2;
#else
   if (printer_icon->PrintDeviceUp() == false)
      len += strlen(MESSAGE(DeviceDownL)) + strlen(attributes[1]->DisplayValue);
#endif
   message = new char[len + 2];
   *message = '\0';
   if (printer_icon->PrintQueueUp() == false)
    {
      sprintf(message, MESSAGE(PrinterDownL), queue->Name());
      strcat(message, "\n");
    }
#ifdef aix
   for (i = 0; i < queue->NumberDevices(); i++)
      if (printer_icon->PrintDeviceUp(i) == false)
       {
         len = strlen(message);
         sprintf(message + len, MESSAGE(DeviceDownL), 
		 attributes[1 + i]->DisplayValue);
	 strcat(message, "\n");
       }
#else
   len = strlen(message);
   if (printer_icon->PrintDeviceUp() == false)
      sprintf(message + len, MESSAGE(DeviceDownL), attributes[1]->DisplayValue);
#endif
   len = strlen(message) - 1;
   if (*(message + len) == '\n')
      *(message + len) = '\0';
   TITLE(MESSAGE(PrinterStatusTitleL), window->Name());
   Dialog *dialog = new Dialog(window, title, message, INFORMATION,
				     MESSAGE(OKL));
   dialog->Visible(true);
   delete [] message;
}

void DtApp::OpenClose(void *data, BaseUI *obj)
{
   // Return if object isn't a queue, since we can't show a print job's contents
   if (obj->Parent()->UISubClass() == SCROLLED_HORIZONTAL_ROW_COLUMN)
      return;

   DtMainW *window = (DtMainW *) data;
   ((DtApp *)window->Parent())->OpenClose(obj);
}

void DtApp::OpenClose(BaseUI *obj)
{
   if (save_state)
      return;

   DtPrinterIcon *printer_icon;
   Container *rc;
   Queue *queue;

   if (app_mode == CONFIG_PRINTERS)
    {
      printer_icon = (DtPrinterIcon *)obj;
      queue = printer_icon->QueueObj();
      char *s = new char[strlen(queue->Name()) + STRLEN(old_dbsearchpath) + 90];
      sprintf(s, "env DTDATABASESEARCHPATH=%s /usr/dt/bin/dtaction %s_Print &",
	      (old_dbsearchpath ? "" : old_dbsearchpath), queue->Name());
      window->WorkingCursor(true);
      window->AddTimeOut(TurnOffHourGlass, NULL, 5000);
      Invoke *_thread = new Invoke(s, NULL, NULL, old_uid);
      delete _thread;
      delete [] s;
      return;
    }

   if (!STRCMP(obj->Category(), STATUS_FLAG))
    {
      printer_icon = (DtPrinterIcon *)obj->Parent();
      ShowStatusDialog(printer_icon);
      return;
    }

   printer_icon = (DtPrinterIcon *)obj;
   queue = printer_icon->QueueObj();
   rc = printer_icon->JobContainer();
   if (obj->Open() == true)
    {
      char *message = new char[100];
      boolean prev_open = false;

      sprintf(message, MESSAGE(UpdatingL), obj->Name());
      if (!rc)
       {
         rc = printer_icon->CreateContainer();
         rc->AddTimeOut(UpdatePrintJobs, window, Frequency);
       }
      else
	 prev_open = rc->Visible();
      if (printer_icon->Visible())
         rc->Visible(true);
      if (queue->IsRemote() && printer_icon->waitForChildren == false)
       {
         if (printer_icon->updating == false)
          {
	    int sockfd = ConnectToPrintServer(queue->Server(), connect_timeout);
	    if (sockfd != -1)
	     {
	       SendPrintJobStatusReguest(sockfd, queue->RemotePrinter());
	       printer_icon->updating = true;
	       if (prev_open == false)
		{
	          rc->BeginUpdate();
	          rc->UpdateMessage(message);
                  rc->Refresh();
		}
               printer_icon->UpdateExpand();
	       rc->Thread(sockfd, RemoteStatusCB, 512);
	       return;
	     }
	    else
	     {
               queue->ParseRemoteStatus("");
	       printer_icon->PrintDeviceUp(false);
	     }
          }
	 printer_icon->updating = false;
       }
      DtWorkArea *container = (DtWorkArea *)obj->Parent();
      window->WorkingCursor(true);
      window->status_line->Name(message);
      container->Refresh();
      if (queue->IsRemote() == false || printer_icon->waitForChildren)
         queue->UpdateChildren();
      PrintJob ** jobs = (PrintJob **)queue->Children();
      int n_jobs = queue->NumChildren();
      DtPrtJobIcon ** icons = (DtPrtJobIcon **)rc->Children();
      int n_icons = rc->NumChildren();

      BaseUI *empty_label = rc->FindByName(MESSAGE(EmptyL));
      if (n_jobs == 0)
       {
	 if (!empty_label)
	  {
            empty_label = new LabelObj(rc, MESSAGE(EmptyL));
            icons = (DtPrtJobIcon **)rc->Children();
	  }
	 else
	    n_icons--;
       }
      else if (empty_label)
       {
	 n_icons--;
         delete empty_label;
         icons = (DtPrtJobIcon **)rc->Children();
       }

      int i;
      // Change children first, then delete or create icons
      if (n_icons)
       {
	 int j;
         for (i = 0; i < n_icons; i++)
	  {
	    boolean found_it = false;
            for (j = 0; j < n_jobs; j++)
	       if (!strcmp(icons[i]->JobNumber(),
	                   jobs[j]->AttributeValue((char *)JOB_NUMBER)))
		{
		  found_it = true;
		  break;
		}
	    if (found_it == false)
	     {
	       delete icons[i];
               icons = (DtPrtJobIcon **)rc->Children();
               n_icons--;
	       i = -1;
	     }
	  }
         for (i = 0; i < n_icons; i++)
	  {
	    boolean found_it = false;
            for (j = 0; j < n_jobs; j++)
	       if (!strcmp(icons[i]->JobNumber(),
	                   jobs[j]->AttributeValue((char *)JOB_NUMBER)))
		{
		  found_it = true;
		  break;
		}
	    if (found_it)
               icons[i]->PrintJobObj(jobs[j]);
	    else
	     {
               DtPrtJobIcon *tmp = new DtPrtJobIcon(window, rc, jobs[j], i + 1);
	       tmp->Order(i);
               icons = (DtPrtJobIcon **)rc->Children();
               n_icons++;
	     }
	  }

       }
      else
	 i = 0;
      if (n_jobs < n_icons) // Delete excess icons
         for (; i < n_icons; i++)
	    delete rc->Children()[n_jobs];
      else // Create new icons
         for (; i < n_jobs; i++)
            new DtPrtJobIcon(window, rc, jobs[i], i + 1);
      icons = (DtPrtJobIcon **)rc->Children();
      n_icons = rc->NumChildren();
      for (i = 0; i < n_jobs; i++)
       {
         char number[5];
	 sprintf(number, "%d", i + 1);
	 icons[i]->TopString(number);
       }
      if (window->setPrefD->ShowOnlyMyJobs())
       {
         if (getuid() == 0) // check to see if we are root
            ShowUserJobs(obj, "root", true);
	 else
            ShowUserJobs(obj, getenv("LOGNAME"), true);
       }
      window->WorkingCursor(false);
      UpdateStatusLine();
      if (app_mode == PRINT_MANAGER)
	{
         if (window->findD && window->findD->Visible())
            window->findD->UpdatePositions(printer_icon);
	}

      delete [] message;
    }
   else if (app_mode != SINGLE_PRINTER)
    {
      if (rc)
         rc->DeleteChildren();
      queue->DeleteChildren();
      if (rc)
         rc->Visible(false);
    }
   printer_icon->UpdateExpand();
}

void DtApp::ActionCB(void *data, BaseUI *obj, char *actionReferenceName)
{
   DtMainW *window = (DtMainW *) data;
   ((DtApp *)window->Parent())->ActionCB(obj, actionReferenceName);
}

void DtApp::ActionCB(BaseUI *obj, char *actionReferenceName)
{
   if (!strcmp(actionReferenceName, EXIT))
    {
      exit(0);
    }
   else if (!strcmp(actionReferenceName, FIND))
    {
      FindCB(window);
    }
   else if (!strcmp(actionReferenceName, HELP))
    {
      if (obj->Parent()->UISubClass() == SCROLLED_ICON_LIST ||
          obj->UIClass() == CONTAINER)
         window->DisplayHelp((char *)PRINTER_ID);
      else
         window->DisplayHelp((char *)PRINTJOB_ID);
    }
   else if (!strcmp(actionReferenceName, PROPERTY))
    {
      if (app_mode == CONFIG_PRINTERS ||
	  obj->Parent()->UISubClass() == SCROLLED_ICON_LIST)
       {
	 ((DtPrinterIcon *)obj)->DisplayProps();
       }
      else if (obj->UIClass() == CONTAINER)
	 ((DtPrinterIcon *)obj->Parent())->DisplayProps();
      else
	 ((DtPrtJobIcon *)obj)->DisplayProps();
    }
   else if (!strcmp(actionReferenceName, OPEN))
    {
       if (obj->Parent()->UISubClass() == SCROLLED_ICON_LIST)
        {
          if (obj->Open() == false)
             obj->Open(true);
        }
       else
        {
          if (obj->Parent()->Open() == false)
             obj->Parent()->Open(true);
        }
    }
   else if (!strcmp(actionReferenceName, CLOSE))
    {
      if (obj->Parent()->UISubClass() == SCROLLED_ICON_LIST)
         obj->Open(false);
      else
         obj->Parent()->Open(false);
    }
   else if (!strcmp(actionReferenceName, RENAME))
    {
      Dialog *info = new Dialog(window, "Print Manager - Rename", 
	 "NOT IMPLEMENTED", INFORMATION);
      info->Visible(true);
    }
   else if (!strcmp(actionReferenceName, HIDE))
    {
      if (obj->Parent()->UISubClass() == SCROLLED_ICON_LIST)
         obj->Visible(false);
      else
         obj->Parent()->Visible(false);
      if (window->setModList && window->setModList->Visible())
         window->setModList->Reset();
      UpdateStatusLine();
    }
   else
    {
      BaseObj *object;
      if (obj->Parent()->UISubClass() == SCROLLED_ICON_LIST)
         object = ((DtPrinterIcon *)obj)->QueueObj();
      else
         object = (BaseObj *)obj->ApplicationData;
      int save_id = obj->UniqueID();
      if (!strcmp(actionReferenceName, CANCEL_PRINT_JOB))
       {
	 char *title = new char[100];
	 sprintf(title, MESSAGE(CancelTitleL), window->Name(), obj->Name());
	 char *message = new char[100];
	 sprintf(message, MESSAGE(CancelQuestionL), obj->Name());
	 Dialog *confirm = new Dialog(window, title, message, QUESTION, 
				      MESSAGE(YesL), MESSAGE(NoL));
	 confirm->Visible(true);
	 boolean answer = confirm->Answer();
	 delete confirm;
	 delete title;
	 delete [] message;
	 if (answer == false)
	    return;
       }
      if (window->container->ObjectExists(save_id) == false)
       {
         Dialog *dialog = new Dialog(window, (char *)window->Name(),
				     MESSAGE(NotFoundMessageL), INFORMATION,
				     MESSAGE(OKL));
         dialog->Visible(true);
         return;
       }
      // get object again, just in case a update occurred
      object = (BaseObj *)obj->ApplicationData;
      (void) object->SendAction(actionReferenceName);
      if (!strcmp(actionReferenceName, CANCEL_PRINT_JOB))
       {
         PrintJob *me = (PrintJob *) object;

         //  If there was a problem getting the job # from the prt server,
         //  (if so, _jobNumber will be set to "not Available") put up an 
         //  error dialog

         if (!(strcmp(me->JobNumber(), MESSAGE(NotAvailableL))))
         {
	    Action *action;
   	    (void) object->HasAction(actionReferenceName, &action);
	    char *title = new char [strlen(MESSAGE(FailedActionTitleL)) +
                                    strlen(window->Name()) +
                                    strlen(action->DisplayName) + 1];

	    sprintf(title, MESSAGE(FailedActionTitleL), window->Name(),
		    action->DisplayName);

	    char *message = new char [strlen(MESSAGE(FailedMessageL))  +
				      strlen(action->DisplayName)      + 
				      strlen(object->DisplayName())    + 
				      strlen(MESSAGE(NoRemoteCancelL)) + 1];

	    sprintf(message, MESSAGE(FailedMessageL),
		             action->DisplayName, object->DisplayName(), 
		             MESSAGE(NoRemoteCancelL));

	    Dialog *dialog = new Dialog(window, title, message, ERROR,
				     MESSAGE(OKL));
	    dialog->Visible(true);
	    delete [] title;
	    delete [] message;

            return;
         }

	 BaseUI *parent = obj->Parent();
	 // If command was successful, remove icon
	 if (object->LastActionStatus() == 0)
	  {
	    delete obj;
            if (parent->NumChildren() == 0)
               new LabelObj((AnyUI *)parent, MESSAGE(EmptyL));
	    else
	     {
	       int i;
	       IconObj **children = (IconObj **)parent->Children();
               for (i = 0; i < parent->NumChildren(); i++)
                {
                  char number[5];
	          sprintf(number, "%d", i + 1);
		  children[i]->TopString(number);
		}
	     }
            if (app_mode == PRINT_MANAGER)
	      {
               if (window->findD && window->findD->Visible())
                  window->findD->UpdatePositions(parent->Parent());
	      }
	  }
       }
      else // This is a queue action, so update status flag
       {
         ((DtPrinterIcon *)obj)->Update();
         if (app_mode == SINGLE_PRINTER)
            UpdateStatusLine();
       }

      if (object->LastActionStatus() != 0)
       {
	 Action *action;
	 (void) object->HasAction(actionReferenceName, &action);
	 char *title = new char [strlen(MESSAGE(FailedActionTitleL)) +
                                 strlen(window->Name()) +
                                 strlen(action->DisplayName) + 1];
	 sprintf(title, MESSAGE(FailedActionTitleL), window->Name(),
		 action->DisplayName);
	 char *message = new char [strlen(object->LastActionOutput()) + 
				   strlen(action->DisplayName) + 
				   strlen(object->DisplayName()) + 70];
	 sprintf(message, MESSAGE(FailedMessageL),
		 action->DisplayName, object->DisplayName(), 
		 object->LastActionOutput());

	 Dialog *dialog = new Dialog(window, title, message, ERROR,
				     MESSAGE(OKL));
	 dialog->Visible(true);
	 delete []title;
	 delete []message;
       }
    }
}

void DtApp::UpdatePrintJobs(BaseUI *obj, // the print list box
			    void *data)
{
   DtMainW *window = (DtMainW *) data;
   if (((DtApp *)window->Parent())->save_state)
      return;

   int Frequency = (int)((DtApp *)window->Parent())->Frequency;
   // if we are doing a Find operation or have been filtered, return
   if (window->in_find || obj->Parent()->Visible() == false)
    {
      obj->AddTimeOut(UpdatePrintJobs, data, Frequency);
      return;
    }

   DtPrinterIcon *icon = (DtPrinterIcon *)obj->Parent();
   Application *app = (Application *) window->Parent();
   // Check app's visibility to see if we are iconified or in a different
   // workspace, also check the icon's visibility to see if it's visible
   // in the scrolled window.
   if (app->IsVisible() == false || icon->IsVisible() == false)
    {
      obj->AddTimeOut(UpdatePrintJobs, data, Frequency);
      return;
    }

   // Queue Icon (parent) is visible, so update it
   icon->Update();

   if (obj->Visible() == false)
    {
      // The jobs list box is not visible, so add a timeout and return
      obj->AddTimeOut(UpdatePrintJobs, window, Frequency);
      return;
    }

   if (obj->Parent()->Open() == true)
    {
      if (icon->updating == false)
         OpenClose(window, obj->Parent());
    }
   obj->AddTimeOut(UpdatePrintJobs, window, Frequency);
}

char *DtApp::GetBottomString(BaseObj *job, boolean need_details)
{
   static char string[200];
   if (window->setPrefD->ShowOnlyMyJobs())
    {
      if (need_details)
         sprintf(string, "%s\n%s\n%s\n%s",
	         job->AttributeValue((char *)JOB_SIZE),
	         job->AttributeValue((char *)JOB_NUMBER),
	         job->AttributeValue((char *)TIME_SUBMITTED),
	         job->AttributeValue((char *)DATE_SUBMITTED));
      else
         return NULL;
    }
   else
    {
      if (need_details)
         sprintf(string, "%s\n%s\n%s\n%s\n%s",
	         job->AttributeValue((char *)OWNER),
	         job->AttributeValue((char *)JOB_SIZE),
	         job->AttributeValue((char *)JOB_NUMBER),
	         job->AttributeValue((char *)TIME_SUBMITTED),
	         job->AttributeValue((char *)DATE_SUBMITTED));
      else
         strcpy(string, job->AttributeValue((char *)OWNER));
    }
   return string;
}

void DtApp::HandleShowDetailsLabelPreferenceRequest(IconStyle style)
{
   int i;
   DtPrinterIcon **queues = (DtPrinterIcon **)window->container->Children();
   for (i = 0; i < window->container->NumChildren(); i++)
      queues[i]->ShowDetailsLabel(style, queues[i]->Visible());
}

void DtApp::HandleDetailsPreferenceRequest(boolean details_on)
{
   int i;
   BaseUI **queues = window->container->Children();
   for (i = 0; i < window->container->NumChildren(); i++)
    {
      BaseUI *jobs_container = (BaseUI *)queues[i]->ApplicationData;
      if (!jobs_container)
	 continue;

      IconObj **jobs = (IconObj **)jobs_container->Children();
      int j;
      for (j = 0; j < jobs_container->NumChildren(); j++)
         if (jobs[j]->UIClass() == ICON)
	  {
	    BaseObj *job = (BaseObj *)jobs[j]->ApplicationData;
            char *bottomString = GetBottomString(job, details_on);
	    jobs[j]->BottomString(bottomString);
          }
    }
}

boolean DtApp::ShowUserJob(DtPrtJobIcon *_job, char *user_name)
{
   BaseObj *job = _job->PrintJobObj();
   char *owner = job->AttributeValue((char *)OWNER);
   char *s = strchr(owner, '@');
   boolean show_it;
   if (s && s != owner)
      show_it = !strncmp(user_name, owner, (int)(s - owner)) ? true : false;
   else
      show_it = !strcmp(user_name, owner) ? true : false;
   return show_it;
}

void DtApp::ShowUserJobs(BaseUI *queue, char *user_name, boolean flag)
{
   BaseUI *jobs_container = (BaseUI *)queue->ApplicationData;
   if (!jobs_container)
      return;

   DtPrtJobIcon **jobs = (DtPrtJobIcon **)jobs_container->Children();
   int j;
   jobs_container->BeginUpdate();
   for (j = 0; j < jobs_container->NumChildren(); j++)
      if (jobs[j]->UIClass() == ICON)
       {
         if (flag == true)
            jobs[j]->Visible(ShowUserJob(jobs[j], user_name));
         else
            jobs[j]->Visible(true);
       }
   ((DtPrinterIcon *)queue)->ShowDetailsLabel(queue->IconView(),
					      queue->Visible());
   jobs_container->EndUpdate();
}

void DtApp::HandleShowOnlyMinePreferenceRequest(boolean flag)
{
   int i;
   char *user_name;
   BaseUI **queues = window->container->Children();
   if (getuid() == 0) // check to see if we are root
      user_name = "root";
   else
      user_name = getenv("LOGNAME");
   boolean details_on = window->container->IconView() == DETAILS ? true : false;
   for (i = 0; i < window->container->NumChildren(); i++)
      ShowUserJobs(queues[i], user_name, flag);
   HandleDetailsPreferenceRequest(details_on);
}

void DtApp::PreferenceCB(void *data, PreferenceRequest req, char *value)
{
   DtMainW *window = (DtMainW *)data;
   ((DtApp *)window->Parent())->PreferenceCB(req, value);
}

void DtApp::PreferenceCB(PreferenceRequest req, char *value)
{
   switch (req)
   {
   case DETAILS_ON:
      HandleDetailsPreferenceRequest(true);
      break;
   case DETAILS_OFF:
      HandleDetailsPreferenceRequest(false);
      break;
   case STATUS_LINE_ON:
      window->container->AttachBottom(window->status_line);
      break;
   case STATUS_LINE_OFF:
      window->container->AttachBottom();
      break;
   case STATUS_FLAG_ON:
   case STATUS_FLAG_OFF:
      {
         int i;
         DtPrinterIcon **queues;
         queues = (DtPrinterIcon **) window->container->Children();
         for (i = 0; i < window->container->NumChildren(); i++)
            queues[i]->Update();
      }
      break;
   case SHOW_ONLY_MINE_ON:
   case SHOW_ONLY_MINE_OFF:
      HandleShowOnlyMinePreferenceRequest(window->setPrefD->ShowOnlyMyJobs());
      UpdateStatusLine();
      break;
   case SHOW_DETAILS_LABEL_ON:
   case SHOW_DETAILS_LABEL_OFF:
      HandleShowDetailsLabelPreferenceRequest(DETAILS);
      break;
   case UPDATE_INTERVAL_CHANGED:
      Frequency = (long) value * 1000;
      break;
   }
}

void DtApp::FilterCB(void *data)
{
   static DtMainW *window = NULL;

   if (!window)
    {
      window = (DtMainW *)data;
      while (window->UIClass() != MAIN_WINDOW)
         window = (DtMainW *)window->Parent();
    }
   ((DtApp *)window->Parent())->FilterCB((BaseUI *)data);
}

void DtApp::FilterCB(BaseUI *container)
{
   BaseUI **queue_list = container->Children();

   int i;
   BaseUI **queues = window->container->Children();
   window->WorkingCursor(true);
   window->container->BeginUpdate();
   window->Refresh();
   for (i = 0; i < container->NumChildren(); i++)
      queues[i]->Visible(queue_list[i]->Selected());
   UpdateStatusLine();
   window->container->EndUpdate();
   window->WorkingCursor(false);
}

void DtApp::ModifyCB(void *data)
{
   DtMainW *window = (DtMainW *) data;
   if (!window->setModList)
    {
      TITLE(MESSAGE(ModifyShowTitleL), window->Name());
      window->setModList = new DtSetModList(window, title, 
					    window->container, FilterCB);
    }
   window->setModList->Visible(true);
}

boolean DtApp::SelectPrintJobs(BaseUI *obj)
{
   if (obj->Parent()->UIClass() == CONTAINER &&
       obj->Parent()->UISubClass() == SCROLLED_HORIZONTAL_ROW_COLUMN)
    {
      if (obj->UIClass() == LABEL)
         return false;
      else
       {
	 static DtMainW *window = NULL;
	 if (!window)
	  {
	    window = (DtMainW *)obj;
            while (window->UIClass() != MAIN_WINDOW)
               window = (DtMainW *)window->Parent();
	  }
	 DtApp *app = (DtApp *)window->Parent();
         if (getuid() == 0) // check to see if we are root
	  {
            if (window->setPrefD->ShowOnlyMyJobs())
               return app->ShowUserJob((DtPrtJobIcon *)obj, "root");
	    else if (window->findD->MatchAnyUser())
               return true;
	    else
               return app->ShowUserJob((DtPrtJobIcon *)obj, "root");
	  }
	 else
            return app->ShowUserJob((DtPrtJobIcon *)obj, getenv("LOGNAME"));
       }
    }
   else
      return false;
}

void DtApp::FindCB(void *data)
{
   DtMainW *window = (DtMainW *) data;
   if (!window->findD)
    {
      TITLE(MESSAGE(FindTitleL), window->Name());
      window->findD = new DtFindD(window, title, SelectPrintJobs);
    }
   window->findD->Visible(true);
}

void DtApp::RestoreApp()
{
   char *value = Restore("Representation");
   if (!STRCMP(value, "Details"))
      window->setPrefD->Representation(DETAILS);
   else if (!STRCMP(value, "NameOnly"))
      window->setPrefD->Representation(NAME_ONLY);
   else if (!STRCMP(value, "SmallIcon"))
      window->setPrefD->Representation(SMALL_ICON);
   else
      window->setPrefD->Representation(LARGE_ICON);

   value = Restore("ShowProblemFlag");
   boolean flag = !STRCMP(value, "0") ? false : true;
   window->setPrefD->ShowStatusFlags(flag);

   value = Restore("ShowMessageLine");
   flag = !STRCMP(value, "0") ? false : true;
   window->setPrefD->ShowStatusLine(flag);

   value = Restore("ShowOnlyMine");
   flag = !STRCMP(value, "1") ? true : false;
   window->setPrefD->ShowOnlyMyJobs(flag);

   value = Restore("ShowDetailsLabel");
   flag = !STRCMP(value, "0") ? false : true;
   window->setPrefD->ShowDetailsLabel(flag);
   window->setPrefD->Apply();

   int interval = 30;
   if (value = Restore("UpdateInterval"))
    {
      interval = atoi(value);
      if (interval < 5 || interval > 300)
         interval = 30;
    }
   window->setPrefD->UpdateInterval(interval);

   window->setPrefD->Apply();

   if (app_mode == PRINT_MANAGER)
    {
      int i;
      DtPrinterIcon **queues;
      char *attribute = new char[100];
      queues = (DtPrinterIcon **) window->container->Children();
      for (i = 0; i < window->container->NumChildren(); i++)
       {
	 sprintf(attribute, "%s.Visible", queues[i]->QueueObj()->Name());
         value = Restore(attribute);
	 if (!STRCMP(value, "0"))
	    queues[i]->Visible(false);
	 else
	  {
	    sprintf(attribute, "%s.Open", queues[i]->QueueObj()->Name());
            value = Restore(attribute);
	    if (!STRCMP(value, "1"))
	       queues[i]->Open(true);
	  }
       }
      UpdateStatusLine();
      delete attribute;
    }
   else
    {
      single_printer->Open(true);
    }
}

void DtApp::UpdateQueues()
{
   window->PointerShape(HOUR_GLASS_CURSOR);
   window->container->BeginUpdate();
   window->status_line->Name(MESSAGE(SearchingL));
   window->Refresh();

   PrintSubSystem *prt;
   if (window->ApplicationData)
    {
      prt = (PrintSubSystem *) window->ApplicationData;
      prt->DeleteChildren();
      window->container->DeleteChildren();
    }
   else
    {
      // Connect to Print SubSystem
      prt = new PrintSubSystem(NULL);
      window->ApplicationData = prt;
    }
   int n_queues = prt->NumChildren();
   if (n_queues == 0)
    {
      window->container->EndUpdate();
      window->PointerShape(LEFT_SLANTED_ARROW_CURSOR);
      UpdateStatusLine();
      Dialog *dialog = new Dialog(window, (char *)Name(),
				  MESSAGE(FoundNoPrintersL),
				  INFORMATION, MESSAGE(OKL));
      dialog->Visible(true);
      return;
    }

   // Get Print Subsystem children, (these are queues)
   Queue **queues = (Queue **)prt->Children();

   BaseUI *lpdest = NULL;
   char *LPDEST = getenv("LPDEST");
   if (app_mode == SINGLE_PRINTER)
    {
      int found = -1, j;
      // First check printer on command line
      if (argc >= 3)
       {
         for (j = 0; j < n_queues; j++)
            if (!STRCMP(argv[2], queues[j]->Name()))
	     {
	       found = j;
	       break;
	     }
       }
      // Next check LPDEST printer
      if (found == -1 && LPDEST && *LPDEST)
       {
         for (j = 0; j < n_queues; j++)
            if (!STRCMP(LPDEST, queues[j]->Name()))
	     {
	       found = j;
	       break;
	     }
       }
      // If a printer is still not found, use system default
      if (found == -1)
       {
#ifdef aix
	 char *cmd = "lsallq | grep -v ^bsh\\$ | head -1";
#else
#ifdef __osf__
	 /* resolve possible alias using lpstat -v<name> */
	 char *cmd = "env LANG=C lpstat -v"
	     "`env LANG=C lpstat -d | awk '$1 != \"no\" {print $NF}'`"
	     "| awk '{ print $10 }'";
#else
	 char *cmd = "env LANG=C lpstat -d | awk '$1 != \"no\" {print $NF}'";
#endif
#endif
	 char *output;
	 Invoke *_thread = new Invoke(cmd, &output);
	 strtok(output, "\n");
         for (j = 0; j < n_queues; j++)
            if (!STRCMP(output, queues[j]->Name()))
	     {
	       found = j;
	       break;
	     }
	 delete _thread;
	 delete output;
       }
      if (found == -1)
       {
         window->container->EndUpdate();
         window->PointerShape(LEFT_SLANTED_ARROW_CURSOR);
         UpdateStatusLine();
	 char *tmp = new char [strlen(MESSAGE(NoDefaultPrinter1L)) +
			       strlen(MESSAGE(NoDefaultPrinter2L)) + 5];
	 sprintf(tmp, "%s\n\n%s", MESSAGE(NoDefaultPrinter1L),
                 MESSAGE(NoDefaultPrinter2L));
         Dialog *dialog = new Dialog(window, (char *)Name(), tmp, INFORMATION,
				     MESSAGE(OKL));
         dialog->Visible(true);
         delete prt;
         delete [] tmp;
         window->ApplicationData = NULL;
         return;
       }
      single_printer = new DtPrinterIcon(window, window->container,
                                         queues[found], app_mode);
      single_printer->updating = false;
    }
   else
    {
      int i;
      for (i = 0; i < n_queues; i++)
       {
         // Create Icon
         DtPrinterIcon *icon = new DtPrinterIcon(window, window->container,
				                 queues[i], app_mode);
         icon->updating = false;
       }
    }
   boolean call_reload_actions = false;
   DtPrinterIcon **printers = (DtPrinterIcon **)window->container->Children();
   int i;
   for (i = 0; i < window->container->NumChildren(); i++)
    {
      if (printers[i]->PrintActionExists() == false)
       {
         printers[i]->CreateActionFile();
	 call_reload_actions = true;
       }
    }
   if (call_reload_actions)
    {
      window->status_line->Name(MESSAGE(UpdatingActionsL));
      window->Refresh();
      system("/usr/dt/bin/dtaction ReloadActions&");
    }
   UpdateStatusLine();
   window->container->EndUpdate();
   window->PointerShape(LEFT_SLANTED_ARROW_CURSOR);
   if (app_mode != CONFIG_PRINTERS)
    {
      AddTimeOut(RestoreAppCB, this, 0);
      printers[0]->AddTimeOut(InitQueueDetails, window, 3000);
    }
}

void DtApp::RestoreAppCB(BaseUI *obj, void *)
{
   ((DtApp *)obj)->RestoreApp();
}

void DtApp::AddQueues(BaseUI *obj, void *)
{
   ((DtApp *)obj)->UpdateQueues();
}

void DtApp::AddActions(BaseObj *dummy)
{
   int i;

   Action **actions = dummy->Actions();
   int n_actions = dummy->NumActions();
   for (i = 0; i < n_actions; i++)
      window->DtAddAction(actions[i]->DisplayName,
			  (char *)dummy->ObjectClassName(),
		          actions[i]->ReferenceName, ActionCB, window,
		          actions[i]->Nmemonic, actions[i]->AcceleratorText,
		          actions[i]->Accelerator);
   delete dummy;
}
