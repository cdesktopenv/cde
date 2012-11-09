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
/* $XConsortium: CleanUp.c /main/10 1996/01/29 12:19:40 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        CleanUp.c
 **
 **   Project:     Cde DtHelp
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
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <stdlib.h>

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"
#include "LinkMgrP.h"

/*
 * private includes
 */
#include "bufioI.h"
#include "CleanUpI.h"
#include "FontAttrI.h"
#include "FormatUtilI.h"
#include "RegionI.h"
#include "XInterfaceI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/

/*****************************************************************************
 *		Private Defines
 *****************************************************************************/
/*****************************************************************************
 *		Private Macros
 *****************************************************************************/
#define	AbbrevStr(x)		(FrmtPrivInfoPtr(x)->abbrev)
#define	DupFlag(x)		(FrmtPrivInfoPtr(x)->dup_flag)
#define	MatchInfo(x)		(FrmtPrivInfoPtr(x)->match_info)
#define	TopFlag(x)		(FrmtPrivInfoPtr(x)->top_block)
/*****************************************************************************
 *		Private Functions
 *****************************************************************************/
static void
CheckFreePrivInfo(_DtCvSegment *seg)
{
    /*
     * free the match information
     */
    if (NULL != MatchInfo(seg))
      {
	SdlMatchData *m = (SdlMatchData *)(MatchInfo(seg));

	if (NULL != m->ssi)
	    free(m->ssi);
	free(m);
      }

    /*
     * free any abbreviation
     */
    if (NULL != AbbrevStr(seg))
	free(AbbrevStr(seg));
}

static void
FreePrivateAndSeg(_DtCvSegment *seg)
{
    if (NULL != seg)
      {
	/*
	 * free the private information block
	 */
	if (NULL != FrmtPrivInfoPtr(seg))
	    free(FrmtPrivInfoPtr(seg));

	/*
	 * free the segment block
	 */
	free(seg);
      }
}

/*****************************************************************************
 *		Semi-Private Variables
 *****************************************************************************/
/*****************************************************************************
 *		Semi-Public Functions
 *****************************************************************************/
/*****************************************************************************
 * Function:	void _DtHelpFreeSegments (_DtCvSegment *seg_list)
 *
 * Parameters:
 *		seg_list	Specifies the Canvas Engine segment list.
 *
 * Returns:	Nothing
 *
 * Purpose:	Free all memory associated with an SDL list.
 *****************************************************************************/
void
_DtHelpFreeSegments (
    _DtCvSegment	*seg_list,
    _DtCvStatus		 unresolved,
    void		(*destroy_region)(),
    _DtCvPointer	 client_data)
{
    int			   i;
    char		   dupFlag;
    char		 **strs;
    _DtCvSegment	 *nextSeg;
    _DtCvSegment	 *topSeg = NULL;
    _DtCvSegment	 *topTab = NULL;
    _DtCvSegment	**tableSeg;

    while (seg_list != NULL)
      {
	dupFlag = DupFlag(seg_list);
	nextSeg = seg_list->next_seg;
	switch (_DtCvPrimaryTypeOfSeg(seg_list))
	  {
	    case _DtCvCONTAINER:
			/*
			 * free the id block.
			 */
			if (False == dupFlag &&
				      NULL != _DtCvContainerIdOfSeg(seg_list))
			    free(_DtCvContainerIdOfSeg(seg_list));

			/*
			 * free the contents of the block
			 */
			_DtHelpFreeSegments(_DtCvContainerListOfSeg(seg_list),
						unresolved,
						destroy_region,
						client_data);
			break;

	    case _DtCvMARKER:
			if (False == dupFlag)
			    free(_DtCvIdOfMarkerSeg(seg_list));
			break;

	    case _DtCvREGION:
			if (False == dupFlag && NULL != destroy_region)
			    (destroy_region)(client_data, 
						_DtCvInfoOfRegionSeg(seg_list));
			break;

	    case _DtCvSTRING:
			if (False == dupFlag)
			  {
			    free(_DtCvStringOfStringSeg(seg_list));
			    if (True == unresolved &&
					NULL != _DtCvFontOfStringSeg(seg_list))
				free(_DtCvFontOfStringSeg(seg_list));
			  }
			break;

	    case _DtCvTABLE:
			if (True == dupFlag)
			    break;

			/*
			 * free the blocks
			 */
			tableSeg = _DtCvCellsOfTableSeg(seg_list);
			while (NULL != tableSeg && NULL != *tableSeg)
			  {
			    /*
			     * free the id block.
			     */
			    if (False == DupFlag(*tableSeg)
				   && NULL != _DtCvContainerIdOfSeg(*tableSeg))
				free(_DtCvContainerIdOfSeg(*tableSeg));

			    /*
			     * free the contents of the block
			     */
			    _DtHelpFreeSegments(
					_DtCvContainerListOfSeg(*tableSeg),
								unresolved,
								destroy_region,
								client_data);
			    tableSeg++;
			  }

			/*
			 * free each of the table cells
			 */
			tableSeg = _DtCvCellsOfTableSeg(seg_list);
			while (NULL != tableSeg && NULL != *tableSeg)
			  {
			    if (True == TopFlag(*tableSeg))
			      {
			        FreePrivateAndSeg(topTab);
				topTab = *tableSeg;
			      }
			    CheckFreePrivInfo(*tableSeg);
			    tableSeg++;
			  }

			FreePrivateAndSeg(topTab);
			topTab = NULL;

			/*
			 * free the list of cells
			 */
			tableSeg = _DtCvCellsOfTableSeg(seg_list);
			free (tableSeg);

			/*
			 * free the row ids.
			 */
			_DtHelpCeFreeStringArray(
					_DtCvCellIdsOfTableSeg(seg_list));

			/*
			 * free the column justification and width.
			 */
			free((void *) _DtCvColJustifyOfTableSeg(seg_list));
			strs = _DtCvColWOfTableSeg(seg_list);
			for (i = 0;
			   NULL != strs && i < _DtCvNumColsOfTableSeg(seg_list);
								i++, strs++)
			    free((void *) *strs);
			if (NULL != _DtCvColWOfTableSeg(seg_list))
			    free((void *) _DtCvColWOfTableSeg(seg_list));

			break;
	  }

	CheckFreePrivInfo(seg_list);
	if (True == TopFlag(seg_list))
	  {
	    FreePrivateAndSeg(topSeg);
	    topSeg = seg_list;
	  }
	seg_list = nextSeg;
      }

    FreePrivateAndSeg(topSeg);

} /* End _DtHelpFreeSegments */

/*****************************************************************************
 * Function:	void _DtHelpDestroyTopicData (
 *					_DtCvSegment *seg_list)
 *
 * Parameters:
 *		seg_list	Specifies the SDL segment list.
 *
 * Returns:	Nothing
 *
 * Purpose:	Free all memory associated with an SDL list.
 *****************************************************************************/
void
_DtHelpDestroyTopicData (
    _DtCvTopicInfo	*topic,
    void		(*destroy_region)(),
    _DtCvPointer	 client_data)
{
    if (NULL != topic)
      {
	/*
	 * free the segment list
	 */
        _DtHelpFreeSegments(topic->seg_list, _DtCvFALSE,
						destroy_region, client_data);

	/*
	 * free the id string
	 */
        if (NULL != topic->id_str)
	    free(topic->id_str);

	/*
	 * free the database
	 */
	_DtLinkDbDestroy(topic->link_data);

	/*
	 * free the structure
	 */
        free(topic);
      }

} /* End _DtHelpDestroyTopicData */
