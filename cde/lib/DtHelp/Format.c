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
/* $XConsortium: Format.c /main/18 1996/11/22 12:23:59 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	Format.c
 **
 **   Project:     Text Graphic Display Library
 **
 **   Description: This body of formats the input into a form understood by
 **		a Display Area.
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
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <Xm/Xm.h>

#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

/*
 * Core Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "bufioI.h"
#include "CvtToArrayP.h"
#include "DisplayAreaP.h"
#include "FontAttrI.h"
#include "FontI.h"
#include "HelpXlate.h"
#include "XInterfaceI.h"
#include "Lock.h"

/*
 * private core engine
 */
#include "Access.h"
#include "AccessP.h"
#include "AccessI.h"
#include "AccessCCDFI.h"
#include "SDLI.h"
#include "FormatUtilI.h"
#include "FormatCCDFI.h"
#include "FormatSDLI.h"
#include "StringFuncsI.h"


#ifdef NLS16
#endif

/********    Private Function Declarations    ********/

/********    End Public Function Declarations    ********/

/******************************************************************************
 *
 * Private variables and defines.
 *
 *****************************************************************************/
#define	BUFF_SIZE	1024

static	char	*ScanString = "\n\t";
static const _FrmtUiInfo defUiInfo = { NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 1, False };

/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function:	int FormatChunksToXmString ()
 *
 * Parameters:
 *               *ret_list may be NULL when called
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	Take some rich text chunks and turn it into an XmString.
 *
 ******************************************************************************/
