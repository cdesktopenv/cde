// $XConsortium: MMDB_EmbeddedObject.hh /main/4 1996/06/11 16:43:43 cde-hal $
# ifndef _MMDB_EmbeddedObject_hh_
# define _MMDB_EmbeddedObject_hh_

# include "UAS_EmbeddedObject.hh"
# include "UAS_Collection.hh"
# include "UAS_Exceptions.hh"
# include "MMDB_Common.hh"

class MMDB_EmbeddedObject: public UAS_EmbeddedObject, public MMDB_Common {
    public:
	MMDB_EmbeddedObject (MMDB &, info_base *, const UAS_String &);
	~MMDB_EmbeddedObject ();

    public:
	
	UAS_String locator();
	unsigned int width ();
	unsigned int height ();
	UAS_String data ();
	unsigned int data_length ();
	UAS_String content_type();
	UAS_String title (UAS_TitleType tt = UAS_LONG_TITLE);

	unsigned int llx ();
	unsigned int lly ();
	unsigned int urx ();
	unsigned int ury ();

    protected:
	graphic_smart_ptr fEmbeddedObject;
};

# endif
