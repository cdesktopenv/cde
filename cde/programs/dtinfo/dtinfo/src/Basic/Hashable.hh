/*
 * $XConsortium: Hashable.hh /main/3 1996/06/11 16:19:09 cde-hal $
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

// This is an abstract base class if you couldn't tell!

class Hashable : public FolioObject
{
public: // functions
  virtual u_int hash_code (u_int lower_bound, u_int upper_bound) const = 0;
  // NOTE: should change this to operator ==
  virtual bool equals (const Hashable &) const = 0;
  //  virtual operator == (const Hashable &) const = 0;
};
