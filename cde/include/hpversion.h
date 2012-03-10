/* $XConsortium: hpversion.h /main/5 1996/08/30 15:22:49 drk $ */
/* -*-C-*-
****************************************************************
*/
/* The following macro should be invoked once by each of the X11 components
   to provide standard revision information. */
#ifndef	lint
#define	version_tag(FILE_VERSION) \
static char _hp_merge_version[] = "@(#)" FILE_VERSION;
#else	/* lint */
#define	version_tag(FILE_VERSION)
#endif	/* lint */

/* Any revision information other than the standard 'hpversion' macro shoud
   be conditionally compiled using the VERBOSE_REV_INFO constant so that it
   can be excluded for official X11 releases. To set VERBOSE_REV_INFO, comment
   out one of the following 2 blocks. */

/* This block turns on VERBOSE_REV_INFO */
/*
#ifndef lint
#define VERBOSE_REV_INFO  1
#else
#ifdef VERBOSE_REV_INFO
#undef VERBOSE_REV_INFO
#endif
#endif
*/

/* This block turns off VERBOSE_REV_INFO */
#ifdef VERBOSE_REV_INFO		
#undef VERBOSE_REV_INFO
#endif
