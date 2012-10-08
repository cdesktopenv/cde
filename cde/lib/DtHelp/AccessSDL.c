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
/* $XConsortium: AccessSDL.c /main/13 1996/09/30 11:22:14 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        AccessSDL.c
 **
 **   Project:     Run Time Project File Access
 **
 **   Description: This body of code handles the access routines for the
 **                SDL files.
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
#include <stdio.h>
#include <string.h>

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
#include "bufioI.h"
#include "CleanUpI.h"
#include "CvStringI.h"
#include "FontAttrI.h"
#include "Access.h"
#include "AccessP.h"
#include "AccessSDLP.h"
#include "AccessSDLI.h"
#include "FormatUtilI.h"
#include "FormatSDLI.h"
#include "StringFuncsI.h"
#include "UtilSDLI.h"

#ifdef NLS16
#endif

/********    Private Defines      ********/
/********    End Private Defines  ********/

/********    Private Function Declarations    ********/
static	int	ProcessEntry (
			_DtHelpVolume	 vol,
			_DtCvSegment	*p_seg,
			char		*parent_key);
/********    End Private Function Declarations    ********/

/********    Private Variable Declarations    ********/
static	const	char	*IsoString = "ISO-8859-1";
static	const	CESDLVolume	DefaultSdlVolume =
  {
     NULL,		/* _DtCvSegment *sdl_info; */
     NULL,		/* _DtCvSegment *toss;     */
     NULL,		/* _DtCvSegment *loids;    */
     NULL,		/* _DtCvSegment *index;    */
     NULL,		/* _DtCvSegment *title;    */
     NULL,		/* _DtCvSegment *snb;      */
     0,			/* short      minor_no; */
     False,		/* short      title_processed; */
  };

/********    Private Macro Declarations    ********/

/******************************************************************************
 *                          Private Functions
 ******************************************************************************/
/******************************************************************************
 * Function:	void FreeIds (
 *
 * Parameters:
 *
 * Return Value:
 *
 * errno Values:
 *
 * Purpose:
 *
 ******************************************************************************/
static void
FreeIds (
    _DtCvSegment	*loids)
{
    _DtCvSegment *p_seg;

    if (NULL == loids)
	return;

    p_seg = _DtCvContainerListOfSeg(loids);

    while (NULL != p_seg)
      {
	if (NULL != _SdlSegToSdlIdInfoPtr(p_seg))
	  {
	    if (NULL != _SdlSegToSdlIdInfoRssi(p_seg))
	        free(_SdlSegToSdlIdInfoRssi(p_seg));

	    free(_SdlSegToSdlIdInfoPtr(p_seg));
	  }

	p_seg = p_seg->next_seg;
      }
}

/******************************************************************************
 * Function:	void FreeTossInfo (
 *
 * Parameters:
 *
 * Return Value:
 *
 * errno Values:
 *
 * Purpose:
 *
 ******************************************************************************/
