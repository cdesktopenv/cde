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
/* $TOG: ActionFind.c /main/11 1998/07/30 12:09:31 mgreess $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:         ActionFind.c
 **
 **   Project:	    DT
 **
 **   Description:  Source file for the action database sorting and 
 **                 locating functions.
 **		  
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*LINTLIBRARY*/
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <X11/Intrinsic.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>

#include <Tt/tttk.h>

#include <Dt/DtP.h>
#include <Dt/Message.h>
#include <Dt/DbReader.h>
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/UserMsg.h>
#include <Dt/CommandM.h>

#include <Dt/ActionP.h>
#include <Dt/ActionDb.h>
#include <Dt/ActionUtilP.h>
#include <Dt/ActionFind.h>
#include "myassertP.h"

#include <Dt/Utility.h>
#include <Dt/DtsDb.h>
#include <Dt/DtsMM.h>
#include <Dt/Dts.h>
#include "DtSvcLock.h"

/********    Private Function Declarations    ********/

DtDtsMMRecord *
_DtActionLocateRecord(
	DtShmBoson 	actQuark,
	unsigned long  	mask,
	DtShmBoson 	arg_type,
	int  		arg_count,
	DtDtsMMDatabase	*act_db);


/********    End Private Function Declarations    ********/


/********    Static Function Declarations    ********/

static int	 _DtActMMCompareObjClassMask(
			 unsigned long objClassMask,
			 char *actClass);
static int	 _DtActMMCompareMode(
			 unsigned long objMask,
			 char *actMode);
static int	 _DtActMMCompareType(
			 DtShmBoson reqType,
			 char *actType);
static int	 _DtActMMCompareCount(
			 int reqCount,
			 char *actCount);
static int _DtActionCompareRecordQuarks(
                    DtDtsDbRecord ** record1,
                    DtDtsDbRecord ** record2) ;
static void _DtActionGetRecordWeight(
                    DtDtsDbRecord ** record,
                    long * primaryWeightPtr,
                    long * secondaryWeightPtr );
static void ParseMessage( 
                        register ActionPtr action,
                        register char * str,
                        register parsedMsg * msg) ;
static Boolean ValidKeyword(
			char *start,
			char *end,
			char **prompt,
			int *keywordId,
			int *argNum,
			Boolean *isFile );

static unsigned long _DtActMMParseArgTypesList(
			char *tlist, 
			DtShmBoson **quarkAddr, 
			int *countp);
static unsigned long _DtActMMParseClassList(
			char *clist);
static unsigned long _DtActMMParseArgCountString(
			char *countStr, 
			int *countp);
static unsigned long _DtActMMParseActionType(
			char *actionTypeStr);
static void	_DtActMMGetCmdInfo(
			DtDtsMMRecord *actRecp,
			ActionPtr actp);
static void	_DtActMMGetTtMsgInfo(
			DtDtsMMRecord *actRecp,
			ActionPtr actp);
static int	_DtActInputSeqCheck(
			DtDtsDbRecord *r1,
			DtDtsDbRecord *r2);
static char *	_DtActDbChooseLabel(
			DtDtsDbRecord *rec);
static char *	_DtActMMChooseLabel(
			DtDtsMMRecord *rec);
static void	_DtCheckForDuplicateRecord(
			DtDtsDbRecord *rec,
			DtDtsDbRecord *duprec);
static char *	_DtActGetDtsDbField(
			DtDtsDbRecord *recp,
			char * fieldName );
static char *
_DtActGetDtsMMField ( DtDtsMMRecord *actRecp, char *name );
static void     _DtSearchDuplicateRecord(
                         DtDtsDbDatabase *db);

#ifdef _DT_ALLOW_DT_MSGS
static void	_DtActDbGetDtNtfyInfo(
			DtDtsDbRecord *actRecp,
			ActionPtr actp);
static void	_DtActDbGetDtReqInfo(
			DtDtsDbRecord *actRecp,
			ActionPtr actp);
#endif /* _DT_ALLOW_DT_MSGS */


/********    End Static Function Declarations    ********/


#define SPECIFIC_ARG_CLASS 8
#define SPECIFIC_ARG_TYPE  4
#define SPECIFIC_ARG_MODE  2
#define SPECIFIC_ARG_COUNT 1

#define SECONDARY_SPECIFIC     4
#define SECONDARY_LESS_THAN    3
#define SECONDARY_GREATER_THAN 2
#define SECONDARY_WILDCARD     1

#define _MAX_MAP_ATTEMPTS	100

/******************************************************************************
 * External data 
 ******************************************************************************/
extern XtAppContext _DtAppContext;

/******************************************************************************
 ******************************************************************************
 *
 *	Public API Functions 
 * 
 ******************************************************************************
 ******************************************************************************/

/*****************************************************************************
 * DtActionDescription:
 *---------------------
 * This function returns the description associated with an action of the
 * given name, "actionName".  If there are multiple actions with the desired
 * name, then the description of the "most generic" action of the given name 
 * is returned.
 *
 * NOTE: A new copy of the description string is returned.
 *	 It is up to the calling function to free the space allocated
 *	 for the description string.
 *****************************************************************************/
static char *_DtActionDbDescription(char *s);
static char *_DtActionMMDescription(char *s);

char *
DtActionDescription (
	char *s	
)
{
	char *actionDesc;

	_DtSvcAppLockDefault();
	actionDesc = (use_in_memory_db) ? _DtActionDbDescription(s) :
		_DtActionMMDescription(s);
	_DtSvcAppUnlockDefault();

	return actionDesc;
}

static char *
_DtActionDbDescription (  char *s )
{
	int n;
	XrmQuark	 tmpq;
	DtDtsDbRecord	**act_rec;
	DtDtsDbRecord	**last_rec_found = NULL;
	DtDtsDbDatabase	*act_db;

	_DtSvcProcessLock();     
	act_db = _DtDtsDbGet(_DtACTION_NAME);
	myassert(s);
	if ( !s ) {
		_DtSvcProcessUnlock();
		return NULL;
	}

	tmpq = XrmStringToQuark(s);
	n = act_db->recordCount;

	for ( act_rec = act_db->recordList; 
		n && act_rec && *act_rec ;
		 act_rec++, n--)
	{
		if ( (*act_rec)->recordName == (long) tmpq )
			last_rec_found = act_rec;

		/* 
		 * Since actions are in name "quark" order, there is
		 * no sense searching past the desired quark value.
		 */

		if ( (*act_rec)->recordName > (long) tmpq )
			break;
	}

	if ( last_rec_found ) {
	        _DtSvcProcessUnlock();
		return XtNewString(_DtActGetDtsDbField(
				*last_rec_found,_DtACTION_DESCRIPTION));
	}

	_DtSvcProcessUnlock();
	return NULL;
}

static char *
_DtActionMMDescription (  char *s )
{
	int		n;
	DtShmBoson	tmpq;
	DtDtsMMRecord	*act_rec;
	DtDtsMMRecord	*act_rec_list;
	DtDtsMMRecord	*last_rec_found = NULL;
	DtDtsMMDatabase	*act_db;
	int		*start;

	_DtSvcProcessLock();     
	act_db = _DtDtsMMGet(_DtACTION_NAME);
	myassert(s);
	if ( !s ) {
		_DtSvcProcessUnlock();
		return NULL;
	}

	if((tmpq = _DtDtsMMStringToBoson(s)) == -1)
	{
		_DtSvcProcessUnlock();
		return(NULL);
	}

	act_rec_list = _DtDtsMMGetPtr(act_db->recordList);
	start =  (int*)_DtDtsMMGetDbName(act_db,tmpq);

	for ( n = *start; n < act_db->recordCount; n++)
	{
		act_rec = &act_rec_list[n];
		if ( act_rec->recordName == tmpq )
			last_rec_found = act_rec;

		/* 
		 * Since actions are in name "boson" order, there is
		 * no sense searching past the desired quark value.
		 */

		if ( act_rec->recordName >  tmpq )
			break;
	}

	if ( last_rec_found ) {
		_DtSvcProcessUnlock();
		return _DtActGetDtsMMField(last_rec_found, 
				_DtACTION_DESCRIPTION);
	}
	_DtSvcProcessUnlock();
	return NULL;
}

/******************************************************************************
 * DtActionExists --
 *	Given a string return True; if the string corresponds to an
 *	action name;  False otherwise. 
 *
 ******************************************************************************/
static Boolean _DtActionDbExists(char *s);
static Boolean _DtActionMMExists(char *s);

Boolean
DtActionExists( char *s)
{
	Boolean actionExists;

	_DtSvcAppLockDefault();
	actionExists = (use_in_memory_db) ? _DtActionDbExists(s) :
		_DtActionMMExists(s);
	_DtSvcAppUnlockDefault();

	return actionExists;
}

static Boolean
_DtActionDbExists(char *s)
{
	int 		n;
	XrmQuark	tmpq;
	DtDtsDbRecord	**act_rec;
	DtDtsDbDatabase	*act_db;

	_DtSvcProcessLock();    
	act_db = _DtDtsDbGet(_DtACTION_NAME);
	myassert(s);
	if (!s) {
	        _DtSvcProcessUnlock();
		return False;
	}

	tmpq = XrmStringToQuark(s);
	n = act_db->recordCount;

	myassert (act_db);

	for ( act_rec = act_db->recordList; 
		n && act_rec && *act_rec;
		 act_rec++, n--)
	{
	    if ( (*act_rec)->recordName == (long) tmpq ) {
			_DtSvcProcessUnlock();
			return True;
	    }
	}
	_DtSvcProcessUnlock();
   	return False;
}

