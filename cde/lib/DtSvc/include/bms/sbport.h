/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * File:         sbport.h $TOG: sbport.h /main/4 1998/03/16 14:40:52 mgreess $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef PORT_H_INCLUDED
#define PORT_H_INCLUDED

/*---------------------------------------------------------------------------

  The following is a list of #defines that may be tested for in the code:
  =======================================================================

  __STDC__		- Will be set for ANSI C compilers
  __cplusplus    	- Will be set for C++ compilers
  __STDCPP__		- Will be set for ANSI and C++ compilers

 __hpux 		- Will be set for HP-UX systems
 __sun			- Will be set for SUN systems
 __aix                  - Will be set for IBM (AIX) systems

  __sysv		- Set when using SYSV semantics (e.g. HP, SUN)
  SYSV			- Set when __sysv is set (needed for some X11 includes)
  __bsd			- Set when using BSD semantics

 __hp_7_0		- For running on HP-UX 7.0 
 __hp_nls_16		- Set if HP's 16 bit support is to be compiled in
 __hp_color_object      - Set if HP's ColorObject extension is available in the Motif library
 __hp9000s300 		- Set for S300 (and S400?) HP-UX machines
 __hp9000s800 		- Set for S800 (PA-RISC) machines

 __sparc		- Set for SUN sparc machines
 __sun68k		- Set for SUN m68k machines

 __identification_strings - Set when RCS header strings are to be in code
 __recursive_includes	- Set to have .h files #includes prerequisite files
 __char_ptr_yytext      - Set if the lex/yacc variable yytext is defined
			  to be of type char[].
 __unsigned_char_ptr_yytext - Set if the lex/yacc variable yytext is defined
			      to be of type unsigned char[].

 KEEP_DEADWOOD          - Currently set to "FALSE".  If set to "TRUE",
                          lots of old code and semantics will be enabled.

----------------------------------------------------------------------------*/



/* First task is to make sure all of the defines that we use in the */
/* code are properly set up.					    */
/* ---------------------------------------------------------------- */
#if !defined(sun)
#if !defined(__STDCPP__) && (defined(__STDC__) || defined(__cplusplus))
#   define __STDCPP__
#endif
#endif

#if defined(sun) && !defined(__sun)
#   define __sun
#endif

#if defined(hpux) && !defined(__hpux)
#   define __hpux
#endif

#if defined(_AIX) && !defined(__aix)
#   define __aix
#   define OSF_MOTIF_1_1_1
#   define OSF_BUG
#endif

#ifdef __hpux 
#   define __sysv
/* __hp9000s300 or __hp9000s800 is defined by HP-UX cpp */
#    if !defined (__hpux_8_0)
#       define __hp_7_0
#    endif
#    define __hp_nls_16
#    define __hp_color_object
#    define __unsigned_char_ptr_yytext
#endif  /* __hpux */

#ifdef __sun
#   if defined(sparc) && !defined(__sparc)
#      define __sparc
#   else
#      if defined(mc68000) || defined(__mc68000)
#         define __sun68k
#      endif
#   endif
#   define __hp_color_object  /* Should only be set if linking with    */
                              /* HP's version of Motif (1.1 or later). */
#   define __char_ptr_yytext
#   define __c_callable         /* Set so VUE can link with libbms. */
#endif

#ifdef __aix
#       define __sysv
#endif

#if defined(__sysv) && !defined(SYSV)
#   define SYSV				/* Needed for X11 include files */
#endif

#ifndef KEEP_DEADWOOD			/* Setting this to "TRUE" will cause */
#   define  KEEP_DEADWOOD FALSE		/* all sorts of unknown problems.    */
#endif

#ifdef __cplusplus

#define UNUSED_PARM(_parm)

#else

#define UNUSED_PARM(_parm) _parm

#endif /* __cplusplus */

/* There are a few things that are not part of XPG3 that we need.    */
/* sockets are one of them.  To use this stuff, we just include the  */
/* non XOPEN include files and things work.  However, there are a    */
/* few XOPEN include files that have stuff to support this extra     */
/* functionality (e.g. sockets) that are #ifdef'ed for conditional   */
/* inclusion with the _XOPEN_SOURCE define.  In order to pull those  */
/* extra types in, we relas the XPG3 compliance for that file.  The  */
/* following are the three areas where we must do this:              */
/* 								     */
/* __need_timeval      gets "struct timeval"   			     */
/* __need_fd_set       gets "typedef struct fd_set {..."	     */
/* __need_S_IF         gets "S_IFNWK, S_IFLNK"    		     */
/* __need_all_signals  gets all SIGxxx values			     */
/* __need_all_errors   gets all errno values                         */
/*								     */
/* We get at these declarations differently on each platform...      */
/* ----------------------------------------------------------------- */

