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
/* $TOG: mp_auth_functions.C /main/3 1999/10/14 18:43:11 mgreess $ */
/******************************************************************************


Copyright 1993, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

Author: Ralph Mor, X Consortium
******************************************************************************/
/*
 * This file was copied and altered from libICE/authutil.c
 * The 'Ice' prefix has been replaced by tt_ for functions
 * and by _tt_ for data types.
 */

#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <X11/Xos.h>
#include "mp/mp_auth_functions.h"

#ifdef X_NOT_STDC_ENV
extern int errno;
extern long time ();
extern char *getenv();
#define Time_t long
#else
#include <time.h>
#define Time_t time_t
#endif
#ifndef X_NOT_POSIX
#include <unistd.h>
#else
#ifndef WIN32
extern unsigned	sleep ();
#else
#define link rename
#endif
#endif

static int read_short(FILE *, unsigned short *);
static int read_string(FILE *, char **);
static int read_counted_string(FILE *, unsigned short *, char **);
static int write_short(FILE *, unsigned short);
static int write_string(FILE *, char *);
static int write_counted_string(FILE *, unsigned short, char *);



/*
 * The following routines are for manipulating the .TTauthority file
 */

char *
_tt_AuthFileName ()

{
    static char slashDotTTauthority[] = "/.TTauthority";
    char    	*name;
    static char	*buf;
    static int	bsize;
    int	    	size;
#ifdef WIN32
#ifndef PATH_MAX
#define PATH_MAX 512
#endif
    char    	dir[PATH_MAX];
#endif

    if (name = getenv ("TTAUTHORITY"))
	return (name);

    name = getenv ("HOME");

    if (!name)
    {
#ifdef WIN32
    register char *ptr1;
    register char *ptr2;
    int len1 = 0, len2 = 0;

    if ((ptr1 = getenv("HOMEDRIVE")) && (ptr2 = getenv("HOMEDIR"))) {
	len1 = strlen (ptr1);
	len2 = strlen (ptr2);
    } else if (ptr2 = getenv("USERNAME")) {
	len1 = strlen (ptr1 = "/users/");
	len2 = strlen (ptr2);
    }
    if ((len1 + len2 + 1) < PATH_MAX) {
	sprintf (dir, "%s%s", ptr1, (ptr2) ? ptr2 : "");
	name = dir;
    }
    if (!name)
#endif
	return (NULL);
    }

    size = strlen (name) + strlen (&slashDotTTauthority[1]) + 2;

    if (size > bsize)
    {
	if (buf)
	    free (buf);
	buf = (char*) malloc ((unsigned) size);
	if (!buf)
	    return (NULL);
	bsize = size;
    }

    strcpy (buf, name);
    strcat (buf, slashDotTTauthority + (name[1] == '\0' ? 1 : 0));

    return (buf);
}



int
_tt_LockAuthFile(char *file_name, int retries, int timeout, long dead)
{
    char	creat_name[1025], link_name[1025];
    struct stat	statb;
    Time_t	now;
    int		creat_fd = -1;

    if ((int) strlen (file_name) > 1022)
	return (_tt_AuthLockError);

    strcpy (creat_name, file_name);
    strcat (creat_name, "-c");
    strcpy (link_name, file_name);
    strcat (link_name, "-l");

    if (stat (creat_name, &statb) != -1)
    {
	now = time ((Time_t *) 0);

	/*
	 * NFS may cause ctime to be before now, special
	 * case a 0 deadtime to force lock removal
	 */

	if (dead == 0 || now - statb.st_ctime > dead)
	{
	    unlink (creat_name);
	    unlink (link_name);
	}
    }
    
    while (retries > 0)
    {
	if (creat_fd == -1)
	{
	    creat_fd = creat (creat_name, 0666);

	    if (creat_fd == -1)
	    {
		if (errno != EACCES)
		    return (_tt_AuthLockError);
	    }
	    else
		close (creat_fd);
	}

	if (creat_fd != -1)
	{
	    if (link (creat_name, link_name) != -1)
		return (_tt_AuthLockSuccess);

	    if (errno == ENOENT)
	    {
		creat_fd = -1;	/* force re-creat next time around */
		continue;
	    }

	    if (errno != EEXIST)
		return (_tt_AuthLockError);
	}

	sleep ((unsigned) timeout);
	--retries;
    }

    return (_tt_AuthLockTimeout);
}



