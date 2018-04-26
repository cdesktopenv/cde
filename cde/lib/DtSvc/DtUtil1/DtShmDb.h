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
/* $XConsortium: DtShmDb.h /main/4 1996/05/09 04:22:30 drk $ */
#ifndef DtShmDb_h
#define DtShmDb_h

/*
  this include file provides prototypes for the various
  shared memory database routines
  */

typedef void * 	DtShmProtoStrtab;
typedef void * 	DtShmProtoInttab;
typedef void * 	DtShmProtoIntList;

typedef const void * 	DtShmStrtab;
typedef const void * 	DtShmInttab;
typedef const int * 	DtShmIntList;
typedef int	DtShmBoson;

/*
  routines used while building shared memory databases
  */

DtShmProtoStrtab 	  _DtShmProtoInitStrtab (int estimated_entries);
DtShmBoson 		   _DtShmProtoAddStrtab (DtShmProtoStrtab prototab, const char * string, int * isnew);
const char *		_DtShmProtoLookUpStrtab (DtShmProtoStrtab prototab, DtShmBoson boson);
int			  _DtShmProtoSizeStrtab (DtShmProtoStrtab prototab);
DtShmStrtab		  _DtShmProtoCopyStrtab (DtShmProtoStrtab prototab, void * dataspace);
int		       _DtShmProtoDestroyStrtab (DtShmProtoStrtab prototab);

DtShmProtoInttab 	  _DtShmProtoInitInttab (int estimated_entries);
int	 		   _DtShmProtoAddInttab (DtShmProtoInttab prototab, unsigned int keyin, int datain);
int	*		_DtShmProtoLookUpInttab (DtShmProtoInttab prototab, unsigned int keyin);
int			  _DtShmProtoSizeInttab (DtShmProtoInttab prototab);
DtShmInttab		  _DtShmProtoCopyInttab (DtShmProtoInttab prototab, void * dataspace);
int		       _DtShmProtoDestroyInttab (DtShmProtoInttab prototab);


DtShmProtoIntList 	  _DtShmProtoInitIntLst (int estimated_entries);
int	* 		   _DtShmProtoAddIntLst (DtShmProtoIntList protolist, int size, int * index_value);
int			  _DtShmProtoSizeIntLst (DtShmProtoIntList protolist);
DtShmIntList		  _DtShmProtoCopyIntLst (DtShmProtoIntList protolist, void * dataspace);
int		       _DtShmProtoDestroyIntLst (DtShmProtoIntList protolist);


/*
  run-time routines once shared memory area is built
  */

DtShmBoson		       _DtShmStringToBoson (DtShmStrtab tab, const char * string);
const char * 		       _DtShmBosonToString (DtShmStrtab tab, DtShmBoson boson);
const int *	          _DtShmFindIntTabEntry (DtShmInttab tab, unsigned int key);

#endif /* DtShmDb_h */

