/* $XConsortium: convert3-4.h /main/1 1996/04/21 19:22:34 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CONVERT3_4_H
#define _CONVERT3_4_H

#include "ansi_c.h"

/* convert3-4.h:  conversion routines for rpc.cmsd version 3 to
   rpc.cmsd version 4 data types
*/

extern Table_Args_4 *_DtCm_tableargs3_to_tableargs4 P((Table_Args_3*));
extern Table_Args_4 *_DtCm_tabledelargs3_to_tabledelargs4 P((Table_Args_3*, Options_4));
extern Registration_4 *_DtCm_reg3_to_reg4 P((Registration_3*));
extern Access_Args_4 *_DtCm_accargs3_to_accargs4 P((Access_Args_3*));
extern Table_Res_4 *_DtCm_tableres3_to_tableres4 P((Table_Res_3*));
extern Table_Status_4 _DtCm_tablestat3_to_tablestat4 P((Table_Status_3));
extern Access_Status_4 _DtCm_accstat3_to_accstat4 P((Access_Status_3));
extern Registration_Status_4 _DtCm_regstat3_to_regstat4 P((Registration_Status_3));

#endif
