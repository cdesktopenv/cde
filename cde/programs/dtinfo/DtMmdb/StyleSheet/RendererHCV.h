/* $XConsortium: RendererHCV.h /main/3 1996/06/11 17:08:17 cde-hal $ */

#ifndef _RendererHCV_h
#define _RendererHCV_h 1

#include "Renderer.h"
#include "HardCopy/autoNumberFP.h"

//   RendererHCV class is a simplified hardcopy renderer responsible 
//   for evaluating feature values specific to hardcopy engine. The
//   feature values are used by the validator.

class RendererHCV : public Renderer 
{
public:
  RendererHCV()  {};
  ~RendererHCV() ;

  FeatureSet *initialize() { return 0; };

  unsigned int
    BeginElement(const Element       &element,
		 const FeatureSet    &featureset,
		 const FeatureSet    &complete,
		 const FeatureSet    &parentComplete) { return 0; };


  void data(const char *data, unsigned int size) {} ;
  void EndElement(const Symbol &element_name) {};
  void Begin() {};
  void End() {};

// These two functions allow a renderer to set up internal variable table.
  unsigned int accept(const char*, const Expression*) ;
  FeatureValue* evaluate(const char*) ;
  void preEvaluate(const Element &) {};
  void postEvaluate(const Element &) {}; 

protected:
  autoNumberFP f_autoNumberFP;
};

#endif