static Boolean
_DtActionMMExists(char *s)
{
	DtShmBoson	tmpq;
	DtDtsMMDatabase	*act_db;

	_DtSvcProcessLock();    
	act_db = _DtDtsMMGet(_DtACTION_NAME);
	myassert(s);
	if (!s) {
	        _DtSvcProcessUnlock();
		return False;
	}

	tmpq = _DtDtsMMStringToBoson(s);

	myassert (act_db);

	if(tmpq != -1 && _DtDtsMMGetDbName(act_db,tmpq))
	{
	        _DtSvcProcessUnlock();
		return True;
	}

	_DtSvcProcessUnlock();
   	return False;
}

/******************************************************************************
 * DtActionLabel --
 *	Given an action name string return the label string
 *      (localizable name) for the action. return NULL if no action is found.
 *	
 *	Return the label associated with the "most generic" action of the
 *	given name in the database.  In this case, we do not track down
 *	MAP actions since even a MAP action may have its own label.
 ******************************************************************************/
static char *_DtActionDbLabel(char *s);
static char *_DtActionMMLabel(char *s);

char *
DtActionLabel( char *s)
{
	char *actionLabel;

	_DtSvcAppLockDefault();
	actionLabel = (use_in_memory_db) ? _DtActionDbLabel(s) :
		_DtActionMMLabel(s);
	_DtSvcAppUnlockDefault();

	return actionLabel;
}

static char *
_DtActionDbLabel (  char *s )
{
	int		n;
	XrmQuark 	tmpq;
	char		*label;
	DtDtsDbRecord	**act_rec;
	DtDtsDbRecord	**last_rec_found = NULL;
	DtDtsDbDatabase	*act_db;

	_DtSvcProcessLock();     
	act_db = _DtDtsDbGet(_DtACTION_NAME);
	myassert(s);
	if ( !s ) {
	        _DtSvcProcessUnlock();
		return NULL;
	}

	tmpq = XrmStringToQuark(s);
	n = act_db->recordCount;

	for ( act_rec = act_db->recordList;
		 n && act_rec && *act_rec;
		 act_rec++, n--)
	{
		if ( (*act_rec)->recordName == (long) tmpq )
			last_rec_found = act_rec;
		/* 
		 * Since actions are in name "quark" order, there is
		 * no sense searching past the desired quark value.
		 */

		if ( (*act_rec)->recordName > (long) tmpq )
			break;
	}

	if ( last_rec_found )
	{
		/*
		 * We have found the desired action
		 * Obtain the associated label (if any).
		 */
	        _DtSvcProcessUnlock();
		return _DtActDbChooseLabel(*last_rec_found);

	}

	/*
	 * We failed to find the desired action -- therefore return NULL.
	 */
	_DtSvcProcessUnlock();
	return NULL;
}

static char *
_DtActionMMLabel (  char *s )
{
	int		n;
	DtShmBoson 	tmpq;
	char		*label;
	DtDtsMMRecord	*act_rec;
	DtDtsMMRecord	*act_rec_list;
	DtDtsMMRecord	*last_rec_found = NULL;
	DtDtsMMDatabase	*act_db;
	int		*start;

	_DtSvcProcessLock();     
	act_db = _DtDtsMMGet(_DtACTION_NAME);
	myassert(s);
	if ( !s ) {
		_DtSvcProcessUnlock();
		return NULL;
	}

	if((tmpq = _DtDtsMMStringToBoson(s)) == -1)
	{
		_DtSvcProcessUnlock();
		return(NULL);
	}

	act_rec_list = _DtDtsMMGetPtr(act_db->recordList);
	start =  (int*)_DtDtsMMGetDbName(act_db,tmpq);
	if ( !start ) {
		_DtSvcProcessUnlock();
		return NULL;
	}

	for ( n = *start; n < act_db->recordCount; n++)
	{
		act_rec = &act_rec_list[n];
		if ( act_rec->recordName == tmpq )
			last_rec_found = act_rec;
		/* 
		 * Since actions are in name "boson" order, there is
		 * no sense searching past the desired quark value.
		 */

		if ( act_rec->recordName > tmpq )
			break;
	}

	if ( last_rec_found )
	{
		/*
		 * We have found the desired action
		 * Obtain the associated label (if any).
		 */
		_DtSvcProcessUnlock();
		return _DtActMMChooseLabel(last_rec_found);

	}

	/*
	 * We failed to find the desired action -- therefore return NULL.
	 */
	_DtSvcProcessUnlock();
	return NULL;
}

/******************************************************************************
 * DtActionIcon --
 *	Given an action name string return the icon name string
 *      for the action. return NULL if no action is found.
 *	
 *	Return the icon associated with the "most generic" action of the
 *	given name in the database.  In this case, we do not track down
 *	MAP actions since even a MAP action may have its own icon.
 ******************************************************************************/
static char *_DtActionDbIcon(char *s);
static char *_DtActionMMIcon(char *s);

char *
DtActionIcon( char *s)
{
	char *actionIcon;

	_DtSvcAppLockDefault();
	actionIcon = (use_in_memory_db) ? _DtActionDbIcon(s) :
		_DtActionMMIcon(s);
	_DtSvcAppUnlockDefault();

	return actionIcon;
}

static char *
_DtActionDbIcon (  char *s )
{
	int		n;
	XrmQuark 	tmpq;
	char		*iconString;
	DtDtsDbRecord	**act_rec;
	DtDtsDbRecord	**last_rec_found = NULL;
	DtDtsDbDatabase	*act_db;

	_DtSvcProcessLock();     
	act_db = _DtDtsDbGet(_DtACTION_NAME);
	myassert(s);
	if ( !s ) {
		_DtSvcProcessUnlock();
		return NULL;
	}

	tmpq = XrmStringToQuark(s);
	n = act_db->recordCount;

	for ( act_rec = act_db->recordList;
		 n && act_rec && *act_rec;
		 act_rec++, n--)
	{
		if ( (*act_rec)->recordName == (long) tmpq )
			last_rec_found = act_rec;
		/* 
		 * Since actions are in name "quark" order, there is
		 * no sense searching past the desired quark value.
		 */

		if ( (*act_rec)->recordName > (long) tmpq )
			break;
	}

	if ( last_rec_found )
	{
		/*
		 * We have found the desired action
		 * Obtain the associated icon name (if any).
		 */
		if ((iconString = 
			_DtActGetDtsDbField(*last_rec_found,_DtACTION_ICON))
		    != NULL ) {
			_DtSvcProcessUnlock();
			return XtNewString(iconString);
		}

		/*
		 * If no icon name is specified for this action
		 * return the default action icon name.
		 */
		_DtSvcProcessUnlock();
		return _DtGetActionIconDefault();
	}

	/*
	 * We failed to find the desired action -- therefore return NULL.
	 */
	_DtSvcProcessUnlock();
	return NULL;
}

static char *
_DtActionMMIcon (  char *s )
{
	int		n;
	DtShmBoson 	tmpq;
	char		*iconString;
	DtDtsMMRecord	*act_rec;
	DtDtsMMRecord	*act_rec_list;
	DtDtsMMRecord	*last_rec_found = NULL;
	DtDtsMMDatabase	*act_db;
	int		*start;

	_DtSvcProcessLock();     
	act_db = _DtDtsMMGet(_DtACTION_NAME);
	myassert(s);
	if ( !s ) {
		_DtSvcProcessUnlock();
		return NULL;
	}

	if((tmpq = _DtDtsMMStringToBoson(s)) == -1)
	{
		_DtSvcProcessUnlock();
		return(NULL);
	}

	act_rec_list = _DtDtsMMGetPtr(act_db->recordList);
	start =  (int*)_DtDtsMMGetDbName(act_db,tmpq);
	for ( n = *start; n < act_db->recordCount; n++)
	{
		act_rec = &act_rec_list[n];
		if ( act_rec->recordName == tmpq )
			last_rec_found = act_rec;
		/* 
		 * Since actions are in name "boson" order, there is
		 * no sense searching past the desired quark value.
		 */

		if ( act_rec->recordName > tmpq )
			break;
	}

	if ( last_rec_found )
	{
		/*
		 * We have found the desired action
		 * Obtain the associated icon name (if any).
		 */
		if ((iconString = 
			_DtActGetDtsMMField(last_rec_found,_DtACTION_ICON))
		    != NULL ) {
			_DtSvcProcessUnlock();
			return iconString;
		}

		/*
		 * If no icon name is specified for this action
		 * return the default action icon name.
		 */
		_DtSvcProcessUnlock();
		return _DtGetActionIconDefault();
	}

	/*
	 * We failed to find the desired action -- therefore return NULL.
	 */
	_DtSvcProcessUnlock();
	return NULL;
}


/******************************************************************************
 ******************************************************************************
 *
 *	Private API Functions
 *
 ******************************************************************************
 ******************************************************************************/

