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
/* $XConsortium: VirtFuncs.c /main/8 1995/12/18 16:24:06 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:  VirtFuncs.c
 **
 **   Project: Cde 1.0
 **
 **   Description:
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <stdlib.h>
#include <string.h>

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "CanvasI.h"
#include "VirtFuncsI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/

/*****************************************************************************
 *		Private Defines
 *****************************************************************************/
/*****************************************************************************
 *		Private Variables
 *****************************************************************************/
/*****************************************************************************
 *		Semi-Private Variables
 *****************************************************************************/
/*****************************************************************************
 *		Private Functions
 *****************************************************************************/
/*****************************************************************************
 *		Semi-Public Functions
 *****************************************************************************/
/******************************************************************************
 * Function:    _DtCvUnit _DtCvGetStringWidth (_DtCanvasStruct *canvas,
 *				_DtCvSegment *segment, char *string, int len)
 *
 * Parameters:
 *
 * Returns:
 *
 *****************************************************************************/
_DtCvUnit
_DtCvGetStringWidth (
    _DtCanvasStruct      *canvas,
    _DtCvSegment           *segment,
    void                *string,
    int                  len)
{
    _DtCvUnit	result = -1;
    _DtCvStringInfo strInfo;

    strInfo.string   = string;
    strInfo.byte_len = len;
    strInfo.wc       = _DtCvIsSegWideChar(segment);
    strInfo.font_ptr = _DtCvFontOfStringSeg(segment);

    if (canvas->virt_functions.get_width != NULL)
	result = (*(canvas->virt_functions.get_width)) (
			canvas->client_data, _DtCvSTRING_TYPE,
			(_DtCvPointer) &strInfo);
    if (result < 0)
	result = 0;

    return result;

} /* End _DtCvGetStringWidth */

/******************************************************************************
 * Function:    void _DtCvFontMetrics (_DtCanvasStruct canvas,
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 ******************************************************************************/
void
_DtCvFontMetrics(
    _DtCanvasStruct	*canvas,
    _DtCvPointer	 font_handle,
    _DtCvUnit		*ret_ascent,
    _DtCvUnit		*ret_descent,
    _DtCvUnit		*ret_ave,
    _DtCvUnit		*ret_super_y,
    _DtCvUnit		*ret_sub_y)
{
    if (ret_ascent != NULL)
	*ret_ascent = 0;
    if (ret_descent != NULL)
	*ret_descent = 0;
    if (ret_ave != NULL)
	*ret_ave = 0;
    if (ret_super_y != NULL)
	*ret_super_y = 0;
    if (ret_sub_y != NULL)
	*ret_sub_y = 0;

    if (canvas->virt_functions.get_font_metrics != NULL)
        (*(canvas->virt_functions.get_font_metrics))(
		canvas->client_data, font_handle,
		ret_ascent, ret_descent, ret_ave, ret_super_y, ret_sub_y);

} /* End _DtCvFontMetrics */
