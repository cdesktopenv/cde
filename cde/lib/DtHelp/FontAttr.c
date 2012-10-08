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
/* $XConsortium: FontAttr.c /main/8 1996/01/29 12:19:57 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	Font.c
 **
 **   Project:     Text Graphic Display Library
 **
 **   Description: Semi private format utility functions that do not
 **		   require the Display Area, Motif, Xt or X11.
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * system includes
 */
#include <string.h>
#include <stdlib.h>

/*
 * private includes
 */
#include "FontAttrI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
/********    End Public Function Declarations    ********/

/******************************************************************************
 *
 * Private variables and defines.
 *
 *****************************************************************************/
#ifndef	NULL
#define	NULL	0
#endif

static	_DtHelpFontHints	DefaultFontAttrs =
  {
	"C",
	"ISO-8859-1",
	10, 0,				/* pointsz, setsize */
	  NULL,					/* color            */
	  NULL, NULL, NULL, NULL,		/* xlfd strings     */
	  NULL, NULL, NULL, NULL,		/* MS-Win strings   */
	_DtHelpFontStyleSanSerif,
	_DtHelpFontSpacingProp  ,
	_DtHelpFontWeightMedium ,
	_DtHelpFontSlantRoman   ,
	_DtHelpFontSpecialNone  ,
	NULL
  };

/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
static int
HintDuplicate(char **string)
{
    if (NULL != *string)
      {
	*string = strdup(*string);
	if (NULL == *string)
	    return -1;
      }

    return 0;
}

/******************************************************************************
 *
 * Semi Public Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function:	void _DtHelpCeCopyDefFontAttrList (char **font_attr)
 *
 * Parameters:	font_attr	Specifies the font attribute list
 *
 * Return Value: void
 *
 * Purpose:	Initialize a font attribute list to the default.
 *		Sets '_DtMB_LEN_MAX' to the default character size.
 *
 *****************************************************************************/
void
_DtHelpCeCopyDefFontAttrList (_DtHelpFontHints *font_attr )
{
    *font_attr = DefaultFontAttrs;
}

/******************************************************************************
 * Function:	void _DtHelpFreeFontHints (
 *
 * Parameters:	font_hints	Specifies the font hint structure
 *
 * Return Value: void
 *
 * Purpose:	frees the strings in the font structure.
 *
 *****************************************************************************/
void
_DtHelpFreeFontHints (_DtHelpFontHints *font_hints )
{
    /*
     * do the language and charset
     */
    if (NULL != font_hints->language)
        free(font_hints->language);
    if (NULL != font_hints->char_set)
        free(font_hints->char_set);

    /*
     * do the color
     */
    if (NULL != font_hints->color)
        free(font_hints->color);

    /*
     * do the xlfd fonts
     */
    if (NULL != font_hints->xlfd)
        free(font_hints->xlfd);
    if (NULL != font_hints->xlfdb)
        free(font_hints->xlfdb);
    if (NULL != font_hints->xlfdi)
        free(font_hints->xlfdi);
    if (NULL != font_hints->xlfdib)
        free(font_hints->xlfdib);

    /*
     * do the ms-windows fonts
     */
    if (NULL != font_hints->typenam)
        free(font_hints->typenam);
    if (NULL != font_hints->typenamb)
        free(font_hints->typenamb);
    if (NULL != font_hints->typenami)
        free(font_hints->typenami);
    if (NULL != font_hints->typenamib)
        free(font_hints->typenamib);
}

/******************************************************************************
 * Function:	void _DtHelpDupFontHints (
 *
 * Parameters:	font_hints	Specifies the font hint structure
 *
 * Return Value: 0 for successful, -1 if failures.
 *
 * Purpose:	to the dup the strings in the font hint structure.
 *
 *****************************************************************************/
int
_DtHelpDupFontHints (_DtHelpFontHints *font_hints )
{
    int result = 0;

    /*
     * do the language and charset
     */
    if (-1 == HintDuplicate(&(font_hints->language)))
	result = -1;
    if (-1 == HintDuplicate(&(font_hints->char_set)))
	result = -1;

    /*
     * do the color
     */
    if (-1 == HintDuplicate(&(font_hints->color)))
	result = -1;

    /*
     * do the xlfd fonts
     */
    if (-1 == HintDuplicate(&(font_hints->xlfd)))
	result = -1;
    if (-1 == HintDuplicate(&(font_hints->xlfdb)))
	result = -1;
    if (-1 == HintDuplicate(&(font_hints->xlfdi)))
	result = -1;
    if (-1 == HintDuplicate(&(font_hints->xlfdib)))
	result = -1;

    /*
     * do the ms-windows fonts
     */
    if (-1 == HintDuplicate(&(font_hints->typenam)))
	result = -1;
    if (-1 == HintDuplicate(&(font_hints->typenamb)))
	result = -1;
    if (-1 == HintDuplicate(&(font_hints->typenami)))
	result = -1;
    if (-1 == HintDuplicate(&(font_hints->typenamib)))
	result = -1;

    if (-1 == result)
	_DtHelpFreeFontHints(font_hints);

    return result;
}
