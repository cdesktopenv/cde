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
/* $XConsortium: DtActions.h /main/3 1995/11/06 09:34:07 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTACTIONS_H
#define DTACTIONS_H

#include "Menu.h"

typedef void (*ActionCallback) (void *callback_data, BaseUI *object,
				char *actionReferenceName);

class DtWorkArea;

class DtActions : public Menu
{

  friend class DtWorkArea;

 protected:
  
   int n_categories;
   char **categories;
   void UpdateActions(int n_items, BaseUI *obj);
   BaseUI *selected_object;

 public:

   DtActions(AnyUI *parent,
	     char *name,
	     char *mnemonic = NULL);

   void AddAction(char *name, char *category, 
		  char *actionReferenceName, ActionCallback callback,
		  void *callback_data, char *mnemonic = NULL,
		  char *acceleratorText = NULL, char *accelerator = NULL);
   void AddSep(char *category);
   BaseUI *SelectedObject() { return selected_object; }
   boolean HandleHelpRequest();

};

#endif // DTACTIONS_H
