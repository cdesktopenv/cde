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
