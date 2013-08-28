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
