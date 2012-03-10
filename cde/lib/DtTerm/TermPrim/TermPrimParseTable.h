/* 
** $XConsortium: TermPrimParseTable.h /main/1 1996/04/21 19:18:18 drk $
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef   _Dt_TermPrimParseTable_h
#define   _Dt_TermPrimParseTable_h

/* state table declarations... */
extern StateTable _DtTermPrimStateStart;

/* parse functions... */
extern void _hpDefaultF(Widget w);
extern void _DtTermPrimParseBell(Widget w);
extern void _DtTermPrimParseBackspace(Widget w);
extern void _DtTermPrimParseTab(Widget w);
extern void _DtTermPrimParseLF(Widget w);
extern void _DtTermPrimParseCR(Widget w);
extern void _DtTermPrimParseWriteChar(Widget w);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimParseTable_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
