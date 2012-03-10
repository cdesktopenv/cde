/*
 * $XConsortium: pattern.h /main/3 1996/06/11 17:20:42 cde-hal $
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



#ifndef _int_pattern_h
#define _int_pattern_h 1

#include "utility/funcs.h"
#include "utility/debug.h"

class int_pattern 
{

public:
   int_pattern(int size = 0);
   virtual ~int_pattern();

   void reset_elmts(int i) { v_count = i; };  
   int no_elmts() { return v_count; }; 

   int operator [](int i) { return v_rep[i]; };

   void insert(int elmt, int i) { v_rep[i] = elmt; }; 

   int duplicate();                // duplication check.

   friend ostream& operator <<(ostream&, int_pattern&);

protected:
   unsigned int v_rep_size ;    
   unsigned int v_count   ;    
   int *v_rep;  
};

#endif
