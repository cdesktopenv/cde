// $XConsortium: page_rep.cc /main/3 1996/06/11 17:44:32 cde-hal $

#include "storage/page_rep.h"

void page_rep_del(const void* x)
{
  delete (page_rep*)x;
}

Boolean page_rep_ls(const void* x, const void* y)
{
   if ( ((page_rep*)x) -> f_page_num < ((page_rep*)y) -> f_page_num )
      return true;
   else
      return false;
}

Boolean page_rep_eq(const void* x, const void* y)
{
   if ( ((page_rep*)x) -> f_page_num == ((page_rep*)y) -> f_page_num )
      return true;
   else
      return false;
}