static int
FormatChunksToXmString(
    DtHelpDispAreaStruct *pDAS,
    Boolean		  free_flag,
    void		**title_chunks,
    XmString		 *ret_title,
    XmFontList		 *ret_list,
    Boolean		 *ret_mod )
{
    int			 result = 0;
    int			 i;
    long		 j;
    int			 quarkCount;
    long		 chunkType;
    long		 myIdx;
    _DtCvPointer	 fontPtr;
    char		*charSet;
    const char		*strChunk;
    char		 buffer[16];
    _DtHelpFontHints		 fontSpecs;
    XmFontContext	 fontContext;
    XmString		 partTitle;
    XmString		 newTitle;
    XrmQuark		 charSetQuark;
    XrmName		 myCharSetQuarks[20];
    XrmName		 xrmName[_CEFontAttrNumber];
    Boolean		 myMore;

    /*
     * Initialize the pointers.
     */
    *ret_title = NULL;
    *ret_mod   = False;

    if (title_chunks == NULL)
	return -1;

    /*
     * initialize the font context
     */
    _DtHelpCeCopyDefFontAttrList(&fontSpecs);
    if ( NULL != *ret_list )
      {
        if (XmFontListInitFontContext (&fontContext, *ret_list) == False)
            result = -1;
        else 
          {
            XFontStruct *myFontStruct;
            /*
             * quarkize all the character sets found.
             */
            quarkCount = 0;
            do
              {
                myMore = XmFontListGetNextFont (fontContext, &charSet,
                                                                &myFontStruct);
                if (myMore)
                  {
                    myCharSetQuarks[quarkCount++] = 
                                                XrmStringToQuark (charSet);
                    XtFree (charSet);
                  }
              } while (myMore);
    
            XmFontListFreeFontContext (fontContext);
          }
      } /* if NULL != *ret_list */
    else
      { /* if NULL == *ret_list */
         quarkCount = 0;
         myCharSetQuarks[0] = 0;
      }

    /*
     * Build the XrmString based on the segments.
     * The format of the returned information is
     *		'DT_HELP_CE_CHARSET  locale  string'
     *		'DT_HELP_CE_FONT_PTR fontptr string'
     *		'DT_HELP_CE_SPC      spc'
     *		'DT_HELP_CE_STRING   string' - uses last specified
     *                                         charset/font_ptr.
     *
     * The order and manner in which the title_chunks are processed
     * is known and depended upon in several locations.
     * Do not change this without changing the other locations.
     * See the _DtHelpFormatxxx() routines and the ones that
     * create the title_chunk arrays in FormatSDL.c and FormatCCDF.c
     */
    myIdx = __DtHelpDefaultFontIndexGet(pDAS);
    _DtHelpCopyDefaultList(xrmName);
    for (i = 0; result == 0 && title_chunks[i] != DT_HELP_CE_END; i++)
      {
        /*
         * create a string for the char set and a quark for it.
         */
	chunkType = (long) title_chunks[i++];

        /*
	 * i now points to the first value after the type
	 */
	if (chunkType & DT_HELP_CE_CHARSET)
	  {
	    char *charSet;
	    char *lang = (char *) title_chunks[i];

	    /*
	     * test to see if the locale is in a lang.codeset form
	     */
	    if (_DtHelpCeStrchr(lang, ".", 1, &charSet) == 0)
	      {
		*charSet = '\0';
		charSet++;
	      }
	    else
	      {
		charSet = lang;
		lang    = NULL;
	      }

	    /*
	     * resolve/load the font for the default fonts
	     */
	    _DtHelpDAResolveFont(pDAS, lang, charSet, fontSpecs, &fontPtr);
	    myIdx = (long) fontPtr;
	    if (lang != NULL)
	      {
		charSet--;
		*charSet = '.';
	      }

	    if (free_flag)
	        free(title_chunks[i]);

	    /*
	     * move the i to point to the string.
	     */
	    i++;
	  }
	else if (chunkType & DT_HELP_CE_FONT_PTR)
	  {
	    /*
	     * get the default font for the language and code set.
	     */
	    (void) __DtHelpFontCharSetQuarkGet(pDAS, (long)title_chunks[i],
					&xrmName[_DT_HELP_FONT_CHAR_SET]);
	    (void) __DtHelpFontLangQuarkGet(pDAS, (long)title_chunks[i],
					&xrmName[_DT_HELP_FONT_LANG_TER]);
	    (void) __DtHelpFontIndexGet(pDAS, xrmName, &myIdx);

	    /*
	     * move the i to point to the string.
	     */
	    i++;
	  }

        /*
	 * the i point spc or string.
	 */
	if (chunkType & DT_HELP_CE_SPC)
	  {
	    j        = (long) title_chunks[i];
	    strChunk = _DtHelpDAGetSpcString(pDAS->spc_chars[j].spc_idx);
	    fontPtr  = pDAS->spc_chars[j].font_ptr;

	    /*
	     * get the default font for the language and code set.
	     */
	    (void) __DtHelpFontCharSetQuarkGet(pDAS, (long)fontPtr,
					&xrmName[_DT_HELP_FONT_CHAR_SET]);
	    (void) __DtHelpFontLangQuarkGet(pDAS, (long)fontPtr,
					&xrmName[_DT_HELP_FONT_LANG_TER]);
	    (void) __DtHelpFontIndexGet(pDAS, xrmName, &myIdx);
	  }
	else /* if (chunkType & _DT_HELP_CE_STRING) */
	    strChunk = (char *) title_chunks[i];

	sprintf(buffer, "%ld", myIdx);
	charSetQuark = XrmStringToQuark(buffer);

        j = 0;
        while (j < quarkCount && myCharSetQuarks[j] != charSetQuark)
            j++;

        /*
         * If we didn't find a matching character set,
         * add it to the list.
         */
        if (j >= quarkCount)
          {
	    /* Copy the input list so XmFontListAppendEntry can mangle it. */
	    /* But only do it once! */
	    if (False == *ret_mod)
	       *ret_list = XmFontListCopy(*ret_list);

	    if (myIdx < 0)
	      {
		XFontSet fontSet = __DtHelpFontSetGet(pDAS->font_info, myIdx);
                XmFontListEntry fontEntry;

		fontEntry = XmFontListEntryCreate (buffer,
						XmFONT_IS_FONTSET,
						(XtPointer) fontSet);
		*ret_list = XmFontListAppendEntry (*ret_list, fontEntry);
		XmFontListEntryFree (&fontEntry);
	      }
	    else
	      {
		XFontStruct *fontStruct =
				__DtHelpFontStructGet(pDAS->font_info, myIdx);
                XmFontListEntry fontEntry;

		fontEntry = XmFontListEntryCreate (buffer,
						XmFONT_IS_FONT,
						(XtPointer) fontStruct);
		*ret_list = XmFontListAppendEntry (*ret_list, fontEntry);
		XmFontListEntryFree (&fontEntry);
	      }

           *ret_mod = True;
            if (*ret_list == NULL)
                result = -1;

            myCharSetQuarks[quarkCount++] = charSetQuark;
          }

        /*
         * add this segment to the XmString.
         */
        if (result == 0)
          {
            if (*ret_title == NULL)
                *ret_title = XmStringGenerate ((char *) strChunk, buffer, 
					       XmCHARSET_TEXT, NULL);
            else
              {
                partTitle = XmStringGenerate ((char *) strChunk, buffer,
					      XmCHARSET_TEXT, NULL);

                newTitle = XmStringConcat (*ret_title, partTitle);

                XmStringFree (*ret_title);
                XmStringFree (partTitle);
                *ret_title = newTitle;
              }

            /*
             * if a newline was specified,
             * replace it with a blank.
             */
            if (*ret_title != NULL && (chunkType & DT_HELP_CE_NEWLINE))
              {
                partTitle = XmStringGenerate (" ", buffer, XmCHARSET_TEXT, NULL);
                newTitle = XmStringConcat (*ret_title, partTitle);
                XmStringFree (*ret_title);
                XmStringFree (partTitle);
                *ret_title = newTitle;
              }

            if (*ret_title == NULL)
                result = -1;
          }

	if (free_flag && (chunkType & DT_HELP_CE_STRING))
	    free(title_chunks[i]);
      }
    /*
     * deallocate the memory.
     */
    if (free_flag) free(title_chunks);
    return result;
}

