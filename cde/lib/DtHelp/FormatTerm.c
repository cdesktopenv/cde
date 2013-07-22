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
/* $TOG: FormatTerm.c /main/14 1997/08/07 10:52:34 samborn $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	   FormatTerm.c
 **
 **   Project:     CDE Help System
 **  
 **   Description: This code uses the core engine functionality of the
 **		   the help system to get topics into a form understood
 **		   by ASCII based applications.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <X11/Xos.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"
#include "LinkMgrP.h"

/*
 * private includes
 */
#include "LinkMgrI.h"
#include "HelpTermP.h"
#include "HelpP.h"
#include "HelpI.h"
#include "bufioI.h"
#include "AsciiSpcI.h"
#include "Access.h"
#include "AccessI.h"
#include "CanvasI.h"
#include "CleanUpI.h"
#include "HelpErrorP.h"
#include "FileUtilsI.h"
#include "FontAttrI.h"
#include "StringFuncsI.h"
#include "SDLI.h"
#include "FormatUtilI.h"
#include "FormatSDLI.h"
#include "FormatCCDFI.h"
#include "Lock.h"

#ifdef NLS16
#endif


/********    Private Function Declarations    ********/
static	void		TermMetrics(
				_DtCvPointer		 client_data,
				_DtCvElemType	elem_type,
				_DtCvPointer	ret_metrics);
static	_DtCvUnit	TermStrWidth(
				_DtCvPointer	client_data,
				_DtCvElemType	elem_type,
				_DtCvPointer	data);
static	void		TermStrDraw(
				_DtCvPointer	 client_data,
				_DtCvUnit	 x,
				_DtCvUnit	 y,
				const void	*string,
				int		 byte_len,
				int		 wc,
				_DtCvPointer	 font_ptr,
				_DtCvUnit	 box_x,
				_DtCvUnit	 box_y,
				_DtCvUnit	 box_height,
				int		 link_type,
				_DtCvFlags	 old_flags,
				_DtCvFlags	 new_flags );
#ifdef	XTAG
static	_DtCvStatus		TermFindGraphic(
				_DtCvPointer	 client_data,
				char		*vol_xid,
				char		*topic_xid,
				char		*file_xid,
				char		*format,
				char		*method,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_height,
				_DtCvPointer	*ret_graphic);
#endif
static	void		TermGetFont(
				_DtCvPointer	 client_data,
				char		*lang,
				char		*charset,
				_DtHelpFontHints font_attr,
				_DtCvPointer	*ret_font );
static	void		TermFontMetrics(
				_DtCvPointer	 client_data,
				_DtCvPointer	 font_ptr,
				_DtCvUnit	*ret_ascent,
				_DtCvUnit	*ret_descent,
				_DtCvUnit	*char_width,
				_DtCvUnit	*ret_super,
				_DtCvUnit	*ret_sub);
static	_DtCvStatus	TermResolveSpc(
				_DtCvPointer	 client_data,
				char		*lang,
				char		*charset,
				_DtHelpFontHints	 font_attr,
				char		*spc_symbol,
				_DtCvPointer	*ret_handle,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_height,
				_DtCvUnit	*ret_ascent);
static	void		TermRenderElem(
				_DtCvPointer	client_data,
				_DtCvElemType	elem_type,
				_DtCvUnit	x,
				_DtCvUnit	y,
				int		link_type,
				_DtCvFlags	old_flags,
				_DtCvFlags	new_flags,
				_DtCvElemType	trav_type,
				_DtCvPointer	trav_data,
				_DtCvPointer	data );

/********    End Public Function Declarations    ********/

/******************************************************************************
 *
 * Private variables and defines.
 *
 *****************************************************************************/
#define	GROW_SIZE	10
#define EOS		'\0'

/*
 * My private values
 */
static	_DtHelpVolumeHdl	 MyVolume     = NULL;
static	int		 HyperErr     = 0;
static	wchar_t		 WcSpace      = 0;

/*
 * These values change as the information is processed.
 */
typedef	struct {
	wchar_t **lines;
	size_t   *wc_num;
	int	  max_lines;
	_DtCvUnit	  max_columns;
	_DtCvUnit	  max_rows;
	short		  nl_to_space;
	wchar_t		*cant_begin_chars;
	wchar_t		*cant_end_chars;
} TerminalInfo;

/* default values */
static TerminalInfo	DfltTermInfo   = {NULL, NULL, 0, 1, 100000, 1, NULL, NULL};
static _DtCvVirtualInfo	TermVirtInfo =
  {
	TermMetrics,		/* void      (*_DtCvGetMetrics)(); */
	TermRenderElem,		/* void      (*_DtCvRenderElem)(); */
	TermStrWidth,		/* _DtCvUnit (*_DtCvGetElemWidth)(); */
	TermFontMetrics,	/* void      (*_DtCvGetFontMetrics)(); */
	NULL,			/* _DtCvStatus   (*_DtCvBuildSelection)(); */
	NULL,			/* _DtCvStatus   (*_DtCvFilterExecCmd)(); */
  };

static TerminalInfo	MyInfo   = {NULL, NULL, 0, 1, 100000, 1, NULL, NULL};
static _DtCvHandle	MyCanvas = NULL;

#ifdef  XTAG
int     missingGraphics = 0;  /* Counter used by cdewalk */
#endif  /* XTAG */

/******************************************************************************
 *
 * Private functions
 *
 ******************************************************************************/
/******************************************************************************
 * Canvas functions
 ******************************************************************************/
