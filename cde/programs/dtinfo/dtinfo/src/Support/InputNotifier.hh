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
// $XConsortium: InputNotifier.hh /main/3 1996/06/11 16:34:55 cde-hal $
#ifndef _InputNotifier_h_
#define _InputNotifier_h_

#include "UAS.hh"


struct InputReady: public Destructable {
    int				fChannel;
};

struct WantInputReady: public Destructable {
    UAS_Receiver<InputReady> *	fReceiver;
    int				fChannel;
};

struct WantInputReadyCancel: public Destructable {
    UAS_Receiver<InputReady> *	fReceiver;
    int				fChannel;
};

class InputNotifier {
    public:
	InputNotifier (UAS_Receiver<InputReady> *, int);
	~InputNotifier ();
	STATIC_SENDER_HH (WantInputReady);
	STATIC_SENDER_HH (WantInputReadyCancel);

    private:
	UAS_Receiver<InputReady> *		fReceiver;
	int				fChannel;
};

#endif
