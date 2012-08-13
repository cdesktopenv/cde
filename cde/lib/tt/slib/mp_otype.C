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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: mp_otype.C /main/4 1998/03/20 14:27:39 mgreess $ 			 				
/*
 * mp_otype.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <stdlib.h>
#if defined(linux) || defined(CSRG_BASED)
/*# include <g++/minmax.h>*/
#else
# include <macros.h>
#endif
#include "mp/mp_global.h"
#include "mp_s_mp.h"
#include "mp_otype.h"
#include "mp_otype_utils.h"     
#include "mp_ce_attrs.h"
#include "util/tt_enumname.h"
#include "util/tt_iostream.h"

//
// This file contains methods for _Tt_otype objects which embody the
// behavior of otype definitions.
//


_Tt_otype::
_Tt_otype()
{
	_ancestors = new _Tt_string_list();
	_children = (_Tt_string_list *)0;
	_inhs = new _Tt_signature_list();
	_osigs = new _Tt_signature_list();
	_hsigs = new _Tt_signature_list();
	visitp = 0;
	ce_entry = 0;
}


bool_t _Tt_otype::
xdr(XDR *xdrs)
{
	return(_otid.xdr(xdrs) &&
	       _ancestors.xdr(xdrs) &&
	       _inhs.xdr(xdrs) &&
	       _osigs.xdr(xdrs) &&
	       _hsigs.xdr(xdrs) &&
	       xdr_int(xdrs, &visitp));
}


_Tt_otype::
_Tt_otype(_Tt_string otid)
{
	_ancestors = new _Tt_string_list();
	_children = (_Tt_string_list *)0;
	_inhs = new _Tt_signature_list();
	_osigs = new _Tt_signature_list();
	_hsigs = new _Tt_signature_list();
	_otid = otid;
	visitp = 0;
	ce_entry = 0;
}


_Tt_otype::
~_Tt_otype()
{
}


void _Tt_otype::
set_otid(_Tt_string otid)
{
	_otid = otid;
}


void _Tt_otype::
set_ancestors(_Tt_string_list_ptr ancs)
{
	_ancestors = ancs;
}


//
// Appends an inherited signature to this object if there isn't the
// same signature already.
//
void _Tt_otype::
append_inhs(_Tt_signature_ptr sig)
{
	if (_inhs.is_null()) {
		_inhs = new _Tt_signature_list();
		_inhs->push(sig);
	} else {
		_Tt_signature_list_cursor	sc(_inhs);

		while (sc.next()) {
			if (**sc == *sig) {
				return;
			}
		}
		_inhs->append(sig);
	}
}


//
// Appends an observer signature to this object if there isn't the
// same signature already.
//
void _Tt_otype::
append_osig(_Tt_signature_ptr sig)
{
	sig->set_pattern_category(TT_OBSERVE);

	if (_osigs.is_null()) {
		_osigs = new _Tt_signature_list();
		_osigs->push(sig);
	} else {
		_Tt_signature_list_cursor 	sc(_osigs);

		while (sc.next()) {
			if (**sc == *sig) {
				return;
			}
		}
		_osigs->append(sig);
	}
}


//
// Appends a handler signature to this object if there isn't the same
// signature already.
//
void _Tt_otype::
append_hsig(_Tt_signature_ptr sig, Tt_category category)
{
	sig->set_pattern_category(category);

	if (_hsigs.is_null()) {
		_hsigs = new _Tt_signature_list();
		_hsigs->push(sig);
	} else {
		_Tt_signature_list_cursor 	sc(_hsigs);

		while (sc.next()) {
			if (**sc == *sig) {
				return;
			}
		}
		_hsigs->append(sig);
	}
}


//
// Appends a list of observer signatures to this object.
//
void _Tt_otype::
append_osigs(_Tt_signature_list_ptr sigs)
{
	_Tt_signature_list_cursor 	sc(sigs);

	while (sc.next()) {
		sc->set_pattern_category(TT_OBSERVE);
	}
	if (_osigs.is_null()) {
		_osigs = sigs;
	} else {
		_osigs->append_destructive(sigs);
	}
}


//
// Appends a list of handler signature to this object.
//
void _Tt_otype::
append_hsigs(_Tt_signature_list_ptr sigs, Tt_category category)
{
	_Tt_signature_list_cursor 	sc(sigs);

	while (sc.next()) {
		sc->set_pattern_category(category);
	}
	if (_hsigs.is_null()) {
		_hsigs = sigs;
	} else {
		_hsigs->append_destructive(sigs);
	}
}


