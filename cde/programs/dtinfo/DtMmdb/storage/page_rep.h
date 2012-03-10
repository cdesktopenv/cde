/* $XConsortium: page_rep.h /main/3 1996/06/11 17:44:37 cde-hal $ */

#ifndef _page_rep
#define _page_rep 1

#include "utility/types.h"

class lru_page;

//////////////////////////////////////////////////
// struct for managing page number/page ptr pair
//////////////////////////////////////////////////
class page_rep 
{

public:
   page_rep(int pnum, lru_page* ptr = 0) {
      f_page_ptr = ptr;
      f_page_num = pnum; 
   };
   virtual ~page_rep() {};

public:
   lru_page* f_page_ptr;
   int       f_page_num;

protected:
};

Boolean page_rep_ls(const void*x, const void* y);
Boolean page_rep_eq(const void*x, const void* y);
void page_rep_del(const void* x);

#endif
