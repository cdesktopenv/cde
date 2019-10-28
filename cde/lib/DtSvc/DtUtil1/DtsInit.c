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
 * +SNOTICE
 * 
 * $XConsortium: DtsInit.c /main/5 1996/08/28 16:08:50 rswiston $
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
#include <stdio.h>
#include <sys/types.h>

#ifdef __hpux
#include <ndir.h>
#else

#if defined(sun) || defined(CSRG_BASED)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif				/* sun || CSRG_BASED */

#endif				/* __hpux */

#include <ctype.h>
#include <string.h>

#ifdef NLS16
#include <limits.h>
#endif

#include <sys/stat.h>
#include <sys/param.h>		/* MAXPATHLEN, MAXHOSTNAMELEN */
#include <Dt/DbReader.h>
#include <Dt/DtsDb.h>
#include <Dt/Dts.h>
#include "DtSvcLock.h"

extern int _DtDtsMMUnLoad(void);

static int      cur_dc_count = 0;
static int      cur_da_count = 0;
#define	_DtFT_NUM_FIELDS	20

int
_DtDtsNextDCSeq(void)
{
	int nextDCSeq;

	_DtSvcProcessLock();
	nextDCSeq = ++cur_dc_count;
	_DtSvcProcessUnlock();

	return(nextDCSeq);
}

int
_DtDtsNextDASeq(void)
{
	int nextDASeq;

	_DtSvcProcessLock();
	nextDASeq = ++cur_da_count;
	_DtSvcProcessUnlock();

	return(nextDASeq);
}

void
_DtDtsSeqReset(void)
{
        _DtSvcProcessLock();
	cur_dc_count = 0;
	cur_da_count = 0;
	_DtSvcProcessUnlock();
}
void
_DtDtsDCConverter(DtDtsDbField * fields,
	       DtDbPathId pathId,
	       char *hostPrefix,
	       Boolean rejectionStatus)
{
	DtDtsDbDatabase *db;
	DtDtsDbRecord  *rec;
	DtDtsDbField   *fld;
	int             i = 0;

	_DtSvcProcessLock();   
	db = _DtDtsDbGet(DtDTS_DC_NAME);
	while (fields[i].fieldName && fields[i].fieldValue)
	{
		if (i == 0)
		{
			if(rec=_DtDtsDbGetRecordByName(db,fields[i].fieldValue))
			{
				char *value = _DtDtsDbGetFieldByName(rec,DtDTS_DA_IS_SYNTHETIC);
				/*
				 * Check if the record is SYNTHETIC --
				 * if so then replace it with this real
				 * definition -- otherwise return.
				 */
				if (value && !strcmp(value,"True") )
				{
					/* free up the current record */
					_DtDtsDbDeleteRecord(rec,db);
				}
				else
				{
					_DtSvcProcessUnlock();
					return;
				}
			}
			rec = _DtDtsDbAddRecord(db);
			rec->recordName = XrmStringToQuark(fields[i].fieldValue);
			rec->seq = _DtDtsNextDCSeq();
			rec->pathId = (int)pathId;
		}
		else
		{
			fld = _DtDtsDbAddField(rec);
			fld->fieldName = fields[i].fieldName;
			fld->fieldValue = strdup(fields[i].fieldValue);
		}
		i++;
	}
	_DtSvcProcessUnlock();
}

void
_DtDtsDAConverter(DtDtsDbField * fields,
	       DtDbPathId pathId,
	       char *hostPrefix,
	       Boolean rejectionStatus)
{
	DtDtsDbDatabase *db;
	DtDtsDbRecord  *rec;
	DtDtsDbField   *fld;
	int             i = 0;

	_DtSvcProcessLock();    
	db = _DtDtsDbGet(DtDTS_DA_NAME);

	while (fields[i].fieldName && fields[i].fieldValue)
	{
		if (i == 0)
		{
			if(rec = _DtDtsDbGetRecordByName(db, fields[i].fieldValue))
			{
				char *value = _DtDtsDbGetFieldByName(rec,DtDTS_DA_IS_SYNTHETIC);
				/*
				 * Check if the record is SYNTHETIC --
				 * if so then replace it with this real
				 * definition -- otherwise return.
				 */
				if (value && !strcmp(value,"True") )
				{
					/* free up the current record */
					_DtDtsDbDeleteRecord(rec,db);
				}
				else
				{
				        _DtSvcProcessUnlock();
					return;
				}
			}
			rec = _DtDtsDbAddRecord(db);
			rec->recordName = XrmStringToQuark(fields[i].fieldValue);
			rec->seq = _DtDtsNextDASeq();
			fld = _DtDtsDbAddField(rec);
			fld->fieldName = XrmStringToQuark(DtDTS_DA_DATA_HOST);
			fld->fieldValue = hostPrefix?strdup(hostPrefix):(char *) 0;
			rec->pathId = (int)pathId;
		}
		else
		{
			fld = _DtDtsDbAddField(rec);
			fld->fieldName = fields[i].fieldName;
			fld->fieldValue = strdup(fields[i].fieldValue);
		}
		i++;
	}
	_DtSvcProcessUnlock();
}


/******************************************************************************
 *
 * DtDtsLoadDataTypes -
 *
 *   Reads in the file types and action databases.
 *
 *   Also initializes the variable DtMaxFileTypes to the number
 *   of entries in the file types database.
 *
 * MODIFIED:
 *
 *   DtMaxFtFileTypes
 *   DtMaxFileTypes - set to the number of real filetypes 
 *	 ( Holdovers from previous filetypes stuff -- still used by
 *	   some clients. i.e. dtfile )
 *
 *****************************************************************************/

void
DtDtsLoadDataTypes(void)
{
	/* with new mmap database this function is not needed to 
	 * load the database. Just to initialize it.
	 */

	_DtDtsMMUnLoad();
}
