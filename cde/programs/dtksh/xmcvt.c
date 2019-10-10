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
/* $TOG: xmcvt.c /main/8 1999/09/16 13:42:47 mgreess $ */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */


#include	"shell.h" 
#include <signal.h>
#include <fcntl.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/StringDefs.h>
#include <Xm/XmStrDefs.h>
#include <Xm/List.h>
#include <Xm/MwmUtil.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Dt/Service.h>
#include <Dt/Wsm.h>
#include <Dt/HourGlass.h>
#include <Dt/Help.h>
#include <Dt/EnvControlP.h>
#include <Tt/tttk.h>
#include "hash.h"
#include "stdio.h"
#define NO_AST
#include "dtksh.h"
#undef NO_AST
#include "xmksh.h"
#include "XtCvtrs.h"
#include "dtkcmds.h"
#include "xmcvt.h"
#include "widget.h"
#include "extra.h"
#include "xmwidgets.h"
#include "msgs.h"
#include <Dt/Help.h>


typedef struct {
   char * modifierName;
   unsigned int modifier;
} ModifierState;


/*
 * Converters for DTKSH
 */

char *
XmStringToString(
        XmString string )
{
	static char *buf = NULL;
	XmStringContext context;
	XmStringCharSet charset;
	XmStringDirection dir;
	char *text;
	Boolean separator = FALSE;

        if (string == NULL)
           return(NULL);

	XmStringInitContext(&context, string);
        XtFree(buf);
	buf = NULL;
	while (!separator) {
		if (XmStringGetNextSegment(context, &text, &charset, &dir,
			&separator)) {
			if (buf) {
				buf = XtRealloc(buf, strlen(buf) + strlen(text) + 2);
				strcat(buf, text);
			} else
				buf = strdup(text);
			XtFree(text);
		} else
			break;
	}
	XmStringFreeContext(context);
	return(buf);
}

void
DtkshCvtXmStringToString(
        Display *dpy,
        XrmValuePtr args,
        Cardinal *nargs,
        XrmValuePtr fval,
        XrmValuePtr toval,
        XtPointer data )
{
	XmString string = ((XmString *)fval->addr)[0];
	char *buf;

	buf = XmStringToString(string);
	toval->addr = (caddr_t)buf;
	toval->size = buf ? strlen(buf) + 1 : 0;
}

void
DtkshCvtKeySymToString(
        XrmValuePtr args,
        Cardinal *nargs,
        XrmValuePtr fval,
        XrmValuePtr toval )
{
	static char buf[2];

	buf[0] = (char) (((KeySym *)(fval->addr))[0]);
	buf[1] = '\0';
	toval->addr = (caddr_t)buf;
	toval->size = 2;
}

/*
 * Convert an XmStringTable to a String.
 * In keeping with the standard CvtStringToStringTable function provided
 * by Motif, we will separate each item by a comma.  This of course does not
 * work properly if there is a comma in the data, but what can we do?
 */

char *
_CvtXmStringTableToString(
        XmStringTable stringtable,
        int itemCount )
{
   static char *buf = NULL;
   XmStringContext context;
   XmStringCharSet charset;
   XmStringDirection dir;
   char *text;
   Boolean separator = FALSE;
   XmString string;
   int i;
   char * ptr;
   char * nextComma;

   XtFree(buf);
   buf = XtMalloc(3);
   buf[0] = '\0';

   for (i = 0; i < itemCount; i++)
   {
      if (i > 0)
         strcat(buf, ",");

      XmStringInitContext(&context, stringtable[i]);
      while (!separator) 
      {
         if (XmStringGetNextSegment(context, &text, &charset, &dir, &separator))
         {
            /*
             * To be consistent with the Motif converter, which will take
             * a comma-separated string, and convert it to an XmStringTable,
             * we need to escape any ',' characters contained within a list
             * item.
             */
 
            ptr = text;
            while (nextComma = strchr(ptr, ','))
            {
               *nextComma = '\0';
               buf = XtRealloc(buf, strlen(buf) + strlen(ptr) + 5);
               strcat(buf, ptr);
               strcat(buf, "\\,");
               *nextComma = ',';
               ptr = nextComma + 1;
            }
            buf = XtRealloc(buf, strlen(buf) + strlen(ptr) + 3);
            strcat(buf, ptr);

            XtFree(text);
         } 
         else
            break;
      }
      XmStringFreeContext(context);
   }

   return(buf);
}

void
DtkshCvtListItemsToString(
        Display *dpy,
        XrmValuePtr args,
        Cardinal *nargs,
        XrmValuePtr fval,
        XrmValuePtr toval,
        XtPointer data )
{
   char *buf;
   XmStringTable stringtable = *((XmStringTable *)fval->addr);
   int itemCount = *((int *) args[0].addr);

   buf = _CvtXmStringTableToString(stringtable, itemCount);

   toval->addr = (caddr_t)buf;
   toval->size = buf ? strlen(buf) + 1 : 0;
}


/*
 * There are a number of resources in motif that consist of a few
 * named integer values.  Most such resources only have 2 to 4 values,
 * none have more than 7.  Because there are so few values, it's not
 * really worth the memory overhead to hash them.  Also, these kinds
 * of resources are rarely read by programmers (most are written but
 * not read).  So, we decided to go with a simple linear search converter
 * that takes as its first argument a table of the values allowed, and
 * as its second argument the number of items in the table.
 *
 * Note that we could not go with a simple indexing scheme because:
 * (1) the values are not guaranteed to be contiguous and (2) some
 * of the tables start with -1 instead of 0.
 *
 * If there are in the future many more items added to these lists, we
 * might want to convert to a hashing scheme or a binary search.
 */

void
DtkshCvtNamedValueToString(
        XrmValue *args,
        Cardinal *nargs,
        XrmValuePtr fval,
        XrmValuePtr toval )
{
	/*
	 * same buffer will get used each time
	 */
	static char *ret = NULL;
	struct named_integer *table;
	int numtable;
	long value;
	int i;
        char * errmsg;

        switch(fval->size)
        {
           case sizeof(char):
           {
	      value = (long)*((char *)fval->addr);
              break;
           }

           case sizeof(short):
           {
	      value = (long)*((short *)fval->addr);
              break;
           }

           case sizeof(long):
           {
	      value = (long)*((long *)fval->addr);
              break;
           }

	   default:
	      if (fval->size == sizeof(int))
	      {
		 value = (long)*((int *)fval->addr);
		 break;
	      }
	      toval->addr = NULL;
	      toval->size = 0;
	      return;
        }

	if (*nargs != 1) 
        {
	   toval->addr = NULL;
	   toval->size = 0;
	   return;
	}
	table = (struct named_integer *)args[0].addr;
	numtable = args[0].size/sizeof(struct named_integer);

	for (i = 0; i < numtable; i++) {
		if (value == table[i].value) {
			toval->addr = (caddr_t)table[i].name;
			toval->size = strlen(table[i].name) + 1;
			return;
		}
	}
	toval->addr = NULL;
	toval->size = 0;
	return;
}

/*************************************************************************/

