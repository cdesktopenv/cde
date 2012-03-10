/* $XConsortium: Common.h /main/4 1995/11/02 14:35:32 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Common.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Public include file for common dialog code
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Common_h
#define _Common_h

/* Buffer type - Used to store buffer drag-n-drop info */
typedef struct
{
  void * buf_ptr;
  int    size;
} BufferInfo;

/* Pixmap Data structure */
typedef struct
{
   int   size;
   char *hostPrefix;
   char *instanceIconName;
   char *iconName;
   char *iconFileName;
} PixmapData;

typedef struct _TypesToggleInfo {
   String name;
   Boolean  selected;
} TypesToggleInfo, *TypesToggleInfoPtr;


#define XmROfft "Offt"

/********    Public Function Declarations    ********/

extern void CvtStringToStringList( 
                        String string,
                        String **listPtr,
                        int *countPtr) ;
extern String CvtStringListToString( 
                        String *list,
                        int count) ;

/********    End Public Function Declarations    ********/

#endif /* _Common_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
