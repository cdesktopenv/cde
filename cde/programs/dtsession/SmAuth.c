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
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1995 Hewlett-Packard Company.
 * (c) Copyright 1995 International Business Machines Corp.
 * (c) Copyright 1995 Sun Microsystems, Inc.
 * (c) Copyright 1995 Novell, Inc.
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * $TOG: SmAuth.c /main/4 1997/03/14 14:11:50 barstow $
 */
/******************************************************************************

Copyright (c) 1993  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <X11/Intrinsic.h>
#include <X11/SM/SMlib.h>
#include <X11/ICE/ICEutil.h>

#include "SmAuth.h"

typedef struct _IceAuthFileEntryList
{
  IceAuthFileEntry *fileEntry;
  struct _IceAuthFileEntryList *next;
} IceAuthFileEntryList;

/*
 * Private data
 */
#define MAGIC_COOKIE_LEN 16
#define AUTH_RETRIES 10
#define AUTH_TIMEOUT 2
#define AUTH_DEADTIME 600L

/*
 * Private functions - forward declarations
 */

static int
writeIceauth (
	int			nEntries,
	IceAuthDataEntry	*entries,
	int			restore);

static int
addToEntryList (
	IceAuthFileEntryList	**entryListP,
	IceAuthFileEntry	*fileEntry);

static int
fileEntryInDataEntries (
	int			nEntries,
	IceAuthDataEntry	*entries,
	IceAuthFileEntry	*fileEntry);

static void
freeEntryList (
	IceAuthFileEntryList	*entryList);

/*
 * Private functions - implemenation.
 */
static void
freeEntryList (
	IceAuthFileEntryList	*entryList)
{
    IceAuthFileEntryList *nextEntryP;

    while (entryList != (IceAuthFileEntryList *)NULL)
    {
	nextEntryP = entryList->next;

	IceFreeAuthFileEntry(entryList->fileEntry);
	XtFree((char *)entryList);

	entryList = nextEntryP;
    }
}

static int
fileEntryInDataEntries (
	int			nEntries,
	IceAuthDataEntry	*entries,
	IceAuthFileEntry	*fileEntry)
{
    int i;

#define SAME_STR(field) \
    ((entries->field != (char *)NULL) &&\
     (fileEntry->field != (char *)NULL) &&\
     (strcmp(entries->field, fileEntry->field) == 0))

    for (i = 0; i < nEntries; i++, entries++)
    {
	if (SAME_STR(protocol_name) &&
	    SAME_STR(network_id) &&
	    SAME_STR(auth_name))
	    return 1;
    }

#undef SAME_STR

    return 0;
}

static int
addToEntryList (
	IceAuthFileEntryList	**entryListP,
	IceAuthFileEntry	*fileEntry)
{
    IceAuthFileEntryList *newItem;

    if ((newItem =
	 (IceAuthFileEntryList *)XtMalloc(sizeof(IceAuthFileEntryList)))
	== (IceAuthFileEntryList *)NULL)
	return 0;

    /* I assume it's ok to reverse the order; otherwise */
    /* we need to add the new item onto the end of the list. */
    newItem->fileEntry = fileEntry;
    newItem->next = *entryListP;
    *entryListP = newItem;

    return 1;
}

