/* $XConsortium: ResolverStack.h /main/4 1996/08/21 15:50:41 drk $ */
#ifndef _ResolverStack_h
#define _ResolverStack_h

#ifndef CDE_NEXT

#else
//#include <StyleSheet/cde_next.h>
#include "dti_cc/CC_Dlist.h"
#endif

class Element ;
class FeatureSet ;

class ResolverStackElement 
{
public:
  // note, this object will delete these items in its destructor
  ResolverStackElement(Element*,
		       FeatureSet *local,
		       FeatureSet *complete);

  ~ResolverStackElement();

  int
  operator==(const ResolverStackElement &);

  // only pass out references because we own these guys 
  FeatureSet &completeFeatureSet()	{ return *f_completeFeatureSet ; }
  FeatureSet &localFeatureSet()		{ return *f_localFeatureSet ; }
  Element    &element()			{ return *f_element ; }
  
private:
  Element      *f_element;
  FeatureSet   *f_localFeatureSet;
  FeatureSet   *f_completeFeatureSet;
};

class ResolverStack : private CC_TPtrDlist<ResolverStackElement>
{

public:
  ResolverStack();
  ~ResolverStack();

  void push(ResolverStackElement*);
  ResolverStackElement* pop();
  ResolverStackElement* top();
};


#endif /* _ResolverStack_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