/******************************************************************************
 *
 * _DtActGetDtsDbField( recp, *name )
 *
 *	This function is a wrapper around _DtDtsGetFieldByName() which provides
 *	default values for certain fields if they are not present in the 
 *	action database record. 
 *
 ******************************************************************************/

static char *
_DtActGetDtsDbField ( DtDtsDbRecord *actRecp, char *name )
{
	char *val = _DtDtsDbGetFieldByName( actRecp, name );

	if ( val )
		return val;

	/*
	 * Return defaults for certain necessary fields.
	 */
	if ( !strcmp( name, _DtACTION_TYPE ) )
		return _DtACT_TYPE_DFLT;
	if ( !strcmp( name, _DtACTION_ARG_CLASS ) )
		return _DtACT_ARG_CLASS_DFLT;
	if ( !strcmp( name, _DtACTION_ARG_TYPE ) )
		return _DtACT_ARG_TYPE_DFLT;
	if ( !strcmp( name, _DtACTION_ARG_COUNT ) )
		return _DtACT_ARG_CNT_DFLT;
	if ( !strcmp( name, _DtACTION_ARG_MODE ) )
		return _DtACT_ARG_MODE_DFLT;

	return NULL;

}

static char *
_DtActGetDtsMMField ( DtDtsMMRecord *actRecp, char *name )
{
	char *val = (char *)_DtDtsMMGetFieldByName( actRecp, name );

	if ( val )
		return _DtDtsMMExpandValue(val);

	/*
	 * Return defaults for certain necessary fields.
	 */
	if ( !strcmp( name, _DtACTION_TYPE ) )
		return strdup(_DtACT_TYPE_DFLT);
	if ( !strcmp( name, _DtACTION_ARG_CLASS ) )
		return strdup(_DtACT_ARG_CLASS_DFLT);
	if ( !strcmp( name, _DtACTION_ARG_TYPE ) )
		return strdup(_DtACT_ARG_TYPE_DFLT);
	if ( !strcmp( name, _DtACTION_ARG_COUNT ) )
		return strdup(_DtACT_ARG_CNT_DFLT);
	if ( !strcmp( name, _DtACTION_ARG_MODE ) )
		return strdup(_DtACT_ARG_MODE_DFLT);

	return NULL;

}

/******************************************************************************
 * 
 * _DtActDbChooseLabel
 * This function returns a label string for the given action record.
 * If the record does not contain a specified label then the action name
 * name itself is returned.
 *
 ******************************************************************************/

static char *
_DtActDbChooseLabel(DtDtsDbRecord *rec)
{
	char *label;

	if ( !rec )
		return NULL;

	if ((label=
		_DtActGetDtsDbField(rec, _DtACTION_LABEL)) != NULL)
		return XtNewString(label);

	/*
	 * If no label is specified for this action
	 * return the action name itself.
	 */
	return XtNewString(XrmQuarkToString(rec->recordName));
}

static char *
_DtActMMChooseLabel(DtDtsMMRecord *rec)
{
	char *label;

	if ( !rec )
		return NULL;

	label= _DtActGetDtsMMField(rec, _DtACTION_LABEL);
	if (NULL != label)
	{
	    /*
	     * Fix FMM Purify error.
	     */
	    char *rtn = XtNewString(label);
	    free((void*) label);
	    return rtn;
	}

	/*
	 * If no label is specified for this action
	 * return the action name itself.
	 */
	return XtNewString((char *)_DtDtsMMBosonToString(rec->recordName));
}

/*
 * This function is invoked by DtDbLoad(), once the complete
 * action database has been loaded.  It will first sort lexically each of 
 * the fields within all of the records.  It will then sort the records,
 * based upon the specificity of the action defined within the record.
 * Lastly, it will replace the record 'compare' function associated with
 * this database, so that it can be used to help locate a record, based
 * on certain criteria.
 */

/* used for building only */
void 
_DtSortActionDb( void )
{
   DtDtsDbDatabase * actionDB;
   int i;

   _DtSvcProcessLock();     
   actionDB = _DtDtsDbGet(_DtACTION_NAME);

   for (i = 0; i < actionDB->recordCount; i++)
   {
      /*
       *  Use the internal default DtsDb fieldsort function
       *  which sorts fields lexically by name; by passing a
       *  NULL for the compare function address.
       */
     _DtDtsDbFieldSort(actionDB->recordList[i], NULL);
   }


   _DtDtsDbRecordSort(actionDB, _DtActionCompareRecordQuarks);
  /*
   * We will  mark duplicate records for deletion by setting
   * the "pathId" field to zero.
   */
   _DtSearchDuplicateRecord(actionDB);

   /*
    * Search for and delete action records which have been marked
    * for deletion by setting PathId to zero.  These should be
    * duplicate records which have been overidden. 
    */
   for ( i=0; i < actionDB->recordCount; i++ )
   {
      if (actionDB->recordList[i]->pathId == 0)
      {
         _DtDtsDbDeleteRecord(actionDB->recordList[i],actionDB);
         /*
          * Record deletion has adjusted the recordCount field for the
          * database.  We must now adjust our counter (i) because record
          * deletion shifts all subsequent records down to backfill holes
          * in the record array. This means we must check the record at "i"
          * again because it is now a new, unchecked record.
          */
         i--;

	/*
	 * RWV: We should probably add the capacity to announce removal of
         *      such duplicate records.
	 */
      }
   }
   _DtSvcProcessUnlock();
}

/* used for building only */
static void
 _DtSearchDuplicateRecord(DtDtsDbDatabase *db)
{
    int i, j;
    DtDtsDbRecord *rec1, *rec2;

    i = 0;
    while(i < db->recordCount - 1) {
        rec1 = db->recordList[i];
        j = ++i;
   
        /* see if rec1 is already marked as a duplicate */
        if ( rec1->pathId == 0 )
		continue;

        while(j < db->recordCount) {
            rec2 = db->recordList[j];
            if(rec1->recordName == rec2->recordName) {
                if(rec2->pathId != 0)
                    _DtCheckForDuplicateRecord(rec1, rec2);
                j++;
            }
            else
                j = db->recordCount;
        }
    }
}

/******************************************************************************
 *
 * _DtCheckForDuplicateRecord
 *
 *  Compare the name, class, type, mode and arg_counts of the two input records.
 *  These are the selection criteria fields for actions.
 *  If they are identical then mark the potential duplicate (duprec) for
 *  deletion by setting its "pathId" field to zero. If the records are not
 *  really duplicates do nothing.
 ******************************************************************************/
/* used for building only */

static void
_DtCheckForDuplicateRecord( DtDtsDbRecord *rec, DtDtsDbRecord *duprec)
{
	char *field1, *field2;

	/*
	 * Compare action record name quarks
	 */

        myassert(rec->recordName == duprec->recordName);
	if (rec->recordName != duprec->recordName)
		return;	/* action names differ */

	/*
	 * Compare class fields
	 */
	field1 = _DtActGetDtsDbField(rec,_DtACTION_ARG_CLASS);
	field2 = _DtActGetDtsDbField(duprec,_DtACTION_ARG_CLASS);

	if ( strcmp(field1,field2) )
		return;	/* arg_class fields differ */

	/*
	 * Compare type fields
	 */
	field1 = _DtActGetDtsDbField(rec,_DtACTION_ARG_TYPE);
	field2 = _DtActGetDtsDbField(duprec,_DtACTION_ARG_TYPE);

	if ( strcmp(field1,field2) )
		return;	/* arg_type fields differ */

	/*
	 * Compare mode fields
	 */
	field1 = _DtActGetDtsDbField(rec,_DtACTION_ARG_MODE);
	field2 = _DtActGetDtsDbField(duprec,_DtACTION_ARG_MODE);

	if ( strcmp(field1,field2) )
		return;	/* arg_mode fields differ */

	/*
	 * Compare arg_count fields
	 */
	field1 = _DtActGetDtsDbField(rec,_DtACTION_ARG_COUNT);
	field2 = _DtActGetDtsDbField(duprec,_DtACTION_ARG_COUNT);

	if ( strcmp(field1,field2) )
		return;	/* arg_count fields differ */

	/*
	 * Mark the potential duplicate record for later deletion
         */
	duprec->pathId = 0;
	return;
}

/*
 * This function is used to sort the action database records; the
 * records are sorted first by name (i.e. quark value) and then, if the names
 * match, according to their specificity; the more specific
 * a definition is, the higher it is in the database.  Records which have
 * the same specificity will maintain their relative ordering.  Each record
 * is assigned a 'weight', which is based upon the specificity of a
 * prioritized set of fields within that record (i.e. the 'signiture'
 * fields).  The weight is assigned according to the following table;
 * the higher the table value, the more specific the action record is.
 *
 *  -------------------------------------------
 *  ARG_CLASS | S | S | S | S | N | N | N | N |
 *  -------------------------------------------
 *  ARG_TYPE  | S | S | W | W | S | S | W | W |
 *  -------------------------------------------
 *  ARG_COUNT | S | W | S | W | S | W | S | W |
 *  -------------------------------------------
 *  weight ->   7   6   5   4   3   2   1   0
 *
 *  KEY:  S = Value is very specific
 *        N = Value is not specific (i.e. "*", ">n", "<n")
 *
 * For action records which have a non-specific ARG_COUNT setting, there
 * is a second weight calculated.  Non-specific ARG_COUNT values are
 * weighted using the following guidelines:
 *
 *     specific  ->   highest
 *     "<n"
 *     ">n"
 *     "*"       ->   lowest
 */

