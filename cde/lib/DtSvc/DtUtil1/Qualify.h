/*****************************************************************************
 *
 * File:         Qualify.h
 * RCS:          $XConsortium: Qualify.h /main/3 1995/10/26 15:10:03 rswiston $
 * Description:  Public header file for the Qualify routine
 * Project:      DT Runtime Library
 * Language:     C
 * Package:      N/A
 *
 *   (c) Copyright 1993 by Hewlett-Packard Company
 *
 *****************************************************************************/

/*********************************************************************
 * _DtQualifyWithFirst
 *
 * takes:   an unqualified filename like foo.txt, and
 *          a colon-separated list of pathnames, such as 
 *                /etc/dt:/usr/dt/config
 *
 * returns: a fully qualified filename.  Space for the filename
 *          has been allocated off the heap using malloc.  It is 
 *          the responsibility of the calling function to dispose 
 *          of the space using free.
 *
 * example: ...
 *          char * filename;
 *          ...
 *          filename = _DtQualifyWithFirst("configFile",
 *                          "/foo/first/location:/foo/second/choice");
 *          < use filename >
 *          free(filename);
 *
 **********************************************************************/

#ifndef _Dt_Qualify_h
#define _Dt_Qualify_h


#  ifdef __cplusplus
extern "C" {
#  endif

extern char * _DtQualifyWithFirst
  (
   char *,  /* file to locate */
   char *   /* list of colon-separated paths in which to look */
  );

#  ifdef __cplusplus
}
#  endif

#endif /* _Dt_Qualify_h */

/* DON'T ADD ANYTHING AFTER THIS #endif */
