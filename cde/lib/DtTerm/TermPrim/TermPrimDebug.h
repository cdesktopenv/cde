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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: TermPrimDebug.h /main/1 1996/04/21 19:17:24 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimDebug_h
#define	_Dt_TermPrimDebug_h
#define	isDebugFSet	_DtTermPrimIsDebugFSet
#define	isDebugSet(c)	isDebugFSet((c), 0)
#define	setDebugFlags	_DtTermPrimSetDebugFlags
#define	shortSleep	_DtTermPrimShortSleep
#define	timeStamp	_DtTermPrimTimeStamp
#define	debugLevel	_DtTermPrimDebugLevel

extern int isDebugFSet(int c, int f);
extern void setDebugFlags(char *c);
extern void shortSleep(int msec);
extern void timeStamp(char *msg);

extern unsigned char *debugLevel[256];
#define	_TERM_MAX_DEBUG_FLAG	100
#define	__TERM_DEBUG_BIT(x)	(1 << (x % 8))
#define	__TERM_DEBUG_BYTE(x)	(x / 8)
#define	_TERM_DEBUG_NUM_BYTES	((_TERM_MAX_DEBUG_FLAG + 7) / 8)
#define DebugF		_TERM_DEBUG_FLAG
#define	Debug		_TERM_DEBUG
#define	_TERM_DEBUG_FLAG(c, f, a) \
	if (((f) < _TERM_MAX_DEBUG_FLAG) && debugLevel[(c)] && \
	(debugLevel[(c)][__TERM_DEBUG_BYTE(f)] & __TERM_DEBUG_BIT(f))) { \
	    a; fflush(stderr); \
	}
#define	_TERM_DEBUG(c, a) \
	_TERM_DEBUG_FLAG(c, 0, a)

void _DtTermPrimDebugDumpEvent (FILE *f, Widget w, XEvent *ev);


#endif	/* _Dt_TermPrimDebug_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
