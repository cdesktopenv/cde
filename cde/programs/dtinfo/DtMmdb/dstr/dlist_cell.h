/*
 * $XConsortium: dlist_cell.h /main/3 1996/06/11 17:16:55 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#ifndef _dlist_cell_h
#define _dlist_cell_h 1

#include "utility/debug.h"

// doubly-linked list cell class

class dlist_cell {

public:
   dlist_cell() ;
   virtual ~dlist_cell() ;

   dlist_cell *v_pred ;  // prec pointer
   dlist_cell *v_succ ;  // succ pointer

};

typedef dlist_cell *dlist_cellPtr;

#endif
