/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: Mark.hh /main/3 1996/06/11 16:29:04 cde-hal $
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

 # include "UAS.hh"


struct Anchor;
struct MarkChanged;
struct MarkDeleted;
struct MarkMoved;
struct EditMark;
struct ViewMark;

class Mark
{
  friend class UAS_Pointer<Mark>;

public:
  Mark();
  virtual ~Mark();

  // Unfortunately, these are all static.  The reason is that when
  // we have a mix of per object and per class senders, the per object
  // send_message() method is inherited from the base class Sender,
  // but then the local statc send_message() methods end up hiding
  // the inherited versions.  Way to go C++...
  // (Maybe a DYNAMIC_SENDER_HH macro could define a method that just
  // put in a call to the base class send method?)  15:30 06-May-94 DJB
  // This should work (see below). 
  STATIC_SENDER_HH (EditMark);
  STATIC_SENDER_HH (ViewMark);
  STATIC_SENDER_HH (MarkDeleted);
  STATIC_SENDER_HH (MarkChanged);

  virtual UAS_Pointer<UAS_Common> &doc_ptr() = 0;
  virtual const Anchor &anchor() = 0;

  virtual const char *name() = 0;
  virtual void set_name (const char *name) = 0;

  virtual const char *notes() = 0;
  virtual void set_notes (const char *notes) = 0;

  virtual bool operator== (const Mark &);

  void view();
  void edit();
  
  void remove();
  void save();

protected:  // functions
  virtual void do_remove() = 0;
  virtual void do_save() = 0;

protected:
  virtual void reference();
  virtual void unreference();

protected:
  // Protect address-of from abuse. 
  Mark *operator &() const
    { return (Mark *)this; }

private:  // variables
  int  f_reference_count;
  bool f_deleted;               // True after mark is removed from database. 
};


struct MarkChanged : public Destructable
{
  UAS_Pointer<Mark> f_mark_ptr;
};

struct MarkCreated : public Destructable
{
  UAS_Pointer<Mark> f_mark_ptr;
};

struct MarkMoved : public Destructable
{
  UAS_Pointer<Mark> f_new_mark_ptr;
  UAS_Pointer<Mark> f_old_mark_ptr;
};

struct MarkDeleted : public Destructable
{
  UAS_Pointer<Mark> f_mark_ptr;
};

struct EditMark : public Destructable
{
  UAS_Pointer<Mark> f_mark_ptr;
};

struct ViewMark : public Destructable
{
  UAS_Pointer<Mark> f_mark_ptr;
};


/*

mark icon object has pointer to mark

mark mgr passes off display requests to the mark editor

the mark editor caches the edit windows, so that attempts
to edit the same mark from two different windows usese the same
edit panel

*/

// The following seems to work, but I don't want to bother with
// changing this class right now:

/*
void
Mark::move()
{
  MarkMoved m;
  UAS_Sender<MarkMoved>::send_message (m);
}
*/
