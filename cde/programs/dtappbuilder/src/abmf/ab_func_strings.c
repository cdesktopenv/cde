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
/* $XConsortium: ab_func_strings.c /main/4 1996/10/02 15:13:50 drk $ */
/*
 * ab_mk_func_strings
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <nl_types.h>
#include <ab_private/AB.h>
#include <ab/util_types.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>

/* Internationalization defines */
nl_catd Dtb_project_catd;

/* Workaround for XPG4 API compatibility */
#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif


/*
 * Buffered file
 */
typedef struct
{
    FILE	*stream;
    BOOL	reverse;
    BOOL	eof;
} B_FILE, *BFile;
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


typedef enum
{
    COMMENT_UNDEF = 0,
    COMMENT_SLASH_STAR,
    COMMENT_SLASH_SLASH,
    COMMENT_TYPE_NUM_VALUES
} COMMENT_TYPE;


typedef struct
{
    STRING	funcCode;
    STRING	funcProto;
    STRING	funcName;
} FileFuncInfoRec, *FileFuncInfo;

static int	write_func_var_decl(FILE *file, FileFuncInfo funcInfo);
static int	write_func_as_strings(FILE *file, FileFuncInfo funcInfo);
static int	get_func(FILE *file, FileFuncInfo funcInfo);
static int	skip_white(BFile file);
static int	skip_white_and_comment(BFile file);
static int 	write_c_string(FILE *file, STRING value);
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
static int grab_string_from_file(
			STRING *stringPtr,
			BFile	file,
			long	stringOff,
			long	stringLen);

static int	find_matching(BFile file, int matchChar, long *matchOffPtr);
static BOOL	char_is_legal_for_ident(int iChar);
static int	format_func_name(STRING *name);
static int	format_func_proto(STRING *proto);
static int	format_func_def(STRING *def);
static int	skip_comment(BFile file, COMMENT_TYPE type);
static int	skip_slash_star_comment(BFile file);
static int	skip_slash_slash_comment(BFile file);
static int	skip_slash_slash_comment_fwd(BFile file);
static int	skip_slash_slash_comment_bwd(BFile file);
static int	get_char_from_c_file(BFile file);
static COMMENT_TYPE	find_comment_start(BFile file, long *commentOffOut);


int 
main (int argc, char *argv[])
{
    STRING	inFilename = NULL;
    FILE	*inFile = NULL;
    FileFuncInfoRec	funcInfo;
    FileFuncInfo	funcs = NULL;
    int			numFuncs = 0;
    int			i = 0;

    /* 
    ** We haven't internationalized this utility program, but it uses
    ** libraries which are I18N-ized.  To resolve this we just set the
    ** message catalog descriptor to -1 here so we get default messages
    ** from library routines.
    */
    Dtb_project_catd = (nl_catd) -1;

    util_init(&argc, &argv);

    if (argc < 2)
    {
	fprintf(stderr, "Usage %s <filename>\n", argv[0]);
	exit(1);
    }

    inFilename = argv[1];
    inFile = util_fopen_locked(inFilename, "r");
    if (inFile == NULL)
    {
	perror(inFilename);
	return -1;
    }

    memset(&funcInfo, 0, sizeof(funcInfo));
    while (get_func(inFile, &funcInfo) >= 0)
    {
        fprintf(stderr, "Function: %s\n", util_strsafe(funcInfo.funcName));

	++numFuncs;
	funcs = (FileFuncInfo)realloc(
			funcs, numFuncs * sizeof(FileFuncInfoRec));
	if (funcs == NULL)
	{
	    fprintf(stderr, "Out of memory!\n");
	    exit(1);
	}
	funcs[numFuncs-1] = funcInfo;
        memset(&funcInfo, 0, sizeof(funcInfo));
    }
    fprintf(stderr, "Functions found in file: %d\n", numFuncs);
    if (numFuncs < 1)
    {
	exit(0);
    }

    for (i = 0; i < numFuncs; ++i)
    {
	write_func_var_decl(stdout, &(funcs[i]));
    }

    abio_puts(stdout, "\n\n\n");

    for (i = 0; i < numFuncs; ++i)
    {
	write_func_as_strings(stdout, &(funcs[i]));
    }


    return 0;
}


