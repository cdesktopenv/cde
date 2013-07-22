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
/* $XConsortium: GenUtils.c /main/10 1996/05/09 03:42:28 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        GenUtils.c
 **
 **   Project:     CDEnext Info Access.
 **
 **   Description: This module contains non public routines used only by
 **		   CDE 1.0 dthelpgen.  If any of these routines are
 **		   renamed or modified to remove or add parameters, the
 **		   original routine should be moved to Obsolete.c, a new
 **		   routine created (and named differently), and the old
 **		   routine should call the new routine.
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
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "bufioI.h"	/* for FormatUtilI.h */
#include "GenUtilsP.h"
#include "FontAttrI.h"
#include "Access.h"
#include "AccessI.h"
#include "AccessP.h"
#include "AccessSDLP.h"
#include "AccessSDLI.h"
#include "AccessCCDFP.h"
#include "AccessCCDFI.h"
#include "SDLI.h"
#include "FormatUtilI.h"
#include "FormatSDLI.h"
#include "Lock.h"

#ifdef NLS16
#endif

/********    Private Defines      ********/
/********    Private Function Declarations    ********/
static	void		DefCvsMetrics(
				_DtCvPointer		 client_data,
				_DtCvElemType	elem_type,
				_DtCvPointer	ret_metrics);
/********    Private Macro Declarations        ********/

/******************************************************************************
*
* Private variables used within this file.
*
*******************************************************************************/
static _DtCvVirtualInfo	MyVirtInfo =
  {
	DefCvsMetrics,	/* void            (*_CEGet_DtCvMetrics)(); */
	NULL,		/* void            (*_DtCvRenderElem)(); */
	NULL, /* DefCvsStrWidth, _DtCvUnit (*_DtCvGetElemWidth)(); */
	NULL, /* DefCvsFontMetrics, void   (*_DtCvGetFontMetrics)(); */
	NULL,		/* VStatus         (*_DtCvBuildSelection)(); */
  };

static	const _DtCvSpaceMetrics	defLinkMetrics = { 0, 0, 0, 0 };

/******************************************************************************
 *
 * Private functions
 *
 ******************************************************************************/
/*****************************************************************************
 * Function:	void DefCvsMetrics ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	void
DefCvsMetrics (
    _DtCvPointer		 client_data,
    _DtCvElemType	elem_type,
    _DtCvPointer	ret_metrics)
{
    _DtCvSpaceMetrics *retLink = (_DtCvSpaceMetrics *) ret_metrics;

    if (_DtCvCANVAS_TYPE == elem_type)
      {
	_DtCvMetrics *retCanvas = (_DtCvMetrics *) ret_metrics;

        retCanvas->width          = 1;
        retCanvas->height         = 1;
	retCanvas->top_margin     = 0;
	retCanvas->side_margin    = 0;
	retCanvas->line_height    = 1;
	retCanvas->horiz_pad_hint = 1;
      }
    else if (_DtCvLINK_TYPE == elem_type || _DtCvTRAVERSAL_TYPE == elem_type)
	*retLink = defLinkMetrics;
}

/******************************************************************************
 *                          Semi-Public Functions
 ******************************************************************************/
/******************************************************************************
 * Function:	int _DtHelpCeGetVolumeTitle(_DtHelpVolume vol, char **retTitle);
 *
 * Parameters:	vol		Specifies the loaded volume.
 *		retTitle	Returns the title of the volume. This string is
 *				owned by the caller and must be freed.
 *
 * Return Value: 0 if successful, -1 if a failure occurs
 *
 * Purpose:	Get the title of a volume.
 *
 * Used by:	dthelpgen 1.0
 *
 ******************************************************************************/
int 
_DtHelpCeGetVolumeTitle (
    CanvasHandle	  canvas,
    VolumeHandle	  volume,
    char		**ret_title)
{
    _FrmtUiInfo	myUiInfo;
    _DtHelpVolume vol = (_DtHelpVolume)volume;
    int result;

    /*
     * set up my UI information
     */
    myUiInfo.load_graphic = NULL;
    myUiInfo.resolve_spc  = NULL;
    myUiInfo.load_font    = NULL;
    myUiInfo.destroy_region = NULL;
    myUiInfo.exec_filter  = NULL;
    myUiInfo.client_data  = NULL;
    /* since we're going for a string, set avg_char width to 1 */
    myUiInfo.line_width   = 0;
    myUiInfo.line_height  = 0;
    myUiInfo.leading      = 0;
    myUiInfo.avg_char     = 1;
    myUiInfo.nl_to_space  = 0;

    /*
     * What type of volume is it?
     */
    _DtHelpProcessLock();
    if (0 == _DtHelpCeGetVolumeFlag(volume))
      {
        result = _DtHelpCeGetCcdfVolumeTitle(vol, ret_title);
      }
    else
      {
	result = _DtHelpCeFrmtSDLVolTitleToAscii(volume, &myUiInfo, ret_title);
      }
    _DtHelpProcessUnlock();
    return result;
}
/******************************************************************************
 * Function:    int _DtHelpCeGetAsciiVolumeAbstract (_DtHelpVolume vol, char **a
bstract);
 *
 * Parameters:  vol     Specifies the loaded volume.
 *              abstract Returns the abstract of the volume.  This string
 *                       is owned by the caller and should be freed.
 *
 * Return Value: 0 if successful, -1 if a failure occurs
 *
 * Purpose:     Get the abstract of a volume.
 *
 * Used by:	dthelpgen 1.0
 *
 ******************************************************************************/
int
_DtHelpCeGetAsciiVolumeAbstract (
    CanvasHandle          canvas,
    VolumeHandle          volume,
    char                **retAbs)
{
    _DtHelpVolume vol = (_DtHelpVolume)volume;
    int result;

    /*
     * What type of volume is it?
     */
    _DtHelpProcessLock();
    if (0 == _DtHelpCeGetVolumeFlag(volume))
      {
        result = _DtHelpCeGetCcdfVolumeAbstract(vol, retAbs);
	_DtHelpProcessUnlock();
	return result;
      }

    *retAbs = _DtHelpCeGetSdlVolumeAsciiAbstract(volume);
    if (*retAbs == NULL)
      {
	_DtHelpProcessUnlock();
        return (-1);
      }

    _DtHelpProcessUnlock();
    return (0);
}

/*****************************************************************************
 * Function:	CanvasHandle _DtHelpCeCreateDefCanvas (void);
 *
 * Parameters:
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:	Create a canvas and attach the appropriate virtual functions
 *		to the canvas.
 *****************************************************************************/
CanvasHandle
_DtHelpCeCreateDefCanvas (void)
{
    return (_DtCanvasCreate(MyVirtInfo, NULL));

} /* End _DtHelpCeCreateDefCanvas */

/******************************************************************************
 * Function:    char *_DtHelpCeGetNxtToken (char *str, char **retToken)
 *
 * Parameters:
 * Return Value:        Returns the pointer to the next unparsed character in
 *                      the input string. A NULL value indicates an error.
 *
 * errno Values:
 *              EINVAL
 *              CEErrorMalloc
 *
 * Purpose:     Parse tokens in resource string values.
 *
 *****************************************************************************/
char *
_DtHelpCeGetNxtToken (
    char        *str,
    char        **retToken)
{
   return (_DtHelpGetNxtToken(str, retToken));
}
