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

/*
 * $XConsortium: resource_file.c /main/3 1995/11/06 18:13:17 rswiston $
 * 
 * @(#)resource_file.c	1.12 04 May 1995	cde_app_builder/src/abmf
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * resource_file.c
 */

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <ab_private/util_err.h>
#include <ab_private/abio.h>
#include "abmfP.h"
#include "utilsP.h"
#include "resource_fileP.h"


/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

#define MAX_USER_SEGS 2

typedef struct
{
    long	offset;
    long	len;
} UserSegRec, *UserSeg;


typedef struct
{
    UserSegRec	segs[MAX_USER_SEGS];
    int		numSegs;
} UserSegsRec, *UserSegs;


typedef struct
{
    ISTRING	fileName;
    File	file;
    BOOL	isIntermediate;
} ResFileInfoRec, *ResFileInfo;

static ResFileInfoRec	resFile;
static STRING		userSegStartString = "DTB_USER_RES_START";
static STRING		userSegEndString = "DTB_USER_RES_END";

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/

static int	write_header(
			File	file, 
			STRING	fileName, 
			ABObj	project, 
			BOOL	isIntermediateFile
			);

static int	write_footer(
			File	file, 
			BOOL	isIntermediateFile
			);

static int	abmfP_res_file_append_intermediate_files(
			File	appResFile,
			ABObj	project
			);

static int	append_intermediate_res_file(
			File toFile, 
			File intFile
		);

static STRING	get_name_for_comment(ABObj obj, STRING buf, int bufSize);
static int	find_user_segs(File file, UserSegs segs);
static int	find_user_seg(File file, UserSeg segs);
static long	find_user_seg_begin(File file);
static long	find_user_seg_end(File file);

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/


/*
 * <0 = error
 *  0 = file written and complete
 *  1 = file written, but is incomplete.
 */
int
abmfP_write_app_res_file(
			File	file,
			ABObj	project,
			STRING	resFileName
)
{
    int		return_value = 0;
    int		rc = 0;
    assert(obj_is_project(project));
    assert(resFile.file = file);

    rc = abmfP_res_file_append_intermediate_files(file, project);
    write_footer(file, resFile.isIntermediate);

    return_value = rc;

    return rc;
}


/*
 * If the resource file exists, the file is backed up and copied. References
 * to modules that will be rewritten are deleted.
 */
File
abmfP_res_file_open(
			STRING	filePath,
			STRING	fileDescName,
			ABObj	obj, 
			BOOL	isIntermediateFile
)
{
    File                fp = NULL;

    if (filePath != NULL)
    {
	if ((fp = util_fopen_locked(filePath, "w")) == NULL)
	{
	    goto epilogue;
	}
    }
    else
    {
        if ((fp = tmpfile()) == NULL)
	{
	    goto epilogue;
	}
    }

    write_header(fp, fileDescName, obj, isIntermediateFile);

epilogue:
    if (fp != NULL)
    {
	resFile.fileName = istr_create(filePath);
	resFile.file = fp;
	resFile.isIntermediate = isIntermediateFile;
    }
    return fp;
}


int
abmfP_res_file_close_impl(File *pfile)
{
#define closeFile (*pfile)
    if (closeFile != NULL)
    {
	assert(resFile.file == closeFile);
	util_fclose(closeFile);
    }

    /* reset resFile */
    istr_destroy(resFile.fileName);
    memset((void *)&resFile, 0, sizeof(ResFileInfoRec));

    closeFile = NULL;
    return 0;
#undef closeFile
}


