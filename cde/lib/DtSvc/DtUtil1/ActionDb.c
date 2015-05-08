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
/* $TOG: ActionDb.c /main/8 1998/07/30 12:09:13 mgreess $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:         ActionDb.c
 **
 **   Project:	    DT
 **
 **   Description:  Source file for the action database loading functions.
 **		  
 **
 **
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/

/*LINTLIBRARY*/
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <X11/Intrinsic.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>

#include <Dt/DtP.h>
#include <Dt/Message.h>
#include <Dt/DbReader.h>
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/UserMsg.h>
#include <Dt/CommandM.h>

#include <Dt/ActionUtilP.h>
#include <Dt/ActionP.h>
#include <Dt/ActionDbP.h>

#include <Dt/Utility.h>
#include <Dt/DtsDb.h>
#include <Dt/Dts.h>

#include "myassertP.h"
#include "DtSvcLock.h"

/********    Global Function Declarations    ********/

extern Boolean _DtEmptyString( 
                        String str) ;

/********    End Private Function Declarations    ********/


/********    Static Function Declarations    ********/

static void InitializeLocalizedStrings ( void ) ;

/********    End Static Function Declarations    ********/

/*
 * Pointers to the localized strings.
 */

static char *noFields;
static char *unrecogField;
static char *unrecogType;
static char *noMapTo;
static char *noExecString;
static char *unrecogMsgType;
static char *dupFields;
static char *invldField;
static char *missingField;
static char *missingName;
static char *unrecogMsgType2;


/*****************************************************************************
 *****************************************************************************
 *
 *	Public API Functions
 *
 *****************************************************************************
 *****************************************************************************/

/* none here */

/******************************************************************************
 ******************************************************************************
 *
 *	Private API Functions
 *
 ******************************************************************************
 ******************************************************************************/

/******************************************************************************
 *	
 *	String Utilities
 *
 ******************************************************************************/

/*
 * See if two strings are exactly the same.
 */

Boolean 
_DtStringsAreEqual(
        String string1,
        String string2 )
{
   if ((strcmp(string1, string2) == 0) && (strlen(string1) == strlen(string2)))
      return(True);

   return(False);
}


/*
 * Return TRUE if the string pointer is NULL, or if the string is empty.
 * Empty is also defined to be nothing but spaces.
 */

Boolean 
_DtEmptyString(
        String str )
{
   if ((str == NULL)||(strlen(str) == 0)||(strlen(str) == DtStrspn(str, " ")))
      return(True);

   return(False);
}

typedef struct {
	char *name;
	unsigned long bit;
} _DtActNameAndBit; 


#ifdef _DT_ALLOW_DT_MSGS
/*
 * The "DT_ARGn_VALUE" and "TT_ARGn_VALUE" field names are represented by
 * macros (_DtACTION_DTN_VALUE, _DtACTION_TTN_VALUE) which define identical
 * strings "_VALUE". (The field names minus the "[DT]_ARGn" prefix.)
 *
 * To allow an unambiguous table lookup by field name we add the
 * _DtPFX prefix macro to the "DT_ARGn" field name. 
 */
#define _DtPFX	"DT_"
#endif	/* _DT_ALLOW_DT_MSGS */

/*
 * List all the action record fields we care about
 * along with the bitmask that represents the field.
 */
static _DtActNameAndBit _DtActNamesAndBits[] = {
	{ _DtACTION_LABEL,		_ActDb_LABEL_SET, },
	{ _DtACTION_TYPE,		_ActDb_TYPE_SET, },
	{ _DtACTION_ARG_CLASS,		_ActDb_ARG_CLASS_SET, },
        { _DtACTION_ARG_MODE,		_ActDb_ARG_MODE_SET, },
	{ _DtACTION_ARG_TYPE,		_ActDb_ARG_TYPE_SET, },
	{ _DtACTION_ARG_COUNT,		_ActDb_ARG_COUNT_SET, },
	{ _DtACTION_DESCRIPTION,	_ActDb_DESCRIPTION_SET, },
	{ _DtACTION_ICON,		_ActDb_ICON_SET, },
/*******************************************************************
Meaningless for actions -- ignore these for now
	{ _DtACTION_INSTANCE_ICON,	_ActDb_INSTANCE_ICON_SET, },
*******************************************************************/
	{ _DtACTION_MAP_ACTION,		_ActDb_MAP_ACTION_SET, },
	{ _DtACTION_EXEC_STRING,	_ActDb_EXEC_STRING_SET, },
	{ _DtACTION_EXEC_HOST,		_ActDb_EXEC_HOST_SET, },
	{ _DtACTION_CWD,		_ActDb_CWD_SET, },
	{ _DtACTION_WINDOW_TYPE,	_ActDb_WINDOW_TYPE_SET, },
	{ _DtACTION_TERM_OPTS,		_ActDb_TERM_OPTS_SET, },
	{ _DtACTION_TT_CLASS,		_ActDb_TT_CLASS_SET, },
	{ _DtACTION_TT_SCOPE,		_ActDb_TT_SCOPE_SET, },
	{ _DtACTION_TT_OPERATION,	_ActDb_TT_OPERATION_SET, },
	{ _DtACTION_TT_FILE,		_ActDb_TT_FILE_SET, },
	{ _DtACTION_TTN_MODE,		_ActDb_TT_ARGN_MODE_SET, },
	{ _DtACTION_TTN_VTYPE,		_ActDb_TT_ARGN_VTYP_SET, },
	{ _DtACTION_TTN_REP_TYPE,	_ActDb_TT_ARGN_RTYP_SET, },
	{ _DtACTION_TTN_VALUE,		_ActDb_TT_ARGN_VAL_SET, },

#ifdef _DT_ALLOW_DT_MSGS
	{ _DtACTION_DT_REQUEST_NAME,	_ActDb_DT_REQ_NAME_SET, },
	{ _DtACTION_DT_SVC,		_ActDb_DT_SVC_SET, },
	{ _DtACTION_DT_NOTIFY_NAME,	_ActDb_DT_NTFY_NAME_SET, },
	{ _DtACTION_DT_NGROUP,		_ActDb_DT_NGROUP_SET, },
	{ _DtPFX _DtACTION_DTN_VALUE,	_ActDb_DT_ARGN_VAL_SET, },
#endif	/* _DT_ALLOW_DT_MSGS */
	{ NULL,				0 },
};

