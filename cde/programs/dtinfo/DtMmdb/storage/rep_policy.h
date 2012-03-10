/*
 * $XConsortium: rep_policy.h /main/4 1996/06/11 17:34:37 cde-hal $
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


#ifndef _rep_policy_h
#define _rep_policy_h 1

#include "utility/funcs.h"
#include "dstr/dlist.h"
#include "storage/rep_cell.h"

class rep_policy {

protected:
   int active_sz;
   int inactive_sz;

public:
   rep_policy(int a_sz, int i_sz) ;
   virtual ~rep_policy() ;

   void set_params(int a_sz, int i_sz) 
   { active_sz = a_sz; inactive_sz = i_sz; };

// promotes the cell x to the window.
   virtual Boolean promote(rep_cell&, rep_cell*& replaced) = 0;
   virtual Boolean promote(rep_cell& x) = 0;

   virtual Boolean remove(rep_cell& x) = 0; 
};

typedef rep_policy *rep_policyPtr;

#endif
