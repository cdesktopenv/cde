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

#include <Xm/RepType.h>

#include "TermPrimP.h"
#include "TermPrimRepType.h"

static void CvtStringToTerminalSizeDestroy
(
	XtAppContext app,
	XrmValue *to,
	XtPointer converter_data,
	XrmValue *args,
	Cardinal *num_args
);

static void CvtStringToTerminalSizeListDestroy
(
	XtAppContext app,
	XrmValue *to,
	XtPointer converter_data,
	XrmValue *args,
	Cardinal *num_args
);

static Boolean CvtStringToTerminalSize
(
	Display *display,
	XrmValue *args,
	Cardinal *num_args,
	XrmValue *from,
	XrmValue *to,
	XtPointer *converter_data
);

static Boolean CvtStringToTerminalSizeList
(
	Display *display,
	XrmValue *args,
	Cardinal *num_args,
	XrmValue *from,
	XrmValue *to,
	XtPointer *converter_data
);



static String TermEmulationModeStrings[] = {
    "termemulation_hp",
    "termemulation_ansi",
};

static String CharCursorStyleStrings[] = {
    "char_cursor_box",
    "char_cursor_bar",
};

XmRepTypeId _DtTermPrimEmulationMode;
XmRepTypeId _DtTermPrimCharCursorStyle;

void _DtTermPrimInitRepTypes(void)
{
    static Boolean first = True;

    _DtTermProcessLock();
    if (first) {
	/* register our resource converters... */
#ifdef NotDefined
	(void) XtSetTypeConverter(XmRString, DtRDtTermTerminalSize,
		CvtStringToTerminalSize, NULL, 0, XtCacheNone,
		CvtStringToTerminalSizeDestroy);
#endif /* NotDefined */
	(void) XtSetTypeConverter(XmRString, DtRDtTermTerminalSizeList,
		CvtStringToTerminalSizeList, NULL, 0, XtCacheNone,
		CvtStringToTerminalSizeListDestroy);
	(void) XmRepTypeRegister(DtRDtTermEmulationMode,
		TermEmulationModeStrings,
		NULL, (unsigned char ) XtNumber(TermEmulationModeStrings));
	(void) XmRepTypeRegister(DtRDtTermCharCursorStyle,
		CharCursorStyleStrings,
		NULL, (unsigned char ) XtNumber(CharCursorStyleStrings));

	_DtTermPrimEmulationMode = XmRepTypeGetId(DtRDtTermEmulationMode);
	_DtTermPrimCharCursorStyle = XmRepTypeGetId(DtRDtTermCharCursorStyle);

	first = False;
    }
    _DtTermProcessUnlock();

    return;
}

#ifdef NotDefined
static void
CvtStringToTerminalSizeDestroy
(
    XtAppContext app,
    XrmValue *to,
    XtPointer converter_data,
    XrmValue *args,
    Cardinal *num_args
)
{
    (void) XtFree(*((char **) to->addr));
}


static Boolean
CvtStringToTerminalSize
(
    Display *display,
    XrmValue *args,
    Cardinal *num_args,
    XrmValue *from,
    XrmValue *to,
    XtPointer *converter_data
)
{
    char *str;
    static DtTermTerminalSize _size;
    DtTermTerminalSize *size = &_size;
    char *c1;

    str = (char *) from->addr;

    if (to->addr) {
	if (to->size < sizeof(*size)) {
	    to->size = sizeof(*size);
	    return(False);
	}
	size = (DtTermTerminalSize *)(to->addr);
    } else {
	to->addr = (XtPointer) size;
	to->size = sizeof(DtTermTerminalSize);
    }

    /* find the separating 'x'... */
    c1 = strchr(str, 'x');

    /* set default values for rows and columns... */
    size->rows = -1;
    size->columns = -1;

    /* we now have one of 3 possible valid expressions:
     *		<columns>x<rows>	use <columns> and <rows>
     *		<columns>[x]		use <columns> and current rows
     *		x<rows>			use <rows> and current columns
     */
    if (c1 && (c1 == str)) {
	/* "x<rows>", no columns supplied... */
	size->rows = atoi(++c1);
    } else {
	/* we have a column value before the 'x'... */
	size->columns = atoi(str);
	if (c1 && *++c1) {
	    /* we have a an 'x' and a row value after the 'x'... */
	    size->rows = atoi(c1);
	}
    }

    return(True);
}
#endif /* NotDefined */

