/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *+SNOTICE
 *
 *	$TOG: MsgHndArray.C /main/6 1998/09/02 15:54:28 mgreess $
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

#include "MsgHndArray.hh"
#include "MemUtils.hh"

MsgStruct*
MsgHndArray::at(int indx)
{
    return(_contents[indx]);
}

void
MsgHndArray::clear()
{
    memset(_contents, 0, sizeof(MsgStruct *)*_size);
}

int
MsgHndArray::length()
{
    return(_length);
}


int
MsgHndArray::indexof(MsgStruct *a_msg_struct)
{
    int tmp;

    for (tmp = 0; tmp < _length; tmp++)
      {
	  if ((_contents[tmp]->sessionNumber == a_msg_struct->sessionNumber) &&
	      (_contents[tmp]->message_handle == a_msg_struct->message_handle))
	    {
		return(tmp);
	    }
      }
    return(-1);
}

int
MsgHndArray::indexof(DtMailMessageHandle a_message_handle)
{
    int tmp;

    for (tmp = 0; tmp < _length; tmp++)
      {
	  if (_contents[tmp]->message_handle == a_message_handle)
	    {
		return(tmp);
	    }
      }
    return(-1);
}
    
void
MsgHndArray::remove_entry(MsgStruct *ms)
{
    for (int i=0; i<_length; i++)
    {
	if (ms == _contents[i]) 
	{
	    remove_entry(i);
	    return;
	}
    }
}


void
MsgHndArray::remove_entry(int position)
{
    int i;
    FORCE_SEGV_DECL(MsgStruct, tmpMS);

    if ((position < 0) || (position >= _length)) return;

    tmpMS = _contents[position];

    for (i=position; i<(_length-1); i++)
      _contents[i] = _contents[i+1];

    _contents[_length] = NULL;
    _length -= 1;
    
}


int
MsgHndArray::insert(
    MsgStruct *a_msg_struct
)
{
    int i, j, orig_size, return_val;
    int sess_num;
    Boolean found;
    FORCE_SEGV_DECL(MsgStruct, tmpMS1);
    FORCE_SEGV_DECL(MsgStruct, tmpMS2);

    found = FALSE;
    sess_num = a_msg_struct->sessionNumber;

    for(i = 0; i < _length; i++) {
	tmpMS1 = _contents[i];
	if (tmpMS1->sessionNumber > sess_num) {
	    _contents[i] = a_msg_struct;
	    return_val = i;
	    found = TRUE;
	    break;
	}
    }
    if (found == TRUE) {
	_length++;

    // If we hit size, then grow by 25% to allow more entries.
    // Zero out the added portion.

	if (_length == _size) {
	    orig_size = _size;
	    _size += (_size >> 2);
	    _contents = (MsgStruct **)realloc(_contents, 
					      _size * sizeof(MsgStruct *));
	    memset(_contents+orig_size, 0, sizeof(MsgStruct *)*_size);
	}

	for (j = i + 1; j < _length; j++) {
	    tmpMS2 = _contents[j];
	    _contents[j] = tmpMS1;
	    tmpMS1 = tmpMS2;
	}
    }
     else {
	 _contents[_length] = a_msg_struct;
	 return_val = _length;
	 _length++;
     }
    return(return_val);

}

void
MsgHndArray::append(
    MsgStruct *a_msg_struct
)

{

    _contents[_length] = a_msg_struct;
    _length++;

    // If we hit size, then grow by 25% to allow more entries.

    if (_length == _size) {
	_size += (_size >> 2);
	_contents = (MsgStruct **)realloc(_contents, 
					 _size * sizeof(MsgStruct *));
    }

}

void
MsgHndArray::mark_for_delete(int indx)
{
    _contents[indx]->is_deleted = TRUE;
}

void
MsgHndArray::compact(
    int start_pos
)
{
    FORCE_SEGV_DECL(MsgStruct, tmpMS);
    int i;

    if ((_length <= 0) || (start_pos < 0) || (start_pos >= _length))
	return;

    for (i = _length - 1; i >= start_pos; i--) {
	tmpMS = _contents[i];
	if (tmpMS->is_deleted) {
	    tmpMS->is_deleted = FALSE;
	    remove_entry(i);
	    compact(i);
	}
    }
}

    
//
// Replace the MsgStruct at 'position' with the passed MsgStruct.
// The MsgStruct previously located at 'position' is *not* destroyed.
//
void
MsgHndArray::replace(
	int position,
	MsgStruct *a_msg_struct
)
{
    if (position < 0 || position >= _length) {
	return;
    }

    _contents[position] = a_msg_struct;

    return;
}
