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
/* $XConsortium: ActionP.h /main/3 1995/10/26 14:59:56 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ActionP.h
 **
 **   Project:     DT
 **
 **   Description: Private include file for the Action Library.
 **
 **
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _ActionP_h
#define _ActionP_h

#include <Dt/DtP.h>
#include <Dt/DbReader.h>
#include <Dt/ActionDb.h>
#include <Dt/Action.h>
#include <Tt/tttk.h>
#include <Dt/DtShmDb.h>
/*
 * Environment Variable Names
 */
#define ENV_SESSION_SVR		"SESSION_SVR"
/*
 * Define maximum static buffer size for action code.
 */
#define	_DtAct_MAX_BUF_SIZE	1024

/*
 * Data-type field which contains the template for producing filenames
 * of a given type.
 */
#define  _DtActNAME_TEMPLATE	"NAME_TEMPLATE"
#define  _DtActIS_EXECUTABLE	"IS_EXECUTABLE"

/******************************************************************************
 *
 * MASK LAYOUT:
 *
 * The same layout is used for all the masks (defined herein);, independent of
 * the data structure in which they are defined.  That is, all bit fields for
 * all masks are non-overlapping.  In general, the mask in any given structure
 * should ONLY contain information in the field appropriate for that structure. 
 *
 * The action bit mask is broken into the following fields
 *
 *   --------------------------------------------------------------------------
 *   |act'n |arg  |arg  |arg | win   |args|  request   | object   |treat| not |
 *   |type  |class|count|typ | type  |    |  status    | status   | as  |     |
 *   |bits  |bits |bits |bit | bits  |used|   bits     | bits     |file |used |
 *   --------------------------------------------------------------------------
 *   0 - 4,5 - 8,9 - 12, 13 ,14 - 17,18-19,20 ------- 22,23 ---- 28, 29 ,30-31
 *   |                               |    |            |          |     |
 *   |<--       action mask       -->|    |<- request->|<- obj. ->|< - >|
 *					       mask         data     |
 *                                                          mask    msgCcomp.
 *						   		      mask	
 ******************************************************************************/

/*
 * generic bit manipulation macros
 */
#define SET_ANY_BITS(mask,bits)		( mask |= (bits) )
#define RESET_ANY_BITS(mask,bits)	( mask &= ~(bits) )
#define TST_ANY_BITS(mask,bits)		( mask & (bits) )

/*
 * Action type bits
 */
#define	_DtAct_CMD_BIT				(1<<0)
#define	_DtAct_MAP_BIT				(1<<1)
#define _DtAct_TT_MSG_BIT			(1<<2)

#ifdef _DT_ALLOW_DT_MSGS
#define _DtAct_DT_REQ_MSG_BIT			(1<<3)
#define _DtAct_DT_NTFY_MSG_BIT			(1<<4)
#endif  /* _DT_ALLOW_DT_MSGS */

/*
 * Action structure mask bits
 */
#define _DtAct_ARG_CLASS_FILE_BIT		(1<<5)
#define _DtAct_ARG_CLASS_STRING_BIT		(1<<6)
#define _DtAct_ARG_CLASS_BUFFER_BIT		(1<<7)
#define _DtAct_ARG_CLASS_WILD_BIT		(1<<8)

#define _DtAct_ARG_COUNT_LT_BIT			(1<<9)
#define _DtAct_ARG_COUNT_GT_BIT			(1<<10)
#define _DtAct_ARG_COUNT_EQ_BIT			(1<<11)
#define _DtAct_ARG_COUNT_WILD_BIT		(1<<12)

#define	_DtAct_ARG_TYPE_WILD_BIT		(1<<13)

#define _DtAct_NO_STDIO_BIT			(1<<14)
#define _DtAct_TERMINAL_BIT			(1<<15)
#define _DtAct_PERM_TERM_BIT			(1<<16)

/*
 * Defines which reflect the actual number
 * of %ARGn% instances found in the action definition
 * NOTE: Is this JUST in the EXEC_STRING? or the total number of %ARGn%'s
 * referenced in all parssed definitions?
 *
 */
#define _DtAct_SINGLE_ARG_BIT			(1<<18)
#define _DtAct_MULTI_ARG_BIT			(1<<19)


#ifdef _DT_ALLOW_DT_MSGS
#define _DtAct_ACTION_TYPE_BITS	(  _DtAct_CMD_BIT \
				 | _DtAct_MAP_BIT \
				 | _DtAct_DT_NTFY_MSG_BIT \
				 | _DtAct_DT_REQ_MSG_BIT \
				 | _DtAct_TT_MSG_BIT )
#else
#define _DtAct_ACTION_TYPE_BITS	(  _DtAct_CMD_BIT \
				 | _DtAct_MAP_BIT \
				 | _DtAct_TT_MSG_BIT )
