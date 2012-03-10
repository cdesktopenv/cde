/* $XConsortium: Expression.h /main/3 1996/08/21 15:46:44 drk $ */
#ifndef __CRec_hdr__
#define __CRec_hdr__

enum OL_DATA_TYPE {
  INVALID_OL_TYPE,
  CONTENT,
  CONCAT,
  FIRSTOF,
  GENERIC_ID,
  LITERAL,
  REFERENCE
};

class OL_Expression {

friend class OL_Data;
friend class FirstOf;
friend class Concat;  
friend int ol_dataparse();
friend class ContentType;
friend class ExprList;
  
private:
  OL_DATA_TYPE   data_type;
  int            ename;
  void           *value_list;  
  OL_Expression  *next;

public:
  OL_Expression( OL_DATA_TYPE dtype=INVALID_OL_TYPE,
	      int dname=-1,
	      void *vlist=0) {
    data_type = dtype; ename = dname; value_list = vlist; next = 0;
  }
  
  ~OL_Expression();
  
  OL_DATA_TYPE type()       const { return data_type; }
  int          name()       const { return ename;      }
  void         *data_list() const { return value_list; }

};

#endif