int
abmfP_res_file_merge(
			File	vanillaFile,
			File	modifiedFile,
			File	*mergedFileOut
)
{
    int		return_value = 0;
    int		rc = 0;
    long	userSegOff = 0;
    File	mergedFile = NULL;
    int		iChar = 0;
    int		iLastChar = 0;
    int		i = 0;
    UserSegsRec	vanillaUserSegs;
    UserSegsRec	modifiedUserSegs;
    long	vanillaOffset = 0;
    int		curSegIndex = 0;
    UserSeg	curVanillaSeg = NULL;
    UserSeg	curModifiedSeg = NULL;
    long	modifiedOffset = 0;

    *mergedFileOut = NULL;

    rc = find_user_segs(vanillaFile, &vanillaUserSegs);
    return_if_err(rc,rc);
    rc = find_user_segs(modifiedFile, &modifiedUserSegs);
    return_if_err(rc,rc);

    /*
     * Copy the "vanilla" file to the merged file
     */
    if ((mergedFile = tmpfile()) == NULL)
    {
	return ERR_INTERNAL;
    }

    rewind(vanillaFile);
    vanillaOffset = -1;
    curSegIndex = 0;
    curVanillaSeg = &(vanillaUserSegs.segs[curSegIndex]);
    curModifiedSeg = &(modifiedUserSegs.segs[curSegIndex]);
    while ((iChar = fgetc(vanillaFile)) != EOF)
    {
	++vanillaOffset;
	if (   (curVanillaSeg != NULL) 
	    && (vanillaOffset >= curVanillaSeg->offset) )
	{
	    /*
	     * We just hit a segment - replace with modified segment
	     */
	    modifiedOffset = curModifiedSeg->offset;
	    fseek(modifiedFile, modifiedOffset, SEEK_SET);
	    for (i = 0; i < curModifiedSeg->len; ++i)
	    {
		iChar = fgetc(modifiedFile);
		assert(iChar != EOF);
		fputc(iChar, mergedFile);
	    }

	    fseek(vanillaFile, 
		curVanillaSeg->offset + curVanillaSeg->len, SEEK_SET);
	    vanillaOffset = ftell(vanillaFile)-1;

	    if (++curSegIndex < 2)
	    {
                curVanillaSeg = &(vanillaUserSegs.segs[curSegIndex]);
                curModifiedSeg = &(modifiedUserSegs.segs[curSegIndex]);
	    }
	    else
	    {
                curVanillaSeg = NULL;
                curModifiedSeg = NULL;
	    }
	}
	else
	{
	    /* not in a segment */
	    fputc(iChar, mergedFile);
	}
    }

    /*
     * End-of-file segment may have gotten skipped
     */
    if (curModifiedSeg != NULL)
    {
	fseek(modifiedFile, curModifiedSeg->offset, SEEK_SET);
	for (i = 0; i < curModifiedSeg->len; ++i)
	{
	    iChar = fgetc(modifiedFile);
	    assert(iChar != EOF);
	    fputc(iChar, mergedFile);
	}
    }

epilogue:
    *mergedFileOut = mergedFile; mergedFile = NULL;
    return return_value;
}


static int
find_user_segs(File file, UserSegs segs)
{
    int		return_value = 0;
    int		rc = 0;

    rewind(file);
    memset(segs, 0, sizeof(UserSegsRec));

    while (   (segs->numSegs < MAX_USER_SEGS)
	   && ((rc = find_user_seg(file, &(segs->segs[segs->numSegs]))) >= 0) )
    {
	++(segs->numSegs);
    }
    if ((rc < 0) && (rc != ERR_EOF))
    {
        return_code(rc);
    }
    if (segs->numSegs != 2)
    {
	util_printf_err(
	  "There are %d DTB_USER segments in the file (there should be two)\n",
	  segs->numSegs);
	return_value = ERR_INTERNAL;
    }

epilogue:
    return return_value;
}


static int
find_user_seg(File file, UserSeg seg)
{
    long	userSegStart = -1;
    long	userSegEnd = -1;

    if ((userSegStart = find_user_seg_begin(file)) < 0)
    {
	return (int)userSegStart;
    }

    if ((userSegEnd = find_user_seg_end(file)) < 0)
    {
	if (userSegEnd == ERR_EOF)
	{
	    /* segment goes to EOF */
	    struct stat	fileInfo;
	    if (fstat(fileno(file), &fileInfo) == 0)
	    {
	        userSegEnd = ((long)fileInfo.st_size) - 1;
		userSegEnd = util_max(userSegEnd,0);
	    }
	}
	else
	{
	    return (int)userSegEnd;
	}
    }

    seg->offset = userSegStart;
    seg->len = userSegEnd - userSegStart + 1;

    return 0;
}


static long
find_user_seg_begin(File modifiedResFile)
{
    long	userSegOff = -1;
    char	lineBuf[256];
    int		lineOff = 0;
    char	magicLine[256];
    int		magicLineLen = 0;
    int		iChar = 0;
    int		iLastChar = 0;
    *lineBuf = 0;
    *magicLine = 0;

    sprintf(magicLine, "! %s", userSegStartString);
    magicLineLen = strlen(magicLine);

    while (   (userSegOff < 0)
	   && ((iChar = fgetc(modifiedResFile)) != EOF) )
    {
	if (iChar == '\n')
	{
	    lineOff = 0;
	}
	else if (lineOff < magicLineLen)
	{
	    lineBuf[lineOff++] = iChar;
	    if (lineOff == magicLineLen)
	    {
		lineBuf[lineOff] = 0;
		if (util_streq(lineBuf, magicLine))
		{
		    /* look for end of comment section */
		    while ((iChar = fgetc(modifiedResFile)) != EOF)
		    {
			if (iChar == '\n')
			{
			    userSegOff = ftell(modifiedResFile);
			    break;
			}
			iLastChar = iChar;
		    }
		    lineOff = 0;
		}
	    }
	}
	iLastChar = iChar;
    }

    /*
     * See if the user segment is longer than 0 bytes
     */
    if (userSegOff < 0)
    {
	if (iChar == EOF)
	{
	    userSegOff = ERR_EOF;
	}
    }
    else if (userSegOff >= 0)

    return userSegOff;
}