#define	NUM_FIELD_NAMES		sizeof(_DtActNamesAndBits)/sizeof(_DtActNameAndBit) - 1

/******************************************************************************
 *
 * _DtActFieldBitLookup
 *	Lookup the mask value associated with the given field name in the array
 *	of names and field bits.  If the field name is not recognized return the
 *      default value associated with the NULL field name  (i.e. 0 ).
 *
 ******************************************************************************/
 
static
unsigned long
_DtActFieldBitLookup(char *name)
{
	register int j;
	char *np = name;
#ifdef _DT_ALLOW_DT_MSGS
	char dtnamebuf[sizeof(_DtPFX) + sizeof(_DtACTION_DTN_VALUE)] = _DtPFX;
#endif	/* _DT_ALLOW_DT_MSGS */

	if ( !np )
		return 0;
	/*
	 * Check for [DT]T_ARGn_ fields
	 * The following code assumes that a unique suffix identifying each
	 * field results from stripping off the leading "[DT]_ARGn" string.
	 * This assumption is not valid for DT_ARGn_VALUE and TT_ARGn_VALUE.
	 * so we resort to further skulduggery.
	 */
	if ( !strncmp("TT_ARG",np,sizeof("TT_ARG")-1) )
	{
		np = np + sizeof("TT_ARG"); /* first char beyond prefix */
		while( *np && (mblen(np,MB_CUR_MAX) == 1) && DtIsdigit(np) )  
                    np=DtNextChar(np);  /* skip past digits */
	} 
#ifdef	_DT_ALLOW_DT_MSGS
	else if ( !strncmp("DT_ARG",np,sizeof("DT_ARG")-1) )
	{
		np = np + sizeof("DT_ARG"); /* first char beyond prefix */
		while( *np && (mblen(np,MB_CUR_MAX) == 1) && DtIsdigit(np) ) 
                       np=DtNextChar(np);  /* skip past digits */

		/*
		 * Add the prefix to disambiguate the field name for
		 * the table lookup.
		 */
		np=strcat(dtnamebuf,np);
		

	}
#endif	/* _DT_ALLOW_DT_MSGS */

	if ( !np )
		return 0;

	for ( j = 0; j < NUM_FIELD_NAMES; j++ )
		if ( !strcmp(_DtActNamesAndBits[j].name,np) )
			break;	/* found matching field name */

	return _DtActNamesAndBits[j].bit;
}

/******************************************************************************
 *
 * _DtActFieldNameLookup
 *	Lookup the name string associated with the given mask bit in the array
 *	of names and field bits.  If the mask bit is not recognized return the
 *      default value associated with the NULL field name  (i.e. 0 ).
 *
 ******************************************************************************/
 
static char *
_DtActFieldNameLookup(long bitmask)
{
	register int j;

	if ( !bitmask )
		return NULL;

	for ( j = 0; j < NUM_FIELD_NAMES; j++ )
		if ( _DtActNamesAndBits[j].bit == bitmask )
			break;	/* found matching field name */

	return _DtActNamesAndBits[j].name;
}


/******************************************************************************
 *
 * _DtActDupFieldNameCheck
 *	return True if "name" duplicates an existing fieldName.
 *
 ******************************************************************************/
static Boolean
_DtActDupFieldNameCheck(DtDtsDbField **fields,int numFields,XrmQuark name)
{
	register int i;

	for ( i = 0; i < numFields; i++ ) 
	{
		if (fields[i]->fieldName == name)
			return True;	
	}

	return False;
}