void
DtkshCvtStringToNamedValue(
        XrmValue *args,
        Cardinal *nargs,
        XrmValuePtr fval,
        XrmValuePtr toval )
{
	/*
	 * same buffer will get used each time
	 */
	static int ret;
	struct named_integer *table;
	int numtable;
	char *value;
	int i;
        char * errbuf;
        char * errmsg;

	value = (String)fval->addr;

	if (*nargs != 1) {
		toval->addr = NULL;
		toval->size = 0;
		return;
	}
	table = (struct named_integer *)args[0].addr;
	numtable = args[0].size/sizeof(struct named_integer);

	for (i = 0; i < numtable; i++) {
		if (DtCompareISOLatin1(value, (char *)(table[i].name))) {
			toval->addr = (caddr_t)&table[i].value;
			toval->size = sizeof(table[i].value);
			return;
		}
	}
	errmsg =strdup(GETMESSAGE(16,2, 
             "DtkshCvtStringToNamedValue: Unable to convert the string '%s'"));
        errbuf = XtMalloc(strlen(errmsg) + strlen(value) + 10);
        sprintf(errbuf, errmsg, value);
	XtWarning(errbuf);
        free(errmsg);
        XtFree(errbuf);
	toval->addr = NULL;
	toval->size = 0;
	return;
}

struct named_integer NI_TraversalDirection[] = {
	{ "TRAVERSE_CURRENT",	XmTRAVERSE_CURRENT },
	{ "TRAVERSE_DOWN",	XmTRAVERSE_DOWN },
	{ "TRAVERSE_HOME",	XmTRAVERSE_HOME },
	{ "TRAVERSE_LEFT",	XmTRAVERSE_LEFT },
	{ "TRAVERSE_NEXT",	XmTRAVERSE_NEXT },
	{ "TRAVERSE_NEXT_TAB_GROUP",	XmTRAVERSE_NEXT_TAB_GROUP },
	{ "TRAVERSE_PREV",	XmTRAVERSE_PREV },
	{ "TRAVERSE_PREV_TAB_GROUP",	XmTRAVERSE_PREV_TAB_GROUP },
	{ "TRAVERSE_PREV_TAB_GROUP",	XmTRAVERSE_PREV_TAB_GROUP },
	{ "TRAVERSE_RIGHT",	XmTRAVERSE_RIGHT },
	{ "TRAVERSE_UP",	XmTRAVERSE_UP },
};

struct named_integer NI_VisibilityType[] = {
	{ "VISIBILITY_UNOBSCURED",	XmVISIBILITY_UNOBSCURED },
	{ "VISIBILITY_PARTIALLY_OBSCURED",XmVISIBILITY_PARTIALLY_OBSCURED },
	{ "VISIBILITY_FULLY_OBSCURED",	XmVISIBILITY_FULLY_OBSCURED },
};

struct named_integer NI_CommandChild[] = {
	{ "DIALOG_COMMAND_TEXT",	XmDIALOG_COMMAND_TEXT },
	{ "DIALOG_PROMPT_LABEL",	XmDIALOG_PROMPT_LABEL },
	{ "DIALOG_HISTORY_LIST",	XmDIALOG_HISTORY_LIST },
	{ "DIALOG_WORK_AREA",		XmDIALOG_WORK_AREA },
};

struct named_integer NI_MessageBoxChild[] = {
	{ "DIALOG_CANCEL_BUTTON",	XmDIALOG_CANCEL_BUTTON },
	{ "DIALOG_DEFAULT_BUTTON",	XmDIALOG_DEFAULT_BUTTON },
	{ "DIALOG_HELP_BUTTON",		XmDIALOG_HELP_BUTTON },
	{ "DIALOG_MESSAGE_LABEL",	XmDIALOG_MESSAGE_LABEL },
	{ "DIALOG_OK_BUTTON",		XmDIALOG_OK_BUTTON },
	{ "DIALOG_SEPARATOR",		XmDIALOG_SEPARATOR },
	{ "DIALOG_SYMBOL_LABEL",	XmDIALOG_SYMBOL_LABEL },
};

struct named_integer NI_FileSelChild[] = {
	{ "DIALOG_APPLY_BUTTON",	XmDIALOG_APPLY_BUTTON },
	{ "DIALOG_CANCEL_BUTTON",	XmDIALOG_CANCEL_BUTTON },
	{ "DIALOG_DEFAULT_BUTTON",	XmDIALOG_DEFAULT_BUTTON },
	{ "DIALOG_DIR_LIST",		XmDIALOG_DIR_LIST },
	{ "DIALOG_DIR_LIST_LABEL",	XmDIALOG_DIR_LIST_LABEL },
	{ "DIALOG_FILTER_LABEL",	XmDIALOG_FILTER_LABEL },
	{ "DIALOG_FILTER_TEXT",		XmDIALOG_FILTER_TEXT },
	{ "DIALOG_HELP_BUTTON",		XmDIALOG_HELP_BUTTON },
	{ "DIALOG_LIST",		XmDIALOG_LIST },
	{ "DIALOG_LIST_LABEL",		XmDIALOG_LIST_LABEL },
	{ "DIALOG_OK_BUTTON",		XmDIALOG_OK_BUTTON },
	{ "DIALOG_SELECTION_LABEL",	XmDIALOG_SELECTION_LABEL },
	{ "DIALOG_SEPARATOR",		XmDIALOG_SEPARATOR },
	{ "DIALOG_TEXT",		XmDIALOG_TEXT },
	{ "DIALOG_WORK_AREA",		XmDIALOG_WORK_AREA },
};

struct named_integer NI_SelBoxChild[] = {
	{ "DIALOG_APPLY_BUTTON",	XmDIALOG_APPLY_BUTTON },
	{ "DIALOG_CANCEL_BUTTON",	XmDIALOG_CANCEL_BUTTON },
	{ "DIALOG_DEFAULT_BUTTON",	XmDIALOG_DEFAULT_BUTTON },
	{ "DIALOG_HELP_BUTTON",		XmDIALOG_HELP_BUTTON },
	{ "DIALOG_LIST",		XmDIALOG_LIST },
	{ "DIALOG_LIST_LABEL",		XmDIALOG_LIST_LABEL },
	{ "DIALOG_OK_BUTTON",		XmDIALOG_OK_BUTTON },
	{ "DIALOG_SELECTION_LABEL",	XmDIALOG_SELECTION_LABEL },
	{ "DIALOG_SEPARATOR",		XmDIALOG_SEPARATOR },
	{ "DIALOG_TEXT",		XmDIALOG_TEXT },
	{ "DIALOG_WORK_AREA",		XmDIALOG_WORK_AREA },
};

/************************************************************************/

struct named_integer NI_ArrowDirection[] = {
	{ "ARROW_UP",	XmARROW_UP },
	{ "ARROW_DOWN",	XmARROW_DOWN },
	{ "ARROW_LEFT",	XmARROW_LEFT },
	{ "ARROW_RIGHT",XmARROW_RIGHT },
};

struct named_integer NI_MultiClick[] = {
	{ "MULTICLICK_DISCARD",	XmMULTICLICK_DISCARD },
	{ "MULTICLICK_KEEP",	XmMULTICLICK_KEEP },
};

struct named_integer NI_DialogStyle[] = {
	{ "DIALOG_SYSTEM_MODAL",	XmDIALOG_SYSTEM_MODAL },
	{"DIALOG_PRIMARY_APPLICATION_MODAL",XmDIALOG_PRIMARY_APPLICATION_MODAL},
	{ "DIALOG_APPLICATION_MODAL",	XmDIALOG_APPLICATION_MODAL },
	{ "DIALOG_FULL_APPLICATION_MODAL", XmDIALOG_FULL_APPLICATION_MODAL },
	{ "DIALOG_MODELESS",		XmDIALOG_MODELESS },
	{ "DIALOG_WORK_AREA",		XmDIALOG_WORK_AREA },
};