#if defined(__need_timeval)		/* Get "struct timeval" */
#   if defined(__sun) || defined(__aix)
#      include <sys/time.h>
#   endif
#endif /* __need_timeval */


#if defined(__need_fd_set)		/* Get "typedef struct fd_set" */
#   ifdef _AIX
#       include <sys/select.h>
#   endif
#endif /* __need_fd_st */

/* We also use the following non XPG3 types.  However, they may be   */
/* defined when we relax the XPG3 compliance to get the stuff talked */
/* about above.  So, declare them only if we don't already have them */
/* ----------------------------------------------------------------- */

#if defined(_HPUX_SOURCE) || defined(__sun) || defined(_INCLUDE_BSD_SOURCE) || defined(__aix) || defined(__linux__)
       /* the "u_types" are defined in standard files */
#      undef _INCLUDE_BSD_SOURCE
#else
       typedef unsigned int   u_int;
       typedef unsigned char  u_char;
       typedef unsigned short u_short;
       typedef unsigned long  u_long;
#endif

/* Use these if you must ensure that  you get a specific number of bits */
/* -------------------------------------------------------------------- */
typedef char  int8;
typedef short int16;
typedef long  int32;
typedef unsigned char  u_int8;
typedef unsigned short u_int16;
typedef unsigned long  u_int32;

#define __xechar_is_signed
#undef  __xechar_is_unsigned

#ifdef __xechar_is_signed
typedef char XeChar;
#else
typedef unsigned char XeChar;
#endif


/* The following type is a VARIABLE pointer to a VARIABLE XeChar. */

typedef XeChar *XeString;

/***********************************************************************
*	The following type is a VARIABLE pointer to a CONST XeChar.
*	Thus, 	ConstXeString p;
*		p = "abcd"; #Legal
*		*p = "\0';  #Illegal
*
*	This is NOT the same as		(const XeString)
*	which is equivilent to		(XeChar * const)
*
*	Thus,	const XeString p;
*		p = "abcd"; #Illegal
*		*p = "\0';  #Legal
*
************************************************************************/

typedef const XeChar *ConstXeString;

typedef unsigned short XeChar16;
typedef XeChar16 *XeString16;

#define XeString_NULL  (XeString) 0
#define XeString_Empty (XeString) ""
#define XeChar_NULL    (XeChar) 0

/* XtPointer was added in R4, so define it ourselves in R3 */
#ifdef __motif10
typedef void *  XtPointer;
#endif


/* The X11 R4 header file "Intrinsic.h" is internally inconsistent  */
/* with respect to XtAppAddInput. The third parameter ("condition") */
/* is defined to be of type XtPointer (void *), but the input masks */
/* commonly used with it are scalar values. Until the problem is    */
/* corrected by M.I.T, use the following type as a cast in calls    */
/* to XtAppAddInput. When it is corrected, change this typedef      */
/* accordingly.							    */
/* ---------------------------------------------------------------- */
typedef void * XeInputMask;

#ifdef __cplusplus
/* Define several macros to handle the C++ vs XtOffset problem.     */
/* The problem is that C++ cannot at compile time evaluate the      */
/* XtOffset macro within an initializer.  XtOffset is most often    */
/* used within an XtResource array initialization list.             */
/* class_names must be a typedef, non pointer type.                 */
/* ---------------------------------------------------------------- */

#define XtOffsetSimpleField(class_name,field) \
          ((Cardinal) &class_name::field - 1)

#define XtOffsetCompoundField(class_name1,field1,class_name2,field2) \
          (((Cardinal) &class_name1::field1 - 1) + ((Cardinal) &class_name2::field2 - 1))

#else
#define XtOffsetSimpleField(class_name,field) \
	((Cardinal) (((char *) (&(((class_name *)NULL)->field))) - ((char *) NULL)))

#define XtOffsetCompoundField(class_name1,field1,class_name2,field2) \
	((Cardinal) (((char *) (&(((class_name1 *)NULL)->field1))) - ((char *) NULL))) + \
	((Cardinal) (((char *) (&(((class_name2 *)NULL)->field2))) - ((char *) NULL)))
#endif

#ifndef __identification_strings
#define SUPPRESS_RCS_ID	/*forces equivalent action for VED/EDIT shared files*/
#endif

#ifndef SBSTDINC_H_NO_INCLUDE
#include <bms/sbstdinc.h>
#endif

#ifdef __aix
#       undef  NULL
#       define NULL 0   /* AIX uses NULL=(void *)0 */
#endif

#endif /* PORT_H_INCLUDED */

