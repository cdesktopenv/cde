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
/*
 *	$TOG: DtsMM.c /main/16 1998/10/23 13:48:28 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *	(c) Copyright 1993,1994,1995 Sun Microsystems, Inc. 
 *		All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SUN_DB
#ifdef	SUN_DB
#include <sys/utsname.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/param.h>
#endif
#include <string.h>
#include <libgen.h>
#define X_INCLUDE_DIRENT_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <Dt/DbReader.h>
#include "Dt/DtsMM.h"
#include "Dt/DtNlUtils.h"
#include <Dt/UserMsg.h>
#include "DtSvcLock.h"

extern char *strdup(const char *);
static int MMValidateDb(DtDirPaths *dirs, char *suffix);
static int _debug_print_name(char *name, char *label);

typedef	int	(*genfunc)(const void *, const void *);

static	DtDtsMMDatabase	*db_list;
static	caddr_t		mmaped_db = 0;
static	size_t		mmaped_size = 0;
static	int		mmaped_fd = 0;
static	DtDtsMMHeader	*head = 0;

void *
_DtDtsMMGetPtr(int index)
{
	DtShmIntList  int_list;

	_DtSvcProcessLock();
	if(!mmaped_db)
	{
		_DtDtsMMInit(0);
	}
	int_list = (DtShmIntList)&mmaped_db[sizeof(DtDtsMMHeader)];
	_DtSvcProcessUnlock();
	return((void *)&int_list[index]);
}

int
_DtDtsMMGetPtrSize(int index)
{
	DtShmIntList  int_list;

	_DtSvcProcessLock();
	if(!mmaped_db)
	{
		_DtDtsMMInit(0);
	}
	int_list = (DtShmIntList)&mmaped_db[sizeof(DtDtsMMHeader)];
	_DtSvcProcessUnlock();
	return(int_list[index-1]);
}

int *
_DtDtsMMGetDCNameIndex(int *size)
{
        int *result;

	_DtSvcProcessLock();
	*size = _DtDtsMMGetPtrSize(head->name_list_offset);	
	result = (int*) _DtDtsMMGetPtr(head->name_list_offset);
	_DtSvcProcessUnlock();
	return(result);
}

int *
_DtDtsMMGetDbName(DtDtsMMDatabase *db, DtShmBoson boson)
{
	DtShmInttab	tab = (DtShmInttab)_DtDtsMMGetPtr(db->nameIndex);
	return((int *)_DtShmFindIntTabEntry(tab, boson));
}

int *
_DtDtsMMGetNoNameIndex(int *size)
{
        int *result;

	_DtSvcProcessLock();

	if(head->no_name_offset == -1)
	{
		*size = 0;
		_DtSvcProcessUnlock();
		return(0);
	}
	*size = _DtDtsMMGetPtrSize(head->no_name_offset);	
	result = (int *) _DtDtsMMGetPtr(head->no_name_offset);
	_DtSvcProcessUnlock();
	return(result);
}

/* returns the pointer to buffer only name list */
int *
_DtDtsMMGetBufferIndex(int *size)
{
	int	*list = (int*)_DtDtsMMGetNoNameIndex(size);
	int	*bufferIndex;

	_DtSvcProcessLock();
	*size -= head->buffer_start_index;
	bufferIndex = &list[head->buffer_start_index];
	_DtSvcProcessUnlock();

	return(bufferIndex);
}

DtShmInttab
_DtDtsMMGetFileList(void)
{
	DtShmInttab file_index;

	_DtSvcProcessLock();
	file_index = (DtShmStrtab)_DtDtsMMGetPtr(head->files_offset);
	_DtSvcProcessUnlock();
	return(file_index);
}

const char *
_DtDtsMMBosonToString(DtShmBoson boson)
{
	DtShmStrtab str_table;

	if (boson == 0)
		return(0);

	_DtSvcProcessLock();
	if(!mmaped_db)
	{
		_DtDtsMMInit(0);
	}

	str_table = (DtShmStrtab)_DtDtsMMGetPtr(head->str_tbl_offset);
	_DtSvcProcessUnlock();

	return(_DtShmBosonToString(str_table, boson));
}

