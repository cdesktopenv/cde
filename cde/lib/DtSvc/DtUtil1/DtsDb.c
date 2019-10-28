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
 *	$TOG: DtsDb.c /main/10 1998/10/23 13:48:04 mgreess $
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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef	SUN_DB
#include <sys/mman.h>
#endif
#include <string.h>
#include <Dt/DbReader.h>
#include "Dt/DtsDb.h"
#include <Dt/UserMsg.h>
#include "DtSvcLock.h"

extern int _MMWriteDb(DtDirPaths *dirs, int num_db, DtDtsDbDatabase **db_list,
                      const char *CacheFile);



#define PADMEM 10

typedef	int	(*genfunc)(const void *, const void *);

static	DtDtsDbDatabase	**db_list;
static	int	num_db = 0;

void
_DtDtsDbPrintFields(DtDtsDbRecord  *rec_ptr, FILE *fd)
{
	int		fld;
	DtDtsDbField	*fld_ptr;

	for(fld = 0; fld < rec_ptr->fieldCount; fld++)
	{
		fld_ptr = rec_ptr->fieldList[fld];
		fprintf(fd, "\t\t[%d]\t%s\t%s\n", fld,
			XrmQuarkToString(fld_ptr->fieldName),
			fld_ptr->fieldValue?
				fld_ptr->fieldValue:"(NULL)");
	}
}

void
_DtDtsDbPrintRecords(DtDtsDbDatabase  *db_ptr, FILE *fd)
{
	int		rec;
	DtDtsDbRecord	*rec_ptr;

	_DtSvcProcessLock();
	fprintf(fd, "%d Records\n", db_ptr->recordCount);
	for(rec = 0; rec < db_ptr->recordCount; rec++)
	{
		rec_ptr = db_ptr->recordList[rec];
		fprintf(fd, "\tRec[%d] name = %s\n\t%d Fields\n", rec,
			XrmQuarkToString(rec_ptr->recordName),
			rec_ptr->fieldCount);
		_DtDtsDbPrintFields(rec_ptr, fd);
	}
	_DtSvcProcessUnlock();
}

void
_DtDtsDbPrint(FILE *org_fd)
{
	int		db;
	int		rec;
	DtDtsDbDatabase	*db_ptr;
	DtDtsDbRecord	*rec_ptr;
	FILE		*fd = org_fd;

	_DtSvcProcessLock();
	for(db = 0; db < num_db; db++)
	{
		if(!db_list[db])
		{
			continue;
		}
		db_ptr = db_list[db];
		if(fd == 0)
		{
			chdir("/tmp");
			if((fd = fopen(db_ptr->databaseName, "w")) == NULL)
			{
			    _DtSimpleError(
					DtProgName, DtError, NULL,
					db_ptr->databaseName, NULL);
			    continue;
			}
		}
		fprintf(fd, "DB[%d] ", db);
		fprintf(fd, "name = %s\n", db_ptr->databaseName);
		_DtDtsDbPrintRecords(db_ptr, fd);
		if(org_fd == 0)
		{
			fclose(fd);
			fd = 0;
		}
	}
	_DtSvcProcessUnlock();
}

int
_DtDtsDbCompareRecordNames(DtDtsDbRecord **a, DtDtsDbRecord **b)
{
	return ((*a)->recordName - (*b)->recordName);
}

int
_DtDtsDbCompareFieldNames(DtDtsDbField **a, DtDtsDbField **b)
{
	return ((*a)->fieldName - (*b)->fieldName);
}

#include <Dt/Dts.h>

DtDtsDbDatabase **
_DtDtsDbInit(void)
{
	DtDtsDbDatabase	**db;

	_DtSvcProcessLock();
	num_db = 0;
	db = db_list = (DtDtsDbDatabase **)calloc(num_db+3, sizeof(DtDtsDbDatabase *));
	db_list[0] = (DtDtsDbDatabase *)calloc(1, sizeof(DtDtsDbDatabase));
	db_list[0]->databaseName = (char *)strdup(DtDTS_DC_NAME);
	db_list[0]->ActionSequenceNumber = 0;
	num_db++;

	db_list[1] = (DtDtsDbDatabase *)calloc(1, sizeof(DtDtsDbDatabase));
	db_list[1]->databaseName = (char *)strdup(DtDTS_DA_NAME);
	db_list[1]->ActionSequenceNumber = 0;
	num_db++;

	_DtSvcProcessUnlock();
	return(db);
}

