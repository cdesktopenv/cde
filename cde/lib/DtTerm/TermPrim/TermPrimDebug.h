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

#endif	/* _Dt_TermPrimDebug_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
