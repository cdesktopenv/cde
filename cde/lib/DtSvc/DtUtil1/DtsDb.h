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
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 *+SNOTICE
 *
 *	$XConsortium: DtsDb.h /main/5 1996/08/28 14:32:17 rswiston $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
#ifndef DT_DTS_DB_H
#define DT_DTS_DB_H

#include <X11/Xresource.h>
#include <Dt/DbReader.h>

typedef int	OtBoolean;

/* typedefs for casting comparison functions if needed */
typedef	int	(*_DtDtsDbFieldCompare)(DtDtsDbField **fld1, DtDtsDbField **fld2);

/* entry of a list of attribute/pairs */
typedef	struct
{
	XrmQuark		recordName;
	_DtDtsDbFieldCompare	compare;
	long			pathId;
	int			seq;
	int			fieldCount;
	DtDtsDbField		**fieldList;
} DtDtsDbRecord;

/* typedefs for casting record comparison functions if needed */
typedef	int	(*_DtDtsDbRecordCompare)(DtDtsDbRecord **rec1, DtDtsDbRecord **rec2);

/* a "database" of a collection of entrys (i.e. OBJECT-TYPE, ACTION, FILE-TYPE 
	This is a private Structure to the DtDtsDb component.
*/
typedef	struct
{
	char			*databaseName;
	_DtDtsDbRecordCompare	compare;
	int			recordCount;
	DtDtsDbRecord		**recordList;
        unsigned long           ActionSequenceNumber;
} DtDtsDbDatabase;

/* for the mmaped database this the use_in_memory_db variable is used
   to call the old API while the database is being built and is set to
   false when the mmaped versions are being accessed.
*/

extern	int	use_in_memory_db;

/*
 *  adds a new database to the list of databases -- returns a pointer to the
 *  new database.  If a database of the given name already exists it returns
 *  a pointer to that database.
 */
extern DtDtsDbDatabase		*_DtDtsDbAddDatabase( char *dbname );

/* returns the handle for the database where name is the Database name */
extern	DtDtsDbDatabase		*_DtDtsDbGet(char *name);
extern	char			**_DtDtsDbListDb(void);

/* Record Sort function:
 * sorts the specified database, usually obtained from _DtDtsDbGet(), in the
 * order specified by the comparison function. If (*compare) == 0 then
 * _DtDtsDbCompareRecordNames() is used as the (*compare) function.
 */
extern	void		_DtDtsDbRecordSort(DtDtsDbDatabase *database, 
					_DtDtsDbRecordCompare compare);
/* Field Sort function:
 * sorts the specified Record in the order specified by the comparison function
 * If (*compare) == 0 then _DtDtsDbCompareFieldNames() is used as the
 * (*compare) function.
 */
extern	void		_DtDtsDbFieldSort(DtDtsDbRecord *record,
					_DtDtsDbFieldCompare compare);

/* Name Comparison functions:
 * These routines can be passed in to the corresponding sort function to
 * sort by name.
 *
 */
extern int _DtDtsDbCompareRecordNames(DtDtsDbRecord **entry1, DtDtsDbRecord **entry2);
extern int _DtDtsDbCompareFieldNames(DtDtsDbField **entry1, DtDtsDbField **entry2);

/* retrieves the Record that matches the specified entry from the record */
extern	DtDtsDbField	*_DtDtsDbGetField(DtDtsDbRecord *record,
					char *value);

/* retrieves the entry of the specified entry from the specified database */
extern	DtDtsDbRecord	*_DtDtsDbGetRecord(DtDtsDbDatabase *database,
					DtDtsDbRecord *value);

/* Get By Name functions:
 * retrieves the entry of the specified name from the specified database
 * ** IF ** the _DtDtsDb*Sort routine has been called with the corresponding
 * _DtDtsDbCompare*Name comparison function. Otherwise use the standard
 * _DtDtsDbGet* functions. 
*/
extern	char *_DtDtsDbGetFieldByName(DtDtsDbRecord *record, char *name);
extern	DtDtsDbRecord *_DtDtsDbGetRecordByName(DtDtsDbDatabase *database, char *name);

extern	DtDtsDbRecord *_DtDtsDbAddRecord(DtDtsDbDatabase *db);

extern	DtDtsDbField *_DtDtsDbAddField(DtDtsDbRecord *rec);

extern	int	_DtDtsDbDeleteDb(DtDtsDbDatabase *db);
extern	int	_DtDtsDbDeleteRecord(DtDtsDbRecord *rec, DtDtsDbDatabase *db);
extern	int	_DtDtsDbDeleteRecords(DtDtsDbDatabase *db);
extern	int	_DtDtsDbDeleteField(DtDtsDbField *fld, DtDtsDbRecord *rec);
extern	int	_DtDtsDbDeleteFields(DtDtsDbRecord *rec);

#endif

