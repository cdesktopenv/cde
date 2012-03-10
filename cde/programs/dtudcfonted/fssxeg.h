/* fssxeg.h 1.5 - Fujitsu source for CDEnext    96/07/18 13:19:30      */
/* $XConsortium: fssxeg.h /main/4 1996/07/19 20:32:54 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



#include "udccom.h"	/* 1995.10.09 */
#include "FaLib.h"

typedef		int		INT;
typedef		short int	SHORT;
typedef		long int	LONG;
typedef		char		CHAR;
typedef		unsigned int	UINT;
typedef		unsigned short	USHORT;
typedef		unsigned char	UCHAR;

#define	ON			1
#define	OFF			0

#define	MAXPTNSIZE	128	
#define	MAXPTNBYTE	MAXPTNSIZE*((MAXPTNSIZE+7)/8)
				

#define ERROR_VAL	fal_utyerror
#define EXISTS_FLAG	fal_utyexists

#define	EDPANE_SIZE	400	
#define	EDLIST_MAX	30	
