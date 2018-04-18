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
/* $TOG: buff.c /main/5 1999/07/20 14:48:23 mgreess $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         buff.c
 **
 **   Description:  Buffer transfer functions for the CDE Drag & Drop Demo.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <time.h>

#include <X11/Intrinsic.h>

#include <Xm/Xm.h>
#include <Xm/List.h>

#include <Dt/Dt.h>
#include <Dt/Dnd.h>

#include "icon.h"
#include "file.h"
#include "demo.h"
#include "buff.h"

extern Widget	topLevel;

 /*************************************************************************
 * 
 *	Data Structures & Private Declarations For Appointment Buffers
 * 
 **************************************************************************/

/*
 * Appointment structure used to store appointments internally.
 */

typedef struct _Appointment {
        char           *date;
        char           *start;
        char           *end;
        char           *what;
} Appointment;

/*
 * List of appointments which always appear to be for the current date.
 */

char    today[9];       /* initialized in apptCreateList() */

Appointment todaysApptList[] = {
#ifdef TEST_APPT_NAMES
        { today, "", "", "Staff Meeting" },		  /* no name */
        { today, " 9:30am", "10:00am", "Will's Party1" }, /* duplicate names */
        { today, " 9:30am", "10:00am", "Will's Party2" },
#else
        { today, " 9:00am", " 9:30am", "Staff Meeting" },
        { today, " 9:30am", "10:00am", "Will's Party" },
#endif
        { today, "10:00am", "10:30am", "Conference Call" },
        { today, "10:30am", "11:30am", "Work on Mail" },
        { today, "11:00am", "11:30pm", "B'fast w/ Robert" },
        { today, " 1:30pm", " 2:30pm", "Design Meeting" },
        { today, " 3:00pm", " 4:00pm", "Communications" },
        { today, " 4:00pm", " 4:30pm", "Pick up Dogs" },
        { today, " 5:00pm", " 6:30pm", "Beer Bust" },
        { today, " 7:00pm", " 9:00pm", "Dinner - Stuart" },
        { NULL, NULL, NULL, NULL }
};

/*
 * CDE appointment format used to transfer the appointments via drag & drop.
 */

char *apptFormat =
"   ** Calendar Appointment **\n\
\n\
        Date:    %s\n\
        Start:   %s\n\
        End:     %s\n\
        What:    %s\n";

/*
 * Private Appointment Buffer Function Declarations
 */

static void		apptConvertCallback(Widget, XtPointer, XtPointer);
static int		apptCreateList(XmString**);
static void		apptDestroyList(XmString*,int);
static Appointment*	apptFromListEntry(XmString);
static char*		apptGetLabel(char*);

 /*************************************************************************
 * 
 *	Appointment Drag & Drop
 * 
 **************************************************************************/

/*
 * apptConvertCallback
 *
 * Fills in buffer structure with calendar appointment string based on which
 * appointments are selected in the scrolled list when the drag is started.
 * ihen no appointments are selected, the appointment under the pointer is
 * used. Supply a label for the calendar appointment based on the contents
 * of the appointment.
 */
static void
apptConvertCallback(
        Widget          dragContext,
        XtPointer       clientData,
        XtPointer       callData)
{
        DtDndConvertCallbackStruct *convertInfo =
                                  (DtDndConvertCallbackStruct *) callData;
        DtDndBuffer     *buffers = convertInfo->dragData->data.buffers;
        Widget          apptList = (Widget)clientData;
        int             selectedPos, ii,
                        selectedItemCount;
        XmStringTable   selectedItems;
        char            apptString[1024];
        String          labelString;
        Appointment     *appt;

	if (convertInfo == NULL) {
		return;
	}

        /*
         * Verify the validity of the callback reason
         */

        if (convertInfo->dragData->protocol != DtDND_BUFFER_TRANSFER ||
	    (convertInfo->reason != DtCR_DND_CONVERT_DATA &&
             convertInfo->reason != DtCR_DND_CONVERT_DELETE)) {
                return;
        }

        /*
         * Get selected items from the list
         */

        XtVaGetValues(apptList,
                XmNuserData,            &selectedPos,
                XmNselectedItemCount,   &selectedItemCount,
                XmNselectedItems,       &selectedItems,
                NULL);

        for (ii = 0; ii < convertInfo->dragData->numItems; ii++) {

                /*
                 * Get the actual appointment(s)
                 */

                if (selectedItemCount == 0) {
                        appt = &todaysApptList[selectedPos-1+ii];
                } else {
                        appt = apptFromListEntry(selectedItems[ii]);
                }
                sprintf(apptString, apptFormat,
			appt->date, appt->start, appt->end, appt->what);

                /*
                 * Supply the appointment(s) for transfer
                 */

                if (convertInfo->reason == DtCR_DND_CONVERT_DATA) {

                        /* Copy the appointment into the buffer for transfer */

                        buffers[ii].bp = XtNewString(apptString);
                        buffers[ii].size = strlen(buffers[ii].bp);

                        /* Supply the name for the appointment */

                        labelString = apptGetLabel(apptString);
			if (labelString == NULL) {
                        	buffers[ii].name = (char *)NULL;
			} else {
                        	buffers[ii].name = XtNewString(labelString);
                        	XtFree(labelString);
			}

                /*
                 * Delete the moved appointment(s)
                 */

                } else if (convertInfo->reason == DtCR_DND_CONVERT_DELETE) {
                        printf("Delete appointment for %s\n", appt->what);
                }
        }
}