#endif  /* _DT_ALLOW_DT_MSGS */
#define	_DtAct_ARG_CLASS_BITS	(   _DtAct_ARG_CLASS_FILE_BIT \
				 | _DtAct_ARG_CLASS_STRING_BIT\
				 | _DtAct_ARG_CLASS_BUFFER_BIT \
				 | _DtAct_ARG_CLASS_WILD_BIT  )
#define _DtAct_ARG_TYPE_BITS	(   _DtAct_ARG_TYPE_WILD_BIT  )
#define _DtAct_ARG_COUNT_BITS	(  _DtAct_ARG_COUNT_LT_BIT \
				 | _DtAct_ARG_COUNT_GT_BIT \
				 | _DtAct_ARG_COUNT_EQ_BIT \
				 | _DtAct_ARG_COUNT_WILD_BIT )
#define _DtAct_WINTYPE_BITS	(  _DtAct_NO_STDIO_BIT \
				 | _DtAct_TERMINAL_BIT \
				 | _DtAct_PERM_TERM_BIT )
#define _DtAct_ARGS_USED_BITS	(  _DtAct_SINGLE_ARG \
				 | _DtAct_MULTI_ARG  )

#define _DtAct_ACTION_BITS	(  _DtAct_ACTION_TYPE_BITS \
				 | _DtAct_ARG_CLASS_BITS \
				 | _DtAct_ARG_TYPE_BITS  \
				 | _DtAct_ARG_COUNT_BITS \
				 | _DtAct_WINTYPE_BITS \
				 | _DtAct_ARGS_USED_BITS  )

#define IS_CMD(mask)			( mask & _DtAct_CMD_BIT )
#define IS_MAP(mask)			( mask & _DtAct_MAP_BIT )
#define IS_TT_MSG(mask)			( mask & _DtAct_TT_MSG_BIT )

#ifdef _DT_ALLOW_DT_MSGS
#define IS_DT_REQ_MSG(mask)		( mask & _DtAct_DT_REQ_MSG_BIT )
#define IS_DT_NOTIFY_MSG(mask)		( mask & _DtAct_DT_NTFY_MSG_BIT )
#endif  /* _DT_ALLOW_DT_MSGS */

#define SET_CMD_ACTION(mask)		( mask |= _DtAct_CMD_BIT )
#define SET_MAP_ACTION(mask)		( mask |= _DtAct_MAP_BIT )
#define SET_TT_MSG(mask)		( mask |= _DtAct_TT_MSG_BIT )
#ifdef _DT_ALLOW_DT_MSGS
#define SET_DT_REQUEST_MSG(mask)	( mask |= _DtAct_DT_REQ_MSG_BIT)
#define SET_DT_NOTIFY_MSG(mask)		( mask |= _DtAct_DT_NTFY_MSG_BIT)
#endif  /* _DT_ALLOW_DT_MSGS */

#define RESET_CMD_ACTION(mask)		( mask &= ~(_DtAct_CMD_BIT))
#define RESET_MAP_ACTION(mask)		( mask &= ~(_DtAct_MAP_BIT))
#define RESET_TT_MSG(mask)		( mask &= ~(_DtAct_TT_MSG_BIT))
#ifdef _DT_ALLOW_DT_MSGS
#define RESET_DT_REQ_MSG(mask)		( mask &= ~(_DtAct_DT_REQ_MSG_BIT))
#define RESET_DT_NOTIFY_MSG(mask)	( mask &= ~(_DtAct_DT_NTFY_MSG_BIT))
#endif  /* _DT_ALLOW_DT_MSGS */

#define IS_NO_STDIO(mask)		( mask & _DtAct_NO_STDIO_BIT )
#define IS_TERMINAL(mask)		( mask & _DtAct_TERMINAL_BIT )
#define IS_PERM_TERM(mask)		( mask & _DtAct_PERM_TERM_BIT)

#define SET_NO_STDIO(mask)		( mask |= _DtAct_NO_STDIO_BIT )
#define SET_TERMINAL(mask)		( mask |= _DtAct_TERMINAL_BIT )
#define SET_PERM_TERM(mask)		( mask |= _DtAct_PERM_TERM_BIT )

#define RESET_NO_STDIO(mask)		( mask &= ~(_DtAct_NO_STDIO_BIT ))
#define RESET_TERMINAL(mask)		( mask &= ~(_DtAct_TERMINAL_BIT ))
#define RESET_PERM_TERM(mask)		( mask &= ~(_DtAct_PERM_TERM_BIT ))

#define	IS_ARG_CLASS_FILE(mask)		( mask & _DtAct_ARG_CLASS_FILE_BIT )
#define IS_ARG_CLASS_STRING(mask)	( mask & _DtAct_ARG_CLASS_STRING_BIT )
#define IS_ARG_CLASS_BUFFER(mask)	( mask & _DtAct_ARG_CLASS_BUFFER_BIT )
#define IS_ARG_CLASS_WILD(mask)		( mask & _DtAct_ARG_CLASS_WILD_BIT )

