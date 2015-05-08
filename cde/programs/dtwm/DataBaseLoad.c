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
/* $TOG: DataBaseLoad.c /main/9 1998/01/12 16:46:23 cshi $ */
/*****************************************************************************
 *
 *   File:         DataBaseLoad.c
 *
 *   Project:	   CDE
 *
 *   Description:  This file contains the functions which load and parse
 *                 the front panel databases.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 *
 *
 ****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xresource.h>	   /* XrmQuark */
#include <Dt/DtP.h>                /* required for DtDirPaths type */
#include <Dt/DbReader.h>           /* required for DtDbPathId type */
#include <Dt/Connect.h>            /* required for DtDbPathId type */
#include <Dt/Dts.h>
#include <Dt/Utility.h>
#include <Dt/IconFile.h>
#include <Dt/Icon.h>
#include <Dt/UserMsg.h>

#include "WmParse.h"
#include "WmPanelP.h"
#include "DataBaseLoad.h"
#include "Parse.h"
#include "UI.h"

#include <pwd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>


/************************************************************************
 *
 *  panel is the global top of the panel tree structure.
 *
 *  switch is a local that is held until the hierary is built.
 *
 ************************************************************************/
 
PanelData panel;
static SwitchData * switch1;
static ElementValue * control_element_values = NULL;
static Boolean control_element_value_found = False;



#define ANY_CONTAINER_TYPE	99

static char * TYPES_DIR = "/.dt/types/fp_dynamic/";
static char * TYPES_DIR_NO_SLASH = "/.dt/types/fp_dynamic";
static char * HOME_DIR = NULL;
static char * RM = "/bin/rm";


typedef Boolean (*FieldParse)(char *, void **);
typedef void (*FieldFree)(void **);

typedef struct
{
   FieldParse   parse_function;
   void       * default_value;
   FieldFree	free_function;
} ParseFunction;




/************************************************************************
 *
 *  Definitions and structure initializations for PANEL record type.
 *
 ************************************************************************/

static char * panel_keywords[] =
{
   "PANEL",
   "PANEL_GEOMETRY",
   "DISPLAY_HANDLES",
   "DISPLAY_MENU",
   "DISPLAY_MINIMIZE",
   "RESOLUTION",
   "CONTROL_BEHAVIOR",
   "DISPLAY_CONTROL_LABELS",
   "SUBPANEL_UNPOST",
   "LOCKED",
   "HELP_STRING",
   "HELP_VOLUME",
   "HELP_TOPIC",
};

static ParseFunction panel_parse_functions[] =
{
   { StringToString,   NULL, 		   FreeString},   /* Name             */
   { StringToString, NULL,               FreeString},   /* Panel Geometry   */
   { StringToBoolean,  (void *) True,      NULL},         /* Display Handles  */
   { StringToBoolean,  (void *) True,      NULL},         /* Display Menu     */
   { StringToBoolean,  (void *) True,      NULL},         /* Display Minimize */
   { StringToResolution, (void *) 3,  	   NULL},  	  /* Resolution       */
   { StringToControlBehavior, (void *) 0,  NULL},         /* Control Behavior */
   { StringToBoolean,  (void *) False,     NULL},   /* Display Control Labels */
   { StringToBoolean,  (void *) True,      NULL},         /* Subpanel Unpost  */
   { StringToBoolean,  (void *) False,     NULL},         /* Locked           */
   { StringToString,   NULL,               FreeString},   /* Help String      */
   { StringToString,   NULL,               FreeString},   /* Help Volume      */
   { StringToString,   NULL,               FreeString},   /* Help Topic       */
};


/************************************************************************
 *
 *  Definitions and structure initializations for BOX record type.
 *
 ************************************************************************/

static char * box_keywords[] =
{
   "BOX",
   "CONTAINER_NAME",
   "POSITION_HINTS",
   "LOCKED",
   "DELETE",
   "HELP_STRING",
   "HELP_VOLUME",
   "HELP_TOPIC",
};

static ParseFunction box_parse_functions[] =
{
   {StringToString,	    NULL,              FreeString}, /* Name           */
   {StringToString,	    NULL,              FreeString}, /* Container Name */
   {StringToPositionHints, (void *) 0,         NULL},       /* Position Hints */
   {StringToBoolean,       (void *) False,     NULL},       /* Locked         */
   {StringToBoolean,       (void *) False,     NULL},       /* Delete         */
   {StringToString,	    NULL,              FreeString}, /* Help String    */
   {StringToString,	    NULL,              FreeString}, /* Help Volume    */
   {StringToString,	    NULL,              FreeString}, /* Help Topic     */
};


/************************************************************************
 *
 *  Definitions and structure initializations for SUBPANEL record type.
 *
 ************************************************************************/

static char * subpanel_keywords[] =
{
   "SUBPANEL",
   "CONTAINER_NAME",
   "CONTROL_INSTALL",
   "TITLE",
   "LOCKED",
   "DELETE",
   "HELP_STRING",
   "HELP_VOLUME",
   "HELP_TOPIC",
};

static ParseFunction subpanel_parse_functions[] =
{
   {  StringToString,	NULL,           FreeString},	/*  Name             */
   {  StringToString,	NULL,           FreeString},	/*  Container Name   */
   {  StringToBoolean,	(void *) True,  NULL},	        /*  Control Install  */
   {  StringToString,	NULL,           FreeString},	/*  Title            */
   {  StringToBoolean,	(void *) False, NULL},	        /*  Locked           */
   {  StringToBoolean,  (void *) False, NULL},          /*  Delete           */
   {  StringToString,   NULL,           FreeString},	/*  Help String      */
   {  StringToString,   NULL,           FreeString},	/*  Help Volume      */
   {  StringToString,   NULL,           FreeString},	/*  Help Topic       */
};


/************************************************************************
 *
 *  Definitions and structure initializations for SWITCH record type.
 *
 ************************************************************************/

static char * switch_keywords[] = 
{
   "SWITCH",
   "CONTAINER_NAME",
   "POSITION_HINTS",
   "NUMBER_OF_ROWS",
   "LOCKED",
   "DELETE",
   "HELP_STRING",
   "HELP_VOLUME",
   "HELP_TOPIC",
};


static ParseFunction switch_parse_functions[] =
{
   { StringToString,	    NULL,            FreeString}, /* Name            */
   { StringToString,	    NULL,            FreeString}, /* Container Name  */
   { StringToPositionHints, (void *) 0,      NULL},       /* Position Hints  */
   { StringToInt,           (void *) 2,      NULL},       /* Number of Rows  */
   { StringToBoolean,	    (void *) False,  NULL},       /* Locked          */
   { StringToBoolean,       (void *) False,  NULL},       /* Delete          */
   { StringToString,	    NULL,            FreeString}, /* Help String     */
   { StringToString,	    NULL,            FreeString}, /* Help Volume     */
   { StringToString,	    NULL,            FreeString}, /* Help Topic      */
};


/************************************************************************
 *
 *  Definitions and structure initializations for CONTROL record type.
 *
 ************************************************************************/

static char * control_keywords[] =
{
   "CONTROL",
   "TYPE",
   "CONTAINER_TYPE",
   "CONTAINER_NAME",
   "POSITION_HINTS",
   "ICON",
   "ALTERNATE_ICON",
   "LABEL",
   "PUSH_ACTION",
   "PUSH_ANIMATION",
   "DROP_ACTION",
   "DROP_ANIMATION",
   "PUSH_RECALL",
   "MONITOR_TYPE",
   "CLIENT_NAME",
   "CLIENT_GEOMETRY",
   "FILE_NAME",
   "DATE_FORMAT",
   "LOCKED",
   "DELETE",
   "HELP_STRING",
   "HELP_VOLUME",
   "HELP_TOPIC",
};

static ParseFunction control_parse_functions[] =
{
   {StringToString,	          NULL,   FreeString},   /* Name            */
   {StringToControlType,  (void *)CONTROL_ICON, NULL},   /* Type            */
   {StringToControlContainerType, NULL,   NULL},         /* Container Type  */
   {StringToString,	          NULL,   FreeString},   /* Container Name  */
   {StringToPositionHints,(void *) 0,     NULL},         /* Position Hints  */
   {StringToString,               NULL,   FreeString},   /* Image           */
   {StringToString,	          NULL,   FreeString},   /* Altername Image */
   {StringToString,               NULL,   FreeString},   /* Label           */
   {StringToAction,               NULL,   FreeAction},   /* Push Action     */
   {StringToString,               NULL,   FreeString},   /* Push Animation  */
   {StringToAction,               NULL,   FreeAction},   /* Drop Action     */
   {StringToString,               NULL,   FreeString},   /* Drop Animation  */
   {StringToBoolean,      (void *) False, NULL},         /* Push Recall     */
   {StringToMonitorType,  (void *) MONITOR_NONE,NULL},   /* Monitor Type    */
   {StringToString,               NULL,   FreeString},   /* Client Name     */
   {StringToGeometry,		  NULL,   FreeGeometry}, /* Client Geometry */
   {StringToFileName,             NULL,   FreeString},   /* File Name       */
   {StringToString,               NULL,   FreeString},   /* Date Format     */
   {StringToBoolean,	  (void *) False, NULL},         /* Locked          */
   {StringToBoolean,      (void *) False, NULL},         /* Delete          */
   {StringToString,               NULL,   FreeString},   /* Help String     */
   {StringToString,               NULL,   FreeString},   /* Help Volume     */
   {StringToString,               NULL,   FreeString},   /* Help Topic      */
};



/************************************************************************
 *
 *  Structure definition and initialization for the front panel
 *  keyword lists.
 *
 ************************************************************************/

char * entry_types[] =
{
   "PANEL",
   "BOX",
   "SUBPANEL",
   "SWITCH",
   "CONTROL"
};

char * control_types[] =
{
   "blank",
   "busy",
   "icon",
   "client",
   "clock",
   "date",
   "file",
};


char * resolution_types[] =
{
   "high",
   "medium",
   "low",
   "match_display"
};

char * monitor_types[] =
{
   "none",
   "mail",
   "file",
};

#define ANIMATION "ANIMATION"

typedef struct
{
   char           * record_keyword;
   int              keyword_count;
   char          ** field_keywords;
   ParseFunction  * parse_functions;
} RecordKeywords;


static RecordKeywords record_keywords[] =
{
   {   "PANEL",           PANEL_KEYWORD_COUNT,
       panel_keywords,    panel_parse_functions      },

   {   "BOX",             BOX_KEYWORD_COUNT,
       box_keywords,      box_parse_functions        },

   {   "SUBPANEL",        SUBPANEL_KEYWORD_COUNT,
       subpanel_keywords, subpanel_parse_functions   },

   {   "SWITCH",          SWITCH_KEYWORD_COUNT,
       switch_keywords,   switch_parse_functions     },

   {   "CONTROL",         CONTROL_KEYWORD_COUNT,  
       control_keywords,  control_parse_functions    },
};


/************************************************************************
 *
 *  Static and external function definitions
 *
 ************************************************************************/
 

extern void WmFrontPanelSetBusy (Boolean);
extern char * _DtDbGetDataBaseEnv (void);

