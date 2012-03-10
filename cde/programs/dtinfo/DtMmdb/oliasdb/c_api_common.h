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
