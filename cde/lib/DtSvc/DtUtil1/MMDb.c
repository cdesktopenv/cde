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
/* $TOG: MMDb.c /main/19 1998/10/23 13:48:52 mgreess $ */
/*
 * +SNOTICE
 * 
 * Copyright 1995 Sun Microsystems, Inc.  All rights reserved.
 * 
 * +ENOTICE
 */
#include <stdio.h>
#include <sys/types.h>

#include <unistd.h>
#include <sys/utsname.h>
#include <stdlib.h>

#include <ctype.h>
#include <string.h>
#include <stdint.h>

#ifdef NLS16
#include <limits.h>
#endif

#include <sys/stat.h>
#include <sys/param.h>		/* MAXPATHLEN, MAXHOSTNAMELEN */
#define X_INCLUDE_DIRENT_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <Dt/DbReader.h>
#include <Dt/DtsDb.h>
#include <Dt/DtsMM.h>
#include <Dt/DtShmDb.h>
#include <Dt/Dts.h>
#include <Dt/ActionP.h>
#include <Dt/ActionDbP.h>
#include <Dt/ActionUtilP.h>
#include <Dt/DtNlUtils.h>
#include <Dt/UserMsg.h>
#include "myassertP.h"
#include "DtSvcLock.h"

static void build_file_list(DtShmProtoIntList, DtDirPaths *,
			    DtDtsMMHeader *, const char *);
extern	int	cde_dc_field_compare(DtDtsDbField **, DtDtsDbField **);
extern	int	cde_dc_compare(DtDtsDbRecord **, DtDtsDbRecord **);
static void	_DtMMSortDataTypes(DtShmProtoStrtab str_handle);
static void	_DtMMAddActionsToDataAttribute(DtDtsDbDatabase *db_ptr);
static int	write_db(DtDtsMMHeader *header, void *index, int size,
			 const char *CacheFile);
static int	build_new_db(DtShmProtoStrtab, DtShmProtoIntList, int, DtDtsDbDatabase **);
static int	build_name_list(DtDtsDbDatabase *, DtShmProtoIntList, DtDtsMMHeader *);

static	DtShmProtoStrtab	shm_handle = 0;
static	DtShmProtoIntList	int_handle = 0;

#define QtB(a)	_DtShmProtoAddStrtab(shm_handle, XrmQuarkToString(a), &isnew)

int
_MMWriteDb(DtDirPaths *dirs, int num_db, DtDtsDbDatabase **db_list,
	   const char *CacheFile)
{
	DtDtsMMHeader		header;
	char			*suffix = ".dt";
	int			tbl_size;
	void			*tbl_data;
	DtDtsDbDatabase	        *db;
	int			returnCode;

	_DtSvcProcessLock();
	memset(&header, '\0', sizeof(header));
	int_handle = _DtShmProtoInitIntLst(50000);
	shm_handle = _DtShmProtoInitStrtab(10000);

	build_file_list(int_handle, dirs, &header, suffix);

	_DtMMSortDataTypes(shm_handle);
	db = (DtDtsDbDatabase	*) _DtDtsDbGet(DtDTS_DA_NAME);
	_DtMMAddActionsToDataAttribute(db);

	header.pathhash = _DtDtsMMPathHash(dirs);
	header.num_db = num_db;
	header.db_offset = build_new_db(shm_handle, int_handle, num_db,
db_list);
	db = (DtDtsDbDatabase	*) _DtDtsDbGet("DATA_CRITERIA");
	build_name_list(db, int_handle, &header);

	tbl_size = _DtShmProtoSizeStrtab(shm_handle);
	tbl_data = (void *) _DtShmProtoAddIntLst(int_handle,
				tbl_size/sizeof(int), &header.str_tbl_offset);
	_DtShmProtoCopyStrtab(shm_handle, tbl_data);


	tbl_size = _DtShmProtoSizeIntLst(int_handle);
	tbl_data = (void *)malloc(tbl_size);
	memset(tbl_data, '\0', tbl_size);
	tbl_data = (void *)_DtShmProtoCopyIntLst(int_handle, tbl_data);

	returnCode = write_db(&header, tbl_data, tbl_size, CacheFile);
	_DtShmProtoDestroyStrtab(shm_handle);
	_DtShmProtoDestroyIntLst(int_handle);
	_DtSvcProcessUnlock();
	free(tbl_data);

	return returnCode;
}

