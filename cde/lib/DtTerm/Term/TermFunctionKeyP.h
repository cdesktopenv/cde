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

/* DONT'T ADD ANYTHING AFTER THIS #endif... */
#endif	/* _Dt_TermFunctionKeyP_h */
