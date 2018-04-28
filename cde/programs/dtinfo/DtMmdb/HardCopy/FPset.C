/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
