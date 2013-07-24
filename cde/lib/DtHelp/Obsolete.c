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
/* $XConsortium: Obsolete.c /main/4 1996/05/09 03:44:04 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Obsolete.c
 **
 **   Project:     Run Time Project File Access
 **
 **   Description: This module is for backwards compatibility only.
 **                These internal routines are used by dthelpview,
 **                dthelpgen and dthelpprint.
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
#include <stdlib.h>
#include <unistd.h>

/*
 * Canvas Engine
 */
#include "CanvasP.h"

/*
 * private includes
 */
#include "ObsoleteP.h"
#include "bufioI.h"
#include "Access.h"
#include "AccessI.h"

#ifdef NLS16
#endif

/********    Private Defines      ********/
/********    End Private Defines  ********/

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/
/********    Private Macro Declarations        ********/
/********    End Private Macro Declarations    ********/

/******************************************************************************
*
* Private variables used within this file.
*
*******************************************************************************/
/******************************************************************************
 *                             Private Functions
 ******************************************************************************/
/******************************************************************************
 *                          Semi-Public Functions
 ******************************************************************************/
/******************************************************************************
 * Function:	int _DtHelpCeOpenVolume (char *volFile, _DtHelpVolume *retVol);
 *
 * Parameters:	volFile		Specifies the name of the Help Volume file
 *				to load.
 *
 *		retVol		Returns the handle to the loaded volume.
 *				If a volume is opened several times, the
 *				same handle will be returned each time.
 *
 * Return Value:		0 if successful, -1 if a failure occurred.
 *
 * Purpose:	This function must be called to open a Help Volume file
 *		before any of the information in the volume can be
 *		accessed. 
 *
 * Used by:	dthelpgen 1.0
 *
 ******************************************************************************/
int 
_DtHelpCeOpenVolume (
    CanvasHandle canvas_handle,
    char	*volFile, 
    VolumeHandle  *retVol)
{
    return (_DtHelpOpenVolume(volFile, retVol));
}

/******************************************************************************
 * Function:    int _DtHelpCeCloseVolume (VolumeHandle vol);
 *
 * Parameters:  vol     Specifies the loaded volume.
 *
 * Return Value: 0 if successful, -1 if a failure occurs
 *
 * Purpose:     When the volume is no longer needed, it should be
 *              closed with this call.  If the volume has been opened
 *              several times, closing it will just decrement the
 *              reference count.  When it has been closed as many times
 *              as it was opened, the memory it is using will be freed
 *              and any handles to the volume will be invalid.
 *
 * Used by:	dthelpgen 1.0
 *
 ******************************************************************************/
int
_DtHelpCeCloseVolume (
     CanvasHandle       canvas,
     VolumeHandle       volume)
{
    return(_DtHelpCloseVolume(volume));
}

/*****************************************************************************
 * Function: int _DtHelpCeGetTopicTitle (CanvasHandle canvas_handle,
 *                                      VolumeHandle volume,
 *                                      char *id, char **ret_title)
 *
 * Parameters:  volume          Specifies the volume containing the id.
 *              id              Specifies the id for the topic desired.
 *              ret_title       Returns a null terminated string containing
 *                              the title.
 *
 * Memory own by caller:
 *              ret_title
 *
 * Returns:     0 if successful, -2 if didn't find the id,
 *              otherwise -1.
 *
 * Purpose:     Get the title of a topic.
 *
 *****************************************************************************/
int
_DtHelpCeGetTopicTitle (
    CanvasHandle          canvas,
    VolumeHandle          volume,
    char                 *id,
    char                **ret_title)
{
    return(_DtHelpGetTopicTitle(volume, id, ret_title));
}

/*****************************************************************************
 * Function:    void _DtHelpCeDestroyCanvas (CanvasHandle canvas);
 *
 * Parameters:
 *              canvas          Specifies the handle for the canvas.
 *
 * Returns:     A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:     Create a canvas and attach the appropriate virtual functions
 *              to the canvas.
 *****************************************************************************/
void
_DtHelpCeDestroyCanvas (CanvasHandle canvas)
{
    _DtCanvasDestroy(canvas);
}

/*****************************************************************************
 * Function: char * _DtHelpCeGetVolumeLocale (_DtHelpVolume vol)
 *
 * Parameters:	vol		Specifies the loaded volume
 *
 * Returns:	The pointer to the locale string if successful. Otherwise
 *		NULL.
 *
 * Purpose:	Get the locale of the specified volume.
 *		Returns the locale in a unix specific format
 *		- locale[_ter][.charset] - This memory is owned by
 *		the caller.
 *
 *****************************************************************************/
char *
_DtHelpCeGetVolumeLocale (
	VolumeHandle	volume)
{
    return (_DtHelpGetVolumeLocale(volume));

}  /* End _DtHelpCeGetVolumeLocale */