#define	SET_ARG_CLASS_FILE(mask)	( mask |= _DtAct_ARG_CLASS_FILE_BIT )
#define SET_ARG_CLASS_STRING(mask)	( mask |= _DtAct_ARG_CLASS_STRING_BIT)
#define SET_ARG_CLASS_BUFFER(mask)	( mask |= _DtAct_ARG_CLASS_BUFFER_BIT)
#define SET_ARG_CLASS_WILD(mask)	( mask |= _DtAct_ARG_CLASS_WILD_BIT )

#define RESET_ARG_CLASS_FILE(mask)	(mask &= ~(_DtAct_ARG_CLASS_FILE_BIT))
#define RESET_ARG_CLASS_STRING(mask)	(mask &= ~(_DtAct_ARG_CLASS_STRING_BIT))
#define RESET_ARG_CLASS_BUFFER(mask)	(mask &= ~(_DtAct_ARG_CLASS_BUFFER_BIT))
#define RESET_ARG_CLASS_WILD(mask)	(mask &= ~(_DtAct_ARG_CLASS_WILD_BIT))


#define IS_ARG_TYPE_WILD(mask)		( mask & _DtAct_ARG_TYPE_WILD_BIT )

#define SET_ARG_TYPE_WILD(mask)		( mask |= _DtAct_ARG_TYPE_WILD_BIT )

#define RESET_ARG_TYPE_WILD(mask)	( mask &= ~(_DtAct_ARG_TYPE_WILD_BIT))

#define IS_ARG_COUNT_GT(mask)		( mask & _DtAct_ARG_COUNT_GT_BIT )
#define IS_ARG_COUNT_LT(mask)		( mask & _DtAct_ARG_COUNT_LT_BIT )
#define IS_ARG_COUNT_EQ(mask)		( mask & _DtAct_ARG_COUNT_EQ_BIT )
#define IS_ARG_COUNT_WILD(mask) 	( mask & _DtAct_ARG_COUNT_WILD_BIT )

#define SET_ARG_COUNT_GT(mask)		( mask |= _DtAct_ARG_COUNT_GT_BIT )
#define SET_ARG_COUNT_LT(mask)		( mask |= _DtAct_ARG_COUNT_LT_BIT )
#define SET_ARG_COUNT_EQ(mask)		( mask |= _DtAct_ARG_COUNT_EQ_BIT )
#define SET_ARG_COUNT_WILD(mask)	( mask |= _DtAct_ARG_COUNT_WILD_BIT )

#define RESET_ARG_COUNT_GT(mask)	( mask &= ~(_DtAct_ARG_COUNT_GT_BIT ))
#define RESET_ARG_COUNT_LT(mask)	( mask &= ~(_DtAct_ARG_COUNT_LT_BIT ))
#define RESET_ARG_COUNT_EQ(mask)	( mask &= ~(_DtAct_ARG_COUNT_EQ_BIT ))
#define RESET_ARG_COUNT_WILD(mask)	( mask &= ~(_DtAct_ARG_COUNT_WILD_BIT ))

/*
 * Use of the logical "NOT" operator(!) instead of the bitwise negation
 * operator(~) is intentional in IS_ARG_NONE_FOUND.
 */
#define IS_ARG_NONE_FOUND(mask)		!( mask & ( _DtAct_SINGLE_ARG_BIT \
						  | _DtAct_MULTI_ARG_BIT ))
#define IS_ARG_SINGLE_ARG(mask)		( mask &  _DtAct_SINGLE_ARG_BIT )
#define IS_ARG_MULTI_ARG(mask)		( mask &  _DtAct_MULTI_ARG_BIT )

#define SET_ARG_NONE_FOUND(mask)	( mask &= ~( _DtAct_SINGLE_ARG_BIT \
					            | _DtAct_MULTI_ARG_BIT ))
#define SET_ARG_SINGLE_ARG(mask)	((mask |= _DtAct_SINGLE_ARG_BIT), \
					( mask &= ~(_DtAct_MULTI_ARG_BIT)))
#define SET_ARG_MULTI_ARG(mask)		((mask |= _DtAct_MULTI_ARG_BIT), \
					( mask &= ~(_DtAct_SINGLE_ARG_BIT)))

#define RESET_ARG_SINGLE_ARG(mask)	( mask &= ~(_DtAct_SINGLE_ARG_BIT))
#define RESET_ARG_MULTI_ARG(mask)	( mask &= ~(_DtAct_MULTI_ARG_BIT))

/*
 * ActionRequest structure mask bits
 */

#define	_DtAct_REPROCESSING_BIT			(1<<20)
#define _DtAct_TOO_MANY_MAPS_BIT		(1<<21)
#define _DtAct_CLONED_REQUEST_BIT		(1<<22)

