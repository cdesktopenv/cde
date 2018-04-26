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
/* $XConsortium: PdmOid.h /main/9 1996/08/12 18:42:42 cde-hp $ */
/*
 * dtpdm/PdmOid.h 
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
#ifndef _PdmOid_h
#define _PdmOid_h

#include <X11/Intrinsic.h>

/*
 * include the auto-generated PdmOid enum definition
 */
#include "PdmOidDefs.h"

/*
 * list of object identifiers
 */
typedef struct _PdmOidList
{
    PdmOid* list;
    int count;
} PdmOidList;

/*
 * linked list of object identifiers
 */
typedef struct PdmOidNodeStruct
{
    PdmOid oid;
    struct PdmOidNodeStruct* next;
} *PdmOidNode;

typedef struct _PdmOidLinkedList
{
    PdmOidNode head;
    PdmOidNode tail;
    PdmOidNode current;
    int count;
} PdmOidLinkedList;

/*
 * PdmOidMediumSourceSize and related definitions
 */
typedef struct
{
    float minimum_x;
    float maximum_x;
    float minimum_y;
    float maximum_y;
} PdmOidArea; /* (not used by PDM) */

/*
 * XXX  need to update continuous size spec to use reals
 */
typedef struct
{
    unsigned long lower_bound;
    unsigned long upper_bound;
} PdmOidUnsignedRange; /* (not used by PDM) */ 

typedef struct
{
    PdmOidUnsignedRange range_across_feed;
    unsigned long increment_across_feed;
    PdmOidUnsignedRange range_in_feed;
    unsigned long increment_in_feed;
    Boolean long_edge_feeds;
    PdmOidArea assured_reproduction_area;
} PdmOidMediumContinuousSize; /* (not used by PDM) */

typedef struct
{
    PdmOid page_size;
    Boolean long_edge_feeds; /* (not used by PDM) */
    PdmOidArea assured_reproduction_area; /* (not used by PDM) */
} PdmOidMediumDiscreteSize;

typedef struct 
{
    PdmOidMediumDiscreteSize* list;
    int count;
} PdmOidMediumDiscreteSizeList;


typedef struct
{
    PdmOid input_tray; /* may be set to pdmoid_none */
    enum { PdmOidMediumSS_DISCRETE, PdmOidMediumSS_CONTINUOUS } mstag;
    union
    {
	PdmOidMediumDiscreteSizeList* discrete;
	PdmOidMediumContinuousSize* continuous_size; /* (not used by PDM) */
    } ms; /* "ms" is short for medium-size */

} PdmOidMediumSourceSize;

typedef struct
{
    PdmOidMediumSourceSize* mss;
    int count;
} PdmOidMediumSS;

typedef struct
{
    PdmOid input_tray;
    PdmOid medium;
} PdmOidTrayMedium;

typedef struct
{
    PdmOidTrayMedium* list;
    int count;
} PdmOidTrayMediumList;

typedef enum {
    PDMOID_NOTIFY_UNSUPPORTED,
    PDMOID_NOTIFY_NONE,
    PDMOID_NOTIFY_EMAIL
} PdmOidNotify;

/*
 * PdmOid public methods
 */
extern const char* PdmOidString(PdmOid);
extern int PdmOidStringLength(PdmOid);
extern PdmOid PdmOidFromString(const char* value);
extern int PdmOidMsgSet(PdmOid);
extern int PdmOidMsgNum(PdmOid);
extern const char* PdmOidDefaultMsg(PdmOid);
extern const char* PdmOidLocalString(PdmOid pdm_oid);

/*
 * PdmOidList public methods
 */
extern PdmOidList* PdmOidListNew(const char* value_string);
extern void PdmOidListDelete(PdmOidList*);
#define PdmOidListCount(l) ((l) ? (l)->count : 0)
#define PdmOidListGetOid(l, i) ((l) ? (l)->list[(i)] : pdmoid_none)
extern int PdmOidListGetIndex(const PdmOidList* list, PdmOid oid);
extern Boolean PdmOidListHasOid(const PdmOidList* list, PdmOid oid);

/*
 * PdmOidLinkedList public methods
 */
extern PdmOidLinkedList* PdmOidLinkedListNew();
extern void PdmOidLinkedListDelete(PdmOidLinkedList*);
#define PdmOidLinkedListCount(l) ((l) ? (l)->count : 0)
extern PdmOid PdmOidLinkedListGetOid(PdmOidLinkedList* list, int i);
extern void PdmOidLinkedListAddOid(PdmOidLinkedList* list, PdmOid oid);
extern int PdmOidLinkedListGetIndex(PdmOidLinkedList* list, PdmOid oid);
extern Boolean PdmOidLinkedListHasOid(PdmOidLinkedList* list,
				      PdmOid oid);
extern PdmOid PdmOidLinkedListFirstOid(PdmOidLinkedList* list);
extern PdmOid PdmOidLinkedListNextOid(PdmOidLinkedList* list);

/*
 * PdmOidMediumSourceSize public methods
 */
extern PdmOidMediumSS* PdmOidMediumSSNew(const char* value_string);
extern void PdmOidMediumSSDelete(PdmOidMediumSS*);
#define PdmOidMediumSSCount(me) ((me) ? (me)->count : 0)
extern PdmOidLinkedList* PdmOidMediumSSGetAllSizes(PdmOidMediumSS*);
extern void PdmOidMediumSSGetTraysSizes(PdmOidMediumSS*,
					PdmOidTrayMediumList*,
					PdmOidList** trays,
					PdmOidList** sizes);

/*
 * PdmOidTrayMediumList public methods
 */
extern PdmOidTrayMediumList* PdmOidTrayMediumListNew(const char* value_string);
extern void PdmOidTrayMediumListDelete(PdmOidTrayMediumList* me);
#define PdmOidTrayMediumListCount(me) ((me) ? (me)->count : 0)
#define PdmOidTrayMediumListTray(me, i) \
    ((me) ? (me)->list[(i)].input_tray : pdmoid_none)
#define PdmOidTrayMediumListMedium(me, i) \
    ((me) ? (me)->list[(i)].medium : pdmoid_none)

/*
 * PdmOidNotify public methods
 */
extern PdmOidNotify PdmOidNotifyParse(const char* value_string);
extern const char* PdmOidNotifyString(PdmOidNotify notify);

/*
 * PdmOidDocumentFormat public methods
 */
extern char* PdmOidDocumentFormatParse(const char* value_string);
extern char* PdmOidDocumentFormatDefault(const char* value_string);

#endif /* _PdmOid_h - don't add any thing after this line */
