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
/* $XConsortium: DtsMM.h /main/8 1996/08/28 14:27:26 rswiston $ */
/*
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
#ifndef DT_DTS_MM_H
#define DT_DTS_MM_H

#include <stdio.h>
#include <Dt/DtShmDb.h>
#include <X11/Intrinsic.h>
#include "Dt/DbReader.h"

#define	DTDTSDB_TMPDATABASENAME	"%s/.dt/.tmp_dt_db_cache.%s\0"
#define	DTDTSDB_DATABASENAME	"%s/.dt/.dt_db_cache.%s\0"
#define	_DTDTSMMTEMPFILE	"dtdbcache_"

/*
 * NOTE: _DTDTSMMTEMPDIR affects the location of the dtdbcache
 * file, and therefore affects the Xsession.src, Xreset.src, and
 * Xstartup.src scripts in dtlogin/config.
 */
#ifdef __osf__
# define _DTDTSMMTEMPDIR	"/var/tmp"
#else
# define _DTDTSMMTEMPDIR	"/tmp"
#endif

typedef	int	DtDtsMMSeqNo;		/* the order it occures in db */
typedef	int	DtDtsMMFieldCount;	/* number of fields in record */
typedef	int	DtDtsMMRecordCount;	/* number of records in field */
typedef	int	DtDtsMMDataBaseCount;	/* how many databases */
typedef	int	DtDtsMMFieldStart;	/* index in table where field list starts */
typedef	int	DtDtsMMRecordStart;	/* index in table where record list starts */
typedef	int	DtDtsMMDataBaseStart;	/* index in table where database list starts */
typedef	int	DtDtsMMIndexOffset;
typedef	int	DtDtsMMNameIndex;
typedef	int	DtDtsMMPathHash;

typedef	struct
{
	DtDtsMMPathHash		pathhash;	/* hash of dir. we visit */
	DtDtsMMDataBaseCount	num_db;		/* number of databases */
	DtDtsMMDataBaseStart	db_offset;	/* index to databases */
	DtDtsMMNameIndex	name_list_offset;	/* index to name list */
	DtDtsMMNameIndex	no_name_offset;		/* index to nonunique names */
	DtDtsMMNameIndex	buffer_start_index;	/* index to list of buffers */
	DtDtsMMIndexOffset	str_tbl_offset;		/* index to table of strings */
	DtDtsMMIndexOffset	files_count;		/* number of loaded files */
	DtDtsMMIndexOffset	files_offset;		/* index to list of loaded files */
	DtDtsMMIndexOffset	mtimes_offset;	/* index to modified times of files */
} DtDtsMMHeader;

/* one set of attribute/pair */
typedef	struct
{
	DtShmBoson		fieldName;	/* name of attribute */
	DtShmBoson		fieldValue;	/* value of attribute */
} DtDtsMMField;

/* typedefs for casting comparison functions if needed */
typedef	int	(*_DtDtsMMFieldCompare)(DtDtsMMField *fld1, DtDtsMMField *fld2);

/* entry of a list of attribute/pairs */
typedef	struct
{
	DtShmBoson		recordName;	/* name of this entry */
	DtShmBoson		pathId;		/* file entry is located in */
	DtDtsMMSeqNo		seq;		/* sequence this got loaded */
	DtDtsMMFieldCount	fieldCount;	/* number of fields in record */
	DtDtsMMFieldStart	fieldList;	/* index to field table */
} DtDtsMMRecord;

/* typedefs for casting record comparison functions if needed */
typedef	int	(*_DtDtsMMRecordCompare)(DtDtsMMRecord *rec1, DtDtsMMRecord *rec2);

/* a "database" of a collection of entrys (i.e. OBJECT-TYPE, ACTION, FILE-TYPE 
	This is a private Structure to the DtDtsMM component.
*/
typedef	struct
{
	DtShmBoson		databaseName;	/* name of database */
	DtDtsMMIndexOffset	nameIndex;	/* index for DataCriteria quick find */
	DtDtsMMRecordCount	recordCount;	/* number of records */
	DtDtsMMRecordStart	recordList;	/* index to records table */
} DtDtsMMDatabase;


/* Db Internal pointers */
int *			_DtDtsMMGetDCNameIndex(int *size);
int *			_DtDtsMMGetBufferIndex(int *size);
int *			_DtDtsMMGetNoNameIndex(int *size);
void *			_DtDtsMMGetPtr(int index);
DtShmInttab		_DtDtsMMGetFileList(void);
int			_DtDtsMMGetPtrSize(int index);
int			_DtDtsMMInit(int);
void			_DtDtsMMPrint(FILE *org_fd);
int			_DtDtsMMCreateDb(DtDirPaths *dirs, const char *CacheFile, int override);
int			_DtDtsMMCreateFile(DtDirPaths *dirs, const char *CacheFile);
char *			_DtDtsMMCacheName(int);
int			_DtDtsMMapDB(const char *CacheFile);

const char *		_DtDtsMMBosonToString(DtShmBoson boson);
DtShmBoson		_DtDtsMMStringToBoson(const char *string);

extern	int	use_in_memory_db;


/* returns the handle for the database where name is the Database name */
extern	DtDtsMMDatabase		*_DtDtsMMGet(const char *name);
extern	char			**_DtDtsMMListDb(void);

/* FIXME: document */
extern int *_DtDtsMMGetDbName(DtDtsMMDatabase *db, DtShmBoson boson);


/* Name Comparison functions:
 * These routines can be passed in to the corresponding sort function to
 * sort by name.
 *
 */
extern int _DtDtsMMCompareRecordNames(DtDtsMMRecord *entry1, DtDtsMMRecord *entry2);
extern int _DtDtsMMCompareFieldNames(DtDtsMMField *entry1, DtDtsMMField *entry2);

/* retrieves the Record that matches the specified entry from the record */
extern	DtDtsMMField	*_DtDtsMMGetField(DtDtsMMRecord *record,
					const char *value);
extern const char *_DtDtsMMGetFieldByName(DtDtsMMRecord *rec, const char *name);

/* retrieves the entry of the specified entry from the specified database */
extern	DtDtsMMRecord	*_DtDtsMMGetRecord(DtDtsMMDatabase *database,
					DtDtsMMRecord *value);
extern	DtDtsMMRecord	*_DtDtsMMGetRecordByName(DtDtsMMDatabase *database,
					const char *value);

/* Get By Name functions:
 * retrieves the entry of the specified name from the specified database
 * ** IF ** the _DtDtsMM*Sort routine has been called with the corresponding
 * _DtDtsMMCompare*Name comparison function. Otherwise use the standard
 * _DtDtsMMGet* functions. 
*/


char *	_DtDtsMMExpandValue(const char *value);
void	_DtDtsMMSafeFree(char *value);
int	_DtDtsMMIsMemory(const char *value);

extern	DtShmBoson	_DtDtsMMNameStringToBoson(const char *string);


#endif /* DT_DTS_MM_H */
