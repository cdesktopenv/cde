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
// $XConsortium: InputMgrX.hh /main/4 1996/07/10 09:38:43 rcs $
#include "UAS.hh"
# include <X11/Intrinsic.h>

class InputMgrX;

struct InputMgrData {
    InputMgrX *			fObj;
    WantInputReady		fMsg;
    XtInputId			fId;
};

class InputMgrX: public UAS_Receiver<WantInputReady>,
		 public UAS_Receiver<WantInputReadyCancel>,
		 public UAS_Sender<InputReady> {

    public:
	InputMgrX ();

    public:
	void		receive (WantInputReady &msg, void *client_data);
	void		receive (WantInputReadyCancel &msg, void *client_data);

    private:
	static void	inputHandler (InputMgrData *, int *, XtInputId *);

    private:
	xList<InputMgrData *>		fWantList;
};