static Boolean PanelParseCB (DtDtsDbField *, DtDbPathId, char *, Boolean);
static Boolean BoxParseCB (DtDtsDbField *, DtDbPathId, char *, Boolean);
static Boolean SubpanelParseCB (DtDtsDbField *, DtDbPathId, char *, Boolean);
static Boolean SwitchParseCB (DtDtsDbField *, DtDbPathId, char *, Boolean);
static Boolean ControlParseCB (DtDtsDbField *, DtDbPathId, char *, Boolean);
static Boolean AnimationParseCB (DtDtsDbField *, DtDbPathId, char *, Boolean);
static Boolean ControlSingleParseCB(DtDtsDbField *, DtDbPathId, char *, Boolean);
static void ProcessRecord (DtDtsDbField *, ElementValue *);
static void AllocateRecordData (RecordData **, int, int *, int *);
static void ReorderByContainerType(RecordData *, int , int);
static void ReorderByContainerName(RecordData *, int , int, int);
static void ReorderByName(RecordData *, int , int, int);
static void ReorderByPosition(RecordData *, int , int, int, int);
static void OrderRecord (RecordData *, int, int);
static char ** GetNameList (RecordData *, int, int, int, int);
static void EliminateUnused (RecordData *, int *, char **, int, int, int, int);
static void EliminateDeleted (RecordData *, int *, int, int, int, int, int);
static void ResolveDuplicates (RecordData *, int *, int, int, int, int, int);
static void InitializeField (ElementValue *, int, ParseFunction *);
static void ProcessBox (BoxData *);
static int ProcessBoxControl (ControlData *);
static void ProcessSubpanel (SubpanelData *);
static void ProcessSwitch (SwitchData *);
static void ProcessControl (XtPointer, char, ControlData ***, int *, ElementValue *);
static void InitializePrimaryControlFields (ElementValue *);
static void InitializeSecondaryControlFields (ElementValue *);
static void InitializeFileControlFields (ElementValue *, char *);



/************************************************************************
 *
 *  Variable and record declarations and initializations to keep track
 *  of the data base info as it is read and to be used as storage for
 *  subsequent parsing and reprocessing.
 *
 ************************************************************************/

static int panel_count = 0;
static int panel_data_count = 0;
static RecordData * panel_data = NULL;

static int box_count = 0;
static int box_data_count = 0;
static RecordData * box_data = NULL;

static int subpanel_count = 0;
static int subpanel_data_count = 0;
static RecordData * subpanel_data = NULL;

static int switch_count = 0;
static int switch_data_count = 0;
static RecordData * switch_data = NULL;

static int control_count = 0;
static int control_data_count = 0;
static RecordData * control_data = NULL;



static DtDbConverter panel_converter[] =    {  (DtDbConverter) PanelParseCB,
                                               NULL  };
static DtDbConverter box_converter[] =      {  (DtDbConverter) BoxParseCB,
                                               NULL  };
static DtDbConverter subpanel_converter[] = {  (DtDbConverter) SubpanelParseCB,
                                               NULL  };
static DtDbConverter switch_converter[] =   {  (DtDbConverter) SwitchParseCB,
                                               NULL  };
static DtDbConverter control_converter[] =  {  (DtDbConverter) ControlParseCB,
                                               NULL  };
static DtDbConverter animation_converter[] ={  (DtDbConverter) AnimationParseCB,
                                               NULL  };

static DtDbRecordDesc record_descriptor[] =
{
   {  "PANEL",     PANEL_KEYWORD_COUNT,	   panel_converter     },
   {  "BOX",       BOX_KEYWORD_COUNT,  	   box_converter       },
   {  "SUBPANEL",  SUBPANEL_KEYWORD_COUNT, subpanel_converter  },
   {  "SWITCH",    SWITCH_KEYWORD_COUNT,   switch_converter    },
   {  "CONTROL",   CONTROL_KEYWORD_COUNT,  control_converter   },
   {  "ANIMATION", DTUNLIMITEDFIELDS,      animation_converter }
};

static DtDbConverter control_single_converter[] =
                               { (DtDbConverter) ControlSingleParseCB, NULL};

static DtDbRecordDesc control_record_descriptor[] =
{
   {  "CONTROL",   CONTROL_KEYWORD_COUNT, control_single_converter  },
   {  "ANIMATION", DTUNLIMITEDFIELDS,     animation_converter       }
};





/************************************************************************
 *
 *  FrontPanelReadDatabases
 *     Read in the front panel database, parse out each line, build 
 *     the internal data structures that are then used to create the 
 *     visual representation.
 *
 *     Processing occurs in a series of steps:
 *
 *	 1. Read in all of the database records and copy the unparsed
 *	    string information into a structure.
 *            a. Group the records according to type.
 *            b. Group the records according to container.
 *            c. Ensure the original read order is maintained to process
 *               overrides.
 *
 *          This step occurs out of the callback for each of the component
 *          types.
 *
 *       2. Eliminate the unused component definitions by working top-down
 *          throught the lists.  Select the PANEL and SWITCH definition
 *          to be used at this time.
 *          
 *       3. Eliminate overridden components by either selecting the first
 *          locked definition or if none are locked, selecting the last
 *          definition.
 *
 *       4. Reorder the component groups according to the position hints
 *
 *       5. Build hierarchical data structure from record sets, top-down.
 *          Fully parse each record as the structure is built.
 *
 *
 *       6. Return.
 *
 ************************************************************************/
 
Boolean
FrontPanelReadDatabases (void)


