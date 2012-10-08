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
/* $TOG: FormatCCDF.c /main/23 1999/10/14 15:06:28 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	FormatCCDF.c
 **
 **   Project:    Unix Desktop Help
 **
 **   Description: This code formats an file in CCDF (Cache Creek
 **		   Distribution Format) into internal format.
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
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#if defined(sun)
#include <sys/utsname.h>
#endif
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>

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
#include "bufioI.h"
#include "Access.h"
#include "AccessI.h"
#include "AccessP.h"
#include "AccessCCDFI.h"
#include "CanvasError.h"
#include "CleanUpI.h"
#include "CvStringI.h"
#include "CvtToArrayP.h"
#include "FontAttrI.h"
#include "FormatUtilI.h"
#include "FormatCCDFI.h"
#include "CCDFUtilI.h"
#include "HelpXlate.h"
#include "StringFuncsI.h"

extern char *_DtHelpGetLocale(void);

#ifdef NLS16
#endif

/******************************************************************************
 * Private variables and defines.
 *****************************************************************************/
#define	BUFF_SIZE	1024
#define	GROW_SIZE	10
#define	INFO_GROW	256

#define	CCDF_LINK_JUMP_REUSE	0
#define	CCDF_LINK_JUMP_NEW	1
#define	CCDF_LINK_DEFINITION	2
#define	CCDF_LINK_EXECUTE	3
#define	CCDF_LINK_MAN_PAGE	4
#define	CCDF_LINK_APP_DEFINE	5

#define	IsTypeHyperText(x)	(((x) & _DtCvHYPER_TEXT) ? 1 : 0)
#define	IsTypeNewLine(x)	(((x) & _DtCvNEW_LINE) ? 1 : 0)
#define	NextAvailSeg(x)		(NULL != x.list ? &(x.list[x.cnt]) : NULL)

/******************************************************************************
 * Private structures
 ******************************************************************************/
typedef	struct	_segList {
	int		 cnt;
	int		 max;
	_DtCvSegment	*list;
} SegList;

typedef	struct _formatVariables {

	int		 last_was_space;
	int		 last_was_mb;
	int		 last_was_nl;

	char		*rd_buf;
	char		*rd_ptr;
	char		*fmt_buf;
	char		*my_path;
	char		*vol_name;
	char		*topic_id;
	char		*topic_char_set;
	char		*topic_lang;

	int		 rd_size;
	int		 rd_flag;

	int		 topic_mb_max;
	int		 cur_mb_max;

	int		 fmt_buf_max;
	int		 fmt_size;
	int		 cell_cnt;

	BufFilePtr	 my_file;
	SegList		 my_list;
	_DtCvLinkDb	 my_links;
	_FrmtUiInfo	*ui_info;
} FormatVariables;


enum	_processState
  {
    NormalState,
    LabelBody
  };

typedef enum _processState	ProcessState;

/******************************************************************************
 * Private Function Declarations
 ******************************************************************************/
static	_DtCvSegment	*AllocateSegments(
				_DtCvSegment      *p_seg,
				int              original_size,
				int              new_size );
static	int		 AppendCharToInfo(
				FormatVariables  *cur_vars,
				char		**src );
static	int		 AppendOctalToInfo(
				FormatVariables *cur_vars,
				char		*src );
static	int		 AppendSpaceToInfo ( 
				FormatVariables  *cur_vars,
				char		**src,
				_DtCvFrmtOption	  type );
static	int		 AppendToInfo (
				FormatVariables  *cur_vars,
				char		**src,
				const char	 *scan_string );
static	int		 ChangeFont(
				int		  whichOne,
				unsigned long	  seg_flags,
				_DtCvFrmtOption	  frmt_type,
				_DtHelpFontHints *font_attr,
				int		  linkspec,
				FormatVariables	 *cur_vars,
				ProcessState	  cur_state,
				int		  flags,
				int		  ret_on_nl,
				int		  fnt_flag);
static	int		 CheckIdString(
				FormatVariables	 *cur_vars,
				ProcessState	  cur_state,
				unsigned long	  seg_flags,
				_DtCvFrmtOption	  frmt_type,
				_DtHelpFontHints *font_attr,
				int		  linkspec,
				int		  flags,
				int		  ret_on_nl,
				int		  fnt_flag);
static	int		 CheckSaveSegment (
				unsigned long	  seg_flags,
				_DtHelpFontHints *font_attr,
				int 		 link_spec,
				FormatVariables	 *cur_vars );
static	int		 CreateSaveGraphic (
				FormatVariables *cur_vars,
				int		 type,
				char		*file_name,
				int		 link_spec );
static	int		 CheckSegList (
				FormatVariables *cur_vars);
static	int		 Format(
				FormatVariables *cur_vars,
				_FrmtUiInfo	*ui_info,
				char		*id_string,
				_DtCvTopicPtr	*ret_handle);
static	int		 GetNextBuffer (
				FormatVariables *cur_vars);
static	int		 GetStringParameter(
				FormatVariables  *cur_vars,
				_DtCvValue	  flag,
				_DtCvValue	  eat_escape,
				_DtCvValue	  ignore_quotes,
				_DtCvValue	  less_test,
				char		**ret_string );
static	int		 GetValueParameter(
				FormatVariables *cur_vars,
				_DtCvValue	 flag,
				int		*ret_value );
static	int		 InitStructure(
				FormatVariables *cur_vars,
				_FrmtUiInfo	*ui_info,
				char		*rd_buf,
				int		 rd_size);
static	int		 Parse(
				int		  cur_cmd,
				ProcessState	  cur_state,
				FormatVariables	 *cur_vars,
				unsigned long	  seg_flags,
				_DtCvFrmtOption	  frmt_type,
				_DtHelpFontHints *font_attr,
				int		  linkspec,
				const char	 *scan_string,
				int		  allowed,
				int		  ret_on_nl,
				int		  fnt_flag);
static	int		 ProcessFigureCmd(
				FormatVariables	 *cur_vars,
				ProcessState	  cur_state,
				_DtCvFrmtOption	  frmt_type,
				_DtHelpFontHints *font_attr,
				int		  ret_on_nl,
				int		  fnt_flag);
static	int		 ProcessHypertext(
				FormatVariables	 *cur_vars,
				ProcessState	  cur_state,
				unsigned long	  seg_flags,
				_DtCvFrmtOption	  frmt_type,
				_DtHelpFontHints *font_attr,
				int		  flags,
				int		  ret_on_nl,
				int		  fnt_flag);
static	int		 ProcessInLine(
				FormatVariables	*cur_vars,
				int		 seg_type,
				int		 link_spec );
static	int		 ProcessLabelCmd(
				FormatVariables	 *cur_vars,
				unsigned long	  seg_flags,
				_DtCvFrmtOption	  frmt_type,
				_DtHelpFontHints *font_attr,
				int		  link_spec,
				int		  flags,
				int		  ret_on_nl,
				int		  fnt_flag,
				_DtCvSegment	**ret_list);
static	int		 ProcessParagraph (
				FormatVariables	*cur_vars,
				ProcessState	 cur_state,
				int		 fnt_flag);
static	int		 SaveNewLine(
				FormatVariables	 *cur_vars,
				int		  cur_cmd,
				_DtHelpFontHints *font_attr,
				int		  link_spec );
static	int		 SaveStringAsSegments (
				FormatVariables	 *cur_vars,
				unsigned long	  seg_flags,
				_DtHelpFontHints *font_attr,
				int		  link_spec);
static	int		 SkipToNextToken (
				FormatVariables	*cur_vars,
				_DtCvValue	 flag );
static	void		 TerminateSegList (
				SegList		*list,
				int		 flag);
static  int              SegmentSave(
				FormatVariables	*cur_vars,
                                unsigned long	 seg_type,
				_DtHelpFontHints *font_attr,
                                char            *string,
                                int              str_size,
                                int              num_chars,
                                int              link_spec);

/********    End Public Function Declarations    ********/

/******************************************************************************
 * Private variables
 *****************************************************************************/
static	const char	*Specials = "<\n\\ ";
static	const char	*SpaceString = "        ";
static	const char	*CString = "C";
static	const char	*Period  = ".";
static	const char	*Slash   = "/";

static const FormatVariables DefVars = {
	True,		/* int		 last_was_space;	*/
	False,		/* int		 last_was_mb;		*/
	False,		/* int		 last_was_nl;		*/

	NULL,		/* char		*rd_buf;		*/
	NULL,		/* char		*rd_ptr;		*/
	NULL,		/* char		*fmt_buf;		*/
	NULL,		/* char		*my_path;		*/
	NULL,		/* char		*vol_name;		*/
	NULL,		/* char		*topic_id;		*/
	NULL,		/* char		*topic_char_set;	*/
	NULL,		/* char		*topic_lang;		*/

	0,		/* int		 rd_size;		*/
	0,		/* int		 rd_flag;		*/

	1,		/* int		 topic_mb_max;		*/
	1,		/* int		 cur_mb_max;		*/

	0,		/* int		 fmt_buf_max;		*/
	0,		/* int		 fmt_size;		*/
	0,		/* int		 cell_cnt;		*/

	NULL,		/* BufFilePtr	 my_file;		*/
	  {		/* SegList	 my_list;		*/
	    0,		    /* int		 cnt;	*/
	    0,		    /* int		 max;	*/
	    NULL,	    /* _DtCvSegment	*list;	*/
	  },		/* SegList	 my_list;		*/
	NULL,		/* _DtCvLinkDb	 my_links;		*/
	NULL,		/* _FrmtUiInfo	 ui_info;		*/
};

static	const	SegList		InitList =
  {
    0,		/* int		 cnt;	*/
    0,		/* int		 max;	*/
    NULL,	/* _DtCvSegment	*list;	*/
  };

static	const	_DtCvContainer	DefContainer =
  {
        NULL,			/* char			*id;		*/
        NULL,			/* char			*justify_char;	*/
        _DtCvDYNAMIC,		/* _DtCvFrmtOption	 type;		*/
        _DtCvBORDER_NONE,	/* _DtCvFrmtOption	 border;	*/
        _DtCvJUSTIFY_LEFT,	/* _DtCvFrmtOption	 justify;	*/
        _DtCvJUSTIFY_TOP,	/* _DtCvFrmtOption	 vjustify;	*/
        _DtCvJUSTIFY_LEFT_MARGIN,/* _DtCvFrmtOption	 orient;	*/
        _DtCvJUSTIFY_TOP,	/* _DtCvFrmtOption	 vorient;	*/
        _DtCvWRAP_NONE,		/* _DtCvFrmtOption	 flow;		*/
        10000,			/* int			 percent;	*/
        0,			/* _DtCvUnit		 leading;	*/
        0,			/* _DtCvUnit		 fmargin;	*/
        0,			/* _DtCvUnit		 lmargin;	*/
        0,			/* _DtCvUnit		 rmargin;	*/
        0,			/* _DtCvUnit		 tmargin;	*/
        1,			/* _DtCvUnit		 bmargin;	*/
	{0, NULL},		/* _DtCvLine             bdr_info;      */
        NULL,			/* _DtCvSegment		 seg_list;	*/
  };

static const FrmtPrivateInfo DefPrivInfo =
  {
	(char) False,		/* char		 top_block;	*/
	(char) False,		/* char		 dup_flag;	*/
	0,			/* int		 sdl_type;	*/
	0,			/* void		*match_info;	*/
	NULL,			/* void		*id_info;	*/
	NULL,			/* void		*tmp_info;	*/
  };

