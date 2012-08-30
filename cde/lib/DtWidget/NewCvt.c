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
/* $XConsortium: NewCvt.c /main/5 1996/03/26 19:53:53 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990,1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/**---------------------------------------------------------------------
***	
***	file:		NewCvt.c
***
***	project:	Motif Widgets
***
***	description:	Source code for new MotifPlus resource converters.
***	
***-------------------------------------------------------------------*/


/*-------------------------------------------------------------
**	Include Files
*/

#include <Dt/DialogBox.h>
#include <Dt/Icon.h>
#include <Dt/TitleBox.h>
#include "DtWidgetI.h"

#include <ctype.h>

extern void _DtRegisterNewConverters( void ) ;

/********    Static Function Declarations    ********/

static Boolean _StringsAreEqual( 
                        String in_str,
                        String test_str) ;
static void _DtCvtStringToBehavior( 
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;
static void _DtCvtStringToChildType( 
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;
static void _DtCvtStringToFillMode( 
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;
static void _DtCvtStringToPixmapPosition( 
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;
static void _DtCvtStringToStringPosition( 
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;
static void _DtCvtStringToTitlePosition( 
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val) ;

/********    End Static Function Declarations    ********/


/*-------------------------------------------------------------
**	_DtRegisterNewConverters
**		Convert MotifPlus resource converters.
*/
void 
_DtRegisterNewConverters( void )
{
  static Boolean new_converters_registered = False;
  
  if (!new_converters_registered)
    {
      _DtProcessLock();
      if (!new_converters_registered)
	{
	  /* Register new converters. */
	  XtAddConverter (XmRString, XmRBehavior,
			  _DtCvtStringToBehavior, NULL, 0);

	  XtAddConverter (XmRString, XmRChildType,
			  _DtCvtStringToChildType, NULL, 0);

	  XtAddConverter (XmRString, XmRFillMode,
			  _DtCvtStringToFillMode, NULL, 0);

	  XtAddConverter (XmRString, XmRPixmapPosition,
			  _DtCvtStringToPixmapPosition, NULL, 0);

	  XtAddConverter (XmRString, XmRStringPosition,
			  _DtCvtStringToStringPosition, NULL, 0);

	  XtAddConverter (XmRString, XmRTitlePosition,
			  _DtCvtStringToTitlePosition, NULL, 0);

	  new_converters_registered = True;
	}
      _DtProcessUnlock();
    }
}



/*-------------------------------------------------------------
**	_StringsAreEqual
**		Compare possibly mixed case string with lowercase string.
*/
static Boolean 
_StringsAreEqual(
        String in_str,
        String test_str )
{
	int	i, j;

	for (;;)
	{
		i = *in_str;
		j = *test_str;

		if (isupper (i))
			i = tolower (i);
		if (i != j) return (False);
		if (i == 0) return (True);

		in_str++;
		test_str++;
	}
}



/*-------------------------------------------------------------
**	_DtCvtStringToBehavior
**		Convert XmRString to XmRBehavior.
*/
static void 
_DtCvtStringToBehavior(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
	String			string = (String) from_val->addr;
	static unsigned char	value;

	to_val->size = sizeof (unsigned char);
	to_val->addr = (XtPointer) &value;

	if (_StringsAreEqual (string, STR_ICON_LABEL))
		value = XmICON_LABEL;
	else if (_StringsAreEqual (string, STR_ICON_BUTTON))
		value = XmICON_BUTTON;
	else if (_StringsAreEqual (string, STR_ICON_TOGGLE))
		value = XmICON_TOGGLE;
	else if (_StringsAreEqual (string, STR_ICON_DRAG))
		value = XmICON_DRAG;
	else
	{
		to_val->size = 0;
		to_val->addr = NULL;
		XtStringConversionWarning ((String) from_val->addr,
				XmRBehavior);
	}
}



/*-------------------------------------------------------------
**	_DtCvtStringToChildType
**		Convert XmRString to XmRChildType.
*/
static void 
_DtCvtStringToChildType(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
	String			string = (String) from_val->addr;
	static unsigned char	value;

	to_val->size = sizeof (unsigned char);
	to_val->addr = (XtPointer) &value;

	if (_StringsAreEqual (string, STR_WORK_AREA))
		value = XmWORK_AREA;
	else if (_StringsAreEqual (string, STR_TITLE_AREA))
		value = XmTITLE_AREA;
	else if (_StringsAreEqual (string, STR_SEPARATOR))
		value = XmSEPARATOR;
	else if (_StringsAreEqual (string, STR_BUTTON))
		value = XmBUTTON;
	else
	{
		to_val->size = 0;
		to_val->addr = NULL;
		XtStringConversionWarning ((String) from_val->addr,
				XmRChildType);
	}
}



/*-------------------------------------------------------------
**	_DtCvtStringToFillMode
**		Convert XmRString to XmRFillMode.
*/
static void 
_DtCvtStringToFillMode(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
	String			string = (String) from_val->addr;
	static unsigned char	value;

	to_val->size = sizeof (unsigned char);
	to_val->addr = (XtPointer) &value;

	if (_StringsAreEqual (string, STR_FILL_NONE))
		value = XmFILL_NONE;
	else if (_StringsAreEqual (string, STR_FILL_PARENT))
		value = XmFILL_PARENT;
	else if (_StringsAreEqual (string, STR_FILL_SELF))
		value = XmFILL_SELF;
	else if (_StringsAreEqual (string, STR_FILL_TRANSPARENT))
		value = XmFILL_TRANSPARENT;
	else
	{
		to_val->size = 0;
		to_val->addr = NULL;
		XtStringConversionWarning ((String) from_val->addr,
				XmRFillMode);
	}
}



/*-------------------------------------------------------------
**	_DtCvtStringToPixmapPosition
**		Convert string to pixmap position.
*/
static void 
_DtCvtStringToPixmapPosition(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
	String			string = (String) from_val->addr;
	static unsigned char	value;

	to_val->size = sizeof (unsigned char);
	to_val->addr = (XtPointer) &value;

	if (_StringsAreEqual (string, STR_PIXMAP_TOP))
		value = XmPIXMAP_TOP;
	else if (_StringsAreEqual (string, STR_PIXMAP_BOTTOM))
		value = XmPIXMAP_BOTTOM;
	else if (_StringsAreEqual (string, STR_PIXMAP_LEFT))
		value = XmPIXMAP_LEFT;
	else if (_StringsAreEqual (string, STR_PIXMAP_RIGHT))
		value = XmPIXMAP_RIGHT;
	else if (_StringsAreEqual (string, "pixmap_middle"))
		value = XmPIXMAP_MIDDLE;
	else
	{
		to_val->size = 0;
		to_val->addr = NULL;
		XtStringConversionWarning ((String) from_val->addr,
				XmRPixmapPosition);
	}
}



/*-------------------------------------------------------------
**	_DtCvtStringToStringPosition
**		Convert XmRString to XmRStringPosition.
*/
static void 
_DtCvtStringToStringPosition(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
	String			string = (String) from_val->addr;
	static unsigned char	value;

	to_val->size = sizeof (unsigned char);
	to_val->addr = (XtPointer) &value;

	if (_StringsAreEqual (string, STR_STRING_TOP))
		value = XmSTRING_TOP;
	else if (_StringsAreEqual (string, STR_STRING_BOTTOM))
		value = XmSTRING_BOTTOM;
	else if (_StringsAreEqual (string, STR_STRING_LEFT))
		value = XmSTRING_LEFT;
	else if (_StringsAreEqual (string, STR_STRING_RIGHT))
		value = XmSTRING_RIGHT;
	else
	{
		to_val->size = 0;
		to_val->addr = NULL;
		XtStringConversionWarning ((String) from_val->addr,
				XmRStringPosition);
	}
}



/*-------------------------------------------------------------
**	_DtCvtStringToTitlePosition
**		Convert XmRString to XmRTitlePosition.
*/
static void 
_DtCvtStringToTitlePosition(
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val )
{
	String			string = (String) from_val->addr;
	static unsigned char	value;

	to_val->size = sizeof (unsigned char);
	to_val->addr = (XtPointer) &value;

	if (_StringsAreEqual (string, STR_TITLE_TOP))
		value = XmTITLE_TOP;
	else if (_StringsAreEqual (string, STR_TITLE_BOTTOM))
		value = XmTITLE_BOTTOM;
	else
	{
		to_val->size = 0;
		to_val->addr = NULL;
		XtStringConversionWarning ((String) from_val->addr,
				XmRTitlePosition);
	}
}