{
   char * search_paths;
   char * new_search_paths;
   struct passwd * pw_info;

   char * dynamic_dir;
   struct stat stat_info;
   int fd;   

   int    i;
   char * fp_database;
   int    num_entries;
   char ** name_list;
   DtDirPaths * dir_paths;


   panel.animation_data = NULL;
   panel.animation_count = 0;



   /*  Before reading the databases, add the front panel dynamic  */
   /*  directory to the search path.                              */
   
   search_paths = _DtDbGetDataBaseEnv();

   if ((HOME_DIR = (char *) getenv ("HOME")) == NULL)
   {
      pw_info = getpwuid (getuid());
      HOME_DIR = pw_info->pw_dir;
   }

   HOME_DIR = XtNewString (HOME_DIR);
   
   new_search_paths = 
      XtMalloc (strlen ("DTDATABASESEARCHPATH") + strlen (HOME_DIR) + 
                strlen (TYPES_DIR_NO_SLASH) + strlen (search_paths) + 3);
   sprintf (new_search_paths, "%s=%s%s,%s", 
            "DTDATABASESEARCHPATH", HOME_DIR, TYPES_DIR_NO_SLASH, search_paths);
   putenv (new_search_paths);

   XtFree (search_paths);


   /*  See if the fp_dynamic directory exists.  If not, create it  */

   dynamic_dir =  XtMalloc (strlen (HOME_DIR) + strlen (TYPES_DIR) + 3);
   sprintf (dynamic_dir, "%s%s", HOME_DIR, TYPES_DIR);

   if (lstat (dynamic_dir, &stat_info) != 0)
      if ((fd=mkdir (dynamic_dir, S_IRWXU | S_IRGRP | S_IROTH | 
                                  S_IXGRP | S_IXOTH)) != 0)
      {
         
      }

   XtFree ((char *) dynamic_dir);


   /*  Read the database and do the initial set of processing on  */
   /*  the file based information.  This work occurs within the   */
   /*  callback function referenced in the data read call.        */

   dir_paths = _DtGetDatabaseDirPaths();

#ifdef DT_PERFORMANCE
_DtPerfChkpntMsgSend("Begin Front panel database read");
#endif

   _DtDbRead (dir_paths, FILE_TYPE_SUFFIX, record_descriptor, 6);

#ifdef DT_PERFORMANCE
_DtPerfChkpntMsgSend("End   Front panel database read");
#endif

   if (panel_count == 0)
   {
      _DtFreeDatabaseDirPaths (dir_paths);
      return False;
   }

   /*  Once all of the PANEL records have been read in, select the panel   */
   /*  record that is to be used to build the front panel.                 */
   /*                                                                      */
   /*  This selection is done by first scanning the list to find a LOCKED  */
   /*  panel.  The first one found is selected.  If none are found, the    */
   /*  last panel read is selectd.                                         */
   
   for (i = panel_count - 1; i > 0; i--)
   {
      if ((intptr_t)(panel_data[i].element_values[PANEL_LOCKED].parsed_value))
         break;
   }

   panel.element_values = panel_data[i].element_values;


   /*  Parse out additional panel element values  */

   InitializeField (panel.element_values, PANEL_GEOMETRY, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_DISPLAY_HANDLES, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_DISPLAY_MENU, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_DISPLAY_MINIMIZE, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_RESOLUTION, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_CONTROL_BEHAVIOR, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_DISPLAY_CONTROL_LABELS, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_SUBPANEL_UNPOST, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_HELP_STRING, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_HELP_TOPIC, panel_parse_functions);
   InitializeField (panel.element_values, PANEL_HELP_VOLUME, panel_parse_functions);

   XtFree ((char *) panel_data);


   /*  Eliminate the records within the other component types that  */
   /*  are not part of this selected panel.                         */
   /*                                                               */
   /*  This elimination occurs top down because boxes have to be    */
   /*  removed before controls can be removed...                    */
   
   name_list = (char **) XtMalloc (sizeof (char **) * 2);
   name_list[0] = panel.element_values[PANEL_NAME].parsed_value;
   name_list[1] = NULL;
   EliminateUnused (box_data, &box_count, name_list, BOX_CONTAINER_NAME,
		    ANY_CONTAINER_TYPE, PANEL, BOX);
   XtFree ((char *) name_list);

   /* Remove entries that have the deleted keyword set to True unless */
   /* it is a locked file.					      */
   EliminateDeleted (box_data, &box_count, BOX_CONTAINER_NAME,
		     BOX_NAME, BOX, BOX_DELETE, BOX_LOCKED);
   EliminateDeleted (switch_data, &switch_count, SWITCH_CONTAINER_NAME,
		     SWITCH_NAME, SWITCH, SWITCH_DELETE, SWITCH_LOCKED);
   EliminateDeleted (subpanel_data, &subpanel_count, SUBPANEL_CONTAINER_NAME,
                     SUBPANEL_NAME, SUBPANEL, SUBPANEL_DELETE, SUBPANEL_LOCKED);
   EliminateDeleted (control_data, &control_count, CONTROL_CONTAINER_NAME,
                     CONTROL_NAME, CONTROL, CONTROL_DELETE, CONTROL_LOCKED);

   name_list = GetNameList (box_data, box_count, 
                            BOX_NAME, ANY_CONTAINER_TYPE, 0);

   EliminateUnused(switch_data, &switch_count, name_list,
		   SWITCH_CONTAINER_NAME, ANY_CONTAINER_TYPE,
		   BOX, SWITCH);

   EliminateUnused(control_data, &control_count, name_list,
		   CONTROL_CONTAINER_NAME, CONTROL_CONTAINER_TYPE,
		   BOX, CONTROL);

   XtFree ((char *) name_list);


   /*  Once the elimination of superfluous switch definitions is complete,  */
   /*  make the selection of the single Switch.  This makes the elimination */
   /*  of the switch based controls efficient.                              */
   /*                                                                       */
   /*  This selection is done by first scanning the list to find a LOCKED   */
   /*  switch.  The first one found is selected.  If none are found, the    */
   /*  last switch read is selected.                                        */

   if (switch_count != 0) {
     for (i = switch_count - 1; i > 0; i--)
     {
       if ((intptr_t)(switch_data[i].element_values[SWITCH_LOCKED].parsed_value))
            break;
     }

     switch1 = (SwitchData *) XtMalloc (sizeof (SwitchData));
     switch1->element_values = switch_data[i].element_values;
     switch1->control_data = NULL;
     switch1->control_data_count = 0;
     switch1->rc = NULL;
     switch1->buttons = NULL;
     switch1->atom_names = NULL;
     switch1->switch_names = NULL;
     switch1->switch_count = 0;
     switch1->active_switch = 0;
     switch1->popup_data = NULL;


      /*  Parse the remaining database values for the switch  */
   
     InitializeField (switch1->element_values, SWITCH_POSITION_HINTS, switch_parse_functions);
     InitializeField (switch1->element_values, SWITCH_NUMBER_OF_ROWS, switch_parse_functions);
     InitializeField (switch1->element_values, SWITCH_HELP_STRING, switch_parse_functions);
     InitializeField (switch1->element_values, SWITCH_HELP_TOPIC, switch_parse_functions);
     InitializeField (switch1->element_values, SWITCH_HELP_STRING, switch_parse_functions);
   }

   XtFree ((char *) switch_data);


   /*  Continue the elimination of unused records  */

   name_list = (char **) XtMalloc (sizeof (char **) * 2);
   if (switch_count != 0)
      name_list[0] = switch1->element_values[SWITCH_NAME].parsed_value;
   else
      name_list[0] = NULL;
   name_list[1] = NULL;
   EliminateUnused (control_data, &control_count, name_list,
		    CONTROL_CONTAINER_NAME, CONTROL_CONTAINER_TYPE,
		    SWITCH, CONTROL);
   XtFree ((char *)name_list);

   name_list = GetNameList (control_data, control_count, 
                            CONTROL_NAME, CONTROL_CONTAINER_TYPE, BOX);

   EliminateUnused (subpanel_data, &subpanel_count, name_list,
		    SUBPANEL_CONTAINER_NAME, ANY_CONTAINER_TYPE,
		    CONTROL, SUBPANEL);
   XtFree ((char *)name_list);

   name_list = GetNameList (subpanel_data, subpanel_count, 
                            SUBPANEL_NAME, ANY_CONTAINER_TYPE, SUBPANEL);

   EliminateUnused (control_data, &control_count, name_list,
		   CONTROL_CONTAINER_NAME, CONTROL_CONTAINER_TYPE,
		   SUBPANEL, CONTROL);

   XtFree ((char *)name_list);
		    

   /*  The record types of CONTROL, SUBPANEL and BOX still have need  */
   /*  several steps of processing.                                   */

   ReorderByContainerType(control_data, control_count, CONTROL_CONTAINER_TYPE);

   ReorderByContainerName(control_data, control_count, CONTROL_CONTAINER_NAME,
		          CONTROL_CONTAINER_TYPE);

   ReorderByName(box_data, box_count, BOX_NAME, BOX_CONTAINER_NAME);

   ReorderByName(control_data, control_count, CONTROL_NAME,
		 CONTROL_CONTAINER_NAME);

#ifdef DEBUG
   for (i = 0; i < control_count; i++) {
       printf("control %s, %d, %s\n",
            control_data[i].element_values[CONTROL_CONTAINER_NAME].parsed_value,
            control_data[i].element_values[CONTROL_CONTAINER_TYPE].parsed_value,
            control_data[i].element_values[CONTROL_NAME].parsed_value);
   }
#endif


   /*  Reprocess each of these groups to eliminate overridden     */
   /*  components.  If a component is locked, use the first lock  */
   /*  encountered.  Otherwise, use the last component read.      */

   ResolveDuplicates (box_data, &box_count, BOX_NAME,
		      ANY_CONTAINER_TYPE, BOX_CONTAINER_NAME,
		      BOX_LOCKED, BOX);
   ResolveDuplicates (subpanel_data, &subpanel_count, SUBPANEL_NAME,
		      ANY_CONTAINER_TYPE, SUBPANEL_CONTAINER_NAME, 
		      SUBPANEL_LOCKED, SUBPANEL);
   ResolveDuplicates (control_data, &control_count, CONTROL_NAME,
		      CONTROL_CONTAINER_TYPE, CONTROL_CONTAINER_NAME, 
		      CONTROL_LOCKED, CONTROL);
   
   /*  Now that the elimination has been completed, initialize the  */
   /*  remaining values for each of the components.                 */

   for (i = 0; i < box_count; i++) 
   {
      InitializeField (box_data[i].element_values,
                       BOX_POSITION_HINTS, box_parse_functions);
      InitializeField (box_data[i].element_values,
                       BOX_HELP_STRING, box_parse_functions);
      InitializeField (box_data[i].element_values,
                       BOX_HELP_TOPIC, box_parse_functions);
      InitializeField (box_data[i].element_values,
                       BOX_HELP_VOLUME, box_parse_functions);
   }

   for (i = 0; i < subpanel_count; i++)
   {
      InitializeField (subpanel_data[i].element_values,
                       SUBPANEL_CONTROL_INSTALL, subpanel_parse_functions);
      InitializeField (subpanel_data[i].element_values,
                       SUBPANEL_TITLE, subpanel_parse_functions);
      InitializeField (subpanel_data[i].element_values,
                       SUBPANEL_HELP_STRING, subpanel_parse_functions);
      InitializeField (subpanel_data[i].element_values,
                       SUBPANEL_HELP_TOPIC, subpanel_parse_functions);
      InitializeField (subpanel_data[i].element_values,
                       SUBPANEL_HELP_VOLUME, subpanel_parse_functions);
   }

   for (i = 0; i < control_count; i++)
   {
      InitializeSecondaryControlFields (control_data[i].element_values);
      InitializeFileControlFields (control_data[i].element_values, NULL);
   }

#ifdef DEBUG
   for (i = 0; i < box_count; i++) {
       printf("box %d = %s\n",
             box_data[i].element_values[BOX_POSITION_HINTS].parsed_value,
             box_data[i].element_values[BOX_NAME].parsed_value);
   }

   for (i = 0; i < subpanel_count; i++) {
       printf("subpanel = %s\n",
             subpanel_data[i].element_values[SUBPANEL_NAME].parsed_value);
   }

   for (i = 0; i < control_count; i++) {
       printf("control %d = %s\n",
            control_data[i].element_values[CONTROL_POSITION_HINTS].parsed_value,
            control_data[i].element_values[CONTROL_NAME].parsed_value);
   }
#endif

   /*  Reorder subgroups according to position hints.  */

   ReorderByPosition (box_data, box_count, BOX_POSITION_HINTS,
		      ANY_CONTAINER_TYPE, BOX_CONTAINER_NAME);
   ReorderByPosition (control_data, control_count, CONTROL_POSITION_HINTS,
		      CONTROL_CONTAINER_TYPE, CONTROL_CONTAINER_NAME);
 
#ifdef DEBUG
   printf("**************************************************************\n\n");

   for (i = 0; i < box_count; i++) {
       printf("box %d = %s\n", (int)
             box_data[i].element_values[BOX_POSITION_HINTS].parsed_value,
             box_data[i].element_values[BOX_NAME].parsed_value);
   }

   for (i = 0; i < control_count; i++) {
       printf("control %d = %s\n", (int)
            control_data[i].element_values[CONTROL_POSITION_HINTS].parsed_value,
            control_data[i].element_values[CONTROL_NAME].parsed_value);
   }
#endif


   /*  Build hierarchical data structure from record sets, top-down.  */
   /*  Fully parse each record as the structure is built.             */

   panel.box_data = (BoxData **) XtMalloc (sizeof (BoxData *) * box_count);
   panel.box_data_count = box_count;
   
   for (i = 0; i < box_count; i++)
   {
       BoxData * box;
       
       panel.box_data[i] = (BoxData *) XtMalloc (sizeof (BoxData));
       box = panel.box_data[i];
       
       box->element_values = box_data[i].element_values;
       box->control_data = NULL;
       box->control_data_count = 0;
       box->form = NULL;
       box->left_arrow_form = NULL;
       box->left_control_form = NULL;
       box->right_arrow_form = NULL;
       box->right_control_form = NULL;
       box->switch_data = NULL;
       box->switch_form = NULL;
       box->switch_edit = NULL;
       box->subpanel_count = 0;

       
       if (switch_count != 0 &&
	   strcmp ((char *) box->element_values[BOX_NAME].parsed_value,
                   (char *) switch1->element_values[SWITCH_CONTAINER_NAME].parsed_value) == 0)
       {
          box->switch_data = switch1;
          switch1->box_data = box;
          ProcessSwitch (box->switch_data);
       }


       /*  Call a function to process each box, allocating the control  */
       /*  data structures needed for them and calling functions to     */
       /*  further process each control.                                */
       
       ProcessBox (box);
   }

   XtFree ((char *) box_data);
   _DtFreeDatabaseDirPaths (dir_paths);

   return True;
}


 
 
/************************************************************************
 *
 *  PanelParseCB
 *	Allocate a structure to contain the Panel record returned by
 *      the database read code and copy the record information into the
 *	structure.
 *
 ************************************************************************/

static Boolean
PanelParseCB (DtDtsDbField  * fields,
              DtDbPathId   path_id, 
              char        * host_prefix,
              Boolean       rejection)

{
   AllocateRecordData (&panel_data, 
                       sizeof (ElementValue) * PANEL_KEYWORD_COUNT,
                       &panel_data_count, &panel_count);

   ProcessRecord (fields, (ElementValue *) 
                  panel_data[panel_count - 1].element_values);

   InitializeField (panel_data[panel_count - 1].element_values,
                    PANEL_NAME, panel_parse_functions);
   InitializeField (panel_data[panel_count - 1].element_values,
                    PANEL_LOCKED, panel_parse_functions);

   return (True);
}




/************************************************************************
 *
 *  BoxParseCB
 *	Allocate a structure to contain the Box record returned by
 *      the database read code and copy the record information into the
 *	structure.
 *
 ************************************************************************/

static Boolean
BoxParseCB (DtDtsDbField  * fields,
            DtDbPathId   path_id, 
            char        * host_prefix,
            Boolean       rejection)


{
   AllocateRecordData (&box_data, 
                       sizeof (ElementValue) * BOX_KEYWORD_COUNT,
                       &box_data_count, &box_count);

   ProcessRecord (fields, box_data[box_count - 1].element_values);

   InitializeField (box_data[box_count - 1].element_values,
                    BOX_NAME, box_parse_functions);
   InitializeField (box_data[box_count - 1].element_values,
                    BOX_CONTAINER_NAME, box_parse_functions);
   InitializeField (box_data[box_count - 1].element_values,
                    BOX_LOCKED, box_parse_functions);
   InitializeField (box_data[box_count - 1].element_values,
                    BOX_DELETE, box_parse_functions);

   OrderRecord (box_data, box_count, BOX_CONTAINER_NAME);

   return (True);
} 



 
/************************************************************************
 *
 *  SubpanelParseCB
 *	Allocate a structure to contain the Subpanel record returned by
 *      the database read code and copy the record information into the
 *	structure.
 *
 ************************************************************************/

static Boolean
SubpanelParseCB (DtDtsDbField  * fields,
                 DtDbPathId   path_id, 
                 char        * host_prefix,
                 Boolean       rejection)

{
   AllocateRecordData (&subpanel_data,
                       sizeof (ElementValue) * SUBPANEL_KEYWORD_COUNT,
                       &subpanel_data_count, &subpanel_count);

   ProcessRecord (fields, subpanel_data[subpanel_count - 1].element_values);

   InitializeField (subpanel_data[subpanel_count - 1].element_values,
                    SUBPANEL_NAME, subpanel_parse_functions);
   InitializeField (subpanel_data[subpanel_count - 1].element_values,
                    SUBPANEL_CONTAINER_NAME, subpanel_parse_functions);
   InitializeField (subpanel_data[subpanel_count - 1].element_values,
                    SUBPANEL_LOCKED, subpanel_parse_functions);
   InitializeField (subpanel_data[subpanel_count - 1].element_values,
                    SUBPANEL_DELETE, subpanel_parse_functions);

   OrderRecord (subpanel_data, subpanel_count, SUBPANEL_CONTAINER_NAME);

   return (True);
}
 


 
/************************************************************************
 *
 *  SwitchParseCB
 *	Allocate a structure to contain the Switch record returned by
 *      the database read code and copy the record information into the
 *	structure.
 *
 ************************************************************************/

static Boolean
SwitchParseCB (DtDtsDbField  * fields,
               DtDbPathId   path_id, 
               char        * host_prefix,
               Boolean       rejection)