struct named_integer NI_MWMInputMode[] = {
	{ "-1",	-1 },
	{ "MWM_INPUT_MODELESS", MWM_INPUT_MODELESS},
	{ "MWM_INPUT_PRIMARY_APPLICATION_MODAL", 
               MWM_INPUT_PRIMARY_APPLICATION_MODAL },
	{ "MWM_INPUT_SYSTEM_MODAL", MWM_INPUT_SYSTEM_MODAL },
	{ "MWM_INPUT_FULL_APPLICATION_MODAL",
               MWM_INPUT_FULL_APPLICATION_MODAL },
};

struct named_integer NI_ResizePolicy[] = {
	{ "RESIZE_NONE",	XmRESIZE_NONE },
	{ "RESIZE_ANY",		XmRESIZE_ANY },
	{ "RESIZE_GROW",	XmRESIZE_GROW },
};

struct named_integer NI_FileTypeMask[] = {
	{ "FILE_REGULAR",	XmFILE_REGULAR },
	{ "FILE_DIRECTORY",	XmFILE_DIRECTORY },
	{ "FILE_ANY_TYPE",	XmFILE_ANY_TYPE },
};

struct named_integer NI_ShadowType[] = {
	{ "SHADOW_IN",		XmSHADOW_IN },
	{ "SHADOW_OUT",		XmSHADOW_OUT },
	{ "SHADOW_ETCHED_IN",	XmSHADOW_ETCHED_IN },
	{ "SHADOW_ETCHED_OUT",	XmSHADOW_ETCHED_OUT },
};

struct named_integer NI_Attachment[] = {
	{ "ATTACH_NONE",	XmATTACH_NONE },
	{ "ATTACH_FORM",	XmATTACH_FORM },
	{ "ATTACH_OPPOSITE_FORM",	XmATTACH_OPPOSITE_FORM },
	{ "ATTACH_WIDGET",	XmATTACH_WIDGET },
	{ "ATTACH_OPPOSITE_WIDGET",	XmATTACH_OPPOSITE_WIDGET },
	{ "ATTACH_POSITION",	XmATTACH_POSITION },
	{ "ATTACH_SELF",	XmATTACH_SELF },
};

struct named_integer NI_ChildType[] = {
	{ "FRAME_TITLE_CHILD",	XmFRAME_TITLE_CHILD },
	{ "FRAME_WORKAREA_CHILD",XmFRAME_WORKAREA_CHILD },
	{ "FRAME_GENERIC_CHILD",XmFRAME_GENERIC_CHILD },
};

struct named_integer NI_ChildVerticalAlignment[] = {
	{ "ALIGNMENT_BASELINE_BOTTOM",	XmALIGNMENT_BASELINE_BOTTOM },
	{ "ALIGNMENT_BASELINE_TOP",XmALIGNMENT_BASELINE_TOP },
	{ "ALIGNMENT_WIDGET_TOP",XmALIGNMENT_WIDGET_TOP },
	{ "ALIGNMENT_CENTER",XmALIGNMENT_CENTER },
	{ "ALIGNMENT_WIDGET_BOTTOM",XmALIGNMENT_WIDGET_BOTTOM },
};

struct named_integer NI_UnitType[] = {
	{ "PIXELS",	XmPIXELS },
	{ "100TH_MILLIMETERS",	Xm100TH_MILLIMETERS },
	{ "1000TH_INCHES",	Xm1000TH_INCHES },
	{ "100TH_POINTS",	Xm100TH_POINTS },
	{ "100TH_FONT_UNITS",	Xm100TH_FONT_UNITS },
};

struct named_integer NI_NavigationType[] = {
	{ "NONE",	XmNONE },
	{ "TAB_GROUP",	XmTAB_GROUP },
	{ "STICKY_TAB_GROUP",	XmSTICKY_TAB_GROUP },
	{ "EXCLUSIVE_TAB_GROUP",	XmEXCLUSIVE_TAB_GROUP },
};

struct named_integer NI_Alignment[] = {
	{ "ALIGNMENT_CENTER",	XmALIGNMENT_CENTER },
	{ "ALIGNMENT_END",	XmALIGNMENT_END },
	{ "ALIGNMENT_BEGINNING",	XmALIGNMENT_BEGINNING },
};

struct named_integer NI_LabelType[] = {
	{ "STRING",	XmSTRING },
	{ "PIXMAP",	XmPIXMAP }
};

struct named_integer NI_StringDirection[] = {
	{ "STRING_DIRECTION_L_TO_R",	XmSTRING_DIRECTION_L_TO_R },
	{ "STRING_DIRECTION_R_TO_L",	XmSTRING_DIRECTION_R_TO_L },
};

struct named_integer NI_SelectionPolicy[] = {
	{ "SINGLE_SELECT",	XmSINGLE_SELECT },
	{ "MULTIPLE_SELECT",	XmMULTIPLE_SELECT },
	{ "EXTENDED_SELECT",	XmEXTENDED_SELECT },
	{ "BROWSE_SELECT",	XmBROWSE_SELECT },
};

struct named_integer NI_ScrollBarPlacement[] = {
	{ "TOP_LEFT",	XmTOP_LEFT },
	{ "BOTTOM_LEFT",	XmBOTTOM_LEFT },
	{ "TOP_RIGHT",	XmTOP_RIGHT },
	{ "BOTTOM_RIGHT",	XmBOTTOM_RIGHT },
};

struct named_integer NI_ScrollBarDisplayPolicy[] = {
	{ "AS_NEEDED",	XmAS_NEEDED },
	{ "STATIC",	XmSTATIC },
};

struct named_integer NI_ScrollingPolicy[] = {
	{ "AUTOMATIC",	XmAUTOMATIC },
	{ "APPLICATION_DEFINED",	XmAPPLICATION_DEFINED },
};

struct named_integer NI_VisualPolicy[] = {
	{ "VARIABLE",	XmVARIABLE },
	{ "CONSTANT",	XmCONSTANT },
};

struct named_integer NI_ListSizePolicy[] = {
	{ "CONSTANT",	XmCONSTANT },
	{ "VARIABLE",	XmVARIABLE },
	{ "RESIZE_IF_POSSIBLE",	XmRESIZE_IF_POSSIBLE },
};

struct named_integer NI_EditMode[] = {
	{ "SINGLE_LINE_EDIT",	XmSINGLE_LINE_EDIT },
	{ "MULTI_LINE_EDIT",	XmMULTI_LINE_EDIT },
};

struct named_integer NI_CommandWindowLocation[] = {
	{ "COMMAND_ABOVE_WORKSPACE",	XmCOMMAND_ABOVE_WORKSPACE },
	{ "COMMAND_BELOW_WORKSPACE",	XmCOMMAND_BELOW_WORKSPACE },
};

struct named_integer NI_DefaultButtonType[] = {
	{ "DIALOG_CANCEL_BUTTON",	XmDIALOG_CANCEL_BUTTON },
	{ "DIALOG_OK_BUTTON",	XmDIALOG_OK_BUTTON },
	{ "DIALOG_HELP_BUTTON",	XmDIALOG_HELP_BUTTON },
	{ "DIALOG_NONE",	XmDIALOG_NONE },
};

