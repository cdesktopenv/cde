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

/*
 *	$XConsortium: util.h /main/4 1995/11/06 18:52:47 rswiston $
 *
 * @(#)util.h	1.39 14 Feb 1994	cde_app_builder/src/libAButil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/*
 * util.h - general utilities
 */
#ifndef _AB_ABUTIL_H_
#define _AB_ABUTIL_H_

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ab_private/AB.h>
#include <ab/util_types.h>
#include <ab_private/util_err.h>

/*
 * <string.h> seems to have a problem cc -Xc is continually complaining
 * about strdup().
 */
#ifdef __cplusplus
extern "C" {
#endif

#if !defined(linux)
extern char	*strdup(const char *);
#endif

#ifdef __cplusplus
} /*extern "C" */
#endif


typedef enum
{
    AB_OS_UNDEF = 0,
    AB_OS_HPUX,
    AB_OS_AIX,
    AB_OS_SUNOS,
    AB_OS_OSF1,
    AB_OS_UNIXWARE,
    AB_OS_UXP,
    AB_OS_LNX,
    AB_OS_FBSD,
    AB_OS_NBSD,
    AB_OS_OBSD,
    AB_OS_TYPE_NUM_VALUES	/* must be last */
} AB_OS_TYPE;


typedef int UTIL_OUTPUT_HANDLER(STRING error_message);
typedef UTIL_OUTPUT_HANDLER *UtilOutputHandler;

extern const STRING	Util_null_string;	/* "(nil)" */
extern const STRING	Util_empty_string;	/* "" */
extern int		utilP_verbosityPriv3602759317;	/* private! */

/*
 * Initialization
 */
int	util_init(int *argc_in_out, STRING **argv_in_out);

/*
 *  Memory management
 */
#define util_free(ptr) {if ((ptr) != NULL) {free(ptr); (ptr)= NULL;}}
#define util_malloc(size) (malloc(size))


/*
 * General
 */
int	util_putenv(STRING string);
#define util_min(a,b) (((a) < (b))? (a):(b))
#define util_max(a,b) (((a) > (b))? (a):(b))
#define util_xor(a,b) ( (((a)?1:0) + ((b)?1:0)) == 1 ) /* logical xor */
int	util_set_program_name(STRING this_prog_name);
STRING	util_get_program_name(void);
int	util_set_program_name_from_argv0(STRING argv0);
AB_OS_TYPE	util_get_os_type(void);
STRING		util_os_type_to_string(AB_OS_TYPE);
STRING		util_os_type_to_ident(AB_OS_TYPE);


/*
 * Verbosity
 *
 * If verbosity is >= 3, debugging is turned on.
 *      verbosity < 3 => debugging level 0
 * 	verbosity 3 => debugging level 1
 *	verbosity 4 => debugging level 2
 *	et cetera...
 */
int	util_set_verbosity(int verbosity);
int	util_get_verbosity(void);
BOOL	util_be_silent(void);
BOOL	util_be_verbose(void);
int	util_get_debug_level(void);
int	debug_level(void);	/* these are special-case shortcuts that */
BOOL	debugging();		/* don't conform to the naming convention */


/*
 * Input/Output
 *
 * util_puts_err() and util_printf_err() should be used for messages
 * that absolutely must be seen by the user (this should cause a popup
 * to appear, when ab is running).
 *
 * util_puts() and util_printf() should be used for informative messages,
 * and may or may not actually be presented to the user.
 *
 * util_dputs() and util_dprintf() should be used for debugging messages.
 * Their first parameter is the debugging level at which the message should
 * be printed. (verbosity 3 = debug level 1). If debugging is disabled
 * (e.g., verbosity < 3), these functions never generate output.
 * 
 * To redirect the error output, use util_set_err_output_handler. ALL error 
 * output will then be sent to that error handler. A value of NULL sends 
 * error messages to stderr (which is the startup default). This is normally 
 * used to add an error handler that will pop up an error dialog when
 * running in a windowed application.
 *
 * util_set_output_handler() works similarly to the error handler.
 */
int	util_set_err_output_handler(UtilOutputHandler);
int	util_set_output_handler(UtilOutputHandler);
int	util_puts(STRING msg);
int	util_puts_err(STRING msg);
int	util_printf(STRING fmt, ...);
int	util_printf_err(STRING fmt, ...);
void	util_set_help_data(STRING help, STRING vol, STRING locID);
int	util_get_help_data(STRING *help, STRING *vol, STRING *locID);

	/* print output if debugging level >= specified */
int	util_dputs(int debug_level, STRING msg);
int	util_dprintf(int debug_level, STRING fmt, ...);

/*
 * files
 */
#define	util_fopen fopen		/* for consistency */
int	util_unbuffer_file(FILE *fp);	/* for debugging - removes all */
					/* buffer from the stream */
