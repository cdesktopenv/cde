/*
 * $XConsortium: slist_char_ptr_cell.h /main/3 1996/06/11 17:18:05 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */



#ifndef _slist_char_ptr_cell_h
#define _slist_char_ptr_cell_h 1

#include "utility/types.h"
#include "dstr/slist_void_ptr_cell.h"

// single-linked list cell class, charPtr as data ptr.

class slist_char_ptr_cell : public slist_void_ptr_cell {

public:
   slist_char_ptr_cell(char* vp, Boolean allocated) : 
     slist_void_ptr_cell(vp), v_alloca(allocated) {};
   virtual ~slist_char_ptr_cell() ;

   char* char_ptr() { return (char*)data; };
   void set_char_ptr(char*x, Boolean allocated) { 
     data = x; 
     v_alloca = allocated;
   };

protected:
   Boolean v_alloca;
};


#endif