/*
 * apptDragFinishCallback
 *
 * Free buffer data/names allocated in apptConvertCallback()
 */
void
apptDragFinishCallback(
        Widget          widget,
        XtPointer       clientData,
        XtPointer       callData)
{
	DtDndDragFinishCallbackStruct *dropFinishInfo =
				(DtDndDragFinishCallbackStruct *)callData;
	DtDndContext	*dragData = dropFinishInfo->dragData;
	int		ii;

	for (ii = 0; ii < dragData->numItems; ii++) {
		XtFree(dragData->data.buffers[ii].bp);
		if (dragData->data.buffers[ii].name != NULL)
			XtFree(dragData->data.buffers[ii].name);
	}
}

/*
 * apptTransferCallback
 *
 * Handles the transfer of an appointment to the draw area drop site.
 * Adds the appropriate icon to the list of icons on the draw area.
 */
void
apptTransferCallback(
        Widget          widget,
        XtPointer       clientData,
        XtPointer       callData)
{
        DtDndTransferCallbackStruct *transferInfo =
                                (DtDndTransferCallbackStruct*) callData;
        DtDndBuffer     *buffers;
	IconInfo	*iconList, *iconPtr;
	char		*name;
	char		*filename;
	int		ii;

	if (transferInfo == NULL) {
		return;
	}

	/*
	 * Verify the validity of the callback reason.
	 */
	
	if (transferInfo->dropData->protocol != DtDND_BUFFER_TRANSFER ||
	    transferInfo->reason != DtCR_DND_TRANSFER_DATA) {
		return;
	}

	/*
	 * Use abbreviated method of reffering to the data buffers.
	 */

	if (transferInfo != NULL && transferInfo->dropData != NULL) {
        	buffers = transferInfo->dropData->data.buffers;
	} else {
		return;
	}

	/*
	 * Process each item being transfered.
	 */

        XtVaGetValues(widget, XmNuserData, &iconList, NULL);
        for (ii = 0; ii < transferInfo->dropData->numItems; ii++) {

		/*
		 * Check format of buffer
		 */

		/*
		 * Transfer the buffer data. Here there is no actual transfer
		 * taking place. Only the icons representing the appointments
		 * are created to indicate the transfer.
		 */

                name = buffers[ii].name;
		if (name == NULL)
			name = "unnamed";

		/* Create file from buffer */

		filename = fileStoreBuffer(buffers[ii].name,
			buffers[ii].bp, buffers[ii].size);
		printf("Stored buffer into '%s'\n", filename);
		XtFree(filename);

		/* Create icon */

                iconPtr = IconNew();
                IconInitialize(widget, iconPtr,
			transferInfo->x + ii * 10,
                	transferInfo->y + ii * 10,
                	buffers[ii].bp, buffers[ii].size, name, IconByData);

		/* Add to icon list on drop site */

                iconPtr->next = iconList;
		if (iconList != NULL) {
			iconList->prev = iconPtr;
		}
		iconList = iconPtr;
                XtVaSetValues(widget, XmNuserData, iconList, NULL);
	}
}

/*
 * apptDragSetup
 *
 * Prepares the appointment list to source drags of appointments with button 1.
 */
