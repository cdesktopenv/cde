// $XConsortium: RendererHCV.cc /main/3 1996/06/11 17:08:12 cde-hal $

#include "RendererHCV.h"

RendererHCV::~RendererHCV()
{
   f_autoNumberFP.clear();
}

FeatureValue* RendererHCV:: evaluate(const char* variable) 
{
   return f_autoNumberFP.evaluate(variable);
/*
   FeatureValue* fv = f_autoNumberFP.evaluate(variable);
fv -> print(cerr);
   return fv;
*/
}

unsigned int RendererHCV:: accept(const char* name, const Expression* expr) 
{
   return f_autoNumberFP.accept(name, expr);
}