static void
FreeTossInfo (
    _DtCvSegment	*toss)
{
    _DtCvSegment *p_seg;
    SDLTossInfo  *info;

    if (NULL == toss)
	return;

    p_seg = _DtCvContainerListOfSeg(toss);

    while (NULL != p_seg)
      {
	info = (SDLTossInfo *) _SdlSegTossInfo(p_seg);

	/* free the ssi */
	if (NULL != _SdlTossInfoPtrSsi(info))
	    free(_SdlTossInfoPtrSsi(info));

	/* free the colj,colw or the enter, exit data */
	if (NULL != _SdlTossInfoPtrStr1(info))
	    free(_SdlTossInfoPtrStr1(info));
	if (NULL != _SdlTossInfoPtrStr2(info))
	    free(_SdlTossInfoPtrStr2(info));

	/* free the font strings */
	if (NULL != _DtHelpFontHintsColor(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsColor(_SdlTossInfoPtrFontSpecs(info)));
	if (NULL != _DtHelpFontHintsXlfd(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsXlfd(_SdlTossInfoPtrFontSpecs(info)));
	if (NULL != _DtHelpFontHintsXlfdb(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsXlfdb(_SdlTossInfoPtrFontSpecs(info)));
	if (NULL != _DtHelpFontHintsXlfdi(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsXlfdi(_SdlTossInfoPtrFontSpecs(info)));
	if (NULL != _DtHelpFontHintsXlfdib(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsXlfdib(_SdlTossInfoPtrFontSpecs(info)));
	if (NULL != _DtHelpFontHintsTypeNam(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsTypeNam(_SdlTossInfoPtrFontSpecs(info)));
	if (NULL != _DtHelpFontHintsTypeNamb(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsTypeNamb(_SdlTossInfoPtrFontSpecs(info)));
	if (NULL != _DtHelpFontHintsTypeNami(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsTypeNami(_SdlTossInfoPtrFontSpecs(info)));
	if (NULL != _DtHelpFontHintsTypeNamib(_SdlTossInfoPtrFontSpecs(info)))
	    free(_DtHelpFontHintsTypeNamib(_SdlTossInfoPtrFontSpecs(info)));

	free(info);

	p_seg = p_seg->next_seg;
      }
}

/******************************************************************************
 * Function:	void FreeEntryInfo (
 *
 * Parameters:
 *
 * Return Value:
 *
 * errno Values:
 *
 * Purpose:
 *
 ******************************************************************************/
static void
FreeEntryInfo (
    _DtCvSegment	*index)
{
    _DtCvSegment *p_seg;
    SDLEntryInfo  *info;

    if (NULL == index)
	return;

    p_seg = _DtCvContainerListOfSeg(index);

    while (NULL != p_seg)
      {
	info = _SdlSegToSdlEntryInfo(p_seg);

	if (NULL != info)
	  {
	    if (NULL != info->main)
	        free(info->main);
	    if (NULL != info->locs)
	        free(info->locs);
	    if (NULL != info->syns)
	        free(info->syns);
	    if (NULL != info->sort)
	        free(info->sort);
	  }

	if (_DtCvIsSegContainer(p_seg))
	    FreeEntryInfo(p_seg);

	free(info);

	p_seg = p_seg->next_seg;
      }
}

/******************************************************************************
 * Function:	int ProcessSubEntries (
 *
 * Parameters:
 *
 * Return Value:
 *
 * errno Values:
 *
 * Purpose:
 *
 ******************************************************************************/
static int
ProcessSubEntries (
    _DtHelpVolume vol,
    _DtCvSegment	*p_seg,
    char	*parent_key)
{
    while (p_seg != NULL)
      {
	/*
	 * the only sub containers of an entry that should have an non-null
	 * internal pointer should be a sub <entry>.
	 */
	if (_DtCvIsSegContainer(p_seg) && NULL != _SdlSegEntryInfo(p_seg)
		&& ProcessEntry(vol, _DtCvContainerListOfSeg(p_seg),
						parent_key) == -1)
	    return -1;

	p_seg = p_seg->next_seg;
      }
    return 0;
}

/******************************************************************************
 * Function:	int AsciiKeyword (
 *
 * Parameters:
 *		p_list		The segment list to process for strings.
 *		parent_str	The string to append information onto.
 *				This may be NULL.
 *		str_size	The malloc'ed size of the parent string.
 *				Includes room for the null byte. If zero
 *				and parent_str is non-null, then memory
 *				must be malloc'ed and parent_str copied
 *				into it. Otherwise, goodStr can just
 *				reuse parent_str.
 *
 * Return Value:
 *
 * errno Values:
 *
 * Purpose:
 *
 ******************************************************************************/
static char * 
AsciiKeyword (
    _DtCvSegment	*p_list,
    char		*parent_str,
    int			*str_size)
{
    int		 len     = 0;
    int		 newLen;
    char	*goodStr;

    /*
     * if a starting string has been passed in, use it.
     */
    if (NULL != parent_str)
      {
	/*
	 * get the actual byte count.
	 */
	len = strlen(parent_str) + 1;

	/*
	 * is the starting value zero? If so, we have to copy it.
	 */
	if (0 == *str_size)
	  {
	    parent_str = strdup(parent_str);
	    if (NULL == parent_str)
		return NULL;

	    *str_size = len;
	  }
      }

    /*
     * start with the parent_string
     */
    goodStr = parent_str;

    while (p_list != NULL)
      {
	if (_DtCvIsSegString(p_list))
	  {
	    /*
	     * get the number of characters in the next string.
	     */
	    newLen = _DtCvStrLen(_DtCvStringOfStringSeg(p_list),
						_DtCvIsSegWideChar(p_list));

	    /*
	     * if this is wide char string, multiply the count by
	     * MB_CUR_MAX to get the maximum number of bytes this
	     * string would take.
	     */
	    if (_DtCvIsSegWideChar(p_list))
		newLen = newLen * MB_CUR_MAX;

	    /*
	     * now add it to our previous size.
	     */
	    len += newLen;

	    /*
	     * are we starting from scratch?
	     */
	    if (goodStr == NULL)
	      {
		/*
		 * include a byte for the end-of-string character.
		 */
		len++;

		/*
		 * malloc the memory
		 */
		goodStr = (char *) malloc (len);

	      }
	    else if (*str_size < len) /* does this have to grow? */
		goodStr = (char *) realloc (goodStr, len);

	    if (goodStr == NULL)
		return NULL;

	    /*
	     * remember the absolute size of the memory for the string
	     */
	    if (*str_size < len)
		*str_size = len;

	    if (_DtCvIsSegWideChar(p_list))
	      {
		/*
		 * back up to the insertion point.
		 */
		len -= newLen;

		/*
		 * transfer
		 */
		newLen = wcstombs(&goodStr[len - 1],
				(wchar_t *) _DtCvStringOfStringSeg(p_list),
					newLen + 1);
		if ((size_t) -1 == newLen)
		    return NULL;

		len += newLen;
	  }
	    else
	        strcpy(&goodStr[len - newLen - 1],
				(char *) _DtCvStringOfStringSeg(p_list));
	  }

	/*
	 * the only containers in an <entry> that should have a non-null
	 * internal pointer should be a sub <entry>. Therefore, if null,
	 * process since it could be a <key>, <sphrase>, etc.
	 */
	else if (_DtCvIsSegContainer(p_list) &&
					NULL == _SdlSegEntryInfo(p_list))
	  {
	    goodStr = AsciiKeyword(_DtCvContainerListOfSeg(p_list), goodStr,
								str_size);
	    if (goodStr == NULL)
		return NULL;

	    len = strlen(goodStr) + 1;
	  }

	p_list = p_list->next_seg;
      }

    return goodStr;
}

/******************************************************************************
 * Function:	int ProcessLocations (
 *
 * Parameters:
 *
 * Return Value:
 *
 * errno Values:
 *
 * Purpose:
 *
 ******************************************************************************/
static int 
ProcessLocations (
    char	*locs,
    char	***list)
{
    char  **myList = NULL;
    char   *nextLoc;

    while (locs != NULL && *locs != '\0')
      {
	locs = _DtHelpGetNxtToken(locs, &nextLoc);
	if (nextLoc == NULL)
	    return -1;

	if (*nextLoc != '\0')
	  {
	    myList = (char **) _DtHelpCeAddPtrToArray ((void **) myList,
							(void *) nextLoc);
	    if (myList == NULL)
		return -1;
	  }
      }

    *list = myList;
    return 0;
}

/******************************************************************************
 * Function:	int ProcessEntry (_DtHelpVolume vol)
 *
 * Parameters:	vol	Specifies the volume whose keywords need to be
 *			loaded from disk.  Once loaded, they can be 
 *			accessed through the fields of the volume structure.
 *
 * Return Value:	0 if successful, -1 if a failure occurs
 *
 * errno Values:	CEErrorMalloc
 *			CEErrorIllegalDatabaseFile
 *					Specifies that the keyword file is
 *					invalid or corrupt.
 *			CEErrorMissingKeywordsRes
 *					Specifies that the keyword file does
 *					not contain the 'Keywords/keywords'
 *					resource or the resource is NULL
 *
 *
 * Purpose:	Load the keywords associated with a volume.
 *
 ******************************************************************************/
static int 
ProcessEntry (
    _DtHelpVolume	 vol,
    _DtCvSegment	*p_seg,
    char		*parent_key)
{
    int           strSize;
    char	**topics;
    char	 *nextKey = NULL;

    /* Now parse the string into the appropriate arrays.  The string has the 
       following syntax:

	<!ELEMENT entry     - - ((%simple; | #PCDATA)*, entry*)       >
	<!ATTLIST entry         id      ID     #IMPLIED
				main    IDREFS #IMPLIED
				locs    IDREFS #IMPLIED
				syns    IDREFS #IMPLIED
				sort    CDATA  #IMPLIED               >
     */

#define	MAIN_STRINGS	(_SdlSegToSdlEntryInfo(p_seg))->main
#define	LOCS_STRINGS	(_SdlSegToSdlEntryInfo(p_seg))->locs

    while (p_seg != NULL)
      {
	strSize = 0;
	nextKey = AsciiKeyword(_DtCvContainerListOfSeg(p_seg),
							parent_key, &strSize);

	if (nextKey == NULL)
	    return -1;

	/* We have the next keyword.  Hang onto it and add it to the list
	   once we get the array of topics.  We don't add it yet because if
	   there are no topics we want to throw it away.  (Silently ignoring
	   keywords which specify no topics is an undocumented feature.) */

	/* Now get the list of topics. */
	topics = NULL;
	if (NULL != FrmtPrivInfoPtr(p_seg) && NULL != _SdlSegEntryInfo(p_seg)
		&& (ProcessLocations(MAIN_STRINGS, &topics) == -1 ||
			ProcessLocations(LOCS_STRINGS, &topics) == -1))
	  {
	    free(nextKey);
	    return -1;
	  }

	if (topics != NULL)
	  {
	    vol->keywords = (char **) _DtHelpCeAddPtrToArray (
						(void **) vol->keywords,
						(void *) nextKey);
	    vol->keywordTopics = (char ***) _DtHelpCeAddPtrToArray (
						(void **) vol->keywordTopics,
						(void *) topics);
	    /*
	     * If we just malloc'ed ourselves out of existance...
	     * stop here.
	     */
	    if (vol->keywords == 0 || vol->keywordTopics == 0)
	      {
		if (vol->keywords != NULL)
		  {
		    free(nextKey);
		    _DtHelpCeFreeStringArray (vol->keywords);
		    _DtHelpCeFreeStringArray (topics);
		    vol->keywords = NULL;
		  }
		if (vol->keywordTopics)
		  {
		    char ***topicList;

		    for (topicList = vol->keywordTopics; topicList; topicList++)
			_DtHelpCeFreeStringArray (*topicList);
		    free (vol->keywordTopics);
		    vol->keywordTopics = NULL;
		  }
		return -1;
	      }
	  }

	if (_DtCvContainerListOfSeg(p_seg) != NULL &&
	    ProcessSubEntries(vol,_DtCvContainerListOfSeg(p_seg),nextKey) == -1)
	    return -1;

	if (topics == NULL)
	    free (nextKey);

	p_seg = p_seg->next_seg;
      }

    return (0);
}

/******************************************************************************
 * Function:	int MapPath (_DtCvSegment *cur_id, int level, char ***ret_ids)
 *
 * Parameters:
 *
 * Return Value:	0 if successful, -1 if failure.
 *
 * Memory:	The memory returned in ret_ids is owned by the caller.
 *
 * Purpose:	To come up with a path from the top of the volume to the
 *		target id.
 *
 ******************************************************************************/
static int 
MapPath (
    _DtCvSegment  **cur_id,
    _DtCvSegment   *target_el,
    int		    stop_lev,
    int		    lev_cnt,
    int		    hidden_no,
    char	***ret_ids)
{
    _DtCvSegment	*mySeg  = *cur_id;
    int          count  = -1;
    int          myLev;
    SDLIdInfo	*info;

    while (mySeg != NULL)
      {
	/*
	 * Does this match the target id?
	 * And, is the element a child of the current path?
	 */
	info  = _SdlSegToSdlIdInfoPtr(mySeg);
	myLev = _SdlIdInfoPtrRlevel(info);
	if (target_el == mySeg && (myLev == -1 || myLev > stop_lev))
	  {
	    /*
	     * matched the target id.
	     * allocate memory and return.
	     */
	    count = 0;
	    if (_SdlIdInfoPtrType(info) == SdlIdVirpage)
	      {
		count++;
		lev_cnt++;
	      }

	    *ret_ids = (char **) malloc (sizeof(char *) * (lev_cnt + 1));
	    if ((*ret_ids) == NULL)
		return -1;

	    (*ret_ids)[lev_cnt] = NULL;

	    if (_SdlIdInfoPtrType(info) == SdlIdVirpage)
		(*ret_ids)[lev_cnt - 1] = strdup(_DtCvContainerIdOfSeg(mySeg));

	    return count;
	  }
	else if (myLev != -1 && myLev != hidden_no
				&& _SdlIdInfoPtrType(info) == SdlIdVirpage)
	  {
	    char *myId = _DtCvContainerIdOfSeg(mySeg);

	    /*
	     * If we've hit a virpage that is a sibling or an aunt
	     * set the search pointer to this segment (since this
	     * is where we want to start searching again) and return
	     * a negative on the successful search.
	     */
	    if (myLev <= stop_lev)
	      {
		*cur_id = mySeg;
		return -1;
	      }

	    /*
	     * this virpage is a child of mine, so look at it's children
	     * for the target id.
	     */
	    mySeg = mySeg->next_seg;
	    count = MapPath(&mySeg, target_el, myLev, lev_cnt + 1, hidden_no,
				ret_ids);

	    /*
	     * successful response on finding the target id in the virpage's
	     * children. Duplicate the virpage's id string and return to
	     * my parent.
	     */
	    if (count != -1)
	      {
		(*ret_ids)[lev_cnt] = strdup(myId);

		count++;
		return count;
	      }
 	  }
	else /* did not match the target id and is not a virpage
	      * or is a hidden virpage */
	    mySeg = mySeg->next_seg;
      }

    *cur_id = mySeg;
    return -1;
}

/******************************************************************************
 *                          Semi-Private Functions
 ******************************************************************************/
/*******************************************************************************
 * Function:    CESDLVolume *_DtHelpCeGetSdlVolumePtr (_DtHelpVolumeHdl vol);
 *
 * Parameters:  vol     Specifies the loaded volume.
 *
 * Return Value:        0 if successful, -1 if a failure occurs
 *
 * errno Values:        None
 *
 * Purpose:     When the volume is no longer needed, it should be unloaded
 *              with this call.  Unloading it frees the memory (which means
 *              any handles on the volume become invalid.)
 *
 ******************************************************************************/
CESDLVolume *
_DtHelpCeGetSdlVolumePtr (
     _DtHelpVolumeHdl	 volume)
{
    _DtHelpVolume vol = (_DtHelpVolume) volume;

    if (vol != NULL)
	return ((CESDLVolume *) vol->vols.sdl_vol);
    return NULL;
}

/******************************************************************************
 *                          Semi-Public Functions
 ******************************************************************************/
/*******************************************************************************
 * Function:    void _DtHelpCeInitSdlVolume (_DtHelpVolume vol);
 *
 * Parameters:  vol     Specifies the loaded volume.
 *
 * Return Value:        0 if successful, -1 if a failure occurs
 *
 * errno Values:        None
 *
 * Purpose:     When the volume is no longer needed, it should be unloaded
 *              with this call.  Unloading it frees the memory (which means
 *              any handles on the volume become invalid.)
 *
 ******************************************************************************/
void
_DtHelpCeInitSdlVolume (
     _DtHelpVolumeHdl	 volume)
{
    CESDLVolume	*sdlVol = _DtHelpCeGetSdlVolumePtr(volume);

    if (sdlVol != NULL)
	*sdlVol = DefaultSdlVolume;

}

/*******************************************************************************
 * Function:    void _DtHelpCeOpenSdlVolume (_DtHelpVolume vol);
 *
 * Parameters:  vol     Specifies the loaded volume.
 *
 * Return Value:        0 if successful, -1 if a failure occurs
 *
 * errno Values:        None
 *
 * Purpose:     When the volume is no longer needed, it should be unloaded
 *              with this call.  Unloading it frees the memory (which means
 *              any handles on the volume become invalid.)
 *
 ******************************************************************************/
int
_DtHelpCeOpenSdlVolume (
     _DtHelpVolumeHdl	 volume)
{
    CESDLVolume	*sdlVol;
    _DtHelpVolume  vol = (_DtHelpVolume) volume;

    sdlVol  = (CESDLVolume *) calloc (1, sizeof(CESDLVolume));
    if (sdlVol != NULL)
      {
	vol->vols.sdl_vol = (SdlVolumeHandle) sdlVol;
	_DtHelpCeInitSdlVolume(volume);
	if (_DtHelpCeFrmtSdlVolumeInfo(vol->volFile,
					vol, &(vol->check_time)) == 0)
	  {
	    vol->sdl_flag = True;
	    return 0;
	  }

	vol->vols.sdl_vol = NULL;
	free(sdlVol);
      }

    return -1;
}

/*******************************************************************************
 * Function:    void _DtHelpCeCleanSdlVolume (_DtHelpVolume vol);
 *
 * Parameters:  vol     Specifies the loaded volume.
 *
 * Return Value:        0 if successful, -1 if a failure occurs
 *
 * errno Values:        None
 *
 * Purpose:     When the volume is no longer needed, it should be unloaded
 *              with this call.  Unloading it frees the memory (which means
 *              any handles on the volume become invalid.)
 *
 ******************************************************************************/
void
_DtHelpCeCleanSdlVolume (
     _DtHelpVolumeHdl	 volume)
{
    CESDLVolume	*sdlVol = _DtHelpCeGetSdlVolumePtr(volume);

    if (sdlVol != NULL)
      {
	_DtHelpFreeSegments(sdlVol->snb  , _DtCvFALSE, sdlVol->destroy_region,
							sdlVol->client_data);
	_DtHelpFreeSegments(sdlVol->title, _DtCvFALSE, sdlVol->destroy_region,
							sdlVol->client_data);

	/*
	 * free the index information
	 */
	FreeEntryInfo(sdlVol->index);
	_DtHelpFreeSegments(sdlVol->index, _DtCvFALSE, NULL, NULL);

	/*
	 * free the toss information.
	 */
	FreeTossInfo(sdlVol->toss);
	_DtHelpFreeSegments(sdlVol->toss , _DtCvFALSE, NULL, NULL);

	/*
	 * free the ids
	 */
	FreeIds(sdlVol->loids);
	_DtHelpFreeSegments(sdlVol->loids, _DtCvFALSE, NULL, NULL);

	/*
	 * free the document information.
	 */
	if (NULL != _SdlDocInfoPtrLanguage(sdlVol->sdl_info))
	    free(_SdlDocInfoPtrLanguage(sdlVol->sdl_info));

	if (NULL != _SdlDocInfoPtrCharSet(sdlVol->sdl_info))
	    free(_SdlDocInfoPtrCharSet(sdlVol->sdl_info));

	if (NULL != _SdlDocInfoPtrDocId(sdlVol->sdl_info))
	    free(_SdlDocInfoPtrDocId(sdlVol->sdl_info));

	if (NULL != _SdlDocInfoPtrFirstPg(sdlVol->sdl_info))
	    free(_SdlDocInfoPtrFirstPg(sdlVol->sdl_info));

	if (NULL != _SdlDocInfoPtrSdlDtd(sdlVol->sdl_info))
	    free(_SdlDocInfoPtrSdlDtd(sdlVol->sdl_info));

	if (NULL != _SdlDocInfoPtrStamp(sdlVol->sdl_info))
	    free(_SdlDocInfoPtrStamp(sdlVol->sdl_info));

	free(sdlVol->sdl_info);
      }
}

/*******************************************************************************
 * Function:    int _DtHelpCeRereadSdlVolume (_DtHelpVolume vol);
 *
 * Parameters:  vol     Specifies the loaded volume.
 *
 * Return Value:        0 if successful, -1 if a failure occurs
 *
 * errno Values:        None
 *
 * Purpose:     When the volume is no longer needed, it should be unloaded
 *              with this call.  Unloading it frees the memory (which means
 *              any handles on the volume become invalid.)
 *
 ******************************************************************************/
int
_DtHelpCeRereadSdlVolume (
     _DtHelpVolumeHdl	 volume)
{
    _DtHelpCeCleanSdlVolume(volume);
    _DtHelpCeInitSdlVolume(volume);
    if (_DtHelpCeFrmtSdlVolumeInfo(_DtHelpCeGetVolumeName(volume),
					volume, NULL) == 0)
	    return 0;

    return -1;
}

/*******************************************************************************
 * Function:    void _DtHelpCeCloseSdlVolume (_DtHelpVolume vol);
 *
 * Parameters:  vol     Specifies the loaded volume.
 *
 * Return Value:        0 if successful, -1 if a failure occurs
 *
 * errno Values:        None
 *
 * Purpose:     When the volume is no longer needed, it should be unloaded
 *              with this call.  Unloading it frees the memory (which means
 *              any handles on the volume become invalid.)
 *
 ******************************************************************************/
void
_DtHelpCeCloseSdlVolume (
     _DtHelpVolumeHdl	 volume)
{
    CESDLVolume	*sdlVol = _DtHelpCeGetSdlVolumePtr(volume);

    if (sdlVol != NULL)
      {
	_DtHelpCeCleanSdlVolume(volume);
	free(sdlVol);
      }
}

/*****************************************************************************
 * Function: Boolean _DtHelpCeGetSdlHomeTopicId (_DtHelpVolume vol,
 *					char *target_id,
 *					char *ret_name,	int *ret_offset)
 *
 * Parameters:	vol		Specifies the loaded volume
 *		target_id	Specifies target location ID
 *		ret_name 	Returns a null terminated string
 *				containing a fully qualified path to
 *				the file that contains 'target_id'.
 *		ret_offset 	Returns the offset into 'ret_name'
 *				to the topic that contains 'target_id'.
 *
 * Memory own by caller:
 *		ret_name
 *
 * Returns:	True if successful, False if a failure occurs
 *
 * errno Values:	EINVAL		Specifies an invalid parameter was
 *					used.
 *			CEErrorMalloc
 *			CEErrorLocIdNotFound
 *					Specifies that 'locId' was not
 *					found.
 *
 * Purpose:	Find which topic contains a specified locationID.
 *
 *****************************************************************************/
char *
_DtHelpCeGetSdlHomeTopicId (
	_DtHelpVolumeHdl	 volume)
{
    _DtCvSegment *idSegs;
    CESDLVolume  *sdlVol  =  _DtHelpCeGetSdlVolumePtr(volume);

    if (sdlVol->sdl_info != NULL)
      {
	/*
	 * Was the first page topic declared in the header?
	 */
	if (NULL != _SdlDocInfoPtrFirstPg(sdlVol->sdl_info))
	    return (_SdlDocInfoPtrFirstPg(sdlVol->sdl_info));

	/*
	 * have to search the list of ids for the home topic.  This is a
	 * bit of a kludge since we are looking for a specific string in
	 * the rssi.  But this is for backwards compatibility since the
	 * Snapshot release of the help system were released with out
	 * the first-page attribute and relied on _hometopic.
	 *
	 * Plus, first-page is #IMPLIED, which means that the parser
	 * that generated this SDL document does not have to use this
	 * attribute.
	 */
        if (_DtHelpCeGetSdlVolIds(volume, -1, &idSegs) != 0)
	    return NULL;

        while (idSegs != NULL)
          {
	    if (SdlIdVirpage == _SdlSegToSdlIdInfoType(idSegs) &&
		_DtHelpCeStrCaseCmpLatin1(_SdlIdInfoPtrRssi(
						_SdlSegToSdlIdInfoPtr(idSegs)),
				"_hometopic") == 0)
	        return _DtCvContainerIdOfSeg(idSegs);

	    idSegs = idSegs->next_seg;
          }
      }

    return NULL;
}

/*****************************************************************************
 * Function: Boolean _DtHelpCeFindSdlId (_DtHelpVolume vol, char *target_id,
 *					char *ret_name,	int *ret_offset)
 *
 * Parameters:	vol		Specifies the loaded volume
 *		target_id	Specifies target location ID
 *		ret_name 	Returns a null terminated string
 *				containing a fully qualified path to
 *				the file that contains 'target_id'.
 *		ret_offset 	Returns the offset into 'ret_name'
 *				to the topic that contains 'target_id'.
 *
 * Memory own by caller:
 *		ret_name
 *
 * Returns:	True if successful, False if a failure occurs
 *
 * errno Values:	EINVAL		Specifies an invalid parameter was
 *					used.
 *			CEErrorMalloc
 *			CEErrorLocIdNotFound
 *					Specifies that 'locId' was not
 *					found.
 *
 * Purpose:	Find which topic contains a specified locationID.
 *
 *****************************************************************************/
int
_DtHelpCeFindSdlId (
	_DtHelpVolumeHdl	 volume,
	char		*target_id,
	int		 fd,
	char		**ret_name,
	int		*ret_offset )
{
    _DtHelpVolume   vol    = (_DtHelpVolume) volume;
    _DtCvSegment	*pEl;

    pEl = _DtHelpCeMapSdlIdToSegment(volume, target_id, fd);

    if (pEl != NULL)
      {
	if (ret_name != NULL)
	    *ret_name   = strdup(vol->volFile);
	*ret_offset = _SdlIdInfoPtrOffset(_SdlSegToSdlIdInfoPtr(pEl));
	return True;
      }

    return False;
}

/*****************************************************************************
 * Function: int _DtHelpCeGetSdlKeywordList (
 *
 * Parameters:
 *
 * Returns:	0 if successful, -1 if not.
 *
 * errno Values:
 *
 * Purpose:	Get the KeywordList for an SDL volume.
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlKeywordList (
	_DtHelpVolumeHdl	 volume)
{
    CESDLVolume	*sdlVol =  _DtHelpCeGetSdlVolumePtr(volume);

    if (_DtHelpCeGetSdlVolIndex(volume) != 0 || NULL == sdlVol->index
			|| NULL == _DtCvContainerListOfSeg(sdlVol->index))
	return -1;

    return(ProcessEntry(((_DtHelpVolume) volume),
			_DtCvContainerListOfSeg(sdlVol->index), NULL));
}

/*****************************************************************************
 * Function: int _DtHelpCeGetSdlVolumeAsciiAbstract(volume);
 *
 * Parameters:
 *
 * Returns:	0 if successful, -1 if not.
 *
 * errno Values:
 *
 * Purpose:	Get the KeywordList for an SDL volume.
 *
 *****************************************************************************/
char *
_DtHelpCeGetSdlVolumeAsciiAbstract(
    _DtHelpVolumeHdl	volume)
{
    return(_DtHelpCeFrmtSdlVolumeAbstractToAscii(volume));
}

/*****************************************************************************
 * Function: int _DtHelpCeGetSdlIdPath(volume, target_id, ret_ids);
 *
 * Parameters:
 *
 * Returns:	> 0 if successful, -1 if not.
 *
 * Memory:	The memory returned is owned by the caller.
 *
 * Purpose:	Get the list of location ids between the top and the
 *		target_id.
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlIdPath(
    _DtHelpVolumeHdl	   volume,
    char		  *target_id,
    char		***ret_ids)
{
    _DtCvSegment   *idSegs;
    _DtCvSegment   *targetEl;
    int		    hiddenNo = -1;

    targetEl = _DtHelpCeMapSdlIdToSegment(volume, target_id, -1);

    if (targetEl == NULL)
	return -1;

    *ret_ids = NULL;

    if (_DtHelpCeGetSdlVolIds(volume, -1, &idSegs) != 0)
	return 0;

    if (_SdlVolumeMinorNumber(_DtHelpCeGetSdlVolumePtr(volume)) >= SDL_DTD_1_1)
	hiddenNo = 0;

    return (MapPath(&idSegs, targetEl, -1, 0, hiddenNo, ret_ids));
}

/*****************************************************************************
 * Function: _DtCvSegment *_DtHelpCeMapSdlIdToSegment(volume, target_id);
 *
 * Parameters:
 *
 * Returns:	> 0 if successful, -1 if not.
 *
 * errno Values:
 *
 * Purpose:	Get the list of location ids between the top and the
 *		target_id.
 *
 *****************************************************************************/
_DtCvSegment *
_DtHelpCeMapSdlIdToSegment(
    _DtHelpVolumeHdl	   volume,
    const char		  *target_id,
    int			   fd)
{
    int		    underScore = False;
    short	    minorNo;
    _DtCvSegment   *idSegs;
    char	   *idString;
    char	    resStr[128] = "SDL-RESERVED-";

    minorNo = _SdlVolumeMinorNumber(_DtHelpCeGetSdlVolumePtr(volume));

    if (*target_id == '_')
      {
	/*
	 * parsers generating SDL_DTD_1_0 and earlier put the special
	 * access points (_hometopic, _abstract, _copyright, etc.) in
	 * the SSI.
	 */
        if (minorNo < SDL_DTD_1_1)
	    underScore = True;
	else
          {
	    target_id++;
	    strcat(resStr, target_id);
	    target_id = resStr;
          }
      }

    if (_DtHelpCeGetSdlVolIds(volume, fd, &idSegs) != 0)
	return NULL;

    while (idSegs != NULL)
      {
	if (underScore == True)
	    idString = _SdlIdInfoPtrRssi(_SdlSegToSdlIdInfoPtr(idSegs));
	else
	    idString = _DtCvContainerIdOfSeg(idSegs);

	if (idString != NULL &&
			_DtHelpCeStrCaseCmpLatin1(idString, target_id) == 0)
	    return idSegs;

	idSegs = idSegs->next_seg;
      }

    return NULL;
}

/*****************************************************************************
 * Function: int _DtHelpCeMapIdToSdlTopicId(volume, target_id);
 *
 * Parameters:
 *
 * Returns:	> 0 if successful, -1 if not.
 *
 * errno Values:
 *
 * Purpose:	Get the id of the virpage containing the target_id.
 *
 *****************************************************************************/
int
_DtHelpCeMapIdToSdlTopicId(
    _DtHelpVolumeHdl	   volume,
    const char		  *target_id,
    char		 **ret_id)
{
    int 		 found = -1;
    _DtCvSegment	*idList;
    _DtCvSegment	*idSeg;
    SDLIdInfo		*idInfo;

    if (_DtHelpCeGetSdlVolIds(volume, -1, &idList) == 0)
      {
        idSeg = _DtHelpCeMapSdlIdToSegment(volume, target_id, -1);
        if (idSeg != NULL)
          {
	    while (found == -1 && idList != NULL)
	      {
	        idInfo = _SdlSegToSdlIdInfoPtr(idList);
	        if (_SdlIdInfoPtrType(idInfo) == SdlIdVirpage)
	            *ret_id = _DtCvContainerIdOfSeg(idList);

		if (idList == idSeg)
		    found = 0;
		else
		    idList = idList->next_seg;
	      }
	  }
      }

    return found;
}

/*****************************************************************************
 * Function: char * _DtHelpCeGetSdlVolCharSet(volume);
 *
 * Parameters:
 *
 * Returns:	the pointer to the locale string. Null otherwise.
 *
 * errno Values:
 *
 * Purpose:	Get the locale of the volume.
 *
 *****************************************************************************/
const char *
_DtHelpCeGetSdlVolCharSet(
    _DtHelpVolumeHdl	   volume)
{
    const char	   *charSet = IsoString;
    CESDLVolume    *sdlVol  =  _DtHelpCeGetSdlVolumePtr(volume);

    if (sdlVol->sdl_info != NULL &&
			NULL != _SdlDocInfoPtrLanguage(sdlVol->sdl_info))
	charSet = _SdlDocInfoPtrCharSet(sdlVol->sdl_info);

    return charSet;
}

/*****************************************************************************
 * Function: char * _DtHelpCeGetSdlVolLanguage(volume);
 *
 * Parameters:
 *
 * Returns:	the pointer to the language used in the volume.
 *
 * errno Values:
 *
 * Purpose:	Get the locale of the volume.
 *
 *****************************************************************************/
char *
_DtHelpCeGetSdlVolLanguage(
    _DtHelpVolumeHdl	   volume)
{
    char	   *language = "C";
    CESDLVolume    *sdlVol  =  _DtHelpCeGetSdlVolumePtr(volume);

    if (sdlVol->sdl_info != NULL &&
			NULL != _SdlDocInfoPtrLanguage(sdlVol->sdl_info))
	language = _SdlDocInfoPtrLanguage(sdlVol->sdl_info);

    return language;
}

/*****************************************************************************
 * Function: char * _DtHelpCeGetSdlVolumeLocale(volume);
 *
 * Parameters:
 *
 * Returns:	the pointer to the locale string. Null otherwise.
 *
 * errno Values:
 *
 * Purpose:	Get the locale of the volume.
 *
 *****************************************************************************/
char *
_DtHelpCeGetSdlVolumeLocale(
    _DtHelpVolumeHdl	   volume)
{
    int		    langLen;
    char	   *locale;
    char	   *lang;
    const char	   *charSet;

    lang    = _DtHelpCeGetSdlVolLanguage(volume);
    charSet = _DtHelpCeGetSdlVolCharSet(volume);

    langLen = strlen(lang);
    locale  = (char *) malloc (langLen + strlen(charSet) + 2);
    if (locale != NULL)
      {
	strcpy(locale, lang);
	if (langLen != 0 && *charSet != '\0')
	  {
	    locale[langLen++] = '.';
	    strcpy(&(locale[langLen]), charSet);
	  }
      }

    return locale;
}

/*****************************************************************************
 * Function: int _DtHelpCeGetSdlDocStamp(volume, ret_doc, ret_time);
 *
 * Parameters:
 *
 * Returns:	0 if successful, -2 if the volume does not contain
 *		one or the other, -1 if any other failure.
 *
 * Memory:	The Caller owns the memory returned in ret_doc and ret_time.
 *
 * Purpose:	Get the doc id and time stamp of a volume.
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlDocStamp(
    _DtHelpVolumeHdl	   volume,
    char		**ret_doc,
    char		**ret_time)
{
    int		    result    = -1;
    char	   *docId     = NULL;
    char	   *timestamp = NULL;
    CESDLVolume    *sdlVol    =  _DtHelpCeGetSdlVolumePtr(volume);

    if (sdlVol->sdl_info != NULL)
      {
	result = 0;
	if (NULL != _SdlDocInfoPtrDocId(sdlVol->sdl_info))
	    docId = strdup(_SdlDocInfoPtrDocId(sdlVol->sdl_info));
	else
	    result = -2;

	if (NULL != _SdlDocInfoPtrStamp(sdlVol->sdl_info))
	    timestamp = strdup(_SdlDocInfoPtrStamp(sdlVol->sdl_info));
	else
	    result = -2;
      }

    if (ret_doc != NULL)
	*ret_doc = docId;
    if (ret_time != NULL)
	*ret_time = timestamp;

    if (result == 0 && (docId == NULL || timestamp == NULL))
	return -1;

    return result;
}

/*****************************************************************************
 * Function: int _DtHelpCeGetSdlTopicChildren(
 *
 * Parameters:
 *
 * Returns:	pointer to the element, Null otherwise.
 *
 * errno Values:
 *
 * Purpose:	Find the specified element.
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlTopicChildren(
    _DtHelpVolumeHdl	 volume,
    char		*target,
    char		***ret_ids)
{
    int		  done   = False;
    int		 count  = 0;
    int		 segLev;
    _DtCvSegment *idSeg;
    SDLIdInfo    *idInfo;

    /*
     * Find the target id.
     */
    idSeg = _DtHelpCeMapSdlIdToSegment(volume, target, -1);

    /*
     * save this level and start looking for its children at the next seg.
     */
    *ret_ids = NULL;
    if (idSeg != NULL)
      {
	idInfo = _SdlSegToSdlIdInfoPtr(idSeg);
	segLev = _SdlIdInfoPtrRlevel(idInfo) + 1;
	idSeg  = idSeg->next_seg;
      }

    /*
     * process any virpage that has the correct level
     */
    while (idSeg != NULL && done == False)
      {
	idInfo = _SdlSegToSdlIdInfoPtr(idSeg);
	if (_SdlIdInfoPtrType(idInfo) == SdlIdVirpage)
	  {
	    /*
	     * If greater, we're at the next sibling.
	     */
	    if (segLev > _SdlIdInfoPtrRlevel(idInfo))
		done = True;
	    else if (segLev == _SdlIdInfoPtrRlevel(idInfo))
	      {
		*ret_ids = (char **) _DtHelpCeAddPtrToArray( (void **) *ret_ids,
			(void *)(strdup(_DtCvContainerIdOfSeg(idSeg))));
		if ((*ret_ids) == NULL)
		    return -1;

		count++;
	      }
	  }
	idSeg = idSeg->next_seg;
      }

    return count;
}