/* used for building only */
static int 
_DtActionCompareRecordQuarks(
        DtDtsDbRecord ** record1,
        DtDtsDbRecord ** record2 )
{
   long r1PrimaryWeight, r1SecondaryWeight;
   long r2PrimaryWeight, r2SecondaryWeight;

   if ( (*record1)->recordName < (*record2)->recordName )
	return -1;
   else if ( (*record1)->recordName > (*record2)->recordName )
   	return 1;

   /* 
    *  For actions whose names (i.e. quarks) match, apply the weighting 
    *  algorithm described above.
    */

   _DtActionGetRecordWeight(record1, &r1PrimaryWeight, &r1SecondaryWeight);
   _DtActionGetRecordWeight(record2, &r2PrimaryWeight, &r2SecondaryWeight);

   if (r1PrimaryWeight > r2PrimaryWeight)
      return(-1);
   else if ( r1PrimaryWeight < r2PrimaryWeight)
      return(1);
   else
   {
      /* If primary weights are the same, check the secondary weight */
      if (r1SecondaryWeight > r2SecondaryWeight)
         return(-1);
      else if (r1SecondaryWeight < r2SecondaryWeight)
         return(1);
      else
      {
	 /*
	  * If everything matches we must preserve input order.
	  * We cannot simply assume that record1 preceded record2 in
	  * the input order since the quick sort itself may have done some
	  * rearranging, we must compare sequence numbers to determine
	  * which action was defined first.
          *
          */
	  return ((*record1)->seq - (*record2)->seq);
      }
   }
   /*myassert (0);
   return 0;
   */
}

/*
 * This function determines the weight which is to be assigned to an
 * action database entry.  The algorithm used was described earlier,
 * and will not be repeated here.  A higher weight is assigned for
 * more specifically defined actions (i.e. the 'signiture' fields
 * have not been wildcarded.
 */

static void
_DtActionGetRecordWeight(
        DtDtsDbRecord ** record,
        long * primaryWeightPtr,
        long * secondaryWeightPtr )
{
   char * argClass = _DtActGetDtsDbField(*record, _DtACTION_ARG_CLASS);
   char * argType = _DtActGetDtsDbField(*record, _DtACTION_ARG_TYPE);
   char * argMode  = _DtActGetDtsDbField(*record, _DtACTION_ARG_MODE);
   char * argCount = _DtActGetDtsDbField(*record, _DtACTION_ARG_COUNT);
   char * p;

   (*primaryWeightPtr) = 0;
   (*secondaryWeightPtr) = 0;

   if (strcmp(argClass, _DtACT_ANY))
      (*primaryWeightPtr) += SPECIFIC_ARG_CLASS;

   if (strcmp(argType, _DtACT_ANY))
      (*primaryWeightPtr) += SPECIFIC_ARG_TYPE;

   if (strcmp(argMode, _DtACT_ARG_MODE_ANY))
      (*primaryWeightPtr) += SPECIFIC_ARG_MODE;

   strtol(argCount, &p, 10);
   if ((p == argCount) || (*p != '\0'))
   {
      /* The ARG_COUNT field is not a simple integer value */
      if (DtStrchr(argCount, '<'))
         (*secondaryWeightPtr) = SECONDARY_LESS_THAN;
      else if (DtStrchr(argCount, '>'))
         (*secondaryWeightPtr) = SECONDARY_GREATER_THAN;
      else
         (*secondaryWeightPtr) = SECONDARY_WILDCARD;
   }
   else
   {
      (*primaryWeightPtr) += SPECIFIC_ARG_COUNT;
      (*secondaryWeightPtr) = SECONDARY_SPECIFIC;
   }
}

static unsigned long
_DtActMMParseArgTypesList(char *tlist, DtShmBoson **quarkAddr, int *countp)
{
	static DtShmBoson	quark_for_wild_char = 0;
	unsigned long 	mask = 0;
	int 		tcount;
	DtShmBoson	*qp;
	char 		**typeVec, **tvp;
	char		buf[_DtAct_MAX_BUF_SIZE];

	_DtSvcProcessLock();
	if ( !quark_for_wild_char )
		quark_for_wild_char = _DtDtsMMStringToBoson(_DtACT_ANY);
	_DtSvcProcessUnlock();

	strcpy(buf,tlist);
	tvp = typeVec = _DtVectorizeInPlace(buf,_DtACT_LIST_SEPARATOR_CHAR);

	/*
	 * Determine the number of types in the list
	 */
	for ( tcount = 0; *tvp; tvp++, tcount++ );

	*countp = tcount;
	myassert(tcount != 0);
	qp = *quarkAddr = (DtShmBoson *) XtCalloc(tcount + 1,sizeof(DtShmBoson));
	
	for ( tvp = typeVec; *tvp; tvp++ )
	{
		*qp = _DtDtsMMStringToBoson(*tvp);
		if ( *qp == quark_for_wild_char )
			SET_ARG_TYPE_WILD(mask);
		qp++;
	}
	
	/*
	 * use XtFree because it deal with free of a NULL string.
	 */
	XtFree((char *)typeVec);
	return  mask;
}

/* 
 * Parse the string containing a coma-separated list of supported classes
 * and convert it to an array of integers.  Allocate space for the int
 * array and fill it in with the internal representation number for each
 * supported class.  Return a mask (long) with the bits representating the
 * supported classes set.
 */
static unsigned long
_DtActMMParseClassList(char *clist)
{
	char buf[_DtAct_MAX_BUF_SIZE];
	char *bp;
	char **classVec, **cvp;
	unsigned long mask = 0;

	if (  !DtStrchr(clist, _DtACT_LIST_SEPARATOR_CHAR ) )
	{
		/*
		 * Only a single class is specified
		 */
		if ( !strcmp(clist,_DtACT_ANY) )
			SET_ARG_CLASS_WILD(mask);
		else if ( !strcmp(clist,_DtACTION_FILE) )
			SET_ARG_CLASS_FILE(mask);
		else if ( !strcmp(clist,_DtACTION_BUFFER) )
			SET_ARG_CLASS_BUFFER(mask);
		else if ( !strcmp(clist,_DtACTION_STRING) )
			SET_ARG_CLASS_STRING(mask);

		myassert(mask != 0 );
		return mask;
	}

	/*
	 * A list of classes was specified in the action definition	
	 * Make a local copy of the class string.
	 * Then vectorize it in place.
	 */
	strcpy(buf,clist);
	cvp = classVec = _DtVectorizeInPlace(buf,_DtACT_LIST_SEPARATOR_CHAR);
	
	for ( bp = *cvp; bp; bp = *cvp ) 
	{
		if ( !strcmp(bp,_DtACT_ANY) )
		{
			/* If its wild -- don't bother with any more parsing */
			XtFree((char *)classVec);
			return SET_ARG_CLASS_WILD(mask);
		}

		if ( !strcmp(bp,_DtACTION_FILE) )
			SET_ARG_CLASS_FILE(mask);
		else if ( !strcmp(bp,_DtACTION_BUFFER) )
			SET_ARG_CLASS_BUFFER(mask);
		else if ( !strcmp(bp,_DtACTION_STRING) )
			SET_ARG_CLASS_STRING(mask);
		cvp++;
	}

	XtFree((char *)classVec);
	myassert(mask != 0 );
	return mask;
}

static unsigned long
_DtActMMParseArgCountString( char *countStr, int *countp)
{
	unsigned long mask = 0;

	if ( !strcmp(countStr,_DtACT_ANY) )	
		return SET_ARG_COUNT_WILD(mask);

	if ( *countStr == _DtACT_GT_CHAR )
	{
		SET_ARG_COUNT_GT(mask);
		*countp = atoi(countStr + 1);
	} else if ( *countStr == _DtACT_LT_CHAR )
	{
		SET_ARG_COUNT_LT(mask);
		*countp = atoi(countStr + 1);
	} else
		*countp = atoi(countStr);

	return mask;
}

static unsigned long
_DtActMMParseActionType(char *actionTypeStr)
{
	unsigned long mask = 0;

	if ( !strcmp(actionTypeStr,_DtACTION_COMMAND) )
		return SET_CMD_ACTION(mask);
	if ( !strcmp(actionTypeStr,_DtACTION_TT_MSG) )
		return SET_TT_MSG(mask);
#ifdef _DT_ALLOW_DT_MSGS
	if ( !strcmp(actionTypeStr,_DtACTION_DT_REQUEST) )
		return SET_DT_REQUEST_MSG(mask);
	if ( !strcmp(actionTypeStr,_DtACTION_DT_NOTIFY) )
		return SET_DT_NOTIFY_MSG(mask);
#endif /* _DT_ALLOW_DT_MSGS */

	myassert(0);	/* we should never get here */
	if ( !strcmp(actionTypeStr,_DtACTION_MAP) )
		return SET_MAP_ACTION(mask);

	return mask;
}


