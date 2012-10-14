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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: TKTemplate.C /main/6 1998/04/17 11:43:37 mgreess $ 
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include "dti_cc/CC_Stack.h"
#include "dti_cc/CC_Stack.C"
#include "dti_cc/CC_Slist.h"
// #include "dti_cc/CC_Slist.C"
#include "dti_cc/cc_pvect.h"
// #include "dti_cc/cc_pvect.C"
#include "dti_cc/cc_hdict.h"
// #include "dti_cc/cc_hdict.C"
#include "BTCollectable.h"
#include "DataRepository.h"
#include "dti_cc/CC_String.h"

#ifdef _IBMR2
#pragma define (Stack<int>)
#pragma define (CC_TPtrSlist<CC_String>)
#pragma define (hashTable<CC_String, BTCollectable>)
#pragma define (hashTable<CC_String, int>)
#pragma define (hashTableIterator<CC_String, BTCollectable>)
#pragma define (hashTableIterator<CC_String, int>)
#pragma define (Stack<Rec>)
#endif

#ifdef __osf__
#pragma define_template Stack<int>
#pragma define_template CC_TPtrSlist<CC_String>
#pragma define_template hashTable<CC_String, BTCollectable>
#pragma define_template hashTable<CC_String, int>
#pragma define_template hashTableIterator<CC_String, BTCollectable>
#pragma define_template hashTableIterator<CC_String, int>
#pragma define_template Stack<Rec>
#pragma define_template CC_TValSlist<int>
#pragma define_template CC_TPtrSlist<kv_pair<CC_String, BTCollectable> >
#pragma define_template CC_TPtrSlist<kv_pair<CC_String, int> >
#pragma define_template CC_TValSlist<Rec>
#pragma define_template CC_TValSlistIterator<int>
#pragma define_template CC_TValSlistIterator<Rec>
#pragma define_template kv_pair<CC_String, BTCollectable>
#pragma define_template pointer_vector<CC_TPtrSlist<kv_pair<CC_String, BTCollectable> > >
#pragma define_template kv_pair<CC_String, int>
#pragma define_template pointer_vector<CC_TPtrSlist<kv_pair<CC_String, int> > >

CC_Boolean kv_pair<CC_String, BTCollectable>::f_needRemove = FALSE;
CC_Boolean kv_pair<CC_String, int>::f_needRemove = FALSE;

#endif

#ifdef USL
#pragma instantiate Stack<int>
#pragma instantiate CC_TPtrSlist<CC_String>
#pragma instantiate hashTable<CC_String, BTCollectable>
#pragma instantiate hashTable<CC_String, int>
#pragma instantiate hashTableIterator<CC_String, BTCollectable>
#pragma instantiate hashTableIterator<CC_String, int>
#pragma instantiate Stack<Rec>
#pragma instantiate CC_TValSlist<int>
#pragma instantiate CC_TPtrSlist<kv_pair<CC_String, BTCollectable> >
#pragma instantiate CC_TPtrSlist<kv_pair<CC_String, int> >
#pragma instantiate CC_TValSlist<Rec>
#pragma instantiate CC_TValSlistIterator<int>
#pragma instantiate CC_TValSlistIterator<Rec>
#pragma instantiate kv_pair<CC_String, BTCollectable>
#pragma instantiate pointer_vector<CC_TPtrSlist<kv_pair<CC_String, BTCollectable> > >
#pragma instantiate kv_pair<CC_String, int>
#pragma instantiate pointer_vector<CC_TPtrSlist<kv_pair<CC_String, int> > >
#endif

#if !defined(__osf__) && !defined(IBMR2)
typedef Stack<int> _f1_;
typedef CC_TPtrSlist<CC_String> _f2_;
typedef hashTable<CC_String, BTCollectable> _f3_;
typedef hashTable<CC_String, int> _f4_;
typedef hashTableIterator<CC_String, BTCollectable> _f5_;
typedef hashTableIterator<CC_String, int> _f6_;
typedef Stack<Rec> _f7_;
#endif