static void
build_file_list(DtShmProtoIntList int_handle, DtDirPaths *dirs,
		DtDtsMMHeader *header, const char *suffix)
{
	DIR 			*dirp;
	struct dirent		*dp = NULL;
	struct stat		buf;
	char			cur_path[MAXPATHLEN+1];
	void			*data;
	int			size = sizeof(buf.st_mtime);
	int			i;
	int			isnew;
	DtShmBoson		*boson_list = 0;
	time_t			*mtime_list = 0;
	int			count = 0;
	_Xreaddirparams		dirEntryBuf;
	struct dirent		*result;

	/* Theses here to make sure it gets into the string tables
	   because actions uses it in its "types" field. */
	_DtShmProtoAddStrtab(shm_handle, DtDTS_DT_UNKNOWN, &isnew);
	_DtShmProtoAddStrtab(shm_handle, DtDTS_DT_RECURSIVE_LINK, &isnew);
	_DtShmProtoAddStrtab(shm_handle, DtDTS_DT_BROKEN_LINK, &isnew);

	getcwd(cur_path, sizeof(cur_path));
	for(i = 0; dirs->paths[i]; i++)
	{
		chdir(dirs->paths[i]);
		stat(".", &buf);
		count++;
		boson_list = (int *)realloc(boson_list, count*sizeof(int));
		mtime_list = (time_t *)realloc(mtime_list, count*sizeof(time_t));

		mtime_list[count-1] = buf.st_mtime;
		boson_list[count-1] = _DtShmProtoAddStrtab(shm_handle, dirs->paths[i], &isnew);
		dirp = opendir(".");
		while ((result = _XReaddir(dirp, dirEntryBuf)) != NULL)
		{
			char	*c = strrchr(result->d_name, suffix[0]);
			if(c && strcmp(c, suffix) == 0)
			{
	                        char	*pathname = malloc(MAXPATHLEN+1);
				sprintf(pathname, "%s/%s", dirs->paths[i], result->d_name);
				stat(result->d_name, &buf);
				count++;
				boson_list = (int *)realloc(boson_list, count*sizeof(int));
				mtime_list = (time_t *)realloc(mtime_list, count*sizeof(time_t));
				mtime_list[count-1] = buf.st_mtime;
				boson_list[count-1] = _DtShmProtoAddStrtab(shm_handle,
					pathname, &isnew);
				if (pathname) free(pathname);
				continue;
			}
		}
		(void)closedir( dirp );
	}
	chdir(cur_path);
	data = _DtShmProtoAddIntLst(int_handle, count, &header->files_offset);
	memcpy(data, boson_list, count*sizeof(int));
	data = _DtShmProtoAddIntLst(int_handle, count*sizeof(time_t)/sizeof(int), &header->mtimes_offset);
	memcpy(data, mtime_list, count*sizeof(time_t));
	header->files_count = count;
	if(boson_list)free(boson_list);
	if(mtime_list)free(mtime_list);
	return;
}