static long
find_user_seg_end(File file)
{
    long	userSegEndOff = -1;
    char	lineBuf[256];
    int		lineOff = 0;
    char	magicLine[256];
    int		magicLineLen = 0;
    long	lastNewlineOff = 0;
    int		iChar = 0;
    *lineBuf = 0;
    *magicLine = 0;

    sprintf(magicLine, "! %s", userSegEndString);
    magicLineLen = strlen(magicLine);

    lastNewlineOff = ftell(file) - 1;
    while ((iChar = fgetc(file)) != EOF)
    {
	if (iChar == '\n')
	{
	    lastNewlineOff = ftell(file)-1;
	    lineOff = 0;
	}
	else if (lineOff <= magicLineLen)
	{
	    lineBuf[lineOff++] = iChar;
	    if (lineOff == magicLineLen)
	    {
		lineBuf[lineOff] = 0;
		if (util_streq(lineBuf, magicLine))
		{
		    userSegEndOff = lastNewlineOff;
		    goto epilogue;
		}
	    }
	}
    }

epilogue:
    if ((userSegEndOff < 0) && (iChar == EOF))
    {
	userSegEndOff = ERR_EOF;
    }

    return userSegEndOff;
}


STRING
abmfP_get_intermediate_res_file_name(
			ABObj	obj,
			char	*fileNameOut,
			int	nameMax
)
{
    ABObj	resFileObj = NULL;
    nameMax = nameMax;	/* avoid warning */

    if ((resFileObj = obj_get_module(obj)) == NULL)
    {
	resFileObj = obj_get_project(obj);
    }
    assert(resFileObj != NULL);

    strcpy(fileNameOut, obj_get_name(resFileObj));
    if (obj_is_project(resFileObj))
    {
	strcat(fileNameOut, ".pres");
    }
    else
    {
	strcat(fileNameOut, ".res");
    }
    return fileNameOut;
}


STRING
abmfP_get_app_res_file_name(
			ABObj	obj,
			char	*fileNameOut,
			int	nameMax
)
{
    ABObj	project = obj_get_project(obj);
    STRING	fileName = NULL;

    strcpy(fileNameOut, obj_get_name(obj));
    fileName = abmfP_capitalize_first_char(fileNameOut);
    if (fileName != fileNameOut)
    {
	util_strncpy(fileNameOut, fileName, nameMax);
    }

    return fileNameOut;
}


/*************************************************************************
 **									**
 **	PRIVATE FUNCTIONS						**
 **									**
 *************************************************************************/

static int 
write_header(File file, STRING fileName, ABObj obj, BOOL isIntermediateFile)
{
    char	buf[MAXPATHLEN+1];
    assert(obj_is_project(obj) || obj_is_module(obj));

    if (!isIntermediateFile)
    {
        abio_printf(file,
    	    "! %s - vvv Add file header below vvv\n"
    	    "! %s - ^^^ Add file header above ^^^\n"
    	    "\n",
	        userSegStartString,
	        userSegEndString);
    }

    abio_printf(file, 
	    "! File: %s\n"
	    "!\n", 
		fileName);

    if (isIntermediateFile)
    {
        abio_printf(file, 
	    "! Intermediate resource file for %s\n",
		get_name_for_comment(obj, buf, MAXPATHLEN));
	abio_puts(file,   
	    "!\n"
	    "! ** DO NOT EDIT BY HAND. ALL MODIFICATIONS WILL BE LOST!\n");
    }
    else
    {
	assert(obj_is_project(obj));
	abio_printf(file,
	"! Resource file for application %s\n", obj_get_name(obj));
        abio_printf(file,
 	"! This file was generated by %s, from project %s\n",
		util_get_program_name(), obj_get_name(obj));
	abio_printf(file,
"!\n"
"! Any text may be added between the %s\n"
"! and %s comments. Descriptive comments are\n"
"! provided only as an aid.\n"
"!\n"
"!  ** EDIT ONLY WITHIN SECTIONS MARKED WITH DTB_USER_ COMMENTS.      **\n"
"!  ** ALL OTHER MODIFICATIONS WILL BE OVERWRITTEN. DO NOT MODIFY OR  **\n"
"!  ** DELETE THE GENERATED COMMENTS!                                 **\n"
"!\n",
		userSegStartString, userSegEndString);

    }

    /*
     * MUST END HEADER WITH \n\n, FOR APP RESOURCE FILE GENERATION TO WORK!!
     */
    abio_puts(file,nlstr);

    return 0;
}