char **
_DtsDbListDb(void)
{
	int	i;
	char	**list = 0;

	_DtSvcProcessLock();
	for ( i = 0; db_list[i]; i++ );

	if(i > 0)
	{
		list = (char **)calloc(i+1, sizeof(char *));
		for ( i = 0; db_list[i]; i++ )
		{
			list[i] = (char *)strdup(db_list[i]->databaseName);
		}
	}
	_DtSvcProcessUnlock();
	return(list);
}

DtDtsDbDatabase *
_DtDtsDbAddDatabase( char *db_name )
{
	int i = 0;
	DtDtsDbDatabase	**new_db_list;
	DtDtsDbDatabase *ret_db;

	_DtSvcProcessLock();
	if ( !db_list )
	{
		_DtDtsDbInit();
	}
	for ( i = 0; db_list[i]; i++ )
	{
		if ( !strcmp(db_list[i]->databaseName,db_name) )
		{
			/*
			 * A database with the given name already exists.
			 * return a pointer to the existing database.
			 */
			ret_db = db_list[i];
		        _DtSvcProcessUnlock();

			return ret_db;
		}
	}	
	/*
	 * We now have a count of the existing databases.
	 * allocate enough space for the existing databases + the new one 
	 * + a NULL pointer to terminate the vector.
	 */

	new_db_list = (DtDtsDbDatabase **)calloc(i+2,sizeof(DtDtsDbDatabase *));

	memmove(new_db_list,db_list,sizeof(DtDtsDbDatabase *) * i );
	new_db_list[i] = (DtDtsDbDatabase *)calloc(1, sizeof(DtDtsDbDatabase));
	new_db_list[i]->databaseName = strdup(db_name);
	new_db_list[i]->ActionSequenceNumber = 0;
	free(db_list);
	db_list = new_db_list;
	num_db++;

	ret_db = db_list[i];
	_DtSvcProcessUnlock();

	return ret_db;
}

int
_DtDtsDbDeleteDb(DtDtsDbDatabase *db)
{
	int	i;
	int	flag = 0;

	_DtSvcProcessLock();
	_DtDtsDbDeleteRecords(db);
	free(db->databaseName);
	free(db);

	for(i = 0; db_list[i]; i++)
	{
		if(db_list[i] == db)
		{
			flag = 1;
			db_list[i] = 0;
		}
		if(flag)
		{
			db_list[i] = db_list[i+1];
		}
	}
	if(db_list[0] == 0)
	{
		free(db_list);
		db_list = 0;
	}
	_DtSvcProcessUnlock();
	return(0);
}


DtDtsDbDatabase *
_DtDtsDbGet(char *name)
{
	DtDtsDbDatabase *ret_db;
	int		i;

	_DtSvcProcessLock();
	if(!db_list)
	{
		_DtDtsDbInit();
	}
	for(i = 0; db_list && db_list[i] && db_list[i]->databaseName; i++)
	{
		if(strcmp(db_list[i]->databaseName, name) == 0)
		{
			ret_db = db_list[i];
		        _DtSvcProcessUnlock();

			return(ret_db);
		}
	}
	_DtSvcProcessUnlock();
	return(NULL);
}

void
_DtDtsDbFieldSort(DtDtsDbRecord *rec, _DtDtsDbFieldCompare compare)
{
	if(compare == NULL)
	{
		compare = _DtDtsDbCompareFieldNames;
	}
	qsort(rec->fieldList,
		rec->fieldCount,
		sizeof(DtDtsDbField *),
		(genfunc)compare);
	rec->compare = compare;
}

void
_DtDtsDbRecordSort(DtDtsDbDatabase *db, _DtDtsDbRecordCompare compare)
{
	if(compare == NULL)
	{
		compare = _DtDtsDbCompareRecordNames;
	}
	qsort(db->recordList,
		db->recordCount,
		sizeof(DtDtsDbRecord *),
		(genfunc)compare);
	db->compare = compare;
}

DtDtsDbField *
_DtDtsDbGetField(DtDtsDbRecord *rec, char *name)
{
	int i;

	/*
	 * Field names have been quarked so quark 'name' and
	 * do a linear search for the quark'ed field name.
	 */
	XrmQuark	tmp = XrmStringToQuark (name);

	for (i = 0; i < rec->fieldCount; i++)
	{
		if (rec->fieldList[i]->fieldName == tmp)
		{
			return (rec->fieldList[i]);
		}
	}
	return(NULL);
}

char *
_DtDtsDbGetFieldByName(DtDtsDbRecord *rec, char *name)
{
	DtDtsDbField	*result;

	result = _DtDtsDbGetField(rec, name);
	if(result)
	{
		return(result->fieldValue);
	}
	else
	{
		return(NULL);
	}

}