#ifdef	XTAG
/*****************************************************************************
 * Function:	_DtCvStatus TermFindGraphic ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static _DtCvStatus
TermFindGraphic (
    _DtCvPointer	 client_data,
    char		*vol_xid,
    char		*topic_xid,
    char		*file_xid,
    char		*format,
    char		*method,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_height,
    _DtCvPointer	*ret_graphic )
{
    char		 *ptr;
    char		 *fileName = file_xid;

    if (fileName != NULL && *fileName != '/')
      {
	fileName = (char *) malloc (strlen(vol_xid) + strlen (file_xid) + 2);
	if (fileName == NULL)
	    return -1;

	strcpy(fileName, vol_xid);

	if (_DtHelpCeStrrchr(fileName, "/", MB_CUR_MAX, &ptr) != -1)
	    *ptr = '\0';

	strcat(fileName, "/");
	strcat(fileName, file_xid);
      }

    if (access(fileName, R_OK) != 0)
      {
	missingGraphics++;
	fprintf (stdout, "\tGRAPHICS LINK            ***UNRESOLVED***\n");
	fprintf (stdout, "\tUnable to find graphic file: %s\n\n", fileName);
      }

    if (fileName != file_xid)
	free (fileName);

    *ret_width   = 0;
    *ret_height  = 0;
    *ret_graphic = 0;

    return 0;
}
#endif /* XTAG */

/*****************************************************************************
 * Function:	void TermStrDraw ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	void
TermStrDraw (
    _DtCvPointer client_data,
    _DtCvUnit	 x,
    _DtCvUnit	 y,
    const void	*string,
    int		 byte_len,
    int		 wc,
    _DtCvPointer font_ptr,
    _DtCvUnit	 box_x,
    _DtCvUnit	 box_y,
    _DtCvUnit	 box_height,
    int		 link_type,
    _DtCvFlags	 old_flags,
    _DtCvFlags	 new_flags )
{
    TerminalInfo   *pTerm  = (TerminalInfo *) client_data;
    wchar_t	   *wcStr  = pTerm->lines[y];
    size_t	    length = 1;

    length = x + byte_len;
    if (wcStr == NULL)
      {
	pTerm->lines[y] = (wchar_t *) malloc (sizeof(wchar_t) * (length + 1));
	if (pTerm->lines[y] != NULL)
	  {
	    register int i;

	    wcStr = pTerm->lines[y];
	    _DtHelpProcessLock();
	    for (i = 0; i < x; i++)
		wcStr[i] = WcSpace;
	    _DtHelpProcessUnlock();
	    /*
	     * this will leave a hole that will be plugged by the next 
	     * mbstowc call.
	     */
	    wcStr[length] = 0;
	    pTerm->wc_num[y] = length;
	  }
      }
    else
      {
        if (length > pTerm->wc_num[y])
          {
	    pTerm->lines[y] = (wchar_t *) realloc (wcStr,
					(sizeof(wchar_t) * (length + 1)));
	    if (pTerm->lines[y] != NULL)
	      {
	        register int i;
	        wcStr = pTerm->lines[y];
		_DtHelpProcessLock();
	        for (i = pTerm->wc_num[y]; i < x; i++)
		    wcStr[i] = WcSpace;
		_DtHelpProcessUnlock();
	        wcStr[length] = 0;
	        pTerm->wc_num[y] = length;
	      }
          }
      }

    if (0 == wc)
        mbstowcs(&wcStr[x], string, byte_len);
    else
      {
	wchar_t *wcp = (wchar_t *) string;

	while (0 < byte_len)
	  {
	    wcStr[x++] = *wcp++;
	    byte_len--;
}
      }
}

/*****************************************************************************
 * Function:	void TermRenderElem ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	void
TermRenderElem (
    _DtCvPointer	client_data,
    _DtCvElemType	elem_type,
    _DtCvUnit		x,
    _DtCvUnit		y,
    int			link_type,
    _DtCvFlags		old_flags,
    _DtCvFlags		new_flags,
    _DtCvElemType	trav_type,
    _DtCvPointer	trav_data,
    _DtCvPointer	data)
{
    _DtCvStringInfo *strInfo;
    _DtCvRenderInfo *posInfo = (_DtCvRenderInfo *) data;

    if (_DtCvSTRING_TYPE == elem_type)
      {
	strInfo = (_DtCvStringInfo *) posInfo->info;
        TermStrDraw (client_data, x, y, strInfo->string, strInfo->byte_len,
			strInfo->wc        , strInfo->font_ptr,
			posInfo->box_x     , posInfo->box_y   ,
			posInfo->box_height, link_type,
			old_flags          , new_flags);
      }
    else if (_DtCvREGION_TYPE == elem_type)
      {
	const char *spcStr = (const char*) posInfo->info;
	int len = spcStr ? strlen(spcStr) : 0;

	TermStrDraw (client_data, x, y, spcStr, len, 0, 0,
			posInfo->box_x     , posInfo->box_y,
			posInfo->box_height, link_type,
			old_flags          , new_flags);
      }
}

/*****************************************************************************
 * Function:	void TermMetrics ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	_DtCvSpaceMetrics	defLinkMetrics = { 0, 0, 0, 0 };

static	void
TermMetrics (
    _DtCvPointer		 client_data,
    _DtCvElemType	elem_type,
    _DtCvPointer	ret_metrics)
{
    TerminalInfo *pTerm = (TerminalInfo *) client_data;
    _DtCvSpaceMetrics *retSpace = (_DtCvSpaceMetrics *) ret_metrics;

    if (_DtCvCANVAS_TYPE == elem_type)
      {
	_DtCvMetrics *retCanvas = (_DtCvMetrics *) ret_metrics;

        retCanvas->width          = pTerm->max_columns;
        retCanvas->height         = 50;
	retCanvas->top_margin     = 0;
	retCanvas->side_margin    = 0;
	retCanvas->line_height    = 1;
	retCanvas->horiz_pad_hint = 1;
      }

    else if (_DtCvLOCALE_TYPE == elem_type)
      {
	_DtCvLocale *retLocale = (_DtCvLocale *) ret_metrics;

	retLocale->line_wrap_mode   = _DtCvModeWrapNone;
	retLocale->cant_begin_chars = pTerm->cant_begin_chars;
	retLocale->cant_end_chars   = pTerm->cant_end_chars;
      }
    else if (_DtCvLINK_TYPE == elem_type || _DtCvTRAVERSAL_TYPE == elem_type)
	*retSpace = defLinkMetrics;
}

/*****************************************************************************
 * Function:	void TermGetFont ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	void
TermGetFont (
    _DtCvPointer		 client_data,
    char		*lang,
    char		*charset,
    _DtHelpFontHints	 font_attr,
    _DtCvPointer	*ret_font )
{
    *ret_font = (_DtCvPointer) 0;
}

/*****************************************************************************
 * Function:	_DtCvStatus TermResolveSpc ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	_DtCvStatus
TermResolveSpc (
    _DtCvPointer	 client_data,
    char		*lang,
    char		*charset,
    _DtHelpFontHints	 font_attr,
    char		*spc_symbol,
    _DtCvPointer	*ret_handle,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_height,
    _DtCvUnit		*ret_ascent)
{
    const char*	spcStr;

    /*
     * initialize the returned information to nothing.
     */
    *ret_handle = NULL;
    *ret_height = 0;
    *ret_width  = 0;
    *ret_ascent = 0;

    spcStr = _DtHelpCeResolveSpcToAscii (spc_symbol);

    if (spcStr != NULL)
      {
	*ret_handle = ((_DtCvPointer)(spcStr));
	*ret_height = 1;
	*ret_width  = strlen(spcStr);

      }

    return 0;
}

