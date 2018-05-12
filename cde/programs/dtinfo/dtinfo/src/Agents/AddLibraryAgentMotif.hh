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
// $XConsortium: AddLibraryAgentMotif.hh /main/4 1996/09/20 13:56:43 cde-hal $
#include <WWL/wwl.h>
#include <WWL/WXmDialogShell.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmLabel.h>

class AddLibraryAgent: public WWL {
    public:
	AddLibraryAgent (Widget parent);
	~AddLibraryAgent ();
	char *getInfolibName ();

    private:
	void create_ui (Widget parent);
	void ok();
	void cancel();
	void clear();
	void help ();
	void pick_dir();			// do file selection
	static void cancelCallback(		// for file selection
			Widget, XtPointer, XtPointer );
	static void fileSelectedCallback(	// for file selection
			Widget, XtPointer, XtPointer );
	void textChanged (WCallback *);
	void verifyNoSpace (WCallback *);

    private:
	WXmDialogShell  f_shell;
	WXmTextField	f_text;
	WXmForm		f_form;
	WXmPushButton   f_ok;
	WXmPushButton   f_clr;
	bool		f_done;
	bool		f_cancelled;
	Widget		f_file_sel;
};
