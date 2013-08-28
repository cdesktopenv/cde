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