struct named_integer NI_DialogType[] = {
	{ "DIALOG_TEMPLATE",	XmDIALOG_TEMPLATE },
	{ "DIALOG_INFORMATION",	XmDIALOG_INFORMATION },
	{ "DIALOG_QUESTION",	XmDIALOG_QUESTION },
	{ "DIALOG_WORKING",	XmDIALOG_WORKING },
	{ "DIALOG_ERROR",	XmDIALOG_ERROR },
	{ "DIALOG_MESSAGE",	XmDIALOG_MESSAGE },
	{ "DIALOG_WARNING",	XmDIALOG_WARNING },
};

struct named_integer NI_EntryVerticalAlignment[] = {
	{ "ALIGNMENT_BASELINE_BOTTOM",	XmALIGNMENT_BASELINE_BOTTOM },
	{ "ALIGNMENT_BASELINE_TOP",XmALIGNMENT_BASELINE_TOP },
	{ "ALIGNMENT_CONTENTS_BOTTOM",XmALIGNMENT_CONTENTS_BOTTOM },
	{ "ALIGNMENT_CENTER",XmALIGNMENT_CENTER },
	{ "ALIGNMENT_CONTENTS_TOP",XmALIGNMENT_CONTENTS_TOP },
};

struct named_integer NI_Orientation[] = {
	{ "HORIZONTAL",	XmHORIZONTAL },
	{ "VERTICAL",	XmVERTICAL },
};

struct named_integer NI_Packing[] = {
	{ "PACK_TIGHT",	XmPACK_TIGHT },
	{ "PACK_COLUMN",XmPACK_COLUMN },
	{ "PACK_NONE",	XmPACK_NONE },
};

struct named_integer NI_RowColumnType[] = {
	{ "WORK_AREA",	XmWORK_AREA },
	{ "MENU_BAR",	XmMENU_BAR },
	{ "MENU_PULLDOWN",XmMENU_PULLDOWN },
	{ "MENU_POPUP",	XmMENU_POPUP },
	{ "MENU_OPTION",XmMENU_OPTION },
};

struct named_integer NI_ProcessingDirection[] = {
	{ "MAX_ON_TOP",	XmMAX_ON_TOP },
	{ "MAX_ON_BOTTOM",XmMAX_ON_BOTTOM },
	{ "MAX_ON_LEFT",XmMAX_ON_LEFT },
	{ "MAX_ON_RIGHT",XmMAX_ON_RIGHT },
};

struct named_integer NI_ChildPlacement[] = {
	{ "PLACE_ABOVE_SELECTION",	XmPLACE_ABOVE_SELECTION },
	{ "PLACE_BELOW_SELECTION",	XmPLACE_BELOW_SELECTION },
	{ "PLACE_TOP",			XmPLACE_TOP },
};

struct named_integer NI_SelectionDialogType[] = {
	{ "DIALOG_PROMPT",	XmDIALOG_PROMPT },
	{ "DIALOG_COMMAND",	XmDIALOG_COMMAND },
	{ "DIALOG_SELECTION",	XmDIALOG_SELECTION },
	{ "DIALOG_FILE_SELECTION",	XmDIALOG_FILE_SELECTION },
	{ "DIALOG_WORK_AREA",	XmDIALOG_WORK_AREA },
};

struct named_integer NI_SeparatorType[] = {
	{ "SINGLE_LINE",	XmSINGLE_LINE },
	{ "DOUBLE_LINE",	XmDOUBLE_LINE },
	{ "SINGLE_DASHED_LINE",	XmSINGLE_DASHED_LINE },
	{ "DOUBLE_DASHED_LINE",	XmDOUBLE_DASHED_LINE },
	{ "NO_LINE",		XmNO_LINE },
	{ "SHADOW_ETCHED_IN",	XmSHADOW_ETCHED_IN },
	{ "SHADOW_ETCHED_OUT",	XmSHADOW_ETCHED_OUT },
	{ "SHADOW_ETCHED_IN_DASH",	XmSHADOW_ETCHED_IN_DASH },
	{ "SHADOW_ETCHED_OUT_DASH",	XmSHADOW_ETCHED_OUT_DASH },
};

struct named_integer NI_IndicatorType[] = {
	{ "ONE_OF_MANY",	XmONE_OF_MANY },
	{ "N_OF_MANY",		XmN_OF_MANY },
};

struct named_integer NI_AudibleWarning[] = {
	{ "BELL",	XmBELL },
	{ "NONE",	XmNONE },
};

struct named_integer NI_DeleteResponse[] = {
	{ "DESTROY",	XmDESTROY },
	{ "UNMAP",	XmUNMAP },
	{ "DO_NOTHING",	XmDO_NOTHING },
};

struct named_integer NI_KeyboardFocusPolicy[] = {
	{ "EXPLICIT",	XmEXPLICIT },
	{ "POINTER",	XmPOINTER },
};

struct named_integer NI_InitialState[] = {
	{ "NormalState",	NormalState },
	{ "IconicState",	IconicState },
};

struct named_integer NI_HelpReturnSelectedWidgetResultType[] = {
	{ "HELP_SELECT_VALID",	DtHELP_SELECT_VALID },
	{ "HELP_SELECT_INVALID",DtHELP_SELECT_INVALID },
	{ "HELP_SELECT_ABORT",	DtHELP_SELECT_ABORT },
	{ "HELP_SELECT_ERROR",	DtHELP_SELECT_ERROR },
};

struct named_integer NI_QuickHelpChildType[] = {
	{ "HELP_QUICK_CLOSE_BUTTON",	DtHELP_QUICK_CLOSE_BUTTON },
	{ "HELP_QUICK_PRINT_BUTTON",DtHELP_QUICK_PRINT_BUTTON },
	{ "HELP_QUICK_HELP_BUTTON",DtHELP_QUICK_HELP_BUTTON },
	{ "HELP_QUICK_SEPARATOR",	DtHELP_QUICK_SEPARATOR },
	{ "HELP_QUICK_MORE_BUTTON",DtHELP_QUICK_MORE_BUTTON },
	{ "HELP_QUICK_BACK_BUTTON",DtHELP_QUICK_BACK_BUTTON },
};

struct named_integer NI_HelpType[] = {
	{ "HELP_TYPE_TOPIC",	DtHELP_TYPE_TOPIC },
	{ "HELP_TYPE_STRING",DtHELP_TYPE_STRING },
	{ "HELP_TYPE_DYNAMIC_STRING",DtHELP_TYPE_DYNAMIC_STRING },
	{ "HELP_TYPE_MAN_PAGE",	DtHELP_TYPE_MAN_PAGE },
	{ "HELP_TYPE_FILE",DtHELP_TYPE_FILE },
};

struct named_integer NI_HelpScrollBarDisplayPolicy[] = {
	{ "HELP_AS_NEEDED_SCROLLBARS",	DtHELP_AS_NEEDED_SCROLLBARS },
	{ "HELP_STATIC_SCROLLBARS",	DtHELP_STATIC_SCROLLBARS },
	{ "HELP_NO_SCROLLBARS",DtHELP_NO_SCROLLBARS },
};

struct named_integer NI_TextHighlightMode[] = {
	{ "HIGHLIGHT_NORMAL",	XmHIGHLIGHT_NORMAL },
	{ "HIGHLIGHT_SELECTED",	XmHIGHLIGHT_SELECTED },
	{ "HIGHLIGHT_SECONDARY_SELECTED",XmHIGHLIGHT_SECONDARY_SELECTED },
};

