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
// $XConsortium: SSTemplates.C /main/8 1996/10/09 15:29:50 rcs $
/* 
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef CDE_NEXT
//#include "StyleSheet/cde_next.h"
#endif

#include "Attribute.h"
#include "FeatureValue.h"
#include "AttributeList.h"
#include "SSPath.h"
#include "SymTab.h"
#include "BitVector.h"
#include "PathTable.h"
#include "Element.h"
#include "Expression.h"
#include "Resolver.h"
#include "Feature.h"
#include "ResolverStack.h"
#include "FeatureDefDictionary.h"

#ifdef CDE_NEXT
#include "dti_cc/cc_hdict.h"
#include "dti_cc/CC_Slist.h"
#include "dti_cc/CC_Dlist.h"
#include "dti_cc/cc_povec.h"               
#include "dti_cc/cc_vvect.h" 
#include "dti_cc/cc_pvect.h" 
#endif

#ifdef BUILD_FEATURES
/* for features test program - jbm */

typedef Stack<FeatureSet*,dlist_array<FeatureSet> > _stack_fs_orvec_fs_;
#endif

//
// Update this field whenever an ID is assigned. - qifan
// the largest ID used in f's: 59
//

#if defined(sun) || defined(hpux)
typedef CC_TPtrSlist<Attribute> _f4_;
typedef CC_TPtrSlist<Feature>   _f5_;
typedef CC_TPtrSlist<PathFeature> _f2_;
typedef CC_TPtrSlist<FeatureValue> _f55_;
typedef CC_TPtrSlist<FeatureDef> _f56_;
typedef CC_TPtrSlist<TypeValues> _f57_;
typedef CC_TPtrSlist<char> _f44_;

typedef CC_TPtrDlist<PathFeature> _f1_;
typedef CC_TPtrDlist<Symbol>    _f6_;
typedef CC_TPtrDlist<ResolverStackElement> _f7_;
typedef CC_TPtrDlist<char>    _f50_;

typedef hashTable<SymbolName, unsigned int> _f3_;
typedef hashTable<Symbol, Expression> _f8_;
typedef hashTable<unsigned int,BitVector> _f9_;

typedef hashTableIterator<SymbolName,unsigned int> _f13_;
typedef hashTableIterator<unsigned int,BitVector> _f21_;
typedef hashTableIterator<Symbol, Expression> _hash_dict_iter_sym_exp_;
typedef hashTableIterator<FeatureDef, FeatureDef> _f59_;

#ifndef CDE_NEXT
typedef CC_TPtrSlistDictionary<SymbolName,unsigned int> _f31_;
typedef CC_TPtrSlistDictionary<unsigned int,BitVector> _f32_;
typedef CC_TPtrSlistDictionary<Symbol,Expression> _f33_;
typedef CC_TPtrSlistDictionary<SymbolName,unsigned int> _f34_;
#endif

typedef dlist_array<CC_String> _ordvec_cstring_ ;
typedef CC_TValSlist<posRecord> _f54_;
typedef value_vector<PathTermPtr> _PathTermPtrvalue_vector_;
typedef pointer_vector<FeatureValue> _pointer_vector_FeatureValue_;
#endif


#if defined(_IBMR2)

#pragma define( CC_TPtrSlist<Attribute> )
#pragma define( CC_TPtrSlist<Feature>  )
#pragma define( CC_TPtrSlist<PathFeature> )
#pragma define( CC_TPtrSlist<FeatureValue> )
#pragma define( CC_TPtrSlist<FeatureDef> )
#pragma define( CC_TPtrSlist<TypeValues> )
#pragma define( CC_TPtrSlist<char> )

#pragma define( CC_TPtrDlist<PathFeature> )
#pragma define( CC_TPtrDlist<Symbol>    )
#pragma define( CC_TPtrDlist<ResolverStackElement> )
#pragma define( CC_TPtrDlist<char>    )

#pragma define( hashTable<Symbol, Expression> )
#pragma define( hashTable<SymbolName, unsigned int> )
#pragma define( hashTable<unsigned int,BitVector> )

#pragma define( hashTableIterator<SymbolName,unsigned int> )
#pragma define( hashTableIterator<unsigned int,BitVector> )
#pragma define( hashTableIterator<Symbol, Expression> )
#pragma define( hashTableIterator<FeatureDef, FeatureDef> )


#ifndef CDE_NEXT
#pragma define( CC_TPtrSlistDictionary<SymbolName,unsigned int> )
#pragma define( CC_TPtrSlistDictionary<unsigned int,BitVector> )
#pragma define( CC_TPtrSlistDictionary<Symbol,Expression> )
#pragma define( CC_TPtrSlistDictionary<SymbolName,unsigned int> )
#endif

typedef dlist_array<CC_String> _ordvec_cstring_ ;
typedef CC_TValSlist<posRecord> _f54_;
typedef value_vector<PathTermPtr> _PathTermPtrvalue_vector_;
typedef pointer_vector<FeatureValue> _pointer_vector_FeatureValue_;

#pragma define( dlist_array<CC_String> )
#pragma define( CC_TValSlist<posRecord> )
#pragma define( value_vector<PathTermPtr> )
#pragma define( pointer_vector<FeatureValue> )
#endif