/*****************************************************************************
 * Function:	void TermFontMetrics ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	void
TermFontMetrics (
    _DtCvPointer	 client_data,
    _DtCvPointer font_ptr,
    _DtCvUnit	*ret_ascent,
    _DtCvUnit	*ret_descent,
    _DtCvUnit	*char_width,
    _DtCvUnit	*ret_super,
    _DtCvUnit	*ret_sub)
{
    if (ret_ascent)
	*ret_ascent = 0;
    if (ret_descent)
	*ret_descent = 0;
    if (char_width)
	*char_width = 0;
    if (ret_super)
	*ret_super = 0;
    if (ret_sub)
	*ret_sub = 0;

    return;
}

/*****************************************************************************
 * Function:	_DtCvUnit TermStrWidth ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	_DtCvUnit
TermStrWidth (
    _DtCvPointer	 client_data,
    _DtCvElemType	elem_type,
    _DtCvPointer	data)
{
   _DtCvStringInfo *strInfo = (_DtCvStringInfo *) data;

   if (elem_type != _DtCvSTRING_TYPE)
	return 0;

   return ((_DtCvUnit)(strInfo->byte_len));
}

/******************************************************************************
 * End Canvas functions
 * Begin other private functions
 ******************************************************************************/

/******************************************************************************
 * Function: DtHelpHyperLines *AddHyperToArray (DtHelpHyperLines *array_ptr,
 *				int value, char *link_spec, char *title)
 *
 * Parameters:
 *		array_ptr	Specifies a NULL terminated list of
 *				DtHelpHyperLines or NULL.
 *		value		Specifies the link type.
 *		link_spec	Specifies the link specification.
 *		title		Specifies the title of the link.
 *
 * Returns:	A ptr to the new DtHelpHyperLines if successful,
 *		NULL if errors.
 *
 * errno Values:
 *		DtErrorMalloc
 *
 * Purpose:	Add a hypertext link to an array of DtHelpHyperLines.
 * 
 *****************************************************************************/
static	DtHelpHyperLines *
AddHyperToArray(
	DtHelpHyperLines *array_ptr,
	int		 value,
	int		 win_hint,
	char		*link_spec,
	char		*title )
{
    DtHelpHyperLines *next;
    int            num = 0;


    if (array_ptr)
      {
	for (next = array_ptr; next->specification != NULL; next++)
		num++;

	if (((num + 1) % GROW_SIZE) == 0)
	    array_ptr = (DtHelpHyperLines *) realloc ((void *) array_ptr,
			(sizeof (DtHelpHyperLines) * (num + 1 + GROW_SIZE)));
      }
    else
	array_ptr = (DtHelpHyperLines *) malloc (
					sizeof (DtHelpHyperLines) * GROW_SIZE);

    if (array_ptr == NULL)
	errno = DtErrorMalloc;
    else
      {
	next = array_ptr + num;

        next->hyper_type    = value;
        next->win_hint      = win_hint;
        next->specification = strdup(link_spec);
        next->title         = title;
        next++;

        next->specification = NULL;
        next->title         = NULL;
      }

    return array_ptr;
}

/******************************************************************************
 * Function:  void DeallocateHyperArray (DtHelpHyperLines *array_ptr)
 *
 * Parameters:
 *		array_ptr	Specifies a NULL terminated array of
 *				DtHelpHyperLines.
 *
 * Returns:	Nothing
 *
 * errno Values:
 *
 * Purpose:	De-allocate an array of DtHelpHyperLines structures.
 *
 *****************************************************************************/