static void
CvtStringToTerminalSizeListDestroy
(
    XtAppContext app,
    XrmValue *to,
    XtPointer converter_data,
    XrmValue *args,
    Cardinal *num_args
)
{
    DtTermTerminalSizeList *sizeList;

    sizeList = (DtTermTerminalSizeList *) (to->addr);
    if (sizeList->sizes) {
	(void) XtFree((char *) sizeList->sizes);
    }
    (void) XtFree((char *) sizeList);
}

static Boolean
CvtStringToTerminalSizeList
(
    Display *display,
    XrmValue *args,
    Cardinal *num_args,
    XrmValue *from,
    XrmValue *to,
    XtPointer *converter_data
)
{
    char *str;
    static DtTermTerminalSizeList _sizeList;
    DtTermTerminalSizeList *sizeList = &_sizeList;
    char *c1;
    char *c2;
    char *tmpStr;
    int numSizes = 0;
    int sizesSize = 0;
    DtTermTerminalSize *sizes = (DtTermTerminalSize *) 0;

    str = (char *) from->addr;

    if (to->addr) {
	if (to->size < sizeof(*sizeList)) {
	    to->size = sizeof(*sizeList);
	    return(False);
	}
	sizeList = (DtTermTerminalSizeList *)(to->addr);
    } else {
	to->addr = (XtPointer) sizeList;
	to->size = sizeof(DtTermTerminalSizeList);
    }

    sizesSize += 10;
    sizes = (DtTermTerminalSize *) XtRealloc((char *) sizes,
                          sizesSize * sizeof(DtTermTerminalSize));

    for (numSizes = 0, c1 = str; c1 && *c1; numSizes++) {
	if (numSizes + 1 > sizesSize) {
	    sizesSize += 10;
	    sizes = (DtTermTerminalSize *) XtRealloc((char *) sizes,
		    sizesSize * sizeof(DtTermTerminalSize));
	}
	sizes[numSizes].rows = 0;
	sizes[numSizes].columns = 0;
	if (*c1 != 'x') {
	    /* parse off columns... */
	    if ((*c1 >= '0') && (*c1 <= '9')) {
		sizes[numSizes].columns = (short) strtol(c1, &c1, 10);
	    } else {
                /* skip over all garbage up to possible 'x' */
                while (c1 && *c1 && *c1 != 'x' && !isspace(*c1)) {
                    c1++;
                }
	    }
	}

	if (*c1 == 'x') {
	    /* parse off rows... */
	    (void) c1++;
	    if ((*c1 >= '0') && (*c1 <= '9')) {
		sizes[numSizes].rows = (short) strtol(c1, &c1, 10);
	    }
	}

	/* skip over all garbage up to whitespace... */
	while (c1 && *c1 && !isspace(*c1)) {
	    c1++;
	}

	/* skip over whitespace... */
	while (c1 && *c1 && isspace(*c1)) {
	    c1++;
	}

        /* check to see if we actually found any numbers */
        if ( sizes[numSizes].rows ==0 && sizes[numSizes].columns == 0)
           numSizes--;
    }

    /* we can now allocate our returned size array... */
    if ( numSizes <= 0 ) {
       numSizes =1;
       sizes[0].rows = 24;
       sizes[0].columns = 80;
     }
    sizeList->numSizes = numSizes;
    sizeList->sizes = (DtTermTerminalSize *)
	    XtMalloc(numSizes * sizeof(DtTermTerminalSize));
    (void) memcpy(sizeList->sizes, sizes, numSizes *
	    sizeof(DtTermTerminalSize));
    (void) XtFree((char *) sizes);

    return(True);
}