static int
write_func_var_decl(FILE *file, FileFuncInfo funcInfo)
{
    char	ptrVarName[256];
    STRING	funcNamePtr = NULL;

    funcNamePtr = funcInfo->funcName;
    if (strncmp(funcNamePtr, "dtb_", 4) == 0)
    {
	funcNamePtr += 4;
    }
    sprintf(ptrVarName, "abmfP_lib_%s", funcNamePtr);
    abio_printf(file, "extern LibFunc %s;\n", ptrVarName);
    return 0;
}


static int	
write_func_as_strings(FILE *file, FileFuncInfo funcInfo)
{
    char	recVarName[256];
    char	ptrVarName[256];
    char	*funcNamePtr = NULL;

    funcNamePtr = funcInfo->funcName;
    if (strncmp(funcNamePtr, "dtb_", 4) == 0)
    {
	funcNamePtr += 4;
    }
    sprintf(recVarName, "%%abmfP_lrc_%s", funcNamePtr);
    sprintf(ptrVarName, "%%abmfP_lib_%s", funcNamePtr);

    abio_puts(file, "\n");
    abio_puts(file,   "/*\n");
    abio_printf(file, " * %s\n", ptrVarName);
    abio_puts(file,   " */\n");

    abio_printf(file, "static LibFuncRec %s = \n", recVarName);
    abio_printf(file, "{\n");
    abio_indent(file);

    abio_puts(file, "/* name */\n");
    write_c_string(file, funcInfo->funcName);
    abio_puts(file, ",\n");
    abio_puts(file, "\n");

    abio_printf(file, "/* proto */\n");
    write_c_string(file, funcInfo->funcProto);
    abio_puts(file, ",\n");
    abio_puts(file, "\n");

    abio_printf(file, "/* def */\n");
    write_c_string(file, funcInfo->funcCode);

    abio_puts(file, "\n");
    abio_outdent(file);
    abio_printf(file, "}; /* %s */\n", recVarName);

    abio_printf(file, "LibFunc %s = &(%s);\n", 
	ptrVarName, recVarName);
    return 0;
}


static int 
write_c_string(FILE *file, STRING value)
{
    BOOL	done = FALSE;
    char	*nlPtr = NULL;
    char	*lineStart = value;
    int		oldNlChar = -1;

    while (!done)
    {
	nlPtr = strchr(lineStart, '\n');
	oldNlChar = -1;
	if (nlPtr != NULL)
	{
	    ++nlPtr;
	    oldNlChar = *nlPtr;
	    *nlPtr = 0;
	}
	abio_put_string(file, lineStart);
	if (nlPtr != NULL)
	{
	    *nlPtr = (char)oldNlChar;
	    oldNlChar = -1;
	}

	if (nlPtr == NULL)
	{
	    done = TRUE;
	}
	else
	{
	    lineStart = nlPtr;
  	    abio_puts(file, "\n");
	}
    }

    return 0;
}


static BOOL
char_is_legal_for_ident(int iChar)
{
    return  (isalnum(iChar) || (iChar == '_'));
}


