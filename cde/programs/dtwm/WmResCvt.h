/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmResCvt.h /main/4 1995/11/01 11:50:23 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern void AddWmResourceConverters (void);
#ifdef WSM
extern void WmCvtStringToAMBehavior (XrmValue *args, Cardinal numArgs, 
				 XrmValue *fromVal, XrmValue *toVal);
#endif /* WSM */
extern void WmCvtStringToCFocus (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToCDecor (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToCFunc (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToFrameStyle (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToIDecor (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToIPlace (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToKFocus (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToSize (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToShowFeedback (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern void WmCvtStringToUsePPosition (XrmValue *args, Cardinal numArgs, XrmValue *fromVal, XrmValue *toVal);
extern unsigned char *NextToken (unsigned char *pchIn, int *pLen, unsigned char **ppchNext);
extern Boolean StringsAreEqual (unsigned char *pch1, unsigned char *pch2, int len);
