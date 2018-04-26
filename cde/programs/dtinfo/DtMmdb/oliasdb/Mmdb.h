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
/* $XConsortium: Mmdb.h /main/4 1996/05/29 13:01:24 rcs $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc.
 * (c) Copyright 1994, 1995, 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _MMDB_C_API_H
#define _MMDB_C_API_H



#define DtMmdb_PROTO1(x) (x)
#define DtMmdb_PROTO2(x,y) (x,y)
#define DtMmdb_PROTO3(x,y,z) (x,y,z)

#undef _not_defined
#ifdef _not_defined
#ifdef __cplusplus
#else
#define DtMmdb_PROTO1(x) ()
#define DtMmdb_PROTO2(x,y) ()
#define DtMmdb_PROTO3(x,y,z) ()
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int DtMmdbBool;
#define DtMmdbTrue 1
#define DtMmdbFalse 0

/*****************************************************/
/* info lib structure */
/*****************************************************/
typedef struct _DtMmdbInfoLibInfo 
{
   const char* path; 
   const char* name; 
   unsigned int num_bookcases;
} DtMmdbInfoLibInfo;

/*****************************************************/
/* basecase structure */
/*****************************************************/
typedef struct _DtMmdbBookCaseInfo 
{
   const char* name; 
   unsigned int num_books;
} DtMmdbBookCaseInfo;

/*****************************************************/
/* oid handler structure */
/*****************************************************/
typedef struct _DtMmdbObjectId 
{
   void* oid_ptr;
} DtMmdbHandle;

extern DtMmdbHandle* DtMmdbGroundId;

/*****************************************************/
/* info request structure */
/*****************************************************/
typedef struct _DtMmdbInfoRequest 
{
   int bookcase_descriptor;
   char* locator_ptr;
   DtMmdbHandle* primary_oid;
   DtMmdbHandle* secondary_oid;
   int sequence_num;
} DtMmdbInfoRequest ;


/*****************************************************/
/* graphic info structure */
/*****************************************************/
typedef struct _DtMmdbGraphicInfo 
{
   unsigned short type;
   unsigned short width;
   unsigned short height;
   unsigned int llx, lly ;
   unsigned int urx, ury ;
   char* version;
} DtMmdbGraphicInfo;


/*****************************************************/
/* mmdb */
/*****************************************************/
void DtMmdbInit();
void DtMmdbQuit();

/*****************************************************/
/* Handle space release*/
/*****************************************************/
void DtMmdbFreeHandle(DtMmdbHandle*);
void DtMmdbFreeHandleList(DtMmdbHandle**);

/*****************************************************/
/* infolib */
/*****************************************************/
int DtMmdbOpenInfoLib DtMmdb_PROTO3(
                const char* infolib_path, 
                const char* selected_base_name,
                DtMmdbBool delayed_infolib_init
               );

void DtMmdbCloseInfoLib DtMmdb_PROTO1(int infolib_descriptor);

DtMmdbInfoLibInfo* DtMmdbInfoLibGetInfo DtMmdb_PROTO1(int infolib_descriptor);

void DtMmdbInfoLibFreeInfo DtMmdb_PROTO1(DtMmdbInfoLibInfo*);


/*****************************************************/
/* bookcase */
/*****************************************************/
int 
DtMmdbGetBookCaseByName 
DtMmdb_PROTO2(int infolib_descriptor, const char* name);

int 
DtMmdbGetBookCaseByIndex DtMmdb_PROTO2(int infolib_descriptor, int index);

int 
DtMmdbGetBookCaseByLoc DtMmdb_PROTO2(
	int infolib_descriptor, 
	const char* locator
	);

int* 
DtMmdbGetBookCaseByLocs DtMmdb_PROTO2(
	int infolib_descriptor, 
	const char** locators
	);

DtMmdbBookCaseInfo* DtMmdbBookCaseGetInfo DtMmdb_PROTO1(int bookcase_descriptor);
void DtMmdbBookCaseFreeInfo DtMmdb_PROTO1(DtMmdbBookCaseInfo*);

/*****************************************************/
/* section  */
/*****************************************************/
const char* DtMmdbSectionGetLoc DtMmdb_PROTO1(DtMmdbInfoRequest* request);

const char* 
DtMmdbSectionGetLongTitle DtMmdb_PROTO2(
	DtMmdbInfoRequest* request,
	unsigned int* title_length
	);

