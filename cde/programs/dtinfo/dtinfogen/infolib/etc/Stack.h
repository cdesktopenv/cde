/* $XConsortium: Stack.h /main/2 1996/07/18 16:49:58 drk $ */

#ifndef STACK_HEADRER
#define STACK_HEADER

//-----------------------------------------------------------------------
class Element{

friend class Stack;
friend class SearchEngine;  
  
protected:
  int name;
  Element *next;
  Element( int aName, Element *v=0) { name = aName; next = v; }
  int GetName() const { return name; }
  
};

class Stack {
private:
  Element *currentToken;
  
public:
  void push( Element *tok ) { tok->next = currentToken; currentToken = tok; }
  
  Element *pop()  {
    Element *ptr;
  
    if ( currentToken ) {
      ptr = currentToken;
      currentToken = ptr->next;
      return ( ptr );
    }
    else {
      return ( NULL );
    }
  }

  Element *GetTopToken() { return ( currentToken ); }

  Stack() { currentToken = NULL; }
  ~Stack();
    
};

inline
Stack::~Stack()
{
  Element *pt = currentToken;
  while ( pt ) {
    Element *tmp = pt;
    pt = pt->next;
    delete tmp;
  }
}
    
#endif
