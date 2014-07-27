/*
 * $XConsortium: MarkBase.hh /main/3 1996/06/11 16:29:16 cde-hal $
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

#include "UAS.hh"

template <class T> class xList;
class Mark;
struct Anchor;

class MarkBase
{
public:
  static MarkBase *open (const char *filename, bool read_only);
  static MarkBase *create (const char *filename, bool read_only,
			   const char *format);

  virtual ~MarkBase();

  virtual UAS_Pointer<Mark> create_mark (UAS_Pointer<UAS_Common> &, const Anchor &,
				     const char *name, const char *notes) = 0;

  virtual void get_marks (UAS_Pointer<UAS_Common> &doc_ptr,
			  xList<UAS_Pointer<Mark> > &list) = 0;

  virtual void get_all_marks (xList<UAS_Pointer<Mark> > &list) = 0;

  virtual bool read_only() = 0;

  typedef MarkBase * (*open_func_t) (const char *filename, bool read_only);
  static int register_open_func (open_func_t open_func);

  typedef MarkBase * (*mb_create_func_t) (const char *filename, bool read_only);
  static int register_create_func (mb_create_func_t create_func,
				   const char *format);
};