//
// Prints out a _Tt_otype object. Note that the format used to print
// the object is the format used to represent this object to the
// Classing Engine. It is important to preserve this characteristic
// for compatibility's sake.
//
void _Tt_otype::
print(const _Tt_ostream &os) const
{
	_Tt_string_list_cursor		anc;
	FILE *fs = os.theFILE();

	if (_otid.len()) {
		fprintf(fs, "\t(\n\t\t(%s,%s,<",
			_tt_ce_attr_string(_TYPE_NAME),
			_tt_ce_attr_string(_TT_TOOLTALK_OTYPE));
		_otid.print(fs);
		fputs(">)\n", fs);
		fprintf(fs,"\t\t(%s,string,<%s>)\n",
			_tt_ce_attr_string(_TT_TOOLTALK_TYPE),
			_tt_ce_attr_string(_TT_TOOLTALK_OTYPE));
		if (! _ancestors.is_null()) {
			anc.reset(_ancestors);
			while (anc.next()) {
				fprintf(fs,"\t\t(%s,string,<%s>)\n",
					_tt_ce_attr_string(_TT_PARENT),
					(char *)*anc);
			}
		}
		fprintf(fs,"\t)");
	}
}


//
// Returns a list of descendant otypes from this otype.
//
_Tt_string_list_ptr & _Tt_otype::
children()
{
	_Tt_otype_table_cursor c;
	_Tt_string_list_ptr plist;
	_Tt_string parname;
	_Tt_otype_ptr cparent;

	if (!_children.is_null()) {
		return _children;
	}
	_children = new _Tt_string_list;

	c.reset(_tt_s_mp->otable);
	while(c.next()) {
		plist = c->parents();
		if (!plist.is_null() && plist->count()!=0) {
			parname = plist->top();
			if (! _tt_s_mp->otable->lookup(parname,cparent)) {
				if (cparent->otid() == otid()) {
					_children->append(c->otid());
				}
			}
		}
	}
	return _children;
}


_Tt_string_list_ptr & _Tt_otype::
parents()
{
	return _ancestors;
}


_Tt_signature_list_ptr &_Tt_otype::
hsigs()
{
	return _hsigs;
}


_Tt_signature_list_ptr &_Tt_otype::
osigs()
{
	return _osigs;
}


int _Tt_otype::
xdr_version_required() const
{
	int version = _Tt_signature::xdr_version_required_( _hsigs );
//     return max(version, _Tt_signature::xdr_version_required_( _osigs ));
	if (version > _Tt_signature::xdr_version_required_( _osigs )) {
		return version;
	} else {
		return _Tt_signature::xdr_version_required_( _osigs );
	}
}

//
// Prints out an otype object in source format. This means that the
// otype is printed out in such a fashion that the result is parseable
// by tt_type_comp and when parsed would reproduce the essential
// structure of this object.
//
void _Tt_otype::
pretty_print(const _Tt_ostream &os) const
{
	_Tt_string_list_cursor		anc;

	FILE *fs = os.theFILE();

	fprintf(fs,"otype ");
	_otid.print(fs);
	if (! _ancestors.is_null() && _ancestors->count()) {
		fprintf(fs," :");
		anc.reset(_ancestors);
		while (anc.next()) {
			fprintf(fs," %s", (char *)*anc);
		}
	}
	fprintf(fs," {\n");
	_Tt_signature::pretty_print_( os, _hsigs, TT_HANDLE, 0 );
	_Tt_signature::pretty_print_( os, _hsigs, TT_HANDLE_PUSH, 0 );
	_Tt_signature::pretty_print_( os, _hsigs, TT_HANDLE_ROTATE, 0 );
	_Tt_signature::pretty_print_( os, _osigs, TT_OBSERVE, 0 );
	fprintf(fs,"};\n");
}


// 
// Returns the otid of the given _Tt_object which is assumed to be a
// _Tt_otype object. This is used by the generic table package to
// generate a key for an entry in the table.
// 
_Tt_string
_tt_otype_otid(_Tt_object_ptr &o)
{
	return(((_Tt_otype *)o.c_pointer())->otid());
}


// 
// Invokes the print method on the given _Tt_object which is assumed to
// really point to a _Tt_otype object. This is used by the generic table
// and list objects in their print methods.
// 
void 
_tt_otype_print(const _Tt_ostream &os, const _Tt_object *obj)
{
	((_Tt_otype *)obj)->print(os);
}