static int
db_table_size(int num_db, DtDtsDbDatabase **db_list)
{
	int			db;
	DtDtsDbDatabase		*db_ptr;
	int			rec;
	DtDtsDbRecord		*rec_ptr;
	int			fld;
	DtDtsDbField		*fld_ptr;
	int			size = 0;

	size += num_db*sizeof(DtDtsMMDatabase);
	for(db = 0; db < num_db; db++)
	{
		db_ptr = db_list[db];
		size += db_ptr->recordCount * sizeof(DtDtsMMRecord);
		for(rec = 0; rec < db_ptr->recordCount; rec++)
		{
			rec_ptr = db_ptr->recordList[rec];
			size += rec_ptr->fieldCount * sizeof(DtDtsMMField);
		}
	}
	return(size/sizeof(int));
}
static void
_DtMMSortDataTypes(DtShmProtoStrtab str_handle)
{
	DtDtsDbDatabase	*dc;
	DtDtsDbDatabase	*da;
	int		i;

	_DtSvcProcessLock();       
	dc = (DtDtsDbDatabase *) _DtDtsDbGet(DtDTS_DC_NAME);
	da = (DtDtsDbDatabase *) _DtDtsDbGet(DtDTS_DA_NAME);

/*_DtDtsDbPrintRecords(dc, stdout);*/
	for(i = 0; i < dc->recordCount; i++)
	{
		if(dc->recordList[i]->compare != cde_dc_field_compare)
		{
			_DtDtsDbFieldSort(dc->recordList[i], 
				cde_dc_field_compare);
		}
	}
	_DtDtsDbRecordSort(dc, cde_dc_compare);

	for(i = 0; i < da->recordCount; i++)
	{
		if(da->recordList[i]->compare !=
				_DtDtsDbCompareFieldNames)
		{
			_DtDtsDbFieldSort(da->recordList[i], 
				_DtDtsDbCompareFieldNames);
		}
	}

	_DtDtsDbRecordSort(da, _DtDtsDbCompareRecordNames);
/*_DtDtsDbPrintRecords(dc, stdout);*/
	_DtSvcProcessUnlock();
}

static void
add_if_missing(DtDtsDbRecord *rec_ptr, XrmQuark name, char *value)
{
	DtDtsDbField	*fld_ptr;
	int		fld;
	int		found = 0;

	for(fld = 0; fld < rec_ptr->fieldCount; fld++)
	{
		fld_ptr = rec_ptr->fieldList[fld];
		if(name == fld_ptr->fieldName)
		{
			found = 1;
			break;
		}
	}
	if(found)
	{
		return;
	}

	fld_ptr = _DtDtsDbAddField(rec_ptr);
	fld_ptr->fieldName = name;
	fld_ptr->fieldValue = value;
	_DtDtsDbFieldSort(rec_ptr, 0);

	return;
}

static void
_DtMMAddActionsToDataAttribute(DtDtsDbDatabase *db_ptr)
{
	int		rec;
	DtDtsDbRecord	*rec_ptr;
	int		action_flag = 0;
	int		sort_flag = 0;
	int		found_flag = 0;
	int		n;
	const	char	*tmp;
	XrmQuark	desc_qrk = XrmStringToQuark(DtDTS_DA_DESCRIPTION);
	XrmQuark	icon_qrk = XrmStringToQuark(DtDTS_DA_ICON);
	XrmQuark	label_qrk = XrmStringToQuark(DtDTS_DA_LABEL);

	for(rec = 0; rec < db_ptr->recordCount; rec++)
	{
		int	found_des = 0;
		int	found_icon = 0;
		int	found_label = 0;
		char	*obj_type;

		rec_ptr = db_ptr->recordList[rec];
		obj_type = XrmQuarkToString(rec_ptr->recordName);

		if ( _DtDtsDbGetFieldByName(rec_ptr,
				DtDTS_DA_IS_ACTION) == 0 )
		{
			continue;
		}
		add_if_missing(rec_ptr, desc_qrk, 
				DtActionDescription(obj_type));
		add_if_missing(rec_ptr, icon_qrk, DtActionIcon(obj_type));
		add_if_missing(rec_ptr, label_qrk, DtActionLabel(obj_type));
	}
}