static int
find_func(
		BFile	file,
		long	*funcOffPtr,
		long	*funcLenPtr,
		long	*bodyOffPtr,
		long	*bodyLenPtr
)
{
#define funcOff (*funcOffPtr)
#define funcLen (*funcLenPtr)
#define bodyOff (*bodyOffPtr)
#define bodyLen (*bodyLenPtr)
    int		returnValue = 0;
    long	openingBraceOff = 0;
    long	closingBraceOff = 0;
    int		iChar = -1;

    skip_white(file);

    /*
     * Scan, looking for opening {
     */		/* } vi hack */
    funcOff = bfile_get_off(file);
    openingBraceOff = -1;
    while ((openingBraceOff < 0) && (!bfile_eof(file)))
    {
	iChar = get_char_from_c_file(file);
	switch (iChar)
	{
	    case ';':	/* function must start after one of these chars */
		funcOff = bfile_get_off(file);
	    break;

	    case '{':	/* } vi hack */
	        openingBraceOff = bfile_get_off(file) - 1;
	    break;
	}
    }
    if ((openingBraceOff < 0) || (openingBraceOff <= funcOff))
    {
	return -1;
    }
    /* printf("opening brace %ld\n", openingBraceOff); */

    if (find_matching(file, '}', &closingBraceOff) < 0)
    {
	fprintf(stderr, "Mismatched braces {}.\nn");
	return -1;
    }
    /* printf("closing brace %ld\n", closingBraceOff); */

    bodyOff = openingBraceOff;
    bodyLen = (closingBraceOff - openingBraceOff) + 1;
    funcLen = (closingBraceOff - funcOff + 1);
    if ((bodyLen < 0) || (funcLen < 0))
    {
	returnValue = -1;
    }

    return returnValue;
#undef funcOff
#undef funcLen
#undef bodyOff
#undef bodyLen
}


static int
trim_opening_comment(
			BFile	file,
			long	*funcOffPtr,
			long	*funcLenPtr,
			long	protoOff
)
{
#define funcOff (*funcOffPtr)
#define funcLen (*funcLenPtr)
    long	startOff = bfile_get_off(file);
    long	newCommentOff = -1;
    long	commentOff = -1;
    long		funcLenDiff = 0;
    long		newFuncOff = 0;
    COMMENT_TYPE	commentType;
    BOOL		done = FALSE;

    bfile_set_off(file, funcOff);
    bfile_forward(file);

    while (!done)
    {
        commentType = find_comment_start(file, &newCommentOff);
        if (   (commentType == COMMENT_UNDEF)
	    || (newCommentOff >= protoOff) )
	{
	    done = TRUE;
	}
	else
	{
	    commentOff = newCommentOff;
	    skip_comment(file, commentType);
	}
    }

    if (commentOff > 0)
    {
	funcLenDiff = (funcOff - commentOff);
	funcOff = commentOff;
	funcLen += funcLenDiff;
    }
	
    /* 
     * trim white space 
     */
    bfile_set_off(file, funcOff);
    skip_white(file);
    newFuncOff = bfile_get_off(file);
    funcLenDiff = (funcOff - newFuncOff);
    funcOff = newFuncOff;
    funcLen += funcLenDiff;

    bfile_set_off(file, startOff);
    return 0;
#undef funcOff
#undef funcLen
}


static int
find_func_name(
			BFile	file,
			long	funcOff,
			long	bodyOff,
			long	*nameOffPtr,
			long	*nameLenPtr
)
{
#define nameOff (*nameOffPtr)
#define nameLen (*nameLenPtr)
    int		returnValue = 0;
    int		iChar = EOF;
    long	nameEndOff = 0;

    bfile_forward(file);
    bfile_set_off(file, bodyOff);
    bfile_reverse(file);
    get_char_from_c_file(file);	/* skip '{' */
    skip_white_and_comment(file);
    if ((iChar = get_char_from_c_file(file)) != ')')
    {
	fprintf(stderr, "Syntax error near function name.\n");
	return -1;
    }
    if (find_matching(file, '(', NULL) < 0)
    {
	fprintf(stderr, "Unbalanced parens.\n");
	return -1;
    }

    skip_white_and_comment(file);
    nameEndOff = bfile_get_off(file);
    iChar = get_char_from_c_file(file);
    while ((iChar != EOF) && (char_is_legal_for_ident(iChar)))
    {
	iChar = bfile_get_char(file);
    }
    nameOff = bfile_get_off(file) + 2;
    if ((nameOff < funcOff) || (nameOff > nameEndOff))
    {
	returnValue = -1;
    }
    else
    {
        nameLen = (nameEndOff - nameOff + 1);
    }

    return returnValue;
#undef nameOff
#undef nameLen
}

