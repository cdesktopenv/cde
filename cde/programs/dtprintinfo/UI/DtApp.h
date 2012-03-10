/* $XConsortium: DtApp.h /main/3 1995/11/06 09:34:32 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTAPP_H
#define DTAPP_H

#include "Application.h"
#include "DtPrinterIcon.h"
#include "DtSetPref.h"

class DtMainW;
class BaseObj;
class DtPrtJobIcon;

extern const char *PROPERTY;
extern const char *HELP;
extern const char *FIND;
extern const char *EXIT;
extern const char *HIDE;
extern const char *RENAME;
extern const char *OPEN;
extern const char *CLOSE;
extern const char *PRINTERS_DIR;

class DtApp : public Application
{

   friend void InitQueueDetails(BaseUI *obj, void *data);
   friend void RemoteStatusCB(BaseUI *obj, char *output, int rc);
   friend void TurnOffHourGlass(BaseUI *obj, void *data);
   friend void OpenClose(void *data, BaseUI *obj);
   friend void ActionCB(void *data, BaseUI *obj, char *actionReferenceName);
   friend void UpdatePrintJobs(BaseUI *obj, void *data);
   friend void PreferenceCB(void *data, PreferenceRequest req, char *value);
   friend void FilterCB(void *data);
   friend void ModifyCB(void *data);
   friend boolean SelectPrintJobs(BaseUI *obj);
   friend void FindCB(void *data);
   friend void AddQueues(BaseUI *obj, void *data);
   friend void RestoreAppCB(BaseUI *obj, void *data);

 private:

   static void InitQueueDetails(BaseUI *obj, void *data);
   static void RemoteStatusCB(BaseUI *obj, char *output, int rc);
   static void TurnOffHourGlass(BaseUI *obj, void *data);
   static void OpenClose(void *data, BaseUI *obj);
   static void ActionCB(void *data, BaseUI *obj, char *actionReferenceName);
   static void UpdatePrintJobs(BaseUI *obj, void *data);
   static void PreferenceCB(void *data, PreferenceRequest req, char *value);
   static void FilterCB(void *data);
   static void ModifyCB(void *data);
   static boolean SelectPrintJobs(BaseUI *obj);
   static void FindCB(void *data);
   static void AddQueues(BaseUI *obj, void *data);
   static void RestoreAppCB(BaseUI *obj, void *data);

   void OpenClose(BaseUI *obj);
   void PreferenceCB(PreferenceRequest req, char *value);
   void FilterCB(BaseUI *container);
   void UpdateStatusLine();
   void UpdateQueues();
   void RestoreApp();
   void AddActions(BaseObj *dummy);
   void ShowStatusDialog(DtPrinterIcon *);
   void HandleShowDetailsLabelPreferenceRequest(IconStyle style);
   void HandleDetailsPreferenceRequest(boolean details_on);
   boolean ShowUserJob(DtPrtJobIcon *_job, char *user_name);
   void ShowUserJobs(BaseUI *queue, char *user_name, boolean flag);
   void HandleShowOnlyMinePreferenceRequest(boolean flag);

   int connect_timeout; // connect timeout to contact server
   long Frequency;
   char *old_dbsearchpath;
   boolean save_state;

 public:

   int old_uid;
   PrinterApplicationMode app_mode;
   DtPrinterIcon *single_printer;
   DtMainW *window;
   char *printer_dir;
   char *lang;
   char *home;

   DtApp(char *progname, int *argc, char **argv);
   ~DtApp();
   void SaveYourSelf();
   char *SessionFile();
   char *SessionPath() { return printer_dir; }
   char *GetBottomString(BaseObj *job, boolean need_details);
   void ActionCB(BaseUI *obj, char *actionReferenceName);

};



#endif // DTAPP_H
