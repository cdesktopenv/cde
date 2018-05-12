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
// $XConsortium: UrlAgentMotif.hh /main/7 1996/12/10 16:28:02 cde-hal $
#include <WWL/WTopLevelShell.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmLabel.h>

class NodeWindowAgent;

class UrlAgent: public WWL {

    //  Constructors, Destructor
    public:
	UrlAgent ();
	virtual ~UrlAgent ();

    //  Public Member Functions
    public:
	void			display (NodeWindowAgent *prefWindow);
	static int		document( char   *locator,
				          Boolean new_window );
	static int		print_document( char   *locator);

    // Private Member functions
    private:
	void			create_ui ();

    // Callbacks
    private:
	void			close ();
	void			popdown ();
	void			clear ();
	void			valueChanged ();
	void			modifyVerify (WCallback *);
	void			displayDocument ();
	void			displayDocumentAndClose ();
	
    // Private Data Members
    private:
	WTopLevelShell		fShell;
	WXmForm			f_form;
	WXmTextField		fTextField;
	WXmPushButton		fOk;
	WXmPushButton		fApply;
	WXmPushButton		fClear;
	bool			fPoppedUp;
	NodeWindowAgent *	fPrefWindow;
};
