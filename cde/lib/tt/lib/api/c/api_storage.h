/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: api_storage.h /main/3 1995/10/23 09:55:04 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * api_storage.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * mark/release storage management for storage returned over api
 */
#if !defined(_API_STORAGE_H)
#define _API_STORAGE_H

#include <stdlib.h>
#include <malloc.h>
#include "util/tt_object.h"
#include "util/tt_list.h"

declare_list_of(_Tt_api_stg_stack_elm)
declare_ptr_to(_Tt_api_stg_stack)

class _Tt_api_stg_stack_elm : public _Tt_object {
      public:
	_Tt_api_stg_stack_elm() {};
	~_Tt_api_stg_stack_elm();
	enum {STACK_MARK, STACK_STORAGE}	entry_type;
	caddr_t					addr;
	void		print(FILE *fs = stdout) const;
};

class _Tt_api_stg_stack : public _Tt_object {
      public:
	_Tt_api_stg_stack();
	virtual ~_Tt_api_stg_stack();
	int			mark();
	void			release(int mark);
#ifdef	_DEBUG_MALLOC_INC
	caddr_t		 	debug_malloc(char *fname,int lnum,size_t size);
#else
	caddr_t		 	malloc(size_t size);
#endif
	// take(p,s) is for taking a normally ::malloc'ed chunk of
        // storage and putting it under mark/release control.
	caddr_t			take(caddr_t addr);
#ifdef	_DEBUG_MALLOC_INC
	void		        debug_free(char *fname,int lnum,caddr_t addr);
#else
	void		        free(caddr_t addr);
#endif
	void		print(FILE *fs = stdout) const;
      private:
	_Tt_api_stg_stack_elm_list_ptr stack;
	int			mark_counter;
};

#endif