/******************************************************************************
 *
 * _DtActValidateFieldValue 
 *	Given a bitmask and action database field value  apply some heuristics
 *	to determine the validity of the value.  Return a newly allocated 
 *	"sanitized" value for valid fields,  NULL otherwise.
 *
 ******************************************************************************/
static char *
_DtActValidateFieldValue( long bit, char *value, char *actName, char *filename)
{
	char *val;
	char *start;
	char *p = NULL;
	char bigBuf[_DtAct_MAX_BUF_SIZE];
	char *buf = bigBuf;

	if (!(value && *value))
	{
		/*
		 * Empty field value -- ignore
		 */
		return NULL;
	}

	/* work on a new copy of the value */
	val =	strdup( value );

	/*
	 * Strip trailing blanks from all fields except EXEC_STRING and
	 * DESCRIPTION.  The database reader should guarantee that leading
	 * blanks have been stripped from the value field.
	 */
	if ( !(bit & ( _ActDb_EXEC_STRING_SET | _ActDb_DESCRIPTION_SET )))
		_DtRemoveTrailingBlanksInPlace(&val);
	
	switch ( bit )
	{
	case _ActDb_TYPE_SET:
		if (    strcmp(_DtACTION_MAP,val)
			&& strcmp(_DtACTION_COMMAND,val)
			&& strcmp(_DtACTION_TT_MSG,val)
#ifdef _DT_ALLOW_DT_MSGS
			&& strcmp(_DtACTION_DT_REQUEST,val)
			&& strcmp(_DtACTION_DT_NOTIFY,val)  
#endif	/* _DT_ALLOW_DT_MSGS */
                         )
		{
			/*
			 * Invalid field value -- issue error msg.
			 */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType, actName, filename,
					val, _DtACTION_TYPE);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if ( val ) free(val);
			if (buf) free(buf);
			return NULL;
		}
		break;
	case _ActDb_EXEC_HOST_SET:
		/*
		 * Do NOT add to an existing EXEC_HOST definition.
		 */
		break;
	case _ActDb_ARG_CLASS_SET:

		/* Check the list of classes */
		p = NULL;
		start = val;
		while ( (p = DtStrchr(start,_DtACT_LIST_SEPARATOR_CHAR)) )
		{
			/*
			 * temporarily truncate string for testing
			 */
			*p = '\0';
			if (    strcmp(_DtACT_ANY,start)          &&
				strcmp(_DtACTION_FILE,start) 	&& 
				strcmp(_DtACTION_BUFFER,start)   )
			{
				/*
				 * Invalid field value -- issue error msg.
				 */
				buf = malloc(_DtAct_MAX_BUF_SIZE);
				(void) sprintf(buf, unrecogMsgType, actName, filename,
						start, _DtACTION_ARG_CLASS);
				_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
				if ( val ) free(val);
				if (buf) free(buf);
				return NULL;
			}
			*p = _DtACT_LIST_SEPARATOR_CHAR;
			start = p+1;
		} 
		if (    strcmp(_DtACT_ANY,start)          &&
			strcmp(_DtACTION_FILE,start) 	&& 
			strcmp(_DtACTION_BUFFER,start)   )
		{
			/*
			 * Invalid field value -- issue error msg.
			 */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType, actName, filename,
					start, _DtACTION_ARG_CLASS);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if ( val ) free(val);
			if (buf) free(buf);
			return NULL;
		}
		break;
        case _ActDb_ARG_MODE_SET:
		/* Check for the valid values of ARG_MODE */
                if (    strcmp(val,_DtACT_ANY)
                     && strcmp(val, _DtACT_ARG_MODE_WRITE)
                     && strcmp(val, _DtACT_ARG_MODE_NOWRITE) )
                {
			/* Invalid Value for ARG_MODE */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType, actName, filename,
					val, _DtACTION_ARG_MODE);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if ( val ) free(val);
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			return NULL;
                }
		break;	/* valid value */
	case _ActDb_ARG_TYPE_SET:
		/* We cannot yet check arg types the database is not complete */
		break;
	case _ActDb_ARG_COUNT_SET:
		if ( !strcmp(val,_DtACT_ANY) )
			break;
		for (p=val; *p; p=DtNextChar(p)) 
		{
			if ( p == val && ((*p == _DtACT_LT_CHAR) 
                             || (*p == _DtACT_GT_CHAR))) 
			{
				/* skip past "> or <" modifiers */
				p=DtNextChar(p);
			}
			if ( !(mblen(p,MB_CUR_MAX) == 1 && DtIsdigit(p)) )
			{
				/*
				 * Report Error -- invalid field
				 */
				buf = malloc(_DtAct_MAX_BUF_SIZE);
				(void) sprintf(buf, unrecogMsgType, actName,
						filename, val,
						_DtACTION_ARG_COUNT);
				_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
				if ( val ) free(val);
				if (buf) free(buf);
				return NULL;
			}
		}
		break;
	case _ActDb_WINDOW_TYPE_SET:
		if (    strcmp(_DtACTION_TERMINAL,val) 		&& 
			strcmp(_DtACTION_PERM_TERMINAL,val)	&&
			strcmp(_DtACTION_NO_STDIO,val)   )
		{
			/*
			 * Invalid field value -- issue error msg.
			 */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType, actName, filename,
					val, _DtACTION_WINDOW_TYPE);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if (val) free(val);
			if (buf) free(buf);
			return NULL;
		}
		break;
	case _ActDb_CWD_SET:
		/*
		 * This field must be in [host:]/path format
		 *
		 * WE REQUIRE A FULL PATH NAME TO BE SPECIFIED FOR THE
		 * DIRECTORY OTHERWISE WE CANNOT DISTINGUISH BETWEEN A
		 * RELATIVE FILE ENDING IN ':' AND A HOSTNAME FOLLOWED
		 * BY A ':'.
		 */
		if ( *val == '/' 
                     && !( (mblen(val,MB_CUR_MAX) == 1) 
                            && DtIsspace(DtNextChar(val))) )
			break;	/* Accept as good with no hostname */

		for ( p = val; *p ; p=DtNextChar(p) ) 
		{

			/*
			 * Accept as a valid hostname as long as 
			 * there is a string of non-white
			 * and non-slash characters before the first
			 * colon and the first colon is immediately
			 * followed by a slash.
			 */
			if ( (*p == ':' )  
                             && ( *(DtNextChar(p)) == '/' ) && ( p != val) )
				break;  /* Accept this as good with hostname */


			if ( (*p == '/') 
                              || ( (mblen(p,MB_CUR_MAX) == 1) && DtIsspace(p)))
			{
				/* looks like an error */
				buf = malloc(_DtAct_MAX_BUF_SIZE);
				(void) sprintf(buf, unrecogMsgType, actName,
						filename, val, _DtACTION_CWD);
				_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
				if ( val ) free(val);
				if (buf) free(buf);
				/*********************
				  reject the entire record containing this field
				*********************/
				return (char *) -1;
			}
		}
		if (*p == '\0')
		{
			/* looks like an error */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType, actName,
					filename, val, _DtACTION_CWD);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if (val) free(val);
			if (buf) free(buf);
			/*********************
			  reject the entire record containing this field
			*********************/
			return (char *) -1;
		}
		break;
	case _ActDb_TT_CLASS_SET:
		if (    strcmp(_DtACTION_TT_NOTICE,val)	&& 
			strcmp(_DtACTION_TT_REQUEST,val)   )
		{
			/*
			 * Invalid field value -- issue error msg.
			 */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType, actName, filename,
					val, _DtACTION_TT_CLASS);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if ( val ) free(val);
			if (buf) free(buf);
			return NULL;
		}
		break;
	case _ActDb_TT_SCOPE_SET:
		if (    strcmp(_DtACTION_TT_SESSION,val) 	&& 
			strcmp(_DtACTION_TT_FILE,val)		&&
			strcmp(_DtACTION_TT_BOTH,val)		&&
			strcmp(_DtACTION_TT_FILE_IN_SESSION,val)  )
		{
			/*
			 * Invalid field value -- issue error msg.
			 */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType, actName, filename,
					val, _DtACTION_TT_SCOPE);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if ( val ) free(val);
			if (buf) free(buf);
			return NULL;
		}
		break;
	case _ActDb_TT_FILE_SET:
		/*
		 * Must be a real file (host:/syntax?) 
		 * -- convert to network file syntax?
		 * or %ARG_n% keyword. 
		 * NOTE: only one file may be specified
		 */ 
		break;
	case _ActDb_TT_ARGN_MODE_SET:
		/*
		 * MODE is required for tooltalk actions
		 * it must be one of TT_IN, TT_OUT or TT_INOUT
		 */
		if (    strcmp(_DtACTION_TT_MODE_IN,val ) &&
			strcmp(_DtACTION_TT_MODE_OUT,val) &&
			strcmp(_DtACTION_TT_MODE_INOUT,val) )
		{
			/*
			 * Invalid field value -- issue error msg.
			 */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType2, actName, filename,
					val, _DtACTION_TTN_ARG,
					"n", _DtACTION_TTN_MODE);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if ( val ) free(val);
			if (buf) free(buf);
			return NULL;
		}
		break;
	case _ActDb_TT_ARGN_VTYP_SET:
		/*
		 * VTYPE -- semantic type name (i.e. title) applied to tooltalk
		 * message value (TT_ARGn_VALUE).  The value of this field is
		 * either an arbitrary string, or an action argument reference
		 * (i.e. %Arg_n%).
		 */
		break;
	case _ActDb_TT_ARGN_RTYP_SET:
		/*
		 * REP_TYPE -- representation type of the corresponding tooltalk value.
		 * Valid values are: UNDEFINED, INTEGER, BUFFER, and STRING.
		 */
		if (    strcmp(_DtACTION_TT_RTYP_UND,val ) &&
			strcmp(_DtACTION_TT_RTYP_INT,val) &&
			strcmp(_DtACTION_TT_RTYP_BUF,val) &&
			strcmp(_DtACTION_TT_RTYP_STR,val) )
		{
			/*
			 * Invalid field value -- issue error msg.
			 */
			buf = malloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf(buf, unrecogMsgType2, actName, filename,
					val, _DtACTION_TTN_ARG,
					"n", _DtACTION_TTN_REP_TYPE);
			_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
			if ( val ) free(val);
			if (buf) free(buf);
			return NULL;
		}
		break;
	case _ActDb_TT_ARGN_VAL_SET:
		/*
		 * The value for the Nth tooltalk argument.  This field may be
		 * an arbitrary string.
		 */
		break;
