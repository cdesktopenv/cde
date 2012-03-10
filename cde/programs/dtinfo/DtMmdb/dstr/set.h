/*
 * $XConsortium: set.h /main/4 1996/06/11 17:17:41 cde-hal $
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



#ifndef _set_h
#define _set_h 1

#include  "utility/funcs.h"

//**************************************************************
//
// an abstract set class
//
//**************************************************************

class set {

public:
   set(cmp_func_ptr_t eq, cmp_func_ptr_t ls) :
     f_cmp_func_eq(eq), f_cmp_func_ls(ls) 
   {};
   virtual ~set() {};

   virtual Boolean insert(void* element) = 0;
   virtual void* remove(void* element) = 0;
   virtual void* member(const void* element) = 0;

   virtual void* smaller_member(const void* element) = 0;
   virtual void* larger_member(const void* element) = 0;


protected:
   cmp_func_ptr_t f_cmp_func_eq; 
   cmp_func_ptr_t f_cmp_func_ls; 
};

#endif
