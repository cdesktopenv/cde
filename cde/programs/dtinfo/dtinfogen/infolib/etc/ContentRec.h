/* $XConsortium: ContentRec.h /main/2 1996/07/18 16:41:34 drk $ */
#ifndef __CRec_hdr__
#define __CRec_hdr__

#include "VarElementList.h"

class ContentRec {

friend class OL_Data;
friend int ol_dataparse();
friend class ContentType;
  
private:
  int            data_type;
  int            attr_name;
  VarElementList var_list;  
  ContentRec *next;

public:
  void Init ( int dtype, int aname=0, VarElement *vList=0 );
  ContentRec() { attr_name = -1; data_type=-1, next = 0; }
};

/*--------------------------------------------------------*/
inline
void
ContentRec::Init( int dtype, int aname, VarElement *vlist )
{
  data_type = dtype;
  attr_name = aname;
  var_list.insert ( vlist );
}

#endif
