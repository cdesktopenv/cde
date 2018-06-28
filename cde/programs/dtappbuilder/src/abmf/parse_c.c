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
/* $XConsortium: parse_c.c /main/4 1996/10/02 13:32:03 drk $ */
/*
 * File: parse_c.c
 *
 * Parses a C file for stubs merge.
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <ab_private/AB.h>
#include <ab_private/util_err.h>
#include <ab/util_types.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include "parse_cP.h"

/* DTB_USER_CODE_START */
/* DTB_USER_CODE_END*/

#define MAX_USER_SEGS_PER_SEG	10

/*
 * Buffered file
 */
typedef struct
{
    FILE		*stream;
    BOOL		reverse;
    BOOL		eof;

    /* the offsets of the last 2 newlines read from the file */
    long		lastNewlineOff;
    long		lastNewlineOff2;

    /* DTB_USER_CODE_START */
    int			userField1;
    char		*userField2;
    /* DTB_USER_CODE_END */
} BFileRec, *BFile;

typedef struct
{
    BOOL	forward;
    long	offset;
    long	lastNewlineOff;
    long	lastNewlineOff2;
} BFileStateRec, *BFileState;

static int	bfile_construct(BFile, FILE *file);
static int	bfile_destruct(BFile);
static BOOL	bfile_is_forward(BFile file);
static BOOL	bfile_is_reverse(BFile file);
static int	bfile_get_char(BFile);
static int	bfile_backup(BFile, long numBytes);
static long	bfile_get_off(BFile);
static int	bfile_set_off(BFile, long off);
static BOOL	bfile_eof(BFile);
static int	bfile_set_file(BFile, FILE *stream);
static int	bfile_reverse(BFile);
static int	bfile_forward(BFile);
static int	bfile_save_state(BFile, BFileState);
static int	bfile_restore_state(BFile, BFileState);


typedef enum
{
    COMMENT_UNDEF = 0,
    COMMENT_SLASH_STAR,
    COMMENT_SLASH_SLASH,
    COMMENT_TYPE_NUM_VALUES
} COMMENT_TYPE;


typedef enum
{
    MAGIC_CMT_UNDEF = 0,
    MAGIC_CMT_USER_START,
    MAGIC_CMT_USER_END,
    MAGIC_COMMENT_TYPE_NUM_VALUES	/* must be last */
} MAGIC_COMMENT_TYPE;
#define MAGIC_COMMENT_MAX_LEN	(20)
#define MAGIC_COMMENT_MAX_SIZE	(MAGIC_COMMENT_MAX_LEN+1)

static STRING	magicCommentUserStart = "DTB_USER_CODE_START";
static STRING	magicCommentUserEnd = "DTB_USER_CODE_END";


static int	write_func_var_decl(FILE *file, CSeg cseg);
static int	write_func_as_strings(FILE *file, CSeg cseg);
static int	get_seg(BFile file, CSeg cseg);
static int	skip_line(BFile file);
static int	skip_string_const(BFile file);
static int	skip_char_const(BFile file);
static int	skip_white(BFile file);
static int	skip_white_and_comment(BFile file);
static int 	write_c_string(FILE *file, STRING value);
static int	find_user_seg_end(BFile file, CUserSegs userSegs);
static int	find_func_name(
			BFile	file, 
			long	funcOff, 
			long	bodyOff,
			long	*nameOffPtr,
			long	*nameLenPtr);
static int	find_func_proto(
			BFile	file,
			long	funcOff,
			long	nameOff,
			long	bodyOff,
			long	*protoOff,
			long	*protoLen);

static int	trim_opening_comment(
			BFile	file,
			long	*funcOffPtr,
			long	*funcLenPtr,
			long	protoOff
		);

static int 	grab_string_from_file(
			STRING *stringPtr,
			BFile	file,
			long	stringOff,
			long	stringLen
		);
static int	find_seg(

			BFile		file,
			CSEG_TYPE	*csegTypePtr,
			char		*namePtr,
			long		*segOffPtr,
			long		*segLenPtr,
			long		*bodyOffPtr,
			long		*bodyLenPtr,
			CUserSegs	userSegs
		);

static int	find_ident(BFile file, char *buf, int bufSize);
static int	find_matching(
			BFile		file, 
			int		matchChar, 
			long		*matchOffPtr,
			CUserSegs	userSegs
		);

static BOOL	char_is_legal_for_ident(int iChar, BOOL firstChar);

static int	skip_comment(
			BFile 			file, 
			COMMENT_TYPE		type,
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
		);
static int	skip_slash_star_comment(
			BFile 			file, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
		);
static int	skip_slash_slash_comment(
			BFile 			file, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
		);
static int	skip_slash_slash_comment_fwd(
			BFile 			file, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
		);
static int	skip_slash_slash_comment_bwd(
			BFile 			file, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
		);

static MAGIC_COMMENT_TYPE	determine_magic_comment_type(STRING cmt);
static int	get_char_from_c_file(
			BFile 			file, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
		);

static COMMENT_TYPE	find_comment_start(BFile file, long *commentOffOut);
static int	reverse_string(STRING buf);
static int	cvt_offset_to_line(BFile file, long offset);
static int	set_user_seg_next_pointers(CSegArray segArray, BFile file);
static int	user_seg_construct(CUserSeg userSeg);
static BOOL	user_seg_is_valid(CUserSeg userSeg);


/*************************************************************************
 *************************************************************************
 **									**
 **		Public Entry Points					**
 **									**
 *************************************************************************
 *************************************************************************/