struct named_integer NI_TextSearchDirection[] = {
	{ "TEXT_FORWARD",	XmTEXT_FORWARD },
	{ "TEXT_BACKWARD",	XmTEXT_BACKWARD },
};

struct named_integer NI_ListSelectionType[] = {
	{ "INITIAL",		XmINITIAL },
	{ "MODIFICATION",	XmMODIFICATION },
	{ "ADDITION",		XmADDITION },
};

struct named_integer NI_TT_STATUS[] = {
        { "TT_OK", TT_OK } ,
        { "TT_WRN_NOTFOUND", TT_WRN_NOTFOUND } ,
	{ "TT_WRN_STALE_OBJID", TT_WRN_STALE_OBJID } ,
        { "TT_WRN_STOPPED", TT_WRN_STOPPED } ,
	{ "TT_WRN_SAME_OBJID", TT_WRN_SAME_OBJID } ,
	{ "TT_WRN_START_MESSAGE", TT_WRN_START_MESSAGE } ,
        { "TT_ERR_CLASS", TT_ERR_CLASS } ,
        { "TT_ERR_DBAVAIL", TT_ERR_DBAVAIL } ,
        { "TT_ERR_DBEXIST", TT_ERR_DBEXIST } ,
	{ "TT_ERR_FILE", TT_ERR_FILE } ,
	{ "TT_ERR_INVALID", TT_ERR_INVALID } ,
        { "TT_ERR_MODE", TT_ERR_MODE } ,
	{ "TT_ERR_ACCESS", TT_ERR_ACCESS } ,
        { "TT_ERR_NOMP", TT_ERR_NOMP } ,
        { "TT_ERR_NOTHANDLER", TT_ERR_NOTHANDLER } ,
        { "TT_ERR_NUM", TT_ERR_NUM } ,
        { "TT_ERR_OBJID", TT_ERR_OBJID } ,
        { "TT_ERR_OP", TT_ERR_OP } ,
        { "TT_ERR_OTYPE", TT_ERR_OTYPE } ,
        { "TT_ERR_ADDRESS", TT_ERR_ADDRESS } ,
        { "TT_ERR_PATH", TT_ERR_PATH } ,
        { "TT_ERR_POINTER", TT_ERR_POINTER } ,
        { "TT_ERR_PROCID", TT_ERR_PROCID } ,
        { "TT_ERR_PROPLEN", TT_ERR_PROPLEN } ,
        { "TT_ERR_PROPNAME", TT_ERR_PROPNAME } ,
        { "TT_ERR_PTYPE", TT_ERR_PTYPE } ,
        { "TT_ERR_DISPOSITION", TT_ERR_DISPOSITION } ,
        { "TT_ERR_SCOPE", TT_ERR_SCOPE } ,
        { "TT_ERR_SESSION", TT_ERR_SESSION } ,
        { "TT_ERR_VTYPE", TT_ERR_VTYPE } ,
	{ "TT_ERR_NO_VALUE", TT_ERR_NO_VALUE } ,
	{ "TT_ERR_INTERNAL", TT_ERR_INTERNAL } ,
	{ "TT_ERR_READONLY", TT_ERR_READONLY } ,
	{ "TT_ERR_NO_MATCH", TT_ERR_NO_MATCH } ,
	{ "TT_ERR_UNIMP", TT_ERR_UNIMP } ,
	{ "TT_ERR_OVERFLOW", TT_ERR_OVERFLOW } ,
	{ "TT_ERR_PTYPE_START", TT_ERR_PTYPE_START } ,
	{ "TT_ERR_CATEGORY", TT_ERR_CATEGORY } ,
	{ "TT_ERR_DBUPDATE", TT_ERR_DBUPDATE } ,
	{ "TT_ERR_DBFULL", TT_ERR_DBFULL } ,
	{ "TT_ERR_DBCONSIST", TT_ERR_DBCONSIST } ,
	{ "TT_ERR_STATE", TT_ERR_STATE } ,
	{ "TT_ERR_NOMEM", TT_ERR_NOMEM } ,
	{ "TT_ERR_SLOTNAME", TT_ERR_SLOTNAME } ,
	{ "TT_ERR_XDR", TT_ERR_XDR } ,
	{ "TT_ERR_NETFILE", TT_ERR_NETFILE } ,
	{ "TT_ERR_TOOLATE", TT_ERR_TOOLATE } ,
	{ "TT_ERR_AUTHORIZATION", TT_ERR_AUTHORIZATION } ,
	{ "TT_ERR_VERSION_MISMATCH", TT_ERR_VERSION_MISMATCH } ,
	{ "TT_DESKTOP_EPERM", TT_DESKTOP_EPERM } ,
	{ "TT_DESKTOP_ENOENT", TT_DESKTOP_ENOENT } ,
	{ "TT_DESKTOP_EINTR", TT_DESKTOP_EINTR } ,
	{ "TT_DESKTOP_EIO", TT_DESKTOP_EIO } ,
	{ "TT_DESKTOP_EAGAIN", TT_DESKTOP_EAGAIN } ,
	{ "TT_DESKTOP_ENOMEM", TT_DESKTOP_ENOMEM } ,
	{ "TT_DESKTOP_EACCES", TT_DESKTOP_EACCES } ,
	{ "TT_DESKTOP_EFAULT", TT_DESKTOP_EFAULT } ,
	{ "TT_DESKTOP_EEXIST", TT_DESKTOP_EEXIST } ,
	{ "TT_DESKTOP_ENODEV", TT_DESKTOP_ENODEV } ,
	{ "TT_DESKTOP_ENOTDIR", TT_DESKTOP_ENOTDIR } ,
	{ "TT_DESKTOP_EISDIR", TT_DESKTOP_EISDIR } ,
	{ "TT_DESKTOP_EINVAL", TT_DESKTOP_EINVAL } ,
	{ "TT_DESKTOP_ENFILE", TT_DESKTOP_ENFILE } ,
	{ "TT_DESKTOP_EMFILE", TT_DESKTOP_EMFILE } ,
	{ "TT_DESKTOP_ETXTBSY", TT_DESKTOP_ETXTBSY } ,
	{ "TT_DESKTOP_EFBIG", TT_DESKTOP_EFBIG } ,
	{ "TT_DESKTOP_ENOSPC", TT_DESKTOP_ENOSPC } ,
	{ "TT_DESKTOP_EROFS", TT_DESKTOP_EROFS } ,
	{ "TT_DESKTOP_EMLINK", TT_DESKTOP_EMLINK } ,
	{ "TT_DESKTOP_EPIPE", TT_DESKTOP_EPIPE } ,
	{ "TT_DESKTOP_ENOMSG", TT_DESKTOP_ENOMSG } ,
	{ "TT_DESKTOP_EDEADLK", TT_DESKTOP_EDEADLK } ,
	{ "TT_DESKTOP_ECANCELED", TT_DESKTOP_ECANCELED } ,
	{ "TT_DESKTOP_ENOTSUP", TT_DESKTOP_ENOTSUP } ,
	{ "TT_DESKTOP_ENODATA", TT_DESKTOP_ENODATA } ,
	{ "TT_DESKTOP_EPROTO", TT_DESKTOP_EPROTO } ,
	{ "TT_DESKTOP_ENOTEMPTY", TT_DESKTOP_ENOTEMPTY } ,
	{ "TT_DESKTOP_ETIMEDOUT", TT_DESKTOP_ETIMEDOUT } ,
	{ "TT_DESKTOP_EALREADY", TT_DESKTOP_EALREADY } ,
	{ "TT_DESKTOP_UNMODIFIED", TT_DESKTOP_UNMODIFIED } ,
	{ "TT_MEDIA_ERR_SIZE", TT_MEDIA_ERR_SIZE } ,
	{ "TT_MEDIA_ERR_FORMAT", TT_MEDIA_ERR_FORMAT } ,
	{ "TT_AUTHFILE_ACCESS", TT_AUTHFILE_ACCESS } ,
	{ "TT_AUTHFILE_LOCK", TT_AUTHFILE_LOCK } ,
	{ "TT_AUTHFILE_LOCK_TIMEOUT", TT_AUTHFILE_LOCK_TIMEOUT } ,
	{ "TT_AUTHFILE_UNLOCK", TT_AUTHFILE_UNLOCK } ,
	{ "TT_AUTHFILE_MISSING", TT_AUTHFILE_MISSING } ,
	{ "TT_AUTHFILE_ENTRY_MISSING", TT_AUTHFILE_ENTRY_MISSING } ,
	{ "TT_AUTHFILE_WRITE", TT_AUTHFILE_WRITE } ,
 };