static	void
DeallocateHyperArray(
	DtHelpHyperLines	*array_ptr )
{
    register DtHelpHyperLines *next = array_ptr;

    if (array_ptr)
      {
        while (next->title && next->specification)
	  {
	    free (next->title);
	    free (next->specification);
	    next++;
	  }
	free (array_ptr);
      }
}

/******************************************************************************
 * Function: int AddHyperToList(
 * 
 * Parameters:
 *
 * Returns:	 0 if successful.
 *		-1 if unrecoverable errors.
 *		-2 if could not resolve the hypertext link.
 *
 * errno Values:
 *		DtErrorMalloc
 *		DtErrorFormattingLabel
 *				A label has illegal syntax.
 *		DtErrorHyperType
 *				Invalid (negative) hypertype.
 *		DtErrorFormattingLink
 *				Invalid <LINK>.
 *		DtErrorHyperSpec
 *				Invalid 'hyper_specification' in the
 *				the <LINK>.
 *		DtErrorFormattingId
 *				Invalid <ID> syntax.
 *
 * Purpose:	Process the result of a hypertext link, filling out
 *		a hypertext structure element with the information.
 *			
 *****************************************************************************/
static	int
AddHyperToList(
    _DtHelpVolumeHdl volume,
    _DtCvHandle canvas,
    int		 i,
    DtHelpHyperLines	**ret_list)
{
    _DtCanvasStruct	*myCanvas = (_DtCanvasStruct *)canvas;
    _DtCvLinkInfo	 hyperInfo;
    _DtHelpVolumeHdl     newVol    = NULL;
    _DtHelpVolumeHdl     useVol    = volume;
    char		*volName   = NULL;
    char		*title     = NULL;
    char		*allocName = NULL;
    char		*spec      = NULL;
    char		*id;
    int                 result    = -3;

    if (_DtLinkDbGetLinkInfo(myCanvas->link_data, i,
				myCanvas->virt_functions.exec_cmd_filter,
				myCanvas->client_data, &hyperInfo) == 0)
      {
	result = 0;
	if (hyperInfo.description == NULL)
	  {
	    id = hyperInfo.specification;
	    switch (hyperInfo.hyper_type)
	      {
	        case _DtCvLinkType_CrossLink:
			spec = strdup (id);
			if (spec != NULL)
			  {
			    volName = spec;
                            id = NULL;
                            _DtHelpCeStrchr (spec," ",MB_CUR_MAX,&id);
			    if (id != NULL)
			      {
				*id = '\0';
				id++;

                                /* find the volume (volName is malloc'd) */
                                allocName = _DtHelpFileLocate(DtHelpVOLUME_TYPE, volName,
                                                 _DtHelpFileSuffixList,False,R_OK);
                                if (allocName == NULL) 
                                     result = -2;

				if (_DtHelpOpenVolume(allocName, &newVol)==0)
				    useVol = newVol;
				else
				    result = -2;
			      }
			    else
				id = spec;
			  }
			else
			    result = -1;
                        /* fall thru */
	        case _DtCvLinkType_SameVolume:
			if (result == 0)
			  {
			    result = _DtHelpGetTopicTitle(useVol, id, &title);
			    if (result == -2)
				HyperErr = DtErrorLocIdNotFound;
			  }

			if (newVol != NULL)
			    _DtHelpCloseVolume(newVol);
			if (spec != NULL)
			    free(spec);
			if (allocName != NULL)
			    free(allocName);

			break;

	        case _DtCvLinkType_Execute:
			title = (char *) malloc(strlen(id) + 11);
			sprintf(title, "Execute \"%s\"", id);
			break;

	        case _DtCvLinkType_ManPage:
			title = (char *) malloc(strlen(id) + 13);
			sprintf(title, "Man Page \"%s\"", id);
			break;

	        case _DtCvLinkType_AppDefine:
			title = (char *) malloc(strlen(id) + 26);
		        sprintf(title, "Application Link Type \"%s\"", id);
			break;

	        case _DtCvLinkType_TextFile:
			title = (char *) malloc(strlen(id) + 12);
		        sprintf(title, "Text File \"%s\"", id);
			break;
	        default:
			title = strdup ("Unkown link type");
			break;

	      }
	  }
	else
	  {
	    title = strdup (hyperInfo.description);
	  }
      }

    if (result == -2)
	*ret_list = AddHyperToArray (*ret_list, -(hyperInfo.hyper_type + 1),
				hyperInfo.win_hint,
				hyperInfo.specification,
				strdup("Invalid Link"));
    else if (result >= 0)
	*ret_list = AddHyperToArray (*ret_list, hyperInfo.hyper_type,
				hyperInfo.win_hint,
				hyperInfo.specification,
				title);

    /*
     * report unable to resolve the hypertext link
     */
    if (result == -2)
	return -2;

    if (result == -1 || *ret_list == NULL)
	return -1;

    return (0);

} /* End AddHyperToList */

/******************************************************************************
 * Function: TerminalInfo * GetTermInfo(canvasHandle)
 *
 * Parameters:
 *		canvasHandle	Canvas whose client_data is a TerminalInfo *
 *
 * Returns:	0 for success, -1 for failure.
 *
 ******************************************************************************/
static TerminalInfo * 
GetTermInfo(
           _DtCvHandle canvasHandle)
{
   _DtCanvasStruct * canvas = (_DtCanvasStruct *) canvasHandle;

   return (TerminalInfo *) canvas->client_data;
}

/******************************************************************************
 *
 * Public functions
 *
 ******************************************************************************/