/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function:	int CreateMarker (
 *
 * Parameters:
 *              cur_vars        Specifies the current setting of
 *				formatting variables.
 *              id_string       The id string for the marker.
 *
 * Returns:	0 if successful, -1 if failure.
 *
 * Purpose:	To create a marker id.
 *
 *****************************************************************************/
static	int
CreateMarker (
    FormatVariables	*cur_vars,
    char		*id_string)
{
    int	result;

    /*
     * make sure there is room for the marker segment
     */
    result = CheckSegList(cur_vars);
    if (-1 != result)
      {
	int		 cnt  = cur_vars->my_list.cnt;
	_DtCvSegment	*list = cur_vars->my_list.list;

	/*
	 * now make a mark segment here.
	 */
	list[cnt].type = _DtCvSetTypeToMarker(list[cnt].type);
	_DtCvIdOfMarkerSeg(&list[cnt]) = id_string;
	cur_vars->my_list.cnt++;
      }

    return result;
}

/******************************************************************************
 * Function:	void GetMbLen (
 *
 * Parameters:
 *              cur_vars        Specifies the current setting of
 *				formatting variables.
 *              font_str        Specifies the CCDF string for a
 *				CHARACTERSET change.  The string is
 *				modified by this routine.
 *              lang_ptr        Returns the language and territory
 *				value.  The caller owns this memory.
 *              set_ptr         Returns the code set of the language.
 *				The caller owns this memory.
 *
 * Returns:	Nothing
 *
 * Purpose:	1) To decompose the font_string into a language and code set
 *		   portions.
 *		2) To set the mb_cur_len variable in cur_vars.
 *
 *****************************************************************************/
static	void
GetMbLen (
    FormatVariables	 *cur_vars,
    char		 *font_str,
    char		**lang_ptr,
    char		**set_ptr)
{
    int   i;
    char *ptr;
    char *langPart = NULL;
    char *codePart = NULL;

    /*
     * strip spaces
     */
    while (*font_str == ' ')
	font_str++;

    for (i = strlen(font_str) - 1; i > -1 && font_str[i] == ' '; i--)
	font_str[i] = '\0';

    /*
     * check for locale.codeset
     */
    if (_DtHelpCeStrchr(font_str, ".", 1, &ptr) == 0)
      {
	/*
	 * make font_str two seperate strings by replacing
	 * the dot with a end of line.
	 */
	*ptr++ = '\0';
	langPart = strdup(font_str);
	codePart = strdup(ptr);
      }

    /*
     * old CCDF volume
     */
    else
      {
	_DtHelpCeXlateOpToStdLocale(DtLCX_OPER_CCDF, font_str,
						NULL, &langPart, &codePart);
	/*
	 * if the language is not specified for the code set,
	 * get the environment's.
	 */
	if (strcmp(langPart, "?") == 0)
	  {
	    free(langPart);

            langPart = _DtHelpGetLocale();
            if (langPart == NULL || *langPart == '\0')
	      langPart = strdup((char *) CString);
	    else if (_DtHelpCeStrrchr(langPart, "_", MB_CUR_MAX, &ptr) == 0)
		*ptr = '\0';
	    else if (_DtHelpCeStrrchr(langPart, Period, MB_CUR_MAX, &ptr) == 0)
		*ptr = '\0';
	  }
      }

    cur_vars->cur_mb_max = _DtHelpCeGetMbLen(langPart, codePart);

    /*
     * now set the return variables
     */
    if (lang_ptr != NULL)
        *lang_ptr = langPart;
    else
	free(langPart);

    if (set_ptr != NULL)
        *set_ptr = codePart;
    else
	free(codePart);
}

/******************************************************************************
 * Function:	int ReturnLinkWinHint (int ccdf_type)
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:
 *****************************************************************************/
static	int
ReturnLinkWinHint (
    int   ccdf_type)
{
    int  winHint = _DtCvWindowHint_CurrentWindow;

    if (ccdf_type == CCDF_LINK_JUMP_NEW)
	winHint = _DtCvWindowHint_NewWindow;

    else if (ccdf_type == CCDF_LINK_DEFINITION)
	winHint = _DtCvWindowHint_PopupWindow;

    return winHint;
}

/******************************************************************************
 * Function:	int ReturnLinkType (int ccdf_type, char *spec)
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:
 *****************************************************************************/
static	int
ReturnLinkType (
    int   ccdf_type,
    char *spec)
{
    int  trueType = _DtCvLinkType_SameVolume;

    switch(ccdf_type)
      {
	case CCDF_LINK_JUMP_NEW:
	case CCDF_LINK_JUMP_REUSE:
        case CCDF_LINK_DEFINITION:
		if (strchr(spec, ' ') != NULL)
		    trueType = _DtCvLinkType_CrossLink;
		break;

	case CCDF_LINK_EXECUTE:
		trueType = _DtCvLinkType_Execute;
		break;

	case CCDF_LINK_MAN_PAGE:
		trueType = _DtCvLinkType_ManPage;
		break;
	
        case CCDF_LINK_APP_DEFINE:
		trueType = _DtCvLinkType_AppDefine;
		break;

      }

    return trueType;
}

/******************************************************************************
 * Function:	int CheckSegList (FormatVariables *cur_vars)
 *
 * Parameters:
 *
 * Returns:	0 for success, -1 for errors.
 *
 * errno Values:
 *		CEErrorReported
 *
 * Purpose:	Check the global variable 'SegCount' against 'SegMax' and
 *		realloc/malloc more memory for the global variable 'SegList'
 *		if necessary. If memory is realloc'ed/malloc'ed, the entry
 *		'para_entry->seg_list' is set to 'SegList'.
 *
 *****************************************************************************/
static	int
CheckSegList (
    FormatVariables	*cur_vars )
{
    if (cur_vars->my_list.cnt >= cur_vars->my_list.max)
      {
	cur_vars->my_list.list = AllocateSegments (cur_vars->my_list.list,
				cur_vars->my_list.max,
				(cur_vars->my_list.max + GROW_SIZE));

	if (NULL == cur_vars->my_list.list)
	    return -1;

	cur_vars->my_list.max += GROW_SIZE;
      }

    return 0;
}

/******************************************************************************
 * Function: static _DtCvSegment *AllocateSegments (_DtCvSegment p_seg,
 *					int original_size, int new_size)
 *
 * Parameters:
 *		p_seg		Specifies a pointer to the old list
 *				of _DtCvSegments.
 *		original_size	Specifies the current size of 'p_seg'.
 *		new_size	Specifies the new size of 'p_seg'.
 *
 * Returns:	A pointer to the new list of structures.
 *		A NULL indicates an error.
 *
 * Purpose:	(Re)Allocates _DtCvSegment structures and initializes
 *		them to the default segment which is type _DtCvNOOP
 *		and the container values set to default values.
 *
 *****************************************************************************/
static _DtCvSegment *
AllocateSegments (
	_DtCvSegment	*p_seg,
	int		 original_size,
	int		 new_size )
{
    FrmtPrivateInfo *priv = NULL;

    if (NULL != p_seg)
      {
	int count = 0;

	/*
	 * reallocate the segments.
	 */
	p_seg = (_DtCvSegment *) realloc ((void *) p_seg,
					(sizeof(_DtCvSegment) * new_size));
	/*
	 * reallocate the private formatting information.
	 */
        priv = p_seg->client_use;
	priv = (FrmtPrivateInfo *) realloc ((void *) priv,
					sizeof(FrmtPrivateInfo) * new_size);
	if (NULL == priv)
	    return NULL;

	/*
	 * reattach the private information with their segments.
	 */
	while (count < original_size)
	    p_seg[count++].client_use = priv++;
      }
    else
	p_seg = _DtHelpAllocateSegments (new_size);

    if (NULL != p_seg)
      {
        /*
         * want to 'empty' the type.
         */
        while (original_size < new_size)
	  {
	    /*
	     * if we went through _DtHelpAllocateSegments, then the
	     * priv pointer will be NULL. But that's okay since the
	     * _DtHelpAllocateSegments routine will initalized the
	     * variables for us. The only reason we have to do it
	     * is if we've reallocated the list.
	     */
	    if (NULL != priv)
	      {
		*priv = DefPrivInfo;

	        p_seg[original_size  ].client_use   = priv++;
	        p_seg[original_size  ].type         = _DtCvNOOP;
	        p_seg[original_size  ].link_idx     = -1;
	        p_seg[original_size  ].next_seg     = NULL;
	        p_seg[original_size  ].next_disp    = NULL;
	      }

	    p_seg[original_size++].handle.container = DefContainer;
	  }
      }

    return p_seg;
}

/******************************************************************************
 * Function: static int CheckList (
 *				FormatVariables cur_vars, int count, int grow)
 *
 * Parameters:
 *		cur_vars	Specifies a structure containing the list
 *				of _DtCvSegments.
 *		count		Specifies the amount needed.
 *		grow		Specifies the amount to grow by.
 *
 * Returns:	0 if successful, -1 if failure.
 *
 * Purpose:     Makes sure there is 'grow' size number of segments
 *		available.  If not, it calls AllocateSegments to 'grow'
 *		the segment list.  cur_vars->seg_list is set to the new
 *		pointer, cur_vars->seg_max indicates the maximum number
 *		of segments in the list.
 *
 *****************************************************************************/
static int
CheckList (
    SegList	*seg_list,
    int		 count,
    int		 grow )
{
    /*
     * what's the new count?
     */
    count += seg_list->cnt;

    /*
     * is it larger than the current max?
     */
    if (count > seg_list->max)
      {
	/*
	 * grow the list by the indicated amount.
	 */
	grow += seg_list->max;

	/*
	 * check to see if it is enough. If not, force the grow value
	 * to be the required count.
	 */
	if (count > grow)
	    grow = count;

	/*
	 * (re)allocate
	 */
	seg_list->list = AllocateSegments(seg_list->list, seg_list->max, grow);

	/*
	 * return an code if no list allocated.
	 */
	if (NULL == seg_list->list)
	    return -1;

	/*
	 * set the max count
	 */
	seg_list->max = grow;
      }
			
    return 0;
}

/******************************************************************************
 * Function: int SegmentSave (int type,
 *			char **font_attr, char *string, int linkspec,
 *			_DtCvSegment *p_seg)
 * 
 * Parameters:
 *		type		Specifies the segment type.
 *		font_attr	Specifies the resource list of quarks for the
 *					font to associate with the segment.
 *		string		Specifies the string segment.
 *		link_spec	Specifies the link specification for the
 *					segment.
 *		p_seg		Specifies the _DtCvSegment structure to
 *					initialize with the data.
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *		CEErrorReported
 *
 * Purpose:	Creates a new segment with the given data.
 *
 *****************************************************************************/
static int
SegmentSave (
    FormatVariables	*cur_vars,
    unsigned long	 seg_type,
    _DtHelpFontHints	*font_attr,
    char		*string,
    int			 str_size,
    int			 num_chars,
    int			 linkspec)
{
    int		  result = 0;
    _DtCvSegment *pSeg   = NextAvailSeg(cur_vars->my_list);

    pSeg->type     = _DtCvSetTypeToNoop(seg_type);
    pSeg->link_idx = linkspec;

    /*
     * We use this routine for many things.
     * If we want an newline in here, we may not have any data.
     */
    if (str_size)
      {
	pSeg->type = _DtCvSetTypeToString(pSeg->type);

	/*
	 * if a wide character representation is required,
	 * convert to wchar_t
	 */
	if (seg_type & _DtCvWIDE_CHAR)
	  {
	    wchar_t *pwcs;

	    /*
	     * include the null byte in the multibyte to widechar
	     * conversion.
	     */
	    num_chars++;

	    /*
	     * malloc the memory
	     */
	    pwcs = (wchar_t *) malloc(sizeof(wchar_t) * num_chars);
	    if (NULL != pwcs)
	      {
		/* convert */
		str_size = mbstowcs(pwcs, string, ((size_t) num_chars));

		/* check to see if it converted everything */
		if (str_size + 1 == num_chars)
		    _DtCvStringOfStringSeg(pSeg) = (void *) pwcs;
		else
		    free(pwcs);
	      }
	  }
	else
	    _DtCvStringOfStringSeg(pSeg) = (void *) strdup(string);

        if (NULL == _DtCvStringOfStringSeg(pSeg))
	    return -1;

	/*
	 * set the default font. This should really be passed in via
	 * the format entry point (or the ui_info structure).
	 */
	_DtCvFontOfStringSeg(pSeg)   = 0;

	/*
	 * load the font
	 */
	if (NULL != cur_vars->ui_info->load_font)
	    (*(cur_vars->ui_info->load_font))(
			    cur_vars->ui_info->client_data,
			    _DtHelpFontHintsLang(*font_attr),
			    _DtHelpFontHintsCharSet(*font_attr),
			    *font_attr,
			    &(_DtCvFontOfStringSeg(pSeg)));
      }

    return result;
}

/******************************************************************************
 * Function: int SaveStringAsSegments (FormatVariables, int type, char **font_attr,
 *				int link_spec)
 * 
 * Parameters:
 *		type		Specifies the segment type.
 *		font_attr	Specifies the resource list of quarks for the
 *					font to associate with the segment.
 *		link_spec	Specifies the link specification for the
 *					segment.
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Saves a segment into the global 'SegList'. This is
 *		a wrapper around SegmentSave that adds specific
 *		information relative to this module.
 *
 *****************************************************************************/
static	int
SaveStringAsSegments (
    FormatVariables	*cur_vars,
    unsigned long	 seg_flags,
    _DtHelpFontHints	*font_attr,
    int			 link_spec)
{
    int     len;
    int     size;
    int     numChars;
    int     newLine = False;
    char    tmpChar;
    char   *ptr;

    seg_flags = _DtCvSetTypeToNoop(seg_flags);
    if (cur_vars->cur_mb_max != 1 && cur_vars->fmt_size > 1)
      {
	if (IsTypeNewLine(seg_flags))
	  {
	    newLine = True;
	    seg_flags = seg_flags & ~(_DtCvNEW_LINE);
	  }

	ptr = cur_vars->fmt_buf;
	do
	  {
	    /*
	     * what type of character is this?
	     */
	    len = mblen (ptr, cur_vars->cur_mb_max);

	    /*
	     * How many of the following characters are of the same size?
	     */
	    size = _DtHelpFmtFindBreak (ptr, len, &numChars);

	    /*
	     * save off the character that doesn't match
	     */
	    tmpChar = ptr[size];

	    /*
	     * put in the null byte for the string.
	     * and set to wide char if not doing multi-byte sequence.
	     */
	    ptr[size] = '\0';
	    if (1 != len)
	       seg_flags = _DtCvSetTypeToWideChar (seg_flags);

	    /*
	     * does this segment get the newline flag?
	     */
	    if (True == newLine && size >= cur_vars->fmt_size)
		seg_flags = _DtCvSetTypeToNewLine(seg_flags);

	    /*
	     * save the segment.
	     */
	    if (CheckSegList(cur_vars) == -1 ||
		SegmentSave (cur_vars, seg_flags, font_attr, ptr, size,
						numChars, link_spec) == -1)
		return -1;

	    /*
	     * remove the wide char flag
	     */
	    seg_flags &= ~(_DtCvWIDE_CHAR);

	    /*
	     * Bump the segment list count.
	     */
	    cur_vars->my_list.cnt++;

	    /*
	     * replace the character that didn't match.
	     * and bump the pointer to it.
	     */
	    ptr[size] = tmpChar;
	    ptr      += size;
	    cur_vars->fmt_size -= size;
	  } while (0 < cur_vars->fmt_size);
      }
    else
      {
	if (CheckSegList (cur_vars) == -1 ||
			SegmentSave (cur_vars, seg_flags, font_attr,
				cur_vars->fmt_buf, cur_vars->fmt_size,
				cur_vars->fmt_size,
				link_spec) == -1)
	    return -1;
        cur_vars->my_list.cnt++;
      }

    cur_vars->fmt_size = 0;
    if (cur_vars->fmt_buf)
      cur_vars->fmt_buf[0] = '\0';

    return 0;
}

/******************************************************************************
 * Function: int CheckSaveSegment (int type, char **font_attr, int link_spec,
 *					FormatVariables cur_vars)
 * 
 * Parameters:
 *		type		Specifies the segment type.
 *		font_attr	Specifies the resource list of quarks for the
 *					font to associate with the segment.
 *		link_spec	Specifies the link specification for the
 *					segment.
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Checks 'cur_vars->fmt_size' for a non-zero value. If it is,
 *		calls SaveSegment.
 *
 *****************************************************************************/
static	int
CheckSaveSegment (
    unsigned long	 seg_flags,
    _DtHelpFontHints	*font_attr,
    int			 link_spec,
    FormatVariables	*cur_vars)
{
    int   result = 0;

    if (cur_vars->fmt_size)
        result = SaveStringAsSegments (cur_vars, seg_flags, font_attr, link_spec);

    return result;
}

/******************************************************************************
 * Function: void TerminateSegList (
 * 
 * Parameters:
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * Purpose:	Links the segments together.
 *
 *****************************************************************************/
static	void
TerminateSegList (
    SegList	*seg_list,
    int		 flag)
{
    int	count;
    _DtCvSegment *lastDisp = NULL;
    _DtCvSegment *pSeg;

    if (True == flag && 0 == seg_list->cnt && NULL != seg_list->list)
      {
	free(seg_list->list);
	*seg_list = InitList;
      }

    /*
     * set the next segment and display segment pointers.
     */
    for (count = 1, pSeg = seg_list->list; count < seg_list->cnt;
							count++, pSeg++)
      {
	/*
	 * link this segment to the next segment
	 */
	pSeg->next_seg  = &(pSeg[1]);

	/*
	 * is this a displayable segment?
	 * If so, link it into the displayable list.
	 */
	if (_DtCvIsSegNoop(pSeg) || _DtCvIsSegRegion(pSeg)
						|| _DtCvIsSegString(pSeg))
	  {
	    /*
	     * Is there a displayable segment yet?
	     * If so, set its next displayable link to this segment.
	     */
	    if (NULL != lastDisp)
	        lastDisp->next_disp = pSeg;

	    /*
	     * this segment becomes the last displayable segment.
	     */
	    lastDisp = pSeg;
	  }
	else
	   lastDisp = NULL;
      }

    if (NULL != pSeg && NULL != lastDisp
	&& (_DtCvIsSegNoop(pSeg) || _DtCvIsSegRegion(pSeg)
						|| _DtCvIsSegString(pSeg)))
	lastDisp->next_disp = pSeg;

}

/******************************************************************************
 * Function:	int InitStructure (FormatVariables *cur_vars,
 *				char *rd_buf, int rd_size,
 * 
 * Parameters:
 *		rd_buffer	Specifies the buffer all reads use.
 *		id_string	Specifies the location ID to search for.
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:     Attach formatting information to the structure that gets
 *		passed around by, among other things, setting the global
 *		variable 'cur_vars->rd_ptr' to 'rd_buffer', and
 *		remembering the location id.
 *
 *****************************************************************************/
static	int
InitStructure(
    FormatVariables	*cur_vars,
    _FrmtUiInfo		*ui_info,
    char		*rd_buf,
    int			 rd_size)
{
    cur_vars->ui_info  = ui_info;
    cur_vars->rd_size  = rd_size;
    cur_vars->rd_buf   = rd_buf;
    cur_vars->rd_ptr   = cur_vars->rd_buf;

    cur_vars->fmt_size = 0;
    if (cur_vars->fmt_buf)
	cur_vars->fmt_buf[0] = '\0';

    /*
     * allocate the link data base.
     */
    cur_vars->my_links = _DtLinkDbCreate();
    if (NULL == cur_vars->my_links)
	return -1;

    return 0;
}

/******************************************************************************
 * Function:	int AppendToInfo (FormatVariables *cur_vars,
 *					char **src, const char *scan_string)
 *
 * Parameters:
 *		src		Specifies the source string to read.
 *				Returns pointing at a special character,
 *					an invalid character or the
 *					end of string.
 *		scan_string	Specifies the special characters to
 *				look for in 'src'.
 *
 * Returns:	 0	if stopped on a special character.
 *		 1	if found the end of string.
 *		 2	if found an invalid character.
 *		-1	if errors.
 *
 * errno Values:
 * 
 * Purpose:	Appends onto 'cur_vars->fmt_buf' the number of characters
 *		found in 'src' that does not match any character in
 *		'scan_string'.
 *
 *		Sets 'cur_vars->last_was_space' to false (assumes that one
 *		of the special characters is a space).
 *
 *****************************************************************************/
static	int
AppendToInfo (
    FormatVariables	 *cur_vars,
    char		**src,
    const char		 *scan_string )
{
    int    spnResult;
    int    size;

    cur_vars->last_was_space = False;
    spnResult = _DtHelpCeStrcspn(*src, scan_string, cur_vars->cur_mb_max,
									&size);
    if (0 < size)
      {
        if (_DtHelpCeAddStrToBuf(src,&(cur_vars->fmt_buf),&(cur_vars->fmt_size),
				&(cur_vars->fmt_buf_max),size,INFO_GROW) == -1)
	    return -1;
        cur_vars->last_was_space = False;
      }

    if (spnResult == -1)
	spnResult = 2;

    if (0 == size && 1 == spnResult)
	spnResult = 0;

    return spnResult;
}

/******************************************************************************
 * Function:	int AppendOctalToInfo (FormatVariables *cur_vars, char *src)
 * 
 * Parameters:
 *		src		Specifies the source string to read.
 *
 * Returns:	 0	if successful, -1 if errors.
 *
 * errno Values:
 *		CEErrorFormattingValue
 *		CEErrorMalloc
 * 
 * Purpose:	Convert the octal representation pointed to by 'src' and
 *		change it into a character byte. The routine only allows
 *		a number between the values 1-255.
 *
 *		Append the byte onto the global variable 'Info'.
 *		Set 'cur_vars->last_was_space' to false.
 *
 *****************************************************************************/
static	int
AppendOctalToInfo(
    FormatVariables	*cur_vars,
    char		*src)
{
    /*
     * must have 0xXX
     */
    if (((int)strlen(src)) < 4 && cur_vars->my_file != NULL)
      {
	if (GetNextBuffer (cur_vars) == -1)
	    return -1;

	src = cur_vars->rd_ptr;
      }

    if (_DtHelpCeAddOctalToBuf(src,&(cur_vars->fmt_buf),&(cur_vars->fmt_size),
				&(cur_vars->fmt_buf_max),INFO_GROW) == -1)
	return -1;

    cur_vars->last_was_space = False;

    return 0;
}

/******************************************************************************
 * Function:	int AppendCharToInfo (FormatVariables *cur_vars, char **src)
 * 
 * Parameters:
 *		src		Specifies the source string to read.
 *				Returns pointing at the next character
 *					the string.
 *
 * Returns:	 0	if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Appends the character pointed to by 'src' onto the
 *		global buffer 'Info', updating the pointers associated
 *		with 'Info' accordingly.
 *
 *		Sets 'cur_vars->last_was_space' to False;
 * 
 *****************************************************************************/ static	int
AppendCharToInfo(
    FormatVariables	*cur_vars,
    char		**src)
{
    cur_vars->last_was_space = False;
    return (_DtHelpCeAddCharToBuf (src, &(cur_vars->fmt_buf),
				&(cur_vars->fmt_size),
				&(cur_vars->fmt_buf_max), INFO_GROW));
}

/******************************************************************************
 * Function:	int AppendSpaceToInfo (char **src, int type)
 *
 * Parameters:
 *		src		Specifies the source string to read.
 *				Returns pointing at the next character
 *					the string.
 *		type		Specifies the type of the segment being
 *					processed.
 *
 * Returns:	 0	if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Appends a space onto the global buffer 'Info', if
 *		the variable 'cur_vars->last_was_space' is false or the
 *		static string flag is set in 'type'.
 *
 *		Sets 'cur_vars->last_was_space' to true.
 *
 *****************************************************************************/
static	int
AppendSpaceToInfo ( 
    FormatVariables	 *cur_vars,
    char		**src,
    _DtCvFrmtOption	  type)
{
    int   result = 0;
    char *space = (char *) SpaceString;

    if (!cur_vars->last_was_space || _DtCvLITERAL == type) {

	result = _DtHelpCeAddCharToBuf (&space, &(cur_vars->fmt_buf),
					&(cur_vars->fmt_size),
					&(cur_vars->fmt_buf_max), INFO_GROW);
        cur_vars->last_was_space = True;
    }

    if (src != NULL)
        *src = *src + 1;
    return result;
}

/******************************************************************************
 * Function:	int FindEndMarker (FormatVariables *cur_vars)
 * 
 * Returns:	 0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Wrapper around __DtHelpFindEndMarker.
 *		Find the '>' token.
 *
 *****************************************************************************/
static int
FindEndMarker(
    FormatVariables	*cur_vars)
{
    return (_DtHelpCeGetCcdfEndMark (cur_vars->my_file, cur_vars->rd_buf,
		&(cur_vars->rd_ptr),cur_vars->rd_size,1));
}

/******************************************************************************
 * Function:	int GetNextBuffer (FormatVariables *cur_vars)
 *
 * Returns:	 0	if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Wrapper around __DtHelpGetNextBuffer.
 *		Read the next buffer's worth of information.
 * 
 *****************************************************************************/
static	int
GetNextBuffer (
    FormatVariables *cur_vars)
{
    cur_vars->rd_flag = _DtHelpCeGetNxtBuf (cur_vars->my_file,
					cur_vars->rd_buf,
					&(cur_vars->rd_ptr),
					cur_vars->rd_size);
    return (cur_vars->rd_flag);
}

/******************************************************************************
 * Function:	int SaveNewLine (FormatVariables *cur_vars, int cur_type,
 *					char **font_attr,
 *					int link_spec)
 *
 * Parameters:
 *		cur_cmd		Specifies the type of segment being processed.
 *		font_attr	Specifies the list of font quarks associated
 *				with the current segment.
 *		link_spec	Specifies the hypertext link specification
 *				associated with the current segment.
 *
 * Returns:	 0	if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Sets the newline flag on a segment and saves it by calling
 *			'SaveStringAsSegments'.
 *		If the current segment is non-null, save it with the
 *			newline flag set.
 *		Otherwise if there are no segments saved, create one with the
 *			a type of CE_old_NOOP.
 *		Otherwise there are other segments, but the current segment is
 *			empty. Look back at the previous segment.
 *			If it doesn't have the newline set on it,
 *			its newline flag is enabled.
 *		Otherwise the previous segment had the newline set, so
 *			create another segment just like it with a 
 *			null length and the newline flag set (if the
 *			previous segment is a graphic, create it
 *			with a type of CE_old_NOOP).
 * 
 *****************************************************************************/
static	int
SaveNewLine(
    FormatVariables	*cur_vars,
    int			 cur_type,
    _DtHelpFontHints	*font_attr,
    int			 link_spec )
{
    int         result = 0;
    _DtCvSegment   *pSeg;

    /*
     * If there is information in the buffer, save it with the newline flag
     */
    if (cur_vars->fmt_size)
	result = SaveStringAsSegments (cur_vars, _DtCvSetTypeToNewLine (cur_type),
					font_attr, link_spec);
    /*
     * check to see if there is any segments. If not create a new one
     * with the type NOOP.
     */
    else if (0 == cur_vars->my_list.cnt)
	result = SaveStringAsSegments (cur_vars, _DtCvSetTypeToNewLine(_DtCvNOOP),
					font_attr, link_spec);
    /*
     * There was not any information in the buffer and we have one or
     * more segments. Try placing the flag on the previous segment.
     */
    else 
      {
	pSeg = NextAvailSeg(cur_vars->my_list);
	pSeg--;

	/*
	 * Does the previous segment already have a newline flag?
	 * If so, create a new NOOP segment with the newline set.
	 */
	if (_DtCvIsSegNewLine (pSeg))
	    result = SaveStringAsSegments (cur_vars, _DtCvSetTypeToNewLine (_DtCvNOOP),
					font_attr, link_spec);
	else
	    pSeg->type = _DtCvSetTypeToNewLine (pSeg->type);
      }

    cur_vars->last_was_space = True;
    return result;
}

/******************************************************************************
 * Function:	int CreateSaveGraphic (FormatVariables cur_vars,
 *				int type,
 *					char *file_name, int link_spec )
 * 
 * Parameters:
 *		type		Specifies the type of graphic segment
 *				being processed.
 *		file_name	Specifies the file name of the graphic.
 *		link_spec	Specifies the hypertext link specification
 *				associated with the graphic.
 *
 * Returns:	 0	if successful, -1 if errors.
 *
 * Purpose:	Save a graphic segment.
 *		If 'file_name' is not an absolute path, resolve it to
 *		a full path by using the path to the volume.
 *
 *****************************************************************************/
static	int
CreateSaveGraphic (
    FormatVariables	*cur_vars,
    int			 type,
    char		*file_name,
    int			 link_spec)
{
    char	 *fullName;
    _DtCvSegment *cvRegion;

    if (CheckList (&(cur_vars->my_list), 1, 1) == -1)
	return -1;

    /*
     * create the path to the file name
     */
    if (*file_name == '/')
	fullName = strdup (file_name);
    else
      {
	fullName = (char *) malloc (strlen (cur_vars->my_path) +
						strlen (file_name) + 2);
	if (fullName == NULL)
	    return -1;

	strcpy (fullName, cur_vars->my_path);
	strcat (fullName, "/");
	strcat (fullName, file_name);
      }

    /*
     * load the graphic into the current segment
     */
    cvRegion = NextAvailSeg(cur_vars->my_list);
    if (NULL != cur_vars->ui_info->load_graphic &&
		(*(cur_vars->ui_info->load_graphic))(
				cur_vars->ui_info->client_data,
				cur_vars->vol_name,
				cur_vars->topic_id,
				fullName,
				NULL,
				NULL,
				&(_DtCvWidthOfRegionSeg(cvRegion)),
				&(_DtCvHeightOfRegionSeg(cvRegion)),
				&(_DtCvInfoOfRegionSeg(cvRegion))) == 0)
      {
	/*
	 * set the type and index.
	 * and indicate that this segment has been used.
	 */
	_DtCvAscentOfRegionSeg(cvRegion) = -1;
	cvRegion->type     = _DtCvSetTypeToRegion(type);
	cvRegion->link_idx = link_spec;
	cur_vars->my_list.cnt++;
      }

    /*
     * free the file name path
     */
    free (fullName);

    return 0;
}

/******************************************************************************
 * Function:	int ChangeFont (int whichOne, int segType,
 *				char **font_attr, int linkspec,
 *				FormatVariables *cur_vars, int flags)
 * 
 * Parameters:
 *		whichOne	Specifies the index into 'font_attr' to
 *				change.
 *		segType		Specifies the type of segment currently
 *				being processed.
 *		font_attr	Specifies the list of font quarks associated
 *				with the current segment.
 *		linkspec	Specifies the hypertext link specification
 *				associated with the segment.
 *		flags		Specifies the routines flags.
 *
 * Returns:	The value returned from 'routine' if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Saves any segment with the current font type and
 *		process the next segment using the new font type.
 *
 *****************************************************************************/
static	int
ChangeFont(
    int			 whichOne,
    unsigned long	 seg_flags,
    _DtCvFrmtOption	 frmt_type,
    _DtHelpFontHints	*font_attr,
    int			 linkspec,
    FormatVariables	*cur_vars,
    ProcessState	 cur_state,
    int			 flags,
    int			 ret_on_nl,
    int			 fnt_flag)
{
    int    result;
    int    oldMb_Len = cur_vars->cur_mb_max;
    char  *fontString;
    _DtHelpFontHints oldFontStruct;

    if (CheckSaveSegment (seg_flags, font_attr, linkspec, cur_vars) == -1)
	return -1;

    oldFontStruct = *font_attr;

    if (GetStringParameter(cur_vars, _DtCvTRUE, _DtCvTRUE, _DtCvFALSE,
						_DtCvFALSE, &fontString) == -1)
	return -1;

    /*
     * Is this font change allowed to go through?
     */
    if (!(fnt_flag & (1 << whichOne)))
      {
        switch (whichOne)
          {
	    case _CEFONT_SPACING:
			font_attr->spacing = _DtHelpFontSpacingProp;
			if (fontString[0] == 'm')
			    font_attr->spacing = _DtHelpFontSpacingMono;
			break;
	    case _CEFONT_SIZE:
			font_attr->pointsz = atoi(fontString);
			break;
	    case _CEFONT_ANGLE:
			font_attr->slant = _DtHelpFontSlantRoman;
			if (fontString[0] == 'i')
			    font_attr->slant = _DtHelpFontSlantItalic;
			break;
	    case _CEFONT_WEIGHT:
			font_attr->weight = _DtHelpFontWeightMedium;
			if (fontString[0] == 'b')
			    font_attr->weight = _DtHelpFontWeightBold;
			break;
	    case _CEFONT_TYPE:
			font_attr->style = _DtHelpFontStyleSanSerif;
			if (*fontString == 's')
			  {
			    if (fontString[1] == 'e')
			        font_attr->style = _DtHelpFontStyleSerif;
			    else if (fontString[1] == 'y')
			        font_attr->style = _DtHelpFontStyleSymbol;
			  }
			break;
	    case _CEFONT_CHAR_SET:
			/*
			 * Change to the correct mb_len.
			 */
			GetMbLen(cur_vars, fontString,
					&(_DtHelpFontHintsLang(*font_attr)),
					&(_DtHelpFontHintsCharSet(*font_attr)));
			break;
          }
      }

    result = FindEndMarker (cur_vars);
    if (!result)
	result = Parse (CCDF_FONT_CMD, cur_state, cur_vars,
				seg_flags, frmt_type,
				font_attr, linkspec,
				Specials, flags, ret_on_nl, fnt_flag);
    /*
     * free the lanugage and code sets strings.
     */
    if (!(fnt_flag & (1 << whichOne)) && whichOne == _CEFONT_CHAR_SET)
      {
	free(_DtHelpFontHintsLang(*font_attr));
	free(_DtHelpFontHintsCharSet(*font_attr));
      }

    /*
     * reset the old quark
     * if necessary, reset the MB_LEN
     */
    *font_attr = oldFontStruct;
    cur_vars->cur_mb_max = oldMb_Len;

    /*
     * free the memory
     */
    free (fontString);

    if (result == -1)
	return -1;

    return 0;

} /* ChangeFont */

/******************************************************************************
 * Function:	int SkipToNextToken (FormatVariables *cur_vars, int	 flag)
 * 
 * Parameters:
 *		flag	Specifies whether the routine returns a -1
 *				if '>' is the next token.
 *
 * Returns:	The value from __DtHelpSkipToNextToken:
 *			-1  If problems encountered finding the next token.
 *			 0  If no problems encountered finding the next token.
 *			 1  If flag is true and the next token is a > character.
 *
 * errno Values:
 *
 * Purpose:	Wrapper around __DtHelpSkipToNextToken.
 *		Skip the current string and any spaces or newline
 *		characters after it.
 *
 *****************************************************************************/
static	int
SkipToNextToken (
    FormatVariables	 *cur_vars,
    _DtCvValue		  flag)
{
    return (_DtHelpCeSkipToNextCcdfToken (cur_vars->my_file, cur_vars->rd_buf,
		cur_vars->rd_size, 1, &(cur_vars->rd_ptr), flag));
}

/******************************************************************************
 * Function:	int GetStringParameter (FormatVariables *cur_vars,
 *				int	 flag, int	 eat_escape,
 *				int	 ignore_quotes, int	 less_test,
 *				char **ret_string)
 * 
 * Parameters:
 *		flag            Specifies whether the routine returns
 *                                      a -1 if '>' is the next token.
 *              eat_secape      Specifies whether the backslash is not
 *                                      placed in the returned string.
 *                                      True - it is skipped.
 *                                      False - it is saved in 'ret_string'.
 *              ignore_quotes   Specifies whether quotes are to be included
 *                                      in the returned string.
 *              less_test       Specifies whether the routine should
 *                                      stop when it finds a '<' character.
 *              ret_string      Returns the string found.
 *				If NULL, throws the information away.
 *
 * Returns:	The value from __DtHelpFormatGetStringParameter:
 *			-1  If problems encountered.
 *			 0  If no problems encountered getting the string.
 *			 1  If flag is false and the no string was found.
 *
 * errno Values:
 *
 * Purpose:	Wrapper around __DtHelpFormatGetStringParameter.
 *		Skip the current string and any spaces or newline
 *		characters after it. Get the next quoted/unquoted
 *		string after that.
 *
 *****************************************************************************/
static	int
GetStringParameter(
    FormatVariables	 *cur_vars,
    _DtCvValue		  flag,
    _DtCvValue		  eat_escape,
    _DtCvValue		  ignore_quotes,
    _DtCvValue		  less_test,
    char		**ret_string)
{
    return (_DtHelpCeGetCcdfStrParam (cur_vars->my_file,
		cur_vars->rd_buf, cur_vars->rd_size, cur_vars->cur_mb_max,
		&(cur_vars->rd_ptr),
		flag, eat_escape, ignore_quotes, less_test, ret_string));
}

/******************************************************************************
 * Function:	int GetValueParameter (FormatVariables *cur_vars,
 *				int	 flag, int *ret_value)
 * 
 * Parameters:
 *              flag            Specifies whether the routine returns
 *                                      a -2 if '>' is the next token.
 *              ret_value       Returns the atoi conversion
 *                              of the string found.
 *
 * Returns:	The value from __DtHelpFormatGetValueParameter:
 *			-1  If problems encountered.
 *			 0  If no problems encountered getting the string.
 *			-2  If flag is true and the next token is a >
 *				character.
 *
 * errno Values:
 *
 * Purpose:	Wrapper around __DtHelpFormatGetValueParameter.
 *		Skip the current string and any spaces or newline
 *		characters after it. Process the next string as
 *		a numeric value.
 *
 *****************************************************************************/
static	int
GetValueParameter(
    FormatVariables	*cur_vars,
    _DtCvValue		 flag,
    int			*ret_value )
{
    return (_DtHelpCeGetCcdfValueParam (cur_vars->my_file, cur_vars->rd_buf,
			cur_vars->rd_size,
			&(cur_vars->rd_ptr), flag, cur_vars->cur_mb_max,
			ret_value));
}

/******************************************************************************
 * Function:	int GetParagraphParameters (FormatVariables *cur_vars,
 *				int seg_type, int graphic_type,
 *				char **label, char **file_name,
 *				char **link_string, int *link_type,
 *				char **description)
 *
 * Parameters:
 *              seg_type	Specifies the default type for the segment.
 *				Returns the new type for the segment.
 *		graphic_type	Specifies the default type for a graphic
 *					if a justified graphic is
 *					encountered in the paragraph options.
 *				Returns the new type for a graphic
 *					if a justified graphic was 
 *					encountered in the paragraph options.
 *		label		Returns the label if one is specified
 *					in the paragraph options.
 *		file_name	Returns the file name of a graphic if
 *					one is specified in the paragraph
 *					options.
 *		link_string	Returns the hypertext specification if one
 *					is specified in the paragraph options.
 *		link_type	Returns the hypertext link type if one is
 *					specified.
 *		description	Returns the hypertext description if one
 *					is specified.
 * 
 * Returns:	0 if successfult, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Wrapper around __DtHelpParagraphGetOptions.
 *		Process the options found in <PARAGRAPH> syntax.
 *		Test to see if the id specified in the <PARAGRAPH>
 *		is the one we are looking at.
 *
 *****************************************************************************/
static	int
GetParagraphParameters(
    FormatVariables	 *cur_vars,
    _DtCvSegment	 *para,
    _DtCvFrmtOption	 *frmt_type,
    _DtCvFrmtOption	 *gpos,
    char		**glink,
    int			 *glinktype,
    _DtCvUnit		 *gspace,
    char		**ret_label,
    char		**file_name,
    char		**description)
{
    int		 done   = False;
    int		 result = 0;
    int		 optionCount = 0;
    int		 value;
    char	*tmpString = NULL;

    /*
     * initialize string variables if valid addresses
     */
    *ret_label   = NULL;
    *file_name   = NULL;
    *glink       = NULL;
    *description = NULL;

    *gpos       = _DtCvJUSTIFY_LEFT;
    *glinktype  = -1;
    *gspace     = 0;

    while (False == done && result != -1)
      {
	if (SkipToNextToken (cur_vars, _DtCvFALSE) == -1)
	    return -1;

	switch (_DtCvToLower(*(cur_vars->rd_ptr)))
	  {
	    /*
	     * end of paragraph spec
	     */
	    case '>':
		(cur_vars->rd_ptr)++;
		done = True;
		break;

	    /*
	     * after value
	     */
	    case 'a':
		if (GetValueParameter (cur_vars, _DtCvFALSE, &value) == -1)
		    return -1;

		_DtCvContainerBMarginOfSeg(para) = 0;
		if (value > 0 && 0 < cur_vars->ui_info->line_height)
		    _DtCvContainerBMarginOfSeg(para) = value *
					(cur_vars->ui_info->line_height / 2);
		break;

	    /*
	     * before value
	     */
	    case 'b':
		if (GetValueParameter (cur_vars, _DtCvFALSE, &value) == -1)
		    return -1;

		_DtCvContainerTMarginOfSeg(para) = 0;
		if (value > 0 && 0 < cur_vars->ui_info->line_height)
		    _DtCvContainerTMarginOfSeg(para) = value *
					(cur_vars->ui_info->line_height / 2);
		break;

	    /*
	     * description [string | "string" | 'string']
	     */
	    case 'd':
		result = GetStringParameter(cur_vars, _DtCvTRUE, _DtCvTRUE,
					_DtCvFALSE, _DtCvFALSE, description);
		break;

	    /*
	     * firstindent value
	     */
	    case 'f':
		if (GetValueParameter (cur_vars, _DtCvFALSE, &value) == -1)
		    return -1;

		_DtCvContainerFMarginOfSeg(para) =
					value * cur_vars->ui_info->avg_char;
		break;

	    /*
	     * glink      [string | "string" | 'string']
	     * gpos       value_string
	     * graphic    string
	     * gspace     value
	     * gtypelink  value
	     */
	    case 'g':
		/*
		 * to determine what token this is we must look at the
		 * second character.
		 */
		(cur_vars->rd_ptr)++;

		/*
		 * do we need to read more?
		 */
		if (*(cur_vars->rd_ptr) == '\0'
					&& GetNextBuffer(cur_vars) == -1)
		    return -1;

		/*
		 * check for the next permutation
		 */
		switch (_DtCvToLower(*(cur_vars->rd_ptr)))
		  {
		    /*
		     * glink [string | "string" | 'string']
		     */
		    case 'l':
			result = GetStringParameter(cur_vars, _DtCvTRUE,
				_DtCvTRUE, _DtCvFALSE, _DtCvFALSE, glink);
			break;

		    /*
		     * gpos value_string
		     */
		    case 'p':
			result = GetStringParameter(cur_vars, _DtCvTRUE,
				_DtCvTRUE, _DtCvFALSE, _DtCvFALSE, &tmpString);

			if (-1 != result && _DtCvToLower (*tmpString) == 'r')
			    *gpos = _DtCvJUSTIFY_RIGHT;

			if (tmpString)
			    free (tmpString);
			break;

		    /*
		     * graphic    string
		     */
		    case 'r':
			result = GetStringParameter(cur_vars, _DtCvTRUE,
				_DtCvTRUE, _DtCvFALSE, _DtCvFALSE, file_name);
			break;

		    /*
		     * gspace     value
		     */
		    case 's':
			if (GetValueParameter(cur_vars,_DtCvFALSE,&value) == -1
								|| value < 0)
			    return -1;

			*gspace = value * cur_vars->ui_info->avg_char;
			break;

		    /*
		     * gtypelink  value
		     */
		    case 't':
			if (GetValueParameter(cur_vars,_DtCvFALSE,glinktype) == -1
							|| *glinktype < 0)
			    return -1;

			break;
		  }
		break;

	    /*
	     * id string
	     */
	    case 'i':
		/*
		 * get the id string
		 */
		if (GetStringParameter(cur_vars, _DtCvTRUE, _DtCvTRUE,
				_DtCvFALSE, _DtCvFALSE, &tmpString) == -1)
		    return -1;

		_DtCvContainerIdOfSeg(para) = tmpString;
		break;

	    /*
	     * label [string | "string" | 'string']
	     * leftindent value
	     */
	    case 'l':
		/*
		 * to determine what token this is we must look at the
		 * second and possibly the third characters.
		 */
		(cur_vars->rd_ptr)++;

		/*
		 * do we need to read more?
		 */
		if (*(cur_vars->rd_ptr) == '\0'
					&& GetNextBuffer(cur_vars) == -1)
		    return -1;

		/*
		 * check for the next permutation
		 */
		if (_DtCvToLower(*(cur_vars->rd_ptr)) == 'a')
		  {
		    /*
		     * label
		     */
		    if (GetStringParameter(cur_vars, _DtCvTRUE, _DtCvFALSE,
				_DtCvFALSE, _DtCvFALSE, &tmpString) == -1)
			return -1;

		    /*
		     * If we got a label process it.
		     */
		    if (ret_label == NULL || *ret_label)
		      {
		        /*
		         * we've already processed a label!
		         * ignore this one!
		         */
			if (tmpString)
		            free (tmpString);
		      }
		    else
			*ret_label = tmpString;
		  }
		else if (_DtCvToLower(*(cur_vars->rd_ptr)) == 'e')
		  {
		    /*
		     * leftindent
		     */
		    if (GetValueParameter (cur_vars, _DtCvFALSE, &value) == -1)
			return -1;

		    _DtCvContainerLMarginOfSeg(para) =
					value * cur_vars->ui_info->avg_char;
		  }
		else
		    result = -1;
		break;

	    /*
	     * nowrap
	     */
	    case 'n':
		_DtCvContainerTypeOfSeg(para) = _DtCvLITERAL;
		*frmt_type = _DtCvLITERAL;
		break;

	    /*
	     * rightindent value
	     */
	    case 'r':
		if (GetValueParameter (cur_vars, _DtCvFALSE, &value) == -1)
		    return -1;

		_DtCvContainerRMarginOfSeg(para) =
					value * cur_vars->ui_info->avg_char;
		break;

	    /*
	     * wrap
	     */
	    case 'w':
		_DtCvContainerTypeOfSeg(para) = _DtCvDYNAMIC;
		*frmt_type = _DtCvDYNAMIC;
		break;

	    /*
	     * Found an option we don't understand.
	     */
	    default:
		result = -1;
	  }
	optionCount++;
      }

    /*
     * adjust the first margin to correctly indicate the offset from the
     * left margin. In the old CCDF, the first margin indicated left plus
     * and additional indent. For the new Canvas Engine, it is suppose
     * to be an addition on top of the left margin.
     */
    _DtCvContainerFMarginOfSeg(para) = _DtCvContainerLMarginOfSeg(para) -
					_DtCvContainerFMarginOfSeg(para);
    if (-1 == result)
	optionCount = 0;

    return (optionCount - 1);

} /* End GetParagraphParameters */

/******************************************************************************
 * Function:	int FlowingParagraph (
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:	Process the <PARAGRAPH> specification.
 *
 *****************************************************************************/
static	int
FlowingParagraph(
    FormatVariables	*cur_vars,
    ProcessState	 cur_state,
    _DtCvFrmtOption 	 frmt_flags,
    int			 allowed,
    int			 link_idx,
    int			 ret_on_nl,
    int			 fnt_flag,
    _DtCvFrmtOption	 gpos,
    _DtCvUnit		 gspace,
    char		*file_name,
    _DtHelpFontHints	*font_attr)
{
    int		type     = 0;
    int		result   = -1;
    _DtCvSegment *graphSeg;
    _DtCvSegment *bodySeg;
    SegList	flowList = InitList;

    /*
     * reset current list
     */
    cur_vars->my_list = InitList;

    /*
     * create two containers.
     */
    if (0 == CheckList(&(flowList), 2, 2))
      {
	/*
	 * point to the containers for the graphic and body.
	 */
	graphSeg = flowList.list;
	bodySeg  = flowList.list;
	bodySeg++;

	/*
	 * set the bottom margins to zero.
	 */
	graphSeg->type = _DtCvSetTypeToContainer(graphSeg->type);
	bodySeg->type  = _DtCvSetTypeToContainer(bodySeg->type);
	_DtCvContainerBMarginOfSeg(graphSeg) = 0;
	_DtCvContainerBMarginOfSeg(bodySeg) = 0;

	/*
	 * so the first segment is the container for the graphic.
	 * set the controller flag and values.
	 */
	graphSeg->type = _DtCvSetTypeToController(graphSeg->type);
	_DtCvContainerFlowOfSeg(graphSeg)    = _DtCvWRAP;
	_DtCvContainerPercentOfSeg(graphSeg) = 0;
	_DtCvContainerOrientOfSeg(graphSeg)  = gpos;
	if (_DtCvJUSTIFY_LEFT == gpos)
	  {
	    _DtCvContainerJustifyOfSeg(graphSeg) = gpos;
	    _DtCvContainerRMarginOfSeg(graphSeg) = gspace;
	  }
	else
	  {
	    _DtCvContainerJustifyOfSeg(graphSeg) = gpos;
	    _DtCvContainerLMarginOfSeg(graphSeg) = gspace;
	  }

	/*
	 * mark the first segment as used.
	 */
	flowList.cnt++;

	/*
	 * check for hypertext link.
	 */
	if (link_idx > -1)
	    type = _DtCvHYPER_TEXT;

	/*
	 * re-set the segment list and create the graphic.
	 */
	if (0 == CreateSaveGraphic(cur_vars, type, file_name, link_idx))
	  {
	    /*
	     * so the first segment in cur_vars is a region.
	     * attach it to the graphic container.
	     */
	    _DtCvContainerListOfSeg(graphSeg) = cur_vars->my_list.list;

	    /*
	     * now process the following information as the body of
	     * the paragraph as the list for the non-controller
	     * container.
	     */
	    cur_vars->my_list = InitList;
	    if (-1 != Parse (CCDF_PARAGRAPH_CMD, cur_state, cur_vars,
						0, frmt_flags,
						font_attr, -1,
						Specials, allowed,
						ret_on_nl, fnt_flag))
	      {
		/*
		 * establish the links between the segments
		 */
		TerminateSegList(&(cur_vars->my_list), True);

		/*
		 * if there was a segment list generated,
		 * attach the it to the non-controller
		 * container and mark it as used.
		 */
		if (NULL != cur_vars->my_list.list)
		  {
	            _DtCvContainerListOfSeg(bodySeg) = cur_vars->my_list.list;
		    flowList.cnt++;
		  }
		result = 0;
	      }
	  }
      }

    cur_vars->my_list        = flowList;
    TerminateSegList(&(cur_vars->my_list), True);
    return result;
}

/******************************************************************************
 * Function:	int ProcessParagraph (
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:	Process the <PARAGRAPH> specification.
 *
 *****************************************************************************/
static	int
ProcessParagraph(
    FormatVariables	*cur_vars,
    ProcessState	 cur_state,
    int			 fnt_flag)
{
    int		 result   = 0;
    int		 labelFnd = False;
    int		 allowed = ~(CCDF_TOPIC_CMD | CCDF_TITLE_CMD | CCDF_ABBREV_CMD);
    int		 oldMbLenMax = cur_vars->cur_mb_max;
    int		 glinktype   = -1;
    int		 linkIndex   = -1;
    _DtCvUnit	 gspace      = 0;
    char	*glinkSpec   = NULL;
    char	*label       = NULL;
    char	*fileName    = NULL;
    char	*description = NULL;
    char	 numChar[16];

    ProcessState	 myState = NormalState;
    _DtCvFrmtOption	 gpos;
    _DtCvFrmtOption	 frmtType = _DtCvDYNAMIC;
    _DtCvSegment	*paraSeg;
    _DtCvSegment	*labelSeg = NULL;
    _DtCvSegment	*col1     = NULL;
    _DtCvSegment	*col2     = NULL;
    _DtCvSegment	**childList;
    SegList		 oldList;
    SegList		 tableList = InitList;
    _DtHelpFontHints	 fontAttrs;

    /*
     * remember the old font list.
     * initialize the font quark list
     * and use the char set specified for this topic.
     */
    _DtHelpCeCopyDefFontAttrList (&fontAttrs);
    cur_vars->cur_mb_max               = cur_vars->topic_mb_max;
    _DtHelpFontHintsLang(fontAttrs)    = cur_vars->topic_lang;
    _DtHelpFontHintsCharSet(fontAttrs) = cur_vars->topic_char_set;

    /*
     * Make next segment in my parent's list for a container for this
     * paragraph.
     */
    if (-1 == CheckSegList(cur_vars))
	return -1;

    /*
     * remember the parent list
     * make sure the defaults are set
     */
    oldList           = cur_vars->my_list;
    paraSeg           = NextAvailSeg(cur_vars->my_list);
    paraSeg->handle.container = DefContainer;
    paraSeg->type     = _DtCvSetTypeToContainer(paraSeg->type);
    _DtCvContainerLeadingOfSeg(paraSeg) = cur_vars->ui_info->leading;
    cur_vars->my_list = InitList;

    /*
     * get the parameters on the paragraph
     * and set the default for the bottom.
     */
    result = GetParagraphParameters (cur_vars, paraSeg, &frmtType,
				&gpos, &glinkSpec, &glinktype, &gspace,
				&label, &fileName, &description);

    /*
     * for lists, we will inherit our parent's bottom margin
     * when we return to the loop processing the list. Therefore,
     * set our bottom margin to zero if we are part of a label body.
     */
    if (0 == result && LabelBody == cur_state)
	_DtCvContainerBMarginOfSeg(paraSeg) = 0;
    else if (result > 0)
	result = 0;

    /*
     * check for a label specification. Either as a 'label' parameter
     * or a the <LABEL> command.
     */
    if (-1 != result)
      {
	/*
	 * check for the parameter
	 */
	if (NULL != label)
	  {
	    FormatVariables   oldVars = *cur_vars;

	    cur_vars->rd_buf  = label;
	    cur_vars->rd_flag = 0;
	    cur_vars->rd_ptr  = label;
	    cur_vars->my_file = NULL;

	    result = Parse (CCDF_LABEL_CMD, NormalState, cur_vars,
					0, frmtType,
					&fontAttrs, -1,
					Specials,
					(CCDF_FONT_CMD | CCDF_OCTAL_CMD),
					False, fnt_flag);

	    if (result != -1)
		result = CheckSaveSegment (_DtCvSTRING, &fontAttrs,
								-1, cur_vars);

	    TerminateSegList(&(cur_vars->my_list), True);
	    cur_vars->rd_buf  = oldVars.rd_buf;
	    cur_vars->rd_flag = oldVars.rd_flag;
	    cur_vars->rd_ptr  = oldVars.rd_ptr;
	    cur_vars->my_file = oldVars.my_file;
	    cur_vars->last_was_space = True;

	    labelSeg = cur_vars->my_list.list;
	    cur_vars->my_list = InitList;
	    labelFnd = True;
	  }
	/*
	 * check for the command.
	 */
	else if (0 == _DtHelpCeCheckNextCcdfCmd("lab", cur_vars->my_file,
						cur_vars->rd_buf,
						cur_vars->rd_size, 1,
						&(cur_vars->rd_ptr)))
	  {
	    result = ProcessLabelCmd(cur_vars, 0, frmtType,
						&fontAttrs, -1,
						allowed, False, fnt_flag,
						&labelSeg);
	    labelFnd = True;
	  }

	/*
	 * if a label was processed, create the containers for it.
	 */
	if (-1 != result && True == labelFnd)
	  {
	    /*
	     * set the correct state.
	     */
	    myState = LabelBody;

	    /*
	     * create containers for the label and it's body
	     */
	    result = CheckList(&tableList, 2, 2);
	    if (-1 != result)
	      {
		/*
		 * set the top and bottom margins on both the child
		 * containers (so they are the same) to reflect
		 * the list's before/after values.
		 */
		/*
		 * set the pointer to the first column
		 * overwrite the left and right margin values.
		 */
		col1       = NextAvailSeg(tableList);
		col1->type = _DtCvSetTypeToContainer(col1->type);
		_DtCvContainerTMarginOfSeg(col1) =
					_DtCvContainerTMarginOfSeg(paraSeg);
		_DtCvContainerBMarginOfSeg(col1) =
					_DtCvContainerBMarginOfSeg(paraSeg);
		_DtCvContainerLMarginOfSeg(col1) = 0;
		_DtCvContainerRMarginOfSeg(col1) = cur_vars->ui_info->avg_char;
		tableList.cnt++;

		/*
		 * set the pointer for the second column
		 * and set the top/bottom values.
		 */
		col2       = NextAvailSeg(tableList);
		col2->type = _DtCvSetTypeToContainer(col2->type);
		_DtCvContainerTMarginOfSeg(col2) =
					_DtCvContainerTMarginOfSeg(paraSeg);
		_DtCvContainerBMarginOfSeg(col2) =
					_DtCvContainerBMarginOfSeg(paraSeg);
		tableList.cnt++;

		/*
		 * create an id for this label
		 */
		sprintf(numChar, "%d", cur_vars->cell_cnt++);
		_DtCvContainerIdOfSeg(col1) = (char *) malloc (
				strlen("&CCDF_RES_") + strlen(numChar) + 1);
		if (NULL != _DtCvContainerIdOfSeg(col1))
		  {
		    /*
		     * copy over the id and attach the list.
		     */
		    strcpy(_DtCvContainerIdOfSeg(col1), "&CCDF_RES_");
		    strcat(_DtCvContainerIdOfSeg(col1), numChar);
		    _DtCvContainerListOfSeg(col1) = labelSeg;

		    /*
		     * set the id for the second column.
		     */
		    sprintf(numChar, "%d", cur_vars->cell_cnt++);
		    _DtCvContainerIdOfSeg(col2) = (char *) malloc (
				strlen("&CCDF_RES_") + strlen(numChar) + 1);
		    if (NULL != _DtCvContainerIdOfSeg(col2))
		      {
			strcpy(_DtCvContainerIdOfSeg(col2), "&CCDF_RES_");
			strcat(_DtCvContainerIdOfSeg(col2), numChar);
		      }
		    else
			result = -1;
		  }
		else
		    result = -1;
	      }
	  }
      }

    /*
     * disallow labels from here on out. Either one was found and processed
     * or not.
     */
    allowed &= (~(CCDF_LABEL_CMD));

    /*
     * Now check and process the graphic specification.
     */
    if (-1 != result)
      {
	/*
	 * initialize the segment list for the children of the paragraph.
	 */
	cur_vars->my_list = InitList;

	/*
	 * check for a graphic in the specification.
	 * If so, then we need to create, as the list for the
	 * paragraph container, two containers; one container
	 * that is a controller and has the graphic and the
	 * other contains the rest of the content of the
	 * paragraph.
	 *
	 * First, create the link index for the graphic.
	 */
	if (NULL != glinkSpec)
	  {
	    /*
	     * but if there isn't a graphic, throw away the link
	     * specifications since they are useless!
	     */
	    if (NULL != fileName && strlen (glinkSpec))
	      {
		linkIndex = _DtLinkDbAddLink (cur_vars->my_links,
					NULL,
					glinkSpec,
					ReturnLinkType(glinktype, glinkSpec),
					ReturnLinkWinHint(glinktype),
					description);

		/*
		 * had problems creating the link index, bail
		 */
		if (linkIndex == -1)
		    result = -1;
	      }
	  }

	/*
	 * now create the graphic and process the graphic body..
	 * or just process the rest of the paragraph....
	 */
	if (0 == result)
	  {
	    if (NULL != fileName)
	        result = FlowingParagraph(cur_vars,
					myState,
					frmtType,
					allowed, linkIndex,
					False, fnt_flag,
					gpos, gspace, fileName,
					&fontAttrs);
	    else
	      {
		result = Parse (CCDF_PARAGRAPH_CMD, myState, cur_vars,
					0, frmtType,
					&fontAttrs, -1,
					Specials, allowed,
					False, fnt_flag);
		if (-1 != result)
		  {
		    /*
		     * establish the links
		     */
		    TerminateSegList (&(cur_vars->my_list), True);
		  }
	      }
	  }

	if (-1 != result)
	  {
	     char	**colW;
	     _DtCvFrmtOption	*colJ;

	    if (True == labelFnd)
	      {
		char *colW1 = "1";		/* default bulletted list */
		char *colW2 = "99,0,98";	/* default bulletted list */
		char *ids;

		/*
		 * set the body of the label in it's container.
		 */
		_DtCvContainerListOfSeg(col2) = cur_vars->my_list.list;

		/*
		 * figure out which column widths should be used.
		 */
		if (14 * cur_vars->ui_info->avg_char
					== _DtCvContainerLMarginOfSeg(paraSeg))
		  {
		    /* Labeled lists */
		    colW1 = "20,20,10";
		    colW2 = "80,10,20";
		  }
		else if (3 * cur_vars->ui_info->avg_char
					== _DtCvContainerLMarginOfSeg(paraSeg))
		    /* Ordered lists */
		    colW1 = "1,98,0";
		else if (0 == _DtCvContainerLMarginOfSeg(paraSeg))
		    /* Plain lists */
		    colW1 = "0";

		/*
		 * create the column width strings.
		 */
		colW = NULL;
		colW = (char **) _DtCvAddPtrToArray((void **) colW,
						((void *) strdup(colW1)));
		if (NULL != colW)
		    colW = (char **) _DtCvAddPtrToArray((void **) colW,
						((void *) strdup (colW2)));

		/*
		 * create the array for column justification.
		 */
		colJ = (_DtCvFrmtOption *) malloc (sizeof(_DtCvFrmtOption) * 2);
		if (NULL != colJ)
		  {
		    colJ[0] = _DtCvJUSTIFY_LEFT;
		    colJ[1] = _DtCvJUSTIFY_LEFT;
		  }

		/*
		 * set this segment's type to a table and fill in all
		 * the relevant information.
		 */
		paraSeg->type                        = _DtCvTABLE;
		_DtCvNumColsOfTableSeg(paraSeg)      = 2;
		_DtCvColWOfTableSeg(paraSeg)         = colW;
		_DtCvColJustifyOfTableSeg(paraSeg)   = colJ;
		_DtCvJustifyCharsOfTableSeg(paraSeg) = NULL;

		/*
		 * now create the list of ids in this table.
		 */
		ids = (char *) malloc (
				strlen(_DtCvContainerIdOfSeg(col1)) +
				strlen(_DtCvContainerIdOfSeg(col2)) + 2);
		if (NULL != ids)
		  {
		    strcpy(ids, _DtCvContainerIdOfSeg(col1));
		    strcat(ids, " ");
		    strcat(ids, _DtCvContainerIdOfSeg(col2));
		  }
		_DtCvCellIdsOfTableSeg(paraSeg) = NULL;
		_DtCvCellIdsOfTableSeg(paraSeg) = (char **)
			_DtCvAddPtrToArray(
				(void **) _DtCvCellIdsOfTableSeg(paraSeg),
				(void *) ids);

		/*
		 * now create the list of cells in the table.
		 */
		_DtCvCellsOfTableSeg(paraSeg) = NULL;
		_DtCvCellsOfTableSeg(paraSeg) = (_DtCvSegment **)
				_DtCvAddPtrToArray(
					(void **) _DtCvCellsOfTableSeg(paraSeg),
					(void  *) col1);
		if (NULL != _DtCvCellsOfTableSeg(paraSeg))
		    _DtCvCellsOfTableSeg(paraSeg) = (_DtCvSegment **)
				_DtCvAddPtrToArray(
					(void **) _DtCvCellsOfTableSeg(paraSeg),
					(void  *) col2);

		if (NULL == colW || NULL == colJ || NULL == ids
				|| NULL == _DtCvCellIdsOfTableSeg(paraSeg)
				|| NULL == _DtCvCellsOfTableSeg(paraSeg))
		    result = -1;
	      }
	    else
	      {
		_DtCvSegment *pSeg;

		/*
		 * attach the list to my container.
		 */
	        _DtCvContainerListOfSeg(paraSeg) = cur_vars->my_list.list;

		/*
		 * consolidate any table children I have.
		 */
		for (pSeg = cur_vars->my_list.list, labelSeg = NULL;
				-1 != result && NULL != pSeg;
							pSeg = pSeg->next_seg)
		  {
		    if (_DtCvIsSegTable(pSeg))
		      {
			if (NULL != labelSeg)
			  {
			    /*
			     * bypass this segment since it will no
			     * longer be 'used' by consolidating it
			     * with another label set.
			     */
			    labelSeg->next_seg = pSeg->next_seg;

			    /*
			     * free the other justification and width info.
			     */
			    free((void *) _DtCvColJustifyOfTableSeg(pSeg));
			    colW = _DtCvColWOfTableSeg(pSeg);
			    free(*colW++);
			    free(*colW);
			    free(_DtCvColWOfTableSeg(pSeg));

			    /*
			     * add the segments to the list
			     */
			    childList = _DtCvCellsOfTableSeg(pSeg);
			    _DtCvCellsOfTableSeg(labelSeg) = (_DtCvSegment **)
				_DtCvAddPtrToArray(
				    (void **) _DtCvCellsOfTableSeg(labelSeg),
					*childList);
			    childList++;
			    if (NULL != _DtCvCellsOfTableSeg(labelSeg))
				_DtCvCellsOfTableSeg(labelSeg) = (_DtCvSegment **)
				    _DtCvAddPtrToArray(
					(void **)_DtCvCellsOfTableSeg(labelSeg),
					*childList);
			    free((void *) _DtCvCellsOfTableSeg(pSeg));

			    /*
			     * add the ids to the list
			     */
			    _DtCvCellIdsOfTableSeg(labelSeg) = (char **)
				_DtCvAddPtrToArray(
				    (void **)_DtCvCellIdsOfTableSeg(labelSeg),
				    (void  *)(*(_DtCvCellIdsOfTableSeg(pSeg))));

			    free((void *) _DtCvCellIdsOfTableSeg(pSeg));

			    if (NULL == _DtCvCellIdsOfTableSeg(labelSeg)
				|| NULL == _DtCvCellsOfTableSeg(labelSeg))
				result = -1;
			  }
			else
			  {
			    /*
			     * this segment becomes the segment holding
			     * the consolidated table.
			     */
			    labelSeg = pSeg;
			  }
		      }
		    else
			labelSeg = NULL;
		  }
	      }
	  }
      }

    /*
     * restore the font MB_CUR_MAX
     */
    cur_vars->cur_mb_max = oldMbLenMax;

    /*
     * free all the strings
     */
    if (label != NULL)
        free (label);
    if (fileName != NULL)
        free (fileName);

    /*
     * free the linkSpec and description,
     */
    if (NULL != description)
	free(description);
    if (NULL != glinkSpec)
	free(glinkSpec);

    if (result == -1)
	return -1;

    oldList.cnt++;
    cur_vars->my_list        = oldList;
    cur_vars->last_was_space = True;
    return 0;

} /* End ProcessParagraph */

/******************************************************************************
 * Function:	int ProcessHypertext (
 *					FormatVariables *cur_vars, int segType,
 *					char **font_attr, int flags )
 *
 * Parameters:
 *		segType		Specifies the type of segment currently
 *				being processed.
 *		font_attr		Specifies the list of font quarks to
 *				associate with the string.
 *		flags		Specifies the formatting commands allowed.
 * 
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Process the <LINK> specification.
 *
 *****************************************************************************/
static	int
ProcessHypertext(
    FormatVariables	*cur_vars,
    ProcessState	 cur_state,
    unsigned long	 seg_flags,
    _DtCvFrmtOption	 frmt_type,
    _DtHelpFontHints	*font_attr,
    int			 flags,
    int			 ret_on_nl,
    int			 fnt_flag)
{
    int    result = 0;
    int    hyperType;
    int    hyperIndex;
    char  *description = NULL;
    char  *hyperlink = NULL;

    /*
     * is a <LINK> command allowed here?
     */
    if (CCDF_NOT_ALLOW_CMD(flags, CCDF_LINK_CMD))
	return -1;

    /*
     * get the hypertext type
     */
    if (GetValueParameter (cur_vars, _DtCvTRUE, &hyperType) < 0)
	return -1;

    if (hyperType < 0)
      {
	errno = CEErrorHyperType;
	return -1;
      }

    /*
     * get the hypertext link spec.
     */
    result = GetStringParameter(cur_vars, _DtCvTRUE, _DtCvTRUE, _DtCvFALSE,
							_DtCvFALSE, &hyperlink);
    if (0 == result)
      {
        /*
         * See if we have the optional description string
         */
        result = GetStringParameter (cur_vars, _DtCvFALSE, _DtCvTRUE,
					_DtCvFALSE, _DtCvFALSE, &description);
        if (result == 1)
	    description = NULL;

	result = _DtLinkDbAddLink (cur_vars->my_links,
					NULL,
					hyperlink,
					ReturnLinkType(hyperType, hyperlink),
					ReturnLinkWinHint(hyperType),
					description);
	if (NULL != description)
	    free(description);

	free(hyperlink);
      }

    /*
     * if no problems encountered, start parsing with this
     * hypertext link.
     */
    if (result != -1)
      {
	hyperIndex = result;
        result = FindEndMarker (cur_vars);
        if (!result)
	  {
	    /*
	     * set the hypertext flag and type.
	     */
	    seg_flags = _DtCvSetTypeToHyperText(seg_flags);

	    result = Parse (CCDF_LINK_CMD, cur_state, cur_vars,
						seg_flags, frmt_type,
						font_attr,
						hyperIndex,
						Specials, flags,
						ret_on_nl, fnt_flag);
	  }
      }

    /*
     * Don't free the hypertext string or description.
     * It is now owned by the Link List.
     */

    if (result == -1)
	return -1;

    return 0;

} /* End ProcessHypertext */

/******************************************************************************
 * Function:	int ProcessLabelCmd (
 *				FormatVariables *cur_vars, int seg_type,
 *				char **font_attr, int link_spec,
 *				int flags )
 *
 * Parameters:
 *		font_attr	Specifies the list of font quarks to
 *				associate with the string.
 *		link_spec	Specifies the hypertext link to associate
 *				with the string.
 *		flags		Specifies the formatting commands allowed.
 * 
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Process the <LABEL> specification.
 *
 *****************************************************************************/
static	int
ProcessLabelCmd(
    FormatVariables	*cur_vars,
    unsigned long	 seg_flags,
    _DtCvFrmtOption	 frmt_type,
    _DtHelpFontHints	*font_attr,
    int			 link_spec,
    int			 flags,
    int			 ret_on_nl,
    int			 fnt_flag,
    _DtCvSegment	**ret_list)
{
    SegList	oldList = cur_vars->my_list;

    /*
     * is a <LABEL> command allowed?
     */
    if (CCDF_NOT_ALLOW_CMD(flags, CCDF_LABEL_CMD))
	return -1;

    /*
     * can't have another <LABEL> command inside this one
     */
    flags &= (~(CCDF_LABEL_CMD));

    /*
     * Find the end marker
     */
    if (FindEndMarker (cur_vars) != 0)
	return -1;

    cur_vars->my_list = InitList;
    if (Parse (CCDF_LABEL_CMD, NormalState, cur_vars, 
					seg_flags,
					frmt_type,
					font_attr,
					link_spec,
					Specials, flags,
					ret_on_nl, fnt_flag) == -1)
	return -1;

    /*
     * set the links, return the segment list and restore the old list.
     */
    TerminateSegList(&(cur_vars->my_list), True);
    *ret_list = cur_vars->my_list.list;
    cur_vars->my_list = oldList;

    /*
     * Indicate that preceding space on the next text should be ignored
     */
    cur_vars->last_was_space = True;

    return 0;

} /* End ProcessLabelCmd */

/******************************************************************************
 * Function:	int ProcessFigureCmd (
 *					FormatVariables *cur_vars,
 *					char **font_attr)
 *
 * Parameters:
 *		font_attr		Specifies the list of font quarks to
 *				associate with the string.
 * 
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Process the <FIGURE> specification.
 *
 *****************************************************************************/
static	int
ProcessFigureCmd(
    FormatVariables	*cur_vars,
    ProcessState	 cur_state,
    _DtCvFrmtOption	 frmt_type,
    _DtHelpFontHints	*font_attr,
    int			 ret_on_nl,
    int			 fnt_flag)
{
    int		cnt;
    int		done         = False;
    int		result       = 0;
    int		linkType     = CCDF_LINK_JUMP_REUSE;
    int		linkIndex    = -1;
    int		segType      = _DtCvREGION;
    char	*description = NULL;
    char	*idString    = NULL;
    char	*filename    = NULL;
    char	*linkspec    = NULL;
    _DtCvFrmtOption	vertOrientCap  = _DtCvJUSTIFY_BOTTOM;
    _DtCvFrmtOption	horzOrientCap  = _DtCvJUSTIFY_CENTER;
    _DtCvFrmtOption	justifyCap     = _DtCvJUSTIFY_CENTER;
    _DtCvFrmtOption	justifyGraphic = _DtCvJUSTIFY_CENTER;
    _DtCvSegment	*figContainer;
    SegList		oldList;
    SegList		capList;
    SegList		figList = InitList;

    /*
     * create a container for this figure (and possibly a caption)
     */
    if (-1 == CheckSegList(cur_vars))
	return -1;

    figContainer       = NextAvailSeg(cur_vars->my_list);
    figContainer->handle.container = DefContainer;
    figContainer->type = _DtCvSetTypeToContainer(figContainer->type);
    _DtCvContainerLeadingOfSeg(figContainer) = cur_vars->ui_info->leading;

    /*
     * remember the old list and initialize for the figure.
     */
    oldList = cur_vars->my_list;
    cur_vars->my_list = InitList;

    /*
     * process the parameters of the figure cmd.
     */
    while (!done && result != -1)
      {
	if (SkipToNextToken (cur_vars, _DtCvFALSE) == -1)
	  {
	    result = -1;
	    continue;
	  }

	switch (_DtCvToLower(*(cur_vars->rd_ptr)))
	  {
	    /*
	     * end of figure spec
	     */
	    case '>':
		/*
		 * move past the end of token marker
		 * and skip the leading blanks in the caption.
		 */
		(cur_vars->rd_ptr)++;
		while (result != -1 && !done)
		  {
		    if (*(cur_vars->rd_ptr) == '\0')
			result = GetNextBuffer (cur_vars);
		    else if ((cur_vars->cur_mb_max == 1 ||
			mblen(cur_vars->rd_ptr, cur_vars->cur_mb_max) == 1)
						&& *(cur_vars->rd_ptr) == ' ')
			(cur_vars->rd_ptr)++;
		    else
			done = True;
		  }
		break;

	    /*
	     * ccenter
	     * center
	     * cbottom
	     * cleft
	     * cright
	     * ctop
	     */
	    case 'c':
		/*
		 * Go to the next character.
		 */
		(cur_vars->rd_ptr)++;

		/*
		 * Do we need to read more information?
		 */
		if (*(cur_vars->rd_ptr) == '\0' && GetNextBuffer (cur_vars) == -1)
		    result = -1;

		if (result != -1)
		  {
		    /*
		     * look at the next charager to determine the token.
		     */
		    switch (_DtCvToLower(*(cur_vars->rd_ptr)))
		      {
		        case 'c': /* caption center */
			    justifyCap    = _DtCvJUSTIFY_CENTER;
			    horzOrientCap = _DtCvJUSTIFY_CENTER;
			    break;

		        case 'e': /* graphic centered */
			    justifyGraphic = _DtCvJUSTIFY_CENTER;
			    break;

		        case 'b': /* caption below graphic */
			    vertOrientCap = _DtCvJUSTIFY_BOTTOM;
			    break;

		        case 'l': /* caption justified left */
			    justifyCap    = _DtCvJUSTIFY_LEFT;
			    horzOrientCap = _DtCvJUSTIFY_LEFT_MARGIN;
			    break;

		        case 'r': /* caption right justified */
			    justifyCap    = _DtCvJUSTIFY_RIGHT;
			    horzOrientCap = _DtCvJUSTIFY_RIGHT_MARGIN;
			    break;

		        case 't': /* caption at top */
			    vertOrientCap = _DtCvJUSTIFY_TOP;
			    break;
		      }
		  }
		break;

	    /*
	     * description [string | "string" | 'string']
	     */
	    case 'd':
		result = GetStringParameter (cur_vars, _DtCvTRUE, _DtCvTRUE,
					_DtCvFALSE, _DtCvFALSE, &description);
		break;

	    /*
	     * file
	     */
	    case 'f':
		result = GetStringParameter (cur_vars, _DtCvTRUE, _DtCvTRUE,
					_DtCvFALSE, _DtCvFALSE, &filename);
		break;

	    /*
	     * id string
	     */
	    case 'i':
		/*
		 * get the id string
		 */
		result = GetStringParameter (cur_vars, _DtCvTRUE, _DtCvTRUE,
					_DtCvFALSE, _DtCvFALSE, &idString);
		/*
		 * if this figure had an id associated with it, create
		 * marker segment for it.
		 */
		if (-1 != result && NULL != idString && '\0' != *idString)
		  {
		    /*
		     * create the marker. If there is an error, free the
		     * string.
		     */
		    result = CreateMarker(cur_vars, idString);
		    if (-1 == result)
		        free (idString);
		  }
		break;

	    /*
	     * left
	     * link [string | "string" | 'string']
	     */
	    case 'l':
		/*
		 * Go to the next character.
		 */
		(cur_vars->rd_ptr)++;

		/*
		 * Do we need to read more information?
		 */
		if (*(cur_vars->rd_ptr) == '\0' && GetNextBuffer (cur_vars) == -1)
		    result = -1;

		if (result != -1)
		  {
		    /*
		     * look at the next charager to determine the token.
		     */
		    switch (_DtCvToLower(*(cur_vars->rd_ptr)))
		      {
		        case 'e':
			    justifyGraphic = _DtCvJUSTIFY_LEFT;
			    break;

		        case 'i':
			    segType = _DtCvSetTypeToHyperText (segType);
			    result = GetStringParameter (cur_vars, _DtCvTRUE,
					_DtCvTRUE, _DtCvFALSE, _DtCvFALSE,
					&linkspec);
			    break;

		        default:
			    result = -1;
		      }
		  }
		break;

	    /*
	     * right
	     */
	    case 'r':
		justifyGraphic = _DtCvJUSTIFY_RIGHT;
		break;

	    /*
	     * typelink value
	     */
	    case 't':
		if (GetValueParameter (cur_vars, _DtCvTRUE, &linkType) == -1)
		    result = -1;
		else if (linkType < 0)
		  {
		    errno = CEErrorHyperType;
		    result = -1;
		  }
		break;

	    default:
		result = -1;
	  }
      }

    if (result != -1 && (filename == NULL || strlen(filename) == 0))
	result = -1;

    if (result != -1)
      {
	/*
	 * check to see if the hypertext flag is set but we don't
	 * have a link specification.
	 */
        if (IsTypeHyperText(segType) && !linkspec)
	  {
	    segType = segType & ~(_DtCvHYPER_TEXT);

	    if (NULL != description)
	        free (description);
	    description = NULL;
	  }

	/*
	 * do we still have a good hypertext?
	 * if so, create a link index for it.
	 */
	if (IsTypeHyperText(segType))
	  {
	    linkIndex = _DtLinkDbAddLink (cur_vars->my_links,
					NULL,
					linkspec,
					ReturnLinkType(linkType, linkspec),
					ReturnLinkWinHint(linkType),
					description);
	    if (linkIndex == -1)
		result = -1;
	  }
      }

    if (result != -1)
      {
	/*
	 * parse the caption.  If there is a caption, we will need to
	 * wrap it and the figure in containers with the appropriate
	 * controller flags set to get the correct layout.
	 */
	result = Parse (CCDF_FIGURE_CMD, cur_state, cur_vars,
	   			 0, frmt_type,
				 font_attr, -1,
				Specials,
				(CCDF_NEWLINE_CMD | CCDF_LINK_CMD |
				 CCDF_FONT_CMD    | CCDF_ID_CMD),
				 ret_on_nl, fnt_flag);

	/*
	 * save the caption List.
	 */
	TerminateSegList(&(cur_vars->my_list), True);
	capList = cur_vars->my_list;

	/*
	 * is there a caption? If so, create two segments instead
	 * of one.
	 */
	cnt = 1;
	if (-1 != result && 0 != capList.cnt)
	    cnt = 2;

	/*
	 * allocate room for the figure (and caption)
	 */
	if (-1 != result)
	    result = CheckList(&figList, cnt, cnt);

	if (-1 != result)
	  {
	    /*
	     * process any caption that was specified
	     */
	    if (0 != capList.cnt)
	      {
	        _DtCvSegment *pSeg = figList.list;

		/*
		 * create a controller for the caption.
		 */
		pSeg->type = _DtCvSetTypeToController(
					_DtCvSetTypeToContainer(pSeg->type));

		/*
		 * set its orientation and justification.
		 */
		_DtCvContainerOrientOfSeg(pSeg)  = horzOrientCap;
		_DtCvContainerVOrientOfSeg(pSeg) = vertOrientCap;
		_DtCvContainerJustifyOfSeg(pSeg) = justifyCap;
		_DtCvContainerBMarginOfSeg(pSeg) = 0;

		/*
		 * attach the caption to the container.
		 */
		_DtCvContainerListOfSeg(pSeg) = capList.list;

		/*
		 * indicate this segment has been used.
		 */
		figList.cnt++;
	      }

	    /*
	     * now load the graphic into the next available segment
	     * allocated for the figure.
	     */
	    cur_vars->my_list = figList;
	    result = CreateSaveGraphic (cur_vars, segType, filename, linkIndex);

	    /*
	     * if no problems, attach the figure (and caption) to the
	     * wrapper container
	     */
	    if (-1 != result)
	      {
		/*
		 * set the links
		 */
		TerminateSegList(&(cur_vars->my_list), True);

		/*
		 * attach the figure (and caption) to the wrapper
		 */
		_DtCvContainerListOfSeg(figContainer) = cur_vars->my_list.list;

		/*
		 * set the justify to the correct value to act on the
		 * figure.
		 */
		_DtCvContainerJustifyOfSeg(figContainer) = justifyGraphic;

		/*
		 * indicate that this segment has been used 
		 */
		oldList.cnt++;
	      }
	  }
      }

    /*
     * restore the segment list.
     * and free memory.
     */
    cur_vars->my_list = oldList;
    free (filename);

    /*
     * don't free the link string or description,
     * the link list owns them now.
     */

    return result;

} /* End ProcessFigureCmd */

/******************************************************************************
 * Function:	int ProcessInLine (FormatVariables cur_vars,
 *					int seg_type, int link_spec )
 *
 * Parameters:
 *		seg_type	Specifes the type of segment currently
 *				being processed.
 *		link_spec	Specifies the hypertext link associated
 *				with this segment.
 * 
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Process a <GRAPHIC> specification.
 *
 *****************************************************************************/
static	int
ProcessInLine(
    FormatVariables	*cur_vars,
    int			seg_type,
    int			link_spec )
{
    int		done = False;
    int		result = 0;
    char	*idString = NULL;
    char	*filename = NULL;

    /*
     * process the graphic parameters
     */
    while (!done && result != -1)
      {
	if (SkipToNextToken (cur_vars, _DtCvFALSE) == -1)
	    return -1;

	switch (_DtCvToLower(*(cur_vars->rd_ptr)))
	  {
	    /*
	     * end of in line spec
	     */
	    case '>':
		/*
		 * move past the end of token marker
		 */
		(cur_vars->rd_ptr)++;
		done = True;
		break;

	    /*
	     * file
	     */
	    case 'f':
		result = GetStringParameter (cur_vars, _DtCvTRUE, _DtCvTRUE,
					_DtCvFALSE, _DtCvFALSE, &filename);
		break;

	    /*
	     * id string
	     */
	    case 'i':
		/*
		 * get the id string
		 */
		result = GetStringParameter (cur_vars, _DtCvTRUE, _DtCvTRUE,
					_DtCvFALSE, _DtCvFALSE, &idString);
		/*
		 * if this graphic had an id associated with it, create
		 * marker segment for it.
		 */
		if (-1 != result && NULL != idString && '\0' != *idString)
		  {
		    /*
		     * create the marker. If there is an error, free the
		     * string.
		     */
		    result = CreateMarker(cur_vars, idString);
		    if (-1 == result)
		        free (idString);
		  }

		break;

	    default:
		result = -1;
	  }
      }

    if (result != -1 && (filename == NULL || strlen(filename) == 0))
	result = -1;

    if (result != -1)
      {
	/*
	 * create the graphic.
	 */
	result = CreateSaveGraphic (cur_vars,
					_DtCvSetTypeToInLine(seg_type),
					filename, link_spec);
	cur_vars->last_was_space = False;
      }

    free (filename);

    return result;

} /* End ProcessInLine */

/******************************************************************************
 * Function:	int CheckIdString (
 *				FormatVariables *cur_vars, int segType,
 *				char **font_attr, int linkspec,
 *				int flags)
 * 
 * Parameters:
 *		segType		Specifies the type of segment currently
 *				being processed.
 *		font_attr		Specifies the list of font quarks to
 *				associate with the string.
 *		linkspec	Specifies the hypertext link associated
 *				with the segment.
 *		flags		Specifies the formatting commands allowed.
 * 
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Create a marker at this location.
 * 
 *****************************************************************************/
static	int
CheckIdString(
    FormatVariables	*cur_vars,
    ProcessState	 cur_state,
    unsigned long	 seg_flags,
    _DtCvFrmtOption	 frmt_type,
    _DtHelpFontHints	*font_attr,
    int			 linkspec,
    int			 flags,
    int			 ret_on_nl,
    int			 fnt_flag)
{
    int    result = -1;
    char  *ptr    = NULL;

    /*
     * is it legal to have a <ID> command at this point?
     */
    if (CCDF_NOT_ALLOW_CMD (flags, CCDF_ID_CMD))
	return -1;

    /*
     * get the id string
     * clear out any current information in the buffer
     * and make sure there is a segment available.
     */
    if (-1 != GetStringParameter (cur_vars, _DtCvTRUE, _DtCvTRUE,
						_DtCvFALSE, _DtCvFALSE, &ptr)
				&&
	-1 != CheckSaveSegment (seg_flags, font_attr, linkspec, cur_vars))
      {
	/*
	 * create the marker. If there is an error, free the
	 * string.
	 */
	result = CreateMarker(cur_vars, ptr);
	if (-1 == result)
	    free (ptr);

	/*
	 * find the end of the <ID> syntax
	 * and parse the data between the begin <ID> and </ID>.
	 */
	if (0 == result && -1 != FindEndMarker (cur_vars) &&
		-1 != Parse (CCDF_ID_CMD, cur_state, cur_vars,
						seg_flags, frmt_type,
						font_attr,
						linkspec,
						Specials, flags,
						ret_on_nl, fnt_flag))
	    result = 0;
      }

    /*
     * return an error code if necessary
     */
    return result;

} /* End CheckIdString */

/******************************************************************************
 * Function:	int	Parse (int cur_cmd, 
 *				FormatVariables *cur_vars, int segType,
 *				char **font_attr,
 *				int  linkspec, int allowed)
 * 
 * Parameters:
 *		cur_cmd		Specifies the current formatting command
 *				being processed.
 *		parent		Specifies the parent paragraph.
 *		segType		Specifies the type of segment currently
 *				being processed.
 *		font_attr		Specifies the list of font quarks to
 *				associate with the string.
 *		linkspec	Specifies the hypertext link associated
 *				with the segment.
 *		allowed		Specifies the formatting commands allowed.
 * 
 * Returns:
 *		1 if ran into a </> specification.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Parse the data.
 *
 *****************************************************************************/
static	int
Parse(
    int			  cur_cmd,
    ProcessState	  cur_state,
    FormatVariables	 *cur_vars,
    unsigned long	  seg_flags,
    _DtCvFrmtOption	  frmt_type,
    _DtHelpFontHints	*font_attr,
    int			  linkspec,
    const char		 *scan_string,
    int			  allowed,
    int			  ret_on_nl,
    int			  fnt_flag)
{
    int      charSize = 1;
    int	     leftOver = 0;
    int	     done = False;
    int	     fontType = False;
    int	     cmdType  = False;
    char    *ptr;

    while (!done)
      {
	if (cur_vars->cur_mb_max != 1)
	    charSize = mblen(cur_vars->rd_ptr, cur_vars->cur_mb_max);

	if (charSize == 1)
	  {
	    /*
	     * check to see if a newline was the previous character.
	     * If so, it may need to be replaced with a space.
	     */
	    if (cur_vars->last_was_nl == True &&
			AppendSpaceToInfo(cur_vars, NULL, frmt_type) == -1)
		return -1;

	    cur_vars->last_was_nl = False;
	    cur_vars->last_was_mb = False;

	    switch (*(cur_vars->rd_ptr))
	      {
		case '<':
		    /*
		     * Go to the next character.
		     */
		    (cur_vars->rd_ptr)++;

		    /*
		     * determine the cmd
		     */
		    cmdType = _DtHelpCeGetCcdfCmd (cur_cmd, cur_vars->rd_buf,
							&(cur_vars->rd_ptr),
							cur_vars->my_file,
							cur_vars->rd_size,
							allowed);
		    switch (cmdType)
		      {
			/*
			 * <figure>
			 */
			case CCDF_FIGURE_CMD:
			    	if (CheckSaveSegment(seg_flags, font_attr,
					linkspec, cur_vars) == -1
						||
					ProcessFigureCmd(cur_vars,
						cur_state,
						frmt_type,
						font_attr,
						ret_on_nl, fnt_flag) == -1)
				    return -1;
				break;

			/*
			 * <angle>
			 * <characterset>
			 * <size>
			 * <spacing>
			 * <type>
			 * <weight>
			 */
			case CCDF_FONT_CMD:
				fontType = _DtHelpCeGetCcdfFontType (cur_vars->rd_ptr);
				done     = ChangeFont(fontType, seg_flags,
							frmt_type,
							font_attr , linkspec,
							cur_vars,
							cur_state,
							allowed,
							ret_on_nl,
							fnt_flag);

				break;

			/*
			 * </>
			 */
			case CCDF_FORMAT_END:
				if (CheckSaveSegment(seg_flags, font_attr,
						linkspec, cur_vars) == -1 ||
					FindEndMarker (cur_vars) == -1)
				    return -1;

				return 1;

			/*
			 * <graphic>
			 */
			case CCDF_GRAPHIC_CMD:
				/*
				 * clear out any information
				 * in the buffer and then
				 * process the figure.
				 */
				if (CheckSaveSegment (seg_flags, font_attr,
						linkspec, cur_vars) == -1
							||
					ProcessInLine (cur_vars, seg_flags,
								linkspec) == -1)
				    done = -1;
				break;

			/*
			 * <id>
			 */
			case CCDF_ID_CMD:
				done = CheckIdString (cur_vars,
						      cur_state,
						      seg_flags,
						      frmt_type,
						      font_attr,
						      linkspec,
						      allowed,
						      ret_on_nl, fnt_flag);
				break;

			/*
			 * <link>
			 */
			case CCDF_LINK_CMD:
				if (CheckSaveSegment (seg_flags, font_attr,
						linkspec, cur_vars) == -1
					||
				     ProcessHypertext(cur_vars, cur_state,
							seg_flags, frmt_type,
						  font_attr, allowed,
						  ret_on_nl, fnt_flag) == -1)
				    done = -1;
				break;

			/*
			 * <newline>
			 */
			case CCDF_NEWLINE_CMD:
				if (SaveNewLine (cur_vars, seg_flags, font_attr,
						linkspec) == -1
						||
					FindEndMarker (cur_vars) == -1)
				    done = -1;
				break;

			case CCDF_OCTAL_CMD:
				if (AppendOctalToInfo(cur_vars,
							cur_vars->rd_ptr) == -1
					|| FindEndMarker (cur_vars) == -1)
				    done = -1;
				break;

			/*
			 * <paragraph>
			 */
			case CCDF_PARAGRAPH_CMD:
				if (CheckSaveSegment (seg_flags, font_attr,
						linkspec, cur_vars) == -1
						||
				      ProcessParagraph(cur_vars, cur_state,
							fnt_flag) == -1)
				    done = -1;
				break;

			/*
			 * <label>
			 * this should never be hit if the markup is
			 * correct. Processing labels is through the
			 * the paragraph processor now.
			 */
			case CCDF_LABEL_CMD:

			/*
			 * unknown command.
			 */
			default:
				return -1;
		      }
		    break;

		case '\n':
			/*
			 * Go to the next character
			 */
			(cur_vars->rd_ptr)++;

			/*
			 * If processing a static segment, we want to
			 * keep the author defined end-of-lines.
			 *
			 * Otherwise, we throw them away.
			 */
			if (_DtCvLITERAL == frmt_type || True == ret_on_nl)
			  {
			    done = SaveStringAsSegments (cur_vars,
					_DtCvSetTypeToNewLine(seg_flags),
					font_attr, linkspec);

			    if (ret_on_nl)
				return 0;
			  }
			else if (cur_vars->last_was_space == False)
			    cur_vars->last_was_nl = True;

			break;

		case '\t':
			/*
			 * go past this character.
			 */
			(cur_vars->rd_ptr)++;

			/*
			 * append 1-8 characters on the end of the buffer.
			 */
			_DtHelpCeCountChars(cur_vars->fmt_buf, MB_CUR_MAX,
								&leftOver);
			leftOver = leftOver % 8;
			ptr = ((char *) SpaceString) + leftOver;
			done = _DtHelpCeAddStrToBuf (&ptr,
						&(cur_vars->fmt_buf),
						&(cur_vars->fmt_size),
						&(cur_vars->fmt_buf_max),
						(8 - leftOver),
						INFO_GROW);
			break;

		case '\\':
			/*
			 * The author has escaped a character.
			 * Increment to the escaped character.
			 */
			(cur_vars->rd_ptr)++;

			/*
			 * Do we need to read more information?
			 */
			if (*(cur_vars->rd_ptr) == '\0' &&
						GetNextBuffer (cur_vars) == -1)
			    return -1;
			/*
			 * If we didn't read more information or
			 * was successful on the read, save the
			 * escaped character.
			 */
			done = AppendCharToInfo (cur_vars, &(cur_vars->rd_ptr));
			break;

		case ' ':
			/*
			 * Put a space in the segment.
			 */
			done = AppendSpaceToInfo (cur_vars,
						&(cur_vars->rd_ptr), frmt_type);
			break;

		default:
			/*
			 * put the information in the buffer
			 */
			if (AppendToInfo (cur_vars, &(cur_vars->rd_ptr),
							scan_string) == -1)
			    return -1;
	      }

	  }
	else if (charSize > 1)
	  {
	    if ((cur_vars->ui_info->nl_to_space == 1 ||
			(cur_vars->last_was_mb == False
					&& cur_vars->last_was_nl == True))
	         && AppendSpaceToInfo(cur_vars, NULL, frmt_type) == -1)
		done = -1;
		
	    cur_vars->last_was_nl = False;
	    cur_vars->last_was_mb = True;
	    if (AppendToInfo (cur_vars, &(cur_vars->rd_ptr), scan_string) == -1)
		return -1;
	  }
	else if (charSize < 0)
	  {
	    /*
	     * we have either invalid characters or part of a multi-byte
	     * character. Read the next buffer for more info.
	     */
	     leftOver = strlen (cur_vars->rd_ptr);
	     if (leftOver < ((int) MB_CUR_MAX))
	       {
		if (GetNextBuffer (cur_vars) == -1)
		    done = -1;
	       }
	     else
	       {
		 /*
		  * In trouble brothers and sisters. We have garbage in the
		  * buffer - BAIL OUT!
		  */
		 done = -1;
	       }
	  }
	if (!done && *(cur_vars->rd_ptr) == '\0')
	  {
	    /*
	     * We're at the end of the buffer, can we read more?
	     */
	    if (cur_vars->rd_flag > 0 && GetNextBuffer (cur_vars) == -1)
		return -1;

	    if (cur_vars->rd_flag == 0)
		done = True;
          }
      }

    /*
     * DO NOT MODIFY 'done' HERE
     * If you do, the caller is unable to test for return value == 1!!!!
     */
    if (done == -1 ||
	CheckSaveSegment (seg_flags, font_attr, linkspec, cur_vars) == -1)
	return -1;

    return done;
}

/*****************************************************************************
 * Function:	int ParseTitle (FormatVariables cur_vars)
 *
 * Parameters:
 *		cur_vars	Specifies the current values for formatting.
 *
 * Returns:	0 if successful, -1 if failure.
 *
 * errno Values:
 *
 * Purpose:	ParseTitle gets the data between a <TITLE> and </TITLE>
 *		pair; putting it in a controlling container.
 *
 *****************************************************************************/
static	int
ParseTitle(
    FormatVariables	*cur_vars,
    int			 cont_flag,
    int			 skip_abbrev,
    int			 fnt_flag,
    _DtHelpFontHints	*fontAttrs)
{
    int	result = -1;
    SegList oldList = cur_vars->my_list;

    /*
     * rest the current segment list
     */
    cur_vars->my_list  = InitList;

    /*
     * check for the <TITLE> directive.
     */
    if (_DtHelpCeCheckNextCcdfCmd ("ti", cur_vars->my_file, cur_vars->rd_buf,
			cur_vars->rd_size, 1, &(cur_vars->rd_ptr)) != 0
		|| FindEndMarker (cur_vars) != 0)
	return -1;

    /*
     * Parse will return
     *    1 when a </> is found,
     *    0 if the data runs out before we finish parsing,
     *   -1 if errors.
     *
     * A return of 1 is required from Parse
     */
    if (1 == Parse (CCDF_TITLE_CMD, NormalState, cur_vars,
			0, _DtCvDYNAMIC,
			fontAttrs, -1, Specials,
			~(CCDF_TOPIC_CMD | CCDF_TITLE_CMD | CCDF_ABBREV_CMD |
			CCDF_PARAGRAPH_CMD | CCDF_FIGURE_CMD | CCDF_LABEL_CMD),
			False, fnt_flag))
      {
	/*
	 * skip the any abbreviation that might be there
	 */
	result = 0;
	if (skip_abbrev)
	    result = _DtHelpCeSkipCcdfAbbrev (cur_vars->my_file,
				cur_vars->rd_buf,
				&(cur_vars->rd_ptr), cur_vars->rd_size,
				cur_vars->cur_mb_max);

	/*
	 * if we successfully skipped the abbreviation and there is
	 * a title - then attach the title body to the controlling
	 * container (and make the container a controller).
	 */
	if (result != -1 && cur_vars->my_list.list)
	  {
	    _DtCvSegment	*list = oldList.list;
	    int			 cnt  = oldList.cnt;

	    TerminateSegList (&(cur_vars->my_list), True);

	    /*
	     * when this segment was allocated, it was initialized
	     * to the correct vertical and horizontal orientation
	     * for a CCDF title.  Simply set the type to container
	     * and controller to get it to act properly.
	     */
	    if (NULL != list)
	      {
		list[cnt].type = _DtCvSetTypeToContainer(list[cnt].type);
		if (True == cont_flag)
		    list[cnt].type = _DtCvSetTypeToController(list[cnt].type);

		/*
		 * attach the title body to the controller container.
		 */
		_DtCvContainerListOfSeg(&list[cnt]) = cur_vars->my_list.list;

		/*
		 * and count this segment as used.
		 */
		oldList.cnt++;
              }
	    /*
	     * otherwise, a container hasn't been created for this list.
	     * but we don't want to loose the information.
	     */
	    else
		oldList.list = cur_vars->my_list.list;
          }
      }

    /*
     * restore segment usage information.
     */
    cur_vars->my_list = oldList;

    return result;
}

/*****************************************************************************
 * Function:	int Format (FormatVariables cur_vars, char *id_string,
 *
 * Parameters:
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:	Format is the top entry point for formating Help Files with
 *		formatting information into a form understood by a display area.
 *		It will keep calling Parse, until the entire topic is read
 *		or an error  occurs.
 *
 *****************************************************************************/
static	int
Format(
    FormatVariables	*cur_vars,
    _FrmtUiInfo		*ui_info,
    char		*id_string,
    _DtCvTopicPtr	*ret_topic )
{
    int			 cnt;
    int			 result  = 0;
    int			 done    = 0;
    char		*charSet = NULL;
    char		 readBuf[BUFF_SIZE];
    _DtHelpFontHints     fontAttrs;
    _DtCvTopicInfo	*topicHandle;
    SegList		 myList = InitList;

    /*
     * malloc space for the topic handle.
     */
    topicHandle = (_DtCvTopicInfo *) malloc (sizeof(_DtCvTopicInfo));
    if (topicHandle == NULL)
	return -1;

    /*
     * initialize the font attributes to the default hints
     */
    _DtHelpCeCopyDefFontAttrList (&fontAttrs);

    /*
     * set up the formatting variable structure
     */
    if (InitStructure (cur_vars, ui_info, readBuf, BUFF_SIZE) == -1)
	return -1;

    /*
     * read the first buffer's worth of the topic.
     */
    cur_vars->rd_flag = _DtHelpCeReadBuf (cur_vars->my_file, cur_vars->rd_buf,
						cur_vars->rd_size);
    if (cur_vars->rd_flag == -1)
	return -1;

    /*
     * The first command in the buffer MUST be the <TOPIC> command.
     * It is in a one byte charset.
     */
    if (_DtHelpCeGetCcdfTopicCmd (((void *) NULL), cur_vars->my_file,
			cur_vars->rd_buf, &(cur_vars->rd_ptr),
			cur_vars->rd_size, 1, &charSet) == -1)
	return -1;

    /*
     * change the character set to the returned character set
     * Assume that the charset is 'iso8859' if not specified.
     */
    cur_vars->cur_mb_max = 1;
    if (NULL != charSet)
	GetMbLen(cur_vars, charSet, &(_DtHelpFontHintsLang(fontAttrs)),
					&(_DtHelpFontHintsCharSet(fontAttrs)));

    /*
     * Remember the topic code set and MB_CUR_MAX.
     */
    cur_vars->topic_char_set = _DtHelpFontHintsCharSet(fontAttrs);
    cur_vars->topic_lang     = _DtHelpFontHintsLang(fontAttrs);
    cur_vars->topic_mb_max   = cur_vars->cur_mb_max;

    /*
     * allocate two segments for the topic - one for the <title>
     * and one for the body of the topic.
     */
    if (0 != CheckList(&(cur_vars->my_list), 2, 2))
      {
	if (NULL != charSet)
	  {
	    free(_DtHelpFontHintsLang(fontAttrs));
	    free(_DtHelpFontHintsCharSet(fontAttrs));
	    free(charSet);
	  }
	return -1;
      }

    /*
     * get the title and skip the abbreviation.
     */
    if (0 != ParseTitle(cur_vars, True, True, 0, &fontAttrs))
      {
	if (NULL != charSet)
	  {
	    free(_DtHelpFontHintsLang(fontAttrs));
	    free(_DtHelpFontHintsCharSet(fontAttrs));
	    free(charSet);
	  }
	return -1;
      }

    /*
     * remember this current list since we will be attaching any more
     * information as a child of these segments.
     */
    myList = cur_vars->my_list;

    /*
     * process the rest of the information
     */
    while (0 == result && !done)
      {
	/*
	 * reset the segment count before processing the information
	 * for this paragraph
	 */
	cur_vars->my_list = InitList;

	/*
	 * make sure we've got a container for this paragraph
	 */
	done = CheckList(&myList, 1, GROW_SIZE);

	/*
	 * now process the information while finding paragraphs.
	 */
	if (0 == done)
	    do {
	        done = Parse (CCDF_PARAGRAPH_CMD, NormalState, cur_vars,
			0, _DtCvDYNAMIC,
			&fontAttrs, -1, Specials,
			~(CCDF_TOPIC_CMD | CCDF_TITLE_CMD | CCDF_ABBREV_CMD),
			False, 0);
	    } while (0 == done);

	/*
	 * if no errors, attach the resulting child information to the
	 * paragraph container.
	 */
	if (done != -1)
	  {
	    /*
	     * clean up the last segment.
	     */
	    if (cur_vars->my_list.list)
	      {
		_DtCvSegment	*list = NextAvailSeg(myList);

	        TerminateSegList (&(cur_vars->my_list), True);

		/*
		 * make it a container.
		 */
		list->type = _DtCvSetTypeToContainer(list->type);

		/*
		 * attach the information to the container
		 */
		_DtCvContainerListOfSeg(list) = cur_vars->my_list.list;

		/*
		 * and count this container as used.
		 */
		myList.cnt++;
	      }
	  }
	else
	    result = -1;
      }

    /*
     * If we had errors, deallocate the memory.
     */
    if (result == -1)
	_DtHelpFreeSegments(myList.list, _DtCvFALSE, ui_info->destroy_region, 
							ui_info->client_data);
    else
      {
	/*
	 * Tighten up the paragraph structures if too many allocated.
	 */
	if (0 != myList.cnt)
	    TerminateSegList (&(myList), True);
	else
	  {
	    free ((char *) myList.list);
	    myList.list = NULL;
	  }
      }

    /*
     * return the values, even if they are null and zero.
     */

    if (id_string == NULL)
        topicHandle->id_str = id_string;
    else
        topicHandle->id_str = strdup(id_string);

    topicHandle->seg_list  = myList.list;
    topicHandle->mark_list = NULL;
    topicHandle->link_data = cur_vars->my_links;

    *ret_topic = (void *) topicHandle;

    /*
     * free memory
     */
    if (NULL != charSet)
      {
	free(_DtHelpFontHintsLang(fontAttrs));
	free(_DtHelpFontHintsCharSet(fontAttrs));
	free(charSet);
      }

    if (result == -1)
	return -1;

    return 0;
}

/*****************************************************************************
 * Function:	int FormatCCDFTitle (FormatVariables cur_vars
 *
 * Parameters:
 *		cur_vars	Specifies the current values for formatting.
 *		id_string	Specifies the id to look for or NULL.
 *		ret_para	Returns a pointer to a list of _DtCvSegment
 *				structures.
 *		ret_num		Specifies the number of structures in
 *				'ret_para'.
 *		ret_seg		Specifies the segment containing 'id_string'
 *				or NULL.
 *
 * Returns:
 *
 * errno Values:
 *
 * Purpose:	Format is the top entry point for formating Help Files with
 *		formatting information into a form understood by a display area.
 *		It will keep calling Parse, until the entire topic is read
 *		or an error  occurs.
 *
 *****************************************************************************/
static	int
FormatCCDFTitle(
    FormatVariables	*cur_vars,
    _DtHelpFontHints	 fontAttrs,
    char		*filename,
    int			 offset,
    int			 level,
    int			 fnt_flags,
    int			 want_abbrev)
{
    char    *strPtr;
    char    *charSet   = NULL;
    int      result    = 0;
    int      origCnt   = cur_vars->my_list.cnt;
    int      len;
    _DtCvSegment	*titleSeg;
    _DtCvSegment	*list;

    cur_vars->rd_buf[0] = '\0';
    cur_vars->rd_ptr    = cur_vars->rd_buf;

    if (_DtHelpCeFileOpenAndSeek(filename, offset, -1,
					&(cur_vars->my_file), NULL) != 0)
	return -1;

    cur_vars->rd_flag = _DtHelpCeReadBuf (cur_vars->my_file, cur_vars->rd_buf,
						cur_vars->rd_size);

    if (cur_vars->rd_flag == -1)
      {
	_DtHelpCeBufFileClose (cur_vars->my_file, True);
	return -1;
      }

    /*
     * The first command in the buffer MUST be the <TOPIC> command.
     * It is in a one byte charset.
     */
    if (_DtHelpCeGetCcdfTopicCmd (((void *) NULL), cur_vars->my_file,
			cur_vars->rd_buf, &(cur_vars->rd_ptr),
			cur_vars->rd_size, 1, &charSet) == -1)
      {
	_DtHelpCeBufFileClose (cur_vars->my_file, True);
	return -1;
      }

    /*
     * change the character set to the returned character set
     * Assume that the charset is 'iso8859' if not specified.
     */
    cur_vars->cur_mb_max = 1;
    if (NULL != charSet)
	GetMbLen(cur_vars, charSet, &(_DtHelpFontHintsLang(fontAttrs)),
					&(_DtHelpFontHintsCharSet(fontAttrs)));

    /*
     * Remember the topic code set and MB_CUR_MAX.
     */
    cur_vars->topic_char_set = _DtHelpFontHintsCharSet(fontAttrs);
    cur_vars->topic_lang     = _DtHelpFontHintsLang(fontAttrs);
    cur_vars->topic_mb_max   = cur_vars->cur_mb_max;

    /*
     * allocate another segment for this title.
     */
    if (0 != CheckSegList(cur_vars))
      {
	if (NULL != charSet)
	  {
	    free(_DtHelpFontHintsLang(fontAttrs));
	    free(_DtHelpFontHintsCharSet(fontAttrs));
	    free(charSet);
	  }
	_DtHelpCeBufFileClose(cur_vars->my_file, True);
	return -1;
      }

    /*
     * set the left margin correctly.
     */
    titleSeg  = NextAvailSeg(cur_vars->my_list);
    titleSeg->handle.container = DefContainer;
    titleSeg->type = _DtCvSetTypeToContainer(titleSeg->type);
    _DtCvContainerLMarginOfSeg(titleSeg) = level*2*cur_vars->ui_info->avg_char;
    _DtCvContainerBMarginOfSeg(titleSeg) = 0;
    _DtCvContainerTypeOfSeg(titleSeg)    = _DtCvLITERAL;
    _DtCvContainerLeadingOfSeg(titleSeg) = cur_vars->ui_info->leading;

    /*
     * Parse the title.
     */
    if (-1 == ParseTitle(cur_vars, False, False, fnt_flags, &fontAttrs))
      {
	if (NULL != charSet)
	  {
	    free(_DtHelpFontHintsLang(fontAttrs));
	    free(_DtHelpFontHintsCharSet(fontAttrs));
	    free(charSet);
	  }
	_DtHelpCeBufFileClose(cur_vars->my_file, True);
	return -1;
      }

    /*
     * We've parsed the title. see if we want the abbrev.
     */
    /*
     * if the title is null or we want the abbrev,
     * process the abbreviation.
     */
    if (NULL == _DtCvContainerListOfSeg(titleSeg) || want_abbrev)
      {
	SegList	titleList = cur_vars->my_list;

	/*
	 * reset the buffer to get the abbreviation
	 */
	if (cur_vars->fmt_buf != NULL)
	  {
	    free(cur_vars->fmt_buf);
	    cur_vars->fmt_buf     = NULL;
	    cur_vars->fmt_size    = 0;
	    cur_vars->fmt_buf_max = 0;
	  }

	result = _DtHelpCeGetCcdfAbbrevCmd (cur_vars->my_file, cur_vars->rd_buf,
				cur_vars->rd_size, cur_vars->cur_mb_max,
				&(cur_vars->rd_ptr), &(cur_vars->fmt_buf));

	/*
	 * If no errors getting the abbreviation, save it
	 */
	if (result != -1)
	  {
	    /*
	     * save the abbreviation
	     */
	    if (cur_vars->fmt_buf != NULL)
	      {
		/*
		 * If there was a title, free it
		 */
		if (NULL != _DtCvContainerListOfSeg(titleSeg))
		    _DtHelpFreeSegments(_DtCvContainerListOfSeg(titleSeg),
					_DtCvFALSE, 
					cur_vars->ui_info->destroy_region,
						cur_vars->ui_info->client_data);

		/*
		 * save the abbreviation
		 */
		cur_vars->my_list      = InitList;
		cur_vars->fmt_size     = strlen (cur_vars->fmt_buf);
		cur_vars->fmt_buf_max  = cur_vars->fmt_size + 1;
		if (cur_vars->fmt_size > 0)
		    result = SaveStringAsSegments (cur_vars, 0, &fontAttrs, -1);

		if (result != -1)
		    TerminateSegList(&(cur_vars->my_list), True);

		titleSeg->type = _DtCvSetTypeToContainer(titleSeg->type);
		_DtCvContainerListOfSeg(titleSeg) = cur_vars->my_list.list;
		titleList.cnt++;
	      }
	  }

	/*
	 * eliminate this segment if no title or abbrev was found.
	 */
	if (NULL == _DtCvContainerListOfSeg(titleSeg))
	  {
	    titleList.cnt = origCnt;
	    _DtCvContainerTypeOfSeg(titleSeg) = _DtCvDYNAMIC;
	  }

	/*
	 * reset the title list
	 */
	cur_vars->my_list = titleList;
      }
    else
	result = _DtHelpCeSkipCcdfAbbrev (cur_vars->my_file, cur_vars->rd_buf,
				&(cur_vars->rd_ptr), cur_vars->rd_size,
				cur_vars->cur_mb_max);

    if (cur_vars->fmt_buf != NULL)
	cur_vars->fmt_buf[0] = '\0';
    cur_vars->fmt_size     = 0;

    /*
     * remove the newline from the lists and
     * change all the newlines on the segments into a space.
     */
    for (list = _DtCvContainerListOfSeg(titleSeg);
					NULL != list; list = list->next_seg)
      {
	if (_DtCvIsSegNewLine(list))
	  {
	    /*
	     * clear the newline flag.
	     */
	    list->type &= ~(_DtCvNEW_LINE);

	    /*
	     * check to see if the last character of this segment or
	     * the first character of the next segment is a space.
	     * if not, add one.
	     */
	    if (NULL != list->next_disp && _DtCvIsSegString(list)
						&& _DtCvIsSegRegChar(list))
	      {
		/*
		 * is the last character of the string a space?
		 * if so do nothing.
		 */
		len = strlen((char *) _DtCvStringOfStringSeg(list)) - 1;
		if (' ' != ((char *) _DtCvStringOfStringSeg(list))[len])
		  {
		    /*
		     * is the next segment a one byte string?
		     * and if so, is the first character a space?
		     * if so, do nothing.
		     */
		    if (_DtCvIsSegString(list->next_disp)
			&& _DtCvIsSegRegChar(list->next_disp)
			&& ' ' != *((char *)_DtCvStringOfStringSeg(list->next_disp)))
		      {
			/*
			 * need to add a space to one of these strings.
			 */
			len += 3;
			_DtCvStringOfStringSeg(list) = (char *) realloc(
					(void *) _DtCvStringOfStringSeg(list),
					sizeof(char *) * len);
			if (NULL == _DtCvStringOfStringSeg(list))
			    return -1;

			strPtr = (char *) _DtCvStrPtr(
						_DtCvStringOfStringSeg(list),
						0,
						len - 3);
			*strPtr++ = ' ';
			*strPtr   = '\0';
		      }
		  }
	      }
	  }
      }

    _DtHelpCeBufFileClose (cur_vars->my_file, True);

    /*
     * free memory
     */
    if (NULL != charSet)
      {
	free(_DtHelpFontHintsLang(fontAttrs));
	free(_DtHelpFontHintsCharSet(fontAttrs));
	free(charSet);
      }

    if (result == -1)
	return -1;

    return 0;
}

/******************************************************************************
 * Function:	int FormatEntryInToc (
 *
 * Parameters:
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:
 *
 ******************************************************************************/
static int
FormatEntryInToc(
    _DtHelpVolumeHdl	  volume,
    char		 *loc_id,
    FormatVariables	 *cur_vars,
    _DtHelpFontHints	  font_attrs,
    int			  level)
{
    int		  result    = -1;
    int		  tocLen    = 0;
    int		  offset;
    int		  num;
    char	 *strPtr = NULL;
    _DtCvSegment *pSeg;
    _DtCvSegment *titleSeg;
    SegList       titleList;

    /*
     * check to see that there is a segment for this title.
     */
    if (0 != CheckSegList(cur_vars))
	return -1;

    /*
     * The next segment in the list will become the next title.
     * Get a pointer to it for later use.
     */
    titleList = cur_vars->my_list;
    titleSeg  = NextAvailSeg(titleList);
    titleSeg->handle.container = DefContainer;
    titleSeg->type = _DtCvSetTypeToContainer(titleSeg->type);
    _DtCvContainerBMarginOfSeg(titleSeg) = 0;
    _DtCvContainerLeadingOfSeg(titleSeg) = cur_vars->ui_info->leading;

    /*
     * Get the file and offset for the topic.
     * and process the topic's title, ignoring most font changes.
     */
    if (True == _DtHelpCeFindId(volume, loc_id, -1, &strPtr, &offset) &&
	-1 != FormatCCDFTitle(cur_vars, font_attrs, strPtr, offset, level,
  ((1<<_CEFONT_SIZE)|(1<<_CEFONT_WEIGHT)|(1<<_CEFONT_ANGLE)|(1<<_CEFONT_TYPE)),
				    False))
      {
	/*
	 * now create the hypertext link index for the ghostlink.
	 */
	num    = _DtLinkDbAddLink(cur_vars->my_links, NULL, loc_id,
					    _DtCvLinkType_SameVolume,
					    _DtCvWindowHint_CurrentWindow,
					    NULL);
	/*
	 * if a link index was successfully created, process the item.
	 */
	if (num != -1)
	  {
	    result = 0;

	    /*
	     * check to see if this title had any segments.
	     */
	    pSeg = _DtCvContainerListOfSeg(titleSeg);
	    if (NULL != pSeg)
	      {
		while (NULL != pSeg)
		  {
		    /*
		     * If this segment has a hypertext link, remove it.
		     */
		    if (pSeg->link_idx != -1)
			_DtLinkDbRemoveLink(cur_vars->my_links, pSeg->link_idx);
    
		    /*
		     * change the link index and flag to the ghost link.
		     */
		    pSeg->link_idx  = num;
		    pSeg->type     &= ~(_DtCvHYPER_TEXT);
		    pSeg->type      = _DtCvSetTypeToGhostLink(pSeg->type);

		    /*
		     * go to the next segment.
		     */
		    pSeg = pSeg->next_seg;
		  }
	      }
	    else
	      {
		/*
		 * no title or abbreviation, so use the location id.
		 * allocate room for it and put it between parens.
		 */
		tocLen = strlen (loc_id) + 9;
		if (tocLen > cur_vars->fmt_buf_max)
		  {
		    if (NULL == cur_vars->fmt_buf)
			cur_vars->fmt_buf = (char *) malloc (tocLen);
		    else
			cur_vars->fmt_buf = (char *) realloc(
					    cur_vars->fmt_buf,
					    tocLen);
		    cur_vars->fmt_buf_max = tocLen;
		  }
    
		cur_vars->fmt_size = tocLen - 1;
		strcpy (cur_vars->fmt_buf, "...(");
		strcat (cur_vars->fmt_buf, loc_id);
		strcat (cur_vars->fmt_buf, ")...");
    
		/*
		 * don't overwrite the title's
		 */
		cur_vars->my_list = InitList;

		/*
		 * save the string off and then attach to the title
		 * container.
		 */
		result = SaveStringAsSegments(cur_vars, _DtCvGHOST_LINK,
							&font_attrs, num);
		if (-1 != result)
		    _DtCvContainerListOfSeg(titleSeg) = cur_vars->my_list.list;

		/*
		 * restore the title list.
		 */
		cur_vars->my_list = titleList;
	      }
	  }
      }

   if (NULL != strPtr)
       free(strPtr);

   return result;

}  /* End FormatEntryInToc */

/******************************************************************************
 * Function:	int FormatExpandToc (
 *				_DtHelpVolumeHdl volume,
 *
 * Purpose:
 ******************************************************************************/
static int
FormatExpandedToc(
    _DtHelpVolumeHdl	  volume,
    char		  *cur_id,
    char		 **path_lst,
    FormatVariables	 *cur_vars,
    _DtHelpFontHints	  font_attrs,
    int			  level)
{
    int    result = 0;

    if (*path_lst != NULL)
      {
	font_attrs.weight = _DtHelpFontWeightMedium;
	if (_DtCvStrCaseCmpLatin1(*path_lst, cur_id) == 0)
	    font_attrs.weight = _DtHelpFontWeightBold;

        result = FormatEntryInToc(volume, *path_lst, cur_vars,
				font_attrs, level);

        if (result != -1)
          {
	    char **children;
	    char **childLst;
	    int    count = _DtHelpCeGetCcdfTopicChildren(volume,
							*path_lst, &children);

	    /*
	     * if there are children, format them
	     */
	    level++;
	    path_lst++;
	    childLst = children;
	    font_attrs.weight = _DtHelpFontWeightMedium;
	    while (result != -1 && count > 0)
	      {
		/*
		 * found the next item in the list
		 */
		if (*path_lst != NULL &&
			_DtCvStrCaseCmpLatin1(*path_lst, *childLst) == 0)
		    result = FormatExpandedToc(volume,
					cur_id, path_lst,
					cur_vars, font_attrs, level);
		else
		    result = FormatEntryInToc(volume, *childLst,
					cur_vars, font_attrs, level);

		childLst++;
		count--;
	      }

	    if (children != NULL)
		_DtCvFreeArray((void **) children);
          }
      }

    return result;
}

/******************************************************************************
 *
 * Semi-Public Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function:	VarHandle *__DtHelpCeSetUpVars (char *rd_buf, grow_size)
 *
 * Parameters:
 *
 * Returns:	NULL if unsuccessful.
 *
 * errno Values:
 *
 * Purpose:	
 *
 ******************************************************************************/
VarHandle
__DtHelpCeSetUpVars(
    char	*lang,
    char	*code_set,
    _FrmtUiInfo	*ui_info)
{
    FormatVariables    *newVars;

    newVars = (FormatVariables *) malloc (sizeof(FormatVariables));
    *newVars = DefVars;
    newVars->ui_info = ui_info;

    newVars->topic_mb_max = _DtHelpCeGetMbLen(lang, code_set);
    newVars->cur_mb_max   = newVars->topic_mb_max;

    return ((VarHandle) newVars);

}  /* End __DtHelpCeSetUpVars */

/******************************************************************************
 * Function:	int __DtHelpCeProcessString (char *in_string, int seg_type,
 *						char **font_attr)
 *
 * Parameters:
 *
 * Returns:	0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:	
 *
 ******************************************************************************/
int
__DtHelpCeProcessString(
    VarHandle		 var_handle,
    BufFilePtr		 my_file,
    _DtCvFrmtOption	 frmt_type,
    char		*scan_string,
    char		*in_string,
    int			 in_size,
    int			 fnt_flag,
    int			 ret_on_nl,
    _DtHelpFontHints	*font_attr )
{
    int			 done = 0;
    FormatVariables	*myVars = (FormatVariables *) var_handle;
    SegList		 oldList = myVars->my_list;

    myVars->rd_buf  = in_string;
    myVars->rd_size = in_size;
    myVars->rd_ptr  = in_string;
    myVars->my_file = my_file;

    if (my_file == NULL)
	myVars->rd_flag = 0;
    else
	myVars->rd_flag = strlen(in_string);

    while (!done)
      {
	if (True == ret_on_nl)
	    myVars->my_list = InitList;

        done = Parse (CCDF_FONT_CMD, NormalState, myVars,
		0, frmt_type,
		font_attr,
		-1, scan_string, (CCDF_FONT_CMD | CCDF_OCTAL_CMD), ret_on_nl,
		fnt_flag);

	if (done != -1 && True == ret_on_nl)
	  {
	    TerminateSegList (&(myVars->my_list), True);
	    if (NULL != myVars->my_list.list)
	      {
		done = CheckList (&oldList, 1, GROW_SIZE);
		if (-1 != done)
		  {
		    _DtCvSegment *newSeg = NextAvailSeg(oldList);

		    newSeg->type = _DtCvSetTypeToContainer(newSeg->type);
		    _DtCvContainerListOfSeg(newSeg) = myVars->my_list.list;
		    _DtCvContainerTypeOfSeg(newSeg) = _DtCvDYNAMIC;

		    oldList.cnt++;

		    TerminateSegList(&oldList, False);
		  }
	      }
	  }
      }

    if (False == ret_on_nl)
	oldList = myVars->my_list;

    myVars->my_list = oldList;
    return done;

}  /* End __DtHelpCeProcessString */

/*****************************************************************************
 * Function:    int __DtHelpCeGetParagraphList (
 *
 * Parameters:
 *
 * Returns:     0 if successful, -1 if errors.
 *
 * errno Values:
 *
 * Purpose:     _DtHelpFormatGetParagraphList places a terminator on the
 *              last segment in the current paragraph and returns the
 *              segment list included in the topic information structure.
 *
 *		If make_cont is true, the segment list is first included
 *		in a container with the specified type.
 *
 *****************************************************************************/
int
__DtHelpCeGetParagraphList (
    VarHandle		 var_handle,
    int			 make_cont,
    _DtCvFrmtOption	 type,
    _DtCvTopicPtr	*ret_handle)
{
    int			 result = -1;
    FormatVariables	*myVars = (FormatVariables *) var_handle;
    _DtCvTopicInfo	*topicHandle;

    /*
     * check the parameters.
     */
    if (ret_handle == NULL || myVars == NULL)
      {
        errno = EINVAL;
        return -1;
      }

    TerminateSegList(&(myVars->my_list), True);

    topicHandle = (_DtCvTopicInfo *) malloc (sizeof(_DtCvTopicInfo));
    if (topicHandle != NULL)
      {
	result = 0;

	if (True == make_cont)
	  {
	    _DtCvSegment *myCont = AllocateSegments(NULL, 0, 1);

	    if (NULL != myCont)
	      {
		myCont->type = _DtCvSetTypeToContainer(myCont->type);
	        _DtCvContainerTypeOfSeg(myCont) = type;
	        _DtCvContainerListOfSeg(myCont) = myVars->my_list.list;

	        myVars->my_list.list = myCont;
	        myVars->my_list.cnt  = 1;
	        myVars->my_list.max  = 1;
	     }
	    else
	        result = -1;
	  }

	topicHandle->id_str    = NULL;
        topicHandle->seg_list  = myVars->my_list.list;
        topicHandle->mark_list = NULL;
        topicHandle->link_data = myVars->my_links;

        *ret_handle = (_DtCvTopicPtr) topicHandle;
      }

    if (myVars->fmt_buf != NULL)
	free(myVars->fmt_buf);

    return result;
}

/******************************************************************************
 * Function:	int _DtHelpCeFrmtCcdfTopic (_DtHelpVolumeHdl volume, char *filename,
 *				int offset, char *id_string,
 *				_DtCvTopicPtr *ret_handle)
 *
 * Parameters:
 *		volume		Specifies the Help Volume the information
 *				is associated with.
 *		filename	Specifies the file containing the Help Topic
 *				desired.
 *		offset		Specifies the offset into 'filename' to
 *				the Help Topic desired.
 *		id_string	Specifies the location id to look for or NULL.
 *		ret_handle	Returns a handle to the topic information
 *				including the number of paragraphs and the
 *				id match segment.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	_DtHelpCeFrmtCcdfTopic formats Help Files with formatting
 *		information into a Canvas Engine structure.
 *
 ******************************************************************************/
int
_DtHelpCeFrmtCcdfTopic(
    _DtHelpVolumeHdl	 volume,
    char		*filename,
    int			 offset,
    char		*id_string,
    _FrmtUiInfo		*ui_info,
    _DtCvTopicPtr	*ret_handle )
{
    int		 result = -1;
    char	*ptr;
    FormatVariables	variables;

    /*
     * Check the parameters
     */
    if (volume == NULL || filename == NULL || offset < 0 || ret_handle == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    /*
     * get the current file path.
     */
    variables = DefVars;
    variables.topic_id = id_string;
    if (volume)
      {
	variables.vol_name = _DtHelpCeGetVolumeName(volume);
	variables.my_path  = strdup (variables.vol_name);
	if (variables.my_path == NULL)
	    return -1;

	result = _DtHelpCeStrrchr (variables.my_path, Slash, MB_CUR_MAX, &ptr);
	if (result == -1)
	    ptr = strrchr (variables.my_path, '/');

	if (ptr)
	    *ptr = '\0';
      }

    /*
     * open the file and seek to the correct place.
     */
    result = _DtHelpCeFileOpenAndSeek (filename, offset, -1,
						&(variables.my_file), NULL);
    if (result != -1)
      {
	/*
	 * Initialize the X variables.
	 */
	result = Format (&variables, ui_info, id_string, ret_handle);

	_DtHelpCeBufFileClose (variables.my_file, True);
      }

    if (variables.fmt_buf != NULL)
	free (variables.fmt_buf);

    if (variables.my_path != NULL)
	free (variables.my_path);

    return result;

}  /* End _DtHelpCeFrmtCcdfTopic */

/******************************************************************************
 * Function:	int _DtHelpCeFrmtCcdfPathAndChildren (
 *				_DtHelpVolumeHdl volume,
 *				_DtCvTopicPtr *ret_handle)
 *
 * Parameters:
 *		ret_handle	Returns a handle to the topic information
 *				including the number of paragraphs and the
 *				id match segment.
 *
 * Returns:	0 if successful, -1 if errors, 1 if there the path is empty.
 *
 * errno Values:
 *
 * Purpose:	_DtHelpCeFormatCcdfToc formats Table of Contents for
 *		a CCDF Help File.
 *
 ******************************************************************************/
int
_DtHelpCeFrmtCcdfPathAndChildren(
    _DtHelpVolumeHdl	  volume,
    char		 *loc_id,
    _FrmtUiInfo		 *ui_info,
    _DtCvTopicPtr	 *ret_handle )
{
    int		  result    = 0;
    int		  pathCnt;
    char	 *ptr;
    char	**pathHead;
    char	  readBuf[BUFF_SIZE];
    FormatVariables     myVars;
    _DtHelpFontHints    fontAttrs;

    _DtHelpCeCopyDefFontAttrList (&fontAttrs);
    fontAttrs.pointsz = 10;
    fontAttrs.weight  = _DtHelpFontWeightMedium;
    fontAttrs.style   = _DtHelpFontStyleSanSerif;

    /*
     * Check the parameters
     */
    if (volume == NULL || ret_handle == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    readBuf[0] = '\0';
    myVars     = DefVars;
    if (InitStructure (&myVars, ui_info, readBuf, BUFF_SIZE) == -1)
	return -1;

    /*
     * get the list of topics in the volume.
     */
    pathCnt = _DtHelpCeGetCcdfIdPath(volume, loc_id, &pathHead);
    if (pathCnt == -1)
	return -1;

    /*
     * Check to see if this topic is in the topic list.
     * I.e. if a count of 1 is returned this should match the
     * top topic, otherwise this is an hidden topic.
     */
    if (pathCnt == 1)
      {
	if (_DtHelpCeGetCcdfTopTopic(volume, &ptr) == -1)
	    result = -1;
	else if (_DtCvStrCaseCmpLatin1(ptr, *pathHead) != 0)
	    result = 1;
      }

    /*
     * if result is non-zero, we had problems or this is a hidden topic
     */
    if (result == 0)
      {
        fontAttrs.slant   = _DtHelpFontSlantRoman;

        myVars.topic_id = loc_id;
	myVars.vol_name = _DtHelpCeGetVolumeName(volume);
	myVars.my_path  = strdup (myVars.vol_name);
        if (myVars.my_path == NULL)
	    return -1;

        if (-1 == _DtHelpCeStrrchr (myVars.my_path, Slash, MB_CUR_MAX, &ptr))
	    ptr = strrchr (myVars.my_path, '/');

        if (ptr)
	    *ptr = '\0';

        result = FormatExpandedToc(volume, loc_id, pathHead,
						&myVars, fontAttrs, 0);
	_DtCvFreeArray((void **) pathHead);
      }

    if (result != -1)
	result = __DtHelpCeGetParagraphList (&myVars, False, _DtCvDYNAMIC,
								ret_handle);

    /*
     * check for an empty path.
     */
    if (-1 != result && NULL == ((_DtCvTopicInfo *) *ret_handle)->seg_list)
	result = 1;

    if (myVars.my_path != NULL)
        free(myVars.my_path);

    return result;

}  /* End _DtHelpCeFrmtCcdfPathAndChildren */

/******************************************************************************
 * Function:	int _DtHelpCeGetCcdfTitleChunks (
 *				_DtHelpVolumeHdl volume,
 *				_DtCvTopicPtr *ret_handle)
 *
 * Parameters:
 *		ret_handle	Returns a handle to the topic information
 *				including the number of paragraphs and the
 *				id match segment.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	_DtHelpCeFormatCcdfToc formats Table of Contents for
 *		a CCDF Help File.
 *
 ******************************************************************************/
int
_DtHelpCeGetCcdfTitleChunks(
    _DtHelpVolumeHdl	   volume,
    char		  *loc_id,
    _FrmtUiInfo		  *ui_info,
    void		***ret_chunks)
{
    int		  type   = 0;
    int		  result = 0;
    int		  offset;
    char	 *fileName;
    char	 *ptr;
    char	  readBuf[BUFF_SIZE];
    _DtCvSegment	*titleSeg;
    _DtCvSegment	*list    = NULL;
    FormatVariables     myVars;
    _DtHelpFontHints    fontAttrs;
    _DtHelpCeLockInfo lockInfo;

    /*
     * Check the parameters
     */
    if (volume == NULL || ret_chunks == NULL)
      {
	errno = EINVAL;
	return -1;
      }

    /*
     * init the return value
     */
    *ret_chunks = NULL;

    /*
     * get the topic in the volume.
     */
    if (_DtHelpCeLockVolume(volume, &lockInfo) != 0)
	return -1;

    if (_DtHelpCeFindId(volume, loc_id, -1, &fileName, &offset) == False)
      {
	_DtHelpCeUnlockVolume(lockInfo);
	return -1;
      }

    _DtHelpCeCopyDefFontAttrList (&fontAttrs);
    myVars = DefVars;
    if (InitStructure (&myVars, ui_info, readBuf, BUFF_SIZE) == -1)
      {
	_DtHelpCeUnlockVolume(lockInfo);
	return -1;
      }

    /*
     * initialize my variables.
     */
    myVars.topic_id = loc_id;
    myVars.vol_name = _DtHelpCeGetVolumeName(volume);
    myVars.my_path  = strdup (myVars.vol_name);
    if (myVars.my_path == NULL)
      {
	_DtHelpCeUnlockVolume(lockInfo);
	return -1;
      }

    if (-1 == _DtHelpCeStrrchr (myVars.my_path, Slash, MB_CUR_MAX, &ptr))
	ptr = strrchr (myVars.my_path, '/');

    if (ptr)
	*ptr = '\0';

    result = FormatCCDFTitle(&myVars, fontAttrs, fileName,
				offset, 0, (1 << _CEFONT_SIZE), True);
    free(fileName);

    if (-1 != result && 0 < myVars.my_list.cnt)
      {
	titleSeg = &(myVars.my_list.list[myVars.my_list.cnt - 1]);
	list     = _DtCvContainerListOfSeg(titleSeg);
      }

    if (result != -1 && NULL != list)
      {
	_DtCvSegment *pSeg = list;

	while (-1 != result && NULL != pSeg)
	  {
	    /*
	     * create the types and add the informatio to the array.
	     */
	    type = DT_HELP_CE_FONT_PTR | DT_HELP_CE_STRING;
	    if (_DtCvIsSegNewLine(pSeg))
		type |= DT_HELP_CE_NEWLINE;

	    if (_DtCvIsSegString(pSeg))
	      {
		*ret_chunks = (void **) _DtCvAddPtrToArray(*ret_chunks,
					(void *) type);
		if (NULL != *ret_chunks)
		    *ret_chunks = (void **) _DtCvAddPtrToArray(*ret_chunks,
					(void *) _DtCvFontOfStringSeg(pSeg));

		if (NULL != *ret_chunks)
		  {
		    if (_DtCvIsSegWideChar(pSeg))
		      {
		        offset = _DtCvStrLen(_DtCvStringOfStringSeg(pSeg), 1)
					* MB_CUR_MAX + 1;
			ptr    = (char *) malloc (sizeof(char) * offset);
			if (NULL != ptr)
			    wcstombs(ptr,
					(wchar_t *)_DtCvStringOfStringSeg(pSeg),
					offset);
		      }
		    else
			ptr = strdup((char *) _DtCvStringOfStringSeg(pSeg));

		    if (NULL != ptr)
		        *ret_chunks = (void **)_DtCvAddPtrToArray(
					*ret_chunks, (void *) ptr);
		  }

		if (NULL == *ret_chunks || NULL == ptr)
		    result = -1;
	      }

	    /*
	     * go to the next segment
	     */
	    pSeg = pSeg->next_seg;
	  }
      }
    else
      {
	char  buf[128];
	char *idStr;
	int   idLen = strlen (loc_id) + 9;

	/*
	 * create the location id string.
	 */
	idStr  = (char *) malloc (idLen);
	if (NULL != idStr)
	  {
	    /*
	     * format the location id
	     */
	    strcpy (idStr, "...(");
	    strcat (idStr, loc_id);
	    strcat (idStr, ")...");
	
	    /*
	     * format the language and codeset
	     */
	    strcpy(buf, _DtHelpFontHintsLang(fontAttrs));
	    strcat(buf, ".");
	    strcpy(buf, _DtHelpFontHintsCharSet(fontAttrs));

	    /*
	     * creat a chunk table
	     */
	    *ret_chunks = (void **) _DtCvAddPtrToArray(*ret_chunks,
	    		(void *) (DT_HELP_CE_CHARSET | DT_HELP_CE_STRING));
	    if (NULL != *ret_chunks)
	        *ret_chunks = (void **) _DtCvAddPtrToArray(*ret_chunks,
						(void *) strdup(buf));
	    if (NULL != *ret_chunks)
	        *ret_chunks = (void **) _DtCvAddPtrToArray(*ret_chunks,
						(void *) idStr);
          }
      }

    if (NULL != *ret_chunks)
	*ret_chunks = (void **) _DtCvAddPtrToArray(*ret_chunks,
						(void *) DT_HELP_CE_END);

    /*
     * free the list
     */
    if (0 < myVars.my_list.cnt)
	_DtHelpFreeSegments(myVars.my_list.list, _DtCvFALSE,
						ui_info->destroy_region, 
						ui_info->client_data);

    /*
     * free the link database
     */
    _DtLinkDbDestroy(myVars.my_links);

    /*
     * free memory
     */
    if (myVars.my_path != NULL)
        free(myVars.my_path);
    if (myVars.fmt_buf != NULL)
        free(myVars.fmt_buf);

    _DtHelpCeUnlockVolume(lockInfo);

    /*
     * indicate an error if problems.
     */
    result = 0;
    if (NULL == *ret_chunks)
	result = -1;

    return result;

}  /* End _DtHelpCeGetCcdfTitleChunks */

/******************************************************************************
 * Function:	int _DtHelpCeGetCcdfVolTitleChunks (
 *				_DtHelpVolumeHdl volume,
 *				void ***ret_chunks)
 *
 * Parameters:
 *		ret_chunks	Returns a ptr to the title information chunks
 *				The title chunks are stored in allocated memory
 *				that is owned by the caller and should be
 *                              freed when no longer needed.
 *				id match segment.
 *
 * Returns:	0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:	_DtHelpCeFormatCcdfVolTitleChunks formats volume title for
 *		a CCDF Help File.
 *
 ******************************************************************************/
int
_DtHelpCeGetCcdfVolTitleChunks(
    _DtHelpVolumeHdl	   volume,
    _FrmtUiInfo		  *ui_info,
    void		***ret_chunks)
{
    char    *charSet;
    char    *titleStr;

    /*
     * get the title of the volume (we own the memory).
     */
    charSet = _DtHelpCeGetCcdfVolLocale(volume);
    if (charSet == NULL)
	charSet = strdup("C.ISO-8859-1");

    titleStr = _DtHelpCeGetCcdfVolTitle(volume);
    if (titleStr != NULL)
      {
        *ret_chunks = (void **) malloc(sizeof(void *) * 4);
        if (*ret_chunks == NULL)
	    return -1;

        (*ret_chunks)[0] = (void *) (DT_HELP_CE_CHARSET | DT_HELP_CE_STRING);
        (*ret_chunks)[1] = (void *) strdup(charSet);
        (*ret_chunks)[2] = (void *) strdup(titleStr);
        (*ret_chunks)[3] = (void *) DT_HELP_CE_END;
      }
    else 
      {
        free(charSet);
        if (_DtHelpCeGetCcdfTitleChunks(volume, "_title",
						ui_info, ret_chunks) != 0
		&&
	    _DtHelpCeGetCcdfTitleChunks(volume, "_hometopic",
						ui_info, ret_chunks) != 0)
 	    return -1;
      }

    return 0;

}  /* End _DtHelpCeGetCcdfVolTitleChunks */