int
abmfP_parse_c_file(FILE *cFile, CSegArray *segmentsOutPtr)
{
    int			return_value = 0;
    int			rc = 0;		/* return code */	
    int			numSegs = 0;
    CSegArray		segArray = NULL;
    CSegRec		seg;
    BFileRec		bfileRec;
    BFile		bfile = &bfileRec;

    bfile_construct(bfile, cFile);
    segArray = cseg_array_create();
    if (segArray == NULL)
    {
	return_code(ERR_NO_MEMORY);
    }

    memset(&seg, 0, sizeof(seg));
    while ((rc = get_seg(bfile, &seg)) >= 0)
    {
	/*
	if (debugging())
	{
	    int		i = 0;
	    util_dprintf(1, "Seg: %s  lines:%d-%d length:%ld userSegs:%d\n", 
		seg.type == CSEG_GLOBAL? "[GLOBAL]": util_strsafe(seg.name),
		cvt_offset_to_line(bfile, seg.offset),
		cvt_offset_to_line(bfile, seg.offset + seg.length 
					+ (seg.length==0? 0:-1)),
		seg.length,
		seg.userSegs.numSegs);

	    for (i = 0; i < seg.userSegs.numSegs; ++i)
	    {
		STRING	string = NULL;
		util_dprintf(1, "    =>UserSeg: %d-%ld [%ld+%ld bytes]\n",
		    cvt_offset_to_line(bfile, seg.userSegs.segs[i].offset),
		    cvt_offset_to_line(bfile, seg.userSegs.segs[i].offset +
				+ seg.userSegs.segs[i].length
				+ (seg.userSegs.segs[i].length<1? 0:-1)),
		    seg.userSegs.segs[i].offset,
		    seg.userSegs.segs[i].length);
	        grab_string_from_file(&string, bfile,
			seg.userSegs.segs[i].offset,
			seg.userSegs.segs[i].length);
		util_dprintf(1, "|%s|\n", util_strsafe(string));
		util_free(string);
	    }
	}
	*/

	++numSegs;
	segArray->segs = (CSeg)realloc(
				segArray->segs, numSegs * sizeof(CSegRec));
	if (segArray->segs == NULL)
	{
	    cseg_array_destroy(segArray);
	    util_printf_err("Out of memory!\n");
	    return_code(-1);
	}
	segArray->segs[numSegs-1] = seg;
        memset(&seg, 0, sizeof(seg));
    }
    segArray->numSegs = numSegs;

    if ((rc < 0) && (rc != ERR_EOF))
    {
	/* failure occurred */
	segArray->numSegs = 0;
	return_code(rc);
    }
    else
    {
        segArray->numSegs = numSegs;
    }

    /*
     * Set up all user segment next pointers
     */
    set_user_seg_next_pointers(segArray, bfile);
    return_value = numSegs;

epilogue:
    *segmentsOutPtr = segArray;
    bfile_destruct(bfile);
    return return_value;
}


/*************************************************************************
 **									**
 **		PRIVATE SYMBOLS						**
 **									**
 *************************************************************************
 *************************************************************************/

static int
set_user_seg_next_pointers(CSegArray segArray, BFile file)
{
    CSeg	curSeg = NULL;
    CUserSeg	curUserSeg = NULL;
    CUserSeg	lastUserSeg = NULL;
    int		segCount = 0;
    int		userSegCount = 0;

    for (segCount = 0; segCount < segArray->numSegs; ++segCount)
    {
	curSeg = &(segArray->segs[segCount]);
	for (userSegCount = 0; userSegCount < curSeg->userSegs.numSegs; 
	    ++userSegCount)
	{
	    curUserSeg = &(curSeg->userSegs.segs[userSegCount]);
	    if (lastUserSeg == NULL)
	    {
		/* first user segment */
		segArray->firstUserSeg = curUserSeg;
	    }
	    else
	    {
		lastUserSeg->next = curUserSeg;
	    }

	    curUserSeg->line = -1;
	    curUserSeg->line = cvt_offset_to_line(file, curUserSeg->offset);
	    lastUserSeg = curUserSeg;
	}
    }

    return 0;
}


static BOOL
char_is_legal_for_ident(int iChar, BOOL firstChar)
{
    return
	(   (iChar == '_')
	 || (firstChar? isalpha(iChar):isalnum(iChar)) );
}


