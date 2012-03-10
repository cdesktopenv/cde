/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: spec.h /main/3 1995/10/20 16:26:43 rswiston $ 			 				 */
/*
 *
 * spec.h
 *
 * Part of the spec inspect&repair tool.
 *
 * accumulates, prints, rewrites(?) data for a spec.
 * This is for the sole use of ttdbck, so much less information
 * hiding is done than the usual TT class definition style:
 * in particular, data members are simply made public,
 * instead of defining access functions.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef _SPEC_H
#define _SPEC_H

#include "util/tt_object.h"
#include "util/tt_list.h"
#include "util/tt_string.h"
#include "binkey.h"
#include "prop.h"

declare_list_of(Spec)

class Spec : public _Tt_object {
      public:
	Spec();
	~Spec() {};
	Binkey_ptr	key;
	_Tt_string	type;
	_Tt_string	filename;
	int		is_filespec;
	virtual void print(FILE * f) const;
	void		process_spec();
	void		print_key(FILE * f) const;
	void		print_mand(FILE * f) const;
	void		print_props(FILE * f) const;
	void		add_prop_and_value(_Tt_string propname,
					   _Tt_string value);
	void		repair_spec();
      private:
	Prop_table_ptr		props;
	_Tt_string_list_ptr	propnames;
};

#endif /* _SPEC_H */
