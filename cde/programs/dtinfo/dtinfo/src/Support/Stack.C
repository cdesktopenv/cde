// $XConsortium: Stack.cc /main/3 1996/06/11 16:35:31 cde-hal $
template <class T> Stack<T>::Stack () {
    fItems = 0;
    fNumItems = 0;
    fStackSize = 0;
}

template <class T> Stack<T>::~Stack () {
    delete fItems;
}

template <class T>
void
Stack<T>::push (const T newItem) {
    if (fNumItems == fStackSize) {
	T *newList = new T[fStackSize += 16];
	for (int i = 0; i < fNumItems; i ++)
	    newList[i] = fItems[i];
	delete fItems;
	fItems = newList;
    }
    fItems[fNumItems ++] = newItem;
}

template <class T>
T &
Stack<T>::pop () {
    if (fNumItems <= 0)
	throw (Exception());
    return fItems[--fNumItems];
}

template <class T>
T &
Stack<T>::top () const {
    if (fNumItems <= 0)
	throw (Exception());
    return fItems[fNumItems - 1];
}
