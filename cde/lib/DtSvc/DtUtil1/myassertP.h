/* $XConsortium: myassertP.h /main/4 1995/10/26 15:14:54 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        myassertP.h
 **
 **   Project:     DT
 **
 **   Description: Private include file for the Action Library.
 **
 **
 **   (c) Copyright  1993 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef MY_ASSERT_INCLUDED
#  define MY_ASSERT_INCLUDED


#  ifndef NDEBUG
#    if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
/* ANSI Version */
#      define myassert(EX)	\
	((EX)? 1 : (fprintf(stderr,"STDC Assertion failed: \""#EX"\"\n\tFile: %s\n\tLine: %d\n",__FILE__,__LINE__),0))
#    else
/* NON-ANSI C */
#      define myassert(EX)	\
	((EX)? 1 : (fprintf(stderr,"NOSTDC Assertion failed: \"EX\"\n\tFile: %s\n\tLine: %d\n",__FILE__,__LINE__),0))
#    endif	/* STD_C */
#  else
#    define myassert(EX)	(1)
#  endif

#endif	/* MY_ASSERT_INCLUDED */
