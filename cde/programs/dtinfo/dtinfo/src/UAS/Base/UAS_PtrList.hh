// $XConsortium: UAS_PtrList.hh /main/5 1996/08/21 15:44:37 drk $
#ifndef _UAS_PtrList_hh_
#define _UAS_PtrList_hh_

class UAS_Exception;
class UAS_String;

template <class T> class UAS_PtrList {
    public:
	UAS_PtrList ();
	UAS_PtrList (const UAS_PtrList<T> &);
	~UAS_PtrList ();

    public:
	void append (T *);
	void remove (T *);
	T *item(int) const;
	int numItems () const;
	UAS_PtrList<T> &operator = (const UAS_PtrList<T> &);

	//  zero-origined
	T * operator [] (int) const;

        void clear();

    private:
        typedef T *Tptr;

	Tptr *fItems;
	int fNumItems;
	int fListSize;
};



#ifdef EXPAND_TEMPLATES
#include "UAS_PtrList.C"
#endif
#endif