#define _DtAct_ACTION_REQUEST_BITS	(   _DtAct_REPROCESSING_BIT \
					  | _DtAct_TOO_MANY_MAPS_BIT \
					  | _DtAct_CLONED_REQUEST_BIT )

#define IS_REPROCESSING(mask)		( mask & _DtAct_REPROCESSING_BIT )
#define IS_TOO_MANY_MAPS(mask)		( mask & _DtAct_TOO_MANY_MAPS_BIT )
#define IS_CLONED_REQUEST(mask)		( mask & _DtAct_CLONED_REQUEST_BIT )

#define SET_REPROCESSING(mask)		( mask |= _DtAct_REPROCESSING_BIT )
#define SET_TOO_MANY_MAPS(mask)		( mask |= _DtAct_TOO_MANY_MAPS_BIT )
#define SET_CLONED_REQUEST(mask)	( mask |= _DtAct_CLONED_REQUEST_BIT )

#define RESET_REPROCESSING(mask)	( mask &= ~(_DtAct_REPROCESSING_BIT))
#define RESET_TOO_MANY_MAPS(mask)	( mask &= ~(_DtAct_TOO_MANY_MAPS_BIT))
#define RESET_CLONED_REQUEST(mask)	( mask &= ~(_DtAct_CLONED_REQUEST_BIT))


/*
 * ObjectData structure mask bits
 *
 */

#define _DtAct_WRITE_OBJ_BIT		(1<<23)
#define _DtAct_FILE_OBJ_BIT		(1<<24)
#define _DtAct_BUFFER_OBJ_BIT		(1<<25)
#define _DtAct_STRING_OBJ_BIT		(1<<26)
#define _DtAct_DIR_OBJ_BIT		(1<<27)
#define _DtAct_UNKNOWN_IF_DIR_BIT	(1<<28)

#define _DtAct_OBJ_DATA_BITS		(  _DtAct_WRITE_OBJ_BIT \
					 | _DtAct_FILE_OBJ_BIT \
					 | _DtAct_DIR_OBJ_BIT  \
					 | _DtAct_BUFFER_OBJ_BIT \
					 | _DtAct_STRING_OBJ_BIT \
					 | _DtAct_UNKNOWN_IF_DIR_BIT )

#define	IS_WRITE_OBJ(mask)		( mask & _DtAct_WRITE_OBJ_BIT )
#define	IS_FILE_OBJ(mask)		( mask & _DtAct_FILE_OBJ_BIT )
#define IS_BUFFER_OBJ(mask)		( mask & _DtAct_BUFFER_OBJ_BIT )
#define IS_STRING_OBJ(mask)		( mask & _DtAct_STRING_OBJ_BIT )
#define IS_UNKNOWN_IF_DIR(mask)		( mask & _DtAct_UNKNOWN_IF_DIR_BIT )
#define	IS_DIR_OBJ(mask)		( mask & \
					  ( _DtAct_UNKNOWN_IF_DIR_BIT \
					  | _DtAct_DIR_OBJ_BIT ) \
					   == _DtAct_DIR_OBJ_BIT)

#define SET_WRITE_OBJ(mask)		( mask |= _DtAct_WRITE_OBJ_BIT )
#define SET_FILE_OBJ(mask)		( mask |= _DtAct_FILE_OBJ_BIT )
#define SET_BUFFER_OBJ(mask)		( mask |= _DtAct_BUFFER_OBJ_BIT )
#define SET_STRING_OBJ(mask)		( mask |= _DtAct_STRING_OBJ_BIT )
#define SET_DIR_OBJ(mask)		( mask |= _DtAct_DIR_OBJ_BIT )
#define SET_UNKNOWN_IF_DIR(mask)	( mask |= _DtAct_UNKNOWN_IF_DIR_BIT)

#define RESET_WRITE_OBJ(mask)		( mask &= ~(_DtAct_WRITE_OBJ_BIT))
#define RESET_FILE_OBJ(mask)		( mask &= ~(_DtAct_FILE_OBJ_BIT))
#define RESET_BUFFER_OBJ(mask)		( mask &= ~(_DtAct_BUFFER_OBJ_BIT))
#define RESET_STRING_OBJ(mask)		( mask &= ~(_DtAct_STRING_OBJ_BIT))
#define RESET_DIR_OBJ(mask)		( mask &= ~(_DtAct_DIR_OBJ_BIT))
#define RESET_UNKNOWN_IF_DIR(mask)	( mask &= ~(_DtAct_UNKNOWN_IF_DIR_BIT))


/*
 * MsgComponent structure mask bits (shared with Object Data?)
 */

#define	_DtAct_TREAT_AS_FILE_BIT	(1<<29)