static int
find_seg(
		BFile		file,
		CSEG_TYPE	*segTypePtr,
		char		*namePtr,
		long		*segOffPtr,
		long		*segLenPtr,
		long		*bodyOffPtr,
		long		*bodyLenPtr,
		CUserSegs	userSegs
)
{
    int		return_value = 0;
    long	segOff = 0;
    long	segLen = 0;
    long	openingParenOff = 0;
    long	openingBraceOff = 0;
    long	closingBraceOff = 0;
    long	identOff = 0;
    int		iChar = -1;
    char	ident[1024];
    int		identLen = 0;
    BOOL	lookForIdent = TRUE;
    CSEG_TYPE	segType = CSEG_UNDEF;
    MAGIC_COMMENT_TYPE	magicCommentType = MAGIC_CMT_UNDEF;
    *ident = 0;

    if (bfile_eof(file))
    {
	return_code(ERR_EOF);
    }

    *namePtr = 0;
    segOff = bfile_get_off(file);	/* for global segments */

    while ((segType == CSEG_UNDEF) && (!bfile_eof(file)))
    {
        /*
         * Scan, looking for opening {
         */		/* } vi hack */
        openingBraceOff = -1;
        while ((openingBraceOff < 0) && (!bfile_eof(file)))
        {
	    iChar = get_char_from_c_file(file, &magicCommentType);

	    if (iChar == EOF)
	    {
		continue;
	    }

	    if (magicCommentType != MAGIC_CMT_UNDEF)
	    {
		/*
		 * Found user seg in global segment
		 */
		CUserSeg	userSeg = NULL;
		++(userSegs->numSegs);
		userSeg = &(userSegs->segs[(userSegs->numSegs)-1]);
		user_seg_construct(userSeg);
		if (iChar != '\n')
		{
		    skip_line(file);
		}
		userSeg->offset = bfile_get_off(file);

		if (find_user_seg_end(file, userSegs) < 0)
		{
		    return_code(-1);
		}
		segLen = bfile_get_off(file) - segOff + 1;
	    }

	    if (iChar == '{') /* } vi hack */
	    {
	        openingBraceOff = (bfile_get_off(file)-1);
		if (userSegs->numSegs > 0)
		{
		    /* we found global segs before this object */
		    segType = CSEG_GLOBAL;
		    bfile_backup(file, 1);
		    segLen = bfile_get_off(file) - segOff + 1;
		    goto epilogue;
		}
	    }
        } /* while openingBraceOff < 0 */
	if (openingBraceOff < 0)
	{
	    /*
	     * Global segment, or end of file
	     */
	    if (bfile_get_off(file) > segOff)
	    {
		/* found segment at end of file */
		segType = CSEG_GLOBAL;
	    }
	    else if (iChar == EOF)
	    {
		return_value = ERR_EOF;
	    }
	    else
	    {
		return_value = -1;
	    }
	    goto epilogue;
	}
	segOff = openingBraceOff;
    
        /*
         * Look for preceding ')', or ident
         */
	bfile_set_off(file, openingBraceOff-1);
	bfile_reverse(file);

	openingParenOff = -1;
	lookForIdent = TRUE;
	identLen = 0;
	while (lookForIdent)
	{
	    iChar = get_char_from_c_file(file, NULL);
	    if (iChar == EOF)
	    {
		lookForIdent = FALSE;
	    }
	    else if (iChar == ';')
	    {
		if (openingParenOff >= 0)
		{
		    lookForIdent = FALSE;	/* end of parse */
		}
		else
		{
		    /* if a ; is seen between () and {}, it should be an
		     * old-style parameter list declaration
		     */
		    identLen = 0;
		}
	    }
	    else if (iChar == ')')
	    {
	        /* no ident - look for () */
		segType = CSEG_FUNC;
		if (find_matching(file, '(', &openingParenOff, NULL)
				/* ) vi hack */			>= 0)
		{
		    segType = CSEG_FUNC;
		    identLen = 0;
		}
		else
		{
		    util_printf_err(
			"Bad function definition at or near line %d\n",
			cvt_offset_to_line(file, openingBraceOff-2));
		    lookForIdent = FALSE;
		    identLen = 0;
		    return_code(-1);
		}
	    }
	    else if (iChar == ']')
	    {
		if (find_matching(file, '[', NULL, NULL) >= 0) /* ] vi hack */
		{
		    segType = CSEG_AGGREGATE_VAR;
		    identLen = 0;
		}
		else
		{
		    util_printf_err("Unbalanced '[' at or near line %d\n",
			cvt_offset_to_line(file, bfile_get_off(file)));
			/* ] vi hack */
 		    lookForIdent = FALSE;
		    identLen = 0;
		    return_code(-1);
		}
	    }
	    else 
	    {
	        if (char_is_legal_for_ident(iChar, FALSE))
		{
		    ident[identLen++] = iChar;
		}
		else if (identLen > 0)
		{
		    /* found end of ident */
		    lookForIdent = FALSE;
		}
	    }
	} /* lookForIdent */
	ident[identLen] = 0;
	reverse_string(ident);		/* was read backwards */
    
        /*
         * If it's a function, find the function name
         * NB: bfile is reversed
         */
        if (identLen < 1)
        {
	    long	off = bfile_get_off(file);
	    *ident = 0;
	    if (find_ident(file, ident, 1024) < 0)
	    {
	        util_printf_err("Syntax error at or near line %d\n",
			cvt_offset_to_line(file, off));
		return_code(-1);
	    }
	    else
	    {
		segOff = identOff = (bfile_get_off(file)+1);
	    }
        }
    
        /*
         * If undef, it should be a type definition.
         * NB: bfile is reversed
         */
        if ((identLen > 0) && (segType == CSEG_UNDEF))
        {
	    ident[identLen] = 0;
	    lookForIdent = FALSE;

	    if (   util_streq(ident, "struct")
		|| util_streq(ident, "union")
		|| util_streq(ident, "enum")
		|| util_streq(ident, "class") )
	    {
		identLen = 0;
		segType = CSEG_AGGREGATE_TYPE;
	    }
	    else
	    {
		/*
		 * This could be a variable assignment or an aggregate
		 * type. The ident is the proper name for this one.
		 */
		segType = CSEG_AGGREGATE_TYPE;
	    }
        } /* segType == CSEG_UNDEF */
    
        /*
         * file is reversed
         * Get closing brace to find end.
         */
        bfile_forward(file);
        bfile_set_off(file, openingBraceOff); 
	bfile_get_char(file);		/* skip brace */
	/* { vi hack */
        if (find_matching(file, '}', &closingBraceOff, userSegs) < 0)
        {
	    util_printf_err("Unbalanced '{' at line %d.\n", /* } vi hack */
		cvt_offset_to_line(file, openingBraceOff));
	    return_code(-1);
        }
    
        /*
         * If name is still undef, it should be an aggregate type, and
         * the name should follow the closing brace.
         *
         * file is forward.
         */
        if ((identLen < 1) && (segType != CSEG_FUNC))
        {
	    if (find_ident(file, ident, 1024) >= 0)
	    {
	        segType = CSEG_AGGREGATE_TYPE;
	    }
        }
    } /* while segType */
    
epilogue:
    (*segTypePtr) = segType;
    (*segOffPtr) = segOff;
    (*segLenPtr) = segLen;
    (*bodyOffPtr) = openingBraceOff;
    (*bodyLenPtr) = (closingBraceOff - openingBraceOff) + 1;
    util_strncpy(namePtr, ident, 1024);
    if ((segType != CSEG_UNDEF) && (segType != CSEG_GLOBAL))
    {
	/* {} segment - calculate metrics by braces 
	 */
        (*bodyOffPtr) = openingBraceOff;
        (*bodyLenPtr) = (closingBraceOff - openingBraceOff) + 1;
	(*segLenPtr) = closingBraceOff - segOff + 1;
    }

    if (   (return_value >= 0)
	&& ((segType == CSEG_UNDEF) || ((*segOffPtr) < 0) || ((*segLenPtr) < 0))
       )
    {
	return_value = -1;
    }

    return return_value;
}


