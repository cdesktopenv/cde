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
 * +SNOTICE
 * 
 * $XConsortium: DbLoad.c /main/7 1996/08/28 14:38:07 rswiston $
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement bertween HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel wihtout Sun's specific written approval.  This documment and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 * +ENOTICE
 */
 
/*								        *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <stdio.h>
#include <sys/types.h>

#ifdef __hpux
#include <ndir.h>
#else

#if defined(sun) || defined(USL) || defined(sco) || defined(__uxp__)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif				/* sun || USL */

#endif				/* __hpux */

#include <ctype.h>
#include <string.h>

#ifdef NLS16
#include <limits.h>
#endif

#include <sys/stat.h>
#include <sys/param.h>		/* MAXPATHLEN, MAXHOSTNAMELEN */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Dt/DtP.h>
#include <Dt/Connect.h>
#include <Dt/FileUtil.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Action.h>
#include <Dt/ActionDbP.h>
#include <Dt/ActionP.h>
#include <Dt/ActionFind.h>
#include <Dt/DbUtil.h>
#include <Dt/DtPStrings.h>
#include <Dt/Utility.h>

#include <Dt/DtsDb.h>
#include <Dt/Dts.h>

#include "myassertP.h"
#include "DtSvcLock.h"

extern	void	_DtDtsDCConverter(DtDtsDbField * fields,
	       DtDbPathId pathId,
	       char *hostPrefix,
	       Boolean rejectionStatus);
extern	void	_DtDtsDAConverter(DtDtsDbField * fields,
	       DtDbPathId pathId,
	       char *hostPrefix,
	       Boolean rejectionStatus);

extern	void	_DtDtsSeqReset(void);
extern	int	_DtDtsNextDCSeq(void);
extern	int	_DtDtsNextDASeq(void);

char **_DtsDbListDb(void);

int      use_in_memory_db = False;

static void
_DtOAConverter(DtDtsDbField * fields,
	       DtDbPathId pathId,
	       char *hostPrefix,
	       Boolean rejectionStatus)
{
	DtDtsDbDatabase *dc_db;
	DtDtsDbDatabase *da_db;
	DtDtsDbRecord  *dc_rec;
	DtDtsDbRecord  *da_rec;
	DtDtsDbField   *fld;

	_DtSvcProcessLock();    
	dc_db = _DtDtsDbGet(DtDTS_DC_NAME);
	da_db = _DtDtsDbGet(DtDTS_DA_NAME);
	if ( _DtDtsDbGetRecordByName(dc_db,
				fields[0].fieldValue) ||
		_DtDtsDbGetRecordByName(da_db, 
				fields[0].fieldValue))
	{
		_DtSvcProcessUnlock();
		return;
	}

	/*
	 * Synthesize criteria record -- for this action
	 */
	dc_rec = _DtDtsDbAddRecord(_DtDtsDbGet(DtDTS_DC_NAME));
	dc_rec->recordName = XrmStringToQuark(fields[0].fieldValue);
	dc_rec->seq = _DtDtsNextDCSeq();
	dc_rec->pathId = (int)pathId;
	fld = _DtDtsDbAddField(dc_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_NAME_PATTERN);
	fld->fieldValue = strdup(fields[0].fieldValue);
	fld = _DtDtsDbAddField(dc_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_MODE);
	fld->fieldValue = strdup("fx");
	fld = _DtDtsDbAddField(dc_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_DATA_ATTRIBUTES_NAME);
	fld->fieldValue = strdup(fields[0].fieldValue);
	/*
	 * Mark the criteria record as synthetic.
	 */
	fld = _DtDtsDbAddField(dc_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_DA_IS_SYNTHETIC);
	fld->fieldValue = strdup("True");

	/*
	 * Synthesize attribute record -- for this action
	 */
	da_rec = _DtDtsDbAddRecord(_DtDtsDbGet(DtDTS_DA_NAME));
	da_rec->recordName = XrmStringToQuark(fields[0].fieldValue);
	da_rec->seq = _DtDtsNextDASeq();
	da_rec->pathId = (int)pathId;
	fld = _DtDtsDbAddField(da_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_DA_ACTION_LIST);
	fld->fieldValue = strdup(fields[0].fieldValue);
	fld = _DtDtsDbAddField(da_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_DA_DATA_HOST);
	fld->fieldValue = strdup(hostPrefix);

	/*
	 * Mark the attribute record as synthetic.
	 */
	fld = _DtDtsDbAddField(da_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_DA_IS_SYNTHETIC);
	fld->fieldValue = strdup("True");

	fld = _DtDtsDbAddField(da_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_DA_IS_ACTION);
	fld->fieldValue = strdup("True");

	fld = _DtDtsDbAddField(da_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_DA_IS_TEXT);
	fld->fieldValue = strdup("False");

        /*
         * Use the action name as the default copy_to action
         */
	fld = _DtDtsDbAddField(da_rec);
	fld->fieldName = XrmStringToQuark(DtDTS_DA_COPY_TO_ACTION);
	fld->fieldValue = strdup(fields[0].fieldValue);
	_DtSvcProcessUnlock();
}


