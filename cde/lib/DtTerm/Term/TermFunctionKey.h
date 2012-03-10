/*
 * $XConsortium: TermFunctionKey.h /main/1 1996/04/21 19:15:56 drk $
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermFunctionKey_h
#define _Dt_TermFunctionKey_h

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */


/* _DtTermFunctionKeyExecute(): This action is invoked via an action when
 * a function key F1..F32 is depressed...
 */
extern void _DtTermFunctionKeyExecute(
	Widget		  w,
	short		  keyNumber,
	Boolean		  shift
);

extern void
_DtTermFunctionKeyUserKeyExecute(
    Widget                w,
    short                 keyno,
    Boolean               shift
);

extern void
_DtTermFunctionKeyStringStore(       /* store the label for the given key number
 */
     Widget w,
     int  keyno,
     char *string
);


extern void
_DtTermFunctionKeyClear(
    Widget   w
);


extern _DtTermFunctionKeyInit(
   Widget w
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'.... */
#endif	/* __cplusplus */

/* DONT'T ADD ANYTHING AFTER THIS #endif... */
#endif	/* _Dt_TermFunctionKey_h */