DtShmBoson
_DtDtsMMStringToBoson(const char *string)
{
	DtShmStrtab str_table;

	if ((string == (char *)NULL) || (*string == '\0'))
		return(-1);

	_DtSvcProcessLock();
	if(!mmaped_db)
	{
		_DtDtsMMInit(0);
	}

	str_table = (DtShmStrtab)_DtDtsMMGetPtr(head->str_tbl_offset);
	_DtSvcProcessUnlock();

	return(_DtShmStringToBoson(str_table, string));
}

void
_DtDtsMMPrintFld(int fld, DtDtsMMField *fld_ptr, FILE *fd_in)
{
	const	char	*tmp;
	const	char	*tmpv;
	FILE	*fd = fd_in;

	if(!fd) fd = stdout;

	tmp = _DtDtsMMBosonToString(fld_ptr->fieldName);
	tmpv = _DtDtsMMBosonToString(fld_ptr->fieldValue);
	fprintf(fd, "\t\t[%d]\t%s(%d)\t%s(%d)\n", fld, tmp,fld_ptr->fieldName,
		tmpv?tmpv:"(NULL)", fld_ptr->fieldValue);
}

void
_DtDtsMMPrintRec(int rec, DtDtsMMRecord	*rec_ptr, FILE *fd_in)
{
	int		fld;
	DtDtsMMField	*fld_ptr;
	DtDtsMMField	*fld_ptr_list;
	const	char	*tmp;
	FILE	*fd = fd_in;

	if(!fd) fd = stdout;

	tmp = _DtDtsMMBosonToString(rec_ptr->recordName);
	fprintf(fd, "\tRec[%d] name = %s(%d)\n\t%d Fields\n", rec,
		tmp, rec_ptr->recordName,
		rec_ptr->fieldCount);
	fld_ptr_list = _DtDtsMMGetPtr(rec_ptr->fieldList);
	for(fld = 0; fld < rec_ptr->fieldCount; fld++)
	{
		fld_ptr = &fld_ptr_list[fld];
		_DtDtsMMPrintFld(fld, fld_ptr, fd);
	}
}

void
_DtDtsMMPrintDb(int db, DtDtsMMDatabase *db_ptr, FILE *fd_in)
{
	int		rec;
	DtDtsMMRecord	*rec_ptr;
	DtDtsMMRecord	*rec_ptr_list;
	const	char	*tmp;
	FILE	*fd = fd_in;

	if(!fd) fd = stdout;

	fprintf(fd, "DB[%d] ", db);
	tmp =  _DtDtsMMBosonToString(db_ptr->databaseName);
	fprintf(fd, "name = %s(%d)\n", tmp, db_ptr->databaseName);
	fprintf(fd, "%d Records\n", db_ptr->recordCount);
	rec_ptr_list = _DtDtsMMGetPtr(db_ptr->recordList);
	for(rec = 0; rec < db_ptr->recordCount; rec++)
	{
		rec_ptr = &rec_ptr_list[rec];
		_DtDtsMMPrintRec(rec, rec_ptr, fd);
	}
}

void
_DtDtsMMPrint(FILE *org_fd)
{
	int		db;
	DtDtsMMDatabase	*db_ptr;
	FILE		*fd = org_fd;
	const	char	*tmp;

	_DtSvcProcessLock();
	if(!mmaped_db)
	{
		_DtDtsMMInit(0);
	}

	for(db = 0; db < head->num_db; db++)
	{
		db_ptr = &db_list[db];
		if(fd == 0)
		{
			chdir("/tmp");
			tmp = _DtDtsMMBosonToString(db_ptr->databaseName);
			if((fd = fopen(tmp, "w")) == NULL)
			{
			    _DtSimpleError(
					DtProgName, DtError, NULL,
					(char*) tmp, NULL);
			    continue;
			}
		}
		_DtDtsMMPrintDb(db, db_ptr, fd);
		if(org_fd == 0)
		{
			fclose(fd);
			fd = 0;
		}
	}
	_DtSvcProcessUnlock();
}