/******************************************************************************
 *
 * DtDbLoad -
 *
 *   Reads in the file types and action databases.
 *   From the Default DtDatabaseDirPaths. 
 *
 *****************************************************************************/

void _DtDbLoad(DtDirPaths *dirs)
{
	_DtDtsMMUnLoad();
}

void
DtDbLoad(void)
{
	DtDirPaths      *dirs;

	/* Load the requested database files */
	dirs = _DtGetDatabaseDirPaths();
        _DtDbLoad(dirs);
	_DtFreeDatabaseDirPaths(dirs);
}

/******************************************************************************
 *
 * _DtDbLoad(dirs) -
 *
 *   Reads in the file types and action databases.
 *   From the the given directory path.  This function is not part of the
 *   public API but may be used internally to avoid repeated generation of
 *   the default databasedir path.
 *
 *****************************************************************************/
int
_DtDtsMMCreateDb(DtDirPaths *dirs, const char *CacheFile, int override)
{
	DtDbRecordDesc  recordDescriptions[3];
	DtDbConverter   criteriaConverters[2];
	DtDbConverter   attributesConverters[2];
	DtDbConverter   actionConverters[3];
	static	int	beenCalled = 0;
	char		**list;
	int		i;

	_DtSvcProcessLock();

	use_in_memory_db = TRUE;

	if ( beenCalled ) {
		/*
		 * Free up existing database.
		 */
		_DtDtsSeqReset();
		DtDtsRelease();
		_DtFreeActionDB();

	}
	beenCalled = 1;

	/*
	 * Initialize the databases
	 * -- this call will initializae the Object databases then
	 *    add the action database.
	 */
	(void) _DtDtsDbAddDatabase(_DtACTION_NAME);
	
        if ( !dirs )
        {
		myassert(dirs);	/* register an assertion failure */
		_DtSvcProcessUnlock();
                return(0);
	}

	/* Build up the record descriptions */
	criteriaConverters[0] = (DtDbConverter) _DtDtsDCConverter;
	criteriaConverters[1] = NULL;
	recordDescriptions[0].recordKeyword = DtDTS_DC_NAME;
	recordDescriptions[0].maxFields = _DtMAX_NUM_FIELDS;
	recordDescriptions[0].converters = criteriaConverters;

	/* Build up the ot record descriptions */
	attributesConverters[0] = (DtDbConverter) _DtDtsDAConverter;
	attributesConverters[1] = NULL;
	recordDescriptions[1].recordKeyword = DtDTS_DA_NAME;
	recordDescriptions[1].maxFields = _DtMAX_NUM_FIELDS;
	recordDescriptions[1].converters = attributesConverters;

	actionConverters[0] = (DtDbConverter) _DtActionConverter;
	actionConverters[1] = (DtDbConverter) _DtOAConverter;
	actionConverters[2] = NULL;
	recordDescriptions[2].recordKeyword = _DtACTION_NAME;
	recordDescriptions[2].maxFields = _ActDb_MAX_NUM_FIELDS;
	recordDescriptions[2].converters = actionConverters;


	_DtDbRead(dirs, ".dt", recordDescriptions, 3);

	_DtSortActionDb();

	/* 
         * we may eventually want to return a count of the new records.
         * for now we return a non-zero value to register success.
         */

	if ((!_DtDtsMMCreateFile(dirs, CacheFile)) ||
	    (!_DtDtsMMapDB(CacheFile)))
	{
		_DtSvcProcessUnlock();
		return(0);
	}
	if(!override)
	{
		unlink(CacheFile);
	}	

	/* now that we have built the databases delete the tmp Db memory
	   used for it (Too, bad we can't delete the memory associcated
	   with the Quarks) */
	list = (char **)_DtsDbListDb();
	for(i = 0; list[i]; i++)
	{
		_DtDtsDbDeleteDb(_DtDtsDbGet(list[i]));
		free(list[i]);
	}
	free(list);
	use_in_memory_db = FALSE;

	_DtSvcProcessUnlock();
	return(1);
}