/******************************************************************************
 *
 * Semi-Public Functions
 *
 *****************************************************************************/
/*****************************************************************************
 * Function:	int _DtHelpFormatAsciiFile (char *filename,
 *					CEParagraph **ret_para, int *ret_num)
 *
 * Parameters:
 *		filename	Specifies the ascii file to read.
 *		ret_para	Returns a pointer to a list of CEParagraph
 *				structures.
 *		ret_num		Returns the number of structures in 'ret_para'.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *		EINVAL
 *
 * Purpose:	_DtHelpFormatAsciiFile formats Ascii Files into a list of
 *		CEParagraph structures.
 *
 *****************************************************************************/
int
_DtHelpFormatAsciiFile(
	XtPointer	  client_data,
	char		 *filename,
	XtPointer	 *ret_handle)
{
    int	       myFile;
    int	       result = -1;
    _DtHelpFontHints fontAttrs;
    char       buffer [BUFF_SIZE];
    BufFilePtr rawInput;
    XtPointer   varHandle;
    _DtCvTopicPtr	  topic    = NULL;
    DtHelpDispAreaStruct *pDAS     = (DtHelpDispAreaStruct *) client_data;
    _FrmtUiInfo           myUiInfo = defUiInfo;

    /*
     * check the parameters
     */
    if (filename == NULL || ret_handle == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    /*
     * Initialize the pointers, buffers and counters
     */
    *ret_handle  = NULL;

    /*
     * open the file.
     */
    myFile = open (filename, O_RDONLY);
    if (myFile != -1)
      {
	/*
	 * set the information
	 */
	rawInput = _DtHelpCeBufFileRdWithFd(myFile);
	if (rawInput == 0)
	  {
	    close (myFile);
	    return -1;
	  }

        result = _DtHelpCeReadBuf (rawInput, buffer, BUFF_SIZE);

	if (result != -1)
	  {
	    _DtHelpCeCopyDefFontAttrList (&fontAttrs);
	    fontAttrs.spacing = _DtHelpFontSpacingMono;
	    _DtHelpCeXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
				setlocale(LC_CTYPE,NULL), NULL,
				&(fontAttrs.language), &(fontAttrs.char_set));

	    /*
	     * fill out the ui information
	     */
	    myUiInfo.load_font    = _DtHelpDAResolveFont;
	    myUiInfo.client_data  = (_DtCvPointer) pDAS;
	    myUiInfo.line_width   = pDAS->lineThickness;
	    myUiInfo.line_height  = pDAS->lineHeight;
	    myUiInfo.leading      = pDAS->leading;
	    myUiInfo.avg_char     = (int)(pDAS->charWidth / 10 +
					((pDAS->charWidth % 10) ? 1 : 0));
	    myUiInfo.nl_to_space  = pDAS->nl_to_space;

	    /*
	     * get the formatting structure.
	     */
	    varHandle = __DtHelpCeSetUpVars(fontAttrs.language,
						fontAttrs.char_set, &myUiInfo);
	    if (varHandle == NULL)
	      {
	        free(fontAttrs.language);
	        free(fontAttrs.char_set);
		return -1;
	      }

	    result = __DtHelpCeProcessString (varHandle, rawInput,
				_DtCvLITERAL,
				ScanString, buffer, BUFF_SIZE,
				0, False, &fontAttrs);

	    if (result != -1)
		result = __DtHelpCeGetParagraphList (varHandle, True,
						_DtCvLITERAL,
						&topic);

	    free(fontAttrs.language);
	    free(fontAttrs.char_set);
	    free(varHandle);
	  }

	_DtHelpCeBufFileClose(rawInput, True);
      }

    *ret_handle = (XtPointer) topic;
    return result;

}  /* End _DtHelpFormatAsciiFile */