#ifdef	_DT_ALLOW_DT_MSGS
	case _ActDb_DT_REQ_NAME_SET:
		break;
	case _ActDb_DT_NTFY_NAME_SET:
		break;
#endif  /* _DT_ALLOW_DT_MSGS */
	default:
	 	break;
	}

	return val;
}

static char *
_DtActCheckRecord( 
	DtDtsDbRecord *actp, 
	char	*actionType,
	unsigned long mask, 
	char *fileName  )
{
	int 	i;
	char	*s;
	char argnFieldName[
		sizeof(_DtACTION_TTN_ARG) +  	 /* ARGn prefix */
		3 +				/* space for 3 decimal digits */
		sizeof(_DtACTION_TTN_REP_TYPE)]; /* space for longest suffix */
	char *buf;

	myassert(actionType);	/* actionType should never be NULL */

	/*
	 * Check action type information for accuracy/completeness.
	 */
	if ( !strcmp(actionType,_DtACTION_COMMAND) )
	{
		/*
		 * Check that all recognized fields are suitable for
		 * COMMAND type actions.
		 */
		if ( mask & ( 
			_ActDb_TT_BITS 
#ifdef _DT_ALLOW_DT_MSGS
                        |  _ActDb_DT_REQUEST_BITS 
			|  _ActDb_DT_NOTIFY_BITS
#endif	/* _DT_ALLOW_DT_MSGS */
			| _ActDb_MAP_BITS ))
		{
			/*
			 * Fields unsuitable for COMMAND type actions were 
			 * found. -- reject this record.
			 */
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, invldField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_COMMAND );
			return buf;
		}

		/*
		 * Check that all required fields are present or that
		 * the appropriate default has been set.
		 */
		if ( !(mask & _ActDb_EXEC_STRING_SET) )
		{
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, noExecString, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_EXEC_STRING);
			return buf;
		}
	}
	else if ( !strcmp(actionType,_DtACTION_MAP) )
	{
		/*
		 * Check that all recognized fields are suitable for
		 * MAP type actions.
		 */
		if ( mask & ( 
			 _ActDb_TT_BITS 
#ifdef _DT_ALLOW_DT_MSGS
                        |  _ActDb_DT_REQUEST_BITS 
			| _ActDb_DT_NOTIFY_BITS
#endif	/* _DT_ALLOW_DT_MSGS */
			| _ActDb_CMD_BITS ))
		{
			/*
			 * Fields unsuitable for MAP type actions were 
			 * found. -- reject this record.
			 */
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, invldField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_MAP );
			return buf;
		}
		/*
		 * Check for required fields
		 */
		if ( !(mask & _ActDb_MAP_ACTION_SET) )
		{
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, missingField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_MAP_ACTION, _DtACTION_MAP);
			return buf;
		}

	}
	else if ( !strcmp(actionType,_DtACTION_TT_MSG) )
	{
		/*
		 * Check that all recognized fields are suitable for
		 * TT_MSG type actions.
		 */
		if ( mask & (
			  _ActDb_CMD_BITS 
#ifdef _DT_ALLOW_DT_MSGS
			| _ActDb_DT_NOTIFY_BITS
			| _ActDb_DT_REQUEST_BITS 
#endif	/* _DT_ALLOW_DT_MSGS */
			| _ActDb_MAP_BITS ))
		{
			/*
			 * Fields unsuitable for TT_MSG type actions were 
			 * found. -- reject this record.
			 */
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, invldField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_TT_MSG );
			return buf;
		}

		/*
		 * Insure that all the required fields are present.
		 */
		if ( !(mask & _ActDb_TT_CLASS_SET) )
		{
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, missingField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_TT_CLASS, _DtACTION_TT_MSG);
			return buf;
		}
		if ( !(mask & _ActDb_TT_SCOPE_SET) )
		{
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, missingField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_TT_SCOPE, _DtACTION_TT_MSG);
			return buf;
		}
		if ( !(mask & _ActDb_TT_OPERATION_SET) )
		{
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, missingField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_TT_OPERATION,
					 _DtACTION_TT_MSG);
			return buf;
		}

		/*
		 * Check if tooltalk arg info is supplied by this action
		 */
		if ( (mask & _ActDb_TT_ARGN_BITS) )
		{
			/*
			 * Check for consistency of the fields
			 * describing tooltalk arguments.
			 */
			if ( !( mask & _ActDb_TT_ARGN_MODE_SET) )
			{
				char argModeString[ 1
					+ sizeof(_DtACTION_TTN_ARG)
					+ sizeof(_DtACTION_TTN_MODE)];
				/*
				 * Missing required arg mode
				 */
				sprintf(argModeString,"%s%s%s",
					_DtACTION_TTN_ARG,
					"n",
					_DtACTION_TTN_MODE);
				buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
				(void) sprintf (buf, missingField, 
					XrmQuarkToString(actp->recordName),
					fileName, 
					argModeString,
					_DtACTION_TT_MSG);
				return buf;
			}

			/* 
			 * For each tt arg mode specifier see that the other
			 * required info has been supplied.
			 * -- If a mode specifier is missing for a TT argument;
			 *    that argument and all subsequent TT args will
			 *    be ignored.
			 *   MOVE THIS UP TO DATABASE LOAD TIME
			 */
			for ( i = 0; True; i++ )
			{
				sprintf(argnFieldName,"%s%d%s",
					 _DtACTION_TTN_ARG, i, _DtACTION_TTN_MODE);
				if ( !(s = _DtDtsDbGetFieldByName(actp,
						argnFieldName)) )
					break;	/* no argn mode  specified */
				/*
				 * See that the other required tt argn 
				 * fields exist for n == i.
				 */
			
				if ( strcmp(s, _DtACTION_TT_MODE_OUT ) )
				{
					char *modeStr = s;
					/*
					 * VTYPE is required for mode 
					 * TT_IN/TT_INOUT 
					*/
					sprintf(argnFieldName,"%s%d%s",
						 _DtACTION_TTN_ARG, i,
						 _DtACTION_TTN_VTYPE);
					if (!(s= _DtDtsDbGetFieldByName(actp,
						 argnFieldName)))
					{
						/*
						 * Missing required field 
						 */
						buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
						(void) sprintf (buf, 
							missingField,
							XrmQuarkToString(actp->recordName),
							fileName,
							argnFieldName,
							modeStr);
						return buf;
					}
				}
				/* RWV -- more checks to add */
			}

		}

	}