#define _DtAct_MSG_COMP_BITS		( _DtAct_TREAT_AS_FILE_BIT )

#define IS_TREAT_AS_FILE(mask)		( mask &  _DtAct_TREAT_AS_FILE_BIT )
#define SET_TREAT_AS_FILE(mask)		( mask |= _DtAct_TREAT_AS_FILE_BIT)
#define RESET_TREAT_AS_FILE(mask)	( mask &= ~(_DtAct_TREAT_AS_FILE_BIT))


/* Keyword defines */

#define NO_KEYWORD   -1
#define LOCAL_HOST    0
#define DATA_HOST     1
#define DATABASE_HOST 2
#define ARG           3
#define DISPLAY_HOST  4
#define LABEL	      5
#define SESSION_HOST  6

/* Special argNum values */

#define NO_ARG   -1
#define ALL_ARGS  0

/*
 * ToolTalk base representation type ( tt_argn_rep_type ) values
 */
#define DtACT_TT_REP_UNDEFINED	0
#define DtACT_TT_REP_INT	1
#define DtACT_TT_REP_BUFFER	2
#define DtACT_TT_REP_STRING	3

/*
 * Resource name and class for the EXEC-HOST resource.
 */
#define DtEXEC_HOSTS_NAME		"executionHosts"
#define DtEXEC_HOSTS_CLASS		"ExecutionHosts"
#define DtEXEC_HOSTS_DEFAULT      	_DtACT_EXEC_HOST_DFLT

/* Flags to force special processing of filenames */
#define _DTAct_TT_VTYPE  1 << 0
#define _DTAct_TT_ARG    1 << 1

/* Structure used to hold the components of a message */

typedef struct {
   char		*precedingText;
   char		*prompt;
   int		keyword;
   int		argNum;
   unsigned long mask;	/* replaces isFile, isBuffer, isString  boolean */
} MsgComponent;


typedef struct {
   MsgComponent *parsedMessage;
   int          numMsgParts;
   char		*compiledMessage;
   int		msgLen;
} parsedMsg;

/******************************************************************************
 *
 * Structs used during the invocation of an action.   Once DtActionInvoke()
 * exits, these structs are generally freed.
 *
 *****************************************************************************/
typedef struct {
    /*int	winMask;    ---> moved into the action mask*/
    parsedMsg		execString;
    parsedMsg		termOpts;
    char		*contextDir;
    char		*contextHost;
    parsedMsg		execHosts;
    char		**execHostArray;	
    int			execHostCount;
} cmdAttr;

typedef struct {
    DtShmBoson	map_action; 
} mapAttr;

typedef struct {
    int 	tt_class;
    int		tt_scope;
    parsedMsg	tt_op;
    parsedMsg	tt_file;	/* must be a single file name */
    int		*tt_argn_mode;
    int		mode_count;
    parsedMsg	*tt_argn_vtype; 
    int		vtype_count;
    parsedMsg	*tt_argn_value;
    int		value_count;
    int		*tt_argn_rep_type;	/* INT, STRING, BUFFER or UNDEFINED */
    int		rep_type_count;
} tt_msgAttr;

#ifdef _DT_ALLOW_DT_MSGS
typedef struct {
    parsedMsg	service;	/* ICCCM service name */
    parsedMsg	request;	/* request name string */
    parsedMsg	*argn_value;
    int		value_count;

} dt_reqAttr;

typedef struct {
    parsedMsg	ngroup;	/* ICCCM notification group */
    parsedMsg	notify;	/* notification to be sent */
    parsedMsg	*argn_value;
    int		value_count;
} dt_notifyAttr;
#endif  /* _DT_ALLOW_DT_MSGS */


typedef struct {
    DtShmBoson	action;		/* Might just use a char * here? */
    DtDbPathId	file_name_id;	/* id of file wherein the action is defined */
    char	*label;		/* localizable action label string */
    char	*description;
    DtShmBoson	*arg_types;
    int		type_count;
    int		arg_count;	/* Number of arguments accepted by the action */
    unsigned long mask;		/* action mask -- class/type/arg info */
    union {			/* attributes for the different action kinds */
	cmdAttr		cmd;
	mapAttr		map;
	tt_msgAttr	tt_msg;
#ifdef _DT_ALLOW_DT_MSGS
	dt_reqAttr	dt_req;
	dt_notifyAttr	dt_notify;
#endif  /* _DT_ALLOW_DT_MSGS */
    } u;
} Action, *ActionPtr;		/* new action structure and pointer */


/* Structure used to hold each of the object components */

typedef struct {
   char * origFilename;
   char * baseFilename;
   char * origHostname;
   int    hostIndex;
   int    dirIndex;
   void * bp;		/* pointer to original buffer -- tmp files only */
   int    sizebp;	/* size of original buf -- for tmp files only */
} fileAttr;
   
typedef struct {
   char * string;
} stringAttr;
  