/******************************************************************************
 * Function: int _DtHelpTermCreateCanvas (int maxColumns,_DtCvHandle * ret_canvas)
 *
 * Parameters:
 *		maxColumns	Specifies the column width of the window
 *				for which to format the information.
 *		ret_canvas	handle to the canvas that was created
 *
 * Returns:	0 for success, -1 for failure.
 *
 * errno Values:
 *		EINVAL		'ret_canvas' was NULL or 'maxColumns'
 *				was less than one.
 *              ENOMEM		unable to allocate necessary memory
 *              DtErrorMalloc
 *
 * Purpose:	_DtHelpTermCreateCanvas creates a canvas that use 
 *		text-only content processing routines
 *
 *****************************************************************************/
int
_DtHelpTermCreateCanvas (
	int		  maxColumns,
        _DtCvHandle *    ret_canvas)
{
    TerminalInfo * termInfo;

    /*
     * check the parameters
     */
    if (maxColumns < 1 || ret_canvas == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    termInfo = (TerminalInfo *) malloc(sizeof(TerminalInfo));
    if (termInfo == NULL)
    {
	errno = ENOMEM;
	return -1;
    }

     /* init info and create a canvas */
    *termInfo = DfltTermInfo;
    termInfo->max_columns = maxColumns;
    if (1 < MB_CUR_MAX)
	_DtHelpLoadMultiInfo(&(termInfo->cant_begin_chars),
				&(termInfo->cant_begin_chars),
				&(termInfo->nl_to_space));

    *ret_canvas  = _DtCanvasCreate (TermVirtInfo, (_DtCvPointer) termInfo);

    if (*ret_canvas == NULL)
	    return -1;

    return 0;
}


/******************************************************************************
 * Function: int _DtHelpTermGetTopicData(canvasHandle,volHandle,
 *                                           locationId,helpList,hyperList)
 *
 * Parameters:
 *              canvasHandle    Canvas used to retrieve the info; MUST
 *				be a Terminal canvas Since it isn't easy
 *				to verify this, we don't try.  So if the
 *				Canvas ISN'T a Terminal Canvas, we'll
 *				crash.
 *              volHandle	Help volume to use
 *		locationId	Specifies the locationId of the desired topic.
 *		helpList	Returns a NULL terminated array of
 *				strings.
 *		hyperList	Returns a NULL terminated array of
 *				DtHelpHyperLines containing the hyperlinks
 *				found in the topic.
 *
 * Returns:	0 for success, -1 for failure.
 *
 * errno Values:
 *		EINVAL		'helpVolume', 'locationId', 'helpList',
 *				or 'hyperList' were NULL. 'maxColumns'
 *				was less than one.
 *              open(2)		errno set via an open call on
 *				the file for 'locationId'.
 *              DtErrorMalloc
 *              DtErrorExceedMaxSize
 *				When following symbolic links, the
 *				new path will exceed the system
 *				maximum file path length.
 *              DtErrorIllegalPath
 *				When following symbolic links, the
 *				new path would change to a parent
 *                              directory beyond the beginning
 *                              of the base path.
 *              DtErrorIllegalDatabaseFile
 *                              Specifies that 'helpVolume' is
 *                              an illegal database file.
 *		DtErrorMissingFilenameRes
 *                              Specifies that the 'Filename/filename'
 *                              resource for the topic does not exist.
 *              DtErrorMissingFileposRes
 *                              Specifies that the 'Filepos/filepos'
 *                              resource for the topic does not exist.
 *              DtErrorLocIdNotFound
 *                              Specifies that 'locationId' was not found.
 *		DtErrorFormattingLabel
 *				A label has illegal syntax.
 *		DtErrorHyperType
 *				Invalid (negative) hypertype.
 *		DtErrorFormattingLink
 *				Invalid <LINK>.
 *		DtErrorHyperSpec
 *				Invalid 'hyper_specification' in the
 *				the <LINK>.
 *		DtErrorFormattingId
 *				Invalid <ID> syntax.
 *		DtErrorFormattingTitle
 *				Invalid <TITLE> syntax.
 *
 * Purpose:	_DtHelpTermGetTopicData retrieves Help Files content with
 *		in a form understood by a terminal
 *
 *****************************************************************************/
int
_DtHelpTermGetTopicData(
    _DtCvHandle      canvasHandle,
    _DtHelpVolumeHdl      volHandle,
    char *            locationId,
    char * * *	      helpList,
    DtHelpHyperLines ** hyperList)
{
    int		 result = -1;
    int		 offset;
    _DtCvUnit	 maxRows;
    _DtCvUnit	 maxWidth;
    char * *     strList;
    char*        fileName = NULL;
    _DtHelpCeLockInfo lockInfo;
    _DtCvTopicPtr	 topic;
    TerminalInfo * termInfo;
    _FrmtUiInfo     myUiInfo;

    termInfo = GetTermInfo(canvasHandle);

    _DtHelpProcessLock();
    if (WcSpace == 0)
	mbtowc (&WcSpace, " ", 1);
    _DtHelpProcessUnlock();

    /*
     * find the filename and the Id string.
     */
    if (_DtHelpCeLockVolume(volHandle, &lockInfo) != 0)
      {
	return -1;
      }

    if (_DtHelpCeFindId(volHandle,locationId,lockInfo.fd,&fileName,&offset)==0)
      {
	_DtHelpCeUnlockVolume(lockInfo);
	return -1;
      }

    /*
     * create the ui structure for the parsing.
     */
#ifdef	XTAG
    myUiInfo.load_graphic = TermFindGraphic;
#else
    myUiInfo.load_graphic = NULL;
#endif /* XTAG */
    myUiInfo.resolve_spc  = TermResolveSpc;
    myUiInfo.load_font    = TermGetFont;
    myUiInfo.destroy_region = NULL;
    myUiInfo.exec_filter  = NULL;
    myUiInfo.client_data  = (_DtCvPointer) termInfo;
    myUiInfo.line_width   = 0;
    myUiInfo.line_height  = 1;
    myUiInfo.leading      = 0;
    myUiInfo.avg_char     = 1;
    myUiInfo.nl_to_space  = termInfo->nl_to_space;

    /*
     * Format the topic.
     */
    result = _DtHelpCeGetVolumeFlag(volHandle);

    _DtHelpProcessLock();
    if (result == 1)
	result = _DtHelpCeParseSdlTopic(volHandle,
						&myUiInfo,
						lockInfo.fd,
						offset, NULL, True, &topic);
    else if (result == 0)
	result = _DtHelpCeFrmtCcdfTopic(volHandle, fileName,
						offset, NULL, &myUiInfo,
						&topic);
    _DtHelpProcessUnlock();

    /* if successfully formatted topic */
    if (result != -1)
    {
	int	i;
	int	len;
	wchar_t **wcList;
        _DtCanvasStruct *myCStruct = (_DtCanvasStruct *)canvasHandle;

        /* build the help text list, if requested */
        if (NULL != helpList)
        {
           _DtCanvasSetTopic(canvasHandle, topic, _DtCvIGNORE_BOUNDARY,
						&maxWidth, &maxRows, NULL);
   
	   /*
	    * The 'maxRows' variable is really misnamed; it's really the
	    * 'maxY', and is 0-based.  Thus, the 'lines' and 'wc_num'
	    * arrays need to be 'maxRows+1', in order to hold all the 
	    * entries.  Likewise, 'strList' must be 'maxRows+2', because
	    * it also needs to be NULL terminated.
	    */
           termInfo->lines = (wchar_t **)malloc(sizeof(wchar_t *) *(maxRows+1));
           termInfo->wc_num = (size_t   *)malloc(sizeof(size_t) * (maxRows+1));
           strList          = (char **)   malloc(sizeof(char *) * (maxRows+2));
   	   if (termInfo->lines == NULL || termInfo->wc_num == NULL ||
							strList == NULL)
           {
   	      free(fileName);
	      if (termInfo->lines != NULL)
		free(termInfo->lines);
	      if (termInfo->wc_num != NULL)
		free(termInfo->wc_num);
	      if (strList != NULL)
		free(strList);
   	      _DtHelpCeUnlockVolume(lockInfo);
   	      return -1;
           }
   
   	   for (i = 0; i <= maxRows; i++)
	     {
   	       termInfo->lines[i]  = NULL;
   	       termInfo->wc_num[i] = 0;
	     }
   
   	   _DtCanvasRender(canvasHandle, 0, 0, maxWidth, maxRows,
				_DtCvRENDER_PARTIAL, _DtCvFALSE, NULL, NULL);
   
   	   *helpList = strList;
   	   wcList    = termInfo->lines;
   	   for (i = 0; i <= maxRows; i++, wcList++, strList++)
   	   {
   	      if (*wcList == NULL)
   	      {
   	  	 *strList  = (char *) malloc (1);
   		 **strList = '\0';
   	      }
	      else
	      {
		len      = (termInfo->wc_num[i] + 1) * MB_CUR_MAX;
		*strList = (char *) malloc (sizeof (char) * len);
		if (*strList != NULL)
		    wcstombs(*strList, *wcList, len);
	      }
   	   }
   
	  *strList = NULL;

	   /*
	    * free the allocated memory
	    */
	   for (i = 0, wcList = termInfo->lines; i <= maxRows; i++, wcList++)
	       if (*wcList != NULL)
		   free(*wcList);
	   free(termInfo->lines);
	   free(termInfo->wc_num);

        }  /* if requested help text */

	/*
	 * build the hyperlinks list, if requested
	 */
        if ( NULL != hyperList )
        {
	   *hyperList = NULL;
	   for (i = 0; result != -1 && i < myCStruct->link_data->max; i++)
	       result = AddHyperToList(volHandle, canvasHandle, i, hyperList);
        }
    }  /* if successfully formatted topic */

    _DtCanvasClean(canvasHandle);
    _DtHelpDestroyTopicData(topic, NULL, NULL);

    free(fileName);
    _DtHelpCeUnlockVolume(lockInfo);
    return result;

}  /* End _DtHelpTermGetTopicData */

/******************************************************************************
 * Function: int _DtHelpGetTopicDataHandles(ret_canvasHandle,ret_volHandle)
 *
 * Parameters:
 *		ret_canvasHandle	Canvas used to retrieve the info
 *              ret_volHandle	Help volume in use
 *
 * Returns:
 *  0:  canvas handle & volHandle are not NULL
 * -1:  canvas handle & volHandle are NULL
 ******************************************************************************/
int
_DtHelpGetTopicDataHandles(
    _DtCvHandle *    ret_canvasHandle,
    _DtHelpVolumeHdl *    ret_volHandle)
{
   _DtHelpProcessLock();
   *ret_canvasHandle = MyCanvas;
   *ret_volHandle = MyVolume;
   if (MyCanvas != NULL && MyVolume != NULL)
     {
       _DtHelpProcessUnlock();
       return 0;
     }
   else
     {
       _DtHelpProcessUnlock();
       return -1;
     }
}


/******************************************************************************
 * Function: int _DtHelpGetTopicData (char *helpVolume, char *locationId,
 *		int maxColumns, char ***helpList, DtHelpHyperLines **hyperList)
 *
 * Parameters:
 *		helpVolume	Specifies a file path to the volume.
 *		locationId	Specifies the locationId of the desired topic.
 *		maxColumns	Specifies the column width of the window
 *				for which to format the information.
 *		helpList	Returns a NULL terminated array of
 *				strings.
 *		hyperList	Returns a NULL terminated array of
 *				DtHelpHyperLines containing the hyperlinks
 *				found in the topic.
 *
 * Returns:	0 for success, -1 for failure.
 *
 * errno Values:
 *		EINVAL		'helpVolume', 'locationId', 'helpList',
 *				or 'hyperList' were NULL. 'maxColumns'
 *				was less than one.
 *              getcwd(2) 	errno set via a getcwd call.
 *              readlink(2)     errno set via a readlink call.
 *              open(2)		errno set via an open call on
 *				the file for 'locationId'.
 *              DtErrorMalloc
 *              DtErrorExceedMaxSize
 *				When following symbolic links, the
 *				new path will exceed the system
 *				maximum file path length.
 *              DtErrorIllegalPath
 *				When following symbolic links, the
 *				new path would change to a parent
 *                              directory beyond the beginning
 *                              of the base path.
 *              DtErrorIllegalDatabaseFile
 *                              Specifies that 'helpVolume' is
 *                              an illegal database file.
 *			DtErrorMissingFilenameRes
 *                              Specifies that the 'Filename/filename'
 *                              resource for the topic does not exist.
 *              DtErrorMissingFileposRes
 *                              Specifies that the 'Filepos/filepos'
 *                              resource for the topic does not exist.
 *              DtErrorLocIdNotFound
 *                              Specifies that 'locationId' was not found.
 *		DtErrorFormattingLabel
 *				A label has illegal syntax.
 *		DtErrorHyperType
 *				Invalid (negative) hypertype.
 *		DtErrorFormattingLink
 *				Invalid <LINK>.
 *		DtErrorHyperSpec
 *				Invalid 'hyper_specification' in the
 *				the <LINK>.
 *		DtErrorFormattingId
 *				Invalid <ID> syntax.
 *		DtErrorFormattingTitle
 *				Invalid <TITLE> syntax.
 *
 * Purpose:	_DtHelpGetTopicData formats Help Files with
 *		formatting information
 *		into a form understood by a terminal
 *
 *****************************************************************************/
int
_DtHelpGetTopicData (
	char *               helpVolume,
	char *               locationId,
	int                  maxColumns,
	char * * *           helpList,
	DtHelpHyperLines * * hyperList )
{
    int           result = -1;
    _DtHelpVolumeHdl  volume = NULL;
    char *        path = NULL;

    /* find the volume (path is malloc'd) */
    path = _DtHelpFileLocate(DtHelpVOLUME_TYPE, helpVolume,
                                  _DtHelpFileSuffixList,False,R_OK);
    if (path == NULL) { errno = EINVAL; return -1; }          /* RETURN */

    /* open new canvas or reuse old one with new size */
    _DtHelpProcessLock();
    if (MyCanvas == NULL)
    {
       _DtHelpTermCreateCanvas(maxColumns,&MyCanvas);
       if (MyCanvas == NULL)
       {
          free(path);
	  _DtHelpProcessUnlock();
          return -1;		/* RETURN: errno=from CreateCanvas */
       }
    }
    else
    {
       MyInfo.max_columns = maxColumns;
       _DtCanvasResize (MyCanvas, _DtCvFALSE, NULL, NULL);
    }

    if (_DtHelpOpenVolume (helpVolume, &volume) == -1)
    {
        free(path);
	_DtHelpProcessUnlock();
	return -1;		/* RETURN: errno=from OpenVolume */
    }

    /* release any previously opened volume */
    if (MyVolume)
	_DtHelpCloseVolume (MyVolume);

    /* assign the new volume */
    MyVolume = volume;

    /* get the terminal info */
   result = _DtHelpTermGetTopicData(MyCanvas,MyVolume,locationId,helpList,hyperList);

   _DtHelpProcessUnlock();
   free(path);
   return result;
}

/*****************************************************************************
 * Function: int _DtHelpProcessLinkData (char * ref_volume,DtHelpHyperLines *hyperLine,
 *					char **ret_helpVolume, char **ret_locationId)
 *
 * Parameters:
 *		hyperLine	Specifies the hypertext line that
 *				the caller wishes to be resolved.
 *		helpVolume	Returns the help volume specified by
 *				'hyperLine'.
 *		locationId	Returns the location Id specified by
 *				'hyperLine'.
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *		EINVAL		'hyperLines', 'helpVolume', or 'locationId'
 *				is NULL.
 *		DtErrorMalloc
 *		DtErrorHyperType
 *				The hyper type is not _DtJUMP_REUSE,
 *				_DtJUMP_NEW, or _DtDEFINITION.
 *		DtErrorHyperSpec
 *				Invalid hyper specification.
 *		DtErrorIllegalPath
 *				The volume used by the link spec (either
 *				embedded or the ref_volume) could not be
 *				located.
 *
 * Purpose:	_DtHelpProcessLinkData resolves a hypertext specification
 *		into a pathname to a help volume and a location Id within
 *		the help volume.
 *
 *****************************************************************************/
int
_DtHelpProcessLinkData (
    char *              ref_volume,
    DtHelpHyperLines  * hyperLine,
    char * *            ret_helpVolume,
    char * *            ret_locationId )
{
    char * linkSpec   = NULL;
    char * volumeName = NULL;
    char * idToken    = NULL;

    /* check params */
    if (ref_volume == NULL || hyperLine == NULL ||
			ret_helpVolume == NULL || ret_locationId == NULL)
    {
	errno = EINVAL;
	return -1;
    }

    /* only process SameVolume or CrossVolume links */
    switch (hyperLine->hyper_type)
    {
	case _DtCvLinkType_SameVolume:
	case _DtCvLinkType_CrossLink:
		break;

	default:
		errno = DtErrorHyperType;
		return -1;			/* RETURN */
    }

    /* Initialize the pointers. */
    *ret_helpVolume  = NULL;
    *ret_locationId = NULL;

    /* get working copy of spec */
    linkSpec = strdup (hyperLine->specification);
    if (linkSpec == NULL)
    {
	errno = DtErrorMalloc;
        return -1;
    }

    /* parse the link spec.  Syntax is: "[volume] locationId" */
    if (linkSpec == NULL || *linkSpec == EOS)
    {
        errno = DtErrorHyperSpec;      /* no spec */
    }
    else  /* at least one token exists */
    {
        volumeName = linkSpec;  /* posit that first token is the volume */

        /* look for another possible token */
        idToken = NULL;
        _DtHelpCeStrchr (linkSpec," ",MB_CUR_MAX,&idToken);

        /* second token? */
        if (idToken) /* second token */
        {
            /* separate the tokens and advance idToken to first valid char */
            *idToken = EOS; idToken++;

            /* find the fully qualified volume (volName is malloc'd) */
            volumeName = _DtHelpFileLocate(DtHelpVOLUME_TYPE, volumeName,
                              _DtHelpFileSuffixList,False,R_OK);
            if (volumeName == NULL)
                errno = DtErrorIllegalPath;
        }
        else /* no second token */
        {
	    idToken = volumeName;
            /* find the fully qualified volume (volName is malloc'd) */
            volumeName = _DtHelpFileLocate(DtHelpVOLUME_TYPE, ref_volume,
                              _DtHelpFileSuffixList,False,R_OK);
	    if (volumeName == NULL)
	        errno = DtErrorIllegalPath;
	}

	if (idToken && volumeName)
	{
            *ret_helpVolume = volumeName;  /* already allocd by _DtHelpFileLocate */
            *ret_locationId = strdup (idToken);
	    if (*ret_locationId == NULL)
	        errno = DtErrorMalloc;
        }
    } /* if at least one token exists */

    free (linkSpec);
         
    return ( (*ret_helpVolume && *ret_locationId) ? 0 : -1 );

}  /* End _DtHelpProcessLinkData */

/*****************************************************************************
 * Function: void _DtHelpFreeTopicData (char **helpList,
 *					DtHelpHyperLines *hyperList)
 *
 * Parameters:
 *		helpList	Specifies a pointer to a NULL terminated
 *				array of strings.
 *		hyperList	Specifies a pointer to a NULL terminated
 *				list of DtHelpHyperLines.
 *
 * Returns:	Nothing
 *
 * Purpose:	_DtHelpFreeTopicData frees the data associated with a topic.
 *
 *****************************************************************************/
void
_DtHelpFreeTopicData (
	char	         **helpList,
	DtHelpHyperLines     *hyperList)
{
    if (helpList != NULL)
        _DtHelpCeFreeStringArray (helpList);

    if (hyperList != NULL)
        DeallocateHyperArray (hyperList);

}  /* End _DtHelpFreeTopicData */

/******************************************************************************
 * Function: int _DtHelpGetTopicChildren (char *helpVolume, char *topic_id,
 *		char ***ret_children)
 *
 * Parameters:
 *		helpVolume	Specifies a file path to the volume.
 *		topic_id	Specifies the id of the desired topic.
 *		ret_children	Returns a NULL terminated array of
 *				strings. The memory for these strings
 *				*IS OWNED* by the caller and must be freed
 *				after use.
 *
 * Returns:	> 0 for success, -1 for failure.
 *
 * errno Values:
 *
 * Purpose:	_DtHelpGetTopicChildren returns the topic ids of the children
 *		for a given topic id.
 *
 *****************************************************************************/
int
_DtHelpGetTopicChildren (
	char	         *helpVolume,
	char		 *topic_id,
	char		***ret_children)
{
    int           result = -1;
    _DtHelpVolumeHdl  volume = NULL;
    char *        path;

    /* Initialize the pointer */
    *ret_children  = NULL;

    /* find the volume (path is malloc'd) */
    path = _DtHelpFileLocate(DtHelpVOLUME_TYPE, helpVolume,
                                  _DtHelpFileSuffixList,False,R_OK);
    if (path == NULL) { errno = EINVAL; return -1; }          /* RETURN */

    /* open new canvas or reuse old one with new size */
    _DtHelpProcessLock();
    if (MyCanvas == NULL)
    {
       _DtHelpTermCreateCanvas(72,&MyCanvas);  /* 72: arbitary value for max columns */
       if (MyCanvas == NULL)
       {
          free(path);
	  _DtHelpProcessUnlock();
          return -1;            /* RETURN: errno=??? */
       }
    }

    if (_DtHelpOpenVolume (helpVolume, &volume) == -1)
    {
        free(path);
	_DtHelpProcessUnlock();
        return -1;              /* RETURN: errno=??? */
    }

    /* release any previously opened volume */
    if (MyVolume)
        _DtHelpCloseVolume (MyVolume);

    /* assign the new volume */
    MyVolume = volume;

    /* Get the children */
   result = _DtHelpCeGetTopicChildren(MyVolume, topic_id, ret_children);

   _DtHelpProcessUnlock();
   free(path);
   return result;
}


