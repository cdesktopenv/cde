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
/* $XConsortium: ShutdownActions.hh /main/3 1996/04/21 19:46:48 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _SHUTDOWNACTIONS_HH
#define _SHUTDOWNACTIONS_HH

class ShutdownActions {
  public:
    ShutdownActions(int num_actions = 32);
    ~ShutdownActions(void);

    typedef int (*ShutdownActionProc)(void *);

    void addAction(ShutdownActionProc, void * cb_data);
    void removeAction(ShutdownActionProc, void * cb_data);

    int doActions(void);

  private:
    struct ActionEntry {
	ShutdownActionProc	proc;
	void *			call_back_data;
    };

    ActionEntry		*_actions;
    int			_action_list_size;
    int			_action_count;

    void removeEntry(int);
};

#endif
