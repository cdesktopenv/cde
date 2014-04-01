// $XConsortium: UAS_Factory.hh /main/5 1996/10/04 12:33:53 cde-hal $
#ifndef _UAS_Factory_hh_
#define _UAS_Factory_hh_

#include "UAS_Base.hh"
#include "UAS_List.hh"
#include "UAS_String.hh"

template <class T> class UAS_Pointer;
class UAS_Common;
class UAS_EmbeddedObject;

class UAS_Factory {
    public:
	virtual ~UAS_Factory() {}

	static void initialize (UAS_List<UAS_String>&);
	static void finalize ();
	static UAS_List<UAS_String> getRootLocators ();
	static void destroyRoot (UAS_Pointer<UAS_Common>);
	//
	//  For creating objects with absolute urls.
	//
	static UAS_Pointer<UAS_Common> create (
			const UAS_String &locator
			);

	//
	//  For creating new objects relative to the
	//  given object.
	//
	static UAS_Pointer<UAS_Common> create_relative (
			const UAS_String &locator,
			UAS_Pointer<UAS_Common> relative_to
			);

	//
	//  For creating embedded objects. These always must
	//  be "relative" creates.
	//
	static UAS_Pointer<UAS_EmbeddedObject> create_embedded (
			const UAS_String &locator,
			UAS_Pointer<UAS_Common> relative_to
			);
    protected:
	enum {
	    UAS_REGISTER_FAIL, UAS_REGISTER_SUCCESS,
	    UAS_REGISTER_RETURN
	};
	static int register_factory (const char *, UAS_Factory *);
	static void unregister_factory (const char *);
	//
	//  For creating objects with absolute urls ("addresses").
	//
	virtual UAS_Pointer<UAS_Common> create_object (
			const UAS_String &locator
			) = 0;

	//
	//  For creating new objects relative to the
	//  given object.
	//
	virtual UAS_Pointer<UAS_Common> create_relative_object (
			const UAS_String &locator,
			UAS_Pointer<UAS_Common> relative_to
			) = 0;

	virtual UAS_Pointer<UAS_EmbeddedObject> create_embedded_object (
			const UAS_String &locator
			) = 0;

	virtual UAS_Pointer<UAS_EmbeddedObject>
		create_relative_embedded_object (
			const UAS_String &locator,
			UAS_Pointer<UAS_Common> relative_to
			) = 0;

	virtual void initializeFactory(UAS_List<UAS_String>&) = 0;
	virtual void finalizeFactory() = 0;
	virtual UAS_List<UAS_String> rootLocators() = 0;
	virtual void destroy_root_object (UAS_Pointer<UAS_Common>) = 0;

    private:
	static UAS_Factory *lookup (const UAS_String& accessType);
	static UAS_Factory *get_factory (
			    const UAS_String& locator
			    );

};

//
//  Little private class used to associate an access type
//  (eg, mmdb, http) with the factory used to create those
//  types of objects. Users don't see this and don't need
//  to see it.
//
class FactoryEntry {
    private:
	const char *fAccess;
        UAS_Factory *fFactory;

    public:
        FactoryEntry (const char *s, UAS_Factory *f):
                fAccess (s), fFactory (f) {
        }
    public:
        UAS_Factory *factory() const { return fFactory; }
        const char *access () const { return fAccess; }
};

#endif
