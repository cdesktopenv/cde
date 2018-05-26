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
/*
 * $XConsortium: TermFunctionKeyP.h /main/1 1996/04/21 19:15:58 drk $
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermFunctionKeyP_h
#define _Dt_TermFunctionKeyP_h

#include "TermFunctionKey.h"

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

typedef struct _PerUserKeyInfoRec {
    UserKeyAttribute	  attribute;
    unsigned char	 *label;
    short		  labelLength;
    unsigned char	 *sendString;
    short		  sendStringLength;
    Boolean		  enabled;
} PerUserKeyInfoRec, *PerUserKeyInfo;

typedef struct _UserKeyInfoRec {
    int numKeys;
    int keyToXmit;
    PerUserKeyInfoRec *keys;
    int workingKeyNumber;
    unsigned char *workingLabel;
    int workingLabelLength;
    int workingLabelTotalLength;
    unsigned char *workingString;
    int workingStringLength;
    int workingStringTotalLength;
} UserKeyInfoRec;

#ifdef	__cplusplus
} /* close scope of 'extern "C"'.... */
#endif	/* __cplusplus */

/* DON'T ADD ANYTHING AFTER THIS #endif... */
#endif	/* _Dt_TermFunctionKeyP_h */
