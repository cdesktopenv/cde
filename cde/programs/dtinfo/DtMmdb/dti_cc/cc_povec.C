// $XConsortium: cc_povec.cc /main/3 1996/06/11 16:56:44 cde-hal $

template <class T>
dlist_array<T>::dlist_array(const dlist_array<T>& da) 
{
   cerr << "Warning: dlist_array(const dlist_array&) called" << endl ;
   exit (-1);
}

template <class T>
dlist_array<T>::dlist_array(size_t)
{
}

template <class T>
dlist_array<T>::~dlist_array()
{
}