static int	
find_func_proto(
			BFile	file,
			long	funcOff,
			long	nameOff,
			long	bodyOff,
			long	*protoOffPtr,
			long	*protoLenPtr
)
{
#define protoOff (*protoOffPtr)
#define protoLen (*protoLenPtr)

    int 	returnValue = 0;
    long	protoEndOff = -1;
    int		iChar = -1;
    long	lastValidOff = -1;
    COMMENT_TYPE	commentType = COMMENT_UNDEF;
    long		commentOff;
    BOOL		done = FALSE;

    bfile_forward(file);

    /*
     * find start of type info (first non-space, non-comment chars)
     */
    protoOff = funcOff;
    bfile_set_off(file, protoOff);
    done = FALSE;
    while (!done)
    {
	commentType = find_comment_start(file, &commentOff);
	if ((commentType == COMMENT_UNDEF) || (commentOff >= bodyOff))
	{
	    done = TRUE;
	}
	else
	{
	    protoOff = commentOff;
	}

    }
    bfile_set_off(file, protoOff);
    skip_white_and_comment(file);
    protoOff = bfile_get_off(file);

    /*
     * Look for end of prototype
     */
    bfile_forward(file);
    bfile_set_off(file, bodyOff);
    bfile_reverse(file);
    get_char_from_c_file(file);
    skip_white_and_comment(file);
    protoEndOff = bfile_get_off(file);

    if (protoEndOff <= protoOff)
    {
	returnValue = -1;
    }
    else
    {
	protoLen = (protoEndOff - protoOff + 1);
    }

    return returnValue;
#undef protoOff
#undef protoLen
}


static int 
get_func(FILE *file, FileFuncInfo funcInfo)
{
    int		rc = 0;		/* return code */
    B_FILE	bfileRec;
    BFile	bfile = &bfileRec;
    long	funcOff = -1;
    long	funcLen = -1;
    long	bodyOff = -1;
    long	bodyLen = -1;
    long	nameOff = -1;
    long	nameLen = -1;
    long	protoOff = -1;
    long	protoLen = -1;
    BOOL	foundFunc = FALSE;
    BOOL	abort = FALSE;
    BOOL	err = FALSE;
    long	off = 0;

    bfile_construct(bfile, file);
    abort = FALSE;
    foundFunc = FALSE;
    while ((!abort) && (!foundFunc) && (!bfile_eof(bfile)))
    {
	err = FALSE;

	if (!err)
	{
            rc = find_func(bfile, &funcOff, &funcLen, &bodyOff, &bodyLen);
            if (rc < 0)
            {
	        err = TRUE;
		abort = TRUE;
            }
            /* printf("find_func: %ld %ld %ld\n", funcOff, bodyOff, bodyLen); */
	}
    
	if (!err)
	{
            rc = find_func_name(bfile, funcOff, bodyOff, &nameOff, &nameLen);
            if (rc < 0)
            {
	        err = TRUE;
	    }
            /* printf("func_name: %ld %ld\n", nameOff, nameLen); */
        }
    
	if (!err)
	{
            rc = find_func_proto(bfile, funcOff, nameOff, bodyOff, 
				    &protoOff, &protoLen);
            if (rc < 0)
            {
	        err = TRUE;
            }
            /* printf("proto: %ld %ld\n", protoOff, protoLen); */
	}

	if (!err)
	{
            rc = trim_opening_comment(bfile, &funcOff, &funcLen, protoOff);
	    if (rc < 0)
	    {
		err = TRUE;
	    }
 	}

	if (!err)
	{
	    foundFunc = TRUE;
	}

	/* printf("new offset %ld\n", funcOff + funcLen + 1); */
	bfile_set_off(bfile, funcOff + funcLen + 1);
	bfile_forward(bfile);
    }

    /*
     * Put the info into the func structure
     */
	
    if (foundFunc)
    {
        grab_string_from_file(&(funcInfo->funcName), bfile, nameOff, nameLen);
	format_func_name(&(funcInfo->funcName));
        grab_string_from_file(&(funcInfo->funcProto), bfile, protoOff, protoLen);
	format_func_proto(&(funcInfo->funcProto));
        grab_string_from_file(&(funcInfo->funcCode), bfile, funcOff, funcLen);
	format_func_def(&(funcInfo->funcCode));
    }

    bfile_destruct(bfile);
    return foundFunc?0:-1;
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

    string = (STRING)util_malloc((size_t)(stringLen + 1));
    bfile_set_off(file, stringOff);
    fread(string, 1, (size_t)stringLen, file->stream);
    string[stringLen]= 0;

    return returnValue;
#undef string
}


