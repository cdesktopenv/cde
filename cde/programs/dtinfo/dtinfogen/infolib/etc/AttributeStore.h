/* $XConsortium: AttributeStore.h /main/2 1996/07/18 16:38:25 drk $ */
//---------------------------------------------------------
// AttributeSore.h


#ifndef ATT_STORE
#define ATT_STORE

#include "AttributeRec.h"

class AttributeStore {
private:
  AttributeRec *head;
  AttributeRec *current;

public:
  char *lookup ( char * );
  int  add ( AttributeRec * );
  AttributeRec *getNext() {
                             AttributeRec *ptr;
			     if ( current ) {
			       ptr = current;
			       current = current->next;
			       return (ptr );
			     }
                             else {
			       return ( NULL );
			     }
			   }
  AttributeStore();
  ~AttributeStore();
};
  
#endif
