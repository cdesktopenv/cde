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