BOOL 	util_file_is_regular_file(STRING filename);
BOOL	util_file_is_directory(STRING filename);
BOOL	util_directory_exists(STRING dirName);
BOOL	util_file_exists(STRING fileName);
long	util_file_size(STRING fileName); /* error if file don't exist */
BOOL	util_paths_are_same_file(STRING path1, STRING path2);
int	util_fdtruncate(int fildes, off_t length);      /* truncate open file*/
int     util_ftruncate(FILE *file, off_t length, const char *accessType);
int	util_flock(
			FILE    *file,
			BOOL    wait, 
			int     lockType, 
			off_t   offset, 
			off_t   length
	);
int	util_funlock(
			FILE    *file,
			off_t   offset, 
			off_t   length
	);
FILE	*util_fopen_locked(const char *filename, const char *type);

BOOL	util_file_name_has_extension(STRING file_name, STRING ext);
BOOL	util_file_name_has_ab_extension(STRING file_name);
BOOL	util_file_name_is_bil_encapsulated(STRING file_name);
BOOL	util_file_name_is_bil_module(STRING file_name);
BOOL	util_file_name_is_bil_proj(STRING file_name);
FILE* 	util_create_tmp_file(char *data);

int	util_cvt_path_to_relative(		/* NULL from = cwd */
			STRING	path,
			STRING	from,
			char	*buf,
			int	buf_size);

/* 
 * strings
 *
 * THE UTIL STRING FUNCTIONS *ALWAYS* NULL-TERMINATE ANY RETURNED STRINGS!
 *
 * Note: len = length of string withOUT terminating 0
 *       size = size of string including terminating 0 ( = len+1 )
 */
BOOL	util_streq(STRING s1, STRING s2);		/* True if strings = */
BOOL	util_strcmp(STRING s1, STRING s2);	/* allows NULL strings */
STRING	util_strsafe(STRING s);		/*returns "(nil)" for NULL strs*/
BOOL	util_strempty(STRING s);		/* looks for NULL AND "" */
int	util_strncpy(STRING to, STRING from, int to_size);
int	util_strcvt_to_lower(STRING to, STRING from, int to_size);
int	util_strcasestr(STRING str, STRING substr);
int	util_strncasecmp(STRING s1, STRING s2, int max_chars);
STRING 	util_strip_white_space(STRING string);


STRING		util_get_file_name_from_path(
			STRING	path,
			STRING	fileNameBuf,
			int	fileNameBufSize
		);

STRING		util_get_dir_name_from_path(
			STRING	path,
			STRING	dirNameBuf,
			int	dirNameBufSize
		);

int		util_mkdir_hier(
			STRING path
		);

BOOL 		util_path_is_absolute(
			STRING      dir 
		);

int 		util_derive_name_from_path(
		    char    *fullpath,
		    char    *objname
		);

int		util_check_name(
		    STRING	name,
		    STRING	new_name
		);

/*
 * Process control
 */
pid_t	util_vfork(void);		/* lightweight fork() */


/* 
 * conversions
 */
STRING	util_cvt_bool_to_string(BOOL bool_value, STRING buf, int buf_size);



/*************************************************************************
**									**
**		Inline implementation					**
**									**
*************************************************************************/

/* check 1st chars before calling strcmp - avoids a lot of calls */
#define util_streq(s1,s2) \
   (   ((s1) == (s2)) \
    || (((s1) != NULL) && ((s2) != NULL) && (((s1)[0] == (s2)[0]) && (strcmp(s1,s2) == 0))) \
   )

/*
 * allows compare of null strings (NULL < "")
 */
#define util_strcmp(s1,s2) 		\
    (((s1) == (s2))? 			\
        0 				\
    :					\
	((s1 == NULL)?			\
	    (-1)			\
        :				\
	    ((s2) == NULL?		\
		(1)			\
	    :				\
		strcmp((s1),(s2))	\
	    )				\
        )				\
    )

#define util_strlen(s)	((s) == NULL? 0:strlen(s))

#define util_strsafe(s)	((s) == NULL? Util_null_string:(s))

#define util_strempty(s) (s == NULL || (strcmp(s, "") == 0))

/*
 * Verbosity
 */
#define util_get_verbosity()	(utilP_verbosityPriv3602759317)
#define util_be_silent()	(util_get_verbosity() < 1)
#define util_be_verbose()	(util_get_verbosity() > 1)
#define util_get_debug_level()	(util_max(0, util_get_verbosity() - 2))
#define debug_level()		((int)(util_get_debug_level()))
#define debugging()		((BOOL)(debug_level() > 0))

/*
 * We key on DEBUG here, so that if DEBUG is turned off, these macros
 * evaluate to constants. That should allow the compiler to remove
 * sections of code that are executed conditionally based on these
 * macros.
 */
#ifndef DEBUG
#undef  util_get_debug_level
#define util_get_debug_level()	(0)
#undef  debug_level
#define debug_level()		(0)
#undef  debugging
#define debugging()		(FALSE)
#endif /* DEBUG */

#define util_fclose(fp) ((fp) == NULL? 0:(util_funlock(fp, 0, 0), fclose(fp), (fp)= NULL, 0))

/*
 * These functions are obsolete.  Here for backwards compatibility
 */
#define util_error(s) util_puts_err(s)

#endif /* _AB_ABUTIL_H_ */

