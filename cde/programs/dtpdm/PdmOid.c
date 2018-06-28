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
/* $TOG: PdmOid.c /main/12 1997/11/20 16:37:12 bill $ */
/*
 * dtpdm/PdmOid.c 
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#include <stdlib.h>
#include "PdmOid.h"

#include <Dt/DtNlUtils.h>

#include "PdmMsgs.h"

/*
 * PdmOidNotify value strings
 */
#define NOTIFY_EMAIL_STR "{{event-report-job-completed} electronic-mail}"
#define NOTIFY_NONE_STR  "{}"

/*
 * entry type for the object identifier string map
 */
typedef struct _PdmOidStringMapEntry
{
    const char* string;
    int length;
    int msg_set;
    int msg_number;
    const char* default_message;
    
} PdmOidStringMapEntry;

/*
 * include the auto-generated static PdmOidStringMap
 */
#include "PdmOidStrs.h"

/*
 * PdmOid static function declarations
 */
static PdmOid PdmOidParse(const char* value_string,
			  const char** ptr_return);
/*
 * PdmOidList static function declarations
 */
static PdmOidList* PdmOidListParse(const char* value_string,
				   const char** ptr_return, int i);


/*
 * PdmOidMediumSourceSize static function declarations
 */
static PdmOidMediumSS* MediumSSParse(const char* value_string,
				     const char** ptr_return, int i);
static PdmOidMediumContinuousSize* MediumContinuousSizeParse(const char*,
							     const char**);
static void MediumContinuousSizeDelete(PdmOidMediumContinuousSize* me);
static PdmOidMediumDiscreteSizeList* MediumDiscreteSizeListParse(const char*,
								 const char**,
								 int i);
static void MediumDiscreteSizeListDelete(PdmOidMediumDiscreteSizeList* list);

static Boolean ParseArea(const char* value_string,
			 const char** ptr_return,
			 PdmOidArea* area_return);
static Boolean ParseUnsignedRange(const char* value_string,
				  const char** ptr_return,
				  PdmOidUnsignedRange* range_return);

/*
 * PdmOidTrayMediumList static function declarations
 */
static PdmOidTrayMediumList* TrayMediumListParse(const char* value_string,
						 const char** ptr_return,
						 int i);

/*
 * PdmOidDocumentFormat
 */
static char* PdmOidDocumentFormatNext(const char* value_string,
				      const char** ptr_return);
/*
 * misc. parsing static function declarations
 */
static Boolean ParseBooleanValue(const char* value_string,
				 const char** ptr_return,
				 Boolean* boolean_return);
static Boolean ParseUnsignedValue(const char* value_string,
				  const char** ptr_return,
				  unsigned long* unsigned_return);
static Boolean ParseRealValue(const char* value_string,
			      const char** ptr_return,
			      float* real_return);
static Boolean ParseSeqEnd(
			   const char* value_string,
			   const char** ptr_return);
static Boolean ParseSeqStart(
			     const char* value_string,
			     const char** ptr_return);
static Boolean ParseUnspecifiedValue(
				     const char* value_string,
				     const char** ptr_return);
static int SpanToken(
			     const char* string);
static int SpanWhitespace(
			  const char* string);


/*
 * ------------------------------------------------------------------------
 * Name: PdmOidString
 *
 * Description:
 *
 *
 * Return value:
 *
 */
