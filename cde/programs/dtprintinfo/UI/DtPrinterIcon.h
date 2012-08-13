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
/* $XConsortium: DtPrinterIcon.h /main/3 1995/11/06 09:36:06 rswiston $ */
#ifndef DTPRINTERICON_H
#define DTPRINTERICON_H

#include "IconObj.h"
#include "Queue.h"
#include "Container.h"
#include "DtDND.h"

extern const char *PROPS_PRINTER_ID;
extern const char *PRINTER_ID;
extern const char *STATUS_FLAG;
extern const char *PRINTER_ICON_FILE;

typedef enum
{
   SINGLE_PRINTER,
   PRINT_MANAGER,
   CONFIG_PRINTERS,
   INITIALIZE_PRINTERS
} PrinterApplicationMode;

class DtMainW;
class DtPrtProps;
class IconObj;
class Container;
class Button;
class DtDetailsLabel;
class Prompt;

class DtPrinterContainer : public Container
{
 public:
   DtDND *dnd;

   DtPrinterContainer(char *category, AnyUI *parent, char *name)
      : Container(category, parent, name, SCROLLED_HORIZONTAL_ROW_COLUMN) { }
   ~DtPrinterContainer() { delete dnd; }

   boolean SetVisiblity(boolean flag)
    {
      Container::SetVisiblity(flag);
      dnd->UpdateActivity(flag);
      return true;
    }
};

class DtPrinterIcon : public IconObj 
{
   friend void DndCB(BaseUI *, char **, int *, DNDProtocol);
   friend void OpenCloseCB(void *);
   friend void AddPrintActionCB(BaseUI *, void *);
   friend void ReloadNotifyCB(void *);
   friend char * GetPrinterIcon(const char *, PrinterApplicationMode);
   friend char * GetPrinterLabel(const char *, PrinterApplicationMode);

   static void DndCB(BaseUI *, char **, int *, DNDProtocol);
   static void OpenCloseCB(void *);
   static void AddPrintActionCB(BaseUI *, void *);
   static void ReloadNotifyCB(void *);
   static char * GetPrinterIcon(const char *, PrinterApplicationMode);
   static char * GetPrinterLabel(const char *, PrinterApplicationMode);

 private:

   static char homeDir[];

   DtMainW *mainw;
   DtDND *dnd;
   Queue *queue;
   Button *expand;
   IconObj *flag;
   DtDetailsLabel *details_label;
   DtPrinterContainer *container;
   DtPrtProps *props;
   boolean _previous_show_only_my_jobs;
   PrinterApplicationMode app_mode;
   boolean _print_queue_up;
   Prompt **status;
#ifdef aix
   boolean *_print_device_up;
   int n_devices;
#else
   boolean _print_device_up;
#endif

   void ShowFlag();
   boolean SetVisiblity(boolean);
   void NotifyVisiblity(BaseUI *);
   boolean HandleHelpRequest();
   boolean SetIcon(IconStyle);
   boolean SetOpen(boolean);
   boolean SetName(char *name);

 public:

   boolean updating;
   boolean waitForChildren;

   DtPrinterIcon(DtMainW *, AnyUI *parent, Queue *queue,
		 PrinterApplicationMode app_mode = SINGLE_PRINTER);
   ~DtPrinterIcon();
   void DisplayProps();
   void Update();
   void UpdateExpand();
   void PrintQueueUp(boolean);
   boolean PrintQueueUp() { return _print_queue_up; }
#ifdef aix
   void PrintDeviceUp(boolean, int index = 0);
   boolean PrintDeviceUp(int index = 0);
#else
   void PrintDeviceUp(boolean);
   boolean PrintDeviceUp() { return _print_device_up; }
#endif
   void ShowDetailsLabel(IconStyle, boolean is_visible);
   char *CreateActionFile();
   char *Description();
   boolean PrintActionExists();
   DtPrinterContainer *JobContainer() { return container; }
   DtPrinterContainer *CreateContainer();
   PrinterApplicationMode PrinterAppMode() { return app_mode; }
   const char *HomeDir() { return homeDir; }
   Queue *QueueObj() { return queue; }

};

#endif // DTPRINTERICON_H 
