// $XConsortium: UAS_Iterator.hh /main/3 1996/06/11 16:37:57 cde-hal $
#ifndef _UAS_Iterator_h_
#define _UAS_Iterator_h_


template <class T> class UAS_Iterator: public UAS_Base {
    public:
	UAS_Iterator (const UAS_List<T> &);
    
	unsigned int length () const;
	void reset ();
	void next ();
	int eol () const;
	UAS_Pointer<T> item () const;
    private:
	int fCurItem;
	const UAS_List<T> &fList;
};

#endif
