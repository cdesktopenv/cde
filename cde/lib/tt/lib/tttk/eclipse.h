/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: eclipse.h /main/3 1995/10/23 10:31:27 rswiston $ 			 				 */
/*
 * @(#)eclipse.h	1.3 93/09/07
 */
#ifndef ttdtprocid_h
#define ttdtprocid_h

#include "ticccm.h"

//
// An instance of this class will install the given procID as
// the new default procID, and will restore the old default
// procID when it goes out of scope.
//
class TtDtProcIDEclipse {
    public:
			TtDtProcIDEclipse(
				const char *procID2Install,
				Tt_status  *status
			);
			~TtDtProcIDEclipse();
    private:
	const char     *_occultedProcID;
};

#endif