typedef struct {
   int size;
   void *bp;
} bufferAttr;
   
   
typedef struct {
   DtShmBoson type;
   unsigned long mask;
   union {
      fileAttr   file;
      stringAttr string;
      bufferAttr buffer;
   } u;
} ObjectData;


/* 
 * Structure attached to the button callbacks in the
 * dialog used to collect missing parameters.
 *
 */

typedef struct {
   char     	*actionName;
   int		objOffset;
   int		objsUsed;
   int          numObjects;
   ObjectData   *objects;
   int          numPromptInputs;
   char       	**promptInputs;
   int          numHostNames;
   char	      	**hostNames;
   int          numDirNames;
   char       	**dirNames;
   char        	*termOpts;
   char        	*cwdHost;
   char        	*cwdDir;
   ActionPtr    clonedAction;
   unsigned long mask;
   char        	*badHostList;
   char        	*currentHost;
   int          hostIndex;
   char	       	*execHost;
   DtActionInvocationID		invocId;
   unsigned long        	childId;
} ActionRequest;

/******************************************************************************
 *
 * Structs used to maintain information on invoked actions until they
 * exit.   A few select elements of the ActionRequest and Action structs
 * will be copied into these strcuts.
 *
 *****************************************************************************/

/******************************************************************************
 ******************************************************************************
 **
 ** Major data-structure diagram for the DtAction invocation layer:
 **
 **    _DtActInvRec    <===  _DtActInvRecArray[ actInvRecArraySize ]
 ** ------------------
 ** |    info per    |
 ** | DtActionInvoke |
 ** |----------------|
 ** |  numChildren   |
 ** |----------------|
 ** |  childRecs[]   | ===>   _DtActChildRec
 ** ------------------      ------------------
 **                         |    info per    |
 **                         | child  request |
 **                         |----------------|
 **                         |    *request    | ===>   ActionRequest
 **                         ------------------      ------------------
 **                                                 | info on users  |
 **                                                 | action request |
 **                                                 |----------------|
 **                               Action       <=== | *clonedAction  |
 **                         ------------------      ------------------
 **                         |info on matching|
 **                         | actionDB entry |
 **                         |----------------|
 **                         | cmd/map/tt/dt  |
 **                         | attributes of  |
 **                         | resulting msg  |
 **                         ------------------
 **
 ** "ActionRequest" and "Action" were present in VUE 3.0, and have a
 ** scratch-pad like function within DtActionInvoke() to get actions
 ** started.  They are not good long-term retainers of information.
 ** Action and ActionRequest are in fact freed when DtActionInvoke()
 ** exits.
 **
 ** "_DtActInvRec" and "_DtActChildRec" have been introduced to retain
 ** information for the entire life of the actions.   Some of the
 ** information from ActionRequest and Action will be copied (mirrored)
 ** up to these structures.
 **
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 *
 *      Child Status Macro Definitions -- These values are used in
 *      the status word of the _DtActChildRec child record.
 *
 *   ****** IMPORTANT NOTE *** IMPORTANT NOTE *** IMPORTANT NOTE ******
 *
 *      The _DtActCHILD_ macros are also used in cde1/dtexec/Main.c
 *      in the _DtActDtexecDone(Request) message, so in effect they
 *      are also protocol constants that should *never* be changed.
 *      If changed, libDtSvc and dtexec may mis-communicate status.
 *
 *****************************************************************************/

#define _DtActCHILD_UNKNOWN		(1<<0)	/* 1 - child status unknown */
#define _DtActCHILD_PENDING_START	(1<<1)	/* 2 - child start pending */
#define _DtActCHILD_ALIVE_UNKNOWN	(1<<2)	/* 4 - child alive but unknown*/
#define _DtActCHILD_ALIVE		(1<<3)	/* 8 - child alive and well */
#define _DtActCHILD_DONE		(1<<4)	/* 16 - child done */
#define _DtActCHILD_FAILED		(1<<5)	/* 32 - child failed */
#define _DtActCHILD_CANCELED		(1<<6)	/* 64 - child canceled */

#define _DtActCHILD_DONE_BITS		( (_DtActCHILD_DONE) | \
                                  	  (_DtActCHILD_FAILED) | \
					  (_DtActCHILD_CANCELED) )

/*** IMPORTANT NOTE ABOVE *** IMPORTANT NOTE ABOVE *** IMPORTANT NOTE ABOVE ***/


#define ARE_CHILDREN_DONE(mask)		( (mask) & _DtActCHILD_DONE_BITS )

