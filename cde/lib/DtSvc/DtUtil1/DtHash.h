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
/* $XConsortium: DtHash.h /main/5 1996/08/29 15:42:13 cde-dec $ */
#ifndef _DtHash_h
#define _DtHash_h

typedef void * DtHashTbl;

DtHashTbl	 _DtUtilMakeHash(int size);
DtHashTbl 	_DtUtilMakeIHash(int size);
void ** 	  _DtUtilGetHash(DtHashTbl tbl, const unsigned char * key);
void ** 	 _DtUtilFindHash(DtHashTbl tbl,const unsigned char * key);
void * 		  _DtUtilDelHash(DtHashTbl tbl, const unsigned char * key);
int    	      _DtUtilOperateHash(DtHashTbl tbl, void (*op_func)(), void * usr_arg);
void   	      _DtUtilDestroyHash(DtHashTbl tbl, int (*des_func)(), void * usr_arg);

typedef void (*DtHashOperateFunc)();
typedef int  (*DtHashDestroyFunc)();

#endif /* _DtHash_h */









