// $XConsortium: UAS_String.hh /main/3 1996/06/11 16:40:38 cde-hal $
# ifndef _UAS_String_hh_
# define _UAS_String_hh_

# include "UAS_Base.hh"

typedef enum {
    UAS_NOT_OWNER = 0,
    UAS_OWNER = 1
} UAS_Owner;

class UAS_String;
template <class T> class UAS_List;

class UAS_StringRep {
    friend class UAS_String;
    private:
	UAS_StringRep (const char *data, int data_size, UAS_Owner owner);
	~UAS_StringRep ();

	void		incRef ();
	void		decRef ();
    private:
	int		fRefCnt;
	char *		fData;
	int		fDataSize;
	UAS_Owner	fOwner;

};

class UAS_String: public UAS_Base {

    friend int operator == (const UAS_String &, const UAS_String &);
    friend int operator == (const UAS_String &, const char *);
    friend int operator != (const UAS_String &, const UAS_String &);
    friend int operator != (const UAS_String &, const char *);

    friend int operator <  (const UAS_String &, const UAS_String &);

    public:
	UAS_String ();
	UAS_String (const char *,int length = -1,UAS_Owner owner = UAS_OWNER);
	UAS_String (const UAS_String &);
	~UAS_String ();

    public:
	operator char * () const;
	UAS_String &operator = (const UAS_String &);
	UAS_String operator + (const UAS_String &);
	UAS_String &append(const UAS_String &);

    public:
	int length () const;
	void split(const char, UAS_String &left, UAS_String &right) const;
	UAS_List<UAS_String> splitFields (const char separator) const;

    private:
	void setString (UAS_StringRep *);
	void unsetString ();

    private:
	UAS_StringRep	*fStringRep;
};

# endif