static int
build_new_db(DtShmProtoStrtab shm_handle, DtShmProtoIntList int_handle, int num_db, DtDtsDbDatabase **db_list)
{
	DtDtsMMDatabase		*new_db_list;
	int			db;
	DtDtsDbDatabase		*db_ptr;
	DtDtsMMDatabase		*new_db_ptr;
	int			rec;
	DtDtsDbRecord		*rec_ptr;
	DtDtsMMRecord		*new_rec_ptr;
	DtDtsMMRecord		*new_rec_ptr_list;
	int			fld;
	DtDtsDbField		*fld_ptr;
	DtDtsMMField		*new_fld_ptr;
	DtDtsMMField		*new_fld_ptr_list;
	int			index;
	int			db_index;
	int			isnew;
	char			*tmp;

	/* create a space to hold the list of database structures */
	new_db_list = (DtDtsMMDatabase *)_DtShmProtoAddIntLst(int_handle,
			num_db*sizeof(DtDtsMMDatabase)/sizeof(int),
			&db_index);
	for(db = 0; db < num_db; db++)
	{
		int	last_boson = -1;
		int	list_count = 0;
		DtShmProtoInttab	nameIndex;
		int		size;
		int		*idx;

		new_db_ptr = &new_db_list[db];
		db_ptr = db_list[db];

		new_db_ptr->databaseName = _DtShmProtoAddStrtab(shm_handle, db_ptr->databaseName, &isnew);
		new_db_ptr->recordCount = db_ptr->recordCount;
		/* create space to hold record list */
		new_rec_ptr_list = (DtDtsMMRecord *)_DtShmProtoAddIntLst(int_handle,
				db_ptr->recordCount*sizeof(DtDtsMMRecord)/sizeof(int),
				&index);

		new_db_ptr->recordList = index;
		/* create index to names list */
		nameIndex = _DtShmProtoInitInttab(db_ptr->recordCount);
		for(rec = 0; rec < db_ptr->recordCount; rec++)
		{
			new_rec_ptr = &new_rec_ptr_list[rec];
			rec_ptr = db_ptr->recordList[rec];
			new_rec_ptr->recordName = QtB(rec_ptr->recordName);

			if(new_rec_ptr->recordName != last_boson)
			{
				/* save name position */
				_DtShmProtoAddInttab(nameIndex, new_rec_ptr->recordName, rec);

				last_boson = new_rec_ptr->recordName;
			}
			new_rec_ptr->pathId = _DtShmProtoAddStrtab(shm_handle,
				tmp = _DtDbPathIdToString(rec_ptr->pathId),
						 &isnew);
			XtFree(tmp);
			new_rec_ptr->seq = rec_ptr->seq;
			new_rec_ptr->fieldCount = rec_ptr->fieldCount;

			/* create space for field list */
			new_fld_ptr_list = (DtDtsMMField *)_DtShmProtoAddIntLst(int_handle,
				rec_ptr->fieldCount*sizeof(DtDtsMMField)/sizeof(int),
				&index);

			new_rec_ptr->fieldList = index;
			for(fld = 0; fld < rec_ptr->fieldCount; fld++)
			{
				new_fld_ptr = &new_fld_ptr_list[fld];
				fld_ptr = rec_ptr->fieldList[fld];

				new_fld_ptr->fieldName  = QtB(fld_ptr->fieldName);
				new_fld_ptr->fieldValue = fld_ptr->fieldValue?_DtShmProtoAddStrtab(shm_handle,
					fld_ptr->fieldValue, &isnew):0;
			}
		}
		/* create table for index and save it */
		size = _DtShmProtoSizeInttab(nameIndex);
		idx = _DtShmProtoAddIntLst(int_handle, size/sizeof(int), &new_db_ptr->nameIndex);
		_DtShmProtoCopyInttab(nameIndex, (void *)idx);
		_DtShmProtoDestroyInttab(nameIndex);
	}
	return(db_index);
}

struct  list
{
	DtShmBoson	boson;
	int		rec;
};

static int
srch(const void *a, const void *b)
{
	int results = ((struct list *)a)->boson - ((struct list *)b)->boson;

	if(results == 0)
	{
		results = ((struct list *)a)->rec - ((struct list *)b)->rec;
	}
	return(results);
}

