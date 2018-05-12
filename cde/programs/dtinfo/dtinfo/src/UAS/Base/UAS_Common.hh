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
// $XConsortium: UAS_Common.hh /main/10 1996/09/04 01:35:26 cde-hal $
# ifndef _UAS_Common_hh_
# define _UAS_Common_hh_

# include "UAS_Sender.hh"
# include "UAS_String.hh"
# include "UAS_Base.hh"
# include "UAS_List.hh"
# include "UAS_Msgs.hh"

typedef enum {
    UAS_RETRIEVED,
    UAS_RETRIEVAL_FAILURE,
    UAS_RETRIEVAL_IN_PROGRESS,
    UAS_NOT_RETRIEVED
} UAS_RetrievalStatus;

typedef enum {
    UAS_LONG_TITLE,
    UAS_SHORT_TITLE
} UAS_TitleType;

typedef enum {
    UAS_UNKNOWN_OBJECT_TYPE,
    UAS_LIBRARY,
    UAS_BOOKCASE,
    UAS_BOOK,
    UAS_SECTION,
    UAS_PATH,
    UAS_BOOKTAB,
    UAS_STYLE_SHEET,
    UAS_EMBEDDED_OBJECT,
    UAS_COLLECTION,
    UAS_SEARCH_ENGINE
} UAS_ObjectType;

class UAS_EmbeddedObject;
class UAS_Collection;
class UAS_Path;
class UAS_SearchEngine;
class UAS_StyleSheet;

class UAS_Common: public UAS_Base,
		  public UAS_Sender<UAS_PartialDataMsg> {
    public:
	UAS_Common ();
	~UAS_Common ();
        static void send_message(const UAS_DocumentRetrievedMsg &message, void *client_data)	
        { 
	   senderUAS_DocumentRetrievedMsg().send_message (message, client_data); 
	}
	static void request(UAS_Receiver<UAS_DocumentRetrievedMsg> *receiver)
	{ 
	   senderUAS_DocumentRetrievedMsg().request (receiver); 
	}	
	static void unrequest (UAS_Receiver<UAS_DocumentRetrievedMsg> *receiver)
	{ 
	   senderUAS_DocumentRetrievedMsg().unrequest(receiver); 
	}
	static UAS_Sender<UAS_DocumentRetrievedMsg>  &senderUAS_DocumentRetrievedMsg();

	STATIC_SENDER_HH(UAS_ErrorMsg);
	STATIC_SENDER_HH(UAS_StatusMsg);
	STATIC_SENDER_HH(UAS_LibraryDestroyedMsg);

    public:
	static void initialize (UAS_List<UAS_String>&);
	static void finalize ();
	static UAS_List<UAS_String> rootLocators ();
	static UAS_Pointer<UAS_Common> create (const UAS_String &);
	UAS_Pointer<UAS_Common> create_relative (const UAS_String &);
	UAS_Pointer<UAS_EmbeddedObject> create_embedded_object (
		const UAS_String &
	    );
	static void destroy (UAS_Pointer<UAS_Common> lib);
    public:
	virtual UAS_RetrievalStatus retrieve(void *client_data = 0);
	virtual UAS_RetrievalStatus retrieval_status () const;
	virtual void cancel_retrieval ();
	virtual void flush ();

	virtual UAS_String error_information () const;

	virtual UAS_String title (
		UAS_TitleType tt = UAS_LONG_TITLE
	    );
	virtual UAS_String tab_title () { return title(UAS_SHORT_TITLE); }

	virtual UAS_String book_name (
		UAS_TitleType tt = UAS_LONG_TITLE
		/*, UAS_Pointer<UAS_Collection> ic = 0 */
	    ) const;

	virtual UAS_String locator ();
	virtual UAS_String id () { return UAS_String(); }
	virtual UAS_String lid () { return UAS_String(); }
	virtual UAS_String bid () { return UAS_String(); }
	virtual UAS_String name () { return UAS_String(); }
	virtual UAS_String locale() { return UAS_String(); }
	virtual UAS_String content_type ();
	virtual UAS_ObjectType type ();

#if 0
	virtual UAS_Pointer<UAS_CollectionElement> collection_element (
		UAS_Pointer<UAS_Collection> ic = 0
	    );

	virtual UAS_Pointer<UAS_PathElement> path_element (
		UAS_Pointer<UAS_Path> ip = 0
	    );
#endif

	virtual UAS_List<UAS_Common> book_tab_list ();
	virtual UAS_List<UAS_StyleSheet> style_sheet_list ();
	virtual UAS_Pointer<UAS_Common> parent ();
	virtual UAS_List<UAS_Common> children ();
	virtual UAS_Pointer<UAS_Common> next ();
	virtual UAS_Pointer<UAS_Common> previous ();

	//
	//  Stuff for collection/path tbd. SWM
	//

	virtual int searchable () const;
	
	virtual UAS_Pointer<UAS_SearchEngine> search_engine ();

	virtual unsigned int data_length ();

	virtual UAS_String data ();

	virtual int input (UAS_String str);

	virtual int subtree_size ();

	virtual int operator == (UAS_Common &);
	virtual int operator != (UAS_Common &);
	virtual UAS_String implementation_type ();
	virtual UAS_Pointer<UAS_Common> get_library ();

    protected:
	UAS_RetrievalStatus	fRetrievalStatus;
};

#endif