/*
 * Finds the end of a user segment.
 * Assumes: beginning of segment as been read.
 * Effects: reads past entire segment
 */
static int
find_user_seg_end(BFile file, CUserSegs userSegs)
{
    int			return_value = 0;
    BOOL		segmentIsBad = FALSE;
    int			iChar = -1;
    MAGIC_COMMENT_TYPE	magicCommentType = MAGIC_CMT_UNDEF;
    CUserSeg		userSeg = &(userSegs->segs[(userSegs->numSegs)-1]);
    long		segEndOff = -1;

    if (userSegs->numSegs < 1)
    {
	return_code(ERR_CLIENT);
    }

    segEndOff = bfile_get_off(file)-1;
    for (iChar = EOF+1;
	(iChar != EOF) && (magicCommentType == MAGIC_CMT_UNDEF);
	iChar = get_char_from_c_file(file, &magicCommentType) )
    {
	if (iChar == '\n')
	{
	    segEndOff = file->lastNewlineOff;
	}
    }

    if (magicCommentType != MAGIC_CMT_USER_END)
    {
	segmentIsBad = TRUE;
	return_value = -1;
    }
    else
    {
	if (iChar != '\n')
	{
	    skip_line(file);
	}
	/* include final newline */
	userSeg->length = segEndOff - userSeg->offset + 1;
	if (userSeg->length < 0)
	{
	    userSeg->length = 0;
	}
    }

    if (!user_seg_is_valid(userSeg))
    {
	segmentIsBad = TRUE;
	--(userSegs->numSegs);
	return_value = ERR_INTERNAL;
    }

    if (segmentIsBad)
    {
	util_printf_err("Bad DTB_USER segment, line %d\n", 
		cvt_offset_to_line(file, userSeg->offset));
	return_code(-1);
    }

epilogue:
    return return_value;
}


static int
find_ident(
			BFile	file,
			char	*buf,
			int	bufSize
)
{
    BOOL	foundIdent = FALSE;
    int		iChar = 0;
    int		bufLen = 0;

    *buf = 0;
    skip_white_and_comment(file);
    while (TRUE)
    {
	iChar = bfile_get_char(file);
	if ((iChar == EOF) || (!char_is_legal_for_ident(iChar, FALSE)))
	{
	    break;
	}
	else if (bufLen >= (bufSize-1))
	{
	    break;
	}
	else
	{
	    buf[bufLen++] = iChar;
	}
    }
    buf[bufLen] = 0;
    foundIdent = ((*buf) != 0);

    if (foundIdent && bfile_is_reverse(file))
    {
	reverse_string(buf);
    }

    /* if (foundIdent)
    {
        printf("found ident '%s'\n", buf);
    }*/

    return foundIdent? 0:-1;
}


static int
reverse_string(STRING buf)
{
    int		bufLen = strlen(buf);
    int 	i;
    int		tempChar;

    for (i = 0; i < (bufLen/2); ++i)
    {
        tempChar = buf[i];
        buf[i] = buf[bufLen-1-i];
        buf[bufLen-1-i] = tempChar;
    }
    return 0;
}


/*
 * Gets the next C segment from the file.
 */

get_seg(BFile file, CSeg cseg)
{
    int		return_value = 0;
    int		rc = 0;		/* return code */
    BOOL	foundSeg = FALSE;
    CSEG_TYPE	segType = CSEG_UNDEF;
    long	segOff = -1;
    long	segLen = -1;
    long	bodyOff = -1;
    long	bodyLen = -1;
    long	nameOff = -1;
    long	nameLen = -1;
    long	protoOff = -1;
    long	protoLen = -1;
    BOOL	abort = FALSE;
    BOOL	err = FALSE;
    long	off = 0;
    char	segName[1024];
    int		i = 0;
    CUserSegsRec	userSegs;

    userSegs.numSegs = 0;
    cseg->type = CSEG_UNDEF;
    rc = find_seg(file,
		    &segType, segName, &segOff, &segLen, &bodyOff, &bodyLen,
		    &userSegs);
    return_if_err(rc,rc);

    /*
     * Put the info into the func structure
     */
    cseg->type = segType;
    cseg->name = strdup(segName);
    cseg->offset = segOff;
    cseg->length = segLen;
    cseg->text = NULL;
    cseg->declaration = NULL;
    cseg->userSegs = userSegs;
    for (i = 0; i < cseg->userSegs.numSegs; ++i)
    {
        grab_string_from_file(&(cseg->userSegs.segs[i].text),
		file, 
		cseg->userSegs.segs[i].offset, 
		cseg->userSegs.segs[i].length);
    }

epilogue:
    return return_value;
}