static void
_DtActMMGetCmdInfo(DtDtsMMRecord *actRecp, ActionPtr actp)
{
	char *s;
	char *tmp = NULL;
	int amtToAlloc;
	static char *titleStr = "-title \"";

	/* set the WINDOW_TYPE action mask bits */

	s = _DtActGetDtsMMField(actRecp,_DtACTION_WINDOW_TYPE);
	if ( !s ) 
	{
		/* 
		 * WINDOW_TYPE not set -- default to PERM_TERM 
		 */
		SET_PERM_TERM(actp->mask);
	} else if ( !strcmp(s,_DtACTION_TERMINAL) )
		SET_TERMINAL(actp->mask);
	else if ( !strcmp(s, _DtACTION_NO_STDIO) )
		SET_NO_STDIO(actp->mask);
	else if ( !strcmp(s, _DtACTION_PERM_TERMINAL) )
		SET_PERM_TERM(actp->mask);
	else
	{
		/* unrecognized terminal type was obtained */
		myassert(0);	/* this should never happen */
		SET_PERM_TERM(actp->mask);
	}
	_DtDtsMMSafeFree(s);

	/* Get the (optional) CWD info */
	if ( (s = _DtActGetDtsMMField(actRecp,_DtACTION_CWD)) != NULL)
	{
		/*
                 * HOST:CWD syntax is no longer recognized here.
		 * actp->u.cmd.contextHost = _DtHostString(s);
		 * actp->u.cmd.contextDir  = _DtPathname(s);
                 */

		/* Keywords are not allowed in the CWD field */
		 actp->u.cmd.contextDir  = XtNewString(s);
	}
	_DtDtsMMSafeFree(s);

	/* Get the (required) Exec String */
	if ( (s = _DtActGetDtsMMField(actRecp,_DtACTION_EXEC_STRING)) != NULL)
	{
		ParseMessage(actp,s,&(actp->u.cmd.execString));
	} else
	{
		/*
		 * Should never get here  -- actions without exec strings
		 * should have been rejected by the action converter.
		 */
		myassert(0);
		ParseMessage(actp,getenv("SHELL"),&(actp->u.cmd.execString));
	}
	_DtDtsMMSafeFree(s);

	/* Get the (optional) Exec Host List */
	if ( (s = _DtActGetDtsMMField(actRecp,_DtACTION_EXEC_HOST)) != NULL)
	{
		ParseMessage(actp,s,&(actp->u.cmd.execHosts));
		/*
		 * The execHostCount field will be filled in later when
		 * the parsed message is interpreted and the execHostArray
		 * is filled in.
		 */
	} else 
	{
		/* set to default exec host */
		tmp = _DtGetExecHostsDefault();
		ParseMessage(actp,tmp,&(actp->u.cmd.execHosts));
		if (tmp)
			XtFree(tmp);
	}
	_DtDtsMMSafeFree(s);
	

	/* Get the (optional) Term Opts */
	s = _DtActGetDtsMMField(actRecp,_DtACTION_TERM_OPTS);
	/* 
	 * Prepend with internally generated title
	 *  -- user supplied titles will appear 
	 * afterwards and therefore take precedence (for most terminal
	 * emulators.)  Allocate enough space for the required strings
         * space separators, quotes and NULL terminator.
	 */
	amtToAlloc = strlen(titleStr) + strlen(actp->label) + 2;
	if (s)
	  amtToAlloc += strlen(s) + 1;
	tmp = XtMalloc(amtToAlloc);
	strcpy(tmp, titleStr);
	strcat(tmp, actp->label);
        strcat(tmp, "\"");
	if (s)
	{
		strcat(tmp, " ");
		strcat(tmp, s);
	}
	_DtDtsMMSafeFree(s);

	ParseMessage(actp,tmp,&(actp->u.cmd.termOpts));
	if (tmp)
		XtFree(tmp);
	
}


static void
_DtActMMGetTtMsgInfo(DtDtsMMRecord *actRecp, ActionPtr actp)
{
	char argNname[ sizeof(_DtACTION_TTN_ARG) /* space for prefix */
		+ 3	/* enough space for three decimal digits */
		+ sizeof(_DtACTION_TTN_REP_TYPE)]; /* space for longest suffix */
	int  i;
	char *s;
	char *tmp = NULL;

	/* Get the (required) tt_class field */
	s = _DtActGetDtsMMField(actRecp,_DtACTION_TT_CLASS);
	if (!s) 
	{
		myassert(0);	/* should never get here */
		actp->u.tt_msg.tt_class = TT_CLASS_UNDEFINED;
	} 
	else if ( !strcmp(s,_DtACTION_TT_REQUEST ))
	{
		actp->u.tt_msg.tt_class = TT_REQUEST;
	} 
	else if ( !strcmp(s,_DtACTION_TT_NOTICE) )
	{
		actp->u.tt_msg.tt_class = TT_NOTICE;
	}
	else	/* unrecognized string */
	{
		/* 
		 * We should never get here -- such records should have
	 	 * been weeded out by the Action converter.
		 */
		myassert(0);
		actp->u.tt_msg.tt_class = TT_CLASS_UNDEFINED;
	}
	_DtDtsMMSafeFree(s);

	/* Get the (required) TT_SCOPE field */
	s = _DtActGetDtsMMField(actRecp,_DtACTION_TT_SCOPE);
	if (!s)
	{
		myassert(0);	/* should never get here */
		actp->u.tt_msg.tt_scope = TT_SCOPE_NONE;
	}
	else if ( !strcmp(s,_DtACTION_TT_SESSION) )
	{
		actp->u.tt_msg.tt_scope = TT_SESSION;
	}
	else if ( !strcmp(s,_DtACTION_TT_FILE) )
	{
		actp->u.tt_msg.tt_scope = TT_FILE;
	}
	else if ( !strcmp(s,_DtACTION_TT_BOTH) )
	{
		actp->u.tt_msg.tt_scope = TT_BOTH;
	}
	else if ( !strcmp(s,_DtACTION_TT_FILE_IN_SESSION) )
	{
		actp->u.tt_msg.tt_scope = TT_FILE_IN_SESSION;
	}
	else
	{
		/* unrecognized tt_scope */
		myassert(0);	/* should never happen */
		actp->u.tt_msg.tt_scope = TT_SCOPE_NONE;
	}
	_DtDtsMMSafeFree(s);

	/* Get the (required) TT_OPERATION field */
	if ( (s = _DtActGetDtsMMField(actRecp,_DtACTION_TT_OPERATION)) )
		ParseMessage(actp,s,&(actp->u.tt_msg.tt_op));
	else
	{
		myassert(0);	/* should never get here */
	}
	_DtDtsMMSafeFree(s);

	/* Get the (optional) TT_FILE field */
	if ( (s = _DtActGetDtsMMField(actRecp,_DtACTION_TT_FILE)) )
	{
		/*
		 * Special considerations:
		 * >> %Args% keyword is invalid.
		 * >> This value must reference a single file
		 * >> host:syntax needs to be parsed.
		 */
		
		ParseMessage(actp,s,&(actp->u.tt_msg.tt_file));
	}
	_DtDtsMMSafeFree(s); s = (char *) 0;

	/*
	 * Get message argument values
	 */

	for ( i=0; True; i++ )
	{
		sprintf(argNname,"%s%d%s",
			 _DtACTION_TTN_ARG,i,_DtACTION_TTN_MODE);
		if ( !(s = _DtActGetDtsMMField(actRecp,argNname)) )
			break;	/* no more args */
		actp->u.tt_msg.tt_argn_mode = 
			(int *)XtRealloc(
				(char *)actp->u.tt_msg.tt_argn_mode, 
				sizeof(int) * (i+1) );
		if ( !strcmp(s,_DtACTION_TT_MODE_IN) )
			actp->u.tt_msg.tt_argn_mode[i] = TT_IN;
		else if ( !strcmp(s,_DtACTION_TT_MODE_OUT) )
			actp->u.tt_msg.tt_argn_mode[i] = TT_OUT;
		else if ( !strcmp(s,_DtACTION_TT_MODE_INOUT) )
			actp->u.tt_msg.tt_argn_mode[i] = TT_INOUT;
		else
		{
			myassert(0); /* should never reach here */
			actp->u.tt_msg.tt_argn_mode[i] = TT_IN;
		}

		/* get tt_argn_vtype info */
		sprintf(argNname,"%s%d%s",
			 _DtACTION_TTN_ARG,i,_DtACTION_TTN_VTYPE);
		actp->u.tt_msg.tt_argn_vtype = 
			(parsedMsg *)XtRealloc(
				(char *)actp->u.tt_msg.tt_argn_vtype, 
				sizeof(parsedMsg) * (i+1) );
		_DtDtsMMSafeFree(s); s = (char *)0;
		s = _DtActGetDtsMMField(actRecp,argNname); 
		ParseMessage(actp,s,actp->u.tt_msg.tt_argn_vtype + i);

		/* get tt_argn_value info */
		sprintf(argNname,"%s%d%s",
			 _DtACTION_TTN_ARG,i,_DtACTION_TTN_VALUE);
		actp->u.tt_msg.tt_argn_value = 
			(parsedMsg *)XtRealloc(
				(char *)actp->u.tt_msg.tt_argn_value, 
				sizeof(parsedMsg) * (i+1) );
		_DtDtsMMSafeFree(s); s = (char *)0;
		s = _DtActGetDtsMMField(actRecp,argNname);
		ParseMessage(actp,s,actp->u.tt_msg.tt_argn_value + i);

		/* get tt_argn_rep_type info */
		sprintf(argNname,"%s%d%s",
			 _DtACTION_TTN_ARG,i,_DtACTION_TTN_REP_TYPE);
		actp->u.tt_msg.tt_argn_rep_type = 
			(int *)XtRealloc(
				(char *)actp->u.tt_msg.tt_argn_rep_type, 
				sizeof(int) * (i+1) );
		_DtDtsMMSafeFree(s); s = (char *)0;
		if ( !(s = _DtActGetDtsMMField(actRecp,argNname)) )
			actp->u.tt_msg.tt_argn_rep_type[i] =	
				 DtACT_TT_REP_UNDEFINED;
		else if ( !(strcmp(s,_DtACTION_TT_RTYP_UND)) )
			actp->u.tt_msg.tt_argn_rep_type[i] =
				 DtACT_TT_REP_UNDEFINED;
		else if ( !(strcmp(s,_DtACTION_TT_RTYP_INT)) )
			actp->u.tt_msg.tt_argn_rep_type[i] =
				 DtACT_TT_REP_INT;
		else if ( !(strcmp(s,_DtACTION_TT_RTYP_STR)) )
			actp->u.tt_msg.tt_argn_rep_type[i] =
				 DtACT_TT_REP_STRING;
		else if ( !(strcmp(s,_DtACTION_TT_RTYP_BUF)) )
			actp->u.tt_msg.tt_argn_rep_type[i] =
				 DtACT_TT_REP_BUFFER;
		else
		{
			myassert(0); /* should never reach here */
			actp->u.tt_msg.tt_argn_rep_type[i] =	
				 DtACT_TT_REP_UNDEFINED;
		}
		_DtDtsMMSafeFree(s); s = (char *)0;
	}
	_DtDtsMMSafeFree(s); s = (char *)0;
	/*
	 * Perhaps we can make do with a single count if the assumption that
	 * the counts for mode,vtype,reptype and value always match is valid.
	 */
	actp->u.tt_msg.mode_count = actp->u.tt_msg.vtype_count = 
		actp->u.tt_msg.rep_type_count = actp->u.tt_msg.value_count = i;
	
}