void
_tt_UnlockAuthFile(char *file_name)
{
#ifndef WIN32
    char	creat_name[1025];
#endif
    char	link_name[1025];

    if ((int) strlen (file_name) > 1022)
	return;

#ifndef WIN32
    strcpy (creat_name, file_name);
    strcat (creat_name, "-c");
#endif
    strcpy (link_name, file_name);
    strcat (link_name, "-l");

#ifndef WIN32
    unlink (creat_name);
#endif
    unlink (link_name);
}



_tt_AuthFileEntry *
_tt_ReadAuthFileEntry(FILE *auth_file)
{
    _tt_AuthFileEntry   	local;
    _tt_AuthFileEntry   	*ret;

    local.protocol_name = NULL;
    local.protocol_data = NULL;
    local.network_id = NULL;
    local.auth_name = NULL;
    local.auth_data = NULL;

    if (!read_string (auth_file, &local.protocol_name))
	return (NULL);

    if (!read_counted_string (auth_file,
	&local.protocol_data_length, &local.protocol_data))
	goto bad;

    if (!read_string (auth_file, &local.network_id))
	goto bad;

    if (!read_string (auth_file, &local.auth_name))
	goto bad;

    if (!read_counted_string (auth_file,
	&local.auth_data_length, &local.auth_data))
	goto bad;

    if (!(ret = (_tt_AuthFileEntry *) malloc (sizeof (_tt_AuthFileEntry))))
	goto bad;

    *ret = local;

    return (ret);

 bad:

    if (local.protocol_name) free (local.protocol_name);
    if (local.protocol_data) free (local.protocol_data);
    if (local.network_id) free (local.network_id);
    if (local.auth_name) free (local.auth_name);
    if (local.auth_data) free (local.auth_data);

    return (NULL);
}



void
_tt_FreeAuthFileEntry(_tt_AuthFileEntry *auth)
{
    if (auth)
    {
	if (auth->protocol_name) free (auth->protocol_name);
	if (auth->protocol_data) free (auth->protocol_data);
	if (auth->network_id) free (auth->network_id);
	if (auth->auth_name) free (auth->auth_name);
	if (auth->auth_data) free (auth->auth_data);
	free ((char *) auth);
    }
}



int
_tt_WriteAuthFileEntry(FILE *auth_file, _tt_AuthFileEntry *auth)
{
    if (!write_string (auth_file, auth->protocol_name))
	return (0);

    if (!write_counted_string (auth_file,
	auth->protocol_data_length, auth->protocol_data))
	return (0);

    if (!write_string (auth_file, auth->network_id))
	return (0);

    if (!write_string (auth_file, auth->auth_name))
	return (0);

    if (!write_counted_string (auth_file,
	auth->auth_data_length, auth->auth_data))
	return (0);

    return (1);
}



_tt_AuthFileEntry *
_tt_GetAuthFileEntry(const char *protocol_name, const char *network_id, const char *auth_name)
{
    FILE    		*auth_file;
    char    		*filename;
    _tt_AuthFileEntry    *entry;

    if (!(filename = _tt_AuthFileName ()))
	return (NULL);

    if (access (filename, R_OK) != 0)		/* checks REAL id */
	return (NULL);

    if (!(auth_file = fopen (filename, "rb")))
	return (NULL);

    for (;;)
    {
	if (!(entry = _tt_ReadAuthFileEntry (auth_file)))
	    break;

	if (strcmp (protocol_name, entry->protocol_name) == 0 &&
	    strcmp (network_id, entry->network_id) == 0 &&
            strcmp (auth_name, entry->auth_name) == 0)
	{
	    break;
	}

	_tt_FreeAuthFileEntry (entry);
    }

    fclose (auth_file);

    return (entry);
}