{
   AllocateRecordData (&switch_data,
                       sizeof (ElementValue) * SWITCH_KEYWORD_COUNT,
                       &switch_data_count, &switch_count);

   ProcessRecord (fields, switch_data[switch_count - 1].element_values);

   InitializeField (switch_data[switch_count - 1].element_values,
                    SWITCH_NAME, switch_parse_functions);
   InitializeField (switch_data[switch_count - 1].element_values,
                    SWITCH_CONTAINER_NAME, switch_parse_functions);
   InitializeField (switch_data[switch_count - 1].element_values,
                    SWITCH_LOCKED, switch_parse_functions);
   InitializeField (switch_data[switch_count - 1].element_values,
                    SWITCH_DELETE, switch_parse_functions);

   OrderRecord (switch_data, switch_count, SWITCH_CONTAINER_NAME);

   return (True);
} 



/************************************************************************
 *
 *  ControlParseCB
 *	Allocate a structure to contain the Control record returned by
 *      the database read code and copy the record information into the
 *	structure.
 *
 ************************************************************************/

static Boolean
ControlParseCB (DtDtsDbField  * fields,
                DtDbPathId   path_id, 
                char        * host_prefix,
                Boolean       rejection)

{
   AllocateRecordData (&control_data,
                       sizeof (ElementValue) * CONTROL_KEYWORD_COUNT,
                       &control_data_count, &control_count);

   ProcessRecord (fields, control_data[control_count - 1].element_values);

   InitializePrimaryControlFields
			(control_data[control_count - 1].element_values);

   OrderRecord (control_data, control_count, CONTROL_CONTAINER_NAME);

   return (True);
}




/************************************************************************
 *
 *  AnimationParseCB
 *	Allocate a structure to contain the Animation records returned by
 *      the database read code and copy the record information into the
 *	structure.
 *
 ************************************************************************/

static Boolean
AnimationParseCB (DtDtsDbField  * fields,
                  DtDbPathId   path_id, 
                  char       * host_prefix,
                  Boolean      rejection)

{
   unsigned char *string, *source, *head_ptr;
   char * field1;
   char * field2;
   int count = panel.animation_count;
   int field_count = 0;
   void * val_rtn;
   int i;
   XrmQuark	animation_quark = XrmStringToQuark(ANIMATION);

   panel.animation_data = (AnimationData *) 
                          XtRealloc ((char *) panel.animation_data, 
                                     sizeof (AnimationData) * (count + 1));

   panel.animation_data[count].name = strdup(fields[0].fieldValue);


   for (i = 1; fields[i].fieldName != NULLQUARK; i++)
   {
       if (fields[i].fieldName == animation_quark)
          field_count++;
   }

   panel.animation_data[count].item_count = field_count;

   panel.animation_data[count].items = (AnimationItem *)
			 XtMalloc(sizeof(AnimationItem) * field_count);

   for (i = 0; i < field_count; i++)
   {
     if (fields[i+1].fieldName == animation_quark)
     {
        head_ptr = source =
		 (unsigned char *) strdup((char *)fields[i+1].fieldValue);

        if ((string = _DtWmParseNextTokenC(&source, False)) != NULL)
	{
	   panel.animation_data[count].items[i].image_name =
						 strdup((char *) string);
	   if ((string = _DtWmParseNextTokenC(&source, False)) != NULL)
	   {
	      StringToInt((char *)string, &val_rtn);
	      panel.animation_data[count].items[i].delay = (int) (intptr_t) val_rtn;
	   }
	   else
	   {
	      panel.animation_data[count].items[i].delay = 0;
	   }
	}

	free((char *)head_ptr);
     }
   }
     
   panel.animation_count++;

   return (True);
}



/************************************************************************
 *
 *  ControlSingleParseCB
 *	Allocate a structure to contain the Control record returned by
 *      the database read code and copy the record information into the
 *	structure.
 *
 ************************************************************************/

static Boolean
ControlSingleParseCB (DtDtsDbField  * fields,
                DtDbPathId   path_id, 
                char        * host_prefix,
                Boolean       rejection)

{
   if (control_element_value_found) {
	return (True);
   }

   control_element_values = (ElementValue *) XtMalloc (sizeof(ElementValue) *
						       CONTROL_KEYWORD_COUNT);
   control_element_value_found = True;

   ProcessRecord (fields, control_element_values);

   InitializeControlFields(control_element_values, NULL);

   return (True);
}


 
 
/************************************************************************
 *
 *  ProcessRecord
 *	Extract the fields of a returned record and put them into an
 *      allocated structure for latter processing.
 *
 ************************************************************************/

static void
ProcessRecord (DtDtsDbField   * fields,
               ElementValue * element_values)

{
   int i, j;
   int entry;

   int keyword_count;
   char ** field_keywords;


   /*  Find the position in the record structure for the value of   */
   /*  the entry keyword contained in the first position of the     */
   /*  fields array                                                 */
   
   for (entry = 0; entry < TOTAL_ENTRY_COUNT; entry++)
   {
      if (fields[0].fieldName == XrmStringToQuark(record_keywords[entry].record_keyword))
      {
         keyword_count = record_keywords[entry].keyword_count;
         field_keywords = record_keywords[entry].field_keywords;
         break;
      }
   }


   /*  Initialize the element_values array  */
   
   for (i = 0; i < keyword_count; i++)
   {
      (element_values + i)->use_default = True;
      (element_values + i)->string_value = NULL;
      (element_values + i)->parsed_value = NULL;
   }


   /*  Loop through each read in keyword value pair, comparing it  */
   /*  to the keywords for this record type and when found, enter  */
   /*  it into the element values array for the component.         */

   for (i = 0; fields[i].fieldName != NULLQUARK; i++)
   {
      XrmQuark field1 = fields[i].fieldName;
      char * field2 = fields[i].fieldValue;


      /*  Search the field_keywords list and when the keyword is found    */
      /*  make a copy of field2 and place in its position.                */
      
      for (j = 0; j < keyword_count; j++)
      {
	 if (field1 == XrmStringToQuark(field_keywords[j]))
	 {
            (element_values + j)->use_default = False;
            (element_values + j)->string_value = strdup (field2);
            break;
	 }
      }

      if (j == keyword_count)
         _DtSimpleError (panel.app_name, DtWarning, NULL,
                         "Invalid keyword -- %s", XrmQuarkToString(field1));
   }
}




/************************************************************************
 *
 *  AllocateRecordData
 *	Allocate a structure to contain the values for a data base record
 *	and stuff it in an array for latter processing.  Do all of this
 *	by indirection so that multiple record types can be handled.
 *
 ************************************************************************/

static void
AllocateRecordData (RecordData ** record_data,
                    int           record_size,
                    int         * record_data_count,
                    int         * record_count)

{
   if (*record_count >= *record_data_count)
   {
      *record_data_count += 10;
      *record_data = (RecordData *) 
                       XtRealloc ((char *) *record_data, 
                                  sizeof (RecordData *) * (*record_data_count));
   }

   (*record_data)[*record_count].element_values = 
                                       (ElementValue *) XtMalloc (record_size);
      
            
   (*record_count)++;
}



/************************************************************************
 *
 *  OrderRecord
 *	Compare the value in the last element values struture referenced
 *      by value_define to previous element values.  Move the element
 *      value structure to a new position defined by the comparison
 *      value being equal or greater the previous position and less
 *      than the next position
 *
 *      The function works on string data.
 *
 ************************************************************************/

static void
OrderRecord (RecordData  * record_data,
             int           record_count,
	     int           value_define)

{
   int i;
   int new_loc;

   char         * position_value;
   char         * compare_value;
   ElementValue * element_values;


   /*  If this is the first record, it is already in position  */
   
   if (record_count == 1)
      return;


   /*  Extract the comparison value and loop backward through the  */
   /*  record_data to find the correct                             */

   element_values = record_data[record_count - 1].element_values;
   position_value = element_values[value_define].parsed_value;

   for (new_loc = record_count - 2; new_loc >= 0; new_loc--)
   {
      compare_value =
         record_data[new_loc].element_values[value_define].parsed_value;

      if (strcmp (position_value, compare_value) >= 0)
         break;

      record_data[new_loc + 1].element_values = 
         record_data[new_loc].element_values;

   } 


   /*  If any values have been moved, place the position value into  */
   /*  the vacated spot.                                             */
   
   if (new_loc != record_count - 2)
      record_data[new_loc + 1].element_values = element_values;
}




/************************************************************************
 *
 *  SwapEntries
 *      Swap element value records based on positions.
 *
 ************************************************************************/

static void
SwapEntries (RecordData * rec_data,
             int          i,
	     int          j)


{
   ElementValue * temp_value;

   temp_value = rec_data[i].element_values;
   rec_data[i].element_values = rec_data[j].element_values;
   rec_data[j].element_values = temp_value;
}




/************************************************************************
 *
 *  QuickSort
 *      Reorder the records according to element index.  This routine
 *	performs a quick sort.  Positions that are equal will retain current
 *	positioning.
 *
 ************************************************************************/

#ifdef NOT_DEF
static void
QuickSort(RecordData * rec_data,
	  int	       left,
	  int	       right,
	  int	       elem_index,
	  int	       container)
{
   int i, last;
     
   if (left >= right)
      return;

   SwapEntries(rec_data, left, (left + right)/2);

   last = left;

   if (container == BOX && elem_index == BOX_POSITION_HINTS ||
       (container == CONTROL && elem_index == CONTROL_POSITION_HINTS))
   {
     for (i = left + 1; i <= right; i++)
 	if (rec_data[i].element_values[elem_index].parsed_value <
	    rec_data[left].element_values[elem_index].parsed_value)
           SwapEntries(rec_data, ++last, i);
   }
   else
   {
     for (i = left + 1; i <= right; i++)
  	if (strcmp(rec_data[i].element_values[elem_index].parsed_value,
	           rec_data[left].element_values[elem_index].parsed_value) < 0)
           SwapEntries(rec_data, ++last, i);
   }

   SwapEntries(rec_data, left, last);

   QuickSort(rec_data, left, last - 1, elem_index, container);
   QuickSort(rec_data, last + 1, right, elem_index, container);
}
#endif /* NOT_DEF */




/************************************************************************
 *
 *  BubbleSort
 *      Reorder the records according to element index.  This routine
 *	performs a bubble sort.  Positions that are equal will retain current
 *	positioning.
 *
 ************************************************************************/

static void
BubbleSort (RecordData * rec_data,
            int          start,
            int          rec_count,
            int	         elem_index)


{
   int bound, t, j;
   Boolean is_string = False;
   char *str1, *str2;

   bound = rec_count;

   if (elem_index == CONTROL_CONTAINER_NAME ||
       elem_index == CONTROL_NAME)
      is_string = True;

   while (bound != 0)
   {
      t = 0;
      for (j = start; j < bound; j++)
      {
        if (is_string)
        {
           str1 = (char *)rec_data[j].element_values[elem_index].parsed_value;
           str2 = (char *)rec_data[j+1].element_values[elem_index].parsed_value;
           if (strcmp(str1, str2) > 0)
           {
                 SwapEntries(rec_data, j, j+1);
                 t = j;
           }
        }
        else
        {
           if ((intptr_t) rec_data[j].element_values[elem_index].parsed_value >
               (intptr_t) rec_data[j+1].element_values[elem_index].parsed_value)
           {
              SwapEntries(rec_data, j, j+1);
              t = j;
           }
        }
      }
      bound = t;
   }
}




/************************************************************************
 *
 *  ReversePositions
 *      Reverse order of the records.  This routine reverses the order of
 *	the records so that user records that have the same positioning
 *	will get a higher presidence.
 *
 *
 ************************************************************************/

static void
ReversePositions (RecordData * rec_data, 
                  int          start, 
		  int          end)


{
   int i, j;

   for (i = start, j = end; i < j; i++, j--)
   {
       SwapEntries(rec_data, i, j);
   }
}




/************************************************************************
 *
 *  ReorderByContainerName
 *      Reorder the records according to container name.  This will be done
 *	using a quick sort.  Container type order must be retained.  Matchinng
 *	names will retain current ordering.
 *	
 *************************************************************************/

static void
ReorderByContainerName (RecordData * rec_data,
                        int	     rec_count,
                        int          name,
                        int          elem_type)

