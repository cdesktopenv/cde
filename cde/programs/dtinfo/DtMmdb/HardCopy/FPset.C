// $TOG: FPset.C /main/4 1998/04/17 11:46:42 mgreess $

#include "HardCopy/FPset.h"

FPset::FPset(int s, const Element& element, 
		const FeatureSet& local, 
		const FeatureSet& complete, 
		const FeatureSet& parent) : 
	ptrs(0), count(s), f_QuaddingString(0),
	f_element(&element),
	f_local(&local),
	f_complete(&complete),
	f_parent(&parent)
{
    ptrs = new featureProcessorPtr[count];
}

unsigned int FPset::operator==(const FPset&)
{
   throw(CASTHCREXCEPT hardCopyRendererException());
   return 1;
}

FPset::~FPset() 
{ 
   for (int i=0; i<count; i++ )
      delete ptrs[i]; 
   delete ptrs;
}