#ifdef _DT_ALLOW_DT_MSGS
	else if  ( !strcmp(actionType,_DtACTION_DT_REQUEST))
	{
		/*
		 * Check that all recognized fields are suitable for
		 * DT_REQUEST type actions.
		 */
		if ( mask & ( _ActDb_CMD_BITS 
			| _ActDb_DT_NOTIFY_BITS
			| _ActDb_TT_BITS 
			| _ActDb_MAP_BITS ))
		{
			/*
			 * Fields unsuitable for DT_REQUEST type actions were 
			 * found. -- reject this record.
			 */
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, invldField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_DT_REQUEST );
			return buf;
		}

	}
	else if  ( !strcmp(actionType,_DtACTION_DT_NOTIFY))
	{
		/*
		 * Check that all recognized fields are suitable for
		 * Dt_NOTIFY type actions.
		 */
		if ( mask & ( _ActDb_DT_REQUEST_BITS 
			| _ActDb_CMD_BITS
			| _ActDb_TT_BITS 
			| _ActDb_MAP_BITS ))
		{
			/*
			 * Fields unsuitable for DT_NOTIFY type actions were 
			 * found. -- reject this record.
			 */
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, invldField, 
				XrmQuarkToString(actp->recordName),
				fileName, _DtACTION_DT_NOTIFY );
			return buf;
		}

	}
