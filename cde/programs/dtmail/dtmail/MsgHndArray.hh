/*
 *+SNOTICE
 *
 *	$TOG: MsgHndArray.hh /main/9 1998/09/02 15:54:58 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef	_MSGHNDARRAY_HH
#define	_MSGHNDARRAY_HH

#include <stdlib.h>
#include <string.h>
#if defined(USL) && (OSMAJORVERSION == 2)
extern "C" {
#endif
#if defined(USL) && (OSMAJORVERSION == 2)
};
#endif
#include <DtMail/DtMail.hh>

class MsgStruct {
  public:
    MsgStruct() {};
    ~MsgStruct() {};
    
    int indexNumber;
    int sessionNumber;
    DtMailMessageHandle message_handle;
    Boolean is_deleted;
};

class MsgHndArray {
  protected:
    int _size;
    MsgStruct **_contents;
    int _length;

  public:
    MsgHndArray(int sz = 1024, int zeroed = 1)
      {
	  _contents = (MsgStruct**) malloc(sizeof(MsgStruct*)*sz);
	  _size = sz;
	  if (zeroed) memset(_contents, 0, sizeof(MsgStruct *)*sz);

	  _length = 0;

      }

    ~MsgHndArray()
      {
//	  for (int ent = 0; ent < _length; ent++) {
//	      delete _contents[ent];
//	  }

	  free((void*) _contents);
      }

    int		length();
    MsgStruct*	at(int a_number);
    int		insert(MsgStruct* a_msg_struct);
    void	clear();
    int		indexof(MsgStruct* a_msg_struct);
    int		indexof(DtMailMessageHandle a_msg_handle);
    void	remove_entry(int position);
    void	remove_entry(MsgStruct *ms);
    void	append(MsgStruct *a_msg_struct);
    void	mark_for_delete(int position);
    void	compact(int start_pos);
    void	replace(int postition, MsgStruct *a_msg_struct);
};

#endif
    
    