struct named_integer NI_TT_SCOPE[] = {
	{ "TT_SCOPE_NONE", TT_SCOPE_NONE },
        { "TT_SESSION", TT_SESSION },
        { "TT_FILE", TT_FILE },
        { "TT_BOTH", TT_BOTH },
	{ "TT_FILE_IN_SESSION", TT_FILE_IN_SESSION },
};

struct named_integer NI_TT_OP[] = {
	{ "TTDT_OP_NONE", TTDT_OP_NONE },
	{ "TTDT_CREATED", TTDT_CREATED },
	{ "TTDT_DELETED", TTDT_DELETED },
	{ "TTDT_DO_COMMAND", TTDT_DO_COMMAND },
	{ "TTDT_SET_ENVIRONMENT", TTDT_SET_ENVIRONMENT },
	{ "TTDT_GET_ENVIRONMENT", TTDT_GET_ENVIRONMENT },
	{ "TTDT_SET_GEOMETRY", TTDT_SET_GEOMETRY },
	{ "TTDT_GET_GEOMETRY", TTDT_GET_GEOMETRY },
	{ "TTDT_SET_ICONIFIED", TTDT_SET_ICONIFIED },
	{ "TTDT_GET_ICONIFIED", TTDT_GET_ICONIFIED },
	{ "TTDT_SET_LOCALE", TTDT_SET_LOCALE },
	{ "TTDT_GET_LOCALE", TTDT_GET_LOCALE },
	{ "TTDT_SET_MAPPED", TTDT_SET_MAPPED },
	{ "TTDT_GET_MAPPED", TTDT_GET_MAPPED },
	{ "TTDT_MODIFIED", TTDT_MODIFIED },
	{ "TTDT_REVERTED", TTDT_REVERTED },
	{ "TTDT_GET_MODIFIED", TTDT_GET_MODIFIED },
	{ "TTDT_MOVED", TTDT_MOVED },
	{ "TTDT_PAUSE", TTDT_PAUSE },
	{ "TTDT_RESUME", TTDT_RESUME },
	{ "TTDT_QUIT", TTDT_QUIT },
	{ "TTDT_RAISE", TTDT_RAISE },
	{ "TTDT_LOWER", TTDT_LOWER },
	{ "TTDT_SAVE", TTDT_SAVE },
	{ "TTDT_REVERT", TTDT_REVERT },
	{ "TTDT_SAVED", TTDT_SAVED },
	{ "TTDT_SET_SITUATION", TTDT_SET_SITUATION },
	{ "TTDT_GET_SITUATION", TTDT_GET_SITUATION },
	{ "TTDT_SIGNAL", TTDT_SIGNAL },
	{ "TTDT_STARTED", TTDT_STARTED },
	{ "TTDT_STOPPED", TTDT_STOPPED },
	{ "TTDT_STATUS", TTDT_STATUS },
	{ "TTDT_GET_STATUS", TTDT_GET_STATUS },
	{ "TTDT_GET_SYSINFO", TTDT_GET_SYSINFO },
	{ "TTDT_SET_XINFO", TTDT_SET_XINFO },
	{ "TTDT_GET_XINFO", TTDT_GET_XINFO },
	{ "TTME_ABSTRACT", TTME_ABSTRACT },
	{ "TTME_DEPOSIT", TTME_DEPOSIT },
	{ "TTME_DISPLAY", TTME_DISPLAY },
	{ "TTME_EDIT", TTME_EDIT },
	{ "TTME_COMPOSE", TTME_COMPOSE },
	{ "TTME_INTERPRET", TTME_INTERPRET },
	{ "TTME_PRINT", TTME_PRINT },
	{ "TTME_TRANSLATE", TTME_TRANSLATE },
};


struct named_integer NI_TEAR_OFF[] = {
	{ "TEAR_OFF_ENABLED", XmTEAR_OFF_ENABLED },
        { "TEAR_OFF_DISABLED", XmTEAR_OFF_DISABLED },
};

/************************************************************************/

struct named_integer NI_Button[] = {
	{ "Button1",	Button1 },
	{ "Button2",	Button2 },
	{ "Button3",	Button3 },
	{ "Button4",	Button4 },
	{ "Button5",	Button5 },
};

struct named_integer NI_EventType[] = {
        {"KeyPress", KeyPress},
        {"KeyRelease", KeyRelease},
        {"ButtonPress", ButtonPress},
        {"ButtonRelease", ButtonRelease},
        {"MotionNotify", MotionNotify},
        {"EnterNotify", EnterNotify},
        {"LeaveNotify", LeaveNotify},
        {"FocusIn", FocusIn},
        {"FocusOut", FocusOut},
        {"KeymapNotify", KeymapNotify},
        {"Expose", Expose},
        {"GraphicsExpose", GraphicsExpose},
        {"NoExpose", NoExpose},
        {"VisibilityNotify", VisibilityNotify},
        {"CreateNotify", CreateNotify},
        {"DestroyNotify", DestroyNotify},
        {"UnmapNotify", UnmapNotify},
        {"MapNotify", MapNotify},
        {"MapRequest", MapRequest},
        {"ReparentNotify", ReparentNotify},
        {"ConfigureNotify", ConfigureNotify},
        {"ConfigureRequest", ConfigureRequest},
        {"GravityNotify", GravityNotify},
        {"ResizeRequest", ResizeRequest},
        {"CirculateNotify", CirculateNotify},
        {"PropertyNotify", PropertyNotify},
        {"SelectionClear", SelectionClear},
        {"SelectionRequest", SelectionRequest},
        {"SelectionNotify", SelectionNotify},
        {"ColormapNotify", ColormapNotify},
        {"ClientMessage", ClientMessage},
        {"MappingNotify", MappingNotify},
};

struct named_integer NI_XBool[] = {
	{ "true",	1 },
	{ "false",	0 },
};

struct named_integer NI_TextFormat[] = {
	{ "FMT_8_BIT",	XmFMT_8_BIT },
	{ "FMT_16_BIT",	XmFMT_16_BIT },
};