static int
grab_string_from_file(
			STRING *stringPtr,
			BFile	file,
			long	stringOff,
			long	stringLen
)
{
#define string (*stringPtr)
    int		returnValue = 0;

    if (stringLen < 1)
    {
	string = strdup("");
    }
    else
    {
	BFileStateRec	fileState;

	bfile_save_state(file, &fileState);
        string = (STRING)util_malloc((size_t)(stringLen + 1));
        bfile_set_off(file, stringOff);
        fread(string, 1, (size_t)stringLen, file->stream);
        string[stringLen]= 0;
	bfile_restore_state(file, &fileState);
    }

    return returnValue;
#undef string
}


/*
 * userSegs is ignored, if it is NULL
 */
static int
find_matching(
			BFile		file, 
			int		matchChar, 
			long		*matchOffPtr,
			CUserSegs	userSegs
)
{
    int		return_value = 0;
    int		iChar = -1;
    long	lCharOff = 0;
    long	matchOff = -1;
    BOOL	foundMatching = FALSE;
    BOOL	quit = FALSE;
    long	startOff = bfile_get_off(file);
    MAGIC_COMMENT_TYPE	magicCommentType =  MAGIC_CMT_UNDEF;

    while ((!quit) && (!bfile_eof(file)))
    {
	iChar = get_char_from_c_file(file, &magicCommentType);
	lCharOff = bfile_get_off(file);

	/*
	 * Process magic comments
	 */
	if (userSegs != NULL)
	{
	    switch (magicCommentType)
	    {
	        case MAGIC_CMT_USER_START:
		    if (   (userSegs->numSegs > 0) 
		        && (!user_seg_is_valid(
			       &(userSegs->segs[(userSegs->numSegs)-1]))))
		    {
		        /* last segment was not terminated */
		        --(userSegs->numSegs);
		    }
		    else
		    {
			CUserSeg	userSeg = NULL;

			if (iChar != '\n')
			{
		            skip_line(file);
			}
		        ++(userSegs->numSegs);
		        userSegs->segs[(userSegs->numSegs)-1].offset 
						    = bfile_get_off(file);
		        userSegs->segs[(userSegs->numSegs)-1].length = -1;
			userSeg = &(userSegs->segs[(userSegs->numSegs)-1]);
			user_seg_construct(userSeg);
		        userSeg->offset = bfile_get_off(file);

			if (find_user_seg_end(file, userSegs) < 0)
			{
			    return_code(-1);
			}
		    }
	        break;

	        case MAGIC_CMT_USER_END:
		    /*
		     * This should have been found by find_user_seg_end().
		     */
		    util_printf_err("Bad DTB_USER segment at line %d\n", 
			    cvt_offset_to_line(file, bfile_get_off(file))-1);
		    return_code(-1);
	        break;
	    }
        } /* userSegs != NULL */

	if (iChar == matchChar)
	{
	    quit = TRUE;
	    foundMatching = TRUE;
	}
	else
	{
	    switch (iChar)
	    {
	    case EOF:
	        quit = TRUE;
	     break;

	     case '"':
		 skip_string_const(file);
	     break;

	     case '\'':
		 skip_char_const(file);
	     break;

	     case '{':
		 if (find_matching(file, '}', NULL, userSegs) < 0)
		 {
		     util_printf_err("Unbalanced '{' at line %d.\n",
			cvt_offset_to_line(file, lCharOff));
		     return_code(-1);
		 }
	     break;

	     case '}':
		 if (find_matching(file, '{', NULL, userSegs) < 0)
		 {
		     util_printf_err("Unbalanced '}' at line %d.\n",
			cvt_offset_to_line(file, lCharOff));
		     return_code(-1);
		 }
	    break;

	    case '(':
		if (find_matching(file, ')', NULL, userSegs) < 0)
		{
		    util_printf_err("Unbalanced '(' at line %d.\n",
			cvt_offset_to_line(file, lCharOff));
		    return_code(-1);
		}
	    break;

	    case ')':
		if (find_matching(file, '(', NULL, userSegs) < 0)
		{
		    util_printf_err("Unbalanced ')' at line %d.\n",
			cvt_offset_to_line(file, lCharOff));
		    return_code(-1);
		}
	    break;
	    } /* switch iChar */
	} /* ! iChar==matchChar */
    } /* while !quit... */

epilogue:
    if (foundMatching)
    {
	/* printf("<<match found(%c) for %ld>>\n", matchChar, startOff); */
	if (matchOffPtr != NULL)
	{
	    matchOff = bfile_get_off(file) + (file->reverse? 1:-1);
	    *matchOffPtr = matchOff;
	}
    }
    if ((return_value >= 0) && (!foundMatching))
    {
	return_value = -1;
    }

    return return_value;
}


static int
user_seg_construct(CUserSeg userSeg)
{
    memset(userSeg, 0, sizeof(*userSeg));
    userSeg->offset = -1;
    userSeg->length = -1;
    return 0;
}


static BOOL
user_seg_is_valid(CUserSeg userSeg)
{
    return (   (userSeg->offset > -1)
	    && (userSeg->length > -1) );
}


static int
skip_line(BFile file)
{
    int		iChar = 0;
    while (((iChar = bfile_get_char(file)) != EOF) && (iChar != '\n'))
    {
    }
    return 0;
}


/*
 * Assumes: opening " has been read
 */