#endif	/* _DT_ALLOW_DT_MSGS */
	else
	{

		/*
		 * Unrecognized action type
		 * Throw out the whole record
		 */
		buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
		(void) sprintf (buf, unrecogType, _DtACTION_TYPE,
			 actionType, fileName );
		return buf;
	}

	/*
	 * The record checks out OK
	 */
	return NULL;
}

/******************************************************************************
 *
 * _DtActionConverter - given a list of the fields for an action
 *   definition, put the definition into an internal array.
 *
 * Returns 'True' if the definition was rejected for any reason, otherwise,
 * 'False' is returned.
 *
 * Note:     Space that is allocated for database structures uses vanilla
 *       malloc/strdup/realloc/free functions because the database 
 *       component (Dts) attempts to be independent of X and Xt.  
 *           Space for local usage is allocated using XtMalloc/XtNewString/
 *       /XtRealloc/XtFree family of functions which are somewhat more robust
 *       than the vanilla allocators.
 *
 * (Replaces _DtParseActionDbEntry)
 *****************************************************************************/

Boolean
_DtActionConverter(DtDtsDbField * fields,
	       DtDbPathId pathId,
	       char *hostPrefix,
	       Boolean rejectionStatus)
{
	static	int	firstTime = True;
	DtDtsDbDatabase	*act_db;
	DtDtsDbRecord	*act_rec;
	DtDtsDbField	*fld;
	register int    i;
	unsigned int	mask = 0;
	char		bigBuf[_DtAct_MAX_BUF_SIZE];
	char		*buf = bigBuf;
	char		*fileName = _DtDbPathIdToString(pathId);
	char		*actionType;

	_DtSvcProcessLock();       
	if (firstTime)
	{
		firstTime = False;
		InitializeLocalizedStrings ();
	}

	/*
	 * The action database should have been initialized in
	 * DtDbLoad().
	 */
	act_db = _DtDtsDbGet(_DtACTION_NAME);
	myassert(act_db);

	/* Action records require that an action name be specified */
	if (fields[0].fieldValue == NULL)
	{
		
		buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
		sprintf (buf, missingName, _DtACTION_NAME, fileName);
		_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
		XtFree(fileName);
		if (buf) XtFree(buf);
		_DtSvcProcessUnlock();
		return(True);
	}
	else if (  fields[1].fieldName == 0)
	{
		/* The record contains no fields */
		buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
		(void) sprintf (buf, noFields, fields[0].fieldValue, fileName);
		_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
		XtFree(fileName);
		if (buf) XtFree(buf);
		_DtSvcProcessUnlock();
		return(True);
	}


	/*
	 * The first field contains the record name.
	 */
	act_rec = _DtDtsDbAddRecord(act_db);
	act_rec->recordName = XrmStringToQuark(fields[0].fieldValue);

	/*
	 * Get all the field data  -- we may get an arbitrary number
	 * of TT_ARGn_MODE, TT_ARGn_VTYPE, or  DT_ARGn_VALUE fields.
	 * We may also want to support user-defined fields at some
	 * time in the future -- for now reject unrecognized fileds.
	 */
	for ( i=1; fields[i].fieldName; i++)
	{
		unsigned long ThisBit = 0;
		char *fieldVal = NULL;

		/* find the corresponding field entry and bitmask */

		if ((ThisBit = _DtActFieldBitLookup(XrmQuarkToString(fields[i].fieldName))) != 0)
		{
			switch ((intptr_t)(fieldVal=_DtActValidateFieldValue(ThisBit,
                                fields[i].fieldValue, fields[0].fieldValue,
                                fileName)))
			{
			case 0:
				/* 
				 *  invalid minor field 
				 *  -- reject field only
				 */
				continue;
				break;
			case -1:
				/*
				 *  Invalid critical field 
				 *  --reject entire record
				 */
				_DtDtsDbDeleteRecord(act_rec,act_db);
				XtFree(fileName);
				_DtSvcProcessUnlock();
				return True;
				break;
			default:
				/*
				 * validated fieldVal returned
				 */
				break;
			}
					
			if ( ThisBit == _ActDb_TYPE_SET )
				actionType =  fieldVal;
		}
		else	/* Unrecognized Field */
		{

			/*
			 * Unrecognized Field:
			 *    issue a warning and ignore it for now.
			 * --- Later we may decide to
			 * store arbitrary fields for users. 
			 * As a control we may require that a user field
			 * name begin with some well-known string
			 * -- say "USER_".
			 */
			buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
			(void) sprintf (buf, unrecogField, 
					fields[0].fieldValue, 
					fileName, 
					XrmQuarkToString(fields[i].fieldName),
					fields[i].fieldValue);



			/*************************************************
			 * Unrecognized Field -- lets NOT reject the 
			 * entire record (for now) and just ignore the
			 * unrecognized field.  -- this allows old
			 * syntax to exist in an action record without
			 * causing outright rejection of the record.
			 * We may decide to reject such records again
			 * once things settle down, hence the UNREACHABLE
			 * code which follows the continue statement.
			 *************************************************/
			_DtSimpleError(DtProgName,DtWarning, NULL, "%s", buf);
			if (buf) XtFree(buf);
			continue;
		}

		if ( mask & ThisBit )
		{
			/*
			 * An exhaustive name check is required for the "*_ARGn_"
			 * fields -- a simple bitmask check is sufficient for
			 * other field names.
			 */
			if ( ((ThisBit & _ActDb_ARGN_BITS) 
				&& _DtActDupFieldNameCheck( act_rec->fieldList,
					act_rec->fieldCount,
					fields[i].fieldName)) 
				|| !(ThisBit & _ActDb_ARGN_BITS) )	
			{
				/*
				 * Attempt to redefine a field
				 * reject this record.
				 */
				buf = XtMalloc(_DtAct_MAX_BUF_SIZE);
				(void) sprintf (buf,dupFields, fields[0].fieldValue, 
						 fileName, 
						 XrmQuarkToString(fields[i].fieldName));
				_DtSimpleError(DtProgName,DtError, NULL, "%s", buf);
				_DtDtsDbDeleteRecord(act_rec,act_db);
				XtFree(fileName);
				if (buf) XtFree(buf);
                                free(fieldVal);
				_DtSvcProcessUnlock();
				return True;
			}
		}


		mask |= ThisBit;
		fld = _DtDtsDbAddField(act_rec);
		fld->fieldName = fields[i].fieldName;
		fld->fieldValue = fieldVal;

	}

	/*
	 * Fill in default action type if necessary
	 */
	if ( !(mask & _ActDb_TYPE_SET) )
	{
		/*
		 * No type specified for this action
		 *  -- revert to default type (i.e. COMMAND).
		 */
		mask |= _ActDb_TYPE_SET;
		actionType = _DtACT_TYPE_DFLT;
	}


	/*
	 * Now check the record for consistency
	 */

	if ( (buf = _DtActCheckRecord(act_rec,actionType,mask,fileName))
		 != NULL )
	{
		/*
		 * This is an invalid record
		 */
		_DtSimpleError(DtProgName,DtError,NULL,"%s",buf);
		_DtDtsDbDeleteRecord(act_rec,act_db);
		XtFree(buf);
		XtFree(fileName);

		_DtSvcProcessUnlock();
		return True;
	}

	act_rec->seq =  ++(act_db->ActionSequenceNumber);
	act_rec->pathId = pathId;

	XtFree(fileName);	
	_DtSvcProcessUnlock();
	return False;
}