#ifdef _DT_ALLOW_DT_MSGS
static void
_DtActDbGetDtNtfyInfo(
	DtDtsDbRecord *actRecp,
	ActionPtr actp)
{
	int i;
	char argNval[ sizeof(_DtACTION_DTN_ARG) /* space for prefix */
			+ 3	/* enough space for three decimal digits */
			+ sizeof(_DtACTION_DTN_VALUE)]; /* space for suffix */
	char *s;
	char *tmp = NULL;

	/* Get the (required) dt_ngroup field */
	if ( !(s = _DtActGetDtsDbField(actRecp,_DtACTION_DT_NGROUP)) )
	{
		myassert(0);	/* should never get here */
	} 
	ParseMessage(actp,s,&(actp->u.dt_notify.ngroup));
	
	/* Get the (required) dt_notify_string field */
	if ( !(s = _DtActGetDtsDbField(actRecp,_DtACTION_DT_NOTIFY_NAME)) )
	{
		myassert(0);	/* should never get here */
	} 
	ParseMessage(actp,s,&(actp->u.dt_notify.notify));

	/*
	 * Get message argument values
	 */

	for ( i=0; True; i++ )
	{
		sprintf(argNval,"%s%d%s",
			 _DtACTION_DTN_ARG,i,_DtACTION_DTN_VALUE);
		if ( !(s = _DtActGetDtsDbField(actRecp,argNval)) )
			break;	/* no more args */
		actp->u.dt_notify.argn_value = 
			(parsedMsg *)XtRealloc(
				(char *)actp->u.dt_notify.argn_value, 
				sizeof(parsedMsg) * (i+1) );
		ParseMessage(actp,s,actp->u.dt_notify.argn_value + i);
	}
	actp->u.dt_notify.value_count = i;
}

static void
_DtActDbGetDtReqInfo(
	DtDtsDbRecord *actRecp,
	ActionPtr actp)
{
	int i;
	char argNval[ sizeof(_DtACTION_DTN_ARG) /* space for prefix */
			+ 3	/* enough space for three decimal digits */
			+ sizeof(_DtACTION_DTN_VALUE)]; /* space for suffix */
	char *s;
	char *tmp = NULL;

	/* Get the (required) dt_svc field */
	if ( !(s = _DtActGetDtsDbField(actRecp,_DtACTION_DT_SVC)) )
	{
		myassert(0);	/* should never get here */
	} 
	ParseMessage(actp,s,&(actp->u.dt_req.service));

	/* Get the (required) dt_request_name field */
	if ( !(s = _DtActGetDtsDbField(actRecp,_DtACTION_DT_REQUEST_NAME)) )
	{
		myassert(0);	/* should never get here */
	} 
	ParseMessage(actp,s,&(actp->u.dt_req.request));

	for ( i=0; True; i++ )
	{
		sprintf(argNval,"%s%d%s",
			 _DtACTION_DTN_ARG,i,_DtACTION_DTN_VALUE);
		if ( !(s = _DtActGetDtsDbField(actRecp,argNval)) )
			break;	/* no more args */
		actp->u.dt_req.argn_value = 
			(parsedMsg *) XtRealloc(
				(char *)actp->u.dt_req.argn_value, 
				sizeof(parsedMsg) * (i+1) );
		ParseMessage(actp,s,actp->u.dt_req.argn_value + i);
	}
	actp->u.dt_req.value_count = i;

}
#endif /* _DT_ALLOW_DT_MSGS */

static int
_DtActMMCompareObjClassMask( unsigned long objClassMask, char *actClass)
{
	char	buf[_DtAct_MAX_BUF_SIZE];
	char	*bp;
	char	**classVec, **cvp;
	

	if ( ! DtStrchr(actClass, _DtACT_LIST_SEPARATOR_CHAR ) )
	{
		/*
		 * Only a single class is specified
		 */
		if ( !strcmp(actClass,_DtACT_ANY) )
			return True;
		if ( !strcmp(actClass,_DtACTION_FILE) )
                        /* 
                         * Is this object REALLY a file (not a tmp file
			 * generated for a buffer object.
			 */
			if ( IS_FILE_OBJ(objClassMask)
				 && !(IS_BUFFER_OBJ(objClassMask)) )
				return True;
		if ( !strcmp(actClass,_DtACTION_BUFFER) )
			if ( IS_BUFFER_OBJ(objClassMask) )
				return True;
		if ( !strcmp(actClass,_DtACTION_STRING) )
			if ( IS_STRING_OBJ(objClassMask) )
				return True;

		/* no single class spec match found */
		return False;
	}

	/* 
	 * A list of classes was specified in the action. 
	 * Make a local copy of the class string
	 * Then vectorize it in place.
	 */
	strcpy(buf,actClass);
	cvp = classVec = _DtVectorizeInPlace(buf,_DtACT_LIST_SEPARATOR_CHAR);
	
	for ( bp = *cvp; bp; bp = *cvp ) 
	{
		if ( !strcmp(actClass,_DtACT_ANY) )
		{
			XtFree((char *)classVec);
			return True;
		}

		if ( !strcmp(bp,_DtACTION_FILE) )
		{
			if ( IS_ARG_CLASS_FILE(objClassMask) )
			{
				XtFree((char *)classVec);
				return True;
			}
		}
		else if ( !strcmp(bp,_DtACTION_BUFFER) )
		{
			if (IS_ARG_CLASS_BUFFER(objClassMask))
			{
				XtFree((char *)classVec);
				return True;
			}
		}
		else if ( !strcmp(bp,_DtACTION_STRING) )
		{
			if (IS_ARG_CLASS_STRING(objClassMask))
			{
				XtFree((char *)classVec);
				return True;
			}
		}
		cvp++;
	}

	XtFree((char *)classVec);
	return False;
}

static int
_DtActMMCompareMode( unsigned long objMask, char *actMode)
{
        if ( !actMode
             || !strcmp(actMode,_DtACT_ARG_MODE_ANY)
             || !strcmp(actMode,DtDTS_DT_UNKNOWN))
		return True;	/* We don't care about the mode */

	if ( !strcmp(actMode,_DtACT_ARG_MODE_WRITE) && IS_WRITE_OBJ(objMask) )
		return True;

	if ( !strcmp(actMode,_DtACT_ARG_MODE_NOWRITE) 
             && !(IS_WRITE_OBJ(objMask)) )
		return True;

	return False;
}

static int
_DtActMMCompareType( DtShmBoson reqType, char *actType)
{
	char buf[_DtAct_MAX_BUF_SIZE];
	char *reqTypeStr = (char *)_DtDtsMMBosonToString(reqType);
	char **typeVec;
	char **tvp;
	char *tp;

	if ( ! DtStrchr(actType, _DtACT_LIST_SEPARATOR_CHAR ) )
	{
		/*
		 * Only a single type is specified
		 */
		if ( !strcmp(actType,_DtACT_ANY) )
			return True;
		if ( !strcmp(actType,reqTypeStr) )
			return True;

		/* no single type spec match found */
		return False;
	}


	/* 
	 * Make a local copy of the class string
	 * Then vectorize it in place.
	 */
	strcpy(buf,actType);
	tvp = typeVec = _DtVectorizeInPlace(buf,_DtACT_LIST_SEPARATOR_CHAR);

	for ( tp = *tvp; tp; tp = *tvp )
	{
		if ( !strcmp(tp,reqTypeStr) )
		{
			XtFree((char *)typeVec);
			return True;
		}
		tvp++;
	}
	
	return False;
}