void apptDragSetup(Widget apptDragSource)
{
    static char	translations[] = "\
	~c ~s ~m ~a <Btn1Down>:\
	    demoProcessPress(ListBeginSelect,apptDragStart)\n\
	c ~s ~m ~a <Btn1Down>:\
	    demoProcessPress(ListBeginToggle,apptDragStart)";
    static char	btn2_translations[] = "\
	~c ~s ~m ~a <Btn2Down>:\
	    demoProcessPress(ListBeginSelect,apptDragStart)\n\
	c ~s ~m ~a <Btn2Down>:\
	    demoProcessPress(ListBeginToggle,apptDragStart)\n\
	<Btn2Motion>:ListButtonMotion()\n\
	~c ~s ~m ~a <Btn2Up>:ListEndSelect()\n\
	c ~s ~m ~a <Btn2Up>:ListEndToggle()";
    static XtActionsRec	actionTable[] =
    {
	{"apptDragStart", (XtActionProc) &apptDragStart},
	{"demoProcessPress", (XtActionProc) &demoProcessPress}
    };

    int		btn1_transfer = 0;
    XtTranslations	new_translations;

    XtAppAddActions(
		demoAppContext,
		actionTable,
		sizeof(actionTable)/sizeof(actionTable[0]));
    new_translations = XtParseTranslationTable(translations);
    XtOverrideTranslations(apptDragSource, new_translations);

    XtVaGetValues(
	(Widget) XmGetXmDisplay(XtDisplayOfObject(apptDragSource)),
	"enableBtn1Transfer", &btn1_transfer,
	NULL);
    
    if (btn1_transfer != True)
    {
	new_translations = XtParseTranslationTable(btn2_translations);
	XtOverrideTranslations(apptDragSource, new_translations);
    }

#if 0
    XtAddEventHandler(apptDragSource, Button1MotionMask, False,
                (XtEventHandler)demoDragMotionHandler,
                (XtPointer)DtDND_BUFFER_TRANSFER);
#endif
}

/*
 * apptDropSetup
 *
 * Such a function is not needed since the demoDropSetup in demo.c registers
 * the draw area as a drop site for drops of buffers such as appointments.
 */

/*
 * apptDragStart
 *
 * Initiates a drag of an appointment from the appointment list provided
 * the pointer is over an appointment in the list.
 */
void
apptDragStart(
        Widget          widget,
        XEvent          *event)
{
        static XtCallbackRec convertCBRec[] = { {apptConvertCallback, NULL},
                                                {NULL, NULL} };
        static XtCallbackRec dragFinishCBRec[] = 
					      { {demoDragFinishCallback, NULL},
					        {apptDragFinishCallback, NULL},
                                                {NULL, NULL} };
        static IconInfo *iconPtr = NULL;
        Widget          dragIcon;
        Display         *display = XtDisplay(widget);
        int             itemCount,
                        selectedPos,
                        selectedItemCount;
        char            apptString[1024];

        convertCBRec[0].closure = (XtPointer)widget;

	/*
	 * Get list of selected items from the scrolled list of appointments
	 */

        XtVaGetValues(widget,
                XmNitemCount, &itemCount,
                XmNselectedItemCount, &selectedItemCount,
                NULL);

	/*
	 * Find out which item the pointer was over when the drag began
	 */

        selectedPos = XmListYToPos(widget, event->xmotion.y);

        if (selectedPos == 0 || selectedPos > itemCount) {
                return;
        }

        XtVaSetValues(widget, XmNuserData, selectedPos, NULL);

	/*
	 * Copy the appointment information into an appointment string
	 */

        sprintf(apptString, apptFormat,
		todaysApptList[1].date, todaysApptList[1].start,
		todaysApptList[1].end, todaysApptList[1].what);

	/*
	 * Create drag icon for appointment buffer drag
	 */

        if (iconPtr == NULL) {
                iconPtr = IconNew();
                IconInitialize(widget, iconPtr, 0, 0,
			apptString, strlen(apptString), NULL, IconByData);
        }

        if (iconPtr->dragIcon == NULL) {
                iconPtr->dragIcon = DtDndCreateSourceIcon(widget,
                        iconPtr->bitmap, iconPtr->mask);
        }
        if (selectedItemCount > 1) {
                dragIcon = NULL;
                itemCount = selectedItemCount;
        } else {
                dragIcon = iconPtr->dragIcon;
                itemCount = 1;
        }

	/*
	 * Start the drag
	 */

        if (DtDndVaDragStart(widget, event, DtDND_BUFFER_TRANSFER, itemCount,
                        XmDROP_COPY,
                        convertCBRec, dragFinishCBRec,
                        DtNsourceIcon, dragIcon,
			DtNbufferIsText, True,
                        NULL)
            == NULL) {

                printf("DragStart returned NULL.\n");
        }
}

 /*************************************************************************
 *
 *	Appointment Creation, Initialization & Destruction
 *
 *************************************************************************/