const char*
PdmOidString(PdmOid pdm_oid)
{
    /*
     * PdmOid enum values are index values into the string map
     */
    return PdmOidStringMap[pdm_oid].string;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidMsgSet
 *
 * Description:
 *
 *
 * Return value:
 *
 */
int
PdmOidMsgSet(PdmOid pdm_oid)
{
    /*
     * PdmOid enum values are index values into the string map
     */
    return PdmOidStringMap[pdm_oid].msg_set;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidMsgNum
 *
 * Description:
 *
 *
 * Return value:
 *
 */
int
PdmOidMsgNum(PdmOid pdm_oid)
{
    /*
     * PdmOid enum values are index values into the string map
     */
    return PdmOidStringMap[pdm_oid].msg_number;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidDefaultMsg
 *
 * Description:
 *
 *
 * Return value:
 *
 */
const char*
PdmOidDefaultMsg(PdmOid pdm_oid)
{
    if(PdmOidStringMap[pdm_oid].default_message == (const char*)NULL)
	return PdmOidStringMap[pdm_oid].string;
    else
	return PdmOidStringMap[pdm_oid].default_message;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidStringLength
 *
 * Description:
 *
 *
 * Return value:
 *
 */
int
PdmOidStringLength(PdmOid pdm_oid)
{
    /*
     * PdmOid enum values are index values into the string map
     */
    return PdmOidStringMap[pdm_oid].length;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidFromString
 *
 * Description:
 *
 *
 * Return value:
 *
 */
PdmOid
PdmOidFromString(const char* value)
{
    if(value == (const char*)NULL)
	return pdmoid_none;
    else
	return PdmOidParse(value, (const char**)NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidParse
 *
 * Description:
 *
 *     Parse the next whitespace-delimited string from 'value_string'
 *     updating 'ptr_return' to point to the next unparsed location in
 *     'value_string'. 'ptr_return' can be NULL.
 *
 * Return value:
 *
 *     The corresponding PdmOid for the parsed name string.
 *     A return value of pdmoid_none is returned if the parsed name
 *     was not a valid oid or if no name was found.
 *
 */
static PdmOid
PdmOidParse(const char* value_string,
	    const char** ptr_return)
{
    const char* ptr;
    int length;
    int i;

    /*
     * skip leading whitespace
     */
    ptr = value_string + SpanWhitespace(value_string);
    /*
     * get the whitespace-delimited token length
     */
    length = SpanToken(ptr);
    /*
     * match the oid string in the map
     */
    for(i = 0; i < PdmOidStringMapCount; i++)
	if(length == PdmOidStringMap[i].length)
	    if(strncmp(ptr, PdmOidStringMap[i].string, length) == 0)
		break;
    if(i == PdmOidStringMapCount)
	i =  pdmoid_none;
    /*
     * update the return pointer and return
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = ptr+length;
    return i;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidListNew
 *
 * Description:
 *
 *
 * Return value:
 *
 *     
 *
 */
PdmOidList*
PdmOidListNew(const char* value_string)
{
    if(value_string == (const char*)NULL)
	return (PdmOidList*)NULL;
    else
    {
	const char* ptr;
	return PdmOidListParse(value_string, &ptr, 0);
    }
}


/*
 * ------------------------------------------------------------------------
 * Name: PdmOidListParse
 *
 * Description:
 *
 *     'ptr_return' *cannot* be NULL.
 *
 * Return value:
 *
 *     
 *
 */
static PdmOidList*
PdmOidListParse(const char* value_string,
		const char** ptr_return, int i)
{
    PdmOid oid = pdmoid_none;
    PdmOidList* list;
    /*
     * parse the next valid oid out of the value string
     */
    ptr_return = &value_string;
    while(**ptr_return != '\0' &&
	  (oid = PdmOidParse(*ptr_return, ptr_return)) == pdmoid_none);
    if(oid == pdmoid_none)
    {
	/*
	 * end of value string; allocate the list structure
	 */
	list = (PdmOidList*)XtCalloc(1, sizeof(PdmOidList));
	list->count = i;
	list->list = (PdmOid*)XtCalloc(i, sizeof(PdmOid));
    }
    else
    {
	/*
	 * recurse
	 */
	list = PdmOidListParse(*ptr_return, ptr_return, i+1);
	/*
	 * set the oid in the list
	 */
	list->list[i] = oid;
    }
    /*
     * return
     */
    return list;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidListHasOid
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
Boolean
PdmOidListHasOid(const PdmOidList* list, PdmOid oid)
{
    int i;
    if(list != (PdmOidList*)NULL)
	for(i = 0; i < list->count; i++)
	    if(list->list[i] == oid)
		return 1;
    return 0;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidListGetIndex
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
int
PdmOidListGetIndex(const PdmOidList* list, PdmOid oid)
{
    int i;
    if(list != (PdmOidList*)NULL)
	for(i = 0; i < list->count; i++)
	    if(list->list[i] == oid)
		return i;
    return -1;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidListDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
void
PdmOidListDelete(PdmOidList* list)
{
    if(list != (PdmOidList*)NULL)
    {
	XtFree((char*)list->list);
	XtFree((char*)list);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLinkedListNew
 *
 * Description:
 *
 *
 * Return value:
 *
 */
PdmOidLinkedList*
PdmOidLinkedListNew(void)
{
    return (PdmOidLinkedList*)XtCalloc(1, sizeof(PdmOidLinkedList));
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLinkedListDelete
 *
 * Description:
 *
 *
 * Return value:
 *
 */
void
PdmOidLinkedListDelete(PdmOidLinkedList* me)
{
    if(me != (PdmOidLinkedList*)NULL)
    {
	while(me->head)
	{
	    me->current = me->head;
	    me->head = me->current->next;
	    XtFree((char*)me->current);
	}
	XtFree((char*)me);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLinkedListGetOid
 *
 * Description:
 *
 *
 * Return value:
 *
 */
PdmOid
PdmOidLinkedListGetOid(PdmOidLinkedList* me, int i)
{
    if(i < 0 || i >= me->count) return pdmoid_none;
    me->current = me->head;
    while(i--) me->current = me->current->next;
    return me->current->oid;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLinkedListAddOid
 *
 * Description:
 *
 *
 * Return value:
 *
 */
void
PdmOidLinkedListAddOid(PdmOidLinkedList* me, PdmOid oid)
{
    me->current = (PdmOidNode)XtCalloc(1, sizeof(struct PdmOidNodeStruct));
    me->current->oid = oid;
    ++me->count;
    if(me->tail)
    {
	me->tail->next = me->current;
	me->tail = me->current;
    }
    else
	me->head = me->tail = me->current;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLinkedListGetIndex
 *
 * Description:
 *
 *
 * Return value:
 *
 */
int
PdmOidLinkedListGetIndex(PdmOidLinkedList* me, PdmOid oid)
{
    int i = 0;
    me->current = me->head;
    while(me->current)
	if(me->current->oid == oid)
	    return i;
	else
	{
	    ++i;
	    me->current = me->current->next;
	}
    return -1;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLinkedListHasOid
 *
 * Description:
 *
 *
 * Return value:
 *
 */
Boolean
PdmOidLinkedListHasOid(PdmOidLinkedList* me,
		       PdmOid oid)
{
    me->current = me->head;
    while(me->current)
	if(me->current->oid == oid)
	    return True;
	else
	    me->current = me->current->next;
    return False;
}
		       
/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLinkedListFirstOid
 *
 * Description:
 *
 *
 * Return value:
 *
 */
PdmOid
PdmOidLinkedListFirstOid(PdmOidLinkedList* me)
{
    if(me->current = me->head)
	return me->current->oid;
    else
	return pdmoid_none;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLinkedListNextOid
 *
 * Description:
 *
 *
 * Return value:
 *
 */
PdmOid
PdmOidLinkedListNextOid(PdmOidLinkedList* me)
{
    if(me->current ? me->current = me->current->next : False)
	return me->current->oid;
    else
	return pdmoid_none;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidLocalString
 *
 * Description:
 *
 *
 * Return value:
 *
 */
const char*
PdmOidLocalString(PdmOid pdm_oid)
{
    /*
     * PdmOid enum values are index values into the string map
     */
    if(PdmOidStringMap[pdm_oid].msg_set != -1)
    {
	return DTPDM_GETMESSAGE(PdmOidStringMap[pdm_oid].msg_set,
				PdmOidStringMap[pdm_oid].msg_number,
				PdmOidStringMap[pdm_oid].string);
    }
    else
	return PdmOidStringMap[pdm_oid].string;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidMediumSSNew
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
PdmOidMediumSS*
PdmOidMediumSSNew(const char* value_string)
{
    if(value_string == (const char*)NULL)
	return (PdmOidMediumSS*)NULL;
    else
    {
	const char* ptr;
	return MediumSSParse(value_string, &ptr, 0);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: MediumSSParse
 *
 * Description:
 *
 *     'ptr_return' *cannot* be NULL.
 *     
 *
 * Return value:
 *
 *     
 *
 */
static PdmOidMediumSS*
MediumSSParse(const char* value_string,
		    const char** ptr_return, int i)
{
    PdmOidMediumSS* medium_ss;
    PdmOidMediumSourceSize mss;
    /*
     * check for the start of a new MediumSourceSize sequence
     */
    if(ParseSeqStart(value_string, ptr_return))
    {
	/*
	 * check for an unspecified tray value
	 */
	if(ParseUnspecifiedValue(*ptr_return, ptr_return))
	    mss.input_tray = pdmoid_unspecified;
	else
	{
	    /*
	     * parse out the input tray
	     */
	    mss.input_tray = PdmOidParse(*ptr_return, ptr_return);
	}
	/*
	 * attempt to parse a Continuous MediumSize sequence
	 */
	mss.ms.continuous_size =
	    MediumContinuousSizeParse(*ptr_return, ptr_return);
	if(mss.ms.continuous_size != (PdmOidMediumContinuousSize*)NULL)
	{
	    mss.mstag = PdmOidMediumSS_CONTINUOUS;
	}
	else
	{
	    /*
	     * not continuous, try Discrete MediumSize
	     */
	    mss.ms.discrete =
		MediumDiscreteSizeListParse(*ptr_return, ptr_return, 0);
	    if(mss.ms.discrete == (PdmOidMediumDiscreteSizeList*)NULL)
	    {
		/*
		 * syntax error (MediumDiscreteSizeListParse reports error)
		 */
		return NULL;
	    }
	    mss.mstag = PdmOidMediumSS_DISCRETE;
	}
	/*
	 * parse out the MediumSourceSize sequence end
	 */
	if(!ParseSeqEnd(*ptr_return, ptr_return))
	{
	    /*
	     * syntax error
	     */
	    fprintf(stderr, "%s\n", PDM_MSG_WARN_MSS);
	    return NULL;
	}
	/*
	 * recurse to parse the next MediumSourceSize sequence
	 */
	medium_ss = MediumSSParse(*ptr_return, ptr_return, i+1);
	if(medium_ss == (PdmOidMediumSS*)NULL)
	{
	    /*
	     * syntax error - clean up and return
	     */
	    switch(mss.mstag)
	    {
	    case PdmOidMediumSS_CONTINUOUS:
		MediumContinuousSizeDelete(mss.ms.continuous_size);
		break;
	    case PdmOidMediumSS_DISCRETE:
		MediumDiscreteSizeListDelete(mss.ms.discrete);
		break;
	    }
	    return NULL;
	}
	/*
	 * copy the current MediumSourceSize into the array
	 */
	memmove((medium_ss->mss)+i, &mss, sizeof(PdmOidMediumSourceSize));
    }
    else
    {
	/*
	 * MediumSourceSize sequence start not found
	 */
	if(**ptr_return == '\0')
	{
	    /*
	     * end of value string; allocate the MediumSS structure
	     */
	    medium_ss = (PdmOidMediumSS*)XtCalloc(1, sizeof(PdmOidMediumSS));
	    medium_ss->count = i;
	    medium_ss->mss = (PdmOidMediumSourceSize*)
		XtCalloc(i, sizeof(PdmOidMediumSourceSize));
	}
	else
	{
	    /*
	     * syntax error
	     */
	    fprintf(stderr, "%s\n", PDM_MSG_WARN_MSS);
	    return NULL;
	}
    }
    return medium_ss;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidMediumSSDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
void
PdmOidMediumSSDelete(PdmOidMediumSS* me)
{
    if(me != (PdmOidMediumSS*)NULL)
    {
	int i;
	for(i = 0; i < me->count; i++)
	{
	    switch((me->mss)[i].mstag)
	    {
	    case PdmOidMediumSS_CONTINUOUS:
		MediumContinuousSizeDelete((me->mss)[i].ms.continuous_size);
		break;
	    case PdmOidMediumSS_DISCRETE:
		MediumDiscreteSizeListDelete((me->mss)[i].ms.discrete);
		break;
	    }
	}
	XtFree((char*)me);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidMediumSSGetAllSizes
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
PdmOidLinkedList*
PdmOidMediumSSGetAllSizes(PdmOidMediumSS* me)
{
    PdmOidLinkedList* all_sizes;
    int i_mss, i_ds;
    PdmOidMediumDiscreteSizeList* ds_list;
    PdmOid page_size;
    
    if(me == (PdmOidMediumSS*)NULL)
	return (PdmOidLinkedList*)NULL;

    all_sizes = PdmOidLinkedListNew();
    /*
     * loop through the MediumSourceSizes
     */
    for(i_mss = 0; i_mss < me->count; i_mss++)
    {
	switch((me->mss)[i_mss].mstag)
	{
	case PdmOidMediumSS_DISCRETE:
	    /*
	     * add unique discrete sizes to the size list
	     */
	    ds_list =  (me->mss)[i_mss].ms.discrete;
	    for(i_ds = 0; i_ds < ds_list->count; i_ds++)
	    {
		page_size = (ds_list->list)[i_ds].page_size;
		if(page_size == pdmoid_none)
		    continue;
		if(!PdmOidLinkedListHasOid(all_sizes, page_size))
		{
		    PdmOidLinkedListAddOid(all_sizes, page_size);
		}
	    }
	    break;

	case PdmOidMediumSS_CONTINUOUS:
	    /*
	     * unsupported
	     */
	    break;
	}
    }
    /*
     * return
     */
    return all_sizes;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidMediumSSGetTraysSizes
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
void
PdmOidMediumSSGetTraysSizes(PdmOidMediumSS* me,
			    PdmOidTrayMediumList* trays_medium,
			    PdmOidList** trays,
			    PdmOidList** sizes)
{
    int i_mss, i_ds, i_itm, i_its;
    PdmOidMediumDiscreteSizeList* ds_list;
    int tray_count;
    PdmOid current_tray, current_medium;
    PdmOidMediumDiscreteSizeList* unspecified_tray_ds;
    PdmOidMediumDiscreteSizeList* tray_ds;

    if(me == (PdmOidMediumSS*)NULL
       ||
       trays_medium == (PdmOidTrayMediumList*)NULL)
    {
	if(trays != (PdmOidList**)NULL) *trays = (PdmOidList*)NULL;
	if(sizes != (PdmOidList**)NULL) *sizes = (PdmOidList*)NULL;
	return;
    }
    /*
     * allocate the trays and sizes lists
     */
    tray_count = PdmOidTrayMediumListCount(trays_medium);
    if(tray_count == 0)
    {
	if(trays != (PdmOidList**)NULL) *trays = (PdmOidList*)NULL;
	if(sizes != (PdmOidList**)NULL) *sizes = (PdmOidList*)NULL;
	return;
    }
    if(trays != (PdmOidList**)NULL)
    {
	*trays = (PdmOidList*)XtCalloc(1, sizeof(PdmOidList));
	(*trays)->list = (PdmOid*)XtCalloc(tray_count, sizeof(PdmOid));
    }
    if(sizes != (PdmOidList**)NULL)
    {
	*sizes = (PdmOidList*)XtCalloc(1, sizeof(PdmOidList));
	(*sizes)->list = (PdmOid*)XtCalloc(tray_count, sizeof(PdmOid));
    }
    /*
     * loop through the input trays medium list
     */
    i_its = 0;
    for(i_itm = 0; i_itm < tray_count; i_itm++)
    {
	current_tray = PdmOidTrayMediumListTray(trays_medium, i_itm);
	if(current_tray == pdmoid_none)
	    continue;
	current_medium = PdmOidTrayMediumListMedium(trays_medium, i_itm);
	if(current_medium == pdmoid_none)
	    continue;
	/*
	 * loop through the MediumSourceSizes, looking for an appropriate
	 * discrete sizes spec for the current tray
	 */
	unspecified_tray_ds = (PdmOidMediumDiscreteSizeList*)NULL;
	tray_ds = (PdmOidMediumDiscreteSizeList*)NULL;
	for(i_mss = 0;
	    i_mss < me->count
	    && tray_ds == (PdmOidMediumDiscreteSizeList*)NULL;
	    i_mss++)
	{
	    switch((me->mss)[i_mss].mstag)
	    {
	    case PdmOidMediumSS_DISCRETE:
		if((me->mss)[i_mss].input_tray == current_tray)
		    tray_ds = (me->mss)[i_mss].ms.discrete;
		else if ((me->mss)[i_mss].input_tray == pdmoid_unspecified)
		    unspecified_tray_ds = (me->mss)[i_mss].ms.discrete;
		break;
		   
	    case PdmOidMediumSS_CONTINUOUS:
		/*
		 * unsupported
		 */
		break;
	    }
	}
	/*
	 * if the tray was not matched, use the unspecifed tray size list
	 */
	if(tray_ds == (PdmOidMediumDiscreteSizeList*)NULL)
	    if(unspecified_tray_ds == (PdmOidMediumDiscreteSizeList*)NULL)
	    {
		/*
		 * not even an unspecified tray, skip this
		 * input-trays-medium entry.
		 */
		continue;
	    }
	    else
		tray_ds = unspecified_tray_ds;
	/*
	 * loop through the discrete sizes list, looking for a size that
	 * matches the medium for the current input tray
	 */
	for(i_ds = 0; i_ds < tray_ds->count; i_ds++)
	{
	    /*
	     * check to see if the current input tray's medium size
	     * matches the current discrete size
	     *
	     * Note: in the CDEnext SI, medium identifiers coincide with
	     *       medium-size identifiers. If the DP-Medium object is
	     *       ever implemented, this check would need to be
	     *       changed so that the input tray's medium size is
	     *       obtained from the indicated Medium object, and not
	     *       inferred from the medium identifier itself.
	     */
	    if((tray_ds->list)[i_ds].page_size == current_medium)
	    {
		/*
		 * the current input tray's medium size matches the
		 * current discrete size; add the tray and medium size
		 * Oids to the return lists.
		 */
		if(trays != (PdmOidList**)NULL)
		    (*trays)->list[i_its] = current_tray;
		if(sizes != (PdmOidList**)NULL)
		    (*sizes)->list[i_its] = (tray_ds->list)[i_ds].page_size;
		++i_its;
		break;
	    }
	}
    }
    if(trays != (PdmOidList**)NULL) (*trays)->count = i_its;
    if(sizes != (PdmOidList**)NULL) (*sizes)->count = i_its;
}

/*
 * ------------------------------------------------------------------------
 * Name: MediumContinuousSizeParse
 *
 * Description:
 *
 *     'ptr_return' *cannot* be NULL.
 *     
 *
 * Return value:
 *
 *     
 *
 */
static PdmOidMediumContinuousSize*
MediumContinuousSizeParse(const char* value_string,
				const char** ptr_return)
{
    const char* first_nonws_ptr;
    PdmOidMediumContinuousSize* mcs = NULL;
    /*
     * skip leading whitespace
     */
    first_nonws_ptr = value_string + SpanWhitespace(value_string);
    /*
     * parse out the MediumSize sequence start char
     */
    if(!ParseSeqStart(first_nonws_ptr, ptr_return))
	goto MediumContinuousSizeParse_error;
    /*
     * peek ahead to see if it looks like we actually have a continuous
     * size spec (looking for the sequence start char on the 1st range spec)
     */
    if(!ParseSeqStart(*ptr_return, (const char**)NULL))
	goto MediumContinuousSizeParse_error;
    /*
     * Ok, let's go for it
     */
    mcs = (PdmOidMediumContinuousSize*)
	XtCalloc(1, sizeof(PdmOidMediumContinuousSize));
    /*
     * "range across the feed direction"
     */
    if(!ParseUnsignedRange(*ptr_return, ptr_return, &mcs->range_across_feed))
	goto MediumContinuousSizeParse_error;
    /*
     * "increment across the feed direction" (optional, default 0)
     */
    if(!ParseUnspecifiedValue(*ptr_return, ptr_return))
	if(!ParseUnsignedValue(*ptr_return, ptr_return,
			       &mcs->increment_across_feed))
	    goto MediumContinuousSizeParse_error;
    /*
     * "range in the feed direction"
     */
    if(!ParseUnsignedRange(*ptr_return, ptr_return, &mcs->range_in_feed))
	goto MediumContinuousSizeParse_error;
    /*
     * "increment in the feed direction" (optional, default 0)
     */
    if(!ParseUnspecifiedValue(*ptr_return, ptr_return))
	if(!ParseUnsignedValue(*ptr_return, ptr_return,
			       &mcs->increment_in_feed))
	    goto MediumContinuousSizeParse_error;
    /*
     * "long edge feeds" flag (default TRUE)
     */
    if(ParseUnspecifiedValue(*ptr_return, ptr_return))
	mcs->long_edge_feeds = True;
    else
	if(!ParseBooleanValue(*ptr_return, ptr_return, &mcs->long_edge_feeds))
	    goto MediumContinuousSizeParse_error;
    /*
     * "generic assured reproduction area"
     */
    if(!ParseArea(*ptr_return, ptr_return, &mcs->assured_reproduction_area))
	goto MediumContinuousSizeParse_error;
    /*
     * parse out the MediumSize sequence end character
     */
    if(!ParseSeqEnd(*ptr_return, ptr_return))
	goto MediumContinuousSizeParse_error;
    /*
     * return
     */
    return mcs;
    

 MediumContinuousSizeParse_error:
    /*
     * syntax error - don't log since this function may be called
     * as a lookahead
     */
    *ptr_return = first_nonws_ptr;
    XtFree((char*)mcs);
    return NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: MediumContinuousSizeDelete
 *
 * Description:
 *
 *     'ptr_return' *cannot* be NULL.
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
MediumContinuousSizeDelete(PdmOidMediumContinuousSize* me)
{
    XtFree((char*)me);
}

/*
 * ------------------------------------------------------------------------
 * Name: MediumDiscreteSizeListParse
 *
 * Description:
 *
 *     'ptr_return' *cannot* be NULL.
 *
 * Return value:
 *
 *     
 *
 */
static PdmOidMediumDiscreteSizeList*
MediumDiscreteSizeListParse(const char* value_string,
			    const char** ptr_return, int i)
{
    PdmOidMediumDiscreteSizeList* list;
    PdmOidMediumDiscreteSize mds;
    /*
     * check for the start of a new MediumSize sequence
     */
    if(ParseSeqStart(value_string, ptr_return))
    {
	/*
	 * "page size"
	 */
	mds.page_size = PdmOidParse(*ptr_return, ptr_return);
	/*
	 * "long edge feeds" flag (default TRUE)
	 */
	if(ParseUnspecifiedValue(*ptr_return, ptr_return))
	    mds.long_edge_feeds = True;
	else
	    if(!ParseBooleanValue(*ptr_return, ptr_return,
				  &mds.long_edge_feeds))
	    {
		/*
		 * syntax error
		 */
		fprintf(stderr, "%s\n", PDM_MSG_WARN_MSS);
		return NULL;
	    }
	/*
	 * "assured reproduction area"
	 */
	if(!ParseArea(*ptr_return, ptr_return,
		      &mds.assured_reproduction_area))
	{
	    /*
	     * syntax error
	     */
	    fprintf(stderr, "%s\n", PDM_MSG_WARN_MSS);
	    return NULL;
	}
	/*
	 * parse out the MediumSize sequence end character
	 */
	if(!ParseSeqEnd(*ptr_return, ptr_return))
	{
	    fprintf(stderr, "%s\n", PDM_MSG_WARN_MSS);
	    return NULL;
	}
	/*
	 * recurse to parse the next Discrete MediumSize sequence
	 */
	list = MediumDiscreteSizeListParse(*ptr_return, ptr_return, i+1);
	if(list != (PdmOidMediumDiscreteSizeList*)NULL)
	{
	    /*
	     * copy the current discrete MediumSize into the list
	     */
	    memmove((list->list)+i, &mds, sizeof(PdmOidMediumDiscreteSize));
	}
    }
    else
    {
	/*
	 * MediumSize sequence start not found; end of the discrete sizes
	 * list
	 */
	list = (PdmOidMediumDiscreteSizeList*)
	    XtCalloc(1, sizeof(PdmOidMediumDiscreteSizeList));
	list->count = i;
	list->list = (PdmOidMediumDiscreteSize*)
	    XtCalloc(i, sizeof(PdmOidMediumDiscreteSize));
    }
    return list;
}

/*
 * ------------------------------------------------------------------------
 * Name: MediumDiscreteSizeListDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
MediumDiscreteSizeListDelete(PdmOidMediumDiscreteSizeList* list)
{
    if(list != (PdmOidMediumDiscreteSizeList*)NULL)
    {
	XtFree((char*)list->list);
	XtFree((char*)list);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidTrayMediumListNew
 *
 * Description:
 *
 *
 * Return value:
 *
 *     
 *
 */
PdmOidTrayMediumList*
PdmOidTrayMediumListNew(const char* value_string)
{
    if(value_string == (const char*)NULL)
	return (PdmOidTrayMediumList*)NULL;
    else
    {
	const char* ptr;
	return TrayMediumListParse(value_string, &ptr, 0);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayMediumListParse
 *
 * Description:
 *
 *     'ptr_return' *cannot* be NULL.
 *
 * Return value:
 *
 *     
 *
 */
static PdmOidTrayMediumList*
TrayMediumListParse(const char* value_string,
		    const char** ptr_return, int i)
{
    PdmOidTrayMedium tm;
    PdmOidTrayMediumList* list;
    /*
     * check for the start of a new InputTrayMedium sequence
     */
    if(ParseSeqStart(value_string, ptr_return))
    {
	/*
	 * "input tray"
	 */
	tm.input_tray = PdmOidParse(*ptr_return, ptr_return);
	/*
	 * "medium"
	 */
	tm.medium = PdmOidParse(*ptr_return, ptr_return);
	/*
	 * parse out the InputTrayMedium sequence end character
	 */
	if(!ParseSeqEnd(*ptr_return, ptr_return))
	{
	    fprintf(stderr, "%s\n", PDM_MSG_WARN_ITM);
	    return NULL;
	}
	/*
	 * recurse to parse the next InputTrayMedium sequence
	 */
	list = TrayMediumListParse(*ptr_return, ptr_return, i+1);
	if(list != (PdmOidTrayMediumList*)NULL)
	{
	    /*
	     * copy the current InputTrayMedium into the list
	     */
	    memmove((list->list)+i, &tm, sizeof(PdmOidTrayMedium));
	}
    }
    else
    {
	/*
	 * InputTrayMedium sequence start not found
	 */
	if(**ptr_return == '\0')
	{
	    /*
	     * end of the list
	     */
	    list = (PdmOidTrayMediumList*)
		XtCalloc(1, sizeof(PdmOidTrayMediumList));
	    list->count = i;
	    if (i == 0)
		list->list = NULL;
	    else
	    	list->list = (PdmOidTrayMedium*)
		    XtCalloc(i, sizeof(PdmOidTrayMedium));
	}
	else
	{
	    /*
	     * syntax error
	     */
	    fprintf(stderr, "%s\n", PDM_MSG_WARN_ITM);
	    return NULL;
	}
    }
    /*
     * return
     */
    return list;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidTrayMediumListDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
void
PdmOidTrayMediumListDelete(PdmOidTrayMediumList* list)
{
    if(list != (PdmOidTrayMediumList*)NULL)
    {
	XtFree((char*)list->list);
	XtFree((char*)list);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseArea
 *
 * Description:
 *
 *     Skips leading whitespace and parses out and returns a PdmOidArea.
 *
 * Return value:
 *
 *     True if the PdmOidArea was successfully parsed. ptr_return is
 *     updated to point to location where the parsing ended.
 *
 *     False if a PdmOidArea was not found; ptr_return is updated
 *     to point to the first non-whitespace char in value_string.
 *
 */
static Boolean
ParseArea(const char* value_string,
	       const char** ptr_return,
	       PdmOidArea* area_return)
{
    const char* first_nonws_ptr;
    const char* ptr;
    /*
     * skip leading whitespace
     */
    first_nonws_ptr = value_string + SpanWhitespace(value_string);
    /*
     * parse out the area sequence start
     */
    if(!ParseSeqStart(first_nonws_ptr, &ptr))
	goto ParseArea_error;
    /*
     * parse the minimum x value
     */
    if(!ParseRealValue(ptr, &ptr,
			   area_return ? &area_return->minimum_x : NULL))
	goto ParseArea_error;
    /*
     * parse the maximum x value
     */
    if(!ParseRealValue(ptr, &ptr,
			   area_return ? &area_return->maximum_x : NULL))
	goto ParseArea_error;
    /*
     * parse the minimum y value
     */
    if(!ParseRealValue(ptr, &ptr,
			   area_return ? &area_return->minimum_y : NULL))
	goto ParseArea_error;
    /*
     * parse the maximum y value
     */
    if(!ParseRealValue(ptr, &ptr,
			   area_return ? &area_return->maximum_y : NULL))
	goto ParseArea_error;
    /*
     * parse out the area sequence end
     */
    if(!ParseSeqEnd(ptr, &ptr))
	goto ParseArea_error;
    /*
     * update the return pointer
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = ptr;
    /*
     * return
     */
    return True;
    

 ParseArea_error:
    /*
     * syntax error
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = first_nonws_ptr;
    return False;
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseUnsignedRange
 *
 * Description:
 *
 *     Skips leading whitespace and parses out and returns a
 *     PdmOidUnsignedRange.
 *
 * Return value:
 *
 *     True if the PdmOidUnsignedRange was successfully
 *     parsed. ptr_return is updated to point to location where the
 *     parsing ended.
 *
 *     False if a PdmOidUnsignedRange was not found; ptr_return is
 *     updated to point to the first non-whitespace char in value_string.
 *
 */
static Boolean
ParseUnsignedRange(const char* value_string,
		   const char** ptr_return,
		   PdmOidUnsignedRange* range_return)
{
    const char* first_nonws_ptr;
    const char* ptr;
    /*
     * skip leading whitespace
     */
    first_nonws_ptr = value_string + SpanWhitespace(value_string);
    /*
     * parse out the range sequence start
     */
    if(!ParseSeqStart(first_nonws_ptr, &ptr))
	goto ParseUnsignedRange_error;
    /*
     * parse the lower bound
     */
    if(!ParseUnsignedValue(ptr, &ptr,
			   range_return ? &range_return->lower_bound : NULL))
	goto ParseUnsignedRange_error;
    /*
     * parse the upper bound
     */
    if(!ParseUnsignedValue(ptr, &ptr,
			   range_return ? &range_return->upper_bound : NULL))
	goto ParseUnsignedRange_error;
    /*
     * parse out the range sequence end
     */
    if(!ParseSeqEnd(ptr, &ptr))
	goto ParseUnsignedRange_error;
    /*
     * update the return pointer
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = ptr;
    /*
     * return
     */
    return True;
    

 ParseUnsignedRange_error:
    /*
     * syntax error
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = first_nonws_ptr;
    return False;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidNotifyParse
 *
 * Description:
 *
 *
 * Return value:
 *
 *
 */
PdmOidNotify PdmOidNotifyParse(const char* value_string)
{
    const char* ptr = value_string;

    if(value_string == (const char*)NULL)
	return PDMOID_NOTIFY_NONE;
    /*
     * look for an event handling profile sequence start
     */
    if(!ParseSeqStart(value_string, &ptr))
    {
	if('\0' == *ptr)
	    /*
	     * empty value is valid
	     */
	    return PDMOID_NOTIFY_NONE;
	else
	    return PDMOID_NOTIFY_UNSUPPORTED;
    }
    /*
     * look for an event set sequence start
     */
    if(!ParseSeqStart(ptr, &ptr))
    {
	/*
	 * check for an empty event handling profile
	 */
	if(ParseSeqEnd(ptr, &ptr))
	{
	    ptr += SpanWhitespace(ptr);
	    if(*ptr == '\0')
		/*
		 * valid empty event handling profile sequence
		 */
		return PDMOID_NOTIFY_NONE;
	}
	return PDMOID_NOTIFY_UNSUPPORTED;
    }
    /*
     * the only event in the set should be report job completed
     */
    if(pdmoid_val_event_report_job_completed != PdmOidParse(ptr, &ptr))
	return PDMOID_NOTIFY_UNSUPPORTED;
    /*
     * event set sequence end
     */
    if(!ParseSeqEnd(ptr, &ptr))
	return PDMOID_NOTIFY_UNSUPPORTED;
    /*
     * delivery method of electronic mail
     */
    if(pdmoid_val_delivery_method_electronic_mail != PdmOidParse(ptr, &ptr))
	return PDMOID_NOTIFY_UNSUPPORTED;
    /*
     * event handling profile sequence end
     */
    if(!ParseSeqEnd(ptr, &ptr))
	return PDMOID_NOTIFY_UNSUPPORTED;
    /*
     * end of value
     */
    ptr += SpanWhitespace(ptr);
    if('\0' == *ptr)
	/*
	 * valid supported notification profile
	 */
	return PDMOID_NOTIFY_EMAIL;
    else
	return PDMOID_NOTIFY_UNSUPPORTED;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidNotifyString
 *
 * Description:
 *
 *
 * Return value:
 *
 *
 */
const char* PdmOidNotifyString(PdmOidNotify notify)
{
    switch(notify)
    {
    case PDMOID_NOTIFY_UNSUPPORTED:
	return (const char*)NULL;
	break;
    case PDMOID_NOTIFY_NONE:
	return NOTIFY_NONE_STR;
	break;
    case PDMOID_NOTIFY_EMAIL:
	return NOTIFY_EMAIL_STR;
	break;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidDocumentFormatParse
 *
 * Description:
 *
 *
 * Return value:
 *
 *
 */
char*
PdmOidDocumentFormatParse(const char* value_string)
{
    char* document_format;
    const char* ptr;
    
    /*
     * get the document format from the value string
     */
    document_format =
	PdmOidDocumentFormatNext(value_string, &ptr);
    if((char*)NULL != document_format)
    {
	/*
	 * verify that the document format is the only value specified
	 */
	ptr += SpanWhitespace(ptr);
	if('\0' == *ptr)
	    /*
	     * valid document-format value
	     */
	    return document_format;
	else
	    /*
	     * invalid; clean up
	     */
	    XtFree(document_format);
    }
    return (char*)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidDocumentFormatDefault
 *
 * Description:
 *
 *
 * Return value:
 *
 *
 */
char*
PdmOidDocumentFormatDefault(const char* value_string)
{
    /*
     * return the first document format from the value string
     */
    return PdmOidDocumentFormatNext(value_string, (const char**)NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmOidDocumentFormatNext
 *
 * Description:
 *
 *
 * Return value:
 *
 *
 */
static char*
PdmOidDocumentFormatNext(const char* value_string,
			 const char** ptr_return)
{
    const char* ptr;
    const char* first_nonws_ptr;
    const char* document_format_start;
    char* document_format;
    int document_format_len;
    int token_len;

    if((const char*)NULL == value_string)
	return (char*)NULL;
    /*
     * skip leading whitespace
     */
    ptr = value_string + SpanWhitespace(value_string);
    first_nonws_ptr = ptr;
    /*
     * sequence start
     */
    if(!ParseSeqStart(ptr, &ptr))
	goto PdmOidDocumentFormatNext_error;
    /*
     * skip whitepace to the start of the document format, and save the
     * location
     */
    ptr += SpanWhitespace(ptr);
    document_format_start = ptr;
    /*
     * document format
     */
    if(0 == (token_len = SpanToken(ptr)))
	goto PdmOidDocumentFormatNext_error;
    ptr += token_len;
    /*
     * optional variant
     */
    ptr += SpanWhitespace(ptr);
    if(0 != (token_len = SpanToken(ptr)))
    {
	ptr += token_len;
	/*
	 * optional version
	 */
	ptr += SpanWhitespace(ptr);
	ptr += SpanToken(ptr);
    }
    /*
     * determine the length of the document format, excluding the
     * sequence delimeters
     */
    document_format_len = ptr - document_format_start;
    /*
     * sequence end
     */
    if(!ParseSeqEnd(ptr, &ptr))
	goto PdmOidDocumentFormatNext_error;
    /*
     * update return pointer
     */
    if((const char**)NULL != ptr_return)
	*ptr_return = ptr;
    /*
     * build the return document format string, and return it
     */
    document_format = XtMalloc(document_format_len+1);
    strncpy(document_format, document_format_start, document_format_len);
    document_format[document_format_len] = '\0';
    return document_format;

 PdmOidDocumentFormatNext_error:
    fprintf(stderr, "%s\n", PDM_MSG_WARN_DOC_FMT);
    if((const char**)NULL != ptr_return)
	*ptr_return = first_nonws_ptr;
    return (char*)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseBooleanValue
 *
 * Description:
 *
 *
 * Return value:
 *
 *
 */
static Boolean
ParseBooleanValue(const char* value_string,
		  const char** ptr_return,
		  Boolean* boolean_return)
{
    const char* ptr;
    int length;
    Boolean status;
    /*
     * skip leading whitespace
     */
    ptr = value_string + SpanWhitespace(value_string);
    /*
     * get the whitespace-delimited token length
     */
    length = SpanToken(ptr);
    /*
     * determine if true or false or bad
     */
    if(strncasecmp(ptr, "TRUE", length) == 0)
    {
	if(boolean_return != (Boolean*)NULL)
	    *boolean_return = True;
	status = True;
    }
    else if(strncasecmp(ptr, "FALSE", length) == 0)
    {
	if(boolean_return != (Boolean*)NULL)
	    *boolean_return = False;
	status = True;
    }
    else
    {
	/*
	 * syntax error
	 */
	status = False;
    }
    /*
     * update the return pointer and return
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = status ? ptr+length : ptr;
    return status;
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseUnsignedValue
 *
 * Description:
 *
 *     Skips leading whitespace and parses out and returns a unsigned number.
 *
 * Return value:
 *
 *     True if a unsigned number was successfully parsed. ptr_return is
 *     updated to point to location where the unsigned number parsing
 *     ended.
 *
 *     False if a unsigned number was not found; ptr_return is updated
 *     to point to the first non-whitespace char in value_string.
 *
 */
static Boolean
ParseUnsignedValue(const char* value_string,
		   const char** ptr_return,
		   unsigned long* unsigned_return)
{
    unsigned long unsigned_value;
    Boolean status;
    const char* first_nonws_ptr;
    const char* ptr;
    /*
     * skip leading whitespace
     */
    first_nonws_ptr = value_string + SpanWhitespace(value_string);
    unsigned_value = strtoul(first_nonws_ptr, (char**)(&ptr), 0);
    if(ptr == first_nonws_ptr)
	status = False;
    else
	status = True;
    /*
     * update return parms
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = ptr;
    if(unsigned_return != (unsigned long*)NULL)
	*unsigned_return = unsigned_value;
    /*
     * return
     */
    return status;
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseRealValue
 *
 * Description:
 *
 *     Skips leading whitespace and parses out and returns a real number.
 *
 * Return value:
 *
 *     xTrue if a real number was successfully parsed. ptr_return is
 *     updated to point to location where the real number parsing
 *     ended.
 *
 *     xFalse if a real number was not found; ptr_return is updated
 *     to point to the first non-whitespace char in value_string.
 *
 */
static Boolean
ParseRealValue(const char* value_string,
	       const char** ptr_return,
	       float* real_return)
{
    float real_value;
    Boolean status;
    const char* first_nonws_ptr;
    const char* ptr;
    /*
     * skip leading whitespace
     */
    first_nonws_ptr = value_string + SpanWhitespace(value_string);
    real_value = (float)strtod(first_nonws_ptr, (char**)(&ptr));
    if(ptr == first_nonws_ptr)
	status = False;
    else
	status = True;
    /*
     * update return parms
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = ptr;
    if(real_return != (float*)NULL)
	*real_return = real_value;
    /*
     * return
     */
    return status;
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseSeqEnd
 *
 * Description:
 *
 * Description:
 *
 *     Skips leading whitespace and parses out the sequence end
 *     character '}'.
 *
 * Return value:
 *
 *     True if the sequence end character was parsed; ptr_return is
 *     updated to point to the first char following the sequence end
 *     character.
 *
 *     False if the sequence end character was not found; ptr_return is
 *     updated to point to the first non-whitespace char in value_string.
 *
 */
static Boolean
ParseSeqEnd(const char* value_string,
	    const char** ptr_return)
{
    const char* ptr;
    Boolean status;
    /*
     * skip leading whitespace
     */
    ptr = value_string + SpanWhitespace(value_string);
    /*
     * parse out the sequence end character
     */
    if(*ptr == '}')
    {
	status = True;
	++ptr;
    }
    else
	status = False;
    /*
     * update the return pointer
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = ptr;
    /*
     * return
     */
    return status;
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseSeqStart
 *
 * Description:
 *
 *     Skips leading whitespace and parses out the sequence start
 *     character '{'.
 *
 * Return value:
 *
 *     True if the sequence start character was parsed; ptr_return is
 *     updated to point to the first char following the sequence start
 *     character.
 *
 *     False if the sequence start character was not found; ptr_return is
 *     updated to point to the first non-whitespace char in value_string.
 *
 */
static Boolean
ParseSeqStart(const char* value_string,
	      const char** ptr_return)
{
    const char* ptr;
    Boolean status;
    /*
     * skip leading whitespace
     */
    ptr = value_string + SpanWhitespace(value_string);
    /*
     * parse out the sequence start character
     */
    if(*ptr == '{')
    {
	status = True;
	++ptr;
    }
    else
	status = False;
    /*
     * update the return pointer
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = ptr;
    /*
     * return
     */
    return status;
}

/*
 * ------------------------------------------------------------------------
 * Name: ParseUnspecifiedValue
 *
 * Description:
 *
 *     Skips leading whitespace and parses out an unspecified optional
 *     value (i.e. matching '' or "" - skips all data between the set of
 *     quotes).
 *
 * Return value:
 *
 *     True if an unspecified value was parsed; ptr_return is updated to
 *     point to the first char following the trailing quote.
 *
 *     False if an unspecified value was not found; ptr_return is updated
 *     to point to the first non-whitespace char in value_string.
 *
 */
static Boolean
ParseUnspecifiedValue(const char* value_string,
		      const char** ptr_return)
{
    Boolean status;
    const char* ptr;
    /*
     * skip leading whitespace
     */
    ptr = value_string + SpanWhitespace(value_string);
    /*
     * parse out an unspecified optional value ('' or "")
     */
    if(*ptr == '\'' || *ptr == '"')
    {
	char delim[2];

	if(ptr_return != (const char**)NULL)
	{
	    delim[0] = *ptr;
	    delim[1] = '\0';
	    /*
	     * skip over the matching delimiter
	     */
	    ++ptr;
	    ptr += strcspn(ptr, delim);
	    if(*ptr != '\0')
		++ptr;
	}
	status = True;
    }
    else
	status = False;
    /*
     * update the return pointer
     */
    if(ptr_return != (const char**)NULL)
	*ptr_return = ptr;
    /*
     * return
     */
    return status;
}

/*
 * ------------------------------------------------------------------------
 * Name: SpanToken
 *
 * Description:
 *
 *     Returns the length of the initial segment of the passed string
 *     that consists entirely of non-whitespace and non-sequence
 *     delimiter characters.
 *
 *
 */
static int
SpanToken(const char* string)
{
    const char* ptr;
    for(ptr = string;
	*ptr != '\0' && !DtIsspace((char*)ptr) && *ptr != '{' && *ptr != '}';
	ptr = DtNextChar((char*)ptr));
    return ptr - string;
}

/*
 * ------------------------------------------------------------------------
 * Name: SpanWhitespace
 *
 * Description:
 *
 *     Returns the length of the initial segment of the passed string
 *     that consists entirely of whitespace characters.
 *
 *
 */
static int
SpanWhitespace(const char* string)
{
    const char* ptr;
    for(ptr = string;
	*ptr != '\0' && DtIsspace((char*)ptr);
	ptr = DtNextChar((char*)ptr));
    return ptr - string;
}
