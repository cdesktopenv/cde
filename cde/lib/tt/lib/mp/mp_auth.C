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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: mp_auth.C /main/2 1999/08/30 14:13:40 mgreess $ 			 				
/*
 * @(#)mp_auth.C	1.84 96/01/10
 *
 * Tool Talk Message Passer (MP) - mp_auth.cc
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 *
 * Implementation of the _Tt_auth class.
 */
#include <unistd.h>
#include <sys/stat.h>
#include "mp/mp_auth.h"
#include "mp/mp_auth_functions.h"
#include "mp/mp_global.h"
#include "util/tt_port.h"

_Tt_auth::
_Tt_auth(_Tt_auth_level auth_level)
{
    _auth_level = auth_level;
    _auth_cookie = "";
}


_Tt_auth::
~_Tt_auth()
{
}

Tt_status _Tt_auth::
generate_auth_cookie()
{
    static const char		*funcname = "_Tt_auth::make_auth_cookie()";
    _tt_AuthFileEntry   *entry;
    int			 exists;
    char                *filename;
    int			 original_umask;
    int			 retval;
    Tt_status		 status;

    _entries_head = _entries_tail = NULL;
    status = TT_OK;
    _auth_cookie = _tt_GenerateMagicCookie(_TT_ICEAUTH_MAGIC_COOKIE_LEN);

    if (! (filename = _tt_AuthFileName())) {
	_tt_syslog(0, LOG_ERR, "%s:  Missing TTAUTHORITY file.\n", funcname);
        return TT_AUTHFILE_MISSING;
    }
    
    if (_tt_AuthLockSuccess !=
	(retval = _tt_LockAuthFile(filename,
    		    		  _TT_ICEAUTH_DEFAULT_RETRIES,
		    		  _TT_ICEAUTH_DEFAULT_TIMEOUT,
		    		  _TT_ICEAUTH_DEFAULT_DEADTIME))) {
	const char *reason = "unknown error";

	_tt_UnlockAuthFile(filename);
	if (retval == _tt_AuthLockTimeout) {
	    reason = "timeout";
	    status = TT_AUTHFILE_LOCK_TIMEOUT;
	}
	else {
	    reason = "lock error";
	    status = TT_AUTHFILE_LOCK;
	}
	_tt_syslog(0, LOG_ERR,
		   "%s:  %s in locking authority file \"%s\".\n",
		   funcname, reason, filename);
	return status;
    }

    original_umask = umask(077);

    exists = (0 == access(filename, F_OK));
    if (exists && (0 != access(filename, R_OK | W_OK)))
    {
	_tt_syslog(0, LOG_ERR,
		   "%s:  \"%s\" not writable, auth entry not added.\n",
		   funcname, filename);
        status = TT_AUTHFILE_ACCESS;
	goto cleanup;
    }

    if (exists && TT_OK != (status = read_auth_file(filename))) {
	_tt_syslog(0, LOG_ERR,
		   "%s:  could not read \"%s\".  New entry not written.\n",
		   funcname, filename);
        goto cleanup;
    }

    entry = (_tt_AuthFileEntry*) malloc(sizeof(_tt_AuthFileEntry));
    if (NULL == entry) {
	_tt_syslog(0, LOG_ERR,
		   "%s:  memory error.  New entry not written.\n",
                   funcname);
	status = TT_ERR_NOMEM;
        goto cleanup;
    }

    entry->protocol_name = strdup(_TT_ICEAUTH_PROTOCOL_NAME);
    entry->protocol_data_length = 0;
    entry->protocol_data = strdup("");
    entry->network_id = strdup((char*) _sessionid);
    entry->auth_name = strdup(_TT_ICEAUTH_AUTH_NAME);
    entry->auth_data_length = strlen((char*) _auth_cookie) + 1;
    entry->auth_data = strdup((char*) _auth_cookie);

    status = modify_auth_entry(entry, &_entries_head);

    status = write_auth_file(filename);
    if (TT_OK != status) {
	_tt_syslog(0, LOG_ERR,
		   "%s:  could not write \"%s\".  New entry not written.\n",
		   funcname, filename);
        goto cleanup;
    }

cleanup:
    (void) umask(original_umask);
    _tt_UnlockAuthFile(filename);
    return status;
}

Tt_status _Tt_auth::
read_auth_entries(FILE *fp, _tt_AuthFileEntryList **headp)
{
    static const char			*funcname = "_Tt_auth::read_auth_entries()";
    _tt_AuthFileEntry		*entry;
    _tt_AuthFileEntryList	*head;
    _tt_AuthFileEntryList	*el_new;
    _tt_AuthFileEntryList	*tail;
    int			 	 n;

    entry = NULL;
    head = tail = NULL;
    n = 0;

    while ((entry = _tt_ReadAuthFileEntry(fp)) != NULL) {

	el_new = (_tt_AuthFileEntryList*) malloc(sizeof(_tt_AuthFileEntryList));
	if (NULL == el_new) {
	    _tt_syslog(0, LOG_ERR,
		       "%s:  memory error.  New entry not written.\n",
                       funcname);
	    return TT_ERR_NOMEM;
	}

	el_new->next = NULL;
	el_new->entry = entry;
	if (tail) 			/* if not first time through append */
	  tail->next = el_new;
	else
	  head = el_new;		/* first time through, so assign */
	tail = el_new;
	n++;
    }

    *headp = head;
    return TT_OK;
}

