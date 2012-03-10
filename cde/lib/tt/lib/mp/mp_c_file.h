/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_c_file.h /main/3 1995/10/23 10:19:40 rswiston $ 			 				 */
/*
 * Tool Talk Message Passer (MP) - mp_c_file.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declaration of the _Tt_c_file class.
 */

#ifndef _MP_C_FILE_H
#define _MP_C_FILE_H
#include <mp/mp_file.h>
#include <mp/mp_c_file_utils.h>

class _Tt_c_file : public _Tt_file {
      public:
	_Tt_c_file();
	_Tt_c_file(
		_Tt_string path
	);
	virtual ~_Tt_c_file();

	Tt_status		c_join(
					_Tt_procid_ptr &p
				);
	Tt_status		c_quit(
					_Tt_procid_ptr &p
				);
	Tt_status		process_message_queue(
					int dispatch = 1
				);
};

#endif				/* _MP_C_FILE_H */