static int
write_footer(File file, BOOL isIntermediate)
{
    if (isIntermediate)
    {
	/* intermediate files have no footer */
	return 0;
    }

    abio_printf(file, 
"\n"
"\n"
"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
"!\n"
"! The remainder of this file (following this comment) may be modified.\n"
"! Add new resources or override values defined above.\n"
"!\n"
"! %s\n",
	userSegStartString);

     return 0;   
}


/*
 * < 0 = error
 * 0 = file built completely, all intermediate files found
 * 1 = file built, but may be incomplete
 *
 * We always write out the individual comments in the output file, showing
 * where the individual modules go. This is so that modules with no
 * resources will create a file identical to modules that have not had
 * a resource file generated, yet. This way, we can tell if the module that
 * is currently being generated actually affects the resource file by 
 * seeing if the app resource file changes.
 *
 * It also makes it obvious to whoever looks at the app resource file
 * if resources for a particular module exist.
 */
static int
abmfP_res_file_append_intermediate_files(
			File	appResFile,
			ABObj	project
)
{
    int			return_value = 0;
    char		partResFileName[MAX_PATH_SIZE];
    File		partResFile = NULL;
    AB_TRAVERSAL	trav;
    ABObj		module = NULL;
    BOOL		allFilesFound = TRUE;
    char		nameBuf[MAXPATHLEN];
    *partResFileName = 0;

    /*
     * Get the project resources
     */
    abmfP_get_intermediate_res_file_name(
	project, partResFileName, MAX_PATH_SIZE);
    partResFile = util_fopen_locked(partResFileName, "r");

    /* always put comment in, whether file exists, or not */
    abio_printf(appResFile, "\n! Resource list for %s\n!\n",
		get_name_for_comment(project, nameBuf, MAXPATHLEN));
    if (partResFile == NULL)
    {
	allFilesFound = FALSE;
    }
    else
    {
	append_intermediate_res_file(appResFile, partResFile);
    }
    util_fclose(partResFile);

    /*
     * Get the module res files
     */
    for (trav_open(&trav, project, AB_TRAV_MODULES);
	(module = trav_next(&trav)) != NULL; )
    {
	abmfP_get_intermediate_res_file_name(
		module, partResFileName, MAX_PATH_SIZE);
	partResFile = util_fopen_locked(partResFileName, "r");

        /* always put comment in, whether file exists, or not */
        abio_printf(appResFile, "\n! Resource list for %s\n!\n",
			get_name_for_comment(module, nameBuf, MAXPATHLEN));
	if (partResFile == NULL)
	{
	    allFilesFound = FALSE;
	}
	else
	{
	    append_intermediate_res_file(appResFile, partResFile);
	}
        util_fclose(partResFile);
    }
    trav_close(&trav);

    if ((return_value >= 0) && (!allFilesFound))
    {
	return_value = 1;
    }
    return return_value;
}


static int
append_intermediate_res_file(
			File toFile, 
			File intFile
)
{
    int		return_value = 0;
    int		iChar = 0;
    int		iLastChar = 0;

    /*
     * Skip file header (the header will always be terminated by one or
     * more empty lines.
     */
    while ((iChar = getc(intFile)) != EOF)
    {
        if ((iLastChar == '\n') && (iChar == '\n'))
        {
	    break;
        }
        iLastChar = iChar;
    }
    while ((iChar = getc(intFile)) != EOF)
    {
	if (iChar != '\n')
	{
	    ungetc(iChar, intFile);
	    break;
	}
    }

    /*
     * Copy the rest of the file
     */
    while ((iChar = getc(intFile)) != EOF)
    {
        fputc(iChar, toFile);
    }

    return return_value;
}


static STRING
get_name_for_comment(ABObj obj, STRING buf, int bufSize)
{
    assert(bufSize >= util_strlen(obj_get_name(obj)) + 20);
    assert(obj_is_project(obj) || obj_is_module(obj));
    assert(obj_get_name(obj) != NULL);

    strcpy(buf, "AppBuilder ");
    if (obj_is_project(obj))
    {
	strcat(buf, "project ");
    }
    else if (obj_is_module(obj))
    {
	strcat(buf, "module ");
    }

    strcat(buf, obj_get_name(obj));

    return buf;
}


