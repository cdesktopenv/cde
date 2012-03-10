/* $XConsortium: cmnutils.h /main/5 1995/11/01 16:12:06 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  cmnutils.h                                                             */
/*                                                                         */
/***************************************************************************/

#ifndef _CMNUTILS_H_INCLUDED
#define _CMNUTILS_H_INCLUDED


#include <Xm/Xm.h>

/********************************************************************************/
/*                                                                              */
/*  Function Declarations                                                       */
/*                                                                              */
/********************************************************************************/


XmStringTable TextStringsToXmStrings (char **);
char *XmStringToText (XmString);
char **XmStringsToTextStrings (void);
int countItems (char **);
void ffree_string_array (void);
Widget findOptionMenuButtonID (void);
void free_string_array (void);
char *getLabelString (void);
Widget getOptionMenuHistory (void);
void setOptionMenuHistoryByText (void);
void setOptionMenuHistoryByWidget (void);
void show_warning_dialog (void);
Widget show_working_dialog (void);
void show_info_dialog (void);
void free_XmStringTable (XmStringTable table);


#endif /* _CMNUTILS_H_INCLUDED */
