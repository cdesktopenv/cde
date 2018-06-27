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
/* $XConsortium: HashP.h /main/4 1995/10/26 15:22:50 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifdef REV_INFO
#ifndef lint
static char SCCSID[] = "OSF/Motif: @(#)_HashP.h	4.16 91/09/12";
#endif /* lint */
#endif /* REV_INFO */
/******************************************************************************
*******************************************************************************
*
*  (c) Copyright 1989, 1990, 1991 OPEN SOFTWARE FOUNDATION, INC.
*  (c) Copyright 1989, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
*  (c) Copyright 1987, 1988, 1989, 1990, 1991 HEWLETT-PACKARD COMPANY
*  ALL RIGHTS RESERVED
*  
*  	THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED
*  AND COPIED ONLY IN ACCORDANCE WITH THE TERMS OF SUCH LICENSE AND
*  WITH THE INCLUSION OF THE ABOVE COPYRIGHT NOTICE.  THIS SOFTWARE OR
*  ANY OTHER COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE
*  AVAILABLE TO ANY OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF THE
*  SOFTWARE IS HEREBY TRANSFERRED.
*  
*  	THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT
*  NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY OPEN SOFTWARE
*  FOUNDATION, INC. OR ITS THIRD PARTY SUPPLIERS  
*  
*  	OPEN SOFTWARE FOUNDATION, INC. AND ITS THIRD PARTY SUPPLIERS,
*  ASSUME NO RESPONSIBILITY FOR THE USE OR INABILITY TO USE ANY OF ITS
*  SOFTWARE .   OSF SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
*  KIND, AND OSF EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES, INCLUDING
*  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE.
*  
*  Notice:  Notwithstanding any other lease or license that may pertain to,
*  or accompany the delivery of, this computer software, the rights of the
*  Government regarding its use, reproduction and disclosure are as set
*  forth in Section 52.227-19 of the FARS Computer Software-Restricted
*  Rights clause.
*  
*  (c) Copyright 1989, 1990, 1991 Open Software Foundation, Inc.  Unpublished - all
*  rights reserved under the Copyright laws of the United States.
*  
*  RESTRICTED RIGHTS NOTICE:  Use, duplication, or disclosure by the
*  Government is subject to the restrictions as set forth in subparagraph
*  (c)(1)(ii) of the Rights in Technical Data and Computer Software clause
*  at DFARS 52.227-7013.
*  
*  Open Software Foundation, Inc.
*  11 Cambridge Center
*  Cambridge, MA   02142
*  (617)621-8700
*  
*  RESTRICTED RIGHTS LEGEND:  This computer software is submitted with
*  "restricted rights."  Use, duplication or disclosure is subject to the
*  restrictions as set forth in NASA FAR SUP 18-52.227-79 (April 1985)
*  "Commercial Computer Software- Restricted Rights (April 1985)."  Open
*  Software Foundation, Inc., 11 Cambridge Center, Cambridge, MA  02142.  If
*  the contract contains the Clause at 18-52.227-74 "Rights in Data General"
*  then the "Alternate III" clause applies.
*  
*  (c) Copyright 1989, 1990, 1991 Open Software Foundation, Inc.
*  ALL RIGHTS RESERVED 
*  
*  
* Open Software Foundation is a trademark of The Open Software Foundation, Inc.
* OSF is a trademark of Open Software Foundation, Inc.
* OSF/Motif is a trademark of Open Software Foundation, Inc.
* Motif is a trademark of Open Software Foundation, Inc.
* DEC is a registered trademark of Digital Equipment Corporation
* DIGITAL is a registered trademark of Digital Equipment Corporation
* X Window System is a trademark of the Massachusetts Institute of Technology
*
*******************************************************************************
******************************************************************************/
#ifndef __HashP_h
#define __HashP_h

#include <X11/Intrinsic.h>
 
#ifdef __cplusplus
extern "C" {
#endif
/*
 * the structure is used as a common header part for different
 * users of the hash functions in order to locate the key
 */
typedef XtPointer DtHashKey;

typedef DtHashKey (*DtGetHashKeyFunc)();
typedef Boolean (*DtHashEnumerateFunc)();
typedef void (*DtReleaseKeyProc)();

typedef struct _DtHashEntryPartRec {
    unsigned int	type:16;
    unsigned int	flags:16;
}DtHashEntryPartRec, *DtHashEntryPart;

typedef struct _DtHashEntryRec {
    DtHashEntryPartRec	hash;
}DtHashEntryRec, *DtHashEntry;

typedef struct _DtHashEntryTypePartRec {
    unsigned int		entrySize;
    DtGetHashKeyFunc		getKeyFunc;
    XtPointer			getKeyClientData;
    DtReleaseKeyProc		releaseKeyProc;
}DtHashEntryTypePartRec, *DtHashEntryTypePart;

typedef struct _DtHashEntryTypeRec {
    DtHashEntryTypePartRec	hash;
}DtHashEntryTypeRec, *DtHashEntryType;

typedef struct _DtHashTableRec *DtHashTable;

/********    Private Function Declarations for Hash.c    ********/

extern void _DtRegisterHashEntry( 
                        DtHashTable tab,
                        DtHashKey key,
                        DtHashEntry entry) ;
extern void _DtUnregisterHashEntry( 
                        DtHashTable tab,
                        DtHashEntry entry) ;
extern DtHashEntry _DtEnumerateHashTable( 
                        DtHashTable tab,
                        DtHashEnumerateFunc enumFunc,
                        XtPointer clientData) ;
extern DtHashEntry _DtKeyToHashEntry( 
                        DtHashTable tab,
                        DtHashKey key) ;
extern DtHashTable _DtAllocHashTable( 
                        DtHashEntryType *hashEntryTypes,
                        Cardinal numHashEntryTypes,
#if NeedWidePrototypes
                        int keyIsString) ;
#else
                        Boolean keyIsString) ;
#endif /* NeedWidePrototypes */
extern void _DtFreeHashTable( 
                        DtHashTable hashTable) ;

/********    End Private Function Declarations    ********/



#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* HashP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */



