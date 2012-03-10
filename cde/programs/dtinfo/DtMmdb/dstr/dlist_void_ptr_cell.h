/* $XConsortium: dlist_void_ptr_cell.h /main/3 1996/06/11 17:17:00 cde-hal $ */


#ifndef _dlist_void_ptr_cell_h
#define _dlist_void_ptr_cell_h 1

#include "utility/types.h"
#include "dstr/dlist_cell.h"

// doubly-linked list cell class, voidPtr as data ptr.

class dlist_void_ptr_cell : public dlist_cell {

public:
   dlist_void_ptr_cell(voidPtr vp) : data(vp) {};
   virtual ~dlist_void_ptr_cell() {};

   voidPtr void_ptr() { return data; };

protected:
   voidPtr data;
};

typedef dlist_void_ptr_cell *dlist_void_ptr_cellPtr;

#endif