/*
 * apptCreateDragSource
 *
 * Create a scrolling list filled with appointments.
 */
Widget
apptCreateDragSource(
	Widget		parent)
{
	Widget		apptList;
	XmString	*apptListItems;
	int		apptCount;

	apptCount = apptCreateList(&apptListItems);

        apptList = XtVaCreateManagedWidget("apptList",
                xmListWidgetClass, parent,
		/*
		 * Uncomment the line specifying the selection policy to
		 * enable multiple buffer transfers from the scrolled list
		 * of appointments. WARNING: There is a bug in the interaction
		 * of the scrolled list and Drag and Drop which causes items
		 * in the scrolled list which are selected to appear unselected
		 * and vice versa.
		 */
                /* XmNselectionPolicy, XmMULTIPLE_SELECT, */
                XmNitems, apptListItems,
                XmNitemCount, apptCount,
                NULL);

	apptDestroyList(apptListItems, apptCount);
        
	return apptList;
}

/*
 * apptCreateDropSite
 *
 * Such a function is not needed since the drop site is the draw area which
 * is created in demoCreateDropSite() in demo.c
 */

/*
 * apptCreateList
 *
 * Creates a list of XmStrings with appointment data in them.
 */
static int
apptCreateList(
        XmString      **appts)
{
        int             ii, apptCount;
        char            tmpStr[256];
        time_t          now;
        struct tm       *tm;

        now = time(&now);
        tm = localtime(&now);

        sprintf(today, "%2d/%2d/%2d", tm->tm_mon+1, tm->tm_mday, tm->tm_year);

        for (ii = 0; todaysApptList[ii].date; ii++);

        apptCount = ii;

        *appts = (XmString *) XtMalloc(sizeof(XmString) * apptCount);

        for (ii = 0; todaysApptList[ii].date; ii++) {

                sprintf(tmpStr, "%s %s", todaysApptList[ii].start,
                                         todaysApptList[ii].what);

                (*appts)[ii] = XmStringCreate(tmpStr, XmFONTLIST_DEFAULT_TAG);

        }
        return apptCount;
}

/*
 * apptDestroyList
 *
 * Destroys a list of XmStrings with appointment data in them.
 */
static void
apptDestroyList(
        XmString       *appts,
	int		apptCount)
{
	int		ii;

        for (ii = 0; ii < apptCount; ii++) {
                XmStringFree(appts[ii]);
        }
	XtFree((char *)appts);
}

 /*************************************************************************
 *
 *	Appointment Utility Functions
 *
 *************************************************************************/

/*
 * apptFromListEntry
 *
 * Returns the full appointment based on the text of the appointment as
 * given in the text entry from the scrolled list of appointments.
 */
static Appointment*
apptFromListEntry(
        XmString        listEntry)
{
        int             ii;
        char            *entryText,
                        *string;

        /*
         * Get text string from XmString for use in comparisons
         */

	entryText = XmStringUnparse(listEntry, NULL, XmCHARSET_TEXT,
                        XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);

        for (ii = 0; todaysApptList[ii].what != NULL; ii++) {
                string = strstr(entryText, todaysApptList[ii].what);
                if (string != NULL) {
                        XtFree(entryText);
                        return(&todaysApptList[ii]);
                }
        }
        XtFree(entryText);
        return NULL;
}

/*
 * apptGetLabel
 *
 * Creates a label for an appointment icon given an appointment.
 */
static char*
apptGetLabel(
        char            *appt)
{
        char            start[128];
        int             count;

        if (appt == NULL) {
                return NULL;
        }
        appt = strstr(appt, "Start:");
        count = sscanf(appt, "Start:%*[ \t]%[^\n]", start);

        if (count != 1) {
                return NULL;
        }

        return XtNewString(start);
}