void 
_DtFreeActionDB( void )
{
	DtDtsDbDatabase *act_db = _DtDtsDbGet(_DtACTION_NAME);

	if ( act_db)
	{
		_DtDtsDbDeleteDb(act_db);
	}

}

/********************
 *
 * InitializeLocalizedStrings - initializes the localized strings.
 *
 * MODIFIED:	The following variables are all initialized.
 *
 *   char	*noFields;
 *   char	*unrecogField;
 *   char	*unrecogType;
 *   char	*noMapTo;
 *   char	*noExecString;
 *   char	*unrecogMsgType;
 *   char	*dupFields;
 *   char	*invldField;
 *   char	*missingField;
 *   char	*missingName;
 *   char	*unrecogMsgType2;
 *
 ********************/

static void
InitializeLocalizedStrings ( void )
{
   noFields = XtNewString (((char *) Dt11GETMESSAGE (6, 1, "The action definition \"%s\" in the file:\n  \"%s\"\n  does not have any fields.\n")));

   unrecogField = XtNewString (((char *) Dt11GETMESSAGE (6, 2, "The action definition \"%s\" in the file\n  \"%s\"\n  contains the following unrecognized field name and value:\n  \"%s\":\"%s\"\n")));

   unrecogType = XtNewString (((char *) Dt11GETMESSAGE (6, 4, "The \"%s\" field in the action definition \"%s\"\n  in the file \"%s\"\n  has an unrecognized action type.\n")));

   noMapTo = XtNewString (((char *) Dt11GETMESSAGE (6, 5, "The action definition \"%s\" in the file\n  \"%s\"\n  is not mapped properly.  The \"%s\" field\n  should be: TYPE MAP action_mapped_to.\n")));

   noExecString = XtNewString (((char *) Dt11GETMESSAGE (6, 8, "The action definition \"%s\" in the file\n  \"%s\"\n  is a \"COMMAND\" action type but lacks the\n  required field \"%s\".\n")));

   unrecogMsgType = XtNewString (((char *) Dt11GETMESSAGE (6, 11, "The action definition \"%s\" in the file\n  \"%s\"\n  has the illegal value \"%s\" in the \"%s\" field.\n")));

   dupFields = XtNewString (((char *) Dt11GETMESSAGE (6, 12, "The action definition \"%s\" in the file\n  \"%s\"\n  contains duplicate fields named:\n \"%s\".\n")));

   invldField = XtNewString (((char *) Dt11GETMESSAGE (6, 13, "The action definition \"%s\" in the file\n  \"%s\"\n  contains invalid fields for \"%s\" type actions.\n")));

   missingField = XtNewString (((char *) Dt11GETMESSAGE (6, 14, "The action definition \"%s\" in the file\n  \"%s\"\n  is missing the required field, \"%s\"\n for \"%s\" type actions.\n")));

   missingName = XtNewString (((char *) Dt11GETMESSAGE (4, 1, "A \"%s\" definition in the file:\n  \"%s\"\n  is missing a name.  Add the name\n  to the defintion.\n")));

   unrecogMsgType2 = XtNewString (((char *) Dt11GETMESSAGE (6, 16, "The action definition \"%s\" in the file\n  \"%s\"\n  has the illegal value \"%s\" in the \"%s%s%s\" field.\n")));
}