#define IS_CHILD_UNKNOWN(mask)		( (mask) == _DtActCHILD_UNKNOWN )
#define IS_CHILD_PENDING_START(mask)	( (mask) & _DtActCHILD_PENDING_START )
#define IS_CHILD_DONE(mask)		( (mask) & _DtActCHILD_DONE)
#define IS_CHILD_FAILED(mask)		( (mask) & _DtActCHILD_FAILED)
#define IS_CHILD_CANCELED(mask)		( (mask) & _DtActCHILD_CANCELED)
#define IS_CHILD_ALIVE(mask)		( (mask) & _DtActCHILD_ALIVE)
#define IS_CHILD_ALIVE_UNKOWN(mask)	( (mask) & _DtActCHILD_ALIVE_UNKNOWN)


/******************************************************************************
 *
 * Information per child of DtActionInvoke()
 *
 *****************************************************************************/

/*
 * Structure useed to map returnable arguments to the child argument list.
 */

typedef struct {
	int argN;	/* Nth returned value (0 based for tooltalk) */
        int argIdx;	/* Nth child argument */
} _DtActArgMap;

typedef struct {
	unsigned long	childId;	/* serial # with DtActInvId */
	unsigned long	childState;	/* child state */

	/*
	 * Information cloned from "ActionRequest *req"
	 */
	int		numObjects;	/* number of child args */
        _DtActArgMap	*argMap;	/* map of returnable args to chd args */

	/*
	 * Information cloned from "ActionPtr clonedAction"
	 */
	unsigned long	mask;		/* action type - IS_xxx() */

	/* a list of tmp file names  5/11/94  --tomg */

	union {
	    struct { /* COMMAND elements */
		char		*TTProcId;	/* TT procID handle to child */
		Tt_message	reqMessage;	/* Initiator TT Request */
		Tt_pattern	magic_cookie;	/* dtexec to libDtSvc cookie */
	    } cmd;
	    struct { /* TT_MSG elements */
		char		*TTProcId;	/* TT procID handle to child */
		Tt_message	reqMessage;	/* Initiator TT Request */
		int		isTtMedia;	/* handled by ttmedia_load ? */
		Tttk_op		TtMediaOp;	/* if ttmedia_load, the op */
		Tt_pattern	*subConPats;	/* for subcontract_manage */
	    } tt;
	} u;

} _DtActChildRecT;


/******************************************************************************
 *
 *	Invocation Status Macro Definitions -- These values are used in
 *	the state word of the _DtActInvRec invocation record.
 *
 *****************************************************************************/

#define _DtActINV_UNKNOWN	0		/* unknown invocation status */
#define _DtActINV_ERROR		(1<<0)		/* invocation error detected */
#define _DtActINV_CANCEL	(1<<1)		/* trying to cancel invocation*/
#define	_DtActINV_PENDING	(1<<2)		/* invocation pending */
#define _DtActINV_WORKING	(1<<3)		/* invocation in process */
#define _DtActINV_DONE		(1<<4)		/* invocation step done */
#define _DtActINV_COMPLETE	(1<<5)		/* invocation steps all done */
#define _DtActINV_CB_CALLED	(1<<6)		/* invocation callback called */
#define _DtActINV_ID_RETURNED	(1<<7)		/* invocation id has returned */
#define _DtActINV_CMD_QUEUED	(1<<8)		/* command has been queued */
#define _DtActINV_INDICATOR_ON	(1<<9)		/* busy indicator active bit */

#define SET_INV_UNKNOWN(mask)		(mask = 0)

#define SET_INV_ERROR(mask)		(mask |= _DtActINV_ERROR) 
#define SET_INV_CANCEL(mask)		(mask |= _DtActINV_CANCEL)
#define SET_INV_PENDING(mask)		(mask |= _DtActINV_PENDING) 
#define SET_INV_WORKING(mask)		(mask |= _DtActINV_WORKING)
#define SET_INV_DONE(mask)		(mask |= _DtActINV_DONE)
#define SET_INV_COMPLETE(mask)		(mask |= _DtActINV_COMPLETE)
#define SET_INV_CB_CALLED(mask)		(mask |= _DtActINV_CB_CALLED)
#define SET_INV_ID_RETURNED(mask)	(mask |= _DtActINV_ID_RETURNED)
#define SET_INV_CMD_QUEUED(mask)	(mask |= _DtActINV_CMD_QUEUED)
#define SET_INV_INDICATOR_ON(mask)	(mask |= _DtActINV_INDICATOR_ON)

#define RESET_INV_ERROR(mask)		(mask &= ~(_DtActINV_ERROR))
#define RESET_INV_CANCEL(mask)		(mask &= ~(_DtActINV_CANCEL))
#define RESET_INV_PENDING(mask)		(mask &= ~(_DtActINV_PENDING))
#define RESET_INV_WORKING(mask)		(mask &= ~(_DtActINV_WORKING))
#define RESET_INV_COMPLETE(mask)	(mask &= ~(_DtActINV_COMPLETE))
#define RESET_INV_DONE(mask)		(mask &= ~(_DtActINV_DONE))
#define RESET_INV_CB_CALLED(mask)	(mask &= ~(_DtActINV_CB_CALLED))
#define RESET_INV_ID_RETURNED(mask)	(mask &= ~(_DtActINV_ID_RETURNED))
#define RESET_INV_CMD_QUEUED(mask)	(mask &= ~(_DtActINV_CMD_QUEUED))
#define RESET_INV_INDICATOR_ON(mask)	(mask &= ~(_DtActINV_INDICATOR_ON))

