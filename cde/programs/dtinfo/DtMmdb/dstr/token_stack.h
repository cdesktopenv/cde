/*
 * $XConsortium: token_stack.h /main/3 1996/06/11 17:18:21 cde-hal $
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



#ifndef _token_stack_h
#define _token_stack_h 1

#include "utility/debug.h"
#include "utility/buffer.h"
#include "dstr/slist.h"
#include "dstr/slist_void_ptr_cell.h"

class token_stack 
{
public:
   token_stack();
   virtual ~token_stack();

   void clear();

   void new_token();
   void add_partial_token(char*);

   char* token_start() { return curr_token_start; };

   friend ostream& operator <<(ostream&, token_stack&);

protected:
   slist chunk_list;
   char* curr_token_start;
   buffer* v_curr_token_buf;
   slist_void_ptr_cell * v_curr_list_cell;
};

typedef token_stack *token_stackPtr;

#endif