static int
find_matching(BFile file, int matchChar, long *matchOffPtr)
{
    int		returnValue = 0;
    int		iChar = -1;
    BOOL	foundMatching = FALSE;
    BOOL	quit = FALSE;
    long	startOff = bfile_get_off(file);

    /* printf("find_matching(%c) at %ld\n", matchChar, startOff); */
    while ((!quit) && (!bfile_eof(file)))
    {
	iChar = get_char_from_c_file(file);
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

	     case '{':
		 if (find_matching(file, '}', NULL) < 0)
		 {
		     fprintf(stderr, "Unbalanced braces {}.\n");
		     quit = TRUE;
		     returnValue = -1;
		 }
	     break;

	     case '}':
		 if (find_matching(file, '{', NULL) < 0)
		 {
		     fprintf(stderr, "Unbalanced braces {}\n");
		     quit = TRUE;
		     returnValue = -1;
		 }
	    break;

	    case '(':
		if (find_matching(file, ')', NULL) < 0)
		{
		    fprintf(stderr, "Unbalanced parens ()\n");
		    quit = TRUE;
		    returnValue = -1;
		}
	    break;

	    case ')':
		if (find_matching(file, '(', NULL) < 0)
		{
		    fprintf(stderr, "Unbalanced parens ()\n");
		    quit = TRUE;
		    returnValue = -1;
		}
	    break;
	    }
	}
    }

    if (foundMatching)
    {
	/* printf("<<match found(%c) for %ld>>\n", matchChar, startOff); */
	if (matchOffPtr != NULL)
	{
	    *matchOffPtr = bfile_get_off(file);
	    if (file->reverse)
	    {
		++(*matchOffPtr);
	    }
	    else
	    {
		--(*matchOffPtr);
	    }
	}
    }
    if ((returnValue >=0) && (!foundMatching))
    {
	returnValue = -1;
    }

    return returnValue;
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
    int	iChar = get_char_from_c_file(file);

    while ((iChar != EOF) && (isspace(iChar)))
    {
	iChar = get_char_from_c_file(file);
    }
    if (iChar != EOF)
    {
	bfile_backup(file, 1);
    }
    return 0;
}

static int	
format_func_name(STRING *namePtr)
{
#define name (*namePtr)
    return 0;
#undef name
}


static int	
format_func_proto(STRING *protoPtr)
{
#define proto (*protoPtr)
    int		protoLen = strlen(proto);
    int		i = 0;

    /*
     * Add ;
     */
    proto = (STRING)realloc(proto, protoLen+2);
    strcat(proto, ";");
    ++protoLen;

    /*
     * Remove preceding space
     */
    while ((protoLen > 0) && isspace(proto[0]))
    {
	memmove(proto, proto+1, protoLen--);
    }


    for (i = 0; i < protoLen; ++i)
    {
	if (proto[i] == '(')
	{
	    break;
	}
	if (proto[i] == '\n')
	{
	    proto[i] = ' ';
	}
    }


    return 0;
#undef proto
}