/*****************************************************************************
 * Function:	int _DtHelpFormatAsciiString (char *input_string,
 *					CEParagraph **ret_para, int *ret_num)
 *
 * Parameters:
 *		input_string	Specifies the ascii string to format.
 *		ret_para	Returns a pointer to a list of CEParagraph
 *				structures.
 *		ret_num		Returns the number of structures in 'ret_para'.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	_DtHelpFormatAsciiString formats a string as if it were a
 *		static string - watching for newlines and using them
 *		to force a break in the line.
 *
 *****************************************************************************/
int
_DtHelpFormatAsciiString(
	XtPointer	  client_data,
	char		 *input_string,
	XtPointer	 *ret_handle)
{
    int      result = -1;
    _DtHelpFontHints fontAttrs;
    XtPointer varHandle;
    _DtCvTopicPtr	  topic    = NULL;
    DtHelpDispAreaStruct *pDAS     = (DtHelpDispAreaStruct *) client_data;
    _FrmtUiInfo           myUiInfo = defUiInfo;

    /*
     * check the parameters.
     */
    if (input_string == NULL || ret_handle == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    /*
     * fill out the ui information
     */
    myUiInfo.load_font    = _DtHelpDAResolveFont;
    myUiInfo.client_data  = (_DtCvPointer) pDAS;
    myUiInfo.line_width   = pDAS->lineThickness;
    myUiInfo.line_height  = pDAS->lineHeight;
    myUiInfo.leading      = pDAS->leading;
    myUiInfo.avg_char     = (int)(pDAS->charWidth / 10 +
				    ((pDAS->charWidth % 10) ? 1 : 0));
    myUiInfo.nl_to_space  = pDAS->nl_to_space;

    /*
     * Get the initialized variables
     */
    *ret_handle  = NULL;

    _DtHelpCeCopyDefFontAttrList (&fontAttrs);
    _DtHelpCeXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,setlocale(LC_CTYPE,NULL),
				NULL, &(fontAttrs.language),
				&(fontAttrs.char_set));

    varHandle = __DtHelpCeSetUpVars(fontAttrs.language, fontAttrs.char_set,
						&myUiInfo);
    if (varHandle == NULL)
      {
        free(fontAttrs.language);
        free(fontAttrs.char_set);
	return -1;
      }

    result = __DtHelpCeProcessString (varHandle, NULL,
				_DtCvLITERAL,
				ScanString, input_string,
				strlen(input_string),
				0, False,
				&fontAttrs);

    if (result != -1)
	result = __DtHelpCeGetParagraphList (varHandle, True, _DtCvLITERAL,
								&topic);

    *ret_handle = (XtPointer) topic;

    free(fontAttrs.language);
    free(fontAttrs.char_set);
    free(varHandle);

    return result;

}  /* End _DtHelpFormatAsciiString */

/*****************************************************************************
 * Function:	int _DtHelpFormatAsciiStringDynamic (char *input_string,
 *					CEParagraph **ret_para, int *ret_num)
 *
 * Parameters:
 *		input_string	Specifies the ascii string to format.
 *		ret_para	Returns a pointer to a list of CEParagraph
 *				structures.
 *		ret_num		Returns the number of structures in 'ret_para'.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	_DtHelpFormatAsciiStringDynamic formats a string as if it were
 *		a dynamic string - it uses newline characters to terminate
 *		the current paragraph, not a line.
 *
 *****************************************************************************/
int
_DtHelpFormatAsciiStringDynamic(
	XtPointer	  client_data,
	char		 *input_string,
	XtPointer	 *ret_handle)
{
    int      result = -1;
    _DtHelpFontHints fontAttrs;
    XtPointer varHandle;
    _DtCvTopicPtr	  topic    = NULL;
    DtHelpDispAreaStruct *pDAS     = (DtHelpDispAreaStruct *) client_data;
    _FrmtUiInfo           myUiInfo = defUiInfo;

    /*
     * check the parameters.
     */
    if (input_string == NULL || ret_handle == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    /*
     * fill out the ui information
     */
    myUiInfo.load_font    = _DtHelpDAResolveFont;
    myUiInfo.client_data  = (_DtCvPointer) pDAS;
    myUiInfo.line_width   = pDAS->lineThickness;
    myUiInfo.line_height  = pDAS->lineHeight;
    myUiInfo.leading      = pDAS->leading;
    myUiInfo.avg_char     = (int)(pDAS->charWidth / 10 +
				    ((pDAS->charWidth % 10) ? 1 : 0));
    myUiInfo.nl_to_space  = pDAS->nl_to_space;

    /*
     * Fake the flag and give the string as the input buffer.
     */
    *ret_handle  = NULL;

    _DtHelpCeCopyDefFontAttrList (&fontAttrs);
    _DtHelpCeXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,setlocale(LC_CTYPE,NULL),
				NULL, &(fontAttrs.language),
				&(fontAttrs.char_set));

    varHandle = __DtHelpCeSetUpVars(fontAttrs.language, fontAttrs.char_set,
						&myUiInfo);
    if (varHandle == NULL)
      {
        free(fontAttrs.language);
        free(fontAttrs.char_set);
	return -1;
      }

    result = __DtHelpCeProcessString (varHandle, NULL,
				_DtCvDYNAMIC,
				ScanString, input_string,
				strlen(input_string),
				0, True,
				&fontAttrs);

    if (result != -1)
	result = __DtHelpCeGetParagraphList (varHandle, False, _DtCvDYNAMIC,
								&topic);
    *ret_handle = (XtPointer) topic;

    free(fontAttrs.language);
    free(fontAttrs.char_set);
    free(varHandle);

    return result;

}  /* End _DtHelpFormatAsciiStringDynamic */