static int
skip_string_const(BFile file)
{
    int		iChar = 0;
    while (((iChar = bfile_get_char(file)) != EOF)
	   && (iChar != '\n')
	   && (iChar != '"') )
    {
	if (iChar == '\\')
	{
	    bfile_get_char(file);
	}
    }
    return 0;
}


/*
 * Assumes: opening ' has been read
 */
static int
skip_char_const(BFile file)
{
    int		iChar = 0;
    while (((iChar = bfile_get_char(file)) != EOF)
	   && (iChar != '\n')
	   && (iChar != '\'') )
    {
	if (iChar == '\\')
	{
	    bfile_get_char(file);
	}
    }
    return 0;
}


static int
skip_white(BFile file)
{
    int	iChar = bfile_get_char(file);

    while ((iChar != EOF) && (isspace(iChar)))
    {
	iChar = bfile_get_char(file);
    }
    if (iChar != EOF)
    {
	bfile_backup(file, 1);
    }
    return 0;
}


static int
skip_white_and_comment(BFile file)
{
    int	iChar = get_char_from_c_file(file, NULL);

    while ((iChar != EOF) && (isspace(iChar)))
    {
	iChar = get_char_from_c_file(file, NULL);
    }
    if (iChar != EOF)
    {
	bfile_backup(file, 1);
    }
    return 0;
}


/*
 * Gets a char from a C source file, skipping comments
 */
static int
get_char_from_c_file(
			BFile 			file,
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
)
{
    int		iChar = EOF;
    BOOL	done = FALSE;

    if (magicCommentTypeOutPtr != NULL)
    {
        *magicCommentTypeOutPtr = MAGIC_CMT_UNDEF;
    }

    while (!done)
    {
	iChar = bfile_get_char(file);
	switch (iChar)
	{
	    case EOF:
		done = TRUE;
	    break;

	    case '\n':
		if (bfile_is_forward(file))
		{
		    done = TRUE;
		}
		else
		{
		    if (skip_slash_slash_comment(file, magicCommentTypeOutPtr) < 0)
		    {
			/* no comment */
			done = TRUE;
		    }
		}
	    break;

	    case '/':
		if (bfile_is_forward(file))
		{   
		    iChar = bfile_get_char(file);
		    if (iChar == '/')
		    {
			skip_slash_slash_comment(file, magicCommentTypeOutPtr);
		    }
		    else if (iChar == '*')
		    {
			skip_slash_star_comment(file, magicCommentTypeOutPtr);
		    }
		    else
		    {
			bfile_backup(file, 1);
		    }
		}
		else
		{
		    /* reversed file */
		    if ((iChar = bfile_get_char(file)) == '*')
		    {
			skip_slash_star_comment(file, magicCommentTypeOutPtr);
		    }
		    else
		    {
			bfile_backup(file, 1);
		    }
		}
	    break;

	    default:
		done = TRUE;
	    break;
	}
    } /* while !done */

    return iChar;
}


static int
skip_comment(
			BFile			file,
			COMMENT_TYPE 		commentType, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
)
{
    int		return_value = -1;

    if (magicCommentTypeOutPtr != NULL)
    {
        *magicCommentTypeOutPtr = MAGIC_CMT_UNDEF;
    }

    switch(commentType)
    {
	case COMMENT_SLASH_STAR:
	    return_value = skip_slash_star_comment(
				file, magicCommentTypeOutPtr);
	break;

	case COMMENT_SLASH_SLASH:
	    return_value = skip_slash_slash_comment(
				file, magicCommentTypeOutPtr);
	break;
    }

    return return_value;
}


static int
skip_slash_slash_comment(
			BFile 			file,
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
)
{
    int		return_value = -1;

    if (bfile_is_forward(file))
    {
	return_value = skip_slash_slash_comment_fwd(
				file, magicCommentTypeOutPtr);
    }
    else
    {
	return_value = skip_slash_slash_comment_bwd(
				file, magicCommentTypeOutPtr);
    }
    return return_value;
}


static int
skip_slash_slash_comment_fwd(
			BFile 			file, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
)
{
    int		iChar = 0;
    char	cmtBuf[MAGIC_COMMENT_MAX_SIZE];
    int		cmtBufLen = 0;

    while ((iChar != EOF) && (iChar != '\n'))
    {
        iChar = bfile_get_char(file);
	if (   ((char_is_legal_for_ident(iChar, FALSE)) || (cmtBufLen > 0))
	    && (cmtBufLen < MAGIC_COMMENT_MAX_LEN) )
	{
	    cmtBuf[cmtBufLen++] = iChar;
	}
    }
    cmtBuf[cmtBufLen] = 0;

    if (magicCommentTypeOutPtr != NULL)
    {
        *magicCommentTypeOutPtr = determine_magic_comment_type(cmtBuf);
    }
    return 0;
}


static int
skip_slash_slash_comment_bwd(
			BFile 			file, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
)
{
    long	startOff = bfile_get_off(file);
    BOOL	foundCommentStart = FALSE;
    BOOL	done = FALSE;
    int		iChar = 0;

    if (magicCommentTypeOutPtr != NULL)
    {
        *magicCommentTypeOutPtr = MAGIC_CMT_UNDEF;
    }
    while (!done)
    {
	iChar = bfile_get_char(file);
	switch (iChar)
	{
	    case '\n':
	    case EOF:
		done = TRUE;
	    break;

	    case '/':
		if ((iChar = bfile_get_char(file)) == '/')
		{
		    foundCommentStart = TRUE;
		    done = TRUE;
		}
		else
		{
		    bfile_backup(file, 1);
		}
	    break;
	}
    }

    if (!foundCommentStart)
    {
	bfile_set_off(file, startOff);
    }

    return (foundCommentStart? 0:-1);
}