static int
_DtActMMCompareCount( int reqCount, char *actCount)
{

	if ( !strcmp(actCount,_DtACT_ANY) )
		return True;

	if ( *actCount == _DtACT_GT_CHAR )
	{
		if ( reqCount > atoi( actCount+1 ))
			return True;
		return False;
	}

	if ( *actCount == _DtACT_LT_CHAR )
	{
		if ( reqCount < atoi( actCount+1 ))
			return True;
		return False;
	}

	if ( reqCount == atoi(actCount) )
		return True;

	return False;
}


/*
 * This function is used when a request is made to locate the first
 * action record meeting the criteria specified with the requestCriteria.
 * The only fields within the requestCriteria which are of interest are
 * the 'signiture' fields (ARG_CLASS, ARG_TYPE and ARG_COUNT).  Of
 * course, the action name is also important.
 * This assumes the action database has already been sorted.
 * and that default ARG_CLASS, ARG_TYPE and ARG_COUNT values have been
 * entered into the database as needed.
 */

DtDtsMMRecord *
_DtActionLocateRecord( 
	DtShmBoson 	actQuark, 
	unsigned long	obj_mask, 
	DtShmBoson	arg_type,
	int  		arg_count, 
	DtDtsMMDatabase	*act_db)
{
	int		n;
	DtDtsMMRecord	*act_rec;
	DtDtsMMRecord	*act_rec_list;
	DtDtsMMRecord	*last_rec_found = NULL;
	char		*tmp = 0;
	int		*start;
	

	myassert (act_db);
	act_rec_list = _DtDtsMMGetPtr(act_db->recordList);

	start =  (int*)_DtDtsMMGetDbName(act_db,actQuark);
	if(!start)
	{
		return(NULL);
	}
	for ( n = *start; n < act_db->recordCount; n++)
	{
		act_rec = &act_rec_list[n];
		/*
		 * compare the name quark field to the action quark
		 */

		if ( act_rec->recordName != actQuark )
			break;

		_DtDtsMMSafeFree(tmp);
		if ( !_DtActMMCompareObjClassMask(obj_mask, 
			tmp =_DtActGetDtsMMField(act_rec,_DtACTION_ARG_CLASS)))
			continue;

		_DtDtsMMSafeFree(tmp);
		if ( !_DtActMMCompareType(arg_type,
			tmp =_DtActGetDtsMMField(act_rec,_DtACTION_ARG_TYPE)))
			continue;
		
		_DtDtsMMSafeFree(tmp);
		if ( !_DtActMMCompareCount(arg_count,
			tmp =_DtActGetDtsMMField(act_rec,_DtACTION_ARG_COUNT)))
			continue;

  		_DtDtsMMSafeFree(tmp);
		if ( !_DtActMMCompareMode(obj_mask,
			tmp =_DtActGetDtsMMField(act_rec,_DtACTION_ARG_MODE)))
			continue;
		_DtDtsMMSafeFree(tmp);
		/*
		 * We've found a match 
		 */

		return act_rec;
	}

	_DtDtsMMSafeFree(tmp);
	/*
	 * No match found
	 */
	return NULL;
}


/***************************************************************************/
/***************************************************************************/
/*                        Message Parsing Functions                        */
/***************************************************************************/
/***************************************************************************/


/*
 * Given pointers to the beginning and end of a possible keyword, see if
 * the string matches any of the known keywords.  Return TRUE if a match
 * is found.  The format for a keyword is:
 *
 *       <Optional Qualifier><Keyword><Optional Prompt>
 *
 * The string passed in already has the leading and trailing `%' removed.
 */

static Boolean 
ValidKeyword(
        char *start,
        char *end,
        char **prompt,
        int *keywordId,
        int *argNum,
        Boolean *isFile )

{
    /* 
     * Fill in the length of the keywords at compile time by
     * using the "sizeof" operator (and subtracting one for the NULL).
     */
   static  int localHostLen = sizeof(_DtACT_LOCALHOST_STR) - 1;
   static  int databaseHostLen = sizeof(_DtACT_DATABASEHOST_STR) - 1;
   static  int displayHostLen = sizeof(_DtACT_DISPLAYHOST_STR) - 1;
   static  int sessionHostLen = sizeof(_DtACT_SESSIONHOST_STR) - 1;
   static  int argsLen = sizeof(_DtACT_ARGS_STR) - 1;
   static  int arg_Len = sizeof(_DtACT_ARG_UNDER_STR) - 1;
   static  int stringQualifierLen = sizeof(_DtACT_STRING_QUALIFIER) - 1;
   static  int fileQualifierLen = sizeof(_DtACT_FILE_QUALIFIER) - 1;
   int len = end - start + 1;
   char *stop;
   char *lastCh;
   int lastChLen;
   char savedChar;
   Boolean FoundQualifier=False;


   *keywordId = NO_KEYWORD;
   *argNum = NO_ARG;


   /*
    * Determine if an optional qualifier was specified.  For now, the
    * only qualifiers we know about are "(String)" and "(File)", which 
    * are only valid for the "Args" , "Arg_<n>" or "Prompt" keywords.
    * Is no qualifier is specified we default to String.
    */
   if ((len >= stringQualifierLen) && 
       (strncmp(_DtACT_STRING_QUALIFIER,start,(size_t)stringQualifierLen) == 0))
   {
      FoundQualifier=True;
      *isFile = False;
      start += stringQualifierLen;
   } else if (( len >= fileQualifierLen) &&
       (strncmp(_DtACT_FILE_QUALIFIER, start,(size_t)fileQualifierLen) == 0))
   {
        FoundQualifier=True;
	*isFile = True;
        start += fileQualifierLen;
   } else
      *isFile = False;	/* default to string behavior (except for Arg*) */


   /* Compare the first portion of the string to each known keyword */
   if ((len >= localHostLen) && 
       (strncmp(_DtACT_LOCALHOST_STR, start, (size_t)localHostLen) == 0))
   {
      *isFile = False;

      /* Flag that a keyword was found */
      *keywordId = LOCAL_HOST;
      *argNum = NO_ARG;

      start += localHostLen;
   }
   else if ((len >= databaseHostLen) && 
            (strncmp(_DtACT_DATABASEHOST_STR,start,(size_t)databaseHostLen)==0))
   {
      *isFile = False;

      /* Flag that a keyword was found */
      *keywordId = DATABASE_HOST;
      *argNum = NO_ARG;

      start += databaseHostLen;
   }
   else if ((len >= displayHostLen) && 
            (strncmp(_DtACT_DISPLAYHOST_STR,start,(size_t)displayHostLen)==0))
   {
      *isFile = False;

      /* Flag that a keyword was found */
      *keywordId = DISPLAY_HOST;
      *argNum = NO_ARG;

      start += displayHostLen;
   }
   else if ((len >= sessionHostLen) && 
            (strncmp(_DtACT_SESSIONHOST_STR,start,(size_t)sessionHostLen)==0))
   {
      *isFile = False;

      /* Flag that a keyword was found */
      *keywordId = SESSION_HOST;
      *argNum = NO_ARG;

      start += sessionHostLen;
   }
   else if ((len >= argsLen) && 
	(strncmp(_DtACT_ARGS_STR, start, (size_t)argsLen) == 0))
   {
      /* Flag that a keyword was found */
      *keywordId = ARG;
      *argNum = ALL_ARGS;
	/*
	 * Args keyword should default to File unless
	 * a String qualifier was encountered.
	 */
      if ( !FoundQualifier )
         *isFile=True;

      start += argsLen;
   }
   else if ((len >= arg_Len) &&
	 (strncmp(_DtACT_ARG_UNDER_STR, start, (size_t)arg_Len) == 0))
   {
      /* Flag that a keyword was found */
      *keywordId = ARG;
	/*
	 * Args keyword should default to File unless
	 * a String qualifier was encountered.
	 */
      if ( !FoundQualifier )
         *isFile=True;

      /* Determine which argument is to be used */
      start += arg_Len;
      *argNum = strtol(start, &stop, 10);

      if (*argNum > 0)
         start = stop;
      else
      {
         /* Keyword was invalidly formed */
         return(False);
      }
   }


   /* 
    * See if this is the end of the keyword, or whether a prompt is
    * there also.
    */
   if (start > end)
   {
      if (*keywordId != NO_KEYWORD)
      {
         /* All done */
         *prompt = NULL;
         return(True);
      }
      else
      {
         /* We never found a keyword */
         return(False);
      }
   }


   /*
    * We've may have extracted a keyword, and the string still contains
    * more characters; let's see if an optional prompt was specified.  If
    * the next character is a '"', then the last character in the string
    * must also be a '"'; otherwise, the keyword was invalid.  If the next
    * character is not a '"', then the keyword is invalid.
    */
   savedChar = *DtNextChar(end);
   lastCh = DtPrevChar(start, end+1);
   lastChLen = mblen(lastCh, MB_LEN_MAX);

   if ((start != end) && (*start == '"') && (lastChLen == 1) &&(*lastCh == '"'))
   {
      /* Valid prompt found; skip the quotes */
      start++;
      *prompt = (char *)XtMalloc((Cardinal)(end - start + 1));
      (void)strncpy(*prompt, start, (size_t)(end - start));
      (*prompt)[end - start] = '\0';
      return(True);
   }


   /* If we made it down here, then the keyword was invalid */
   return(False);
}

