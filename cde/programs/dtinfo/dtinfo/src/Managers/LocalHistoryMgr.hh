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
 * $XConsortium: LocalHistoryMgr.hh /main/4 1996/06/11 16:25:58 cde-hal $
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

#include "UAS.hh"

struct Anchor;

class LocalHistoryEntry
{
private:  // functions
  LocalHistoryEntry (UAS_Pointer<UAS_Common> &doc_ptr)
    : f_doc_ptr (doc_ptr), f_anchor (NULL), f_previous(NULL), f_next(NULL) { }
  LocalHistoryEntry()
    { }
  ~LocalHistoryEntry();

private:  // variables
friend class LocalHistoryMgr;
  UAS_Pointer<UAS_Common>  f_doc_ptr;

  Anchor	         *f_anchor;

  LocalHistoryEntry *f_previous;
  LocalHistoryEntry *f_next;
};

class LocalHistoryMgr
{
public:
  LocalHistoryMgr();
  ~LocalHistoryMgr();

  void library_removed (UAS_Pointer<UAS_Common> &doc);

  UAS_Pointer<UAS_Common> current_doc()
    { return (f_current->f_doc_ptr); }

  // NOTE: This anchor code needs to be integrated into the back-end. 

  Anchor *previous_anchor()
    { return (f_current->f_previous->f_anchor); }

  Anchor *next_anchor()
    { return (f_current->f_next->f_anchor); }

  // The history takes over the anchor and will delete it. 
  void set_anchor (Anchor *anchor);

  UAS_Pointer<UAS_Common> previous()
    { if (f_current->f_previous == NULL) return (NULL);
      else return (f_current->f_previous->f_doc_ptr);
    }

  UAS_Pointer<UAS_Common> next()
    { if (f_current->f_next == NULL) return (NULL);
      else return (f_current->f_next->f_doc_ptr);
    }

  void go_back()
    { assert (f_current->f_previous != NULL);
      f_current = f_current->f_previous; }

  void go_forward()
    { assert (f_current->f_next != NULL);
      f_current = f_current->f_next; }

  void append (UAS_Pointer<UAS_Common> &doc_ptr);

private: // functions
  void truncate();

private: // variables
  LocalHistoryEntry *f_first;
  LocalHistoryEntry *f_current;
};