{
   int i = 0, start = 0;
   int cont_type;
   
   while (i < rec_count)
   {
       cont_type = (int) (intptr_t)
		   (rec_data[start].element_values[elem_type].parsed_value);
       while ((intptr_t)(rec_data[i].element_values[elem_type].parsed_value) ==
	      cont_type)
       {
           i++;
           if (rec_count == i) break;
       }

       BubbleSort(rec_data, start, i - 1, name);
       start = i;
   }
}




/************************************************************************
 *
 *  ReorderByName
 *      Reorder the records according to names.  This will be done
 *	using a quick sort.  Container name order must be retained.
 *      Matching names will retain current order.
 *
 *************************************************************************/

static void
ReorderByName (RecordData * rec_data,
               int	    rec_count,
               int          name,
               int          elem_type)

{
   int i = 0, start = 0;
   char *cont_name;
   
   while (i < rec_count)
   {
       cont_name = (char *)
                  (rec_data[start].element_values[elem_type].parsed_value);
       while (strcmp((char *)rec_data[i].element_values[elem_type].parsed_value,
	             cont_name) == 0)
       {
             i++;
             if (rec_count == i) break;
       }

       BubbleSort(rec_data, start, i - 1, name);
       start = i;
   }
}




/************************************************************************
 *
 *  ReorderByPosition
 *      Reorder the records according to position_hints.  This will be done
 *	using a quick sort.  Positions that are equal will retain current
 *	positioning.
 *************************************************************************/

static void
ReorderByPosition(RecordData *rec_data,
		  int	      rec_count,
		  int	      pos_hints,
		  int	      cont_type,
		  int	      cont_name)

{
   int   i = 0, start = 0;
   int   container_type;
   char  *container_name;

   ReversePositions(rec_data, 0, rec_count - 1);

   while (i < rec_count)
   {
       container_name = (char *)
                  (rec_data[start].element_values[cont_name].parsed_value);

       if (cont_type == ANY_CONTAINER_TYPE)
       {
          while (strcmp((char *)
                        rec_data[i].element_values[cont_name].parsed_value,
                        container_name) == 0)
          {
               i++;
               if (rec_count == i) break;
          }
       }
       else
       {
          container_type = (int) (intptr_t)
                  (rec_data[start].element_values[cont_type].parsed_value);
          while (((intptr_t)rec_data[i].element_values[cont_type].parsed_value ==
                                                           container_type) &&
                 (strcmp((char *)
		 	 rec_data[i].element_values[cont_name].parsed_value,
                         container_name) == 0))
          {
               i++;
               if (rec_count == i) break;
          }
       }

       BubbleSort(rec_data, start, i - 1, pos_hints);
       start = i;
   }

}




/************************************************************************
 *
 *  ReorderByContainerType
 *      Reorder the records according to container type.  This will be done
 *	using a bubble sort.  Container types that are match must retain
 *      current positioning.
 *
 *************************************************************************/

static void
ReorderByContainerType (RecordData * rec_data,
                        int	     rec_count,
                        int          container_type)

{
   BubbleSort(rec_data, 0, rec_count - 1, container_type);
}




/************************************************************************
 *
 *  RemoveEntry
 *
 ************************************************************************/
 
void
RemoveEntry (RecordData * record_data,
             int          record_type)


{
   int i;
 
   for (i = 0; i < record_descriptor[record_type].maxFields; i++)
   {
       if (record_keywords[record_type].parse_functions[i].free_function
							     != NULL &&
	   record_data->element_values[i].parsed_value != NULL &&
	   record_data->element_values[i].use_default == False)
       {
	  (*(record_keywords[record_type].parse_functions[i].free_function))
	    (&(record_data->element_values[i].parsed_value));
       }

       if (record_data->element_values[i].use_default == False &&
           record_data->element_values[i].string_value != NULL)
          free(record_data->element_values[i].string_value);
   }
		    
   XtFree((char *)record_data->element_values);
}




/************************************************************************
 *
 *  EliminateEntries
 *      Given a container type and the indicies of the records to be deleted,
 *      delete any components which are within the specified container type
 *      and range.
 *
 ************************************************************************/

static void
EliminateEntries (RecordData * record_data,
                  int        * record_count,
                  int          start,
                  int          end,
                  int        * i,
                  int        * count,
                  int          record_type)

{
   int j, k;

   if (start > end) return;

   for (j = start; j <= end; j++)
   {
       RemoveEntry(&(record_data[j]), record_type);
   }

   if (end + 1 < *record_count)
   {
      memmove((void *) &record_data[start], (void *) &record_data[end+1],
              (size_t) ((*record_count - end) * sizeof(RecordData)));
   }


   *i += (end - start) + 1;
   *count = *record_count -= (end - start) + 1;
}




/************************************************************************
 *
 *  ResolveDuplicates
 *      Reprocess each group to eliminate overridden components.
 *      If a component is locked, use the first lock encountered.
 *      Otherwise, use the last component read.   
 *
 ************************************************************************/

static void
ResolveDuplicates (RecordData * record_data,
                   int        * record_count,
	           int          name_type,
	           int          container_type,
	           int          container_name,
		   int	        lock_type,
		   int          record_type)

{
   int i, start_index, lock_index, last_index;
   ElementValue * element_values;
   int cont_type;
   char *cont_name, *record_name;
   int count = *record_count;
   Boolean locked;

   i = count - 1;

   /* Go through the array of records */

   while (i >= 0)
   {
      element_values = record_data[i].element_values;
      if (ANY_CONTAINER_TYPE != container_type)
      {
         cont_type = (int) (intptr_t) (element_values[container_type].parsed_value);
         cont_name = (char *) (element_values[container_name].parsed_value);
      }
      record_name = (char *) (element_values[name_type].parsed_value);

      locked = (Boolean) (intptr_t) (element_values[lock_type].parsed_value);
      if (locked)
      {
         i--;
         if (i < 0) break;
         start_index = last_index = i;
         element_values = record_data[i].element_values;
         if (ANY_CONTAINER_TYPE == container_type)
         {
            while ((strcmp((char *) (element_values[name_type].parsed_value),
                           cont_name) == 0))
            {
               last_index = i;
               i--;
               if (i < 0) break;
               element_values = record_data[i].element_values;
            }
         }
         else
         { 
            while (((intptr_t)(element_values[container_type].parsed_value)
	                  == cont_type) &&
                  (strcmp((char *)(element_values[container_name].parsed_value),
	                   cont_name) == 0) &&
                  (strcmp((char *)(element_values[name_type].parsed_value),
                           record_name) == 0))
            {
               last_index = i;
               i--;
               if (i < 0) break;
               element_values = record_data[i].element_values;
            }
         }
         if (start_index != last_index)
            EliminateEntries(record_data, record_count, last_index,
			     start_index - 1, &i, &count, record_type);

         if (i < 0) break;
      }
      else
      {
         start_index = lock_index = last_index = i;
         i--;
         if (i < 0) continue;
         element_values = record_data[i].element_values;
         if (ANY_CONTAINER_TYPE == container_type)
         {
            while ((strcmp((char *) (element_values[name_type].parsed_value),
                           record_name) == 0))
            {
                locked = (Boolean) (intptr_t) (element_values[lock_type].parsed_value);
                if (locked && start_index == lock_index)
                   lock_index = i;
                last_index = i;
                i--;
                if (i < 0) break;
                element_values = record_data[i].element_values;
            }
         }
         else
         {
            while (((intptr_t)(element_values[container_type].parsed_value)
	                   == cont_type) &&
                  (strcmp((char *)(element_values[container_name].parsed_value),
	                   cont_name) == 0) &&
                  (strcmp((char *)(element_values[name_type].parsed_value),
                           record_name) == 0))
            {
                locked = (Boolean) (intptr_t) (element_values[lock_type].parsed_value);
                if (locked && start_index == lock_index)
                   lock_index = i;
                last_index = i;
                i--;
                if (i < 0) break;
                element_values = record_data[i].element_values;
            }
         }
         if (start_index != last_index)
         {
            if (start_index == lock_index)
	       EliminateEntries(record_data, record_count, last_index + 1,
				start_index, &i, &count, record_type);
            else
            {
               int diff = start_index - lock_index;
	       EliminateEntries(record_data, record_count, lock_index + 1,
				start_index, &i, &count, record_type);
               lock_index += diff;
               last_index += diff;
               if (lock_index != last_index)
	          EliminateEntries(record_data, record_count, last_index,
				   lock_index - 1, &i, &count, record_type);
            }
         }
         if (i < 0) break;
      }
   }
}




/************************************************************************
 *
 *  GetNameList
 *      Given a record_data array and a particular container type, 
 *      allocate an array of pointers to strings and set to the component
 *      names of each component which matches the above criteria.
 *
 ************************************************************************/

static char **
GetNameList (RecordData * record_data,
             int          record_count,
	     int          name_type,
	     int          container_type,
             int          container)

{
   int i;

   char ** name_list = NULL;
   int     name_list_count = 0;
   int     found_count = 0;

   for (i = 0; i < record_count; i++)
   {
      if (ANY_CONTAINER_TYPE == container_type ||
          (intptr_t)(record_data[i].element_values[container_type].parsed_value) == 
          container)
      {
	 if (found_count >= name_list_count)
	 {
            name_list_count += 10;
	    name_list = 
	       (char **) XtRealloc ((char *) name_list, 
                                   sizeof (char **) * (name_list_count + 1));
	 }	 
         name_list [found_count] = 
	    record_data[i].element_values[name_type].parsed_value;
         found_count++;
      }
   }

   if (name_list != NULL)
      name_list [found_count] = NULL;

   return (name_list);
}





/************************************************************************
 *
 *  EliminateUnused
 *      Given a record_data array, container name list and container type,
 *      delete any components which are within the specified container type
 *      but do not contain the container name.
 *
 ************************************************************************/

static void
EliminateUnused (RecordData    * record_data,
                 int           * record_count,
                 char         ** cont_name_list,
	         int             cont_name,
	         int             cont_type,
	         int             container,
		 int 		 record_type)

{
   int i, j, dummy = 0;
   char * container_name;
   int count = *record_count;
   Boolean name_found = False;
   ElementValue * element_values;

   i = 0;

   while (i < count)
   {
       element_values = record_data[i].element_values;
       if (cont_type == ANY_CONTAINER_TYPE ||
           (intptr_t)(element_values[cont_type].parsed_value) == container)
       {
          if (cont_name_list != NULL)
          {
             for (j = 0; cont_name_list[j] != NULL; j++)
	     {
	       container_name = cont_name_list[j];
	       if (strcmp((char *)element_values[cont_name].parsed_value,
			  container_name) == 0)
	       {
		  name_found = True;
		  break;
	       }
	     }
          }
          if (!name_found)
          {
#ifdef DEBUG
            printf("Entry Eliminated - %s\n",
		    (char *)element_values[cont_name].parsed_value);
#endif
	    EliminateEntries(record_data, record_count, i, i, &dummy, &count,
			     record_type);
          }
          else
          {
            name_found = False;
            i++;
          }
       }
       else
          i++;
   }
}



/************************************************************************
 *
 *  EliminateDeleted
 *      Given a record_data array, delete any components which have the
 *      DELETE keyword set to True.  Also delete any duplicates that match
 *      the container name, container type, record name, and record type 
 *      but do not have the LOCKED keyword set to True.
 *
 ************************************************************************/

static void
EliminateDeleted (RecordData    * record_data,
                 int           * record_count,
	         int             cont_name,
		 int 		 record_name,
		 int 		 record_type,
		 int 		 delete_type,
		 int		 lock_type)

