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
/* $XConsortium: dberr.h /main/5 1996/09/23 21:00:53 cde-ibm $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: none
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/* $Log$
 * Revision 2.2  1995/10/24  21:37:15  miker
 * Add prolog.
 * */
/* dberr error/status codes */
static char *user_error[] = {
   "database not opened",
   "invalid set",
   "invalid record",
   "invalid database",
   "invalid field name",
   "invalid db_address",
   "no current record",
   "set has no current owner",
   "set has no current member",
   "key value required",
   "invalid lock value",
   "record is owner of non-empty set(s)",
   "record is member of set(s)",
   "member already owned",
   "field is a compound key",
   "record not connected to set",
   "field is not a valid key",
   "record not legal owner of set",
   "record not legal member of set",
   "error in d_setpages (database open or bad param)",
   "incompatible dictionary file",
   "illegal attempt to delete system record",
   "illegal attempt to lock locked set/record",
   "attempt to access unlocked set/record",
   "transaction id not supplied",
   "transaction already active",
   "transaction not active",
   "transaction cannot begin due to locked files",
   "cannot free locks within a transaction",
   "too many changed database pages",
   "cannot update database outside a transaction",
   "exclusive access required",
   "write locks not allowed on static files",
   "unspecified or duplicate user id",
   "database, path or file name too long",
   "invalid file number was passed to d_renfile",
   "field is not an optional key field",
   "field is not defined in current record type",
   "record/field has/in a compound key",
   "invalid record or set number",
   "record/set not timestamped",
   "bad DBUSERID (contains non-alphanumeric)",
   "NetBIOS has not been installed on machine",
   "wrong number of elements in DBDPATH",
   "wrong number of elements in DBFPATH",
   "no current record type",
   "invalid country table sort string",
   "database not closed"
};
static char *system_error[] = {
   "no more space on file",
   "system error",
   "page fault",
   "no working file in dio",
   "unable to allocate sufficient memory",
   "error opening file",
   "unable to get access to database taf file",
   "database taf/log file error",
   "inconsistent database locks",
   "file record limit exceeded",
   "key file inconsistency",
   "maximum concurrent user limit exceeded",
   "error seeking in database file",
   "invalid file specified",
   "error reading from a database/overflow file",
   "lock manager synchronization error",
   "debug check interrupt",
   "network communications error",
   "auto-recovery in process",
   "error writing to a database/overflow file",
   "no lock manager is installed",
   "DBUSERID is already being used",
   "the lock manager is busy"
};
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dberr.h */