int
_DtDtsMMCompareRecordNames(DtDtsMMRecord *a, DtDtsMMRecord *b)
{
	return (a->recordName - b->recordName);
}

int
_DtDtsMMCompareFieldNames(DtDtsMMField *a, DtDtsMMField *b)
{
	return (a->fieldName - b->fieldName);
}

#include <Dt/Dts.h>

int
_DtDtsMMInit(int override)
{
	DtDirPaths *dirs = _DtGetDatabaseDirPaths();
	char	*CacheFile = _DtDtsMMCacheName(1);
	if(override)
	{
		if (!_DtDtsMMCreateDb(dirs, CacheFile, override))
		{
			free(CacheFile);
			_DtFreeDatabaseDirPaths(dirs);
			return 0;
		}
		_debug_print_name(CacheFile, "Init");
	}
	else
	{
		int success = _DtDtsMMapDB(CacheFile);
		if(success)
		{
			if(!MMValidateDb(dirs, ".dt"))
			{
				success = 0;
			}
			else
			{
				_debug_print_name(CacheFile, "Mapped");
			}
		}
		if(!success)
		{
			free(CacheFile);
			CacheFile = _DtDtsMMCacheName(0);
			_debug_print_name(CacheFile, "Private");
			/* Check return status, and pass status to caller. */
			if (!_DtDtsMMCreateDb(dirs, CacheFile, override))
			{
				free(CacheFile);
				_DtFreeDatabaseDirPaths(dirs);
				return 0;
			}
		}
	}
	free(CacheFile);
	_DtFreeDatabaseDirPaths(dirs);
	return 1;
}

char **
_DtsMMListDb()
{
	int	i;
	char	**list;

	_DtSvcProcessLock();
	if(!mmaped_db)
	{
		_DtDtsMMInit(0);
	}

	list = (char **)malloc((head->num_db+1)*sizeof(char *));
	for ( i = 0; i < head->num_db; i++ )
	{
		list[i] = (char *)_DtDtsMMBosonToString(db_list[i].databaseName);
	}
	list[i] = 0;
	_DtSvcProcessUnlock();
	return(list);
}


DtDtsMMDatabase *
_DtDtsMMGet(const char *name)
{
	int		i;
	DtShmBoson	boson = _DtDtsMMStringToBoson(name);
	DtDtsMMDatabase *ret_db;

	_DtSvcProcessLock();
	if(!mmaped_db)
	{
		_DtDtsMMInit(0);
	}
	for(i = 0; i < head->num_db; i++)
	{
		if(db_list[i].databaseName == boson)
		{
			ret_db = &db_list[i];
		        _DtSvcProcessUnlock();

			return(ret_db);
		}
	}
	_DtSvcProcessUnlock();
	return(NULL);
}


DtDtsMMField *
_DtDtsMMGetField(DtDtsMMRecord *rec, const char *name)
{
	register int i;
	int		fld;
	DtDtsMMField	*fld_ptr;
	DtDtsMMField	*fld_ptr_list;

	/*
	 * Field names have been quarked so quark 'name' and
	 * do a linear search for the quark'ed field name.
	 */
	DtShmBoson	tmp = _DtDtsMMStringToBoson (name);

	fld_ptr_list = _DtDtsMMGetPtr(rec->fieldList);
	for (i = 0; i < rec->fieldCount; i++)
	{
		fld_ptr = &fld_ptr_list[i];
		if (fld_ptr->fieldName == tmp)
		{
			return (fld_ptr);
		}
	}
	return(NULL);
}

const char *
_DtDtsMMGetFieldByName(DtDtsMMRecord *rec, const char *name)
{
	DtDtsMMField	*result;

	result = _DtDtsMMGetField(rec, name);
	if(result)
	{
		return(_DtDtsMMBosonToString(result->fieldValue));
	}
	else
	{
		return(NULL);
	}

}

