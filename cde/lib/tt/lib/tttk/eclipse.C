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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: eclipse.C /main/3 1995/10/23 10:31:20 rswiston $ 			 				
/*
 * @(#)eclipse.C	1.3 93/09/07
 * Copyright 1993 Sun Microsystems, Inc. All rights reserved.
 */								
#include "ttdt.h"
#include "ttdtprocid.h"
#include "ttdtutils.h"

TtDtProcIDEclipse::TtDtProcIDEclipse(
	const char *procID2Install,
	Tt_status  *status
)
{
	static const char *here = "TtDtProcIDEclipse::TtDtProcIDEclipse()";

	_occultedProcID = tt_default_procid();
	*status = tt_ptr_error( _occultedProcID );
	if (*status != TT_OK) {
		_occultedProcID = 0;
		if ((procID2Install == 0) && (*status == TT_ERR_NOMP)) {
			//
			// There is no default procid to eclipse.  OK.
			//
			*status = TT_OK;
		} else {
			ttDtPrintStatus( here, "tt_default_procid()", *status);
			return;
		}
	}
	if (procID2Install != 0) {
		*status = tt_default_procid_set( procID2Install );
		if (*status != TT_OK) {
			ttDtPrintStatus( here, "tt_default_procid_set()",
					 *status );
			tt_free( (caddr_t)_occultedProcID );
			_occultedProcID = 0;
		}
	}
}

TtDtProcIDEclipse::~TtDtProcIDEclipse()
{
	static const char *here = "TtDtProcIDEclipse::~TtDtProcIDEclipse()";

	if (_occultedProcID != 0) {
		Tt_status status = tt_default_procid_set( _occultedProcID );
		if (status != TT_OK) {
			ttDtPrintStatus( here, "tt_default_procid_set()",
					 status );
		}
		tt_free( (caddr_t)_occultedProcID );
		_occultedProcID = 0;
	}
}