{
   int i, j, start, dummy = 0;
   int count;
   ElementValue * element_values, * other_element_values;
   char * container_name, * rec_name;
   int container_type, rec_type;

   count = *record_count;
   i = 0;

   while (i < count)
   {
       start = i;
       element_values = record_data[i].element_values;
       if ((intptr_t)element_values[delete_type].parsed_value)
       {
          Boolean delete_rest = False;

          if (record_type == CONTROL)
          {
             rec_type = (intptr_t)element_values[CONTROL_TYPE].parsed_value;
             container_type =
		       (intptr_t)element_values[CONTROL_CONTAINER_TYPE].parsed_value;
          }
          container_name = 
             XtNewString ((char *)element_values[cont_name].parsed_value);
          rec_name = 
             XtNewString ((char *)element_values[record_name].parsed_value);

          for (j = count - 1; j >= 0; j--)
          {
             other_element_values = record_data[j].element_values;
             if ((record_type != CONTROL ||
                 ((intptr_t)other_element_values[CONTROL_CONTAINER_TYPE].parsed_value
		   == container_type &&
                  (intptr_t) other_element_values[CONTROL_TYPE].parsed_value
		   == rec_type)) &&
	         strcmp((char *)other_element_values[cont_name].parsed_value,
		        container_name) == 0 &&
	         strcmp((char *)other_element_values[record_name].parsed_value,
                        rec_name) == 0)
             {
	        if ((intptr_t)other_element_values[lock_type].parsed_value)
                {
                   if ((intptr_t)other_element_values[delete_type].parsed_value)
                   {
                       EliminateEntries(record_data, record_count, j, j, &dummy,
                                        &count, record_type);
                       delete_rest = True;
                   }
                   else if (delete_rest)
                   {
                       EliminateEntries(record_data, record_count, j, j, &dummy,
                                        &count, record_type);
                   }
                }
                else
                   EliminateEntries(record_data, record_count, j, j, &dummy,
                                    &count, record_type);
             }
          }

          XtFree(container_name);
          XtFree(rec_name);
       }
       else
          i++;
   }
}




/************************************************************************
 *
 *  InitializeField
 *	Given an ElementValues array, a particular keyword that references
 *	a single element and the parse function array for the record
 *	type:  if the element has a value string, parse it otherwise
 *	set the parsed value to the default.
 * 
 ************************************************************************/
 
static void
InitializeField (ElementValue  * element_values,
                 int             keyword,
                 ParseFunction * parse_functions)

{
   if (element_values[keyword].string_value != NULL)
   {
      element_values[keyword].use_default = False;

      if (!parse_functions[keyword].parse_function (
                                      element_values[keyword].string_value,
                                      &(element_values[keyword].parsed_value)))
      {
         XtFree(element_values[keyword].string_value);
         element_values[keyword].string_value = NULL;

         element_values[keyword].use_default = True;
         element_values[keyword].parsed_value = 
                                         parse_functions[keyword].default_value;
      }
   }
   else
   {
      element_values[keyword].use_default = True;
      element_values[keyword].parsed_value = 
         parse_functions[keyword].default_value;
   }	       
}



/************************************************************************
 *
 *  CountElements
 *      Given a record_data array, a container name and container type
 *      count how many records match and return the value.
 *
 ************************************************************************/

static int
CountElements (RecordData * record_data,
               int          record_count,
               char       * container_name,
               int          name_type,
	       int          container_type,
               int          container)


{
   int i;
   static int found_count = 0;

   for (i = 0; i < record_count; i++)
   {
      if (ANY_CONTAINER_TYPE == container_type ||
          (intptr_t)(record_data[i].element_values[container_type].parsed_value) == 
          container)
      {
         if (strcmp (container_name, 
                     record_data[i].element_values[name_type].parsed_value) ==0)
            found_count++;
      }
   }

   return (found_count);
}




/************************************************************************
 *
 *  DeleteControlActionList
 *
 ************************************************************************/
 

void
DeleteControlActionList (ControlData * control_data)


{
   int i;

   if (control_data->move_action != NULL)
      free(control_data->move_action);
   if (control_data->copy_action != NULL)
      free(control_data->copy_action);
   if (control_data->link_action != NULL)
      free(control_data->link_action);

   if (control_data->actions != NULL)
   {
       for (i = 0; control_data->actions[i] != NULL; i++)
       {
	  free (control_data->actions[i]->action_name);
	  if (control_data->actions[i]->action_label != NULL)
	     free (control_data->actions[i]->action_label);
	  XtFree ((char *) control_data->actions[i]);
       }

       XtFree((char *)control_data->actions);
   }
}




/************************************************************************
 *
 *  AddControlActionList
 *
 ************************************************************************/
 

void
AddControlActionList (ControlData * control_data)


{
   char * data_type = NULL;
   char * act_list = NULL;
   char * file_name = NULL;
   char * file_str;
   char ** action_list = NULL;
   int num_actions = 0;
   int i = 0;
   int result;
   Boolean is_file_control = False;
   PanelActionData * drop_action;
   char * label;
   int j = 0;


   if ((intptr_t)control_data->element_values[CONTROL_TYPE].parsed_value ==
								 CONTROL_FILE)
   {
      file_name = (char *)
		  control_data->element_values[CONTROL_FILE_NAME].parsed_value;

      data_type = DtDtsFileToDataType (file_name);
      is_file_control = True;   
   }
      
   if (data_type != NULL)
   {
      DtDtsAttribute  ** attr_list;
      char * attr[5];

      attr_list = DtDtsDataTypeToAttributeList (data_type, file_name);
      

      /*  The attribute list is ordered alphabetically by name so      */
      /*  do a linear search of the atr_list array and get the values  */
      /*  as use run into them.  Use the following array to get the    */
      /*  values for these attributes.                                 */

      attr[0] = DtDTS_DA_ACTION_LIST;
      attr[1] = DtDTS_DA_COPY_TO_ACTION;
      attr[2] = DtDTS_DA_LINK_TO_ACTION;
      attr[3] = DtDTS_DA_MOVE_TO_ACTION;

      if (attr_list != NULL)
      {
         for (i = 0; attr_list[i] != NULL; i++)
         {
            for (j = 0; j < 4; j++)
	       if (strcmp (attr_list[i]->name, attr[j]) == 0)
	          break;


            /*  If we have found a match, find the appropriate attr  */
            /*  and assign the value.                                */

            switch (j)
            {
               /* DtDTS_DA_ACTION_LIST */
               case 0:
                  act_list = (char *) strdup(attr_list[i]->value);
               break;

               /* DtDTS_DA_COPY_TO_ACTION */
               case 1:
                  control_data->copy_action =
				 (char *) strdup(attr_list[i]->value);
               break;

               /* DtDTS_DA_LINK_TO_ACTION */
               case 2:
                  control_data->link_action = 
		                 (char *) strdup(attr_list[i]->value);
               break;

	       /* DtDTS_DA_MOVE_TO_ACTION */
               case 3:
                  control_data->move_action =
				 (char *) strdup(attr_list[i]->value);
               break;

	       /* No Match */
               case 4:
               break;
            }
	 }
      }

      if (act_list)
      {
	 action_list = _DtVectorizeInPlace (act_list, ',');
	 for (i = 0; action_list[i] != NULL; i++)
	     num_actions++;
      }

      if (DtDtsDataTypeIsAction(data_type) && is_file_control)
	 control_data->is_action = True;

      if (attr_list != NULL)
         DtDtsFreeAttributeList(attr_list);
   }

   if (action_list == NULL)
   {
      if (control_data->element_values[CONTROL_PUSH_ACTION].string_value != NULL)
      {
         action_list = (char **) XtMalloc(sizeof(char *) * 2);

         action_list[0] = 
            control_data->element_values[CONTROL_PUSH_ACTION].string_value;
         action_list[1] = NULL;
         num_actions = 1;
      }
      else
         num_actions = 0;
   }
   
   control_data->actions = (PanelActionData **)
				  XtMalloc(sizeof(PanelActionData *) *
					   (num_actions + 1));

   for (i = 0, j = 0; j < num_actions; i++, j++)
   {
      /* Remove the OpenInPlace action from the list of actions */
       if (strcmp(action_list[j],"OpenInPlace") == 0)
       {
          i--;
          continue;
       }

       control_data->actions[i] = (PanelActionData *)
				       XtMalloc(sizeof(PanelActionData));

       control_data->actions[i]->action_name = strdup(action_list[j]);

       control_data->actions[i]->aap = NULL;
       control_data->actions[i]->count = 0;

       label = DtActionLabel (action_list[j]);

       if (label != NULL)
	  control_data->actions[i]->action_label = label;
       else
	  control_data->actions[i]->action_label = strdup (action_list[j]);


   }

   control_data->actions[i] = NULL;

   if (act_list)
       free (act_list);

   if (action_list)
       XtFree ((char *) action_list);

   if ((data_type != NULL) && is_file_control)
      DtDtsFreeDataType (data_type);
}




/************************************************************************
 *
 *  ProcessBox
 *      For a box structure, find the control set that is to be contained
 *      by it, allocate a ControlData array and reassign the element
 *	values pointers.  Further process each control.
 *
 *  Inputs: box_data - a pointer the BoxData structure to be initialized.
 *
 ************************************************************************/

static void
ProcessBox (BoxData * box_data)


{
   ElementValue * element_values;

   int box_control_count = 0;
   int i;


   /*  Loop the control array and check each control to see if it belongs  */
   /*  in this box.  If so, call a function to add it and then see if the  */
   /*  control has a subpanel to process.                                  */

   for (i = 0; i < control_count; i++)
   {
      element_values = control_data[i].element_values;

      if ((intptr_t) element_values[CONTROL_CONTAINER_TYPE].parsed_value == BOX &&
          strcmp ((char *) box_data->element_values[BOX_NAME].parsed_value,
	          (char *) element_values[CONTROL_CONTAINER_NAME].parsed_value) == 0)
      {
         ProcessControl ((XtPointer) box_data, BOX, &(box_data->control_data),
	                  &box_control_count, element_values);

         box_data->subpanel_count +=
	    ProcessBoxControl (box_data->control_data[box_control_count - 1]);
      }
   }

   box_data->control_data_count = box_control_count;
}




/************************************************************************
 *
 *  ProcessControl
 *	For each control that is created, reallocate the array of 
 *	pointer that hold the control and allocate a control structure.
 *	Initialize the control fields.  This is called for BOX, SUBPANEL
 *	and SWITCH controls.
 *
 *  Inputs: parent - a pointer to the head of the structure that is to
 *          contain this control.
 *          parent_type - the type of parent of the control.
 *          control_data_ptr - a pointer to the array of control pointers.
 *	    control_count - the current count of controls within the array.
 *	    element_values - the new controls database values.
 *
 ************************************************************************/
 
 
static void
ProcessControl (XtPointer       parent,
                char            parent_type,
                ControlData *** control_data_ptr,
                int           * control_count,
                ElementValue  * element_values)


{
   ControlData * control;

   *control_data_ptr = 
      (ControlData **) XtRealloc ((char *) *control_data_ptr,
                                 sizeof (ControlData *) * (*control_count + 1));
   (*control_data_ptr)[*control_count] = 
      (ControlData *) XtMalloc (sizeof (ControlData));

   control = (*control_data_ptr)[*control_count];

   control->element_values = element_values;
   control->parent_data = parent;
   control->parent_type = parent_type;
   control->subpanel_data = NULL;
   control->icon = NULL;
   control->arrow = NULL;
   control->arrow_separator = NULL;
   control->indicator = NULL;
   control->actions = NULL;
   control->is_action = False;
   control->move_action = NULL;
   control->copy_action = NULL;
   control->link_action = NULL;
   control->operation = 0;

   AddControlActionList (control);

   *control_count = *control_count + 1;
}




/************************************************************************
 *
 *  ProcessBoxControl
 *      For a control structure within a box, see if there is a subpanel
 *      attached to it and if so, process the subpanel and its controls.
 *      Return the subpanel count to be stored as part of the box data.
 *
 *  Inputs: control_data - a pointer to the control to be processed
 *
 ************************************************************************/