DtDtsMMRecord *
_DtDtsMMGetRecordByName(DtDtsMMDatabase *db, const char *name)
{
	DtDtsMMRecord	srch;
	DtDtsMMRecord	*result;
	DtDtsMMRecord	*s = &srch;
	register int i;
	DtShmBoson 	name_quark = _DtDtsMMStringToBoson(name);
	DtDtsMMRecord	*rec_ptr;
	DtDtsMMRecord	*rec_ptr_list;

	/*
	 * If the fields are not sorted in alphanumeric order
	 * by name a binary search will fail.  So do the slow but
	 * sure linear search.
	 */
	rec_ptr_list = _DtDtsMMGetPtr(db->recordList);

	for (i = 0; i < db->recordCount; i++)
	{
		rec_ptr = &rec_ptr_list[i];
		if (rec_ptr->recordName == name_quark)
		{
			return (rec_ptr);
		}
	}
	return NULL;
}
int
_DtDtsMMPathHash(DtDirPaths *dirs)
{
	int	pathhash = 0;
	DIR	*dirp;
	struct	dirent	*dp = NULL;
	int	suffixLen;
	int	nameLen;
	char	*file_suffix;
	char	*next_path;
	char	*suffix = ".dt";
	int	i;
	char	*cur_dir = getcwd(0,MAXPATHLEN);
	struct	stat	buf;

	_Xreaddirparams dirEntryBuf;
	struct dirent *result;

	for(i = 0; dirs->paths[i] ; i++)
	{
		if(chdir(dirs->paths[i]) == -1)
		{
			continue;
		}
		dirp = opendir (".");
		while ((result = _XReaddir(dirp, dirEntryBuf)) != NULL)
		{
			if ((int)strlen (result->d_name) >= (int)strlen(suffix))
			{
				suffixLen = DtCharCount(suffix);
				nameLen = DtCharCount(result->d_name);
				file_suffix = (char *)_DtGetNthChar(result->d_name,
						nameLen - suffixLen);
				stat(result->d_name, &buf);
				if (file_suffix &&
					(strcmp(file_suffix, suffix) == 0) &&
					(buf.st_mode&S_IFREG))
				{
					char *c = dirs->paths[i];
					while(*c)
					{
						pathhash += (int)*c;
						c++;
					}
					break;
				}
			}
		}
		closedir(dirp);
	}
	chdir(cur_dir);
	free(cur_dir);
	return(pathhash);
}

char *
_DtDtsMMCacheName(int override)
{
	char	*dsp = getenv("DISPLAY");
	char	*results = 0;
	char	*c;

	if(override && dsp)
	{
		results = malloc(strlen(_DTDTSMMTEMPDIR)+
				 strlen(_DTDTSMMTEMPFILE)+
				strlen(dsp)+3);
		sprintf(results, "%s/%s%s\0",
				_DTDTSMMTEMPDIR,
				_DTDTSMMTEMPFILE,
				dsp);
		c = strchr(results, ':');
		c = strchr(c, '.');
		if(c)
		{
			*c = '\0';
		}
	}
	else
	{
	/* tempnam(3) is affected by the TMPDIR environment variable. */
	/* This creates problems for rename() if "tmpfile" and "cacheFile" */
	/* are on different file systems.  Use tmpnam(3) to create the */
	/* unique file name instead. */
		char tmpnam_buf[L_tmpnam + 1];

		results = (char *)malloc(strlen(_DTDTSMMTEMPDIR) +
					 strlen(_DTDTSMMTEMPFILE) +
					 L_tmpnam + 3);
		tmpnam(tmpnam_buf);
		sprintf(results, "%s/%s%s", _DTDTSMMTEMPDIR, _DTDTSMMTEMPFILE,
			basename(tmpnam_buf));
	}
	return(results);
}


