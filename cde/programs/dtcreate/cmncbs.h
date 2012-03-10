/* $XConsortium: cmncbs.h /main/4 1995/11/01 16:10:49 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  cmncbs.h                                                               */
/*                                                                         */
/***************************************************************************/

#ifndef _CMNCBS_H_INCLUDED
#define _CMNCBS_H_INCLUDED

#ifndef STORAGECLASS
#ifdef  NOEXTERN
#define STORAGECLASS
#else
#define STORAGECLASS extern
#endif
#endif

/***************************************************************************/
/*                                                                         */
/*  Prototypes for functions                                               */
/*                                                                         */
/***************************************************************************/

void activateCB_open_FindSet (Widget,XtPointer,XmFileSelectionBoxCallbackStruct *);
void activateCB_edit_icon (Widget,XtPointer,XmPushButtonCallbackStruct *);
void DisplayHelpDialog (Widget, XtPointer, XtPointer);
void helpCB_general (Widget, XtPointer, XtPointer);


#endif /* _CMNCBS_H_INCLUDED */
