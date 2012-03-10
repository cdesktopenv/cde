// $XConsortium: UAS_ObjList.hh /main/4 1996/08/21 15:44:30 drk $
#ifndef _UAS_ObjList_hh_
#define _UAS_ObjList_hh_

template <class T> class UAS_ObjList {
    public:
	UAS_ObjList ();
	UAS_ObjList (const UAS_ObjList<T> &);
	~UAS_ObjList ();

    public:
	void append (const T &);
	void remove (const T &);
	T &item(int) const;
	int numItems () const;
	UAS_ObjList<T> &operator = (const UAS_ObjList<T> &);
	//  zero-origined
	T & operator [] (int) const;

        void clear();

    private:
	T *fItems;
	int fNumItems;
	int fListSize;
};



#ifdef EXPAND_TEMPLATES
#include "UAS_ObjList.C"
#endif
#endif