static int	
format_func_def(STRING *defPtr)
{
#define def (*defPtr)
    return 0;
#undef def
}


/*
 * Gets a char from a C source file, skipping comments
 */
static int
get_char_from_c_file(BFile file)
{
    int		iChar = EOF;
    BOOL	done = FALSE;

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
		    if (skip_slash_slash_comment(file) < 0)
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
			skip_slash_slash_comment(file);
		    }
		    else if (iChar == '*')
		    {
			skip_slash_star_comment(file);
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
			skip_slash_star_comment(file);
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


static COMMENT_TYPE
find_comment_start(BFile file, long *commentOffOutPtr)
{
#define commentOffOut (*commentOffOutPtr)
    COMMENT_TYPE	commentType = COMMENT_UNDEF;
    int			iChar = -1;
    BOOL		done = FALSE;
    long		commentOff = 0;

    while (!done)
    {
	commentOff = bfile_get_off(file);
        iChar = bfile_get_char(file);
	if (iChar == EOF)
	{
	    done = TRUE;
	}
	else if (iChar == '/')
	{
	    iChar = bfile_get_char(file);
	    if (iChar == EOF)
	    {
		done = TRUE;
	    }
	    else if (iChar == '*')
	    {
		commentType = COMMENT_SLASH_STAR;
		done = TRUE;
	    }
	    else if (iChar == '/')
	    {
	        commentType = COMMENT_SLASH_SLASH;
	        done = TRUE;
	    }
	    else
	    {
	        bfile_backup(file, 1);
	    }
	}
    }

    if (commentType != COMMENT_UNDEF)
    {
	commentOffOut = commentOff;
    }

    return commentType;
#undef commentOffOut
}


static int
skip_comment(BFile file, COMMENT_TYPE commentType)
{
    int		return_value = -1;

    switch(commentType)
    {
	case COMMENT_SLASH_STAR:
	    return_value = skip_slash_star_comment(file);
	break;

	case COMMENT_SLASH_SLASH:
	    return_value = skip_slash_slash_comment(file);
	break;
    }

    return return_value;
}


static int
skip_slash_slash_comment(BFile file)
{
    int		return_value = -1;

    if (bfile_is_forward(file))
    {
	return_value = skip_slash_slash_comment_fwd(file);
    }
    else
    {
	return_value = skip_slash_slash_comment_bwd(file);
    }
    return return_value;
}



static int
skip_slash_slash_comment_fwd(BFile file)
{
    int		iChar = 0;

    while ((iChar != EOF) && (iChar != '\n'))
    {
        iChar = bfile_get_char(file);
    }

    return 0;
}


static int
skip_slash_slash_comment_bwd(BFile file)
{
    long	startOffset = bfile_get_off(file);
    BOOL	foundCommentStart = FALSE;
    BOOL	done = FALSE;
    int		iChar = 0;

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
	bfile_set_off(file, startOffset);
    }

    return (foundCommentStart? 0:-1);
}


static int
skip_slash_star_comment(BFile file)
{
    int		iChar = 0;
    BOOL	done = FALSE;
    BOOL	foundCommentEnd = FALSE;

    while (!done)
    {
	iChar = bfile_get_char(file);
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

    return (foundCommentEnd? 0:-1);
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
    /*
    if ((iChar != EOF) && (!(file->reverse)))
    {
	printf("%c", iChar);
    }
    */
    if (iChar == EOF)
    {
	file->eof = TRUE;
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

    if (file->reverse)
    {
	/* dunno... */
    }
    else
    {
	file->eof = (feof(file->stream) || ferror(file->stream));
    }

epilogue:
    return 0;
}


