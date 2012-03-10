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









