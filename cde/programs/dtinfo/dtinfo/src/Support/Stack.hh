// $XConsortium: Stack.hh /main/4 1996/08/21 15:43:59 drk $
#ifndef _Stack_hh
#define _Stack_hh

# include "Exceptions.hh"

template <class T> class Stack: public Destructable {
    public:
	Stack ();
	~Stack ();

    public:
	T&	pop ();       // reference type is returned for speed
	void	push (const T);
	T&	top () const;
        int     entries() const { 
	  return(fNumItems);  //ie no. of elements in the stack
	}
        int     empty() const {
          return(!fNumItems);
        }

    private:
	T *	fItems;
	int	fNumItems;
	int	fStackSize;
};

#ifdef EXPAND_TEMPLATES
#include "Stack.C"
#endif

#endif