static int
writeIceauth (
	int			nEntries,
	IceAuthDataEntry	*entries,
	int			restore)
{
    FILE *fp = NULL;
    char *path;
    char *extraPath;
    int oldUmask;
    int i;
    IceAuthDataEntry *dataEntry;
    IceAuthFileEntry *fileEntry;
    IceAuthFileEntry newEntry;
    IceAuthFileEntryList *fileEntryList = (IceAuthFileEntryList *)NULL;
    IceAuthFileEntryList *fileEntryP;

    if ((path = IceAuthFileName()) == (char *)NULL)
	return 0;

    if (IceLockAuthFile(path, AUTH_RETRIES, AUTH_TIMEOUT, AUTH_DEADTIME)
	!= IceAuthLockSuccess) {
	/*
	 * Let's try another PATH, in case IceLockAuthFile's call to
	 * link() fails.  This workaround code was taken from 
	 * dtlogin/auth.c.
	 */
        IceUnlockAuthFile(path);
	extraPath = XtMalloc (MAXPATHLEN);
	(void) strcpy (extraPath, CDE_CONFIGURATION_TOP ".ICEauthority");
	if (IceLockAuthFile(extraPath, AUTH_RETRIES, AUTH_TIMEOUT, AUTH_DEADTIME)
		!= IceAuthLockSuccess) {
	    IceUnlockAuthFile (extraPath);
	    return 0;
	 }
	 path = extraPath;
    }

    /* If file exists, read entries into memory. */
    if (access(path, F_OK) == 0)
    {
	if ((fp = fopen(path, "rb")) == (FILE *)NULL)
	{
	    IceUnlockAuthFile(path);
	    return 0;
	}

	/* For each file entry: if matches something in entries, discard. */
	/* Otherwise, hold onto it - we'll be writing it back to file. */
	while ((fileEntry = IceReadAuthFileEntry(fp))
	       != (IceAuthFileEntry *)NULL)
	{
	    if (!fileEntryInDataEntries(nEntries, entries, fileEntry) &&
		!addToEntryList(&fileEntryList, fileEntry))
	    {
		freeEntryList(fileEntryList);
		IceUnlockAuthFile(path);
		fclose(fp);
		return 0;
	    }
	}

	fclose(fp);
    }

    /* Set umask to disallow non-owner access. */
    oldUmask = umask(0077);

    /* Write entries and fileEntryList to file. */
    if ((fp = fopen(path, "wb")) == (FILE *)NULL)
    {
	freeEntryList(fileEntryList);
	IceUnlockAuthFile(path);
	umask(oldUmask);
	return 0;
    }

    for (fileEntryP = fileEntryList;
	 fileEntryP != (IceAuthFileEntryList *)NULL;
	 fileEntryP = fileEntryP->next)
    {
	if (IceWriteAuthFileEntry(fp, fileEntryP->fileEntry) == 0)
	{
	    fclose(fp);
	    umask(oldUmask);
	    freeEntryList(fileEntryList);
	    IceUnlockAuthFile(path);
	    return 0;
	}
    }

    /* Done with fileEntryList - free it up. */
    freeEntryList(fileEntryList);

    if (!restore)
    {
	for (i = 0; i < nEntries; i++)
	{
	    dataEntry = &(entries[i]);
	    newEntry.protocol_name = dataEntry->protocol_name;
	    newEntry.protocol_data_length = 0;
	    newEntry.protocol_data = "";
	    newEntry.network_id = dataEntry->network_id;
	    newEntry.auth_name = dataEntry->auth_name;
	    newEntry.auth_data_length = dataEntry->auth_data_length;
	    newEntry.auth_data = dataEntry->auth_data;

	    if (IceWriteAuthFileEntry(fp, &newEntry) == 0)
	    {
		fclose(fp);
		umask(oldUmask);
		IceUnlockAuthFile(path);
		return 0;
	    }
	}
    }

    /* Success! */
    fclose(fp);
    umask(oldUmask);
    IceUnlockAuthFile(path);
    return 1;
}

/*
 * Host Based Authentication Callback.  This callback is invoked if
 * the connecting client can't offer any authentication methods that
 * we can accept.  We can accept/reject based on the hostname.
 */
Bool
HostBasedAuthProc (
	char			*hostname)

{
    /* 
     * For now, we don't support host based authentication 
     */
    return (0);	      
}


/*
 * Provide authentication data to clients that wish to connect
 */
Status
SetAuthentication (
	int			count,
	IceListenObj		*listenObjs,
	IceAuthDataEntry	**authDataEntries)
{
    int		i;
    int		nEntries = count * 2;

    if ((*authDataEntries = (IceAuthDataEntry *) XtMalloc (
	nEntries * sizeof (IceAuthDataEntry))) == NULL)
	return 0;

    for (i = 0; i < nEntries; i += 2)
    {
	(*authDataEntries)[i].network_id =
	    IceGetListenConnectionString (listenObjs[i/2]);
	(*authDataEntries)[i].protocol_name = "ICE";
	(*authDataEntries)[i].auth_name = "MIT-MAGIC-COOKIE-1";

	(*authDataEntries)[i].auth_data =
	    IceGenerateMagicCookie (MAGIC_COOKIE_LEN);
	(*authDataEntries)[i].auth_data_length = MAGIC_COOKIE_LEN;

	(*authDataEntries)[i+1].network_id =
	    IceGetListenConnectionString (listenObjs[i/2]);
	(*authDataEntries)[i+1].protocol_name = "XSMP";
	(*authDataEntries)[i+1].auth_name = "MIT-MAGIC-COOKIE-1";

	(*authDataEntries)[i+1].auth_data = 
	    IceGenerateMagicCookie (MAGIC_COOKIE_LEN);
	(*authDataEntries)[i+1].auth_data_length = MAGIC_COOKIE_LEN;

	IceSetHostBasedAuthProc (listenObjs[i/2], HostBasedAuthProc);
    }

    /* Merge new entries into auth file. */
    if (!writeIceauth(nEntries, *authDataEntries, 0))
	return 0;

    IceSetPaAuthData(nEntries, *authDataEntries);

    return 1;
}

/*
 * Free up authentication data.
 */
void
FreeAuthenticationData (
	int			count,
	IceAuthDataEntry 	*authDataEntries)
{
    int i;
    int nEntries = count * 2;

    /* Restore auth file to (approx) state before we ran.  We remove all */
    /* new entries... some of these entries may have existed in the auth */
    /* file before but were replaced when we came up. */
    writeIceauth(nEntries, authDataEntries, 1);

    /* Each transport has entries for ICE and XSMP */
    for (i = 0; i < nEntries; i++)
    {
	free (authDataEntries[i].network_id);
	free (authDataEntries[i].auth_data);
    }

    XtFree ((char *) authDataEntries);
}