struct named_integer NI_HelpHyperType[] = {
	{ "HELP_LINK_TOPIC",		DtHELP_LINK_TOPIC },
	{ "HELP_LINK_MAN_PAGE",		DtHELP_LINK_MAN_PAGE },
	{ "HELP_LINK_APP_DEFINE",	DtHELP_LINK_APP_DEFINE },
};

struct named_integer NI_WIN_GRAVITY[] = {
	{ "ForgetGravity",	ForgetGravity },
	{ "NorthWestGravity",	NorthWestGravity },
	{ "NorthGravity",	NorthGravity },
	{ "NorthEastGravity",	NorthEastGravity },
	{ "WestGravity",	WestGravity },
	{ "CenterGravity",	CenterGravity },
	{ "EastGravity",	EastGravity },
	{ "SouthWestGravity",	SouthWestGravity },
	{ "SouthGravity",	SouthGravity },
	{ "SouthEastGravity",	SouthEastGravity },
	{ "StaticGravity",	StaticGravity },
	{ "-1",			-1 },
};

/************************************************************************/

void
DtkshRegisterNamedIntConverters( void )
{
	XtConvertArgRec args[1];

#define SETARGS(X) args[0].address_mode = XtAddress; args[0].address_id = (caddr_t)&X[0]; args[0].size = sizeof(X);

        /* Special!! Converts string to named value */
	SETARGS(NI_TraversalDirection);
	XtAddConverter(XmRString, "TraversalDirection", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_CommandChild);
	XtAddConverter(XmRString, "CommandChildType", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_MessageBoxChild);
	XtAddConverter(XmRString, "MessageBoxChildType", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_FileSelChild);
	XtAddConverter(XmRString, "FileSelChildType", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_SelBoxChild);
	XtAddConverter(XmRString, "SelBoxChildType", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_QuickHelpChildType);
	XtAddConverter(XmRString, "QuickHelpChildType", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_HelpType);
	XtAddConverter(XmRString, "HelpType", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_HelpScrollBarDisplayPolicy);
	XtAddConverter(XmRString, "HelpScrollBarDisplayPolicy", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_TextHighlightMode);
	XtAddConverter(XmRString, "TextHighlightMode", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_TextSearchDirection);
	XtAddConverter(XmRString, "TextSearchDirection", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_TextFormat);
	XtAddConverter(XmRString, "TextFormat", 
		DtkshCvtStringToNamedValue, 
		args, 1);
	SETARGS(NI_TT_OP);
	XtAddConverter(XmRString, "TtOp", DtkshCvtStringToNamedValue, 
                       args, 1);
	SETARGS(NI_TT_SCOPE);
	XtAddConverter(XmRString, "TtScope", DtkshCvtStringToNamedValue, 
		       args, 1);
	SETARGS(NI_TT_STATUS);
	XtAddConverter(XmRString, "TtStatus", DtkshCvtStringToNamedValue, 
		       args, 1);
	SETARGS(NI_TEAR_OFF);
	XtAddConverter(XmRString, XmRTearOffModel, DtkshCvtStringToNamedValue, 
		       args, 1);
	SETARGS(NI_WIN_GRAVITY);
	XtAddConverter(XmRString, "XWinGravity", DtkshCvtStringToNamedValue, 
		       args, 1);
	SETARGS(NI_MWMInputMode);
	XtAddConverter(XmRString, "MWMInputMode", DtkshCvtStringToNamedValue, 
		       args, 1);

        /* The rest of these convert named values into strings */
	SETARGS(NI_ArrowDirection);
	XtAddConverter(XmRArrowDirection, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_MultiClick);
	XtAddConverter(XmRMultiClick, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_DialogStyle);
	XtAddConverter(XmRDialogStyle, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_MWMInputMode);
	XtAddConverter("MWMInputMode", XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ResizePolicy);
	XtAddConverter(XmRResizePolicy, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_FileTypeMask);
	XtAddConverter(XmRFileTypeMask, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ShadowType);
	XtAddConverter(XmRShadowType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_Attachment);
	XtAddConverter(XmRAttachment, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ChildType);
	XtAddConverter(XmRChildType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_Alignment);
	XtAddConverter(XmRChildHorizontalAlignment, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ChildVerticalAlignment);
	XtAddConverter(XmRChildVerticalAlignment, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_UnitType);
	XtAddConverter(XmRUnitType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_NavigationType);
	XtAddConverter(XmRNavigationType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_Alignment);
	XtAddConverter(XmRAlignment, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_LabelType);
	XtAddConverter(XmRLabelType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_StringDirection);
	XtAddConverter(XmRStringDirection, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_SelectionPolicy);
	XtAddConverter(XmRSelectionPolicy, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ScrollBarPlacement);
	XtAddConverter(XmRScrollBarPlacement, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ScrollBarDisplayPolicy);
	XtAddConverter(XmRScrollBarDisplayPolicy, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ScrollingPolicy);
	XtAddConverter(XmRScrollingPolicy, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_VisualPolicy);
	XtAddConverter(XmRVisualPolicy, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ListSizePolicy);
	XtAddConverter(XmRListSizePolicy, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_EditMode);
	XtAddConverter(XmREditMode, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_CommandWindowLocation);
	XtAddConverter(XmRCommandWindowLocation, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_DefaultButtonType);
	XtAddConverter(XmRDefaultButtonType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_DialogType);
	XtAddConverter(XmRDialogType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_EntryVerticalAlignment);
	XtAddConverter(XmRVerticalAlignment, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_Orientation);
	XtAddConverter(XmROrientation, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_Packing);
	XtAddConverter(XmRPacking, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_RowColumnType);
	XtAddConverter(XmRRowColumnType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ProcessingDirection);
	XtAddConverter(XmRProcessingDirection, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ChildPlacement);
	XtAddConverter(XmRChildPlacement, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_SelectionDialogType);
	XtAddConverter(XmRSelectionType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_SeparatorType);
	XtAddConverter(XmRSeparatorType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_IndicatorType);
	XtAddConverter(XmRIndicatorType, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_AudibleWarning);
	XtAddConverter(XmRAudibleWarning, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_DeleteResponse);
	XtAddConverter(XmRDeleteResponse, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_KeyboardFocusPolicy);
	XtAddConverter(XmRKeyboardFocusPolicy, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_InitialState);
	XtAddConverter(XmRInitialState, XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_HelpReturnSelectedWidgetResultType);
	XtAddConverter("HelpReturnSelectedWidgetResultType", XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_HelpType);
	XtAddConverter("HelpType", XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_HelpScrollBarDisplayPolicy);
	XtAddConverter("HelpScrollBarDisplayPolicy", XtRString, 
		DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_Button);
	XtAddConverter("XE_Button", XtRString, 
                DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_EventType);
	XtAddConverter("XE_EventType", XtRString, 
                DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_XBool);
	XtAddConverter("X_Bool", XtRString, 
                DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_TraversalDirection);
	XtAddConverter("TraversalDirection", XtRString, 
                DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_VisibilityType);
	XtAddConverter("VisibilityType", XtRString,
                DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_ListSelectionType);
	XtAddConverter("ListSelectionType", XtRString, 
                DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_TextFormat);
	XtAddConverter("TextFormat", XtRString, 
                DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_HelpHyperType);
	XtAddConverter("HelpHyperType", XtRString, 
                DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_TT_STATUS);
	XtAddConverter("TtStatus", XtRString, DtkshCvtNamedValueToString, 
		        args, 1);
	SETARGS(NI_TT_OP);
	XtAddConverter("TtOp", XtRString, DtkshCvtNamedValueToString, args, 1);
	SETARGS(NI_TEAR_OFF);
	XtAddConverter(XmRTearOffModel, XtRString, DtkshCvtNamedValueToString, 
		args, 1);
	SETARGS(NI_WIN_GRAVITY);
	XtAddConverter("XWinGravity", XtRString, DtkshCvtNamedValueToString, 
		args, 1);
}