#define IS_INV_FINISHED(mask)	( !((mask) & _DtActINV_CMD_QUEUED)  \
                                  && ((mask) & (_DtActINV_COMPLETE \
				       | _DtActINV_CANCEL)) )
#define IS_INV_UNKNOWN(mask)    ( (mask) == _DtActINV_UNKNOWN )

#define IS_INV_ERROR(mask)		((mask) & _DtActINV_ERROR )
#define IS_INV_CANCEL(mask)		((mask) & _DtActINV_CANCEL )
#define IS_INV_PENDING(mask)		((mask) & _DtActINV_PENDING )
#define IS_INV_WORKING(mask)		((mask) & _DtActINV_WORKING )
#define IS_INV_DONE(mask)		((mask) & _DtActINV_DONE )
#define IS_INV_COMPLETE(mask)		((mask) & _DtActINV_COMPLETE )
#define IS_INV_CB_CALLED(mask)		((mask) & _DtActINV_CB_CALLED )
#define IS_INV_ID_RETURNED(mask)	((mask) & _DtActINV_ID_RETURNED )
#define IS_INV_CMD_QUEUED(mask)		((mask) & _DtActINV_CMD_QUEUED )
#define IS_INV_INDICATOR_ON(mask)	((mask) & _DtActINV_INDICATOR_ON )

#define CALL_INV_CB(mask)	((IS_INV_FINISHED(mask)) && \
					 (IS_INV_ID_RETURNED(mask)) && \
					 !(IS_INV_CB_CALLED(mask)))

/******************************************************************************
 *
 * Information per DtActionInvoke()
 *
 * Note: For TT_MSG(NOTICE)'s during CDE 1.0, this struct will disappear
 *       immediately after the NOTICEs are sent.
 *
 *****************************************************************************/


typedef struct {
	unsigned long mask;	/* encodes object class and writable flag */
        int size;		/* original size (buffers only) */
        char *type;		/* original type (buffers only) */
        char *name;		/* encodes (tmp) file name associated with the
                                   object (if any) */
} _DtActArgInfo;

typedef struct {
	DtActionStatus	userStatus;
	DtActionArg	*newArgp;
	int		newArgc;
} _DtActUpdateCache;

typedef struct _DtActInvRec {
	unsigned long		state;		/* invocation state */
	DtActionInvocationID	id;		/* identifying invocation ID */
	Widget			w;		/* users widget id */
	DtActionCallbackProc	cb;		/* users callback */
	XtPointer		client_data;	/* users client data for cb */
	_DtActArgInfo		*info;		/* template w/ .argClass info */
	int			ac;		/* original # of  arguments */
	int			numChildren;	/* number of childRec's. */
	_DtActChildRecT		**childRec;	/* array of child Rec's */
	int			cachedUploadCnt;/* cached callback updates */
	_DtActUpdateCache	*cachedUploads;	/* data for */
} _DtActInvRecT;


/******************************************************************************
 *
 * Structs for dialogs/prompts
 *
 *****************************************************************************/

/* Structure used to hold a prompt string destined for a dialog */

typedef struct {
   int    argIndex;
   char *prompt;
} PromptEntry;


/* Structure used to contain prompt dialog information */

typedef struct {
   int    argIndex;
   Widget promptWidget;
} DialogPromptEntry;

typedef struct {
   ActionRequest     * request;
   Widget              topLevel;
   Widget              associatedWidget;
   int                 numPrompts;
   DialogPromptEntry * prompts;
} PromptDialog;


/* Structure used to contain abort/continue dialog information */

typedef struct {
   ActionRequest     * request;
   Widget              topLevel;
   Widget              associatedWidget;
   int                 numPrompts;
   PromptEntry       * prompts;
} ContinueDialog;


/* Structure passed to request passed/failed callbacks */

typedef struct {
   Widget 		associatedWidget;
   char 		*actionLabel;
   int    		offset;
   ActionPtr		actionPtr;		/* Ptr to the action that 
						   was invoked. */
   ActionRequest      	*requestPtr;
   DtActionInvocationID	actInvId;		/* A standard invocation id */
   unsigned long	childId;		/* An id to further identify
						   children of actInvId */
} CallbackData;

/******************************************************************************
 *
 * Private External Function Declarations -- not for public consumption
 *
 ******************************************************************************/

extern void _DtFreeActionStruct( ActionPtr action) ;

#endif /* _ActionP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
