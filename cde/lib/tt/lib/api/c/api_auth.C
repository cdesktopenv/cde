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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: api_auth.C /main/1 1999/08/30 13:41:59 mgreess $ 			 				
/* @(#)api_mp.C	1.48 93/07/30
 *
 * api_mp.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file contains API functions related to the MP. For
 * each API function named tt_<name> there is a _tt_<name> function in
 * some file named api_mp_*.cc.
 */

#include "api/c/tt_c.h"
#include "api/c/api_auth.h"

char *
tt_AuthFileName()
{
    return _tt_AuthFileName();
}

int
tt_LockAuthFile(char *file_name, int retries, int timeout, long dead)
{
    return _tt_LockAuthFile(file_name, retries, timeout, dead);
}

void
tt_UnlockAuthFile(char *file_name)
{
    _tt_UnlockAuthFile(file_name);
}

Tt_AuthFileEntry
tt_ReadAuthFileEntry(FILE *auth_file)
{
    return _tt_ReadAuthFileEntry(auth_file);
}

void
tt_FreeAuthFileEntry(Tt_AuthFileEntry auth)
{
    _tt_FreeAuthFileEntry(auth);
}

int
tt_WriteAuthFileEntry(FILE *auth_file, Tt_AuthFileEntry auth)
{
    return _tt_WriteAuthFileEntry(auth_file, auth);
}

Tt_AuthFileEntry
tt_GetAuthFileEntry(char *protocol_name, char *network_id, char *auth_name)
{
    return _tt_GetAuthFileEntry(protocol_name, network_id, auth_name);
}

char *
tt_GenerateMagicCookie(int len)
{
    return _tt_GenerateMagicCookie(len);
}
