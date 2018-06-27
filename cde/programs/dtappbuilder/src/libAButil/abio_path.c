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
 *	$XConsortium: abio_path.c /main/4 1995/11/06 18:50:21 rswiston $
 *
 * @(#)abio_path.c	3.27 02 Apr 1995	cde_app_builder/src/libAButil
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
 * abio_path.c
 *
 * Functions to manipulate file paths.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>

#define	GIL_SUFFIX		".G"
#define GIL_PROJ_SUFFIX		".P"
#define	BIL_MODULE_SUFFIX	"bil"
#define BIL_PROJ_SUFFIX		"bip"
#define BIL_ENCAP_SUFFIX	"bix"

static char	Buf[MAXPATHLEN]; 	/* Work buffer */

static void	expand_path(
		    char *nm, 
		    char *buf
		);

/*
 * Expands the path and file name of a file.
 * exp_dir and exp_file store the expanded path and file name returned
 * from this function.
 * Note: exp_dir and exp_file have to be allocated memory.
 */

int
abio_expand_file(
    char    *file,
    char    *exp_file 
)
{
        if (!file || !*file)
                return -1;
        /*                           
         * Expand the file name and store to it.
         */
        snprintf(Buf, sizeof(Buf), "%s", file);
        if (abio_expand_bil_module_path(Buf) == -1)
	    return -1;

        strcpy(exp_file, Buf);
        return 0;
}

/*
 * Expand a path in place.  Returns OK if successful, otherwise sets errno
 * and returns ERROR.
 */
int
abio_expand_path(char *path)
{
	char		buf[MAXPATHLEN];

	expand_path(path, buf);
	strcpy(path, buf);
	return OK;
}

/*
 * Expand a path to a GIL file in place.  Returns OK if successful, 
 * otherwise sets errno and returns error.
 */
int
abio_gil_expand_path(char *path)
{
	if (abio_expand_path(path) != OK)
		return ERROR;

	if (abio_is_gil_path(path))
		return OK;

	if (strlen(path) + strlen(GIL_SUFFIX) >= (size_t)MAXPATHLEN) {
		errno = ENAMETOOLONG;
		return ERROR;
	}

	strcat(path, GIL_SUFFIX);
	return OK;
}

int
abio_expand_bil_module_path(
    char *path
)
{
    if (util_file_name_is_bil_module(path))
	return 0;

    if (strlen(path) + strlen(BIL_MODULE_SUFFIX) >= (size_t)MAXPATHLEN) 
    {
	errno = ENAMETOOLONG;		
	return -1;
    }
    strcat(path, ".");
    strcat(path, BIL_MODULE_SUFFIX);
    return 0;
}

/*
 * Expand a path to a project file in place.
 */
int
abio_gil_expand_proj_path(char *path)
{
	if (abio_gil_expand_path(path) != 0)
		return ERROR;

	if (abio_is_gil_proj_path(path))
		return OK;

	if (strlen(path) + strlen(GIL_PROJ_SUFFIX) >= (size_t)MAXPATHLEN) {
		errno = ENAMETOOLONG;
		return ERROR;
	}

	strcat(path, GIL_PROJ_SUFFIX);
	return 0;
}

int
abio_expand_bil_proj_path(
    char *path
)
{
    if (util_file_name_is_bil_proj(path))
    {
	return 0;
    }

    if (strlen(path) + strlen(BIL_PROJ_SUFFIX) >= (size_t)MAXPATHLEN) 
    {
	errno = ENAMETOOLONG;
	return -1;
    }
    strcat(path, ".");
    strcat(path, BIL_PROJ_SUFFIX);
    return 0;
}

int
abio_expand_bil_encapsulated_path(STRING path)
{
    if (util_file_name_is_bil_encapsulated(path))
        return 0;

    if (strlen(path) + strlen(BIL_ENCAP_SUFFIX) >= (size_t)MAXPATHLEN)
    {
        errno = ENAMETOOLONG;
        return -1;
    }
    strcat(path, ".");
    strcat(path, BIL_ENCAP_SUFFIX);
    return 0;
}
/*
 * Return True if the given path ends with the gil suffix.
 */
int
#ifdef __STDC__
abio_is_gil_path(char *path)
#else
abio_is_gil_path(path)
	char	*path;
#endif
{
	return (strcmp(path + strlen(path) - strlen(GIL_SUFFIX),
		       GIL_SUFFIX) == 0);
}

BOOL
util_file_name_is_bil_module(STRING path)
{
    return util_file_name_has_extension(path, BIL_MODULE_SUFFIX);
}

/*
 * Return True if the given path ends with the project suffix.
 */
int
abio_is_gil_proj_path(STRING path)
{
	return (strcmp(path + strlen(path) - strlen(GIL_PROJ_SUFFIX),
		       GIL_PROJ_SUFFIX) == 0);
}

BOOL
util_file_name_is_bil_proj(STRING path)
{
    return util_file_name_has_extension(path, BIL_PROJ_SUFFIX);
}

BOOL
util_file_name_is_bil_encapsulated(
    char    *path
)
{
        return (strcmp(path + strlen(path) - strlen(BIL_ENCAP_SUFFIX),
                       BIL_ENCAP_SUFFIX) == 0);
}

/*
 * expand_path from OpenWindows V2 FCS XView libraries
 *
 * Handles:
 *	~/ => home dir
 *	~user/ => user's home dir
 *   If the environment variable a = "foo" and b = "bar" then:
 *	$a	=>	foo
 *	$a$b	=>	foobar
 *	$a.c	=>	foo.c
 *	xxx$a	=>	xxxfoo
 *	${a}!	=>	foo!
 *	\$a	=>	\$a
 *
 * Arguments:
 *	nm		input string
 *	pathname	buffer to output expanded path
 */
static void
expand_path(char *nm, char *buf)
{
	char  *s, *d;
	char            lnm[MAXPATHLEN];
	int             q;
	char  *trimchars = "\n \t";

	/* Strip off leading & trailing whitespace and cr */
	while (strchr(trimchars, *nm) != NULL)
		nm++;
	s = nm + (q = strlen(nm)) - 1;
	while (q-- && strchr(trimchars, *s) != NULL)
		*s = '\0';

	s = nm;
	d = lnm;
	q = nm[0] == '\\' && nm[1] == '~';

	/* Expand inline environment variables */
	while (*d++ = *s)
	{
		if (*s == '\\') {
			if (*(d - 1) = *++s)
			{
				s++;
				continue;
			} else
				break;
		}
		else if (*s++ == '$') {
			char  *start = d;
			char  braces = *s == '{';
			char  *value;
			while (*d++ = *s)
				if (braces ? *s == '}' : !isalnum(*s))
					break;
				else
					s++;
			*--d = 0;
			value = getenv(braces ? start + 1 : start);
			if (value) {
				for (d = start - 1; *d++ = *value++;);
				d--;
				if (braces && *s)
					s++;
			}
		}
	}

	/* Expand ~ and ~user */
	nm = lnm;
	s = "";
	if (nm[0] == '~' && !q) { /* prefix ~ */
		if (nm[1] == '/' || nm[1] == 0)
		{ /* ~/filename */
			if (s = getenv("HOME"))
			{
				if (*++nm)
					nm++;
			}
		}
		else
		{	/* ~user/filename */
			char  *nnm;
			struct passwd *pw;
			for (s = nm; *s && *s != '/'; s++);
			nnm = *s ? s + 1 : s;
			*s = 0;
			pw = (struct passwd *) getpwnam(nm + 1);
			if (pw == 0) {
				*s = '/';
				s = "";
			} else {
				nm = nnm;
				s = pw->pw_dir;
			}
		}
	}
	d = buf;
	if (*s) {
		while (*d++ = *s++);
		*(d - 1) = '/';
	}
	s = nm;
	while (*d++ = *s++);
}


/*
 * Return whether a file is a BIL file.  Returns non-0 version number
 * if TRUE, otherwise FALSE. Leaves file positioned at the beginning
 * of first comment (should be version number).
 *
 * pLinesRead is incremented by the # of lines read to find the 
 * bil_version line. This number includes the line with the version 
 * number on it.
 */
float
abio_get_bil_version(
    FILE 	*fp,
    int 	*pLinesRead
)
{
    STRING              ascii_version_num;
    STRING              tmp;
    int                 len = strlen(":bil-version");
    long                int_version_major;
    long                int_version_minor;
    long                first_char;
    float               version = 0.0;
    char		*tok = NULL;
    char		*s1 = NULL;
 
    rewind(fp);
    *pLinesRead = 0;
    while (fgets(Buf, sizeof(Buf), fp))
    {
        ++(*pLinesRead);
 
        /* Skip any leading white space.
         * Ignore lines until a BIL prefix is found.
         */
	s1 = strdup(Buf);
	tok = strtok(s1, " \t");
        if (tok && (strncmp(tok, ":bil-version", len) == 0))
        {
            /* Prefix matched.  Point to the version number 
	     * and convert it to a float.
             */
            first_char = ftell(fp);
            ascii_version_num = Buf + (len);
            int_version_major = strtol(ascii_version_num, &tmp, 10);
            int_version_minor = strtol(tmp, &tmp, 10);
 
            if (ascii_version_num != tmp)
            {
                version = (float) (int_version_major);
                version = version +
                    ((float) (((float) (int_version_minor)) /
                              10.0));
            }
            fseek(fp, first_char, 0);
            break;
        }
 
#ifdef BOGUS
        version_string = strcat("//      ", AB_VERSION_PREFIX);
        if (strncmp(Buf, version_string, (len + 3)) == 0)
        {
 
            /*
             * Prefix matched.  Point to the version number and convert it to
             * an integer.
             */
            first_char = ftell(fp);
            Buf[strlen(Buf) - 1] = '\0';
            ascii_version_num = Buf + (len + 3);
            int_version_num = strtol(ascii_version_num, &tmp, 10);

            if (ascii_version_num != tmp)
                version = (int) int_version_num;
            fseek(fp, first_char, 0);
            break;
        }
#endif                          /* BOGUS */

	/* Need to free space allocated by strdup() above */
        if (s1) { 
	    free(s1);  
	    s1 = NULL;
	}
    }
    if (s1) free(s1);
 
    return version;
}

FILE * 
util_create_tmp_file(
    char	*data
)
{
    FILE	*tmpFile = NULL;

    tmpFile = tmpfile();
    if (tmpFile != NULL)
    {
	fputs(data, tmpFile);
	rewind(tmpFile);
    }

    return (tmpFile);
}