/*
 * MIT-MAGIC-COOKIE-1 is a sample authentication method implemented by
 * the SI.  It is not part of standard ICElib.
 */


char *
_tt_GenerateMagicCookie(int len)
{
    char    *auth;
    long    ldata[2];
    int	    seed;
    int	    value;
    int	    i;
    
    if ((auth = (char *) malloc (len + 1)) == NULL)
	return (NULL);

#ifdef ITIMER_REAL
    {
	struct timeval  now;
	X_GETTIMEOFDAY (&now);
	ldata[0] = now.tv_sec;
	ldata[1] = now.tv_usec;
        seed = (int) ((ldata[0] << 11) ^ ldata[1]);
    }
#else
    {
	long    time ();

	ldata[0] = time ((long *) 0);
	ldata[1] = getpid ();
        seed = (int) (ldata[0]) + (ldata[1] << 16);
    }
#endif
    srand (seed);
    for (i = 0; i < len; i++)
    {
	value = rand ();
	auth[i] = (
		  ( (value & 0xff000000) >> 24) ^
		  ( (value & 0xff0000) >> 16) ^
		  ( (value & 0xff00) >> 8)  ^
		  ( (value & 0xff) )
		  ) & 0xff;
    }
    auth[len] = '\0';

    return (auth);
}


/*
 * local routines
 */

static int
read_short(FILE *file, unsigned short *shortp)
{
    unsigned char   file_short[2];

    if (fread ((char *) file_short, (int) sizeof (file_short), 1, file) != 1)
	return (0);

    *shortp = file_short[0] * 256 + file_short[1];
    return (1);
}


static int
read_string(FILE *file, char **stringp)
{
    unsigned short  len;
    char	    *data;

    if (!read_short (file, &len))
	return (0);

    if (len == 0)
    {
	data = 0;
    }
    else
    {
    	data = (char*) malloc ((unsigned) len + 1);

    	if (!data)
	    return (0);

    	if (fread (data, (int) sizeof (char), (int) len, file) != len)
	{
	    free (data);
	    return (0);
    	}

	data[len] = '\0';
    }

    *stringp = data;

    return (1);
}


static int
read_counted_string(FILE *file, unsigned short *countp, char **stringp)
{
    unsigned short  len;
    char	    *data;

    if (!read_short (file, &len))
	return (0);

    if (len == 0)
    {
	data = 0;
    }
    else
    {
    	data = (char*) malloc ((unsigned) len);

    	if (!data)
	    return (0);

    	if (fread (data, (int) sizeof (char), (int) len, file) != len)
	{
	    free (data);
	    return (0);
    	}
    }

    *stringp = data;
    *countp = len;

    return (1);
}


static int
write_short(FILE *file, unsigned short s)
{
    unsigned char   file_short[2];

    file_short[0] = (s & (unsigned) 0xff00) >> 8;
    file_short[1] = s & 0xff;

    if (fwrite ((char *) file_short, (int) sizeof (file_short), 1, file) != 1)
	return (0);

    return (1);
}


static int
write_string(FILE *file, char *string)
{
    unsigned short count = strlen (string);

    if (!write_short (file, count))
	return (0);

    if (fwrite (string, (int) sizeof (char), (int) count, file) != count)
	return (0);

    return (1);
}


static int
write_counted_string(FILE *file, unsigned short count, char *string)
{
    if (!write_short (file, count))
	return (0);

    if (fwrite (string, (int) sizeof (char), (int) count, file) != count)
	return (0);

    return (1);
}
