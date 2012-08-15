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
/* $TOG: mp_auth_functions.h /main/1 1999/08/30 10:55:06 mgreess $ */
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
 * This file was copied and altered from libICE/ICEutil.h
 * The 'Ice' prefix has been replaced by tt_ for functions
 * and by _tt_ for data types.
 */

#ifndef MP_AUTH_FUNCTIONS_H
#define MP_AUTH_FUNCTIONS_H

#include <stdio.h>

/*
 * Data structure for entry in ICE authority file
 */

typedef struct _tt_AuthFileEntry {
    char    	    *protocol_name;
    unsigned short  protocol_data_length;
    char   	    *protocol_data;
    char    	    *network_id;
    char    	    *auth_name;
    unsigned short  auth_data_length;
    char   	    *auth_data;
} _tt_AuthFileEntry;

typedef struct _tt_AuthFileEntryList {
    struct _tt_AuthFileEntryList	*next;
    _tt_AuthFileEntry			*entry;
} _tt_AuthFileEntryList;

/*
 * Authentication data maintained in memory.
 */

typedef struct {
    char    	    *protocol_name;
    char	    *network_id;
    char    	    *auth_name;
    unsigned short  auth_data_length;
    char   	    *auth_data;
} _tt_AuthDataEntry;

/*
 * Return values from tt_LockAuthFile
 */

#define _tt_AuthLockSuccess	0   /* lock succeeded */
#define _tt_AuthLockError	1   /* lock unexpectely failed, check errno */
#define _tt_AuthLockTimeout	2   /* lock failed, timeouts expired */


/*
 * Function Prototypes
 */

extern char *_tt_AuthFileName (void);

extern int _tt_LockAuthFile (
    char *		/* file_name */,
    int			/* retries */,
    int			/* timeout */,
    long		/* dead */
);

extern void _tt_UnlockAuthFile (
    char *		/* file_name */
);

extern _tt_AuthFileEntry *_tt_ReadAuthFileEntry (
    FILE *		/* auth_file */
);

extern void _tt_FreeAuthFileEntry (
    _tt_AuthFileEntry *	/* auth */
);

extern int _tt_WriteAuthFileEntry (
    FILE *		/* auth_file */,
    _tt_AuthFileEntry *	/* auth */
);

extern _tt_AuthFileEntry *_tt_GetAuthFileEntry (
    const char *		/* protocol_name */,
    const char *		/* network_id */,
    const char *		/* auth_name */
);

extern char *_tt_GenerateMagicCookie (
    int			/* len */
);


#endif /* MP_AUTH_FUNCTIONS_H */
