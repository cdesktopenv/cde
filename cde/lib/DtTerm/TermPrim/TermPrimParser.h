/* 
** $XConsortium: TermPrimParser.h /main/1 1996/04/21 19:18:24 drk $
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef   _Dt_TermPrimParser_h
#define   _Dt_TermPrimParser_h

typedef struct _StateTableRec *StateTable;
typedef struct _ParserContextRec *ParserContext;

extern void
_DtTermPrimParserInitContext
(
    Widget w
);

extern Boolean
_DtTermPrimParse
(
    Widget w,
    unsigned char *parseChar,
    int parseCharLength
);

extern void
_DtTermPrimParserNumParmPush
(
    Widget w,
    int    parmNum
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimParser_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