/*****************************************************************************/

static ModifierState modifierTable[] = {
   {"Button1Mask", Button1Mask},
   {"Button2Mask", Button2Mask},
   {"Button3Mask", Button3Mask},
   {"Button4Mask", Button4Mask},
   {"Button5Mask", Button5Mask},
   {"ControlMask", ControlMask},
   {"LockMask",    LockMask},
   {"Mod1Mask",    Mod1Mask},
   {"Mod2Mask",    Mod2Mask},
   {"Mod3Mask",    Mod3Mask},
   {"Mod4Mask",    Mod4Mask},
   {"Mod5Mask",    Mod5Mask},
   {"ShiftMask",   ShiftMask},
   {NULL,          0},
};


void
DtkshCvtXEModifierStateToString(
        XrmValuePtr args,
        Cardinal *nargs,
        XrmValuePtr fval,
        XrmValuePtr toval )
{
   static char *string = NULL;
   unsigned int value;
   int i = 0;
   Boolean firstOne = True;

   string = XtRealloc(string, 1);
   string[0] = '\0';
   value = ((unsigned int *)(fval->addr))[0];

   while (modifierTable[i].modifierName)
   {
      if (value & modifierTable[i].modifier)
      {
         if (!firstOne)
         {
            string = XtRealloc(string,
                  strlen(string) + strlen(modifierTable[i].modifierName) + 2);
            strcat(string, "|");
            strcat(string, modifierTable[i].modifierName);
         }
         else
         {
            firstOne = False;
            string = XtRealloc(string,
                  strlen(string) + strlen(modifierTable[i].modifierName) + 1);
            strcat(string, modifierTable[i].modifierName);
         }
      }
      i++;
   }

   toval->addr = (caddr_t)string;
   toval->size = strlen(string) + 1;
}


void
DtkshCvtXEMotionHintToString(
        XrmValuePtr args,
        Cardinal *nargs,
        XrmValuePtr fval,
        XrmValuePtr toval )
{
   if (fval->size != sizeof(char)) 
   {
      toval->addr = NULL;
      toval->size = 0;
      return;
   }
   if ((((char *)(fval->addr))[0]) == NotifyNormal) 
      toval->addr = (caddr_t)("NotifyNormal");
    else
      toval->addr = (caddr_t)("NotifyHint");
   toval->size = strlen(toval->addr)+1;
}

/*
 * The following routines exist in order to enable ToolTalk commands
 * to operate without the script having to call XtInitialize.  Another way
 * of accomplishing the conversions done by these routines is by calling
 * XtConvert with appropriate parameters, but XtConvert requires a valid
 * widget in order to look up the converter tables.
 *
 * The routines provided are:
 *	DtkshCvtTtStatusToString()
 *	DtkshCvtStringToTtStatus()
 * 	DtkshCvtStringToBool()
 *	DtkshCvtStringToTtOp()
 *	DtkshCvtTtOpToString()
 *	DtkshCvtStringToTtScope()
 */

/*
 * DtkshCvtTtStatusToString takes a toolTalk error code and a pointer to an
 * XrmValue struct.  The XrmValue struct will be filled in with the
 * results of the translation - presumably a string version of the
 * error code.
 * This routine is in this file because it must "know" the size of the
 * NI_TT_STATUS array.
 */
void
DtkshCvtTtStatusToString(
        Tt_status ttStatus,
        XrmValue *pToVal)
{
   XrmValue cval[1], fval;
   Cardinal nargs;

   fval.addr = (caddr_t)&ttStatus;
   fval.size = sizeof(Tt_status);
   nargs = 1;
   cval[0].size = sizeof(NI_TT_STATUS);
   cval[0].addr = (caddr_t)NI_TT_STATUS;
   DtkshCvtNamedValueToString(cval, &nargs, &fval, pToVal);
   return;
}

/*
 * DtkshCvtStringToTtStatus - converts a string to the corresponding
 * TtStatus value.
 */
void
DtkshCvtStringToTtStatus(
        char *pString,
        XrmValue *pToVal)
{
   XrmValue cval[1], fval;
   Cardinal nargs;

   fval.addr = (caddr_t)pString;
   fval.size = strlen(pString);
   nargs = 1;
   cval[0].size = sizeof(NI_TT_STATUS);
   cval[0].addr = (caddr_t)NI_TT_STATUS;
   DtkshCvtStringToNamedValue(cval, &nargs, &fval, pToVal);
   return;
}

/*
 * DtkshCvtStringToBool - takes a string (either "true" or "false")
 * and a pointer to an XrmValue struct.  The XrmValue struct will be filled
 * out with the results of the conversion of the string to a boolean.
 * This routine is in this file because it must "know" the size of the
 * NI_XBool array.
 */
void
DtkshCvtStringToBool(
        char *pString,
        XrmValue *pToVal)
{
   XrmValue cval[1], fval;
   Cardinal nargs;

   cval[0].size = sizeof(NI_XBool);
   cval[0].addr = (caddr_t)NI_XBool;
   fval.addr = pString;
   fval.size = strlen(pString);
   nargs = 1;
   DtkshCvtStringToNamedValue(cval, &nargs, &fval, pToVal);
   return;
}


/*
 * DtkshCvtStringToTtOp - converts a string to the corresponding
 * TtOp value.
 */
void
DtkshCvtStringToTtOp(
        char *pString,
        XrmValue *pToVal)
{
   XrmValue cval[1], fval;
   Cardinal nargs;

   fval.addr = (caddr_t)pString;
   fval.size = strlen(pString);
   nargs = 1;
   cval[0].size = sizeof(NI_TT_OP);
   cval[0].addr = (caddr_t)NI_TT_OP;
   DtkshCvtStringToNamedValue(cval, &nargs, &fval, pToVal);
   return;
}

/*
 * DtkshCvtTtOpToString - converts a TtOp to the representative string.
 */
void
DtkshCvtTtOpToString(
	Tttk_op op,
        XrmValue *pToVal)
{
   XrmValue cval[1], fval;
   Cardinal nargs;

   fval.addr = (caddr_t)&op;
   fval.size = sizeof(Tttk_op);
   nargs = 1;
   cval[0].size = sizeof(NI_TT_OP);
   cval[0].addr = (caddr_t)NI_TT_OP;
   DtkshCvtNamedValueToString(cval, &nargs, &fval, pToVal);
   return;
}

/*
 * DtkshCvtStringToTtScope - converts a string to the appropriate
 * TtScope numeric value.
 */
void
DtkshCvtStringToTtScope(
	char *pString,
        XrmValue *pToVal)
{
   XrmValue cval[1], fval;
   Cardinal nargs;

   fval.addr = (caddr_t)pString;
   fval.size = strlen(pString);
   nargs = 1;
   cval[0].size = sizeof(NI_TT_OP);
   cval[0].addr = (caddr_t)NI_TT_OP;
   DtkshCvtNamedValueToString(cval, &nargs, &fval, pToVal);
   return;
}