static int
ProcessBoxControl (ControlData * control_data)


{
   int i;
   ElementValue * element_values;
   int box_subpanel_count = 0;
   SubpanelData * subpanel;


   /*  Loop through the subpanel data and find one that is attached  */
   /*  to the provided control.                                      */

   for (i = 0; i < subpanel_count; i++)
   {
      element_values = subpanel_data[i].element_values;

      if (strcmp ((char *) control_data->element_values[CONTROL_NAME].parsed_value,
	          (char *) element_values[SUBPANEL_CONTAINER_NAME].parsed_value) == 0)
      {
         box_subpanel_count = 1;
	 
         control_data->subpanel_data = 
	    (SubpanelData *) XtMalloc (sizeof (SubpanelData));

         subpanel = control_data->subpanel_data;
         subpanel->element_values = element_values;
         subpanel->control_data = NULL;
         subpanel->control_data_count = 0;
         subpanel->parent_control_data = control_data;
	 subpanel->default_control = NULL;
	 subpanel->shell = NULL;
	 subpanel->form = NULL;
	 subpanel->dropzone = NULL;
	 subpanel->separator = NULL;
	 subpanel->main_panel_icon_copy = NULL;
	 subpanel->torn = False;

         ProcessSubpanel (subpanel);

      }
   }

   return (box_subpanel_count);
}




/************************************************************************
 *
 *  ProcessSubpanel
 *      For a Subpanel, find all of the controls within it an allocate
 *	a ControlData array and move the element values for the controls.
 *
 *  Inputs: subpanel_data - A pointer the the subpanel structure to be
 *          processed.
 *
 ************************************************************************/

static void
ProcessSubpanel (SubpanelData * subpanel_data)


{
   ElementValue * element_values;

   int subpanel_control_count = 0;
   int i;



   /*  Loop the control array and check each control to see if it belongs  */
   /*  in this subpanel.  If so, call a function to add it.                */

   for (i = 0; i < control_count; i++)
   {
      element_values = control_data[i].element_values;

      if ((intptr_t) element_values[CONTROL_CONTAINER_TYPE].parsed_value == SUBPANEL &&
          strcmp ((char *) subpanel_data->element_values[SUBPANEL_NAME].parsed_value,
	          (char *) element_values[CONTROL_CONTAINER_NAME].parsed_value) == 0)
      {
         ProcessControl ((XtPointer) subpanel_data, SUBPANEL,
	                  &(subpanel_data->control_data),
	                  &subpanel_control_count, element_values);
      }
   }

   subpanel_data->control_data_count = subpanel_control_count;
}




/************************************************************************
 *
 *  ProcessSwitch
 *      For a Switch, find all of the controls within it an allocate
 *	a ControlData array and move the element values for the controls.
 *
 *  Inputs: switch_data - a pointer to the switch data structure to be
 *          processed.
 *
 ************************************************************************/

static void
ProcessSwitch (SwitchData * switch_data)


{
   ElementValue * element_values;

   int switch_control_count = 0;
   int i;


   /*  Count the number of controls that are to go into this switch  */
   /*  and allocate a ControlData array to hold the controls.        */

   for (i = 0; i < control_count; i++)
   {
      element_values = control_data[i].element_values;

      if ((intptr_t) element_values[CONTROL_CONTAINER_TYPE].parsed_value == SWITCH &&
          strcmp ((char *) switch_data->element_values[SWITCH_NAME].parsed_value,
	          (char *) element_values[CONTROL_CONTAINER_NAME].parsed_value) == 0)
      {
         ProcessControl ((XtPointer) switch_data, SWITCH,
	                  &(switch_data->control_data),
	                  &switch_control_count, element_values);
      }
   }

   switch_data->control_data_count = switch_control_count;
}




/************************************************************************
 *
 *  CreateComponentFileName
 *	Create a file name in which to store a component file.  This is
 *	accomplished by using the components name with an integer value
 *      appended.
 *
 *  Inputs: record_data - a pointer to the struture that contains the
 *            components name, as well as other values.
 *
 ************************************************************************/
 

static char *
CreateComponentFileName (RecordData * record_data)


{
   char * file_name;
   char * component_name;
   struct stat stat_info;
   int i;


   component_name = XtMalloc(9);
   strncpy (component_name, record_data->element_values[0].string_value, 8);
   component_name[8] = '\0';

   file_name = XtMalloc (strlen (HOME_DIR) + strlen (TYPES_DIR) + 14);

   for (i = 1; i < 1000; i++)
   {
      sprintf(file_name, "%s%s%s%d.fp", HOME_DIR, TYPES_DIR, component_name, i);

      if (lstat (file_name, &stat_info) != 0)
         break;
   }

   XtFree(component_name);

   return (file_name);
}




/************************************************************************
 *
 *  WriteComponentToFile
 *	Write a component (contained within record_data) to a .fp file.
 *
 *  Inputs: record_data - a pointer to the data for the component,
 *            including its element values.
 *          record_type - the type of component (CONTROL, SUBPANEL, ...)
 *          keywords - the ordered array of keywords for the component
 *            type, used to reference into the element values array.
 *          container_name - the parent of the component.
 *          container_type - the type of parent of the component.
 *          delete - whether the component is be added or deleted.
 *
 ************************************************************************/
 

static void
WriteComponentToFile (RecordData * record_data,
                      int          record_type,
                      char      ** keywords,
                      char       * container_name,
                      int          container_type,
                      Boolean      delete)


{
   FILE * fd;
   char * file_name = CreateComponentFileName (record_data);
   int k;

   if ((fd = fopen(file_name, "w")) != NULL)
   {
      fprintf (fd, "%s	%s\n{\n", keywords[0],
               record_data->element_values[0].string_value);

      if (delete)
      {
         if (record_type == CONTROL)
	 {
	    fprintf(fd, "   TYPE      %s\n",
		   record_data->element_values[CONTROL_TYPE].string_value);
            fprintf(fd, "   CONTAINER_TYPE      %s\n",
	      record_data->element_values[CONTROL_CONTAINER_TYPE].string_value);
         }

         fprintf(fd, "   CONTAINER_NAME      %s\n", container_name);
         fprintf(fd, "   DELETE      True\n");
         
      }
      else
      {
         for (k = 1; k < record_descriptor[record_type].maxFields; k++)
         {
             if (record_data->element_values[k].string_value != NULL &&
		 record_data->element_values[k].use_default == False)
             {
                if (record_type == CONTROL &&
		    (intptr_t) record_data->element_values[CONTROL_TYPE].parsed_value == CONTROL_FILE &&
                    (k == CONTROL_PUSH_ACTION ||
                     k == CONTROL_LABEL || k == CONTROL_DROP_ACTION))
	           continue;

                fprintf (fd, "   %s	%s\n", keywords[k],
                         record_data->element_values[k].string_value);
             }
         }
      }

      fprintf(fd, "}\n");
   
      fflush(fd);
      fclose(fd);

      if (record_type == CONTROL)
         SessionAddFileData(file_name,
			 record_data->element_values[CONTROL_NAME].string_value,
   	                 (int) CONTROL,
			 container_name, container_type, delete);
      else
         SessionAddFileData(file_name,
			    record_data->element_values[0].string_value,
			    record_type, container_name, container_type,delete);
   }

   WmFrontPanelSessionSaveData ();

   XtFree (file_name);
}




/************************************************************************
 *
 *  RemoveComponentFile
 *	Find the dynamic .fp file name of a file in the session data and
 *	unlink the file to remove the component.  If no file name is
 *	found or the unlink fails, Call a function to write a .fp for
 *	the file which will have the DELETE keyword set to True.
 *
 ************************************************************************/
 

static void
RemoveComponentFile (RecordData * record_data,
                     int          record_type,
                     char      ** keywords,
                     char       * container_name,
                     int          container_type)


{
   char * file_name;

   file_name = 
      SessionFileNameLookup (record_data->element_values[0].string_value, 
                             record_type, container_name, container_type);

   if (file_name == NULL || unlink (file_name) < 0)
      WriteComponentToFile (record_data, record_type, keywords,
		            container_name, container_type, True);
   else
   {
      SessionDeleteFileData (file_name);
      WmFrontPanelSessionSaveData ();
   }
}




/************************************************************************
 *
 *  WriteControlComponentFile
 *	Set up the parameters to call a function which will write out
 *	to a .fp file a control description.
 *
 *	Inputs: control_data - a pointer to the control to be written.
 *
 ************************************************************************/
 

void
WriteControlComponentFile (ControlData * control_data)


{
   ElementValue * element_values;

   element_values = control_data->element_values;
   
   WriteComponentToFile ((RecordData *) control_data, CONTROL, control_keywords,
                         (char *) element_values[CONTROL_CONTAINER_NAME].parsed_value,
                         (int) (intptr_t) element_values[CONTROL_CONTAINER_TYPE].parsed_value,
			 False);
}




/************************************************************************
 *
 *  WriteSubpanelComponentFile
 *	Set up the parameters to call a function which will write out
 *	to a .fp file a subpanel description.
 *
 *	Inputs: subpanel_data - a pointer to the subpanel to be written.
 *
 ************************************************************************/
 
void
WriteSubpanelComponentFile (SubpanelData * subpanel_data)


{
   ElementValue * element_values = subpanel_data->element_values;

   WriteComponentToFile ((RecordData *) subpanel_data, SUBPANEL, subpanel_keywords,
                         (char *) element_values[SUBPANEL_CONTAINER_NAME].parsed_value,
                         CONTROL, False);
}




/************************************************************************
 *
 *  RemoveControlComponentFile
 *	Set up the parameters to call a function which delete a control's
 *	.fp file.
 *
 *	Inputs: control_data - a pointer to the control to be deleted.
 *
 ************************************************************************/
 

void
RemoveControlComponentFile (ControlData * control_data)


{
   ElementValue * element_values = control_data->element_values;

   RemoveComponentFile ((RecordData *) control_data, CONTROL, control_keywords,
                        (char *) element_values[CONTROL_CONTAINER_NAME].parsed_value,
                        (int) (intptr_t) element_values[CONTROL_CONTAINER_TYPE].parsed_value);
}




/************************************************************************
 *
 *  RemoveSubpanelComponentFile
 *	Set up the parameters to call a function which delete a subpanel's
 *	.fp file.
 *
 *	Inputs: subpanel_data - a pointer to the subpanel to be deleted.
 *
 ************************************************************************/
 
void
RemoveSubpanelComponentFile (SubpanelData * subpanel_data)


{
   ElementValue * element_values = subpanel_data->element_values;

   RemoveComponentFile ((RecordData *) subpanel_data, SUBPANEL, subpanel_keywords,
                        (char *) element_values[SUBPANEL_CONTAINER_NAME].parsed_value,
                        CONTROL);
}




/************************************************************************
 *
 *  _WriteControlElementValues
 *	Set up a loop which write out all of the values that define
 *	a control.  These are written as normal keyword, value pairs.
 *
 *  Inputs: element_values - a pointer to the array of element value
 *          strutures each of which contain a value for a keyword.
 *
 ************************************************************************/
 

void
_WriteControlElementValues (ElementValue * element_values)


{
   int k; 

   printf("%s	%s\n{\n", control_keywords[0], element_values[0].string_value);

   for (k = 1; k < record_descriptor[CONTROL].maxFields; k++)
   {
       if (element_values[k].string_value != NULL)
          printf("   %s	%s\n", 
	         control_keywords[k], element_values[k].string_value);
   }

   printf("}\n");
}





/************************************************************************
 *
 *  InitParse
 *
 ************************************************************************/
 
void
InitParse (char          * file_name,
	   ElementValue ** elem_vals)


