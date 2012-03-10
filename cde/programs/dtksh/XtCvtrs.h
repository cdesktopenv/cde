/* $XConsortium: XtCvtrs.h /main/4 1995/11/01 15:50:01 rswiston $ */
/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


#ifndef _Dtksh_XtCvtrs_h
#define _Dtksh_XtCvtrs_h


extern Boolean DtkshCvtStringToPixel(
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValuePtr fromVal,
                        XrmValuePtr toVal,
                        XtPointer *closure_ret) ;
extern void RegisterXtOverrideConverters( void ) ;
extern Boolean DtCompareISOLatin1(
                        char *first,
                        char *second) ;


#endif  /* _Dtksh_XtCvtrs_h */
