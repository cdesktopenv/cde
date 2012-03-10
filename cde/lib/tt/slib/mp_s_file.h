/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_s_file.h /main/3 1995/10/23 11:54:56 rswiston $ 			 				 */
/*
 * Tool Talk Message Passer (MP) - mp_s_file.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declaration of the _Tt_s_file class.
 */

#ifndef _MP_S_FILE_H
#define _MP_S_FILE_H

#include "mp/mp_file.h"
#include "mp_s_procid_utils.h"

class _Tt_s_file : public _Tt_object {
      public:
	_Tt_s_file();
	_Tt_s_file(
		const _Tt_string &path
	);
	virtual ~_Tt_s_file();

	Tt_status		s_join(_Tt_s_procid_ptr &p);
	Tt_status		s_quit(_Tt_s_procid_ptr &p);

      protected:
	int			procs_joined();

      private:
	_Tt_string		networkPath;
};

#endif				/* _MP_S_FILE_H */
