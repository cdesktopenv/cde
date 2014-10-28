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
/*
 * $TOG: dynarray.h /main/5 1999/10/15 17:22:58 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/* Handle dynamic arrays of arbitrary type and range. */

#ifndef __DYNARRAY_H_
#define __DYNARRAY_H_

#if !defined(linux) && !defined(CSRG_BASED) && !defined(sun)
#include <generic.h>
#endif
#if defined(USL) || defined(__uxp__) || defined(__osf__) || defined(sun) || defined(linux) || defined(CSRG_BASED)
#define _DELETE_ARRAY(sz) delete[]
#else
#define _DELETE_ARRAY(sz) delete[(sz)]
#endif


// this is used to create an ARRAY of a TYPE
#define declare_array(ARRAY, TYPE, BUMP) \
class ARRAY \
{ \
	long len; \
	long max; \
	TYPE *arr; \
protected: \
	TYPE &bumpsize(long); \
public: \
	ARRAY() { arr = 0; max = len = 0; } \
	ARRAY(long siz) \
		{ arr = 0; max = len = 0; if (siz > 0) bumpsize(siz-1); } \
	ARRAY(const ARRAY &); \
	~ARRAY() { _DELETE_ARRAY(max) arr; } \
	ARRAY &operator=(const ARRAY &); \
	long size() const { return len; } \
	void reset(long l = 0) { bumpsize(l); len = l; } \
	TYPE &operator[](long e) \
		{ if (e < len) return arr[e]; else return bumpsize(e); } \
	TYPE &elt(long e) const { return arr[e]; } \
	TYPE &end() { return bumpsize(len); } \
	TYPE *getarr() const { return arr; } \
	TYPE *operator()() const { return arr; } \
};

// this implements an ARRAY of a TYPE
// - this must be done once and only once in the user code
//	printf("0x%X:  max=%d  len=%d  elt=%d\n", this, max, len, elt);
#define implement_array(ARRAY, TYPE, BUMP) \
TYPE &ARRAY::bumpsize(long elt) \
{ \
	if (elt < 0) \
	    elt = 0; \
	if (elt >= max) \
	{ \
		if (max <= 0) \
			max = 1; \
		long omax = max; \
		TYPE *narr = new TYPE[max = elt + (omax > BUMP ? BUMP : omax)]; \
		for (long i = 0; i < len; i++) \
			narr[i] = arr[i]; \
		_DELETE_ARRAY(omax) arr; \
		arr = narr; \
	} \
	if (elt >= len) \
		len = elt + 1; \
	return arr[elt]; \
} \
ARRAY &ARRAY::operator=(const ARRAY &a) \
{ \
	if (&a == this) \
	    return *this; \
	if (a.len > len) \
		bumpsize(a.len); \
	len = a.len; \
	for (long i = 0; i < len; i++) \
		arr[i] = a.arr[i]; \
	return *this; \
} \
ARRAY::ARRAY(const ARRAY &t) \
{ \
	arr = 0; \
	max = len = 0; \
	*this = t; \
}

// the user can also use these to define an array of any type
#define darray(TYPE) name2(TYPE,array)
#define darraydeclare(TYPE) declare_array(darray(TYPE), TYPE, 1024)
#define darrayimplement(TYPE) implement_array(darray(TYPE), TYPE, 1024)
#define darraydeclare2(TYPE,BUMP) declare_array(darray(TYPE), TYPE, BUMP)
#define darrayimplement2(TYPE,BUMP) implement_array(darray(TYPE), TYPE, BUMP)


// this is used to define a DYNARRAY of a TYPE
#define declare_dynarray(DYNARRAY, TYPE) \
class DYNARRAY \
{ \
	long low; \
	long high; \
	long min; \
	long max; \
	unsigned bump; \
	TYPE *arr; \
	TYPE *aptr; \
	void init(long, long, unsigned); \
	void chsize(long, long); \
public: \
	DYNARRAY(long l, long s = 0, unsigned b = 1024) { init(l, s, b); } \
	DYNARRAY() { init(0, 0, 1024); } \
	DYNARRAY(const DYNARRAY &t) \
		{ init(t.high - t.low + 1, t.low, t.bump); *this = t; } \
	~DYNARRAY() { _DELETE_ARRAY(max - min + 1) arr; } \
	DYNARRAY &operator=(const DYNARRAY &); \
	long size() const { return high - low + 1; } \
	void reset(long len = 0, long st = 0) \
		{ chsize(st, st + len - 1); high = st + len - 1; low = st; } \
	long smallest() const { return low; } \
	long largest() const { return high; } \
	TYPE &operator[](long e) \
		{ if (e <= low || e >= high) chsize(e,e); return aptr[e]; } \
	TYPE &elt(long e) const { return aptr[e]; } \
	TYPE &end() { return (*this)[largest() + 1]; } \
	TYPE *getarr() const { return aptr; } \
	TYPE *operator()() const { return aptr; } \
};

// this creates the code needed for a DYNARRAY of TYPE
// - this must be done once and only once in the user code
#define implement_dynarray(DYNARRAY, TYPE) \
void DYNARRAY::init(long len, long start, unsigned bmp) \
{ \
	if (len < 0) \
	    len = 0; \
	high = start + len - 1; \
	low = start; \
	max = high; \
	min = low; \
	bump = bmp > 0 ? bmp : 1024; \
	if (max < min) \
	    max = min; \
	arr = new TYPE[max - min + 1]; \
	aptr = arr - min; \
} \
void DYNARRAY::chsize(long lelt, long helt) \
{ \
	long nlow = lelt < low ? lelt : low; \
	long nhigh = helt > high ? helt : high; \
	if (nlow <= min || nhigh >= max) \
	{ \
		long nmin = nlow < min ? nlow : min; \
		long nmax = nhigh > max ? nhigh : max; \
		long m = max - min + 1; \
		long nm = nmax - nmin + 1 + (m > bump ? bump : m); \
		long nl = nhigh - nlow + 1; \
		TYPE *narr = new TYPE[nm]; \
		TYPE *naptr = narr - nmin; \
		for (long i = low; i <= high; i++) \
			naptr[i] = aptr[i]; \
		_DELETE_ARRAY(m) arr; \
		arr = narr; \
		aptr = naptr; \
		min = nmin; \
		max = nmax; \
	} \
	high = nhigh; \
	low = nlow; \
} \
DYNARRAY &DYNARRAY::operator=(const DYNARRAY &a) \
{ \
	if (&a == this) \
	    return *this; \
	if (a.low < low || a.high > high) \
		chsize(a.low, a.high); \
	low = a.low; \
	high = a.high; \
	for (long i = a.low; i <= a.high; i++) \
		aptr[i] = a.aptr[i]; \
	bump = a.bump; \
	return *this; \
}

// the user can also use these to define arrays of any type
#define dynarray(TYPE) name2(TYPE,dynarray)
#define dynarraydeclare(TYPE) declare_dynarray(dynarray(TYPE), TYPE)
#define dynarrayimplement(TYPE) implement_dynarray(dynarray(TYPE), TYPE)

// Predefined dynarrays for most common uses
/* declare_array(Charbuf, char, 256) */


#endif /* __DYNARRAY_H_ */
