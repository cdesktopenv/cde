// $XConsortium: UAS_Iterator.cc /main/3 1996/06/11 16:37:52 cde-hal $
template <class T>
UAS_Iterator<T>::UAS_Iterator (const UAS_List<T> &l):
	fList(l), fCurItem(0) {
}

template <class T>
unsigned int
UAS_Iterator<T>::length() const {
    return fList.length();
}

template <class T>
void
UAS_Iterator<T>::reset() {
    fCurItem = 0;
}

template <class T>
void
UAS_Iterator<T>::next() {
    fCurItem ++;
}

template <class T>
int
UAS_Iterator<T>::eol() const {
    return fCurItem < fList.length() ? 0 : 1;
}

template <class T>
UAS_Pointer<T>
UAS_Iterator<T>::item() const {
    if (fCurItem < fList.length())
	return fList[i];
    return UAS_Pointer((T *) 0);
}