Tt_status _Tt_auth::
read_auth_file(char *filename)
{
    static const char		*funcname = "Tt_auth::read_auth_file()";
    FILE		*authfp;
    Tt_status		 status = TT_OK;

    if (0 == (authfp = fopen (filename, "rb"))) {
	_tt_syslog(0, LOG_ERR,
                   "%s:  unable to read auth entries from file \"%s\"\n",
                   funcname, filename);
        return TT_AUTHFILE_ACCESS;
    }

    status = read_auth_entries(authfp, &_entries_head);
    if (TT_OK != status)
      _tt_syslog(0, LOG_ERR,
                 "%s:  unable to read auth entries from file \"%s\"\n",
                 funcname, filename);

    (void) fclose (authfp);
    return status;
}

Tt_status _Tt_auth::
modify_auth_entry(_tt_AuthFileEntry *entry, _tt_AuthFileEntryList **headp)
{
    static const char			*funcname = "Tt_auth::modify_auth_entry()";
    _tt_AuthFileEntryList	*list, *prev, *el_new;

    for (prev=NULL, list=*headp; list; list=list->next) {
        if ((0 == strcmp(list->entry->protocol_name, entry->protocol_name)) &&
            (list->entry->protocol_data_length==entry->protocol_data_length) &&
            (0 == strncmp(list->entry->protocol_data, entry->protocol_data,
			 entry->protocol_data_length)) &&
            (0 == strcmp(list->entry->network_id, entry->network_id)) &&
            (0 == strcmp(list->entry->auth_name, entry->auth_name)) ) {

	    _tt_FreeAuthFileEntry(list->entry);
	    list->entry = entry;
	    return TT_OK;
        }
	prev = list;
    }

    el_new = (_tt_AuthFileEntryList*) malloc(sizeof(_tt_AuthFileEntryList));
    if (NULL == el_new) {
	_tt_syslog(0, LOG_ERR,
		   "%s:  memory error.  New entry not written.\n",
                   funcname);
	return TT_ERR_NOMEM;
    }

    el_new->next = NULL;
    el_new->entry = entry;
    if (NULL == prev)
      *headp = el_new;
    else
      prev->next = el_new;

    return TT_OK;
}


Tt_status _Tt_auth::
write_auth_file(char *filename)
{
    static const char			*funcname = "Tt_auth::write_auth_file()";
    static const char			*suffix = "-n";
    FILE			*fp;
    _tt_AuthFileEntryList	*list;
    char			*tmpnam;

    tmpnam = (char*) malloc(strlen(filename) + strlen(suffix) + 1);
    if (NULL == tmpnam) {
	_tt_syslog(0, LOG_ERR,
		   "%s:  memory error.  New entry not written.\n",
                   funcname);
	return TT_ERR_NOMEM;
    }

    strcpy(tmpnam, filename);
    strcat(tmpnam, "-n");             /* for new */
    (void) unlink(tmpnam);
    fp = fopen (tmpnam, "wb");         /* umask is still set to 0077 */
    if (!fp) {
        _tt_syslog(0, LOG_ERR,
		 "%s:  unable to open tmp file \"%s\"\n",
                 funcname, tmpnam);
        return TT_AUTHFILE_ACCESS;
    }

    for (list=_entries_head; list; list=list->next)
      _tt_WriteAuthFileEntry (fp, list->entry);

    (void) fclose (fp);

    (void) unlink(filename);
    if (link (tmpnam, filename) == -1)
      _tt_syslog(0, LOG_ERR,
                 "%s:  unable to link authority file %s, use %s\n",
                 funcname, filename, tmpnam);
    else
      (void) unlink(tmpnam);

    return TT_OK;
}
Tt_status _Tt_auth::
retrieve_auth_cookie()
{
    char		*buff = NULL;
    _tt_AuthFileEntry	*entry = NULL;

    entry = _tt_GetAuthFileEntry(_TT_ICEAUTH_PROTOCOL_NAME,
				_sessionid,
				_TT_ICEAUTH_AUTH_NAME);
    if (NULL == entry)
      return TT_AUTHFILE_ENTRY_MISSING;

    buff = (char*) malloc(entry->auth_data_length + 1);
    if (NULL == buff) return TT_ERR_NOMEM;

    strncpy(buff, entry->auth_data, entry->auth_data_length);
    buff[entry->auth_data_length] = '\0';
    _auth_cookie = buff;

    free(buff);
    _tt_FreeAuthFileEntry(entry);
    return TT_OK;
}

Tt_status _Tt_auth::
set_auth_level(_Tt_auth_level auth_level)
{
    _auth_level = auth_level;
    return TT_OK;
}

Tt_status _Tt_auth::
set_sessionid(
	int		rpc_program,
	_Tt_auth_level	auth_level,
	_Tt_string	hostaddr,
	int		rpc_version)
{
    char	strid[BUFSIZ];
    const char	*format = "%d/%d/%s/%d";

    _rpc_program = rpc_program;
    _auth_level = auth_level;
    _hostaddr = hostaddr;
    _rpc_version = rpc_version;

    sprintf(strid,
	    format,
	    _rpc_program,
	    (int) _auth_level,
	    (char*) _hostaddr,
	    _rpc_version);
    _sessionid = strid;

    return TT_OK;
}