static void
showtable(
	DtDtsDbDatabase *db,
	struct list *name_index, 
	struct list *other, 
	DtDtsMMHeader *head,
	int other_break)
{
	int	i;

	printf("============== names =====================\n");
	for(i = 0; name_index[i].boson; i++)
	{
		printf("%20s -> %s\n",
			XrmQuarkToString(db->recordList[name_index[i].rec]->recordName),
			_DtShmProtoLookUpStrtab(shm_handle, 
					name_index[i].boson));
	}
	printf("%d entries\n", i);
	
	printf("============= other ======================\n");
	for(i = 0; i < other_break; i++)
	{
		printf("%s\n",
			XrmQuarkToString(db->recordList[other[i].rec]->recordName));
	}
	printf("%d entries\n", i);			
}

static int
build_name_list(DtDtsDbDatabase *db,
		DtShmProtoIntList int_handle,
		DtDtsMMHeader	*head)
{
	struct list 	*other;
	int		i;
	char		*c;
	int		isnew;
	struct list	*name_index;
	int		next = 0;
	int		other_break = 0;
	DtShmProtoInttab	indexList = 0;
	DtShmBoson	last_boson = -1;
	int		*list_of_recs = 0;
	int		list_count = 0;
	int		index = 0;
	int		size;
	void		*space;

	/* create tmp space for two lists */
	name_index = (struct list *)calloc(db->recordCount*2,
					sizeof(struct list));
	other = (struct list *)calloc(db->recordCount, sizeof(struct list));

	/* step through all records */
	for(i = 0; i < db->recordCount; i++)
	{
		DtShmBoson	boson;
		char	*attr;
		char	*t;

		/* see if a name pattern exist */
		attr = _DtDtsDbGetFieldByName(db->recordList[i],
				DtDTS_NAME_PATTERN);
		if(!attr)
		{
			/* it didn't so check path pattern */
			attr = _DtDtsDbGetFieldByName(db->recordList[i],
				DtDTS_PATH_PATTERN);
			if(!attr)
			{
				/* neither exist so save it as plain buffer */
				if(!head->buffer_start_index)
				{
					head->buffer_start_index = other_break;
				}
				other[other_break++].rec = i;
				continue; /* go to next record */
			}
		}

		/* we have a name now find its final component */
		c = strrchr(attr, '/');
		if(c)
		{
			c++;
		}
		if(!c)
		{
			c = attr;
		}
		else
		{
			attr = c;
		}

		/* now see if that final component has any *,?,[ */
		while(c && *c &&
			  !(*c == '*' ||
			    *c == '[' ||
			    *c == '?' ||
			    *c == '$' ))
		{
			c++;
		}


		if(c && *c == '\0')
		{
			/* it doesn't so save it in the name index */
			name_index[next].boson = 
				_DtShmProtoAddStrtab(shm_handle,
					(const char *)attr, &isnew);
			name_index[next++].rec = i;
			continue; /* next record */
		}

		/* the name had something in it now lets get the suffix */
		c = strrchr(attr, '.');
		attr = c;

		/* lets see if the suffix has any  *,?,[ */
		while(c && *c &&
			  !(*c == '*' ||
			    *c == '[' ||
			    *c == '?' ||
			    *c == '$' ))
		{
			c++;
		}
		if(c && *c == '\0')
		{
			/* it doesn't so save it in the name index */
			name_index[next].boson = 
			_DtShmProtoAddStrtab(shm_handle,
					(const char *)attr, &isnew);
			name_index[next++].rec = i;
		}
		else
		{
			/* couldn't find any thing so save it as other */
			other[other_break++].rec = i;
		}

	}

	if (next > 0)
	{
		qsort(name_index, next, sizeof(struct list), srch);
	}

/*
showtable(db, name_index, other, head, other_break);
printf("                    next = %d\n", next);
printf("             other_break = %d\n", other_break);
printf("head->buffer_start_index = %d\n", head->buffer_start_index);
*/
	/* create a table and add the records to it. However
	   duplicates need to be in separate lists.
	*/
	indexList = _DtShmProtoInitInttab(next+3);
	for(i = 0; i <= next; i++)
	{
		if(i != next && (last_boson == -1 || name_index[i].boson == last_boson))
		{
			/* this a new list of records or an addition to one */
			list_of_recs = (int *)realloc(list_of_recs, 
					++list_count*sizeof(int));
			last_boson = name_index[i].boson;
			list_of_recs[list_count-1] = name_index[i].rec;
		}
		else
		{
			/* we reached the end of a list now we check how many
				are in the list. 
			*/
			if(list_count == 1)
			{
				/* if just one just add it in the index */
				_DtShmProtoAddInttab(indexList,
						last_boson, list_of_recs[0]);
			}
			else
			{
				/* if there are multiple items in the list
				   create a table for them */
				int	*list = _DtShmProtoAddIntLst(int_handle,
						list_count, &index);

				/* write the list to the to the table */
				memcpy(list, list_of_recs,
						list_count*sizeof(int));

				/* then index on the negative of the boson
				   so that we know it is a list */
				_DtShmProtoAddInttab(indexList,
						last_boson, -index);
				list_count = 0;
				list_of_recs = (int *)realloc(list_of_recs, 
						++list_count*sizeof(int));
			}
			if ( i != next )
			{
				/* reset for the next set */
				last_boson = name_index[i].boson;
				list_of_recs[list_count-1] = name_index[i].rec;
			}
		}

	}

	/* same thing but they all go into a separate list */
	if(other_break > 0)
	{
		/* create the space */
		int	*list = _DtShmProtoAddIntLst(int_handle,
				other_break, &head->no_name_offset);

		/* copy it into the list */
		for(i = 0; i < other_break; i++)
		{
			list[i] = other[i].rec;
		}
	}
	else
	{
		head->no_name_offset = -1;
	}

	/* make the real space */
	size = _DtShmProtoSizeInttab(indexList);
	space = _DtShmProtoAddIntLst(int_handle, size/sizeof(int), 
				&head->name_list_offset);
	_DtShmProtoCopyInttab(indexList, space);
	_DtShmProtoDestroyInttab(indexList);
	if(name_index)free(name_index);
	if(list_of_recs)free(list_of_recs);
	if(other)free(other);
	return(index);
}