int
_DtDtsMMapDB(const char *CacheFile)
{
	struct	stat	buf;
	int	success = FALSE;

	_DtSvcProcessLock();

	if (mmaped_fd > 0)
	{
		/* Already have a file memory-mapped.  Unload it. */
		_DtDtsMMUnLoad();
	}

	mmaped_fd  = open(CacheFile, O_RDONLY, 0400);
	if(mmaped_fd !=  -1)
	{
		if(fstat(mmaped_fd, &buf) == 0 && buf.st_uid == getuid())
		{
			mmaped_db = (char *)mmap(NULL,
					buf.st_size,
					PROT_READ,
#if defined(sun) || defined(USL)
					/* MAP_NORESERVE is only supported
					   on sun and novell platforms */
					MAP_SHARED|MAP_NORESERVE,
#else
					MAP_SHARED,
#endif
					mmaped_fd,
					NULL);
			if(mmaped_db != (void *) -1)
			{
				success = TRUE;
				mmaped_size = buf.st_size;
				head = (DtDtsMMHeader *)mmaped_db;
				db_list = (DtDtsMMDatabase *)_DtDtsMMGetPtr(head->db_offset);
			}
			else
			{
			    _DtSimpleError(
					DtProgName, DtError, NULL,
					(char*) CacheFile, NULL);
			}
		}
	}
	if(!success)
	{
		mmaped_db = 0;
	}
	_DtSvcProcessUnlock();
	return(success);
}

static int
MMValidateDb(DtDirPaths *dirs, char *suffix)
{
	DIR 			*dirp;
	struct dirent		*direntp;
	struct stat		buf;
	struct stat		new_buf;
	int			size = sizeof(buf.st_mtime);
	DtShmBoson		*boson_list = 0;
	time_t			*mtime_list;
	int			count = 0;
	int			i;
	const char		*file;
	int			pathhash = _DtDtsMMPathHash(dirs);

	_DtSvcProcessLock();
	if(head->pathhash != pathhash)
	{
	        _DtSvcProcessUnlock();
		return(0);
	}

	count = head->files_count;
	mtime_list = _DtDtsMMGetPtr(head->mtimes_offset);
	boson_list = _DtDtsMMGetPtr(head->files_offset);

	for(i = 0; i < count; i++)
	{
		file = _DtDtsMMBosonToString(boson_list[i]);
		stat(file, &buf);
		if(mtime_list[i]  != buf.st_mtime)
		{
		        _DtSvcProcessUnlock();
			return(0);
		}
	}

	_DtSvcProcessUnlock();
	return(1);

}

char *
_DtDtsMMExpandValue(const char *value)
{
	char *newval;

	if(!value)
	{
		return NULL;
	}
	newval = (char *)malloc(1024);

	strcpy(newval, value);
	_DtDbFillVariables(&newval);
	return(newval);
}

void
_DtDtsMMSafeFree(char *value)
{
	if(value && !_DtDtsMMIsMemory(value))
	{
		free(value);
	}
}

int
_DtDtsMMIsMemory(const char *value)
{
	_DtSvcProcessLock();
	if((caddr_t)value < mmaped_db || (caddr_t)value > mmaped_db+mmaped_size)
	{
	        _DtSvcProcessUnlock();
		return(0);
	}
	else
	{
	        _DtSvcProcessUnlock();
		return(1);
	}
}

int
_DtDtsMMUnLoad()
{
	int	error = 0;

	_DtSvcProcessLock();
	_DtDtsClear();
	if(mmaped_db == 0)
	{
	        _DtSvcProcessUnlock();
		return(error);
	}
	if(munmap(mmaped_db, mmaped_size) == -1)
	{
		_DtSimpleError(DtProgName, DtError, NULL,
			       "munmap of dts_cache file", NULL);
		error = -1;
	}
	if(close(mmaped_fd) == -1)
	{
		_DtSimpleError(DtProgName, DtError, NULL,
			       "close of dts_cache file", NULL);
	}

	db_list = 0;
	mmaped_db = 0;
	mmaped_size = 0;
	mmaped_fd = 0;
	head = 0;
	_DtSvcProcessUnlock();
	return(error);
}

#include "Dt/UserMsg.h"

static int
_debug_print_name(char *name, char *label)
{
#ifdef DEBUG
	static char	*db = (char *)-1;

	_DtSvcProcessLock();
	if(db == (char *)-1)
	{
		db = getenv("MMAP_DEBUG");
	}
	_DtSvcProcessUnlock();

	if(db)
		_DtSimpleError(db,
			DtInformation,
			NULL,
			"%s - db name = %s\n", label,
			name);
#endif /* DEBUG */
	return(0);
}
