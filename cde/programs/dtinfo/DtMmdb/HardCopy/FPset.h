/* $XConsortium: FPset.h /main/3 1996/06/11 17:04:12 cde-hal $ */

#ifndef _FPset_h
#define _FPset_h

#include "StyleSheet/Renderer.h"
#include "StyleSheet/Const.h"
#include "StyleSheet/Element.h"

#include "HardCopy/FPExceptions.h"
#include "HardCopy/HardCopyFP.h"
#include "HardCopy/lout_limits.h"

class FPset
{
public:
/*
  FPset(const FPset& fps) : ptrs(fps.ptrs), count(fps.count), f_QuaddingString(0) {};
  FPset(featureProcessorPtr* p, int s) : ptrs(p), count(s), f_QuaddingString(0) {};
*/
  FPset(int s, const Element&, const FeatureSet&, 
	const FeatureSet&, const FeatureSet&);
  ~FPset();

  const Element& element() { return *f_element; };
  const FeatureSet& local() { return *f_local; };
  const FeatureSet& complete() { return *f_complete; };
  const FeatureSet& parent() { return *f_parent; };

  FPset operator=(const FPset&);
  unsigned int operator==(const FPset&);

  featureProcessorPtr* ptrs;
  int count;

  const Element* f_element;
  const FeatureSet* f_local;
  const FeatureSet* f_complete;
  const FeatureSet* f_parent;

  const char* f_QuaddingString;
};

#endif