static int
skip_slash_star_comment(
			BFile 			file, 
			MAGIC_COMMENT_TYPE	*magicCommentTypeOutPtr
)
{
    int		iChar = 0;
    BOOL	done = FALSE;
    BOOL	foundCommentEnd = FALSE;
    char	cmtBuf[MAGIC_COMMENT_MAX_SIZE];
    int		cmtBufLen = 0;

    while (!done)
    {
	iChar = bfile_get_char(file);

	if (iChar == EOF)
	{
	    done = TRUE;
	    continue;
	}

	if (   ((char_is_legal_for_ident(iChar, FALSE)) || (cmtBufLen > 0))
	    && (cmtBufLen < MAGIC_COMMENT_MAX_LEN) )
	{
	    cmtBuf[cmtBufLen++] = iChar;
	}

	if (iChar == '*')
	{
	    if ((iChar = bfile_get_char(file)) == '/')
	    {
		foundCommentEnd = TRUE;
		done = TRUE;
	    }
	    else
	    {
		bfile_backup(file, 1);
	    }
	}
    }
    cmtBuf[cmtBufLen] = 0;
    if (magicCommentTypeOutPtr != NULL)
    {
        *magicCommentTypeOutPtr = determine_magic_comment_type(cmtBuf);
    }

    return (foundCommentEnd? 0:-1);
}


static MAGIC_COMMENT_TYPE
determine_magic_comment_type(STRING cmt)
{
    MAGIC_COMMENT_TYPE	magicType = MAGIC_CMT_UNDEF;
    static int		startLen = -1;
    static int		endLen = -1;

    if (startLen < 0)
    {
	startLen = strlen(magicCommentUserStart);
	endLen = strlen(magicCommentUserEnd);
    }

    if (strncmp(cmt, magicCommentUserStart, startLen) == 0)
    {
	magicType = MAGIC_CMT_USER_START;
    }
    else if (strncmp(cmt, magicCommentUserEnd, endLen) == 0)
    {
	magicType = MAGIC_CMT_USER_END;
    }

    return magicType;
}


/*************************************************************************
 **									**
 ** 		BFile							**
 **									**
 *************************************************************************/
static BOOL bfileP_set_eof(BFile file);

static int
bfile_construct(BFile file, FILE *stream)
{
    file->stream = NULL;
    file->reverse = FALSE;
    file->lastNewlineOff = -1;
    file->lastNewlineOff2 = -1;
    file->eof = TRUE;

    return bfile_set_file(file, stream);
}


static int
bfile_destruct(BFile file)
{
    return bfile_set_file(file, NULL);
}


static BOOL
bfile_eof(BFile file)
{
    return file->eof;
}


static BOOL
bfile_is_forward(BFile file)
{
    return (!(file->reverse));
}


static BOOL
bfile_is_reverse(BFile file)
{
    return file->reverse;
}


static int
bfile_get_char(BFile file)
{
    int		iChar = EOF;

    if (file->eof)
    {
	goto epilogue;
    }

    iChar = fgetc(file->stream);
    if (iChar == EOF)
    {
	if (file->reverse)
	{
	    if (fseek(file->stream, -1, SEEK_END) == 0)
	    {
		iChar = fgetc(file->stream);
	    }
	    else
	    {
	        goto epilogue;
	    }
	}
	else
	{
	    goto epilogue;
	}
    }

    if (file->reverse)
    {
        if (fseek(file->stream, -2, SEEK_CUR) != 0)
        {
           file->eof = TRUE;
        }
    }

epilogue:
    if (iChar == EOF)
    {
	file->eof = TRUE;
    }
    if (iChar == '\n')
    {
	file->lastNewlineOff2 = file->lastNewlineOff;
	file->lastNewlineOff = (bfile_get_off(file) + (file->reverse? 1:-1));
    }

    return iChar;
}


static int
bfile_backup(BFile file, long numBytes)
{
    int		returnValue = 0;
    long	posChange = 0;

    if (file->reverse)
    {
	posChange = numBytes;
    }
    else
    {
	posChange = -1 * numBytes;
    }

    if (fseek(file->stream, posChange, SEEK_CUR) == 0)
    {
	file->eof = FALSE;
    }
    else
    {
	file->eof = TRUE;
	returnValue = -1;
    }

    return returnValue;
}


static int
bfile_set_file(BFile file, FILE *stream)
{
    file->stream = stream;
    file->reverse = FALSE;
    bfileP_set_eof(file);
    return 0;
}


static int
bfile_reverse(BFile file)
{
    file->reverse = TRUE;
    bfileP_set_eof(file);
    return 0;
}

static int
bfile_forward(BFile file)
{
    file->reverse = FALSE;
    bfileP_set_eof(file);
    return 0;
}


static long
bfile_get_off(BFile file)
{
    long	off = -1;

    if (file->reverse)
    {
	if (file->eof)
	{
	    off = -1;
	}
	else
	{
	    off = ftell(file->stream);
	}
    }
    else
    {
        off = ftell(file->stream);
    }

    return off;
}


static int
bfile_set_off(BFile file, long off)
{
    int		returnValue = 0;
    if (fseek(file->stream, off, SEEK_SET) != 0)
    {
	returnValue = -1;
    }
    bfileP_set_eof(file);
    return returnValue;
}


