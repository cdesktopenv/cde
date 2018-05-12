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
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: QueryGroupView.hh /main/4 1996/08/30 17:21:57 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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


class QueryGroup;
class QueryTerm;
class QueryTermView;
class QueryEditor;

#include <WWL/WXmForm.h>
#include "Widgets/WRestraint.h"

class QueryGroupView : public WWL, public FolioObject
{
public:
  friend class QueryTermView;

  QueryGroupView (QueryGroup *group, Widget parent);
  ~QueryGroupView();

  QueryGroup *query_group()
    { return (f_query_group); }
  void display()
    { f_form.Manage(); }
  void destroy_widgets()
    {
	f_restraint.Destroy();
	f_restraint = (Widget)0;
    }

//#ifdef UseFJMTF
#if 1
  void re_load_terms(QueryGroup *);
#endif

  operator Widget()
    { return ((Widget) f_form); }

  Dimension Height()
    { return (f_form.Height()); }

  void traverse_here();

private: // functions
  static void resize (Widget, Widget, XConfigureEvent *);

private: // variables
  QueryGroup       *f_query_group;
  WRestraint        f_restraint;
  WXmForm           f_form;
  QueryTermView    *f_term_view_list;
};
