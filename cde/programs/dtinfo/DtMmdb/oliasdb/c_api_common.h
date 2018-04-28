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
/* $XConsortium: c_api_common.h /main/4 1996/06/11 17:27:02 cde-hal $ */

#ifndef cc_api_common_h
#define cc_api_common_h

#include "oliasdb/mmdb.h"
#include "oliasdb/DtMmdb.h"

extern OLIAS_DB* mmdb_ptr;
extern info_base* getBookCase(int);
extern oid_t* getPrimiaryOid(DtMmdbInfoRequest* x);
extern oid_t* getSecondaryOid(DtMmdbInfoRequest* x);
extern int getSeqNum(DtMmdbInfoRequest* x);
extern const char* getLocator(DtMmdbInfoRequest* x);
extern DtMmdbHandle* newDtMmdbHandle(const oid_t& x);
extern DtMmdbGraphicInfo* newDtMmdbGraphicInfo();


#endif
