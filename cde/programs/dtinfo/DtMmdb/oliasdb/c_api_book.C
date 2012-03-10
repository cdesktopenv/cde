// $XConsortium: c_api_book.cc /main/4 1996/06/11 17:26:37 cde-hal $

#include "oliasdb/c_api_common.h"

static doc_smart_ptr*
getDocUsingPrOrSecOrSeq(DtMmdbInfoRequest* request)
{
   try {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      oid_t * id = getPrimiaryOid(request);

      if ( id ) {
         return new doc_smart_ptr(x, *id);
      }

      id = getSecondaryOid(request); // book id

      if ( id ) {
         return new doc_smart_ptr(*id, x);
      }

      int seq = getSeqNum(request); // seq number

      if ( seq == -1 ) return 0;

// bookcase is treated 1 based internally. But 0-based externally.
      return new doc_smart_ptr(x, seq+1);
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle*
DtMmdbBookGetTocObjectId(DtMmdbInfoRequest* request)
{
   try {
      doc_smart_ptr* x = getDocUsingPrOrSecOrSeq(request);
      if ( x == 0 ) return 0;

      DtMmdbHandle *z = newDtMmdbHandle(x -> locator_id());

      delete x;
      return z;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbBookGetShortTitle(DtMmdbInfoRequest* request, unsigned int* length)
{
   try {
      doc_smart_ptr* x = getDocUsingPrOrSecOrSeq(request);
      if ( x == 0 ) return 0;

      const char* z = x -> short_title();

      if ( length ) *length = strlen(z);

      delete x;
      return z;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbBookGetLongTitle(DtMmdbInfoRequest* request, unsigned int* length)
{
   try {
      doc_smart_ptr* x = getDocUsingPrOrSecOrSeq(request);
      if ( x == 0 ) return 0;

      const char* z = x -> long_title();

      if ( length ) *length = strlen(z);

      delete x;
      return z;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

int DtMmdbBookGetSeqNum(DtMmdbInfoRequest* request)
{
   try {
      doc_smart_ptr* x = getDocUsingPrOrSecOrSeq(request);
      if ( x == 0 ) return 0;

      int z = x -> seq_num();

      delete x;
      return z;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbBookGetSeqLIcense(DtMmdbInfoRequest* request, unsigned int* length)
{
    try {
      doc_smart_ptr* x = getDocUsingPrOrSecOrSeq(request);
      if ( x == 0 ) return 0;

      const char* z = x -> license_terms();
      if ( length ) 
        *length = x -> license_terms_size();

      delete x;
      return z;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle** DtMmdbBookGetTabList(DtMmdbInfoRequest* request, unsigned int* length)
{
   try {
      doc_smart_ptr* x = getDocUsingPrOrSecOrSeq(request);
      if ( x == 0 ) return 0;

      short_list_handler* z = x -> tab_list();

      if ( z == 0 ) return 0;

      int count = (*z) -> count();

      DtMmdbHandle** u = (DtMmdbHandle**)malloc(sizeof(DtMmdbHandle*)* (count+1));

      if ( u == 0 ) return 0;
         
      const char* desc = 0;
      pstring_handler *p = 0;

      for (int i=0; i<count; i++) {
         p = (pstring_handler *)((*z)->get_component (i+1));

// The format is the title, a tab char, then the section oid.
         desc = (*p)->get();

         while (*desc != '\0' && *desc != '\t') desc++;

         if (*desc != '\t')
           return 0;

         desc++;

         oid_t w((char*)desc, true, false);
         u[i] = newDtMmdbHandle(w);

         delete p;

      }

      u[i] = 0;

      if ( length ) *length = count;

      delete z;
      delete x;
      return u;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}