static int
write_db(DtDtsMMHeader *header, void *index, int size, const char *CacheFile)
{
	int	fd;
	mode_t	cmask = umask((mode_t)077);
	char	*tmpfile;

	if ((tmpfile = malloc(sizeof(_DTDTSMMTEMPDIR) +
	    sizeof(_DTDTSMMTEMPFILE) + 7)) == NULL) {
		_DtSimpleError(DtProgName, DtError, NULL, tmpfile, NULL);
		return 0;
	}

	sprintf(tmpfile, "%s/%sXXXXXX", _DTDTSMMTEMPDIR, _DTDTSMMTEMPFILE);
	fd = mkstemp(tmpfile);

	umask(cmask);

	if(fd ==  -1)
	{
		_DtSimpleError(
			DtProgName, DtError, NULL,
			(char*) tmpfile, NULL);
		return(0);
	}

	/* Remove file on write failure - we don't */
	/* want a partial dtdbcache file. */
	if ((write(fd, header, sizeof(DtDtsMMHeader))
	     != sizeof(DtDtsMMHeader)) ||
	    (write(fd, index, size) != size))
	{
		close(fd);
		unlink(tmpfile);
		free(tmpfile);
		return(0);
	}

	close(fd);

	if(rename((const char *)tmpfile, CacheFile) == -1)
	{
		_DtSimpleError(
			DtProgName, DtError, NULL,
			(char*) CacheFile, NULL);
		unlink(CacheFile); /* Just in case? */
		unlink(tmpfile);
		free(tmpfile);
		return(0);
	}
	free(tmpfile);
	return(1);
}


_DtActionCompareRecordBoson(
        DtDtsMMRecord *record1,
        DtDtsMMRecord *record2 )
{
	int results = (int)record1->recordName - (int)record2->recordName;

	if (results)
		return(results);

	return((intptr_t)record1 - (intptr_t)record2);
}