const char* 
DtMmdbSectionGetShortTitle DtMmdb_PROTO2(
	DtMmdbInfoRequest* request,
	unsigned int* title_length
	);

const char* 
DtMmdbSectionGetData  DtMmdb_PROTO2(
	DtMmdbInfoRequest* request,
	unsigned int* data_length
	);

int DtMmdbSectionGetDataSize DtMmdb_PROTO1(DtMmdbInfoRequest* request);
	
const char* DtMmdbSectionGetTocLoc DtMmdb_PROTO1( DtMmdbInfoRequest* request);

DtMmdbHandle* 
DtMmdbSectionGetBookId DtMmdb_PROTO1( DtMmdbInfoRequest* request);

DtMmdbHandle* 
DtMmdbSectionGetStyleSheetId DtMmdb_PROTO1(DtMmdbInfoRequest* request);

/*****************************************************/
/* TOC */
/*****************************************************/
DtMmdbHandle* 
DtMmdbTocGetParentId DtMmdb_PROTO1(DtMmdbInfoRequest* request);

DtMmdbHandle** 
DtMmdbTocGetChildIds DtMmdb_PROTO2(
	DtMmdbInfoRequest* request, 
	unsigned int* list_length
	);

int DtMmdbTocGetNumOfChildren DtMmdb_PROTO1( DtMmdbInfoRequest* request);

/*****************************************************/
/* Locator */
/*****************************************************/
const char* 
DtMmdbLocatorGetSectionLoc DtMmdb_PROTO1(DtMmdbInfoRequest* request);

DtMmdbHandle* 
DtMmdbLocatorGetSectionObjectId DtMmdb_PROTO1(DtMmdbInfoRequest* request);

/*****************************************************/
/* Graphic */
/*****************************************************/
const char* 
DtMmdbGraphicGetData DtMmdb_PROTO2(
	DtMmdbInfoRequest* request, 
	unsigned int* data_length
	);

DtMmdbGraphicInfo* 
DtMmdbGraphicGetInfo DtMmdb_PROTO1(DtMmdbInfoRequest* request);
void DtMmdbFreeGraphicInfo(DtMmdbGraphicInfo*);


/*****************************************************/
/* Stylesheet */
/*****************************************************/
const char* 
DtMmdbStylesheetGetName DtMmdb_PROTO1(DtMmdbInfoRequest* request);

const char* 
DtMmdbStylesheetGetata DtMmdb_PROTO2(
	DtMmdbInfoRequest* request, 
	unsigned int* data_length
	);

/*****************************************************/
/* BOOk  */
/*****************************************************/
DtMmdbHandle* 
DtMmdbBookGetTocObjectId DtMmdb_PROTO1(DtMmdbInfoRequest* request);

const char* 
DtMmdbBookGetShortTitle DtMmdb_PROTO2(
	DtMmdbInfoRequest* request, 
	unsigned int* length
	);

const char* 
DtMmdbBookGetLongTitle DtMmdb_PROTO2(
	DtMmdbInfoRequest* request, 
	unsigned int* length
	);

int DtMmdbBookGetSeqNum DtMmdb_PROTO1(DtMmdbInfoRequest* request);

const char* 
DtMmdbBookGetSeqLIcense DtMmdb_PROTO2(
	DtMmdbInfoRequest* request, 
	unsigned int* length
	);

DtMmdbHandle** DtMmdbBookGetTabList DtMmdb_PROTO2(
	DtMmdbInfoRequest* request, unsigned int* length
	);

/*****************************************************/
/* DLP */
/*****************************************************/
DtMmdbHandle* 
DtMmdbDlpGetPrevSectionId  DtMmdb_PROTO1(DtMmdbInfoRequest* request);

DtMmdbHandle* 
DtMmdbDlpGetNextSectionId  DtMmdb_PROTO1(DtMmdbInfoRequest* request);

DtMmdbInfoRequest* newDtMmdbInfoRequestWithLoc(int bc_id, char* loc);
DtMmdbInfoRequest* newDtMmdbInfoRequestWithPrimaryOid(int bc_id, char* oid_str);
DtMmdbInfoRequest* newDtMmdbInfoRequestWithSecondaryOid(int bc_id, char* oid_str);
DtMmdbInfoRequest* newDtMmdbInfoRequestWithSeqnum(int bc_id, char* oid_str);
char* DtMmdbHandleToString(DtMmdbHandle* x);
void DtMmdbFreeInfoRequest(DtMmdbInfoRequest*);

#ifdef __cplusplus
}
#endif



#endif 