static int 
bfile_save_state(BFile file, BFileState state)
{
    state->forward = bfile_is_forward(file);
    state->offset = bfile_get_off(file);
    state->lastNewlineOff = file->lastNewlineOff;
    state->lastNewlineOff2 = file->lastNewlineOff2;
    return 0;
}


static int
bfile_restore_state(BFile file, BFileState state)
{
    if (!(state->forward))
    {
	bfile_reverse(file);
    }
    else
    {
	bfile_forward(file);
    }
    bfile_set_off(file, state->offset);
    file->lastNewlineOff = state->lastNewlineOff;
    file->lastNewlineOff2 = state->lastNewlineOff2;

    return 0;
}


/******************* PRIVATE METHODS **********************/

static BOOL
bfileP_set_eof(BFile file)
{
    file->eof = FALSE;

    if (file->stream == NULL)
    {
	file->eof = TRUE;
	goto epilogue;
    }

    /* DTB_USER_CODE_START */
    /* DTB_USER_CODE_END*/

    if (file->reverse)
    {
	/* dunno... */
    }
    else
    {
	file->eof = (feof(file->stream) || ferror(file->stream));
    }

    /* DTB_USER_CODE_START */

	if (file->reverse)
	{
	    /* put some goobies here */
	}

    /* DTB_USER_CODE_END*/

epilogue:
    return 0;
}


CSegArray
cseg_array_create(void)
{
    CSegArray	newArray = (CSegArray)util_malloc(sizeof(CSegArrayRec));
    if (newArray != NULL)
    {
        memset(newArray, 0, sizeof(CSegArrayRec));
    }
    return newArray;
}


int
cseg_arrayP_destroy_impl(CSegArray *arrayInOutPtr)
{
    CSegArray	array = *arrayInOutPtr;
    if (array == NULL)
    {
	return 0;
    }

    util_free(array->segs);
    util_free(array);

    *arrayInOutPtr = NULL;
    return 0;
}


static int
cvt_offset_to_line(BFile file, long offset)
{
    static long		lastLineOff = 0;
    static int		lastLineNum = 1;
    int			lineNum = 1;
    long		curOff = 0;
    int			c = 0;
    BFileStateRec	startFileState;

    bfile_save_state(file, &startFileState);
    bfile_forward(file);

    if (offset >= lastLineOff)
    {
	curOff = lastLineOff;
	lineNum = lastLineNum;
    }
    else
    {
	curOff = 0;
	lineNum = 1;
    }
    bfile_set_off(file, curOff);
    while ((curOff < offset) && ((c = bfile_get_char(file)) != EOF))
    {
	++curOff;
	if (c == '\n')
	{
	    ++lineNum;
    	    lastLineNum = lineNum;
    	    lastLineOff = curOff;
	}
    }

    bfile_restore_state(file, &startFileState);

    /*
     * Return file to original state
     */
    return lineNum;
}


/********
 * This is here as a test (run this program on this file)
 */
/*
static int
old_style_params(x, y, z)
	int	x;
	char	*y;
	CUserSegRec	*z;
{
    x = x; y = y; z = z;	
    return 69;
}
*/


/*************************************************************************
 *************************************************************************
 **									**
 **		DEBUGGING SUPPORT					**
 **									**
 *************************************************************************
 *************************************************************************/


#ifdef MAIN

/* Internationalization defines */
nl_catd Dtb_project_catd = (nl_catd)-1;

/* Workaround for XPG4 API compatibility */
#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif

int 
main(int argc, char *argv[])
{
    STRING		inFilename = NULL;
    FILE		*inFile = NULL;
    CSegArray	segs = NULL;
    CSeg		cseg = NULL;
    int			numSegs = 0;
    int			segIndex = 0;
    int			usegIndex = 0;

    util_init(&argc, &argv);

    if (argc < 2)
    {
	util_printf_err("Usage %s <filename>\n", util_get_program_name());
	exit(1);
    }

    inFilename = argv[1];
    inFile = util_fopen_locked(inFilename, "r");
    if (inFile == NULL)
    {
	perror(inFilename);
	return -1;
    }

    numSegs = abmfP_parse_c_file(inFile, &segs);
    if (numSegs < 0)
    {
	util_printf_err("Error parsing C file. Aborting\n");
	exit(1);
    }
    util_printf_err("Segs found in file: %d\n", numSegs);
    if (numSegs < 1)
    {
	exit(0);
    }

    for (segIndex = 0; segIndex < numSegs; ++segIndex)
    {
	cseg = &(segs->segs[segIndex]);
	util_dprintf(1, "Seg: %s  offset:%ld length:%ld userSegs:%d\n", 
	    cseg->type == CSEG_GLOBAL? "[GLOBAL]": util_strsafe(cseg->name),
	    cseg->offset,
	    cseg->length,
	    cseg->userSegs.numSegs);
	if (cseg->text != NULL)
	{
            printf("|||%s|||\n", cseg->text);
	}

        if (cseg->userSegs.numSegs < 1)
        {
            printf("[ No user segments ]\n");
        }
        else
        {
            for (usegIndex = 0; usegIndex < cseg->userSegs.numSegs; ++usegIndex)
            {
                printf("--- user seg %d [line %ld] ---\n%s---\n", 
	                usegIndex, 
			cseg->userSegs.segs[usegIndex].line,
			util_strsafe(cseg->userSegs.segs[usegIndex].text));
            }
	}
    }
    printf("\n");

    return 0;
}
#endif /* MAIN */

/*
 * Leave this user segment at the end of the file (this file can be
 * used to test itself).
 */

/* DTB_USER_CODE_START */

static int
myfunc(void)
{
    return 0;
}
/* DTB_USER_CODE_END*/