/******************************************************************************
 * Function:	int _DtHelpFormatTopicTitle (Display *dpy, _XvhVolume volume,
 *				char *filename, int offset,
 *				XmString *ret_title, XmFontList *ret_list,
 *				Boolean *ret_mod)
 *
 * Parameters:
 *		dpy		Specifies the connection to the server.
 *		volume		Specifies the Help Volume the information
 *				is associated with.
 *		filename	Specifies the file containing the Help Topic
 *				desired.
 *		offset		Specifies the offset into 'filename' to
 *				the Help Topic desired.
 *		ret_title	Returns XmString containing the title.
 *		ret_list	Specifies the current font list being used.
 *				Returns a new font list if new character
 *				sets are added to it.
 *		ret_mod		Returns True if fonts were added to the
 *				font list.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	_DtHelpFormatTopicTitle formats a topic title and associates
 *		the correct font with it.
 *
 ******************************************************************************/
int
_DtHelpFormatTopicTitle(
    XtPointer		  client_data,
    _DtHelpVolumeHdl	  volume_handle,
    char		 *location_id,
    XmString		 *ret_title,
    XmFontList		 *ret_list,
    Boolean		 *ret_mod )
{
    int		 result = -1;
    void	**titleChunks = NULL;
    _FrmtUiInfo           myUiInfo = defUiInfo;
    _DtHelpCeLockInfo	  lockInfo;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * Check the parameters
     */
    if (ret_title == NULL || ret_list == NULL || ret_mod == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    /*
     * lock the volume
     */
    if (_DtHelpCeLockVolume(volume_handle, &lockInfo) != 0)
	return -1;

    /*
     * set up my UI information
     */
    myUiInfo.load_graphic = _DtHelpDALoadGraphic;
    myUiInfo.resolve_spc  = _DtHelpDAResolveSpc;
    myUiInfo.load_font    = _DtHelpDAResolveFont;
    myUiInfo.exec_filter  = pDAS->exec_filter;
    myUiInfo.destroy_region = _DtHelpDADestroyRegion;
    myUiInfo.client_data  = (_DtCvPointer) pDAS;
    /* since we're going for chunks, set avg_char width to 1 */
    myUiInfo.line_width   = 0;
    myUiInfo.line_height  = 0;
    myUiInfo.leading      = 0;
    myUiInfo.avg_char     = 1;
    myUiInfo.nl_to_space  = pDAS->nl_to_space;

    /*
     * Get the title and charsets associated with the title segments.
     * The format of the returned information is
     *		[type,charset/fontptr,]type,string/spc
     */
    result = _DtHelpCeGetVolumeFlag(volume_handle);
    _DtHelpProcessLock();
    if (result == 1)
        result = _DtHelpCeGetSdlTitleChunks(volume_handle, location_id,
				&myUiInfo, &titleChunks);
    else if (result == 0)
        result = _DtHelpCeGetCcdfTitleChunks( volume_handle, location_id,
				&myUiInfo, &titleChunks);
    _DtHelpProcessUnlock();
    if (result != -1)
        result = FormatChunksToXmString(pDAS, True, titleChunks,
						ret_title, ret_list, ret_mod);
    _DtHelpCeUnlockVolume(lockInfo);
    return result;

}  /* End _DtHelpFormatTopicTitle */

/******************************************************************************
 * Function:	int _DtHelpFormatVolumeTitle (DtHelpDispAreaStruct *pDAS,
 *				_XvhVolume volume,
 *				XmString *ret_title, XmFontList *ret_list,
 *				Boolean *ret_mod)
 *
 * Parameters:
 *		volume		Specifies the Help Volume the information
 *				is associated with.
 *		ret_title	Returns XmString containing the title.
 *		ret_list	Specifies the current font list being used.
 *				Returns a new font list if new character
 *				sets are added to it.
 *		ret_mod		Returns True if fonts were added to the
 *				font list.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	_DtHelpFormatVolumeTitle formats a volume title and associates
 *		the correct font with it.
 *
 ******************************************************************************/
int
_DtHelpFormatVolumeTitle(
    XtPointer		  client_data,
    _DtHelpVolumeHdl	  volume_handle,
    XmString		 *ret_title,
    XmFontList		 *ret_list,
    Boolean		 *ret_mod )
{
    int		 result = -1;
    void	**titleChunks = NULL;
    _FrmtUiInfo           myUiInfo = defUiInfo;
    _DtHelpCeLockInfo lockInfo;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * Check the parameters
     */
    if (ret_title == NULL || ret_list == NULL || ret_mod == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    /*
     * lock the volume
     */
    *ret_title = NULL;
    if (_DtHelpCeLockVolume(volume_handle, &lockInfo) != 0)
	return -1;

    /*
     * set up my UI information
     */
    myUiInfo.load_graphic = _DtHelpDALoadGraphic;
    myUiInfo.resolve_spc  = _DtHelpDAResolveSpc;
    myUiInfo.load_font    = _DtHelpDAResolveFont;
    myUiInfo.exec_filter  = pDAS->exec_filter;
    myUiInfo.destroy_region = _DtHelpDADestroyRegion;
    myUiInfo.client_data  = (_DtCvPointer) pDAS;
    /* since we're going for chunks, set avg_char width to 1 */
    myUiInfo.line_width   = 0;
    myUiInfo.line_height  = 0;
    myUiInfo.leading      = 0;
    myUiInfo.avg_char     = 1;
    myUiInfo.nl_to_space  = pDAS->nl_to_space;

    /*
     * Get the title and charsets associated with the volume title.
     * The format of the returned information is
     *		[type,charset/fontptr,]type,string/spc
     */
    result = _DtHelpCeGetVolumeFlag(volume_handle);
    _DtHelpProcessLock();
    if (result == 1)
        result = _DtHelpCeGetSdlVolTitleChunks(volume_handle, &myUiInfo,
				&titleChunks);
    else if (result == 0)
        result = _DtHelpCeGetCcdfVolTitleChunks(
				(_DtHelpVolume) volume_handle,
				&myUiInfo,
				&titleChunks);
    _DtHelpProcessUnlock();
    if (result != -1)
        result = FormatChunksToXmString(pDAS, True, titleChunks,
						ret_title, ret_list, ret_mod);
    _DtHelpCeUnlockVolume(lockInfo);
    return result;

}  /* End _DtHelpFormatVolumeTitle */

/******************************************************************************
 * Function:	int _DtHelpFormatIndexEntries (DtHelpDispAreaStruct *pDAS,
 *				_XvhVolume volume,
 *				XmString *ret_title, XmFontList *ret_list,
 *				Boolean *ret_mod)
 *
 * Parameters:
 *		volume		Specifies the Help Volume the information
 *				is associated with.
 *              ret_cnt         number of valid entries in the array
 *		ret_words	Returns NULL-termintaed array of 
 *                                XmStrings containing the words.
 *		ret_list	Specifies the current font list being used.
 *				Returns a new font list if new character
 *				sets are added to it.
 *		ret_mod		Returns True if fonts were added to the
 *				font list.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	_DtHelpFormatIndexEntries formats index entries and associates
 *		the correct font with it.
 *
 ******************************************************************************/
int
_DtHelpFormatIndexEntries(
    XtPointer		  client_data,
    _DtHelpVolumeHdl	  volume_handle,
    int			 *ret_cnt,
    XmString		**ret_words,
    XmFontList		 *ret_list,
    Boolean		 *ret_mod )
{
    int		  i;
    int		  result = -1;
    void	 *titleChunks[4];
    char	 *charSet;
    char	**keyWords;
    Boolean       myMod = False;
    XmFontList    oldList;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * Check the parameters
     */
    if (ret_words == NULL || ret_list == NULL || ret_mod == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    *ret_mod = False;

    charSet = _DtHelpGetVolumeLocale(volume_handle);
    if (charSet != NULL)
      {
	*ret_cnt = _DtHelpCeGetKeywordList(volume_handle, &keyWords);
	if (*ret_cnt > 0)
	  {

	    *ret_words = (XmString *) malloc (sizeof(XmString) * (*ret_cnt+1));
	    if (*ret_words == NULL)
		return -1;

            /* the order of the string ptr and charset ptr in the titleChunks
               is determined by the order of the if(..) tests on the
               flags and subsequent processing in FormatChunksToXmString() */
	    titleChunks[0] = (void *) (DT_HELP_CE_STRING | DT_HELP_CE_CHARSET);
	    titleChunks[1] = (void *) charSet;
	    titleChunks[3] = (void *) DT_HELP_CE_END;
	    for (i = 0, result = 0; result == 0 && i < *ret_cnt; i++)
	      {
		oldList        = *ret_list;
		titleChunks[2] = (void *) keyWords[i];
		result = FormatChunksToXmString(pDAS, False, titleChunks,
				&(*ret_words)[i], ret_list, &myMod);
		if (myMod == True)
		  {
		    /*
		     * if the list has been modified before,
		     * free the previously modified list.
		     */
		    if (True == *ret_mod)
			XmFontListFree(oldList);

		    *ret_mod = True;
		  }
	      }
            (*ret_words)[i] = NULL;
	  }
	free(charSet);
      }

    return result;

}  /* End _DtHelpFormatIndexEntries */

/******************************************************************************
 * Function:	int _DtHelpFormatTopic (
 *				DtHelpDispAreaStruct *pDAS,
 *				_DtHelpVolumeHdl volume,
 *				char *id_string,
 *				Boolean look_at_id,
 *				XtPointer *ret_handle)
 *
 * Parameters:
 *		pDAS		Specifies the display area to use.
 *		volume		Specifies the Help Volume the information
 *				is associated with.
 *		id_string	Specifies the location id for the topic.
 *				This location id can be contained in a
 *				topic.
 *		look_at_id	Specifies whether or not to start the
 *				display of the topic at the location id.
 *		ret_handle	Returns a handle to the topic information
 *				including the number of paragraphs and the
 *				id match segment.
 *
 * Returns:	0	if successful.
 *		-1	if errors.
 *		-2	if the id was not found.
 *		-3	if unable to format the topic.
 *
 * errno Values:
 *
 * Purpose:	_DtHelpParseTopic accesses and parses Help topics.
 *
 ******************************************************************************/
int
_DtHelpFormatTopic(
	XtPointer	  client_data,
	_DtHelpVolumeHdl  volume,
	char		 *id_string,
	Boolean		  look_at_id,
	XtPointer	 *ret_handle )
{
    char  *filename = NULL;
    int    offset;
    int    result = -2;
    _DtHelpCeLockInfo   lockInfo;
    _FrmtUiInfo           myUiInfo = defUiInfo;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    _DtCvTopicInfo	*topic;

    if (_DtHelpCeLockVolume(volume, &lockInfo) != 0)
	return -1;

    if (_DtHelpCeFindId(volume,id_string,lockInfo.fd,&filename,&offset)==True)
      {
	if (look_at_id == False)
	    id_string = NULL;

	myUiInfo.load_graphic = _DtHelpDALoadGraphic;
	myUiInfo.resolve_spc  = _DtHelpDAResolveSpc;
	myUiInfo.load_font    = _DtHelpDAResolveFont;
	myUiInfo.exec_filter  = pDAS->exec_filter;
        myUiInfo.destroy_region = _DtHelpDADestroyRegion;
	myUiInfo.client_data  = (_DtCvPointer) pDAS;
        myUiInfo.line_width   = pDAS->lineThickness;
        myUiInfo.line_height  = pDAS->lineHeight;
        myUiInfo.leading      = pDAS->leading;
	myUiInfo.avg_char     = (int)(pDAS->charWidth / 10 +
					((pDAS->charWidth % 10) ? 1 : 0));
        myUiInfo.nl_to_space  = pDAS->nl_to_space;

	result = _DtHelpCeGetVolumeFlag(volume);
	_DtHelpProcessLock();
        if (result == 1)
            result = _DtHelpCeParseSdlTopic(volume,
						&myUiInfo,
						lockInfo.fd, offset,
						id_string, True, &topic);
        else if (result == 0)
            result = _DtHelpCeFrmtCcdfTopic((_DtHelpVolume) volume,
						filename, offset,
						id_string,
						&myUiInfo, &topic);
	_DtHelpProcessUnlock();
	*ret_handle = (XtPointer) topic;
	if (result != 0)
	    result = -3;

	if (filename != NULL)
	    free(filename);
      }

    _DtHelpCeUnlockVolume(lockInfo);

    return result;

}  /* End _DtHelpFormatTopic */


/******************************************************************************
 * Function:	int _DtHelpGetAsciiVolumeTitle (
 *
 * Parameters:	vol		Specifies the loaded volume.
 *		retTitle	Returns the title of the volume. This string is
 *				owned by the caller and must be freed.
 *
 * Return Value: 0 if successful, -1 if a failure occurs
 *
 * errno Values:	CEErrorMalloc
 *			EINVAL		'vol' or 'retTitle' is NULL.
 *			CEErrorMissingTitleRes
 *					'vol' does not contain the resource
 *					'Title' or 'title' or the resource
 *					is zero length.
 *
 * Purpose:	Get the title of a volume.
 *
 ******************************************************************************/
int 
_DtHelpGetAsciiVolumeTitle (
     XtPointer		  client_data,
    _DtHelpVolumeHdl	  volume,
    char		**ret_title)
{
    int			  result;
    _FrmtUiInfo           myUiInfo = defUiInfo;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * What type of volume is it?
     */
    result = _DtHelpCeGetVolumeFlag(volume);
    if (1 == result)
      {
        /*
         * set up my UI information
         */
        myUiInfo.load_graphic = _DtHelpDALoadGraphic;
        myUiInfo.resolve_spc  = _DtHelpDAResolveSpc;
        myUiInfo.load_font    = _DtHelpDAResolveFont;
        myUiInfo.exec_filter  = pDAS->exec_filter;
        myUiInfo.destroy_region = _DtHelpDADestroyRegion;
        myUiInfo.client_data  = (_DtCvPointer) pDAS;
        /* since we're going for a string, set avg_char width to 1 */
        myUiInfo.line_width   = 0;
        myUiInfo.line_height  = 0;
        myUiInfo.leading      = 0;
        myUiInfo.avg_char     = 1;
        myUiInfo.nl_to_space  = pDAS->nl_to_space;

	_DtHelpProcessLock();
        result = _DtHelpCeFrmtSDLVolTitleToAscii(volume, &myUiInfo, ret_title);
      }
    else if (0 == result)
      {
	_DtHelpProcessLock();
        result = _DtHelpCeGetCcdfVolumeTitle(volume, ret_title);
      }
    _DtHelpProcessUnlock();
    return result;
}

/*****************************************************************************
 * Function:	int _DtHelpFormatToc (
 *			_DtHelpVolumeHdl volume, id);
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	-1 if errors.
 *		 0 if no errors. 
 *		 1 if empty path.
 *
 * Purpose:
 *
 *****************************************************************************/
int
_DtHelpFormatToc (
    DtHelpDispAreaStruct	*pDAS,
    _DtHelpVolumeHdl		 volume,
    char		         *id,
    char			**ret_id,
    XtPointer			 *ret_handle)
{
    int             result   = 0;
    _DtCvTopicPtr    topic;
    _DtHelpCeLockInfo lockInfo;

    if (volume == NULL || id == NULL)
	return -1;

    /*
     * This becomes my volume; want to ensure that it doesn't
     * get closed out from under me, so tell the system that
     * i'm using it.
     */
    if (_DtHelpCeLockVolume(volume, &lockInfo) != 0)
	return -1;

    result = _DtHelpCeMapTargetToId(volume, id, ret_id);

    if (result == 0)
      {
        _FrmtUiInfo  myUiInfo = defUiInfo;

	myUiInfo.client_data  = (XtPointer) pDAS;
	myUiInfo.load_graphic = _DtHelpDALoadGraphic;
	myUiInfo.resolve_spc  = _DtHelpDAResolveSpc;
	myUiInfo.load_font    = _DtHelpDAResolveFont;
	myUiInfo.exec_filter  = pDAS->exec_filter;
        myUiInfo.destroy_region = _DtHelpDADestroyRegion;
	myUiInfo.client_data  = (_DtCvPointer) pDAS;
        myUiInfo.line_width   = pDAS->lineThickness;
        myUiInfo.line_height  = pDAS->lineHeight;
        myUiInfo.leading      = pDAS->leading;
	myUiInfo.avg_char     = (int)(pDAS->charWidth / 10 +
					((pDAS->charWidth % 10) ? 1 : 0));
        myUiInfo.nl_to_space  = pDAS->nl_to_space;

	result = _DtHelpCeGetVolumeFlag(volume);
	_DtHelpProcessLock();
        if (result == 1)
	    result = _DtHelpCeFrmtSdlPathAndChildren(volume,
							&myUiInfo,
							lockInfo.fd,
							*ret_id, &topic);
        else if (result == 0)
	    result = _DtHelpCeFrmtCcdfPathAndChildren(volume,
							*ret_id,
							&myUiInfo, &topic);
	_DtHelpProcessUnlock();
	*ret_handle = (XtPointer) topic;
      }
    /*
     * didn't successfully format a path, so close my copy
     * of the volume.
     */
    else
        _DtHelpCloseVolume (volume);

    _DtHelpCeUnlockVolume(lockInfo);
    return result;

} /* End _DtHelpFormatToc */
