/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: ttdtprocid.h /main/3 1995/10/23 10:32:34 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)ttdtprocid.h	1.3	93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef ttdtprocid_h
#define ttdtprocid_h

#include "util/tt_new.h"
#include "tttk/tttk.h"

class _TtDtProcid: public _Tt_allocated {
    public:
				_TtDtProcid(
					const char	       *toolName,
					const char	       *vendor,
					const char	       *version
				);
				~_TtDtProcid();

	const char	       *toolname()	const {return _toolname;}
	const char	       *vendor()	const {return _vendor;}
	const char	       *version()	const {return _version;}

	Tt_pattern	       *pats_create(
					Tt_message		contract,
					Ttdt_contract_cb	cb,
					Widget			shell,
					void		       *clientData
				) const;
    private:
	char		       *_toolname;
	char		       *_vendor;
	char		       *_version;
};

#endif
