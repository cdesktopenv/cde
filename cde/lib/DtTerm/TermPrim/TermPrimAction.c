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
/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermPrimP.h"
#include "TermPrimI.h"
#include "TermPrimData.h"
#include "TermPrimAction.h"
#include "TermPrimFunction.h"

static void
invokeAction(Widget w, char *transmitString, TermFunction function, int count)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    if (KEYBOARD_LOCKED(tpd->keyboardLocked)) {
	/* keyboard locked -- ring the bell...
	 */
	(void) _DtTermPrimBell(w);
    } else if (tpd->transmitFunctions && transmitString) {
	/* transmit functions mode -- transmit them...
	 */
	(void) _DtTermPrimSendInput(w, (unsigned char *) transmitString,
		strlen(transmitString));
    } else {
	/* perform the function...
	 */
	(*function)(w, count, fromAction);

#ifdef	NOTDEF
	/* we need to wait for the scroll to complete before turning
	 * on the cursor...
	 */
	(void) _DtTermPrimScrollComplete(w, True);
#endif	/* NOTDEF */
	(void) _DtTermPrimCursorOn(w);
    }
    return;
}

typedef struct {
    const char *string;
    char value;
} EnumType;

static int
stringToEnum(char *c, EnumType *enumTypes, int numEnumTypes)
{
    int i;

    for (i = 0; i < numEnumTypes; i++) {
	if (!strcmp(enumTypes[i].string, c))
	    return(i);
    }

    return(-1);
}


/*** INSERT CHAR/LINE *********************************************************
 * 
 *     #    #    #   ####   ######  #####    #####
 *     #    ##   #  #       #       #    #     #
 *     #    # #  #   ####   #####   #    #     #
 *     #    #  # #       #  #       #####      #
 *     #    #   ##  #    #  #       #   #      #
 *     #    #    #   ####   ######  #    #     #
 * 
 *                                       #
 *   ####   #    #    ##    #####       #   #          #    #    #  ######
 *  #    #  #    #   #  #   #    #     #    #          #    ##   #  #
 *  #       ######  #    #  #    #    #     #          #    # #  #  #####
 *  #       #    #  ######  #####    #      #          #    #  # #  #
 *  #    #  #    #  #    #  #   #   #       #          #    #   ##  #
 *   ####   #    #  #    #  #    # #        ######     #    #    #  ######
 */

void
_DtTermPrimActionInsert(Widget w, XEvent *event,
	String *params, Cardinal *num_params)
{
    Debug('i', fprintf(stderr,
	    ">>_DtTermPrimActionInsert: not yet implemented\n"));
}



/*** CURSOR MOTION ************************************************************
 * 
 *   ####   #    #  #####    ####    ####   #####
 *  #    #  #    #  #    #  #       #    #  #    #
 *  #       #    #  #    #   ####   #    #  #    #
 *  #       #    #  #####        #  #    #  #####
 *  #    #  #    #  #   #   #    #  #    #  #   #
 *   ####    ####   #    #   ####    ####   #    #
 * 
 * 
 *  #    #   ####    #####     #     ####   #    #
 *  ##  ##  #    #     #       #    #    #  ##   #
 *  # ## #  #    #     #       #    #    #  # #  #
 *  #    #  #    #     #       #    #    #  #  # #
 *  #    #  #    #     #       #    #    #  #   ##
 *  #    #   ####      #       #     ####   #    #
 */

void
_DtTermPrimActionRedrawDisplay(Widget w, XEvent *event,
	String *params, Cardinal *num_params)
{
    (void) invokeAction(w, NULL, _DtTermPrimFuncRedrawDisplay, 1);
    return;
}


void
_DtTermPrimActionReturn(Widget w, XEvent *event,
	String *params, Cardinal *num_params)
{
    Debug('i', fprintf(stderr,
	    ">>_DtTermPrimActionReturn: not yet implemented\n"));
    return;
}

void
_DtTermPrimActionTab(Widget w, XEvent *event,
	String *params, Cardinal *num_params)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    Boolean shiftedTab = False;
    Boolean numericTab = False;
    int i;

    (void) _DtTermPrimSendInput(w, (unsigned char *) "\t", 1);
    return;
}


/*** STRING *******************************************************************
 *  
 *   ####    #####  #####      #    #    #   ####
 *  #          #    #    #     #    ##   #  #    #
 *   ####      #    #    #     #    # #  #  #
 *       #     #    #####      #    #  # #  #  ###
 *  #    #     #    #   #      #    #   ##  #    #
 *   ####      #    #    #     #    #    #   ####
 */

void
_DtTermPrimActionString(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    unsigned char *c;
    unsigned char hexVal;

    /* this is based on the functionality offered by xterm...
     */

    if (*num_params != 1)
	return;

    /* process hex values... */
    if (((*params)[0] == '0') &&
	    (((*params)[1] == 'x') || ((*params)[1] == 'X')) &&
	    ((*params)[2] != '\0')) {
	for (hexVal = 0, c = (unsigned char *) (*params + 2); *c; c++) {
	    hexVal *= 16;
	    *c = tolower(*c);
	    if ((*c >= '0') && (*c <= '9')) {
		hexVal += *c - '0';
	    } else if ((*c >= 'a') && (*c <= 'f')) {
		hexVal += *c - 'a' + 10;
	    } else {
		break;
	    }
	}
	/* if we hit the end of the string, send the hex value... */
	if (*c == '\0') {
	    (void) _DtTermPrimSendInput(w, &hexVal, 1);
	}
    } else {
	(void) _DtTermPrimSendInput(w, (unsigned char *) *params, strlen(*params));
    }
    return;
}
	    

/*** KEYMAP *******************************************************************
 * 
 *  #    #  ######   #   #  #    #    ##    #####
 *  #   #   #         # #   ##  ##   #  #   #    #
 *  ####    #####      #    # ## #  #    #  #    #
 *  #  #    #          #    #    #  ######  #####
 *  #   #   #          #    #    #  #    #  #
 *  #    #  ######     #    #    #  #    #  #
 */

void
_DtTermPrimActionKeymap(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    XtTranslations keymap;
    char buffer[BUFSIZ];
    static XtTranslations original = (XtTranslations) 0;
    static XtResource key_resources[] = {
	{XtNtranslations, XtCTranslations, XtRTranslationTable,
		sizeof(XtTranslations), 0, XtRTranslationTable, (caddr_t) NULL}
    };
    char mapName[BUFSIZ];
    char mapClass[BUFSIZ];

    if (*num_params != 1) {
	return;
    }

    _DtTermProcessLock();
    if (original == (XtTranslations) 0) {
	original = w->core.tm.translations;
    }
    _DtTermProcessUnlock();

    if (!strcmp(params[0], "None")) {
	(void) XtOverrideTranslations(w, original);
	return;
    }

    (void) sprintf(mapName, "%sKeymap", params[0]);
    (void) strcpy(mapClass, mapName);
    mapClass[0] = toupper(mapClass[0]);
    (void) XtGetSubresources(w, (XtPointer) &keymap, mapName, mapClass,
	    key_resources, XtNumber(key_resources), NULL, (Cardinal) 0);

    if (keymap != NULL) {
	(void) XtOverrideTranslations(w, keymap);
    }
    return;
}
