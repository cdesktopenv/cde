/*
 * $XConsortium: code.h /main/3 1996/06/11 17:15:01 cde-hal $
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


#ifndef _code_h
#define _code_h 1

#include "utility/ostring.h"
////////////////////////////////////////
//
////////////////////////////////////////

class htr_node;

class encoding_unit
{
public:
   ostring* word;
   int bits;
   unsigned int code;
   unsigned int freq;
   htr_node* leaf_htr_node;

public:
   encoding_unit(ostring* w, unsigned int freq);
   ~encoding_unit();

   friend ostream& operator <<(ostream&, encoding_unit&);
   
};

typedef encoding_unit* encoding_unitPtr;

#endif