DtDtsDbRecord *
_DtDtsDbGetRecordByName(DtDtsDbDatabase *db, char *name)
{
	DtDtsDbRecord	srch;
	DtDtsDbRecord	**result;
	DtDtsDbRecord	*s = &srch;
	int i;
	XrmQuark 	name_quark = XrmStringToQuark(name);

	/*
	 * If the fields are not sorted in alphanumeric order
	 * by name a binary search will fail.  So do the slow but
	 * sure linear search.
	 */
	if(db->compare != _DtDtsDbCompareRecordNames)
	{

		for (i = 0; i < db->recordCount; i++)
		{
			if (db->recordList[i]->recordName == name_quark)
			{
				return (db->recordList[i]);
			}
		}
		return NULL;
	}

	srch.recordName = name_quark;	

	if(db->recordCount == 0 || db->recordList == NULL)
	{
		result = NULL;
	}
	else
	{
		result = (DtDtsDbRecord **)bsearch(&s,
			db->recordList,
			db->recordCount,
			sizeof(DtDtsDbRecord *),
			(genfunc)_DtDtsDbCompareRecordNames);
	}

	if(result)
	{
		return(*result);
	}
	else
	{
		return(NULL);
	}

}

DtDtsDbRecord *
_DtDtsDbAddRecord(DtDtsDbDatabase *db)
{
	DtDtsDbRecord	**newlist;
	int		rec = db->recordCount;

	db->compare = (_DtDtsDbRecordCompare)NULL;
	if(rec%PADMEM == 0)
	{
		newlist = (DtDtsDbRecord **)calloc(rec+PADMEM,
				sizeof(DtDtsDbRecord *));
		if(db->recordList)
		{
			memmove(newlist, db->recordList,
				rec*sizeof(DtDtsDbRecord *));
			free(db->recordList);
		}
		db->recordList = newlist;
	}
	db->recordList[rec] = (DtDtsDbRecord *)calloc(1, sizeof(DtDtsDbRecord));
	db->recordCount++;

	return(db->recordList[rec]);
}

int
_DtDtsDbDeleteRecord(DtDtsDbRecord *rec, DtDtsDbDatabase *db)
{
	int	i;

	_DtDtsDbDeleteFields(rec);
	free(rec);

	for(i = 0; i < db->recordCount; i++)
	{
		if(db->recordList[i] == rec)
		{
			memmove(	&(db->recordList[i]),
				&(db->recordList[i+1]),
				(db->recordCount - i - 1)*
					sizeof(DtDtsDbRecord *));
			db->recordCount--;
			return(1);
		}
	}

	return(0);
}

int
_DtDtsDbDeleteRecords(DtDtsDbDatabase *db)
{
	int	i;

	for(i = 0; i < db->recordCount; i++)
	{
		_DtDtsDbDeleteFields(db->recordList[i]);
		free(db->recordList[i]);
	}
	free(db->recordList);
	db->recordList = 0;
	return(0);
}

DtDtsDbField *
_DtDtsDbAddField(DtDtsDbRecord *rec)
{
	DtDtsDbField	**newlist;
	int		flds = rec->fieldCount;

	if(flds%PADMEM == 0)
	{
		newlist = (DtDtsDbField **)calloc(flds+PADMEM,
				sizeof(DtDtsDbField *));
		if(rec->fieldList)
		{
			memmove(newlist, rec->fieldList,
				flds*sizeof(DtDtsDbField *));
			free(rec->fieldList);
		}
		rec->fieldList = newlist;
	}
	rec->fieldList[flds] = (DtDtsDbField *)calloc(1, sizeof(DtDtsDbField));
	rec->fieldCount++;

	return(rec->fieldList[flds]);
}

int
_DtDtsDbDeleteField(DtDtsDbField *fld, DtDtsDbRecord *rec)
{
	int	i;

	free(fld);
	for(i = 0; i < rec->fieldCount; i++)
	{
		if(rec->fieldList[i] == fld)
		{
			memmove(	&(rec->fieldList[i]),
				&(rec->fieldList[i+1]),
				(rec->fieldCount - i - 1)*
					sizeof(DtDtsDbField *));
			rec->fieldCount--;
			return(1);
		}
	}

	return(0);
}

int
_DtDtsDbDeleteFields(DtDtsDbRecord *rec)
{
	int	i;

	for(i = 0; i < rec->fieldCount; i++)
	{
		free(rec->fieldList[i]->fieldValue);
		free(rec->fieldList[i]);
	}
	free(rec->fieldList);
	rec->fieldList = 0;
	return(0);
}

int
_DtDtsMMCreateFile(DtDirPaths *dirs, const char *CacheFile)
{
	return _MMWriteDb(dirs, num_db, db_list, CacheFile);
}