{
   char * tmpPath;
   char * tmpDir;
   char * hostName;
   char * tmpName;
   char * baseName;
   char * tmpFile;
   DtDirPaths * dirPath;


   /* create directory for dir path */

   tmpPath = XtMalloc (strlen(HOME_DIR) + strlen(TYPES_DIR) + 34);
   sprintf (tmpPath, "%s%s%s%d", HOME_DIR, TYPES_DIR, "fp", (int) getpid());
   mkdir (tmpPath, S_IRUSR | S_IWUSR | S_IXUSR);


   /* create symbolic link to file_name */

   tmpName = XtNewString(file_name);
   baseName = strrchr(tmpName, '/');
   tmpFile = XtMalloc(strlen(tmpPath) + strlen(baseName) + 1);
   sprintf(tmpFile, "%s%s", tmpPath, baseName);
   symlink(file_name, tmpFile);

   hostName = XtMalloc((Cardinal)(MAXHOSTNAMELEN + 1));
   DtGetShortHostname (hostName, MAXHOSTNAMELEN + 1);

   tmpDir = XtMalloc(strlen(hostName) + strlen(tmpPath) + 2);
   sprintf(tmpDir, "%s:%s", hostName, tmpPath);

   dirPath = (DtDirPaths *) XtMalloc(sizeof(DtDirPaths));
   dirPath->dirs = (char **) XtMalloc(sizeof(char *) * 2);
   dirPath->paths = (char **) XtMalloc(sizeof(char *) * 2);

   dirPath->dirs[0] = tmpDir;
   dirPath->dirs[1] = NULL;
   dirPath->paths[0] = tmpPath;
   dirPath->paths[1] = NULL;

   _DtDbRead (dirPath, FILE_TYPE_SUFFIX, control_record_descriptor, 2);

   control_element_value_found = False;

   /* remove link and directory */

   unlink(tmpFile);
   rmdir(tmpPath);

   XtFree((char *) dirPath->dirs);
   XtFree((char *) dirPath->paths);
   XtFree((char *) dirPath);
   XtFree(hostName);
   XtFree(tmpName);
   XtFree(tmpDir);
   XtFree(tmpPath);
   XtFree(tmpFile);

   *elem_vals = control_element_values;
}




/************************************************************************
 *
 *  FreeFileControlField
 *
 ************************************************************************/


static void
FreeFileControlField (ElementValue * element_values, 
                      int            indx)


{
   if (control_parse_functions[indx].free_function != NULL &&
       element_values[indx].parsed_value != NULL &&
       element_values[indx].use_default == False)
   {
      (*(control_parse_functions[indx].free_function))
         (&(element_values[indx].parsed_value));
      element_values[indx].parsed_value = NULL;
   }

   if (element_values[indx].use_default == False &&
       element_values[indx].string_value != NULL)
   {
      free(element_values[indx].string_value);
      element_values[indx].string_value = NULL;
   }
}
		    



/************************************************************************
 *
 *  InitializePrimaryControlFields
 *
 ************************************************************************/
 

static void
InitializePrimaryControlFields (ElementValue * element_values)


{
   InitializeField (element_values,
                    CONTROL_NAME, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_CONTAINER_NAME, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_CONTAINER_TYPE, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_LOCKED, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_DELETE, control_parse_functions);

}




/************************************************************************
 *
 *  InitializeSecondaryControlFields
 *
 ************************************************************************/


static void
InitializeSecondaryControlFields (ElementValue * element_values)


{
   InitializeField (element_values,
                    CONTROL_TYPE, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_POSITION_HINTS, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_ALTERNATE_ICON, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_PUSH_ANIMATION, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_DROP_ANIMATION, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_PUSH_RECALL, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_MONITOR_TYPE, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_CLIENT_NAME, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_CLIENT_GEOMETRY, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_FILE_NAME, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_DATE_FORMAT, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_HELP_STRING, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_HELP_VOLUME, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_HELP_TOPIC, control_parse_functions);

}




/************************************************************************
 *
 *  InitializeFileControlFields
 *
 ************************************************************************/
 

static void
InitializeFileControlFields (ElementValue * element_values,
                             char         * data_type)


{
   if ((intptr_t)element_values[CONTROL_TYPE].parsed_value == CONTROL_FILE)
   {
      Boolean free_data_type = False;
      char * act_list;
      char ** action_list = NULL;
      char * icon_name;
      char * file_name;
      char * description;
      char * file_str;
      char * label = NULL;
      int i, j, result;

      struct stat stat_info;
      Boolean valid_file = True;
   

      file_name = (char *) element_values[CONTROL_FILE_NAME].parsed_value;

      if (lstat (file_name, &stat_info) != 0)
         valid_file = False;

      if (data_type == NULL && file_name != NULL)
      {
         data_type = DtDtsFileToDataType (file_name);
         free_data_type = True;
      }

      if (data_type != NULL)
      {
         if (valid_file)
         {
            icon_name = DtDtsDataTypeToAttributeValue (data_type,
                                                       DtDTS_DA_ICON, NULL);

            if (element_values[CONTROL_NORMAL_ICON].string_value != NULL)
            {
               XtFree (element_values[CONTROL_NORMAL_ICON].string_value);
	       element_values[CONTROL_NORMAL_ICON].string_value = NULL;
            }

            element_values[CONTROL_NORMAL_ICON].string_value =
	                                               XtNewString (icon_name);

            DtDtsFreeAttributeValue(icon_name);
         }

         description = 
           DtDtsDataTypeToAttributeValue(data_type, DtDTS_DA_DESCRIPTION, NULL);

         if (element_values[CONTROL_HELP_STRING].string_value != NULL)
         {
            XtFree (element_values[CONTROL_HELP_STRING].string_value);
            element_values[CONTROL_HELP_STRING].string_value = NULL;
         }

         element_values[CONTROL_HELP_STRING].string_value = XtNewString (description);
         DtDtsFreeAttributeValue (description);


         act_list = DtDtsDataTypeToAttributeValue (data_type,
						   DtDTS_DA_ACTION_LIST,
						   NULL);
         if (act_list)
            action_list = _DtVectorizeInPlace (act_list, ',');

	 if (action_list && action_list[0] != NULL)
	 {
	    element_values[CONTROL_PUSH_ACTION].string_value =
					      strdup (action_list[0]);
	    element_values[CONTROL_DROP_ACTION].string_value =
					      strdup (action_list[0]);

            if (DtDtsDataTypeIsAction (data_type))
               label = DtActionLabel (action_list[0]);
	 }

	 /* try to set it to the name of the type */
	 if (label == NULL) {
	     label = DtDtsDataTypeToAttributeValue (data_type,
						    DtDTS_DA_LABEL, NULL);

	     /* copy & correctly free the memory */
	     if (label) {
		 char *t = label;

		 label = XtNewString(t);

		 DtDtsFreeAttributeValue(t);
	     }
	 }
	 

         if (label == NULL && file_name != NULL)
         {
            if ((file_str = (char *)strrchr(file_name, '/')) == NULL)
                file_str = file_name;
            else
                file_str++;
            label = XtNewString(file_str);
         }

         if (label != NULL)
	    element_values[CONTROL_LABEL].string_value = label;


	 if (data_type != NULL)
	 {
	    if (act_list)
	    {
               DtDtsFreeAttributeValue (act_list);
               XtFree ((char *) action_list);
	    }

	    if (free_data_type)
               DtDtsFreeDataType (data_type);
	 }
      }
   } 

   InitializeField (element_values,
                    CONTROL_NORMAL_ICON, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_HELP_STRING, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_LABEL, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_PUSH_ACTION, control_parse_functions);
   InitializeField (element_values,
                    CONTROL_DROP_ACTION, control_parse_functions);
}




/************************************************************************
 *
 *  InitializeControlFields
 *
 ************************************************************************/


void
InitializeControlFields (ElementValue * element_values,
                         char         * data_type)


{
   InitializePrimaryControlFields (element_values);
   InitializeSecondaryControlFields (element_values);
   InitializeFileControlFields (element_values, data_type);
}




/************************************************************************
 *
 *  UpdateFileType
 *
 ************************************************************************/
 

static void
UpdateFileType (ControlData * control_data)


{
   char * icon_name;
   char * control_label;
   XmString icon_label;
   Arg al[2];

   FreeFileControlField (control_data->element_values, CONTROL_NORMAL_ICON);
   FreeFileControlField (control_data->element_values, CONTROL_PUSH_ACTION);
   FreeFileControlField (control_data->element_values, CONTROL_DROP_ACTION);
   FreeFileControlField (control_data->element_values, CONTROL_LABEL);
   InitializeFileControlFields (control_data->element_values, NULL);

   DeleteControlActionList (control_data);
   AddControlActionList (control_data);

   icon_name =
	 (char *)control_data->element_values[CONTROL_NORMAL_ICON].parsed_value;

   if ((intptr_t)control_data->element_values[CONTROL_CONTAINER_TYPE].parsed_value ==
							               SUBPANEL)
   {
      icon_name = GetIconName (icon_name, panel.sub_icon_size);
   }
   else
   {
      icon_name = GetIconName (icon_name, panel.main_icon_size);

      if (icon_name == NULL)
         icon_name = GetIconName (icon_name, panel.sub_icon_size);
   }

   control_label = (char *)
                   control_data->element_values[CONTROL_LABEL].parsed_value;

   icon_label = XmStringCreateLocalized (control_label);

   XtSetArg(al[0], XmNimageName, icon_name);
   XtSetArg(al[1], XmNstring, icon_label);
   XtSetValues(control_data->icon, al, 2);
}




/************************************************************************
 *
 *  UpdateFileTypeControlFields
 *
 ************************************************************************/
 

void
UpdateFileTypeControlFields (void)


{
   BoxData * box_data;
   ControlData * control_data;
   SubpanelData * subpanel_data;
   SwitchData * switch_data;
   int i, j, k;

   if (panel_count == 0) return;

   for (i = 0; i < panel.box_data_count; i++)
   {
      box_data = panel.box_data[i];

      for (j = 0; j < box_data->control_data_count; j++)
      {
         control_data = box_data->control_data[j];

         if ((intptr_t)control_data->element_values[CONTROL_TYPE].parsed_value ==
								 CONTROL_FILE)
            UpdateFileType(control_data);

         if (control_data->subpanel_data != NULL)
         {
            subpanel_data = control_data->subpanel_data;

            for (k = 0; k < subpanel_data->control_data_count; k++)
            {
               control_data = subpanel_data->control_data[k];

               if ((intptr_t)control_data->element_values[CONTROL_TYPE].parsed_value
							        == CONTROL_FILE)
                  UpdateFileType(control_data);
            }
         }
      }

      if (box_data->switch_data != NULL)
      {
         switch_data = box_data->switch_data;
         for (j = 0; j < switch_data->control_data_count; j++)
         {
            control_data = switch_data->control_data[j];

            if ((intptr_t)control_data->element_values[CONTROL_TYPE].parsed_value ==
	  						           CONTROL_FILE)
               UpdateFileType(control_data);
         }
      }
   }
}




/************************************************************************
 *
 *  InitializeSubpanelFields
 *
 ************************************************************************/
 

void
InitializeSubpanelFields (ElementValue * element_values)


{
   InitializeField (element_values,
                    SUBPANEL_NAME, subpanel_parse_functions);
   InitializeField (element_values,
                    SUBPANEL_CONTAINER_NAME, subpanel_parse_functions);
   InitializeField (element_values,
                    SUBPANEL_LOCKED, subpanel_parse_functions);
   InitializeField (element_values,
                    SUBPANEL_CONTROL_INSTALL, subpanel_parse_functions);
   InitializeField (element_values,
                    SUBPANEL_TITLE, subpanel_parse_functions);
   InitializeField (element_values,
                    SUBPANEL_DELETE, subpanel_parse_functions);
   InitializeField (element_values,
                    SUBPANEL_HELP_STRING, subpanel_parse_functions);
   InitializeField (element_values,
                    SUBPANEL_HELP_TOPIC, subpanel_parse_functions);
   InitializeField (element_values,
                    SUBPANEL_HELP_VOLUME, subpanel_parse_functions);
}