ActionPtr
_DtActionFindDBEntry(	ActionRequest *reqp,
			DtShmBoson actQuark )
{
	int numSearches = 0;
	DtDtsMMRecord	*actRecp;
	char		*mapto = NULL;
	long		mask = 0;
	DtShmBoson	type = 0;
	int		num = 0;
	ActionPtr	actp = NULL;
        char            *origLabel = NULL;
	char		*tmp = 0;
	DtDtsMMDatabase	*act_db;

	_DtSvcProcessLock();     
	act_db = _DtDtsMMGet(_DtACTION_NAME);
	if ( reqp && reqp->numObjects && reqp->objects )
	{
		mask = reqp->objects[0].mask;
		type = reqp->objects[0].type;
		num  = reqp->numObjects;
	} else	/* Treat missing objects as writable */
		SET_WRITE_OBJ(mask);
	

	/*
	 * Get the action record structure from the action database
	 */

	for (actRecp = _DtActionLocateRecord(actQuark,mask,type,num, act_db);
		actRecp;
		actRecp = _DtActionLocateRecord(_DtDtsMMStringToBoson(mapto),
				mask,type,num, act_db), numSearches++)
	{
		/*
		 * Check to see if this is a map action
		 */
		if(!(mapto=_DtActGetDtsMMField(actRecp,_DtACTION_MAP_ACTION)))
			break;

                /*
                 * Save the label/name of the original action for displays.
                 */
                if ( !origLabel )
			origLabel =  _DtActMMChooseLabel(actRecp);

		if ( numSearches++ > _MAX_MAP_ATTEMPTS ) 
		{
			if ( reqp )
				SET_TOO_MANY_MAPS(reqp->mask);
			_DtSvcProcessUnlock();
			return NULL;
		}
	}
	
	
	/*
	 * Fill in Action structure
	 *   -- do some parsing here.
	 */
	if ( actRecp )
	{
		actp = (Action *)XtCalloc(1,sizeof(Action));

		actp->action = actRecp->recordName;	/* save action quark */

		/* pathId is boson; file_name_id needs to be quark! */
		actp->file_name_id =
		  XrmStringToQuark(_DtDtsMMBosonToString(actRecp->pathId));
                if ( origLabel )
			actp->label = origLabel;
                else
			actp->label = _DtActMMChooseLabel(actRecp);
		actp->description = _DtActGetDtsMMField(actRecp,
			_DtACTION_DESCRIPTION);

		actp->mask |= _DtActMMParseClassList(
			tmp =_DtActGetDtsMMField(actRecp,_DtACTION_ARG_CLASS));
		_DtDtsMMSafeFree(tmp);

		actp->mask |= _DtActMMParseArgTypesList(
			tmp =_DtActGetDtsMMField(actRecp,_DtACTION_ARG_TYPE),
			&actp->arg_types, &actp->type_count );
		_DtDtsMMSafeFree(tmp);

		actp->mask |= _DtActMMParseArgCountString(
			tmp =_DtActGetDtsMMField(actRecp,_DtACTION_ARG_COUNT),
			&actp->arg_count);
		_DtDtsMMSafeFree(tmp);

		actp->mask |= _DtActMMParseActionType(
			tmp =_DtActGetDtsMMField(actRecp,_DtACTION_TYPE));
		_DtDtsMMSafeFree(tmp);

		switch ( actp->mask & _DtAct_ACTION_TYPE_BITS )
		{
			case _DtAct_CMD_BIT:
				_DtActMMGetCmdInfo(actRecp,actp);
				break;
			case _DtAct_TT_MSG_BIT:
				_DtActMMGetTtMsgInfo(actRecp,actp);
				break;
#ifdef _DT_ALLOW_DT_MSGS
			case _DtAct_DT_REQ_MSG_BIT:
				_DtActMMGetDtReqInfo(actRecp,actp);
				break;
			case _DtAct_DT_NTFY_MSG_BIT:
				_DtActMMGetDtNtfyInfo(actRecp,actp);
				break;
#endif /* _DT_ALLOW_DT_MSGS */
			case _DtAct_MAP_BIT:
				myassert(0);	/* should never get here */
				_DtFreeActionStruct(actp);
				_DtSvcProcessUnlock();
				return NULL;
				break;
			default:	/* unkown action type */
				myassert(0);	/* should never get here */
				_DtFreeActionStruct(actp);
				_DtSvcProcessUnlock();
				return NULL;
				break;
		}

	}

	/*
	 * actRecp is NULL -- therefore we were unable to locate an action.
	 * It may be that the action we could not locate was a mapped action.
	 * To give the user as much information as possible about the 
	 * problem, put the name of the mapped action (if any) into the
	 * request's action name field -- this information will later be
	 * displayed in an error dialog.
	 */
	if ( mapto )
	{
		XtFree(reqp->actionName);
		reqp->actionName = mapto;
	}
			
	_DtSvcProcessUnlock();
	return actp;
}

/*
 * Take a message string, and break it up into its components.  A component
 * is composed of preceding text, a keyword and an optional prompt.  The
 * parsing will make our job easier when it comes time to substitute in
 * the supplied set of parameters.
 *
 * During the parse operation, `string' always points to the beginning
 * of where the last segment ended, and the next segment starts.  'start'
 * always points to where within `string' the search for a keyword should
 * start.
 */

static void 
ParseMessage(
        register ActionPtr action,
        register char * str,
        register parsedMsg * msg )

{
   char *start; 
   register char *string;
   char *keywordStart, *keywordEnd;
   char *prompt;
   int keyword, argNum;
   Boolean isFile;
   register MsgComponent *newBlock;

   /* Initialize all fields */
   msg->parsedMessage = NULL;
   msg->numMsgParts = 0;
   msg->compiledMessage = NULL;

   /* Make sure there is a message to parse */
   if ((start = string = str) == NULL)
      return;

/* 
 * This would be a good place to allow popen invocations 
 * for load balancing, etc. (See expand_shell() in Dts.c)
 */

   /* Parse out each keyword */
   while (1)
   {
      /* Look for the start and end of a keyword */
      if ((keywordStart = DtStrchr(start, '%')))
         keywordEnd = DtStrchr(keywordStart+1, '%');
      else
         keywordEnd = NULL;

      /* See if a keyword was found */
      if ((keywordStart == NULL) || (keywordEnd == NULL))
      {
         /*
          * No more keywords left; bundle the remaining string into
          * a parse block, and return.
          */
         if (strlen(string) > 0)
         {
            msg->numMsgParts++;
            msg->parsedMessage = (MsgComponent *)
                XtRealloc((char *)msg->parsedMessage, 
                      (Cardinal)(sizeof(MsgComponent) * msg->numMsgParts));

            newBlock = msg->parsedMessage + (msg->numMsgParts - 1);
            newBlock->precedingText = XtNewString(string);
            newBlock->prompt = NULL;
            newBlock->keyword = NO_KEYWORD;
            newBlock->argNum = NO_ARG;
            newBlock->mask = 0;
         }

         /* Free up the old msg string; we no longer need it */
         return;
      }
      else
      {
         /*
          * We've encountered what looks to be a keyword; see if it is
          * a valid one.  If it's not valid, then continue looking; 
          * otherwise, save it, and continue the parsing processing.
          */
         if (ValidKeyword(keywordStart+1, keywordEnd-1, &prompt,
                          &keyword, &argNum, &isFile))
         {
            msg->numMsgParts++;
            msg->parsedMessage = (MsgComponent *)
                XtRealloc((char *)msg->parsedMessage, 
                       (Cardinal)(sizeof(MsgComponent) * msg->numMsgParts));

            newBlock = msg->parsedMessage + (msg->numMsgParts - 1);
            if (keywordStart != string)
            {
               /* Extract what we need; convert to NULL terminated */
               newBlock->precedingText = (char *)
                      XtMalloc((Cardinal)(keywordStart - string + 1));
               (void)strncpy(newBlock->precedingText, string, 
                             (size_t)(keywordStart-string));
               newBlock->precedingText[keywordStart - string] = '\0';
            }
            else
               newBlock->precedingText = NULL;

            newBlock->prompt = prompt;
            newBlock->keyword = keyword;
            newBlock->argNum = argNum;
            newBlock->mask = 0;
            if (isFile)
               SET_TREAT_AS_FILE(newBlock->mask);

            if (keyword == ARG)
            {
               /* Are we a single parameter action? */
               if (argNum == 1)
               {
                  if (IS_ARG_NONE_FOUND(action->mask))
                     SET_ARG_SINGLE_ARG(action->mask);
               }
               else
                  SET_ARG_MULTI_ARG(action->mask);
            }

            /* Continue the search following this keyword */
            string = start = keywordEnd + 1;
         }
         else
         {
            /* Invalid keyword; keep searching */
            start = keywordEnd;
         }
      }
   }
}
